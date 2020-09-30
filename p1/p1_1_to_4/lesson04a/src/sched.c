#include "sched.h"
#include "irq.h"
#include "printf.h"


#define WAIT_CYCLE 3 // wait for cycle
static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

//void preempt_disable(void)
//{
//	current->preempt_count++;
//}
//
//void preempt_enable(void)
//{
//	current->preempt_count--;
//}


void _schedule(void)
{
//	preempt_disable();
	int next,c, wait_ready_to_run_isFound=-1,back_to_idle_counter=-1;
	int running_flag=0;
	struct task_struct * p;
	
	while (1) {
		c = -1;
		next = 1;
		wait_ready_to_run_isFound = -1;
		for (int i = 1; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
				printf("next is %d\n",next);
				break;
			}
		}
		if(next!=1){
			running_flag =1;
			//break;
		}
		//Check Wait State
		//if(c==-1){
		back_to_idle_counter = 0;
		//IF only idle left
		//next = 1;
		//check Wait state and plus 1 for each
		for(int i =1;i <NR_TASKS;i++){
			p = task[i];
			if(p &&p->state == TASK_WAIT){
				if(wait_array[i]!=-1 &&
				   wait_array[i]!=0 &&
			           wait_array[i]%WAIT_CYCLE ==0){

					printf("find task[%d] back from wait to running\n",i);
					back_to_idle_counter++;
					wait_array[i] = -1;
					p->state = TASK_RUNNING;
					
					if(wait_ready_to_run_isFound==-1){
						next = running_flag==1?next:i;
						//next = i;
						wait_ready_to_run_isFound = 1;
					}
					
				}
			//notify handler to increase
				else if(wait_array[i]==-1){
					wait_array[i]=0;
				printf("init task[%d] to wait\n",i);
				}
			}

		}

		if(back_to_idle_counter ==0 && running_flag!=1) next = 1;
		break;
		//}
		//printf("Need recharge\n");
		/*
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
			iii = i;
		}
		*/
	}
	printf("switch to %d state:%d\n",next,task[next]->state);
	switch_to(task[next]);
//	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
//	preempt_enable();
}

void change_to_state(int state_num){
	switch(state_num){
		case TASK_WAIT:
			current->state = TASK_WAIT;
			printf("changed\n");
			break;
		case TASK_EXIT:
			current->state = TASK_EXIT;
			break;
		case TASK_RUNNING:
			current->state = TASK_RUNNING;
			break;
		
		default:
			printf("wrong state: %d\n",state_num);
	}
	schedule();

}

void init_wait_array(){
	for(int i=0; i <NR_TASKS; i++){
		wait_array[i]=-1;
	}
	for(int i=0; i <NR_TASKS; i++){
		printf("%d ",wait_array[i]);
	}
	printf("\n");

}

//void timer_tick()
//{
//	--current->counter;
//	if (current->counter>0 || current->preempt_count >0) {
//		return;
//	}
//	current->counter=0;
//	enable_irq();
//	_schedule();
//	disable_irq();
//}
