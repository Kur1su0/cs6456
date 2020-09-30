#include "utils.h"
#include "printf.h"
#include "sched.h"
#include "peripherals/timer.h"
#include "timer.h"

const unsigned int interval = ( (1<<26)/10);
//const unsigned int interval = 6710886;
//const unsigned int interval = 1 * 1000 * 1000;
unsigned int curVal = 0;

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
	gen_timer_reset(interval);
    timer_tick();
}
