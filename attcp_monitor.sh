#Don't change the file name
#!/bin/sh
#
#监视hlin_tcp_server程序运行
#
#
:<<!
把此脚本hlinmonitor.sh加入到/etc/rc.d/rc.local里，
在rc.local添加如下语句。
ls /opt/hlin/hlinmonitor.sh > /dev/null                                               
if [ "$?" -eq 0 ]                                                               
then                                                                            
        chmod 777 /opt/hlin/hlinmonitor.sh                                            
        /opt/hlin/hlinmonitor.sh &                                                    
        echo "run /opt/hlin/hlinmonitor.sh"                                           
else                                                                            
        echo "/opt/hlin/hlinmonitor.sh is not exist."                                 
fi
!
#
#
#------------------------------------------------------------------------------
# 函数: log
# 功能:  log message
# 说明: 写信息保存到文件中
#------------------------------------------------------------------------------
LOG_FILE=/opt/hlin/log/monitor.log
log()
{
	local t=$(date +"%F %X")
	echo "[ $t ] $0 : $1 " >> ${LOG_FILE}
}
#
#
#------------------------------------------------------------------------------
# 函数: CheckProcess
# 功能: 检查一个进程是否存在
# 参数: $1 --- 要检查的进程名称
# 返回: 如果存在返回0, 否则返回1.
#------------------------------------------------------------------------------
CheckProcess()
{
  # 检查输入的参数是否有效
  if [ "$1" = "" ];
  then
    return 1
  fi

  #$PROCESS_NUM获取指定进程名的数目，为1返回0，表示正常，不为1返回1，表示有错误，需要重新启动
  PROCESS_NUM=`ps -ef | grep "$1" | grep -v "grep" | wc -l` 
  if [ $PROCESS_NUM -eq 1 ];
  then
    return 0
  else
	log "hlindata process is inexistence."
    return 1
  fi
}
#------------------------------------------------------------------------------
# 函数: main
# 功能: 检查hlindata是否已经在运行
# 参数:  hlindata &
# 返回:  
#------------------------------------------------------------------------------
while [ 1 ]  
do
 CheckProcess "hlindata"
 CheckEFOS_RET=$?
 if [ $CheckEFOS_RET -eq 1 ];
 then
   sleep 1
# 杀死所有hlindata进程
  killall -9 hlindata
  log "restart hlindata."
  echo "restart hlindata." 
  /opt/efos/hlindata
 fi
  echo "CheckProcess hlinmonitor.sh." 
 sleep 3
done
#
#
#end of file
#Hlin
#
#
