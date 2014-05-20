if [ -z $1 ]; then
    echo "Executable not defined"
    exit 6;
fi

if [ ! -f "$1" ]; then
    echo "Executable $1 not found"
    exit 5;
fi

exec_name=$1;
exit_status=0;

function do_test() {
    local test_file=$1;
    local test_result=$2;

    echo "Excecuting test \"$test_file\" expecting return value equals to \"$test_result\"";

    echo | ./$exec_name $test_file ;
    local given_result=$?;

    if [ "$given_result" != "$test_result" ]; then
        echo "Test $test_file failed."
        echo "Expected exit: $test_result"
        echo "Given exit: $given_result"
        exit_status=1;
    fi;
    return
}

do_test test/sample.txt 0;

if [ "$exit_status" == "0" ]; then
    echo "Test was successful!!"
fi;


exit $exit_status; 
