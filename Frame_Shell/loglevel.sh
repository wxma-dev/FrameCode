
#!/bin/bash

#实现日志的级别不同颜色输出





#可将log函数单独放一个文件，通过.命令引入，这样就可以共用了
#. log.sh 
#设置日志级别
loglevel=0 #debug:0; info:1; warn:2; error:3
logfile=$0".log"
function log {
    local msg;local logtype
    logtype=$1
    msg=$2
    datetime=`date +'%F %H:%M:%S'`
    #使用内置变量$LINENO不行，不能显示调用那一行行号
    #logformat="[${logtype}]\t${datetime}\tfuncname:${FUNCNAME[@]} [line:$LINENO]\t${msg}"
    logformat="[${logtype}]\t${datetime}\tfuncname: ${FUNCNAME[@]/log/}\t[line:`caller 0 | awk '{print$1}'`]\t${msg}"
    #funname格式为log error main,如何取中间的error字段，去掉log好办，再去掉main,用echo awk? ${FUNCNAME[0]}不能满足多层函数嵌套
    {  
    case $logtype in 
        debug)
            [[ $loglevel -le 0 ]] && echo -e "\033[30m${logformat}\033[0m" ;;
        info)
            [[ $loglevel -le 1 ]] && echo -e "\033[32m${logformat}\033[0m" ;;
        warn)
            [[ $loglevel -le 2 ]] && echo -e "\033[33m${logformat}\033[0m" ;;
        error)
            [[ $loglevel -le 3 ]] && echo -e "\033[31m${logformat}\033[0m" ;;
    esac
    } | tee -a $logfile
}

#以下为测试
debug () {
    log debug "there are $# parameters:$@"
}
info() {
    log info "funcname:${FUNCNAME[@]},lineno:$LINENO"
}
warn() {
    log warn "funcname:${FUNCNAME[0]},lineno:$LINENO"
}
error() {
    log error "the first para:$1;the second para:$2"
}
set -x
debug first second
set +x
info first second
warn first second 
error first second





