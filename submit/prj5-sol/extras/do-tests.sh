#!/usr/bin/env sh

if [ $# -eq 0 ]
then
    prog=`basename $0`
    echo "usage: $prog CACHE_SIM_EXEC_PATH [TRACE_FILE_PATH...]"
    exit 1
fi

exec=$1
shift

if [ $# -eq 0 ]
then
    tests=`dirname $0`/tests/*.test
else
    tests="$@"
fi

mkdir -p ~/tmp

for t in $tests
do
    test_dir=`dirname $t`
    b=`basename $t .test`
    echo $b | grep -P '([lm]ru|rand)_\d+-\d+-\d+-\d+' > /dev/null 
    if [ $? -ne 0 ]
    then
	echo "test basename must be of the form [lm]ru-D-D-D-D"
	exit 1
    fi
    replace=`echo $b |cut -d_ -f1`
    cache_spec=`echo $b |cut -d_ -f2`
    out_file="$b.out";	
    $exec -r $replace $cache_spec < $t > $out_file
    gold_file="$test_dir/$b.gold"
    if diff $gold_file $out_file
    then
	rm $out_file
	echo "*** OK $b";
    else
	echo "*** FAIL $b; expected output in $gold_file; " \
	     "actual output in $out_file"
    fi
    valgrind_file="$b.valgrind"
    valgrind --error-exitcode=1 -s --leak-check=full \
	     $exec -r $replace $cache_spec < $t 2> $valgrind_file >/dev/null
    if [ $? -eq 0 ]
    then
	rm $valgrind_file
	echo "*** OK valgrind $b"
    else
	echo "*** FAIL valgrind $b; valgrind output in $valgrind_file"
    fi
done
