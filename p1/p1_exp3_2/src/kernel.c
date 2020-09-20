#include "printf.h"
#include "timer.h"
#include "irq.h"
#include "mini_uart.h"
#include "utils.h"


void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	printf("kernel boots...\n");
	
	int el = get_el();

	printf("sending HVC at EL%d\n",el);
	asm ("HVC #0"	"\n\t");

	el = get_el();
	printf("back to EL%d\n",el);

	while (1){
		uart_send(uart_recv());
	}	
}
