# Exercise 3 — Blink from a timer interrupt (CLINT machine timer in C)

> These exercises accompany the **Bare-metal RISC-V on the SiFive FE310** workshop
> run within **CC Chip.si**, the Slovenian Competence Center on Chips and
> Semiconductor Technologies ([cc-chip.si](https://cc-chip.si/about/)).

So far the LED changed because `main` sat in a busy-wait (`primitivedelay`) between
writes. In this exercise the **machine-timer interrupt** does the work instead: the
CPU is interrupted at a fixed interval, an **interrupt handler** toggles the LED,
and `main` is left with an **empty loop**. This is your first step from polling
into interrupt-driven firmware.

> Read [§3 *RISC-V interrupts and exceptions*](https://bulicp.github.io/riscv-fe310-book/interrupts/index.html),
> [§3.3 *FE310 interrupts and the CLINT*](https://bulicp.github.io/riscv-fe310-book/interrupts/fe310-interrupts.html),
> and [§3.5 *Vector table and handlers*](https://bulicp.github.io/riscv-fe310-book/interrupts/vector-table.html)
> before you start.

---

## What you'll practise

- Reading the **CLINT** machine-timer registers `mtime` and `mtimecmp`.
- Writing an **interrupt handler** in C with the `interrupt` attribute.
- Decoding `mcause` to confirm which interrupt fired.
- **Re-arming** a level-triggered timer so it doesn't fire forever.
- **Registering** the handler through a vectored `mtvec` table and enabling the
  machine-timer and global interrupts.

---

## How the machine timer works

The CLINT exposes a free-running 64-bit counter, `mtime`, and a compare register,
`mtimecmp`. The timer interrupt is **asserted whenever `mtime >= mtimecmp`** — it
is *level-triggered*, with no flag to clear. The only way to lower the line is to
move `mtimecmp` ahead of `mtime`. So the **first job of the handler is to re-arm**:
push `mtimecmp` one interval into the future. Forget this and the handler re-enters
immediately after `mret`, hanging the program.

On the FE310 `mtime` ticks at **32768 Hz**, so `+16384` ticks is about **0.5 s**
and `+32768` would be a full second.

---

## The HAL / runtime you'll use

GPIO is the same as before; the new pieces are the CLINT registers and a small
interrupt runtime declared in `main.h` and implemented in `csr.S`:

| Name                                          | What it is                                                   |
| --------------------------------------------- | ------------------------------------------------------------ |
| `CLINT_MTIME` / `MTIME_ADDRESS`               | Address of the 64-bit `mtime` counter.                       |
| `CLINT_MTIME_CMP` / `MTIMECMP_ADDRESS`        | Address of the 64-bit `mtimecmp` compare register.           |
| `__attribute__((weak, interrupt))`            | Marks a C function as an ISR (save/restore + `mret`); `weak` lets it override the table's default slot. |
| `mtime_handler`                               | The name the vector table jumps to for the timer (cause 7).  |
| `_register_handler(_vector_table, INT_MODE_VECTORED)` | Points `mtvec` at the table in vectored mode.        |
| `_enable_global_interrupts()`                 | Sets the global interrupt enable (`mstatus.MIE`).            |
| `_enable_mtimer_interrupt()`                  | Enables the machine-timer source (`mie.MTIE`).              |
| `HAL_GPIO_TogglePin(GPIO, pin)`               | Flips a pin's output level.                                 |

> **Registering = naming.** You don't fill a pointer into a table at run time.
> The vector table in `csr.S` already jumps to `mtime_handler` for cause 7; you
> "register" your handler simply by **defining a function with that exact name**.
> The `weak` default in the runtime steps aside for yours.

---

## Starter code

Open `main.c` and copy it to the project. The GPIO setup and the `mcause` read are
already in place; you fill in the handler body and the interrupt setup in `main`.

## Step-by-step

### Step 0 — Set up the project

Create and configure the project as described in
[*Setting up Freedom Studio*](https://bulicp.github.io/riscv-fe310-book/freedom-studio.html),
then import the necessary files into your project:

- GPIO driver: `hal_gpio.h`, `hal_gpio.c`
- the interrupt runtime: `main.h` and `csr.S`
- the vector table: `vectored_interrupts.S`
- starter code: `main.c`

### Step 1 — Complete the interrupt handler

Inside the `if (mcause_value & 0x8000007)` branch, do three things in order:

1. **Toggle** the blue LED with `HAL_GPIO_TogglePin`.
2. **Re-arm** the timer: set `mtimecmp` to the current `mtime` plus your interval
   (`16384` ticks ≈ 0.5 s).
> **Hint — writing a 64-bit memory-mapped register.** 
> To store a computed value into a register at a known address, cast the address to a pointer and write
> through it:
>
> ```c
> *(uint64_t*)REG1_ADDRESS = *(uint64_t*)REG2_ADDRESS + (uint64_t)VALUE;
> ```
>
> `REG1_ADDRESS` is the register you write, `REG2_ADDRESS` is the one you read,
> and `VALUE` is the constant interval you add. Work out which CLINT register is
> which, and what interval you want.
> 
3. **Set** `mtime_int_flag` so the rest of the program can tell a tick occurred.

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Step 1a: Toggle the blue LED
HAL_GPIO_TogglePin(GPIO, LED_BLUE_PIN);

// Step 1b: Re-arm — next deadline is +16384 ticks (~0.5 s at 32768 Hz)
*(uint64_t*)MTIMECMP_ADDRESS = *(uint64_t*)MTIME_ADDRESS + (uint64_t)16384;

```

</details>

### Step 2 — Register the vector table

In `main`, after the GPIO init, point `mtvec` at the runtime's vector table in
vectored mode.

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Set vectored interrupts (declared in csr.S)
_register_handler(_vector_table, INT_MODE_VECTORED);
```

</details>

### Step 3 — Enable interrupts and arm the timer

Three calls, in this order: enable global interrupts, set the **first** deadline by
writing `mtimecmp`, then enable the machine-timer source. (Setting the deadline
before enabling the source avoids a spurious immediate interrupt.)

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Enable global interrupts (declared in csr.S)
_enable_global_interrupts();

// First timer deadline
*(uint64_t*)MTIMECMP_ADDRESS = (uint64_t)16384;

// Enable the machine-timer interrupt (declared in csr.S)
_enable_mtimer_interrupt();
```

</details>

### Step 4 — Leave main idle

`main` has nothing left to do: the empty `while(1)` just keeps the program alive
while the handler runs on every timer tick. Flash and watch the blue LED blink on
its own — no `primitivedelay` anywhere.

---


## Solution

Try the steps yourself first. When you want to check your work, expand the full
solution below.

<details>
<summary><strong>Show the full solution</strong></summary>

```c
#include <stdint.h>

#include "hal_gpio.h"
#include "main.h"

#define CLINT_BASE        0x02000000
#define CLINT_MSIP        0x02000000
#define CLINT_MTIME       0x0200bff8
#define CLINT_MTIME_CMP   0x02004000

#define MTIME_ADDRESS     ((uint64_t *)CLINT_MTIME)
#define MTIMECMP_ADDRESS  ((uint64_t *)CLINT_MTIME_CMP)

// Pointer to the GPIO device
GPIO_TypeDef *GPIO = GPIO_BASE_ADDRESS;

unsigned int mtime_int_flag = 0;

__attribute__ ((weak, interrupt))
void mtime_handler (void) {

    volatile unsigned int mcause_value;
    // mcause is a non memory-mapped CSR; read it with a CSR instruction:
    __asm__ volatile ("csrr %0, mcause"
                      : "=r" (mcause_value)
                      :
                      :);

    if (mcause_value & 0x8000007) {   // machine-timer interrupt

        // Toggle the blue LED
        HAL_GPIO_TogglePin(GPIO, LED_BLUE_PIN);

        // Re-arm — next deadline is +16384 ticks (~0.5 s at 32768 Hz)
        *(uint64_t*)MTIMECMP_ADDRESS = *(uint64_t*)MTIME_ADDRESS + (uint64_t)16384;

        // Record that a tick happened
        mtime_int_flag = 1;

    }
    else {   // not the timer — ignore
        __asm__ volatile("nop");
    }
}

int main() {

    // GPIO setup: blue LED as output
    GPIO_InitTypeDef cfg = {0};
    cfg.Mode = GPIO_MODE_OUTPUT;
    cfg.Pin  = LED_BLUE_PIN;
    HAL_GPIO_Init(GPIO, &cfg);

    // Set vectored interrupts (in csr.S)
    _register_handler(_vector_table, INT_MODE_VECTORED);

    // Enable global interrupts (in csr.S)
    _enable_global_interrupts();

    // First timer deadline
    *(uint64_t*)MTIMECMP_ADDRESS = (uint64_t)16384;

    // Enable the machine-timer interrupt (in csr.S)
    _enable_mtimer_interrupt();

    while(1){
        // Nothing to do — the handler blinks the LED on every tick.
    }

    return 0;
}
```

</details>

---

## Where to go next

You've gone from polling to interrupt-driven code. The book's capstone,
[§3.6 *Case study: a round-robin scheduler*](https://bulicp.github.io/riscv-fe310-book/interrupts/scheduler.html),
builds on exactly this machine-timer interrupt to switch between tasks — a tiny
preemptive multitasking kernel.