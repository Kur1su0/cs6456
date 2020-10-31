#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <string.h>
#include <assert.h>

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#ifdef USE_VTUNE
#include <ittnotify.h>
#endif

#include "common.h"  // xzl
#include "measure.h"  // xzl

#include <glib.h>
#include <time.h>

SortedList_t* lists;
SortedListElement_t* elements;
int* keys;
int* index;
int cur_key_id=0;
int key_factor;
int mod;
int numThreads;

int* check_array;

int* spinLocks = NULL;
pthread_mutex_t* mutexes = NULL;
pthread_mutex_t* thread_mutexes = NULL;

int the_n_elements = 0;

struct prog_config the_config;

long long runTime = 0;

GHashTable** hTable = NULL;
int num_hashtables;

#define ONE_BILLION 1000000000L;

#ifdef USE_VTUNE
	__itt_domain * itt_domain = NULL;
	__itt_string_handle * sh_sort = NULL; // the overall task name
	__itt_string_handle ** sh_parts = NULL; // per part task name

	#define vtune_task_begin(X) __itt_task_begin(itt_domain, __itt_null, __itt_null, sh_parts[X])
	#define vtune_task_end() __itt_task_end(itt_domain)
#else
	#define vtune_task_begin(X)
	#define vtune_task_end()
#endif

int* gen_key(int num){
	int *array = (int*)calloc(1*num,sizeof(int));
		srand(time(NULL));
	for(int i=0; i<num; i++){
		array[i] = rand(); 
	}


	return array;
	
}

void check_key(gpointer key, gpointer value,gpointer data){
	int * _key = key;
	int* _val = value;
	if(keys[(*_key)] == *_val) check_array[ (*_key)]+=1;
}

void print_errors(char* error){
    if(strcmp(error, "clock_gettime") == 0){
        fprintf(stderr, "Error initializing clock time\n");
        exit(1);
    }
    if(strcmp(error, "thread_create") == 0){
        fprintf(stderr, "Error creating threads.\n");
        exit(2);
    }
    if(strcmp(error, "thread_join") == 0){
        fprintf(stderr, "Error with pthread_join.\n");
        exit(2);
    }
    if(strcmp(error, "mutex") == 0){
        fprintf(stderr, "Error with pthread_join. \n");
        exit(2);
    }
    if(strcmp(error, "segfault") == 0){
        fprintf(stderr, "Segmentation fault caught! \n");
        exit(2);
    }
    if(strcmp(error, "size") == 0){
        fprintf(stderr, "Sorted List length is not zero. List Corrupted\n");
        exit(2);
    }
    if(strcmp(error, "lookup") == 0){
        fprintf(stderr, "Could not retrieve inserted element due to corrupted list.\n");
        exit(2);
    }
    if(strcmp(error, "length") == 0){
        fprintf(stderr, "Could not retrieve length because list is corrupted.\n");
        exit(2);
    }
    if(strcmp(error, "delete") == 0){
        fprintf(stderr, "Could not delete due to corrupted list. \n");
        exit(2);
    }
}

void signal_handler(int sigNum){
    if(sigNum == SIGSEGV){
        print_errors("segfault");
    }
}//signal handler for SIGSEGV


// @idx: the idx in the input key table
//SortedListElement_t *get_element(int idx) {
	//return &elements[idx];
//}

SortedListElement_t *alloc_elements(int numElements) {
    fprintf(stderr, "init %d elements\n", numElements);

    SortedListElement_t * elements = malloc(sizeof(SortedListElement_t) * numElements);
    assert(elements);

//#ifdef USE_INIT_ELEMENTS
    for(int i = 0; i < numElements; i++){
        elements[i].key = getRandomKey();
        // debugging
//        if (i < 10)
//        	printf("random keys: %lx \n", elements[i].key);
    }
//#endif

    return elements;
}

/*
SortedListElement_t *get_element(int idx) {
	SortedListElement_t *p = malloc(sizeof(SortedListElement_t));
	assert(p);
	p->key = keys[idx];
	return p;
}*/

void* thread_func(void* thread_id){
    int id = *((int*)thread_id);


    //int which_table= keys[cur_key_id%num_hashtables]
    int which_table; //= keys[cur_key_id]%num_hashtables;

    which_table = 0;
    pthread_mutex_lock(&thread_mutexes[0]);
    k2_measure("tr start");
    pthread_mutex_unlock(&thread_mutexes[0]);

//    printf("i'm thread %d\n", id);
    int i=0;
    int key_id=0;

    vtune_task_begin(id);
    while(i < key_factor){ //key_factor: num iter
	    key_id = id + i*numThreads;
	    which_table = 0;
	    pthread_mutex_lock(&mutexes[which_table]);
	    g_hash_table_insert(hTable[which_table],&index[key_id],
			    &keys[key_id]);//GINT_TO_POINTER(keys[i]));
	    pthread_mutex_unlock(&mutexes[which_table]);
	    i++;

    }
    vtune_task_end();
	   //printf("%s---%d,%d\n", ret==0?"FALSE":"TRUE",i,keys[i]);
//    int ll = SortedList_length(&lists[id]);
//    fprintf(stderr, "tr -- list %d: %d items per_part %d\n", id, ll, per_part);

    pthread_mutex_lock(&thread_mutexes[0]);
    k2_measure("tr done");
    pthread_mutex_unlock(&thread_mutexes[0]);

    return NULL;
}


void final_check(){
	int total=0,part=0;
	/*
	for(int i=0; i<num_hashtables;i++){
		part = g_hash_table_size(hTable[i]);
		//printf("table[%i]=%i\n",i, part);
		total += part;

	}
	*/
	//printf("\nTotal items:%d\n",total);

	for(int i=0; i<num_hashtables;i++){
		g_hash_table_foreach(hTable[i], check_key, NULL);
	}

	for(int i=0; i<the_n_elements;i++){
		//printf("key[%d]:%d\n",i,check_array[i]);
		if(check_array[i]==1) total++;
	}
	if(total==the_n_elements){
		printf("key verification: Identical\n");
	}
	else{
		printf("key verification: Failed\n");
	}

}


int main(int argc, char** argv) {

    the_config = parse_config(argc, argv);

		numThreads = the_config.numThreads;
		int iterations = the_config.iterations;
		int numParts = the_config.numParts;
		//num_hashtables = atoi(argv[3]);
		//printf("numbers of tables%d\n",num_hashtables);


    signal(SIGSEGV, signal_handler);

    k2_measure("init");

    srand((unsigned int) time(NULL));

    the_n_elements = numThreads * iterations;

    key_factor = iterations;

    num_hashtables = 1;
    //numParts = num_hashtables;


    //init hash table
    hTable = (GHashTable**)calloc(1*num_hashtables, sizeof(*hTable));
    for (int i=0;i<num_hashtables;i++){
	    hTable[i] = g_hash_table_new(g_int64_hash, g_int64_equal); //direct_hash
	    assert(hTable[i]!=NULL);

    }
    //GHashTable* hash_table = hTable[0];:
    
    keys = gen_key(the_n_elements);
    index = (int*)calloc(1*the_n_elements,sizeof(int));
    for(int i=0;i<the_n_elements;i++){index[i]=i;}
    check_array = (int*)calloc(1*the_n_elements,sizeof(int));



    //1 lcok per table
    alloc_locks(&mutexes, num_hashtables, NULL, 0);  // multilists or biglock: only 1 mutex, no spinlocks
    alloc_locks(&thread_mutexes, 1, NULL, 0);  // multilists or biglock: only 1 mutex, no spinlocks

   // lists = alloc_lists(numThreads);
    //lists = alloc_lists(1);

    
#ifdef USE_VTUNE
    itt_domain = __itt_domain_create("my domain");
		__itt_thread_set_name("my main");

		// pre create here, instead of doing it inside tasks
		sh_parts = malloc(sizeof(__itt_string_handle *) * numParts);
		assert(sh_parts);
		char itt_task_name[32];
		for (int i = 0; i < numParts; i++) {
			snprintf(itt_task_name, 32, "part-%d", i);
			sh_parts[i] = __itt_string_handle_create(itt_task_name);
		}
#endif

    k2_measure("init done");

    pthread_t threads[numThreads];
    int thread_id[numThreads];

    struct timespec start, end;
    if (clock_gettime(CLOCK_MONOTONIC, &start) < 0){
        print_errors("clock_gettime");
    }
    
#ifdef USE_VTUNE
    __itt_task_begin(itt_domain, __itt_null, __itt_null,
    					__itt_string_handle_create("list"));
#endif
    
    k2_measure("tr launched");

    for(int i = 0; i < numThreads; i++){
        thread_id[i] = i;
        int rc = pthread_create(&threads[i], NULL, thread_func, &thread_id[i]);
        if(rc < 0){
            print_errors("thread_create");
        }
    }

    k2_measure("tr launched");

    for(int i = 0; i < numThreads; i++){
        int rc = pthread_join(threads[i], NULL);
        if(rc < 0){
            print_errors("thread_join");
        }
    }

    
    //for(int i=0;i<100;i++){printf("(%d,%d)",i,keys[i]);}
    
	//printf("---------------inserting key\n");
	//printf("\n");
    //g_hash_table_foreach(hTable[0], print_key, NULL);

#ifdef USE_VTUNE
    __itt_task_end(itt_domain);
    free(sh_parts);
#endif

    k2_measure("tr joined");

    if(
		    clock_gettime(CLOCK_MONOTONIC, &end) < 0){
        print_errors("clock_gettime");
    }
    
    long long diff = (end.tv_sec - start.tv_sec) * ONE_BILLION;
    diff += end.tv_nsec;
    diff -= start.tv_nsec;
    
    // we're done. correctness check up
    {
    	//long total = 0;
	final_check();
	/*
			for(int i = 0; i < numThreads; i++) {
			//for(int i = 0; i < 1; i++) {
					//int ll = SortedList_length(&lists[i]);
					//fprintf(stderr, "list %d: %d items; ", i, ll);
					//total += ll;
			}
			fprintf(stderr, "\ntotal %ld items\n", total);
		*/
    }

    k2_measure_flush();

    int numOpts = iterations * numThreads; //get number of operations

    char testname[32];
    getTestName(&the_config, testname, 32);
    print_csv_line(testname, numThreads, iterations, numParts, numOpts, diff);

    // --- clean up ---- //
    free_locks(mutexes, num_hashtables, NULL);
    free_locks(thread_mutexes, 1, NULL);
    for (int i=0; i<num_hashtables; i++){
	    g_hash_table_destroy(hTable[i]);
    }
    free(keys);
    free(index);
    free(check_array);
    //free(lists);
    exit(0);

}



