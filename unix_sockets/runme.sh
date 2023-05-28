#!/bin/bash

CONF_FILENAME="config.txt"
LOG_FILENAME="/tmp/server.log"
NUMBERS_FILENAME="numbers.txt"
CLIENT="build/client"
SERVER="build/server"
SERVER_PID=""

function setup () {
    # make client and server
    make clean
    make

    # remove results.txt
    rm -f result.txt

    # create config file
    rm -f $CONF_FILENAME
    echo "server.sock" >> $CONF_FILENAME

    # generate file with 1000 numbers
    rm -f $NUMBERS_FILENAME
    python3 generate_nums.py

    # start server
    ./$SERVER $CONF_FILENAME &
    SERVER_PID=$!
    sleep 0.5
}

function test1 () {
    echo "==== TEST 1 ====" >> result.txt

    for i in {1..100}; do
        (./$CLIENT $CONF_FILENAME $i 0.02 < $NUMBERS_FILENAME) >> /dev/null &
        pids[${i}]=$!
    done

    wait "${pids[@]}"

    echo "100 clients done" >> result.txt
    echo "Check server state by sending zero" >> result.txt

    # check server state
    echo "0" | ./$CLIENT $CONF_FILENAME 101 >> /dev/null
    tail -n 1 $LOG_FILENAME >> result.txt
    echo "" >> result.txt
}

function test2 () {
    echo "==== TEST 2 ====" >> result.txt
    echo "Just run test 1 several times" >> result.txt

    for i in {1..10}; do
        test1
    done

    echo "Check heap size" >> result.txt

    head -n 2 $LOG_FILENAME | tail -n 1 >> result.txt
    tail -n 2 $LOG_FILENAME | head -n 1 >> result.txt
}

function test3 () {
    echo "==== TEST 3 ====" >> result.txt
    clients_count=(10 20 30 40 50 60 70 80 90 100)
    delays=(0 0.2 0.4 0.6 0.8 1.0)

    for count in ${clients_count[@]}; do
        for d in ${delays[@]}; do

            echo "Test $count clients with $d delay" >> result.txt

            SECONDS=0
            for ((i=1; i<=$count; i++)); do
                (./$CLIENT $CONF_FILENAME $i $d < $NUMBERS_FILENAME) >> /tmp/client.log &
                pids[${i}]=$!
            done

            wait "${pids[@]}"

            client_time=$(cat /tmp/client.log | grep "run in " | grep -Eo '[0-9]+\.[0-9]+' | sort -rn | head -n 1)
            echo "Client time $client_time" >> result.txt

            duration=$SECONDS
            effective_time=`echo "$duration-$client_time" | bc`
            echo "Total time $duration" >> result.txt
            echo "Effective time $effective_time" >> result.txt

            rm /tmp/client.log
        done
    done

}

# Setup server and client
setup
# Run test 1
test1
# Run test 2
test2
# Run test 3
test3

kill $SERVER_PID