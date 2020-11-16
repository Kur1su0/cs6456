NUM_LOOP=$1
#1KB, 4KB, 16KB, 128KB, 1MB, 4MB, 16MB
BUFFER_LIST="1024 4096 16384 131072 1048576" #4194304 16777216"
var=0
for buffer in $BUFFER_LIST
do
	var=$((var+1))
	echo -e "OP:$var\t\c"
	#echo "xtest --sdp-basic -s $buffer -n $NUM_LOOP "
	xtest --sdp-basic -s $buffer -n $NUM_LOOP
done
