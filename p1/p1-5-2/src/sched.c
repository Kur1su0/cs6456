#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "fork.h"
#include "utils.h"
#include "mm.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}


void _schedule(void)
{
	preempt_disable();
	int next,c;
	struct task_struct * p;
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		if (c) {
			break;
		}
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
	}
	switch_to(task[next], next);
	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}


void switch_to(struct task_struct * next, int index) 
{
	if (current == next) 
		return;
	struct task_struct * prev = current;
	current = next;
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}

void timer_tick()
{
	--current->counter;
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	current->counter=0;
	enable_irq();
	_schedule();
	disable_irq();
}

void exit_process(){
	preempt_disable();
	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task[i]->state = TASK_ZOMBIE;
			break;
		}
	}
	if (current->stack) {
		free_page(current->stack);
	}
	preempt_enable();
	schedule();
}

void change_to_state(int state_num){
	switch(state_num){
		case TASK_WAIT:
			current->state = TASK_WAIT;
			printf("changed\n");
			break;
		case TASK_RUNNING:
			current->state = TASK_RUNNING;
			break;
		
		default:
			printf("wrong state: %d\n",state_num);
	}
	schedule();

}

void sleep_state(int second){
	preempt_disable();
	//->WAIT
		//get cur id
	int task_id=-1;
	for (int i = 0; i < NR_TASKS; i++){
		if (task[i] == current) {
			task_id = i;
			break;
		}
	}
	if(task_id == -1){
		printf("no running task avial\n");
		return;
	}
	if(current->state==TASK_WAIT){
		printf("Already in WAIT state\n");
		return;
	}
	current->state= TASK_WAIT;
	//->init wait array
	wait_array[task_id] = second;
	
	preempt_enable();
	schedule();

}



void init_wait_array(){
	for(int i=0;i<NR_TASKS;i++){
		wait_array[i] = -1;
	}

}
