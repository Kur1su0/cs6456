#!/bin/bash
# set -x 

# TODO: Switch between the following two to disable/enable VTUNE profiling
#VTUNE="vtune -collect hotspot -knob sampling-mode=hw"
VTUNE=

# TODO: Switch among the following three for # of iterations
#ITER=10k  # small
ITER=1M # med
#ITER=10M # large

run() {
  PROG=$1
  TRACEFILE=$2
  FACTOR=$3  # #of parts=factor x # of threads

      
  rm -rf $TRACEFILE
  touch $TRACEFILE
  
  echo $1 $2 $3
  
  # TODO: Set thread counts to test here
  for tr in 1 2 4 6 8 10 12 16 20
  #for tr in 1 2 4 6 8
  #for tr in {1..8}

  #for tr in 1 2
  do 
    $VTUNE $PROG --iterations=$ITER  --threads=$tr --parts=`expr $tr \* $FACTOR` >> $TRACEFILE 2>&1   
  done
  
  cat $TRACEFILE | grep "verifi\|test="
}


run_hash() {
  PROG=$1
  TRACEFILE=$2
  FACTOR=$3  # #of parts=factor x # of threads
	
  NUM_TABLE=$4
  rm -rf $TRACEFILE
  touch $TRACEFILE
  
  echo $1 $2 $3 $4
  
  # TODO: Set thread counts to test here
  for tr in 1 2 4 6 8 10 12 16 20
  #for tr in 1 2 4 6 8
  #for tr in {1..8}

  #for tr in 1 2
  do
	  echo "thr=$tr"
    $VTUNE $PROG --iterations=$ITER  --threads=$tr $NUM_TABLE >> $TRACEFILE 2>&1   
  done
  
  cat $TRACEFILE | grep "verifi\|test="
}




run_hash_big_table() {
  PROG=$1
  TRACEFILE=$2
  FACTOR=$3  # #of parts=factor x # of threads

      
  rm -rf $TRACEFILE
  touch $TRACEFILE
  
  echo $1 $2 $3
  
  # TODO: Set thread counts to test here
  for tr in 1 2 4 6 8 10 12 16 20
  #for tr in 1 2 4 6 8
  #for tr in {1..8}

  #for tr in 1 2
  do 
	  echo "thr=$tr"
    $VTUNE $PROG --iterations=$ITER  --threads=$tr 1 >> $TRACEFILE 2>&1   
  done
  
  cat $TRACEFILE | grep "verifi\|test="
}


#####################
# biglock
#run "./list" "trace-.txt" 1

#####################
# malloc
#run "./lab2_list-malloc" "trace-malloc.txt" 1
#run "./list-p" "trace-p.txt" 1

#####################
# no malloc, no stealing
#run "./lab2_list-steal" "trace-nomalloc-nosteal.txt" 1
#run "./list-pm" "trace-pm.txt" 1

#####################
# no malloc, stealing
#run "./lab2_list-steal" "trace-nomalloc-steal.txt" 4
#run "./list-pml" "trace-pml.txt" 1

#####################
# no malloc, stealing, padding
# run "./lab2_list-steal-padding" "trace-nomalloc-steal-padding.txt" 4
#run "./list-pmla" "trace-pmla.txt" 1


#####################
#run-hash
run_hash "./list-hash" "trace-hash10.txt" 1 10
run_hash "./list-hash" "trace-hash100.txt" 1 100
run_hash "./list-hash" "trace-hash750.txt" 1 750
run_hash "./list-hash" "trace-hash1000.txt" 1 1000
run_hash "./list-hash" "trace-hash10000.txt" 1 10000
run_hash_big_table "./list-hash" "trace-hash_big.txt" 1



#####################
# best
#run "./lab2_list-steal2-naivepadding" "lab2_list-steal2-naivepadding.txt" 4
