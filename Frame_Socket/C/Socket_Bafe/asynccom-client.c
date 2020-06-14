#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <time.h>

#include <Beai3Config.h>
#include <Beai3Dict.h>
#include <Beai3IC.h>
#include <Beai3Log.h>
#include <Beai3AsyncReg.h>

#include "../esb/esbconfig.h"

#define REFID_LEN (38)
#define LINEMAX (32)
#define MSGMAX (1048576)
#define FILEMODE  (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)

static int (*dictToMessage)(DictHandle handle, char *message);

static int (*getMessageLength)(char*, int);
static int (*getReferenceID)(DictHandle, char*, int);
static int (*outputMessageProcess)(char**, int);

/* static char message[MSGMAX]; */
static char buffer[MSGMAX];

typedef struct com_config
{
	char  foreign_address[64];
	short foreign_port[LINEMAX];
	short local_port[LINEMAX];
	char  message_convert[128];
	char  extension[128];
} COMCONFIG;

static char sysname[32];
static int  line = -1;

static void MakeDir(char *pathfilename)
{
	char dir[1024];
	struct stat buf;
	char *p;

	strncpy(dir, pathfilename, sizeof(dir) - 1);
	dir[sizeof(dir) - 1] = '\0';

	p = dir;
	while(*p == '/') p++;
	p = strchr(p, '/');
	while(p)
	{
		*p = '\0';
		if(stat(dir, &buf) == -1)
		{
			if(errno == ENOENT)
			{
				if(mkdir(dir, FILEMODE|S_IXUSR|S_IXGRP) == -1)
				{
					PERROR("%s: mkdir %s error: %s",
						sysname, dir, strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				PERROR("%s: stat %s error: %s", sysname, dir, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		*p = '/';
		while(*p == '/') p++;
		p = strchr(p, '/');
	}
}

static void Exit()
{
	char filename[512];
	int lockfd, return_len, len;
	struct stat buf;
	pid_t lockdata[LINEMAX + 1];
	void *p;

	memset(lockdata, 0, sizeof(lockdata));

	snprintf(filename, sizeof(filename),
		"%s/var/%s.lock", getenv("HOME"), sysname);

	MakeDir(filename);

	lockfd = open(filename, O_RDWR|O_DELAY|O_RSHARE, FILEMODE);
	if(lockfd == -1)
	{
		PERROR("%s: open [%s] error: %s", sysname, filename, strerror(errno));
	}

	if(fstat(lockfd, &buf) == -1)
	{
		PERROR("%s: fstat() error: %s", sysname, strerror(errno));
		goto EXIT_ERROR;
	}

	if(buf.st_size)
	{
		lseek(lockfd, 0, SEEK_SET);
		len = buf.st_size < sizeof(lockdata) ? buf.st_size : sizeof(lockdata);
		p = lockdata;
		while(len > 0)
		{
			if((return_len = read(lockfd, p, len)) == -1)
			{
				PERROR("%s: read() error: %s", sysname, strerror(errno));
				goto EXIT_ERROR;
			}
			p += return_len;
			len -= return_len;
		}
	}

	if(!lockdata[0])
	{
		PERROR("%s: can not get server's pid", sysname);
		goto EXIT_ERROR;
	}

	if(lockdata[line + 1] == getpid())
		lockdata[line + 1] = 0;
	else
	{
		PERROR("%s: line [%d] client pid invalid.", sysname, line);
		goto EXIT_ERROR;
	}

	lseek(lockfd, 0, SEEK_SET);
	len = sizeof(lockdata);
	p = lockdata;
	while(len > 0)
	{
		if((return_len = write(lockfd, p, len)) == -1)
		{
			PERROR("%s: write() error: %s", sysname, strerror(errno));
			goto EXIT_ERROR;
		}
		p += return_len;
		len -= return_len;
	}

	if(lockdata[0])
	{
		if(kill(lockdata[0], SIGUSR1) == -1)
		{
			PERROR("%s: kill() error: %s", sysname, strerror(errno));
		}
	}

EXIT_ERROR:
	close(lockfd);
}

static void HandleSIGTERM(int signo)
{
	exit(EXIT_FAILURE);
}

static void HandleSIGINT(int signo)
{
	exit(EXIT_FAILURE);
}

static void InitGlobVar()
{
	g_iLogLevel = DEFAULTLOGLEVEL;
	strcpy( gsSysName,"plt" );
}

static int LoadComConfig(COMCONFIG *comconfig)
{
	HANDLE config;
	char valuestr[1024], *port;
	int i;

	if((config = LoadConfigure("asynccom.conf")) < 0)
	{
		PERROR("%s: load asynccom.conf failed.", sysname);
		return -1;
	}

	if(GetValue(config, sysname, "foreign_address",
		comconfig->foreign_address, sizeof(comconfig->foreign_address)) < 0)
	{
		PERROR("%s: can not get foreign address", sysname);
		return -1;
	}

	memset(comconfig->foreign_port, 0, sizeof(comconfig->foreign_port));
	if(GetValue(config, sysname, "foreign_server_port",
		valuestr, sizeof(valuestr)) < 0)
	{
		PERROR("%s: can not get foreign port", sysname);
		return -1;
	}
	port = strtok(valuestr, ", \t");
	for(i = 0; port &&
		i < sizeof(comconfig->foreign_port)/sizeof(comconfig->foreign_port[0]);
		i++)
	{
		comconfig->foreign_port[i] = atoi(port);
		port = strtok(NULL, ", \t");
	}

	memset(comconfig->local_port, 0, sizeof(comconfig->local_port));
	if(GetValue(config, sysname, "local_client_port",
		valuestr, sizeof(valuestr)) < 0)
	{
		PINFO("%s: can not get local port", sysname);
	}
	else
	{
		port = strtok(valuestr, ", \t");
		for(i = 0; port &&
			i < sizeof(comconfig->local_port)/sizeof(comconfig->local_port[0]);
			i++)
		{
			comconfig->local_port[i] = atoi(port);
			port = strtok(NULL, ", \t");
		}
	}

	if(GetValue(config, sysname, "message_convert",
		comconfig->message_convert, sizeof(comconfig->message_convert)) < 0)
	{
		PERROR("%s: can not get message convert.", sysname);
		return -1;
	}

	if(GetValue(config, sysname, "extension",
		comconfig->extension, sizeof(comconfig->extension)) < 0)
	{
		PERROR("%s: can not get extension.", sysname);
		return -1;
	}

	return 0;
}

static int LoadConvModule(char *modname)
{
	void *module;

	PDEBUG("load %s", modname);
	module = dlopen(modname, RTLD_LAZY);
	if(!module)
	{
		PERROR("load module %s error: %s", modname, strerror(errno));
		if(errno == ENOEXEC)
			PERROR("%s: %s", modname, dlerror());
		return -1;
	}

	dictToMessage = dlsym(module, "DictToMessage");
	if(!dictToMessage)
	{
		PERROR("dlsym %s DictToMessage error: %s", modname, strerror(errno));
		if(errno == EINVAL)
			PERROR("%s: %s", modname, dlerror());
		return -1;
	}

    return 0;
}

static int LoadExtModule(char *modname)
{
	void *module;

	PDEBUG("load %s", modname);
	module = dlopen(modname, RTLD_LAZY);
	if(!module)
	{
		PERROR("load module %s error: %s", modname, strerror(errno));
		if(errno == ENOEXEC)
			PERROR("%s: %s", modname, dlerror());
		return -1;
	}

	getMessageLength = dlsym(module, "GetMessageLength");
	if(!getMessageLength)
	{
		PERROR("dlsym %s GetMessageLength error: %s", modname, strerror(errno));
		if(errno == EINVAL)
			PERROR("%s: %s", modname, dlerror());
		return -1;
	}

    getReferenceID = dlsym(module, "GetReferenceID");
	if(!getReferenceID)
	{
		PERROR("dlsym %s GetReferenceID error: %s", modname, strerror(errno));
		if(errno == EINVAL)
			PERROR("%s: %s", modname, dlerror());
		return -1;
	}

	outputMessageProcess = dlsym(module, "OutputMessageProcess");
	if(!outputMessageProcess)
	{
		PINFO("%s OutputMessageProcess not implement.", modname);
		if(errno == EINVAL)
			PERROR("%s: %s", modname, dlerror());
	}

    return 0;
}

static int Connect(COMCONFIG *comconfig, int index)
{
	struct sockaddr_in sin, l_sin;
	int connfd;
	int ov;
	PDEBUG("[%d]: address [%s], port [%d]",
		index, comconfig->foreign_address, (unsigned short)comconfig->foreign_port[index]);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(comconfig->foreign_port[index]);
	if(inet_pton(AF_INET, comconfig->foreign_address, &sin.sin_addr) == -1)
	{
		PERROR("%s: inet_pton() error: %s", sysname, strerror(errno));
		return -1;
	}

	if((connfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		PERROR("%s: socket() error: %s", sysname, strerror(errno));
		return -1;
	}
	PDEBUG("%s: socket() = [%d]", sysname, connfd);

	if(comconfig->local_port[index])
	{
		memset(&l_sin, 0, sizeof(l_sin));
		l_sin.sin_family = AF_INET;
		l_sin.sin_port = htons(comconfig->local_port[index]);
		l_sin.sin_addr.s_addr = INADDR_ANY;

		ov = 1;
		if(setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR, &ov, sizeof(ov)) == -1)
		{
			PERROR("%s: setsockopt() SO_REUSEADDR error: %s",
				sysname, strerror(errno));
			close(connfd);
			return -1;
		}
		ov = 1;
		if(setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &ov, sizeof(ov)) == -1)
		{
			PERROR("%s: setsockopt() SO_KEEPALIVE error: %s",
				sysname, strerror(errno));
			close(connfd);
			return -1;
		}

		PDEBUG("%s: before bind()", sysname);
		if(bind(connfd, (struct sockaddr *)&l_sin, sizeof(l_sin)) == -1)
		{
			PERROR("%s: bind() error: %s", sysname, strerror(errno));
			close(connfd);
			return -1;
		}
	}

	ov = 1;
	if(setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, &ov, sizeof(ov)) == -1)
	{
		PERROR("%s: setsockopt() SO_KEEPALIVE error: %s",
			sysname, strerror(errno));
		close(connfd);
		return -1;
	}

	ov = 1;
	if(setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, &ov, sizeof(ov)) == -1)
	{
		PERROR("%s: setsockopt() SO_KEEPALIVE error: %s",
			sysname, strerror(errno));
		close(connfd);
		return -1;
	}

	PDEBUG("before connect()");
	if(connect(connfd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		PERROR("%s: connect() error: %s", sysname, strerror(errno));
		close(connfd);
		return -1;
	}

	return connfd;
}

static int HandleTransaction(COMCONFIG *comconfig, int index, int fd)
{
	char *message;
	int length, write_length, left_length;
	char *p, refid[REFID_LEN + 1];
	IMHEADER imheader;
	DictHandle handle = NULL;
	DictHandle failedRsp = NULL;
	fd_set infds;
	struct timeval tv;
	int nfds, ret;

	struct timeval sendtv;
	struct tm *sendtm;
	char strtm[64];
	time_t current_time;

	while(1)
	{
		g_lSessionID = 0;

		PDEBUG("%s: before BEAIComReceive()", sysname);
		strncpy(imheader.Sender, sysname, sizeof(imheader.Sender)-1);
		imheader.Sender[sizeof(imheader.Sender) - 1] = '\0';
		if(BEAIComReceive(&imheader, &handle) == -1)
		{
			PERROR("%s: receive internal message error.", sysname);
			close(fd);
			return -1;
		}

		g_lSessionID = imheader.SessionID;
		current_time = time(NULL);
		/*超时判断*/
		if(imheader.SessionTime.tv_sec+imheader.Timeout <= current_time)
		{
			PERROR("交易已经超时，不予处理start[%d]outtime[%d]", imheader.SessionTime.tv_sec, imheader.Timeout );
			DestroyDict(handle);
			continue;
		}
		
		PDEBUG("before GetReferenceID()");
		if((*getReferenceID)(handle, refid, sizeof(refid)) < 0)
		{
			PERROR("%s: GetReferenceID() error.", sysname);
			DestroyDict(handle);

			imheader.IMType = 2;
			if(BEAIComSend(&imheader, &failedRsp) == -1)
			{
				PERROR("send internal message error.");
				close(fd);
				return -1;
			}

			continue;
		}

		if(!(message = malloc(MSGMAX)))
		{
			PERROR("%s: malloc() failed.", sysname);
			DestroyDict(handle);
			imheader.IMType = 2;
			if(BEAIComSend(&imheader, &failedRsp) == -1)
			{
				PERROR("send internal message error.");
				close(fd);
				return -1;
			}

			continue;
		}

		PDEBUG("before DictToMessage()");
		if((length = (*dictToMessage)(handle, message)) < 0)
		{
			PERROR("%s: DictToMessage() error.", sysname);
			DestroyDict(handle);
			free(message);

			imheader.IMType = 2;
			if(BEAIComSend(&imheader, &failedRsp) == -1)
			{
				PERROR("send internal message error.");
				close(fd);
				return -1;
			}
			continue;
		}

		PINFO("%s: RegisterTransaction(), refid=[%s]", sysname, refid);
		if(RegisterTransaction(sysname, refid, line, &imheader) < 0)
		{
			PERROR("%s: RegisterTransaction failed.", sysname);
			DestroyDict(handle);
			free(message);
			imheader.IMType = 2;
			if(BEAIComSend(&imheader, &failedRsp) == -1)
			{
				PERROR("send internal message error.");
				close(fd);
				return -1;
			}
			continue;
		}

		imheader.Stage ++;

		message[length] = '\0';
		PINFO("%s: SEND MESG asyn: [%s]", sysname, message);

		PDEBUG("%s: before check connection.", sysname);
		FD_ZERO(&infds);
		FD_SET(fd, &infds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		if((nfds = select(fd + 1, &infds, NULL, NULL, &tv)) < 0)
		{
			PERROR("%s: select() error: %s", sysname, strerror(errno));
			DestroyDict(handle);
			free(message);
			close(fd);
			return -1;
		}
		else if(nfds > 0)
		{
			if(FD_ISSET(fd, &infds))
			{
				if((ret = read(fd, buffer, sizeof(buffer))) < 0)
				{
					PERROR("%s: read() error: %s", sysname, strerror(errno));
					close(fd);

					PDEBUG("before Connect()");
					if((fd = Connect(comconfig, index)) < 0)
					{
						PERROR("%s: Connect() error.", sysname);
						DestroyDict(handle);
						free(message);
						return 0;
					}
				}
				else if(ret == 0)
				{
					PERROR("%s: read() = 0, socket [%d] closed by partner.",
						sysname, fd);
					close(fd);

					PDEBUG("before Connect()");
					if((fd = Connect(comconfig, index)) < 0)
					{
						PERROR("%s: Connect() error.", sysname);
						DestroyDict(handle);
						free(message);
						return 0;
					}
				}
			}
			else
			{
				DestroyDict(handle);
				free(message);
				close(fd);
				return -1;
			}
		}

		if(outputMessageProcess)
		{
			PDEBUG("before OutputMessageProcess()");
			length = (*outputMessageProcess)(&message, length);
			if(length < 0)
			{
				PERROR("%s: OutputMessageProcess() error.", sysname);
				DestroyDict(handle);
				free(message);

				imheader.IMType = 2;
				if(BEAIComSend(&imheader, &failedRsp) == -1)
				{
					PERROR("send internal message error.");
					close(fd);
					return -1;
				}

				continue;
			}
		}

		gettimeofday(&sendtv, NULL);
		sendtm = localtime(&sendtv.tv_sec);
		strftime(strtm, sizeof(strtm), "%Y%m%d %H:%M:%S", sendtm);
		PDEBUG("%s: Sending... SESSIONID[%ld]，TIME[%s.%06ld]",
			sysname, imheader.SessionID, strtm, sendtv.tv_usec);

		PDEBUG("%s: before send message", sysname);
		left_length = length;
		p = message;
		while(left_length > 0)
		{
			if((write_length = write(fd, p, left_length)) == -1)
			{
				PERROR("%s: write() error: %s", sysname, strerror(errno));
				DestroyDict(handle);
				free(message);
				close(fd);
				return 0;
			}

			left_length -= write_length;
			p += write_length;
		}

		DestroyDict(handle);
		free(message);
	}

	return 0;
}

static int DoService(COMCONFIG *comconfig)
{
	int connfd;

	if(line < 0 || line >= LINEMAX)
	{
		PERROR("%s: line[%d] error.", sysname, line);
		return -1;
	}

	PDEBUG("%s: line [%d], before Connect().", sysname, line);
	if((connfd = Connect(comconfig, line)) < 0)
	{
		PERROR("%s: Connect() error.", sysname);
		return -1;
	}

	PDEBUG("%s: before DoService().", sysname);
	if(HandleTransaction(comconfig, line, connfd) < 0)
	{
		PERROR("%s: DoService() error.", sysname);
		return -1;
	}

	return 0;
}

static void SetSignalHandle()
{
	struct sigaction act;

	atexit(Exit);

	act.sa_sigaction = HandleSIGTERM;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGTERM);
	act.sa_flags = 0;
	sigaction(SIGTERM, &act, NULL);

	act.sa_sigaction = HandleSIGINT;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGINT);
	act.sa_flags = 0;
	sigaction(SIGINT, &act, NULL);
}

static int WriteLockFile(COMCONFIG *comconfig)
{
	char filename[512];
	int lockfd, return_len, len;
	struct stat buf;
	pid_t lockdata[LINEMAX + 1];
	void *p;
	int i;

	memset(lockdata, 0, sizeof(lockdata));

	snprintf(filename, sizeof(filename),
		"%s/var/%s.lock", getenv("HOME"), sysname);

	MakeDir(filename);

	lockfd = open(filename, O_RDWR|O_DELAY|O_RSHARE, FILEMODE);
	if(lockfd == -1)
	{
		PERROR("%s: open [%s] error: %s", sysname, filename, strerror(errno));
		return -1;
	}

	if(fstat(lockfd, &buf) == -1)
	{
		PERROR("%s: fstat() error: %s", sysname, strerror(errno));
		goto WRITELOCKFILE_ERROR;
	}

	if(buf.st_size)
	{
		lseek(lockfd, 0, SEEK_SET);
		len = buf.st_size < sizeof(lockdata) ? buf.st_size : sizeof(lockdata);
		p = lockdata;
		while(len > 0)
		{
			if((return_len = read(lockfd, p, len)) == -1)
			{
				PERROR("%s: read() error: %s", sysname, strerror(errno));
				goto WRITELOCKFILE_ERROR;
			}
			p += return_len;
			len -= return_len;
		}
	}

	if(!lockdata[0])
	{
		PERROR("%s: can not get server's pid", sysname);
		goto WRITELOCKFILE_ERROR;
	}

	for(i = 0; i < LINEMAX; i++)
	{
		if(!lockdata[i + 1])
		{
			lockdata[i + 1] = getpid();
			break;
		}
	}

	if(i > LINEMAX)
	{
		PERROR("%s: number of client process more than maximum(32).", sysname);
		goto WRITELOCKFILE_ERROR;
	}
	else
		line = i;

	lseek(lockfd, 0, SEEK_SET);
	len = sizeof(lockdata);
	p = lockdata;
	while(len > 0)
	{
		if((return_len = write(lockfd, p, len)) == -1)
		{
			PERROR("%s: write() error: %s", sysname, strerror(errno));
			goto WRITELOCKFILE_ERROR;
		}
		p += return_len;
		len -= return_len;
	}

	close(lockfd);
	return 0;

WRITELOCKFILE_ERROR:
	close(lockfd);
	return -1;
}

int main(int argc, char *argv[])
{
	COMCONFIG comconfig;

	memset(sysname, 0, sizeof(sysname));

	SetSignalHandle();

	InitGlobVar();
	if(InitLogenv(gsSysName) < 0)
	{
		fprintf(stderr, "init log failed.\n");
		exit(EXIT_FAILURE);
	}

	if(GetValue(GetESBConfig(), "process-sysname",
		basename(argv[0]), sysname, sizeof(sysname)) < 0)
	{
		PERROR("can not get sysname [%s].", basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	if(LoadComConfig(&comconfig) < 0)
	{
		PERROR("%s: can not get comconfig", sysname);
		exit(EXIT_FAILURE);
	}

	if(WriteLockFile(&comconfig) < 0)
	{
		PERROR("%s: WriteLockFile() failed.", sysname);
		exit(EXIT_FAILURE);
	}

	if(LoadConvModule(comconfig.message_convert) < 0)
	{
		PERROR("%s: load module %s failed.",
			sysname, comconfig.message_convert);
		exit(EXIT_FAILURE);
	}

	if(LoadExtModule(comconfig.extension) < 0)
	{
		PERROR("%s: load module %s failed.", sysname, comconfig.extension);
		exit(EXIT_FAILURE);
	}

	if(DoService(&comconfig) < 0)
	{
		PERROR("%s: DoService() failed.", sysname);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

