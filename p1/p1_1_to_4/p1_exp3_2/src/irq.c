#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"
const char *entry_error_messages[] = {
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",		
    "FIQ_INVALID_EL1t",		
    "ERROR_INVALID_EL1T",		

    "SYNC_INVALID_EL1h",		
    "IRQ_INVALID_EL1h",		
    "FIQ_INVALID_EL1h",		
    "ERROR_INVALID_EL1h",		

    "SYNC_INVALID_EL0_64",		
    "IRQ_INVALID_EL0_64",		
    "FIQ_INVALID_EL0_64",		
    "ERROR_INVALID_EL0_64",	

    "SYNC_INVALID_EL0_32",		
    "IRQ_INVALID_EL0_32",		
    "FIQ_INVALID_EL0_32",		
    "ERROR_INVALID_EL0_32"	
};

void enable_interrupt_controller()
{
    printf("enable int cont \r\t");    
// Enables Core 0 Timers interrupt control for the generic timer 
    //put32(TIMER_INT_CTRL_0, TIMER_INT_CTRL_0_VALUE);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
    printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
    printf("invalid entry\r\n");
}

void han(void){
	printf("error fixed\n");
}

void handle_sync(void){
    int el = get_el();
    printf("handling exception at EL%d\n",el);
}
