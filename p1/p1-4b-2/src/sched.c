#include "sched.h"
#include "irq.h"
#include "printf.h"
#include "utils.h"

#define T_STAMP 0
#define TASK_A 1
#define TASK_B 2
#define num_contentSW 200

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task[NR_TASKS] = {&(init_task), };
int nr_tasks = 1;

int times = 0;
int next_pid=-1;

unsigned long long int sw_AtoB_array[num_contentSW][3]={{-1},{-1},{-1}};

void save_switch_info(int now,int after){
	times++;
	sw_AtoB_array[times-1][T_STAMP]=get_tstamp();
	sw_AtoB_array[times-1][TASK_A]=now;
	sw_AtoB_array[times-1][TASK_B]=after;
	//printf("time:%d\n",times);
	if(times==num_contentSW){
		for(int i=0;i<num_contentSW;i++){
			printf("stamp:%u\tFrom:%d\tTo:%d\t\n",sw_AtoB_array[i][T_STAMP],
							      sw_AtoB_array[i][TASK_A],
							      sw_AtoB_array[i][TASK_B]);
		}	
	times = 0;
	}

}
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
	next_pid=next;
	switch_to(task[next]);
	preempt_enable();
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
	int now,after;
	now = current->cur_pid;
	after = next->cur_pid;
	//printf("cur:%d,next:%d\n",now,after);
	save_switch_info(now,after);

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
