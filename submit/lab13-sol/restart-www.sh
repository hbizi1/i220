#!/usr/bin/sh

if [ $# -ne 1 -a $# -ne 2 ]
then
    echo "usage: $0 EXEC_FILE [PORT]"
    exit 1
fi

EXEC_PATH=`realpath $1`
PORT=$2
SCRIPT_DIR=`dirname $0`

: "${PORT:=2200}"

fuser -k $PORT/tcp

cd $SCRIPT_DIR/www-io


npm start $EXEC_PATH $PORT
