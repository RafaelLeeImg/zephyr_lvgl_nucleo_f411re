README.md
=========


This project is a demo project to use LVGL with SSD1306 0.96 inch OLED screen and NUCLEO_F411RE board.


The program will stuck at /home/r/proj/arm/west/zephyr/include/arch/arm/aarch32/asm_inline_gcc.h:59

As I understand this program, the 


```C
		"mov %1, %2;"
		"mrs %0, BASEPRI;"
		"msr BASEPRI_MAX, %1;" // here
		"isb;"
		: "=r"(key), "=r"(tmp)
		: "i"(_EXC_IRQ_DEFAULT_PRIO)
		: "memory");
```

The code will be compiled into the following lines:

```asm
   0x0800371e <+42>:	movs	r3, #0
   0x08003720 <+44>:	str	r3, [r7, #12]
   0x08003722 <+46>:	ldr	r3, [r7, #12]
=> 0x08003724 <+48>:	msr	BASEPRI, r3
   0x08003728 <+52>:	isb	sy
```

When running the line "msr	BASEPRI, r3" $r3 is 0. That will pend all ISR with the priority higher 0. Basically all.

The debugging method in GDB including set bit CPIEVTENA in DWT_CTRL to enable cycle count.
More debugging detail are in gdbscript in the repository.

After 90 clock cycles (937ns), the program is stuck at the breakpoint of z_arm_usage_fault.

```bash
# DWT_CTRL bit 17, CPIEVTENA, Enables CPI count event.
set *0xE0001000 = 0x20001
# reset DWT_CYCCNT value, Cycle count register
set *0xE0001004 = (long int)0x0
# DWT_CPICNT, Instruction cycle count event register
set *0xE0001008 = (long int)0x0
```
