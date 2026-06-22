# Bare-metal RISC-V on the SiFive FE310 — Exercises

> These exercises accompany the **Bare-metal RISC-V on the SiFive FE310** workshop
> run within **CC Chip.si**, the Slovenian Competence Center on Chips and
> Semiconductor Technologies ([cc-chip.si](https://cc-chip.si/about/)).

A set of three hands-on exercises in bare-metal C for the **SiFive FE310-G002**,
the RISC-V microcontroller on the **HiFive1 Rev B** board. You build, flash, and
run each one on the real device. The exercises build on each other: you start by
driving a single GPIO pin and finish with a timer interrupt that blinks the LED
on its own.

Work through them in order — each folder contains its own `README.md` with the
task and the steps to follow.

## Exercises

| #  | Folder                | What you do                                                        |
| -- | --------------------- | ----------------------------------------------------------------- |
| 1  | `01-led-blink/`       | Blink the on-board RGB LED through the GPIO registers.            |
| 2  | `02-uart-print/`      | Send `Hello, FE310!` to a PC over the UART.                       |
| 3  | `03-timer-interrupt/` | Use the machine-timer interrupt to blink the LED in the background. |

## Prerequisites

- A **HiFive1 Rev B** board (FE310-G002) and a USB cable.
- **SiFive Freedom Studio** installed and working. See the book's
  [*Setting up Freedom Studio*](https://bulicp.github.io/riscv-fe310-book/freedom-studio.html)
  page for project creation, building, flashing, and opening a serial console.
- A read through the relevant book chapter before each exercise — the folder
  README points you to the right section.

## How to use this repository

```
.
├── README.md
├── 01-led-blink/
│   └── README.md
├── 02-uart-print/
│   └── README.md
└── 03-timer-interrupt/
    └── README.md
```

1. Open the exercise folder and read its `README.md`.
2. Create or import the project in Freedom Studio.
3. Build, flash, and observe the result on the board.

## Reference

The companion book is online at
[bulicp.github.io/riscv-fe310-book](https://bulicp.github.io/riscv-fe310-book/welcome.html).