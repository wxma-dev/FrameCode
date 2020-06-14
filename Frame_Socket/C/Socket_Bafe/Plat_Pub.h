#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <ulimit.h>
#include <varargs.h>
#include <fcntl.h>	
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h> 
#include <pwd.h>
#include <setjmp.h>
#include <dirent.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <sqlca.h>
#include "BeaiMacro.h"
#include "BeaiStruct.h"
#include "BeaiFunction.h"
#include "BeaiLog.h"
#include "BeaiXym.h"
#include "BeaiKmdy.h"
#include "BeaiPz.h"
#include "BeaiSec.h"
#include "VostFunction.h"
#include "VostStruct.h"
#include "algorithm.h"
#include "Beai3Dict.h"
/*  20090505
#include "isti.h"
*/
#include "tables/dbzhsxdzb.h"
#include "tables/dbjykzb.h"
#include "tables/dbroutedy.h"
#include "tables/dbjgdy.h"
#include "tables/dbkmtsmdzb.h"
#include "tables/dbkmczdzb.h"
#include "tables/dbtdwlzhdzb.h"
#include "tables/dbvtxyxx.h"
#include "tables/dbdzmxb.h"
#include "tables/dbsamt.h"
#include "tables/dbzfxtmc.h"
#include "tables/dbsffcpysb.h"
#include "pub_sxfkslsb.h"
#include "cache.h"

/*BEAI平台的版本号*/
#define BEAISYSVER  "1.0.0"
/*默认的日志级别，在平台读取交易控制表的日志级别前，此值有效。*/
#define DEFAULTLOGLEVEL 4

