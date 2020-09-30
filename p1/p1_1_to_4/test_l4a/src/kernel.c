#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"

#define RUN_LENS 9
#define WAIT_LENS 11


#define CP_PROC(CONTANT, PROC_NUM) \
{ \
res = copy_process((unsigned long)&process, (unsigned long)CONTANT); \
if (res != 0) { \
printf("error while starting process %d",PROC_NUM); \
return; \
} \
} 

void idle_process(char *array){
	while(1){	
	printf("TASK IDLE.1: deep sleep...\n");
	_WFI();
	printf("TASK IDLE.2: im back! \n");
	schedule();
	}
}

void wait_process(char* array){
/*
	for (int i = 0; i < WAIT_LENS; i++){
		uart_send(array[i]);
		delay(5000000);
	}
 */
	printf("Task B.1: Before changing to WAIT STATE \n");

	change_to_state(TASK_WAIT);
	printf("Task B.2: Back from WAIT STATE\n");
	change_to_state(TASK_EXIT);
	
}

void process(char *array)
{
	//while (1){
	/*
		for (int i = 0; i < RUN_LENS; i++){
			uart_send(array[i]);
			delay(5000000);
		}
     	*/
		printf("Task A: Only run once\n");
		change_to_state(TASK_EXIT);
		//schedule(); // yield
	//}
}

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);

	printf("kernel boots\n");

	irq_vector_init();
	generic_timer_init();
	enable_interrupt_controller();
	init_wait_array();
	int res=-1 ;
	res = copy_idle_process((unsigned long)&idle_process, (unsigned long)"idlee");
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	res = copy_process((unsigned long)&process, (unsigned long)"T1:runrun");
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	
	res = copy_process((unsigned long)&wait_process, (unsigned long)"TB:waitwait");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}
	




	enable_irq();
	//CP_PROC("qqqqq",3);
	//printf("proc 3: %d\n", res );
	while (1){
		schedule();
	}	
}

