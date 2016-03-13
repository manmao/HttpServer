#关闭wfs进程
#!/bin/sh
ps -A|grep "http_cpp" |awk '{print $1}' |while read proc
do
	kill -9 ${proc};
done

