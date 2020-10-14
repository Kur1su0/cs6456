#!/bin/bash

rm -f no_lock
rm -f m_lock
rm -f spin_lock
rm -f cas

touch no_lock
touch m_lock
touch spin_lock
touch cas

thread_cnt="1 5 10 20 40"
iter_cnt="1 10 20 40 80 100 1000 10000 100000"

#add-none test
for i in 1, 5, 10, 20, 40
do
    for j in 1, 10, 20, 40, 80, 100, 1000, 10000, 100000
    do
        ./counter --iterations=$j --threads=$i >> no_lock
    done
    echo " " >> no_lock
done

    echo "------------------------------------" >> no_lock

for i in 1, 10, 20, 40, 80, 100, 1000, 10000, 100000
do
    for j in 1, 5, 10, 20, 40
    do
        ./counter --iterations=$i --threads=$j >> no_lock
    done
    	echo " " >> no_lock
done

#####################################################
#add-m test
for i in 1, 5, 10, 20, 40
do
        for j in 1, 10, 20, 40, 80, 100, 1000, 10000, 100000
        do
                ./counter --iterations=$j --threads=$i --sync=m >> m_lock
        done
	echo " " >>m_lock
done
    echo "--------------------------------------">>m_lock

for i in $iter_cnt
do
        for j in $thread_cnt
        do
                ./counter --iterations=$i --threads=$j --sync=m >> m_lock
        done
	echo " ">>m_lock
done

####################################################

#add-s test
for i in 1, 5, 10, 20, 40
do
        for j in 1, 10, 20, 40, 80, 100, 1000, 10000, 100000
        do
		./counter --iterations=$j --threads=$i --sync=s >> spin_lock
        done
	echo  " ">>spin_lock
done

	echo "-----------------------------------------">>spin_lock
for i in $iter_cnt
do
        for j in $thread_cnt 
	do
                ./counter --iterations=$i --threads=$j --sync=s >> spin_lock
        done
	echo  " ">>spin_lock
done
############################################################
#add-c test
for i in 1, 5, 10, 20, 40
do
        for j in 1, 10, 20, 40, 80, 100, 1000, 10000, 100000
        do
                ./counter --iterations=$j --threads=$i --sync=c >> cas
        done
	echo  " " >>cas
done

echo "-----------------------------------------">>cas

for i in $iter_cnt
do
        for j in $thread_cnt 
        do
                ./counter --iterations=$i --threads=$j --sync=c >> cas
        done
	echo " ">>cas
done
