#include <stdint.h>

#include "hal_gpio.h"
#include "main.h"

// CLINT (Core-Local Interruptor) memory map
#define CLINT_BASE        0x02000000
#define CLINT_MSIP        0x02000000
#define CLINT_MTIME       0x0200bff8
#define CLINT_MTIME_CMP   0x02004000

#define MTIME_ADDRESS     ((uint64_t *)CLINT_MTIME)
#define MTIMECMP_ADDRESS  ((uint64_t *)CLINT_MTIME_CMP)

// Pointer to the GPIO device (global so the handler can reach it)
GPIO_TypeDef *GPIO = GPIO_BASE_ADDRESS;

// Set by the handler so the rest of the program can observe a tick
unsigned int mtime_int_flag = 0;

// Machine-timer interrupt handler.
//   interrupt -> compiler emits the register save/restore and 'mret'
//   weak      -> overrides the default slot-7 entry in the vector table
__attribute__ ((weak, interrupt))
void mtime_handler (void) {

    volatile unsigned int mcause_value;
    // mcause is a non memory-mapped CSR, so it can only be read with a
    // CSR instruction — hence the inline assembly:
    __asm__ volatile ("csrr %0, mcause"
                      : "=r" (mcause_value)  /* output */
                      :                      /* input : none */
                      :                      /* clobbers: none */);

    if (mcause_value & 0x80000007) {   // machine-timer interrupt

        // Step 1a: Toggle the blue LED


        // Step 1b: Re-arm the timer — set mtimecmp one interval into the future
        //          (+16384 ticks ~ 0.5 s, since mtime runs at 32768 Hz)



    }
    else {   // not the timer — ignore
        __asm__ volatile("nop");
    }
}

int main() {

    // --- GPIO setup: blue LED as output ---
    GPIO_InitTypeDef cfg = {0};
    cfg.Mode = GPIO_MODE_OUTPUT;
    cfg.Pin  = LED_BLUE_PIN;
    HAL_GPIO_Init(GPIO, &cfg);

    // Step 2: Register the vector table in vectored mode.
    //         _register_handler, _vector_table and INT_MODE_VECTORED are in csr.S

    // Step 3: Enable global interrupts (_enable_global_interrupts, in csr.S)

    // Step 4: Set the first timer deadline by writing mtimecmp

    // Step 5: Enable the machine-timer interrupt (_enable_mtimer_interrupt, in csr.S)

    while(1){
        // Nothing to do here — the LED now blinks from the interrupt handler.
    }

    return 0;
}