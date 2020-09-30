#include "utils.h"
#include "printf.h"
#include "sched.h"
#include "peripherals/timer.h"
#include "timer.h"

const unsigned int interval = (1<<26)/10;
unsigned int curVal = 0;

int frq=0;
void update_wait(){
	frq++;
	if(frq!=10) return;
	else{
		frq =0;
	}
	for(int i=0;i<NR_TASKS;i++){
		if(wait_array[i]==1){
			wait_array[i]=-1;
			task[i]->state=TASK_RUNNING;
			printf("task[%d]: back to running\n",i);
		}else if(wait_array[i]!=-1){
			wait_array[i]--;
			printf("task[%d]: sleep countdown %d\n",i,wait_array[i]);
		}
	
	}

}

void timer_init ( void )
{
	curVal = get32(TIMER_CLO);
	curVal += interval;
	put32(TIMER_C1, curVal);
}

void handle_timer_irq( void ) 
{
	curVal += interval;
	put32(TIMER_C1, curVal);
	put32(TIMER_CS, TIMER_CS_M1);
	timer_tick();
}

void generic_timer_init ( void )
{
	gen_timer_init();
	gen_timer_reset(interval);
}

void handle_generic_timer_irq( void ) 
{
	update_wait();
	gen_timer_reset(interval);
    	timer_tick();
}
