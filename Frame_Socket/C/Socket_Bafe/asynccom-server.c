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
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



#include "Beai3Config.h"
#include "Beai3Dict.h"
#include "Beai3IC.h"
#include "Beai3Log.h"
#include "Beai3AsyncReg.h"

#include "esbconfig.h"

#include "Log_Frame.h"

#define QLEN                 (1)
#define REFID_LEN            (38)
#define LINEMAX              (32)
#define FRAGMENT_PAGE_SIZE   (8192)
#define FILEMODE             (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)

static int (*messageToDict)(char*, int, DictHandle);

static int (*getMessageLength)(char*, int);
static int (*getReferenceID)(DictHandle, char*, int);
static int (*inputMessageProcess)(char**, int);

typedef struct com_config
{
	char  foreign_address[64];
	short foreign_port[LINEMAX];
	short local_port[LINEMAX];
	char  message_convert[128];
	char  extension[128];
} COMCONFIG;

static int clrflag;
static fd_set clrfds;
static struct
{
	int   listenfd;
	int   acceptfd;
	short localeport;
	int   foreignport;
	int   fragment_size;
	int   fragment_offset;
	void *fragment;
} accept_data[LINEMAX];

static char sysname[32];

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
					DEBUG_ERROR("%s: mkdir %s error: %s",
						sysname, dir, strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				DEBUG_ERROR("%s: stat %s error: %s", sysname, dir, strerror(errno));
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
	int lockfd, rlen, len;
	struct stat buf;
	pid_t lockdata[LINEMAX + 1];
	void *p;
	int i;

	memset(lockdata, 0, sizeof(lockdata));

	snprintf(filename, sizeof(filename),
			"%s/var/%s.lock", getenv("HOME"), sysname);

	/*lockfd = open(filename, O_RDONLY|O_DELAY|O_RSHARE, FILEMODE); 原版*/
	lockfd = open(filename, O_RDONLY, FILEMODE);
	if(lockfd == -1)
	{
		DEBUG_ERROR("%s: open [%s] error: %s", sysname, filename, strerror(errno));
		return;
	}

	if(fstat(lockfd, &buf) == -1)
	{
		DEBUG_ERROR("%s: fstat() error: %s", sysname, strerror(errno));
		goto EXIT_ERROR;
	}

	if(buf.st_size)
	{
		if(lseek(lockfd, 0, SEEK_SET) == -1)
		{
			DEBUG_ERROR("%s: lseek() error: %s", sysname, strerror(errno));
			goto EXIT_ERROR;
		}

		len = buf.st_size < sizeof(lockdata) ? buf.st_size : sizeof(lockdata);
		p = lockdata;
		while(len > 0)
		{
			if((rlen = read(lockfd, p, len)) == -1)
			{
				DEBUG_ERROR("%s: read() error: %s", sysname, strerror(errno));
				goto EXIT_ERROR;
			}
			p += rlen;
			len -= rlen;
		}

		for(i = 0; i < LINEMAX; i++)
		{
			if(lockdata[i + 1])
			{
				if(kill(lockdata[i + 1], SIGTERM) == -1)
				{
					DEBUG_ERROR("%s: kill(%ld, SIGTERM) error: %s",
						sysname, lockdata[i + 1], strerror(errno));
				}
			}
		}
	}

EXIT_ERROR:
	close(lockfd);

	if(unlink(filename) == -1)
	{
		DEBUG_ERROR("%s: unlink [%s] error: %s",
			sysname, filename, strerror(errno));
	}
}

static void HandleSIGTERM(int signo)
{
	exit(EXIT_FAILURE);
}

static void HandleSIGINT(int signo)
{
	exit(EXIT_FAILURE);
}

static void HandleSIGUSR1(int signo)
{
	char filename[512];
	int i, lockfd, rlen, len;
	struct stat buf;
	pid_t lockdata[LINEMAX + 1];
	void *p;

	memset(lockdata, 0, sizeof(lockdata));

	snprintf(filename, sizeof(filename),
			"%s/var/%s.lock", getenv("HOME"), sysname);

	/*lockfd = open(filename, O_RDONLY|O_DELAY|O_RSHARE, FILEMODE);  原版 */
	lockfd = open(filename, O_RDONLY, FILEMODE);
	if(lockfd == -1)
	{
		DEBUG_ERROR("%s: open [%s] error: %s", sysname, filename, strerror(errno));
		return;
	}

	if(fstat(lockfd, &buf) == -1)
	{
		DEBUG_ERROR("%s: fstat() error: %s", sysname, strerror(errno));
		goto HANDLESIGUSR1_ERROR;
	}

	if(buf.st_size)
	{
		if(lseek(lockfd, 0, SEEK_SET) == -1)
		{
			DEBUG_ERROR("%s: lseek() error: %s", sysname, strerror(errno));
			goto HANDLESIGUSR1_ERROR;
		}

		len = buf.st_size < sizeof(lockdata) ? buf.st_size : sizeof(lockdata);
		p = lockdata;
		while(len > 0)
		{
			if((rlen = read(lockfd, p, len)) == -1)
			{
				DEBUG_ERROR("%s: read() error: %s", sysname, strerror(errno));
				goto HANDLESIGUSR1_ERROR;
			}
			p += rlen;
			len -= rlen;
		}

		for(i = 0; i < LINEMAX; i++)
		{
			if(!lockdata[i + 1])
			{
				if(accept_data[i].acceptfd)
				{
					clrflag = 1;
					FD_SET(accept_data[i].acceptfd, &clrfds);
				}
			}
		}
	}

HANDLESIGUSR1_ERROR:
	close(lockfd);
}

static void HandleSIGUSR2(int signo)
{
}

/*
static void InitGlobVar()
{
	g_iLogLevel = DEFAULTLOGLEVEL;
	strcpy( gsSysName,"plt" );
}
*/

static int LoadComConfig(COMCONFIG *comconfig)
{
	HANDLE config;
	char valuestr[1024], *port;
	int i;

	if((config = LoadConfigure("asynccom.conf")) < 0)
	{
		DEBUG_ERROR("%s: load asynccom.conf failed.", sysname);
		return -1;
	}

	if(GetValue(config, sysname, "foreign_address",
		comconfig->foreign_address, sizeof(comconfig->foreign_address)) < 0)
	{
		DEBUG_ERROR("%s: can not get foreign address", sysname);
		return -1;
	}

	memset(comconfig->local_port, 0, sizeof(comconfig->local_port));
	if(GetValue(config, sysname, "local_server_port",
		valuestr, sizeof(valuestr)) < 0)
	{
		DEBUG_ERROR("%s: can not get local port", sysname);
		return -1;
	}
	port = strtok(valuestr, ", \t");
	for(i = 0; port &&
		i < sizeof(comconfig->local_port)/sizeof(comconfig->local_port[0]); i++)
	{
		comconfig->local_port[i] = atoi(port);
		port = strtok(NULL, ", \t");
	}

	memset(comconfig->foreign_port, 0, sizeof(comconfig->foreign_port));
	if(GetValue(config, sysname, "foreign_client_port",
		valuestr, sizeof(valuestr)) < 0)
	{
		PINFO("%s: can not get foreign port", sysname);
	}
	else
	{
		port = strtok(valuestr, ", \t");
		for(i = 0; port && i <
			sizeof(comconfig->foreign_port)/sizeof(comconfig->foreign_port[0]);
			i++)
		{
			comconfig->foreign_port[i] = atoi(port);
			port = strtok(NULL, ", \t");
		}
	}

	if(GetValue(config, sysname, "message_convert",
		comconfig->message_convert, sizeof(comconfig->message_convert)) < 0)
	{
		DEBUG_ERROR("%s: can not get message convert.", sysname);
		return -1;
	}

	if(GetValue(config, sysname, "extension",
		comconfig->extension, sizeof(comconfig->extension)) < 0)
	{
		DEBUG_ERROR("%s: can not get extension.", sysname);
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
		DEBUG_ERROR("load module %s error: %s", modname, strerror(errno));
		if(errno == ENOEXEC)
			DEBUG_ERROR("%s: %s", modname, dlerror());
		return -1;
	}

	messageToDict = dlsym(module, "MessageToDict");
	if(!messageToDict)
	{
		DEBUG_ERROR("dlsym %s DictToMessage error: %s", modname, strerror(errno));
		if(errno == EINVAL)
			DEBUG_ERROR("%s: %s", modname, dlerror());
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
		DEBUG_ERROR("load module %s error: %s", modname, strerror(errno));
		if(errno == ENOEXEC)
			DEBUG_ERROR("%s: %s", modname, dlerror());
		return -1;
	}

	getMessageLength = dlsym(module, "GetMessageLength");
	if(!getMessageLength)
	{
		DEBUG_ERROR("dlsym %s GetMessageLength error: %s", modname, strerror(errno));
		if(errno == EINVAL)
			DEBUG_ERROR("%s: %s", modname, dlerror());
		return -1;
	}

	getReferenceID = dlsym(module, "GetReferenceID");
	if(!getReferenceID)
	{
		DEBUG_ERROR("dlsym %s GetReferenceID error: %s", modname, strerror(errno));
		if(errno == EINVAL)
			DEBUG_ERROR("%s: %s", modname, dlerror());
		return -1;
	}

	inputMessageProcess = dlsym(module, "InputMessageProcess");
	if(!inputMessageProcess)
	{
		PINFO("%s InputMessageProcess not implement.", modname);
		if(errno == EINVAL)
			DEBUG_ERROR("%s: %s", modname, dlerror());
	}

    return 0;
}

static int CreateListenFD(COMCONFIG *comconfig, int index)
{
	struct sockaddr_in sin;
	int sock;
	int optval = 1;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(comconfig->local_port[index]);

	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		DEBUG_ERROR("%s: socket() error: %s", sysname, strerror(errno));
		return -1;
	}

	if(fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK) == -1)
	{
		DEBUG_ERROR("%s: fcntl() error: %s", sysname, strerror(errno));
		return -1;
	}

	if(setsockopt(sock, SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		DEBUG_ERROR("%s: setsockopt() error: %s", sysname, strerror(errno));
		return -1;
	}

	if(bind(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1)
	{
		DEBUG_ERROR("%s: bind() error: %s", sysname, strerror(errno));
		return -1;
	}

	if(listen(sock, QLEN) == -1)
	{
		DEBUG_ERROR("%s: listen() error: %s", sysname, strerror(errno));
		return -1;
	}

	return sock;
}

static void HandleListenFD(int index, int fd, fd_set *infds, int *maxfd, COMCONFIG *comconfig)
{
	int acceptfd;
	char remote_addr[32];
	socklen_t sin_len;
	struct sockaddr_in sin;
	int ov;

	sin_len = sizeof(sin);
	if((acceptfd = accept(fd, (struct sockaddr *)&sin, &sin_len)) == -1)
	{
		/*
		 * Ignore the following errors on the subsequent call to accept:
		 * EWOULDBLOCK(for Berkeley-derived implementations, when the client
		 * aborts the connect), ECONNABORTED(for POSIX implementations, when the
		 * clent aborts the connection), EPROTO(for SVR4 implementations, when
		 * the client aborts the connection, and EINTR(if signals are being
		 * caught).
		 */
		if(errno != EAGAIN && errno != EWOULDBLOCK
			&& errno != ECONNABORTED && errno != EPROTO
			&& errno != EINTR)
		{
			DEBUG_ERROR("%s: accept() error: %s", sysname, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		memset(remote_addr, 0x00, sizeof(remote_addr));
		if(! inet_ntop(AF_INET, &sin.sin_addr,remote_addr, sizeof(remote_addr)) )
		{
			DEBUG_ERROR("%s: inet_ntop() error: %s", sysname, strerror(errno));
			exit(EXIT_FAILURE);
		}
		/*PDEBUG("sockaddr_in->sin_port[%d], [%s]",sin.sin_port, remote_addr);*/
		if(memcmp(remote_addr, comconfig->foreign_address, strlen(comconfig->foreign_address)) )
		{
			/*PINFO("非法的远程连接:[%s]，fildes [%d]",remote_addr , acceptfd);*/
			close(acceptfd);
			return;
		}
		accept_data[index].foreignport=sin.sin_port;	
		ov = 1;
		if(setsockopt(acceptfd, SOL_SOCKET, SO_KEEPALIVE, &ov, sizeof(ov))== -1)
		{
			DEBUG_ERROR("%s: setsockopt() SO_KEEPALIVE error: %s",
				sysname, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if(fcntl(acceptfd, F_SETFL, fcntl(acceptfd, F_GETFL) | O_NONBLOCK) < 0)
		{
			DEBUG_ERROR("%s: fcntl() error: %s", sysname, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if(accept_data[index].acceptfd)
		{
			close(accept_data[index].acceptfd);
			FD_CLR(accept_data[index].acceptfd, infds);
		}

		if(accept_data[index].fragment)
		{
			free(accept_data[index].fragment);
			accept_data[index].fragment = NULL;
		}

		accept_data[index].acceptfd = acceptfd;
		FD_SET(acceptfd, infds);

		*maxfd = *maxfd > acceptfd ? *maxfd : acceptfd;

		PINFO("Accept line [%d], fildes [%d]", index, acceptfd);
	}
}

static int HandleTransaction(char **message, int length)
{
	DictHandle handle;
	char refid[REFID_LEN + 1];
	int number;
	IMHEADER imheader;

	if(inputMessageProcess)
		length = (*inputMessageProcess)(message, length);

	if(length > 0)
	{
		handle = InitDict(20);
		if(!handle)
		{
			DEBUG_ERROR("%s: InitDict() error.", sysname);
			return -1;
		}

		/*PDEBUG("%s: Before MessageToDict()", sysname);*/
		if((*messageToDict)(*message, length, &handle) < 0)
		{
			DEBUG_ERROR("%s: MessageToDict() error.", sysname);
			goto TRANSACTION_END;
		}

		/*PDEBUG("%s: Before GetReferenceID()", sysname);*/
		if((*getReferenceID)(handle, refid, sizeof(refid)) < 0)
		{
			DEBUG_ERROR("%s: GetReferenceID() error.", sysname);
			goto TRANSACTION_END;
		}
		PDEBUG("%s: refid = [%s]", sysname, refid);

		/*PDEBUG("%s: Before RetrieveTransaction()", sysname);*/
		if(RetrieveTransaction(sysname, refid, &number, &imheader) < 0)
		{
			DEBUG_ERROR("%s: RetrieveTransaction() error.", sysname);
			goto TRANSACTION_END;
		}
		g_lSessionID = imheader.SessionID;
		imheader.Stage ++;

		/*PDEBUG("%s: Before UnregisterTransaction()", sysname);*/
		if(UnregisterTransaction(sysname, refid) < 0)
		{
			DEBUG_ERROR("%s: UnregisterTransaction() error.", sysname);
			goto TRANSACTION_END;
		}

		PDEBUG("%s: Before BEAIComSend()", sysname);
		if(BEAIComSend(&imheader, &handle) < 0)
		{
			if(UnregisterTransaction(sysname, refid) < 0)
			{
				DEBUG_ERROR("%s: UnregisterTransaction() error.", sysname);
				goto TRANSACTION_END;
			}
		}

TRANSACTION_END:
		DestroyDict(handle);
		return 0;
	}
	else if(length == -1)
		return -1;

	return 0;
}

static void KillClientSIGTERM(int index)
{
	char filename[512];
	int lockfd, rlen, len;
	struct stat buf;
	pid_t lockdata[LINEMAX + 1];
	void *p;
	int i;

	memset(lockdata, 0, sizeof(lockdata));

	snprintf(filename, sizeof(filename),
			"%s/var/%s.lock", getenv("HOME"), sysname);

	/* lockfd = open(filename, O_RDONLY|O_DELAY|O_RSHARE, FILEMODE); 原版 */
	lockfd = open(filename, O_RDONLY, FILEMODE);
	if(lockfd == -1)
	{
		DEBUG_ERROR("%s: open [%s] error: %s", sysname, filename, strerror(errno));
		return;
	}

	if(fstat(lockfd, &buf) == -1)
	{
		DEBUG_ERROR("%s: fstat() error: %s", sysname, strerror(errno));
		goto KILLCLIENTSIGTERM_ERROR;
	}

	if(buf.st_size)
	{
		if(lseek(lockfd, 0, SEEK_SET) == -1)
		{
			DEBUG_ERROR("%s: lseek() error: %s", sysname, strerror(errno));
			goto KILLCLIENTSIGTERM_ERROR;
		}

		len = buf.st_size < sizeof(lockdata) ? buf.st_size : sizeof(lockdata);
		p = lockdata;
		while(len > 0)
		{
			if((rlen = read(lockfd, p, len)) == -1)
			{
				DEBUG_ERROR("%s: read() error: %s", sysname, strerror(errno));
				goto KILLCLIENTSIGTERM_ERROR;
			}
			p += rlen;
			len -= rlen;
		}

	}

	if(lockdata[index + 1])
	{
		if(kill(lockdata[index + 1], SIGTERM) == -1)
		{
			DEBUG_ERROR("%s: kill() error: %s", sysname, strerror(errno));
		}
	}

KILLCLIENTSIGTERM_ERROR:
	close(lockfd);

}

static void HandleMessage(int index, int fd, fd_set *infds)
{
	int message_length;
	char *message;

	while(accept_data[index].fragment)
	{
		message_length = (*getMessageLength)(accept_data[index].fragment,
			accept_data[index].fragment_offset);
		if(message_length == -1)
		{
			DEBUG_ERROR("%s: line [%d], fildes [%d], GetMessageLength() error.",
				sysname, index, fd);

			close(fd);
			FD_CLR(fd, infds);
			accept_data[index].acceptfd = 0;
			if(accept_data[index].fragment)
			{
				free(accept_data[index].fragment);
				accept_data[index].fragment = NULL;
			}

			KillClientSIGTERM(index);
		}
		else if(message_length > 0 &&
			message_length <= accept_data[index].fragment_offset)
		{
			PDEBUG("%s: message_length = [%d]", sysname, message_length);

			message = accept_data[index].fragment;
			if(message_length < accept_data[index].fragment_offset)
			{
				accept_data[index].fragment = malloc(FRAGMENT_PAGE_SIZE);
				if(!accept_data[index].fragment)
				{
					DEBUG_ERROR("%s: malloc() error: %s", sysname, strerror(errno));
					exit(EXIT_FAILURE);
				}

				memcpy(accept_data[index].fragment, message + message_length,
					accept_data[index].fragment_offset - message_length);
				accept_data[index].fragment_size = FRAGMENT_PAGE_SIZE;
				accept_data[index].fragment_offset -= message_length;
			}
			else if(message_length == accept_data[index].fragment_offset)
			{
				accept_data[index].fragment = NULL;
				accept_data[index].fragment_size = 0;
				accept_data[index].fragment_offset = 0;
			}

			((char *)message)[message_length] = '\0';
			PDEBUG("%s: RECV MESG asyn: [%s]", sysname, message);

			if(HandleTransaction(&message, message_length) < 0)
			{
				DEBUG_ERROR("%s: line [%d], fildes [%d], "
					"HandleTransaction() error.", sysname, index, fd);

				close(fd);
				FD_CLR(fd, infds);
				accept_data[index].acceptfd = 0;
				if(accept_data[index].fragment)
				{
					free(accept_data[index].fragment);
					accept_data[index].fragment = NULL;
				}

				KillClientSIGTERM(index);
			}

			PDEBUG("%s: Transaction completed.", sysname);

			g_lSessionID = 0;
			if(message)
				free(message);
		}
		else
		{
			g_lSessionID = 0;
			break;
		}
	}
}

static void HandleAcceptFD(int index, int fd, fd_set *infds)
{
	int read_length;

	if(accept_data[index].fragment)
	{
		if(accept_data[index].fragment_offset
			== accept_data[index].fragment_size)
		{
			accept_data[index].fragment_size += FRAGMENT_PAGE_SIZE;
			accept_data[index].fragment = realloc(accept_data[index].fragment,
				accept_data[index].fragment_size);
			if(!accept_data[index].fragment)
			{
				DEBUG_ERROR("%s: realloc() error: %s", sysname, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		accept_data[index].fragment_size = FRAGMENT_PAGE_SIZE;
		accept_data[index].fragment_offset = 0;
		accept_data[index].fragment = malloc(FRAGMENT_PAGE_SIZE);
		if(!accept_data[index].fragment)
		{
			DEBUG_ERROR("%s: malloc() error: %s", sysname, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	read_length = read(fd,
		accept_data[index].fragment + accept_data[index].fragment_offset,
		accept_data[index].fragment_size - accept_data[index].fragment_offset);
	if(read_length == -1)
	{
		if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
		{
			DEBUG_ERROR("%s: line [%d], fildes [%d], read() error: %s",
				sysname, index, fd, strerror(errno));

			close(fd);
			FD_CLR(fd, infds);
			accept_data[index].acceptfd = 0;
			if(accept_data[index].fragment)
			{
				free(accept_data[index].fragment);
				accept_data[index].fragment = NULL;
			}

			KillClientSIGTERM(index);
		}
	}
	else if(read_length == 0)
	{
		DEBUG_ERROR("%s: line [%d], fildes [%d], connection closed.",
			sysname, index, fd);

		close(fd);
		FD_CLR(fd, infds);
		accept_data[index].acceptfd = 0;
		if(accept_data[index].fragment)
		{
			free(accept_data[index].fragment);
			accept_data[index].fragment = NULL;
		}

		KillClientSIGTERM(index);
	}
	else
	{
		accept_data[index].fragment_offset += read_length;
		PDEBUG("%s: read_length = [%d], fragment_offset = [%d],index=[%d],localeport[%d],foreignport[%d]",
			sysname, read_length, accept_data[index].fragment_offset,index,accept_data[index].localeport,accept_data[index].foreignport);

		HandleMessage(index, fd, infds);
	}
}

static int DoService(COMCONFIG *comconfig)
{
	int i, fd, maxfd = 0;
	fd_set rfds, infds;
	sigset_t set, oldset;

	FD_ZERO(&infds);
	for(i = 0; comconfig->local_port[i] &&
		i < sizeof(comconfig->local_port)/sizeof(comconfig->local_port[0]); i++)
	{
		if((fd = CreateListenFD(comconfig, i)) < 0)
		{
			DEBUG_ERROR("%s: CreateListenFD() error: %s", sysname, strerror(errno));
			return -1;
		}
		else
		{
			PINFO("%s: listen %d port [%d]",
				sysname, i, comconfig->local_port[i]);
			FD_SET(fd, &infds);
			accept_data[i].listenfd = fd;
			accept_data[i].localeport = comconfig->local_port[i];
			maxfd = maxfd > fd ? maxfd : fd;
		}
	}

	while(1)
	{
		sigemptyset(&set);
		sigaddset(&set, SIGUSR1);
		sigaddset(&set, SIGUSR2);
		sigprocmask(SIG_BLOCK, &set, &oldset);
		if(clrflag)
		{
			for(fd = 0; fd <= maxfd; fd++)
			{
				if(FD_ISSET(fd, &clrfds) && FD_ISSET(fd, &rfds))
				{
					close(fd);
					FD_CLR(fd, &infds);

					for(i = 0; accept_data[i].listenfd &&
						i < sizeof(accept_data)/sizeof(accept_data[0]); i++)
					{
						if(fd == accept_data[i].acceptfd)
						{
							accept_data[i].acceptfd = 0;
							break;
						}
					}
				}
			}
			clrflag = 0;
			FD_ZERO(&clrfds);
		}
		sigprocmask(SIG_SETMASK, &oldset, NULL);
		rfds = infds;

		if(select(maxfd + 1, &rfds, NULL, NULL, NULL) == -1)
		{
			if(errno != EINTR && errno != EBADF)
			{
				DEBUG_ERROR("%s: select() error: %s", sysname, strerror(errno));
				return -1;
			}
			else if(errno == EBADF)
			{
				DEBUG_ERROR("%s: fildes close by SIGUSR1.", sysname);
			}
		}
		else
		{
			for(fd = 0; fd <= maxfd; fd++)
			{
				if(FD_ISSET(fd, &rfds))
				{
					for(i = 0; accept_data[i].listenfd &&
						i < sizeof(accept_data)/sizeof(accept_data[0]); i++)
					{
						if(fd == accept_data[i].listenfd)
						{
							HandleListenFD(i, fd, &infds, &maxfd, comconfig);
							break;
						}
						else if(fd == accept_data[i].acceptfd)
						{
							HandleAcceptFD(i, fd, &infds);
							break;
						}
					}

					if(i == sizeof(accept_data)/sizeof(accept_data[0])
						|| !accept_data[i].listenfd)
					{
						DEBUG_ERROR("%s: Unknown fildes [%d].", sysname, fd);
					}
				}
			}
		}
	}
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

	act.sa_sigaction = HandleSIGUSR1;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	sigaddset(&act.sa_mask, SIGUSR2);
	act.sa_flags = 0;
	sigaction(SIGUSR1, &act, NULL);

	act.sa_sigaction = HandleSIGUSR2;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGUSR1);
	sigaddset(&act.sa_mask, SIGUSR2);
	act.sa_flags = 0;
	sigaction(SIGUSR2, &act, NULL);
}

static int WriteLockFile(COMCONFIG *comconfig)
{
	char filename[512];
	int lockfd, return_len, len;
	struct stat buf;
	pid_t lockdata[LINEMAX + 1];
	void *p;
	int i;

	snprintf(filename, sizeof(filename),
		"%s/var/%s.lock", getenv("HOME"), sysname);

	MakeDir(filename);

	/* lockfd = open(filename, O_RDWR|O_CREAT|O_DELAY|O_RSHARE, FILEMODE);  原版 */
	lockfd = open(filename, O_RDWR|O_CREAT, FILEMODE);
	if(lockfd == -1)
	{
		DEBUG_ERROR("%s: open [%s] error: %s",
			sysname, filename, strerror(errno));
		return -1;
	}

	if(fstat(lockfd, &buf) == -1)
	{
		DEBUG_ERROR("%s: fstat() error: %s", sysname, strerror(errno));
		goto WRITELOCKFILE_ERROR;
	}

	if(buf.st_size)
	{
		memset(lockdata, 0, sizeof(lockdata));
		lseek(lockfd, 0, SEEK_SET);
		len = buf.st_size < sizeof(lockdata) ? buf.st_size : sizeof(lockdata);
		p = lockdata;
		while(len > 0)
		{
			if((return_len = read(lockfd, p, len)) == -1)
			{
				DEBUG_ERROR("%s: read() error: %s", sysname, strerror(errno));
				goto WRITELOCKFILE_ERROR;
			}
			p += return_len;
			len -= return_len;
		}

		for(i = 0; i < LINEMAX; i++)
		{
			if(lockdata[i + 1])
			{
				if(kill(lockdata[i + 1], SIGTERM) == -1)
				{
					DEBUG_ERROR("%s: kill() error: %s", sysname, strerror(errno));
				}
			}
		}
	}

	lseek(lockfd, 0, SEEK_SET);
	memset(lockdata, 0, sizeof(lockdata));
	lockdata[0] = getpid();
	len = sizeof(lockdata);
	p = lockdata;
	while(len > 0)
	{
		if((return_len = write(lockfd, p, len)) == -1)
		{
			DEBUG_ERROR("%s: write() error: %s", sysname, strerror(errno));
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

	clrflag = 0;
	FD_ZERO(&clrfds);

	memset(accept_data, 0, sizeof(accept_data));
	memset(sysname, 0, sizeof(sysname));

	SetSignalHandle();

	/*InitGlobVar();
	if(InitLogenv(gsSysName) < 0)
	{
		fprintf(stderr, "init log failed.\n");
		exit(EXIT_FAILURE);
	}
	*/
	if(GetValue(GetESBConfig(), "process-sysname", basename(argv[0]),
		sysname, sizeof(sysname)) < 0)
	{
		DEBUG_ERROR("can not get sysname [%s].", basename(argv[0]));
		exit(EXIT_FAILURE);
	}

	if(LoadComConfig(&comconfig) < 0)
	{
		DEBUG_ERROR("%s: can not get comconfig", sysname);
		exit(EXIT_FAILURE);
	}

	if(WriteLockFile(&comconfig) < 0)
	{
		DEBUG_ERROR("%s: WriteLockFile() failed.", sysname);
		exit(EXIT_FAILURE);
	}

	if(LoadConvModule(comconfig.message_convert) < 0)
	{
		DEBUG_ERROR("%s: load module %s failed.",
			sysname, comconfig.message_convert);
		exit(EXIT_FAILURE);
	}

	if(LoadExtModule(comconfig.extension) < 0)
	{
		DEBUG_ERROR("%s: load module %s failed.", sysname, comconfig.extension);
		exit(EXIT_FAILURE);
	}

	if(DoService(&comconfig) < 0)
	{
		DEBUG_ERROR("%s: DoService() failed.", sysname);
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

