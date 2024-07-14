


code with driverlib tutorial   
https://embedded-lab.com/blog/tinkering-ti-msp430f5529/3/ 

1 Tinkering TI MSP430F5529
* dev board
* TI MSP430Ware Driver Library and Other Stuffs

2 Digital Input-Output  - DIO

3 Unified Clock System - UCS



4 Alphanumerical LCD

5 2 Wire (Software I2C) LCD

6 3 Wire (Softwrare SPI) LCD

7 Low Power Modes (LPM) and External Interrupt

8 Internal Flash Memory and Cyclic Redundancy Check (CRC) Module



# 9 Timer Overview
[timers of F5529](https://embedded-lab.com/blog/tinkering-ti-msp430f5529/9/#:~:text=MSP430F5529%20microcontroller%20has%20six%20timers,Real%20Time%20Clock%20(RTC).&text=The%20cut%2Daway%20block%20diagram,shown%20below%20highlights%20these%20timers.)

There are 6 timers in 3 catagories
* T0A  5CC
* T1A  3CC
* T2A  3CC
* Real Time Clock A
* Watchdog Timer A

10 Free-Running Timer – TA0

11 Timer Interrupt – TA1

12 Single Pulse Width Module (PWM) – TA2

13 Multiple Pulse Width Module(PWM) -TA0
* TA0 has 5 CC channels, 可以应用在 P1.2, P1.3, P1.4, ?, ?

14 Timer as Counter - TA2

15 Timer Input Capture Mode - TA0
* an input-side feature, for measuremnet of pulse widths. frequency and time period of an incoming waveform.

16 Real Time Clock - RTC-A

17 RTC-A as Counter

18 Watchdog Timer - WDTA

19 WDTA as an Interval TimerWDTA as an Interval TimerWDTA as an Interval Timer

20 Hardware Multiplier Module - MPY32

21 Analogue Hardware Overview
* ADC12_A  and Comp_B+

22 REF Module and ADC12 in polling Mode
* REF module is not clock dependent.
* REF module doesn't require use of any external pin.
* Can be set to 1.5V / 2.0V / 2.5V
* 应用示例 芯片内 温度传感器 连接到 ADC12

23 ADC12 Interrupt


24 ADC12 Sampling Mutiple Input

25 ADC12 and Direct Memory Access(DMA) Module

26 Analogue Comparator - Comp_B+ Module

27 Communication Hardware Overview

28 One-Wire Communication with DHT11

29 One-Wire Communication with DS18B20

30 USCI - SPI - I2C Example 1
* MMA7455L, I2C接口， 加速度传感器
* ST7735, SPI 接口 TFT显示屏

31 USCI - SPI - I2C Example 2
* MAX6675, SPI接口 热电偶
* SSD1306, I2C接口 OLED显示屏

32 USCI - SPI - I2C Example 3
* BH1750, I2C接口，光传感器， 
* PCD8544, SPI接口 GLCD显示屏

33 USCI - UART

34 USB Module Overview

35 USB CDC

36 USB HID

37 Eplogue




[Offical download website](https://www.ti.com/tool/MSPDRIVERLIB#downloads)