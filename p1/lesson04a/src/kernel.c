#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"

#define CP_PROC(CONTANT, PROC_NUM) \
{ \
res = copy_process((unsigned long)&process, (unsigned long)CONTANT); \
if (res != 0) { \
printf("error while starting process %d",PROC_NUM); \
return; \
} \
} 



void process(char *array)
{
	while (1){
		for (int i = 0; i < 5; i++){
			uart_send(array[i]);
			delay(5000000);
		}
		schedule(); // yield
	}
}

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);

	printf("kernel boots\n");

//	irq_vector_init();
//	generic_timer_init();
//	enable_interrupt_controller();
//	enable_irq();

	int res = copy_process((unsigned long)&process, (unsigned long)"12345");
	if (res != 0) {
		printf("error while starting process 1");
		return;
	}
	printf("proc 1: %d\n", res );
	
	res = copy_process((unsigned long)&process, (unsigned long)"abcde");
	if (res != 0) {
		printf("error while starting process 2");
		return;
	}
	printf("proc 2: %d\n", res );
	
	CP_PROC("qqqqq",3);
	printf("proc 3: %d\n", res );
	
	while (1){
		schedule();
	}	
}
