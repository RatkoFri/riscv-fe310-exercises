# Exercise 2 — Print over UART (GPIO + UART in C)

> These exercises accompany the **Bare-metal RISC-V on the SiFive FE310** workshop
> run within **CC Chip.si**, the Slovenian Competence Center on Chips and
> Semiconductor Technologies ([cc-chip.si](https://cc-chip.si/about/)).

This exercise **builds on Exercise 1**. You keep the R → G → B LED cycle, and on
each step you also print a line to your PC over the **UART**. By the end you'll see
the colour names scrolling in a serial terminal in sync with the LED.

```
Red on!
Green on!
Blue on!
Red on!
...
```

> Read [§2 *Case study: the UART on the FE310*](https://bulicp.github.io/riscv-fe310-book/mmio/index.html)
> and [§2.3 *Programming the UART in C*](https://bulicp.github.io/riscv-fe310-book/mmio/uart-in-c.html)
> before you start.

---

## What you'll practise

- Configuring the UART (baud rate, stop bits, direction) through a HAL config struct.
- Initialising **UART0** for transmit.
- Sending a string with `HAL_UART_Transmit`, and getting the **byte count** right.
- Driving two peripherals — GPIO and UART — from the same loop.

---

## The HAL you'll use

The GPIO calls are the same as Exercise 1. The new pieces come from `hal_uart.h`:

| Name                                       | What it is                                              |
| ------------------------------------------ | ------------------------------------------------------- |
| `UART_InitTypeDef`                         | Config struct with `.BaudRate`, `.StopBits`, `.Mode`.   |
| `UART_BAUD_115200`                         | Baud-rate setting (115200 bps).                         |
| `UART_STOPBITS_1`                          | One stop bit.                                           |
| `UART_MODE_TX`                             | Transmit-only mode.                                     |
| `UART0`                                     | Handle to the UART0 peripheral.                         |
| `HAL_UART_Init(UART0, &cfg)`               | Applies a configuration to the UART.                    |
| `HAL_UART_Transmit(UART0, data, length)`   | Sends `length` bytes of `data`.                         |

> **Counting bytes.** `HAL_UART_Transmit` needs the exact number of bytes to send.
> The line ending `\r\n` is **two** bytes. For example `"Red on!\r\n"` is **9**
> bytes, not 7. A wrong count either truncates the message or sends garbage past
> the string.

---

## Starter code

Open `main.c` and copy it to the project. The GPIO setup from Exercise 1 is already
filled in; the parts you write are the UART steps marked with numbered comments.

## Step-by-step

### Step 0 — Set up the project

Create and configure the project as described in
[*Setting up Freedom Studio*](https://bulicp.github.io/riscv-fe310-book/freedom-studio.html),
then import the necessary HAL library files into your project:

- GPIO driver: `hal_gpio.h`, `hal_gpio.c`
- UART driver: `hal_uart.h`, `hal_uart.c`
- starter code: `main.c`

### Step 1 — Configure the UART

Create a `UART_InitTypeDef` and fill in three fields: a baud rate of
`UART_BAUD_115200`, `UART_STOPBITS_1`, and `UART_MODE_TX` (you only need to send,
not receive).

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Step 1: Configuration for the UART.
//         The config structure is declared in hal_uart.h.
UART_InitTypeDef cfg_uart = {
    .BaudRate = UART_BAUD_115200,
    .StopBits = UART_STOPBITS_1,
    .Mode     = UART_MODE_TX,
};
```

</details>

### Step 2 — Initialise UART0

Call `HAL_UART_Init` with the `UART0` handle and the address of your config struct.

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Step 2: Initialize UART0 with the configuration.
HAL_UART_Init(UART0, &cfg_uart);
```



</details>

### Step 3 — Print on each colour

Inside `while(1)`, after lighting each colour, send the matching message with
`HAL_UART_Transmit`. Use the correct byte length for each string (remember `\r\n`
is two bytes):

- After **red** on: `"Red on!\r\n"` → 9 bytes
- After **green** on: `"Green on!\r\n"` → 11 bytes
- After **blue** on: `"Blue on!\r\n"` → 10 bytes

Example code for printing a string over UART0 is:

```c
HAL_UART_Transmit(UART0, "Hello, FE310!\r\n", 15);
```


<details>
<summary><strong>Show the solution</strong></summary>

```c
while(1){
    // Red on
    HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_RESET); // red on
    HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
    HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
    HAL_UART_Transmit(UART0, "Red on!\r\n", 9);
    primitivedelay(500000);

    // Green on
    HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
    HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_RESET); // green on
    HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
    HAL_UART_Transmit(UART0, "Green on!\r\n", 11);
    primitivedelay(500000);

    // Blue on
    HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
    HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
    HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_RESET); // blue on
    HAL_UART_Transmit(UART0, "Blue on!\r\n", 10);
    primitivedelay(500000);
}
```

</details>

### Step 4 — Open a serial terminal

Open a serial console (in Freedom Studio, or any terminal such as `screen` /
PuTTY) on the board's port at **115200 baud, 8 data bits, no parity, 1 stop bit
(8N1)**. You should see the colour names print in step with the LED.

---

## Checkpoints

You're done when:

- [ ] The LED still cycles red → green → blue from Exercise 1.
- [ ] Each colour prints its matching line over the UART, in sync with the LED.
- [ ] The byte counts are correct — no truncated or trailing-garbage messages.
- [ ] You can explain why the UART is set to `UART_MODE_TX` and not RX.

---

## Solution

Try to complete all the steps yourself first. When you want to check your work,
expand the full solution below.

<details>
<summary><strong>Show the full solution</strong></summary>

```c
#include "hal_gpio.h"
#include "hal_uart.h"

// This function should be reentrant as it is called by several threads
void primitivedelay(unsigned int delay){
    while(delay > 0){
        __asm__ volatile ( "nop ;"
                           "nop");
        delay--;
    }
}

int main() {

    // --- GPIO setup (from Exercise 1) ---
    GPIO_InitTypeDef cfg_gpio = {0};
    cfg_gpio.Mode = GPIO_MODE_OUTPUT;
    cfg_gpio.Pin  = LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN;

    GPIO_TypeDef *GPIO = GPIO_BASE_ADDRESS;
    HAL_GPIO_Init(GPIO, &cfg_gpio);

    // --- UART setup ---
    UART_InitTypeDef cfg_uart = {
        .BaudRate = UART_BAUD_115200,
        .StopBits = UART_STOPBITS_1,
        .Mode     = UART_MODE_TX,
    };
    HAL_UART_Init(UART0, &cfg_uart);

    // Cycle R -> G -> B forever, printing on each step.
    // LEDs are active-low: RESET = on, SET = off.
    while(1){
        // Red on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_RESET); // red on
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
        HAL_UART_Transmit(UART0, "Red on!\r\n", 9);
        primitivedelay(500000);

        // Green on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_RESET); // green on
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
        HAL_UART_Transmit(UART0, "Green on!\r\n", 11);
        primitivedelay(500000);

        // Blue on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_RESET); // blue on
        HAL_UART_Transmit(UART0, "Blue on!\r\n", 10);
        primitivedelay(500000);
    }

    return 0;
}
```

</details>

---

## What's next

Next is **Exercise 3 — Timer interrupt**, where a machine-timer interrupt drives
the LED change in the background instead of a busy-wait delay.