#/bin/bash
REPEAT_TIMES=100
LOG_ITEMS=1000000
TOTAL_TIME_COST_MS=0

if [ $# -ge 1 ];then
    REPEAT_TIMES=$1
fi

if [ $# -ge 2 ];then
    LOG_ITEMS=$2
fi

echo "repeat times:[$REPEAT_TIMES] times, log items:[$LOG_ITEMS]"

for ((i = 1; i != ($REPEAT_TIMES + 1); i++))
do
    ms=$(./logger_test $LOG_ITEMS | grep 'cost time' | cut -d : -f 2 | awk {'print $1'})
    echo "time:[$i], cost time:[$ms]ms"
    TOTAL_TIME_COST_MS=$((TOTAL_TIME_COST_MS + ms))
done

avg=$(($TOTAL_TIME_COST_MS / $REPEAT_TIMES))
echo "total repeat:[$REPEAT_TIMES] times, cost:[$TOTAL_TIME_COST_MS] ms, avg:[$avg] ms"
