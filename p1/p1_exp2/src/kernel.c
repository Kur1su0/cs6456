#include "printf.h"
#include "utils.h"
#include "mini_uart.h"

void kernel_main(void)
{
	uart_init();
	init_printf(0, putc);
	printf("attempting to get el_val\r\n");
	int el =get_el();
	
	printf("got el_val\r\n");
	printf("Exception level: %d \r\n", el);
	while (1) {
		printf("looooop\r\n");
		uart_send(uart_recv());
	}
}
