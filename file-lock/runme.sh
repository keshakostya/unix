BUILD_DIR="$(pwd)/build"
APP_NAME=testapp
n=10

echo "Building test app..."
make clean && make

cd ${BUILD_DIR}

for i in $(seq 1 $n); do
    export LD_LIBRARY_PATH=`pwd`:$LD_LIBRARY_PATH
    ./testapp myfile.txt &
    echo "run test app in process $!"
    pids[${i}]=$!
done

sleep 5m
for pid in ${pids[*]}; do
    kill -s 2 $pid
    if [[ "0" == "$?" ]]; then
        echo "Succesfully killed $pid"
    else
        echo "Error while killing $pid"
    fi
done

cat myfile.txt | sort | uniq -c > result.txt

echo "Statistic saved in $BUILD_DIR/result.txt"

cd ..