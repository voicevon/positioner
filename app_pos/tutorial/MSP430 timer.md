[Using the Real-Time Clock library](https://www.ti.com/lit/an/slaa290a/slaa290a.pdf?ts=1720161210287&ref_url=https%253A%252F%252Fwww.google.com%252F#:~:text=It%20is%20often%20the%20case,and%20displays%20the%20current%20time.)
* 一秒是最小时间单位。
* 我们继续寻找 ms 为单位的  api. 

[用定时器实现 10ms 一次中断](http://www.ocfreaks.com/msp430-timer-programming-tutorial/)
* 定时器的工作模式： Up Mode, Continuouse Mode,  Up-Down Mode. 
** 由 TACTL MCx设置。 如： TA0CTL = MC_1， Up Mode, Timer counts up to TACCR0.
* 代码示例中
**  #pragma vector = TIMER0_A0_VECTOR


[用定时器实现中断 ](https://www.embeddedrelated.com/showarticle/182.php)



