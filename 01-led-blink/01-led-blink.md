# Exercise 1 — Cycle the RGB LED (GPIO in C)

> These exercises accompany the **Bare-metal RISC-V on the SiFive FE310** workshop
> run within **CC Chip.si**, the Slovenian Competence Center on Chips and
> Semiconductor Technologies ([cc-chip.si](https://cc-chip.si/about/)).

In this first exercise you drive the on-board **RGB LED** of the **HiFive1 Rev B**
through a small GPIO **HAL** (hardware abstraction layer). Your goal is to light
the three colours one at a time, in a repeating pattern:

```
RED  ->  GREEN  ->  BLUE  ->  RED  ->  GREEN  ->  BLUE  ->  ...
```

Only one colour is on at any moment, and the pattern loops forever with a short
delay between steps.

> Read [§1 *Case study: the GPIO interface on the FE310*](https://bulicp.github.io/riscv-fe310-book/mmio/gpio-index.html)
> and [§1.2 *Programming GPIO in C*](https://bulicp.github.io/riscv-fe310-book/mmio/gpio-c.html)
> before you start.

---

## What you'll practise

- Configuring GPIO pins as **outputs** through a HAL configuration structure.
- Getting a handle to the GPIO peripheral and **initialising** it.
- Writing pin states with a HAL call, and understanding **active-low** LEDs.

---

## The HAL you'll use

Everything you need is declared in `hal_gpio.h`:

| Name                                  | What it is                                                        |
| ------------------------------------- | ----------------------------------------------------------------- |
| `GPIO_InitTypeDef`                    | Configuration struct with `.Mode` and `.Pin` fields.              |
| `GPIO_MODE_OUTPUT`                    | Mode value that makes a pin an output.                            |
| `LED_RED_PIN`, `LED_GREEN_PIN`, `LED_BLUE_PIN` | Bit masks for the three LED pins.                       |
| `GPIO_TypeDef`, `GPIO_BASE_ADDRESS`   | Peripheral type and its base address.                            |
| `HAL_GPIO_Init(GPIO, &cfg)`           | Applies a configuration to the GPIO device.                      |
| `HAL_GPIO_WritePin(GPIO, pin, state)` | Drives a pin to `GPIO_PIN_SET` or `GPIO_PIN_RESET`.              |

> **Active-low LEDs.** The on-board RGB LED is **active-low**: writing
> `GPIO_PIN_RESET` (0) turns a colour **on**, and `GPIO_PIN_SET` (1) turns it
> **off**. Keep this in mind throughout — it is the most common source of "why is
> my LED inverted?" confusion.

---

## Starter code

Open `main.c` and copy it to the project. The structure is already in place; the parts you write are marked
with numbered steps.

## Step-by-step

### Step 0 — Set up the project

Create and configure the project as described in
[*Setting up Freedom Studio*](https://bulicp.github.io/riscv-fe310-book/freedom-studio.html),
then import the necessary HAL library files into your project:

- `hal_gpio.h`
- `hal_gpio.c`


### Step 1 — Describe the pin configuration

Create a `GPIO_InitTypeDef` and zero-initialise it. Set its mode to
`GPIO_MODE_OUTPUT`, and set its `Pin` field to the **combined mask** of all three
LED pins using bitwise OR (`|`). Combining the masks lets one configuration apply
to all three pins at once.

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Step 1: Set up the configuration.
//         The config structure is declared in hal_gpio.h.
GPIO_InitTypeDef cfg = {0};
cfg.Mode = GPIO_MODE_OUTPUT;                          // pins act as outputs
cfg.Pin  = LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN; // select all three LEDs
```

</details>

### Step 2 — Get a handle to the GPIO device

Declare a `GPIO_TypeDef *` pointing at `GPIO_BASE_ADDRESS`. This is how your code
reaches the peripheral's memory-mapped registers through the HAL.



<details>
<summary><strong>Show the solution</strong></summary>

```c
// Step 2: Set up pointer to the GPIO device.
GPIO_TypeDef *GPIO = GPIO_BASE_ADDRESS;
```

</details>

### Step 3 — Initialise the device

Call `HAL_GPIO_Init` with your device pointer and the address of your config
struct. After this, the three pins are driving outputs.

<details>
<summary><strong>Show the solution</strong></summary>

```c
// Step 3: Initialize the GPIO device with the configuration.
HAL_GPIO_Init(GPIO, &cfg);
```

</details>



### Step 4 — Cycle the colours

Inside `while(1)`, produce the pattern. For **each** step, set the colour you want
**on** to `GPIO_PIN_RESET` and the other two to `GPIO_PIN_SET`, then call
`primitivedelay(500000)`:

1. Red on, green off, blue off → delay
2. Green on, red off, blue off → delay
3. Blue on, red off, green off → delay

The loop repeats, giving you R → G → B forever.

Example code for one step is:

```c
HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN, GPIO_PIN_RESET);  // turn the blue LED on
```

> **Tip.** Writing all three pins every step (rather than only the one that
> changed) keeps the logic obvious and avoids leaving a colour stuck on.

<details>
<summary><strong>Show the solution</strong></summary>

```c
while(1){
        // Red on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_RESET); // red on
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
        primitivedelay(500000);

        // Green on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_RESET); // green on
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
        primitivedelay(500000);

        // Blue on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_RESET); // blue on
        primitivedelay(500000);
    }
```

</details>


---

## Solution

Try to complete all four steps yourself first. When you want to check your work,
expand the solution below.

<details>
<summary><strong>Show the full solution</strong></summary>

```c
#include "hal_gpio.h"

// This function should be reentrant as it is called by several threads
void primitivedelay(unsigned int delay){
    while(delay > 0){
        __asm__ volatile ( "nop ;"
                           "nop");
        delay--;
    }
}

int main() {

    // Step 1: Set up the configuration.
    //         The config structure is declared in hal_gpio.h.
    GPIO_InitTypeDef cfg = {0};
    cfg.Mode = GPIO_MODE_OUTPUT;                          // pins act as outputs
    cfg.Pin  = LED_RED_PIN | LED_GREEN_PIN | LED_BLUE_PIN; // select all three LEDs

    // Step 2: Set up pointer to the GPIO device.
    GPIO_TypeDef *GPIO = GPIO_BASE_ADDRESS;

    // Step 3: Initialize the GPIO device with the configuration.
    HAL_GPIO_Init(GPIO, &cfg);

    // Step 4: Cycle R -> G -> B forever (LEDs are active-low: RESET = on, SET = off).
    while(1){
        // Red on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_RESET); // red on
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
        primitivedelay(500000);

        // Green on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_RESET); // green on
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_SET);   // blue off
        primitivedelay(500000);

        // Blue on
        HAL_GPIO_WritePin(GPIO, LED_RED_PIN,   GPIO_PIN_SET);   // red off
        HAL_GPIO_WritePin(GPIO, LED_GREEN_PIN, GPIO_PIN_SET);   // green off
        HAL_GPIO_WritePin(GPIO, LED_BLUE_PIN,  GPIO_PIN_RESET); // blue on
        primitivedelay(500000);
    }

    return 0;
}
```

</details>

---
