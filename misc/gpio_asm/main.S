# ----------------------------------------------------------------------------
#
# Blink the blue LED (assembly)
# Uses the GPIO driver in gpio.s
#
# ----------------------------------------------------------------------------

.section .text

.option norvc                       # disable compressed instructions

.equ LED_BLUE_PIN, 21               # blue LED is on GPIO 21
.equ DELAY_COUNT,  500000           # software-delay iterations


/*---------------------------------------
    main: configure the blue LED as an
    output, then blink it forever using a
    software delay.
---------------------------------------*/
.balign 4
.global main
.type main, @function
main:
    # prologue (save the return address and frame pointer)
    addi sp, sp, -16                # Allocate the routine stack frame
    sw ra, 12(sp)                   # Save the return address
    sw fp, 8(sp)                    # Save the frame pointer
    addi fp, sp, 16                 # Set the frame pointer

    # configure the blue LED pin as an output
    li a0, LED_BLUE_PIN
    call gpio_output_en

blink_loop:
    # LED ON: clear the pin (drive output low; LED is active-low)
    li a0, LED_BLUE_PIN
    call gpio_clear_pin
    li a0, DELAY_COUNT
    call primitive_delay

    # LED OFF: set the pin (drive output high)
    li a0, LED_BLUE_PIN
    call gpio_set_pin
    li a0, DELAY_COUNT
    call primitive_delay

    j blink_loop                    # while(1)

    # epilogue (never reached — the loop above never exits)
    lw fp, 8(sp)                    # restore the frame pointer
    lw ra, 12(sp)                   # restore the return address
    addi sp, sp, 16                 # de-allocate the routine stack frame
    ret


/*---------------------------------------
    primitive_delay: busy-wait loop.

    # a0 - iteration count

    Leaf routine: it calls nothing and
    touches only a0 (caller-saved), so it
    needs no stack frame.
---------------------------------------*/
.balign 4
.global primitive_delay
.type primitive_delay, @function
primitive_delay:
delay_test:
    beqz a0, delay_done             # while (count > 0)
    nop
    nop
    addi a0, a0, -1                 # count--
    j delay_test
delay_done:
    ret