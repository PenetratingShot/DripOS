#include "timer.h"
#include "isr.h"
#include "ports.h"
#include "../libc/function.h"
#include "../drivers/screen.h"
<<<<<<< HEAD
#include "sysState.h"
=======
>>>>>>> ab5c5fe1324ce987a89224738614a54b559d9946

u32 tick = 0;
u32 prev = 0;

static void timer_callback(registers_t regs) {
<<<<<<< HEAD
	sys_state_manager();
=======
>>>>>>> ab5c5fe1324ce987a89224738614a54b559d9946
    tick++;
    UNUSED(regs);
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);

}

void wait(u32 ticks) {
	prev = tick;
	while(tick - prev < ticks) {
		
	}
}