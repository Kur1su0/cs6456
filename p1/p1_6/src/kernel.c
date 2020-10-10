#include <stddef.h>
#include <stdint.h>

#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"
#include "sys.h"
#include "user.h"

#include "mm.h"

#include "peripherals/irq.h"


//#define GEN
void kernel_process(){
	printf("Kernel process started. EL %d\r\n", get_el());
	unsigned long begin = (unsigned long)&user_begin;
	unsigned long end = (unsigned long)&user_end;
	unsigned long process = (unsigned long)&user_process;
	int err = move_to_user_mode(begin, end - begin, process - begin);
	if (err < 0){
		printf("Error while moving process to user mode\n\r");
	} 
}


void kernel_main()
{
	uart_init();
	init_printf(NULL, putc);

	printf("kernel boots ...\n\r");
	//map_table_entry(current->mm.kernel_pages[current->mm.kernel_pages_count] ,VA_START+GEN_TIMER_ADDR,GEN_TIMER_ADDR);
	unsigned long pgd_addr = get_pgd();
	//unsigned long pgd_addr = pg_dir;
	//unsigned long pgd_addr = 0x100400;
	unsigned long phy_timer_addr = 0x40000040; 
	
	printf("pgd_addr:%x\n",pgd_addr);
	printf("mapping timer\n");
	printf("%s:%d...\n",__func__,__LINE__);
	
	irq_vector_init();
	printf("%s:%d...\n",__func__,__LINE__);
	//map_timer_reg(current,pgd_addr, VA_START +phy_timer_addr ,phy_timer_addr);
	//printf("%s:%d...\n",__func__,__LINE__);
	//unsigned long val = *(unsigned long*)phy_timer_addr;
	//printf("%s:%d...\n",__func__,__LINE__);
	//printf("---------val:%x\n",val);
	
	
	//map_timer_reg(current,pgd_addr , (VA_START +TIMER_INT_CTRL_0) ,TIMER_INT_CTRL_0);
	map_timer_reg(current,pgd_addr, (VA_START +phy_timer_addr) ,phy_timer_addr);
	printf("%s:%d...\n",__func__,__LINE__);
	unsigned long val1 = *(unsigned long*) (phy_timer_addr+VA_START);
	printf("%s:%d...\n",__func__,__LINE__);
	//map_timer_reg(current,pgd_addr, (VA_START +0x40000060) ,0x40000060);
	//unsigned long val2 = *(unsigned long*) (0x40000060+VA_START);
	//timer_init()
//#if define GEN
	generic_timer_init();
//#endif
	enable_interrupt_controller();
	enable_irq();

	int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
	if (res < 0) {
		printf("error while starting kernel process");
		return;
	}

	while (1){
		schedule();
	}	
}
