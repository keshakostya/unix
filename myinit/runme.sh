INPUT="/tmp/myinit_in"
OUTPUT="/tmp/myinit_out"
CONF="/tmp/myinit_conf"
MYINIT="./build/myinit"
tasks=("`which sleep` 5" "`which sleep` 10" "`which sleep` 15")

> "/tmp/myinit.log"

> $INPUT

> $OUTPUT

> $CONF
OLD_IFS=$IFS
IFS=""
for item in ${tasks[*]}
do
    echo "$item $INPUT $OUTPUT" >> $CONF
done
IFS=$OLD_IFS
make clean
make

./$MYINIT $CONF

sleep 1

echo Started three sleeps: > result.txt
OLD_IFS=$IFS
IFS=""
for item in ${tasks[*]}
do
    ps -ef | grep "$item" | grep -v grep >> result.txt
done
IFS=$OLD_IFS

sleep 5

echo "`which sleep` 7 $INPUT $OUTPUT" > $CONF
killall -HUP myinit
echo >> result.txt
echo Started one sleep after SIGHUP: >> result.txt
ps -ef | grep "`which sleep` 7" | grep -v grep >> result.txt

sleep 5

killall myinit

echo >> result.txt
echo "Logs:" >> result.txt
cat /tmp/myinit.log >> result.txt