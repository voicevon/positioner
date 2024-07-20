#include "adc12_a.h"
#include "driverlib.h"
#include "delay.h"
#include "./ri_signal/pid/pid.h"
#include "./ri_signal/filter/formular/ax_b.h"
#include "gpio.h"
#include "intrinsics.h"
#include "msp430f5529.h"
#include "timer_a.h"
#include "app_motor.h"


extern void init_CLOCK(void);
extern void init_GPIO(void);
extern void init_ADC_REF(void);
extern void init_ADC12(void);
// extern void init_PWM_TIMER_A0(void);
extern void init_PWM_TIMER_B0(void);
extern void init_timer_T1A1(void);
extern bool init_UART(uint16_t baseAddress, uint32_t Baudrate);
extern void UART_printf(uint16_t baseAddress, const char *format,...);

// extern int check_keys();
volatile bool g_timer_10ms_flag = false;

int check_keys();

enum Enum_State{
    IDLE = 0,
    WORKING = 1,
    MOTOR_CAL_MOVE_TO_MAX = 2,
    MOTOR_CAL_MOVING_TO_MAX =3,
    MOTOR_CAL_MOVE_TO_MIN = 4,
    MOTOR_CAL_MOVING_TO_MIN = 5,
    PID_CAL_ON_KP_RESET,
    PID_CAL_KP,
    PID_CAL_ON_KI_RESET,
    PID_CAL_KI,
    PID_CAL_ON_KD_RESET,
    PID_CAL_KD,
};

Enum_State  g_state = Enum_State::IDLE;
int g_uptime_per_10ms = 0;
static int idle_down_counter = 50;

unsigned long adc_result[2] ={0, 0};
Motor my_motor = Motor();
signalx::PIDController my_pid_main = signalx::PIDController(1, 0, 0, 999,999);


void to_state(Enum_State new_state){
	// printf("g_state is chaning from:  %s  to:  %s", AppMain::GetStateName(g_state), AppMain::GetStateName(new_state));
    // UART_printf(USCI_A1_BASE, "from %s, to %s", g_state, new_state);
	g_state = new_state;
}

void state_machine(){
	static int force;
    static int key_value = 0;
	switch (g_state){
		case IDLE:
			idle_down_counter--;
			if (idle_down_counter == 0){
				g_state = WORKING;
			}
			break;
		case WORKING:
            key_value = check_keys();
			switch (key_value){
			case 5:
				// 进入标定 Motor
				g_state = MOTOR_CAL_MOVE_TO_MAX;
				break;
			case 6:
				// 进入标定 PID
				g_state = PID_CAL_ON_KP_RESET;
				break;
            case 7:
				g_state = PID_CAL_ON_KP_RESET;
                break;
            case 8:
				g_state = PID_CAL_ON_KP_RESET;
                break;
			default:
				// 显示当前开度

				// PID 控制。 当前还不敢运行之。
				// force = my_pid_main.FeedBack(my_motor.PV);
				// my_motor.SetForce(force);

                // 仅为了调试方便，无需按钮，直接进入一级标定，
                g_state =  MOTOR_CAL_MOVE_TO_MAX;
                UART_printf(USCI_A1_BASE, "to state:  MOTOR_CAL_MOVE_TO_MAX\n");
				break;
			} 
			break;
		case MOTOR_CAL_MOVE_TO_MAX:
			if (my_motor.GetVelocity() < 10){
				// 没有达到预期速度，增加力度
                my_motor.Dump("MOVE_TO_MAX  ");
				my_motor.ForceUp(10);
			}else{
				// 开始正确方向移动了， 记住 这时候的 force 值
                my_motor.Dump("MOV_TO_MAX  ");

				my_motor.SaveForce_ForwardMin();
				to_state(MOTOR_CAL_MOVING_TO_MAX);
                UART_printf(USCI_A1_BASE, "---------------------------------------------------------->  MOVING_TO_MAX\n");
			}
			break;
		case MOTOR_CAL_MOVING_TO_MAX:
            my_motor.Dump("MOVING_TO_MAX  ");
			if (my_motor.GetVelocity() < 1){
				// 到达开度最大位置
                UART_printf(USCI_A1_BASE, "---------------------------------------------------------->   GOT_MAX_POSITION");
				my_motor.SavePosition_Max();
				my_motor.ForceUp(-100);
				to_state(MOTOR_CAL_MOVE_TO_MIN);
			} 
			break;
		case MOTOR_CAL_MOVE_TO_MIN:
			if (my_motor.GetVelocity() > -500){
				//没有移动，增加力度。
				my_motor.ForceUp(-3);

			} else {
				//开始向 Min方向移动了, 记录这个力度。
				my_motor.SaveForce_BackwardMax();
				to_state(MOTOR_CAL_MOVING_TO_MIN);
			}
			break;
		case MOTOR_CAL_MOVING_TO_MIN:
			if (my_motor.GetVelocity() > -5){
				//到达重点了，当前位置是： Min, 开度 百分之零 的位置
				my_motor.SavePosition_Min();
				//motor标定结束，应用并保存标定结果。
				my_motor.UpdateFilter();
				my_motor.SaveToFile();
				// to_state(IDLE);
			}
			break;
		default:
			break;
		}
}


void load_config(){

}

void main(void)
{
    WDT_A_hold(WDT_A_BASE);
    init_CLOCK();
    init_GPIO();
    init_ADC_REF();

    init_ADC12();
    // init_PWM_TIMER_A0();
    init_PWM_TIMER_B0();
    init_timer_T1A1();
    init_UART(USCI_A1_BASE, 9600);

    __enable_interrupt();

    UART_printf(USCI_A1_BASE, "Hello world");
    UART_printf(USCI_A1_BASE, "\n%d\n", 9600);
    // 进入休眠，10ms定时器 将 唤醒系统
    __bis_SR_register(LPM0_bits + GIE);
    //for Debugger ?
    __no_operation();
    while(1)
    {   
            // 启动 ADC 多通道 单次转换。
            ADC12_A_startConversion(ADC12_A_BASE, ADC12_A_MEMORY_0, ADC12_A_SEQOFCHANNELS);
            // 进入休眠状态，ADC12_A 转换完毕后，系统被唤醒，继续运行
            // __bis_SR_register(LPM0_bits + GIE);
            //for Debugger ?
            // __no_operation();
            my_motor.SetAdc(adc_result[0]);
            // UART_printf(USCI_A1_BASE, "adc=%d, velocity=%d \n ", adc_result[0], my_motor.GetVelocity());
            state_machine();


            // 再次进入休眠，10ms定时器 将 唤醒系统
            __bis_SR_register(LPM0_bits + GIE);
            //for Debugger ?
            __no_operation();


            //1秒反转 LED, 确认定时正确
            g_uptime_per_10ms++;            
            if (g_uptime_per_10ms % 100 == 0){
                
                GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
                if (g_uptime_per_10ms % 200 == 0){
                    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);
                }
            }
    }
}


#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    // CPU不会自动清除这个 中断标志 ？
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);

    // 退出休眠模式，主程序可以继续
    __bic_SR_register_on_exit(LPM0_bits);

    // g_uptime_per_10ms++;
    // if (g_uptime_per_10ms % 100 == 0){
    //     //反转 LED, 确认定时正确
    //     GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);
    //     if (g_uptime_per_10ms % 200 == 0){
    //         GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);
    //     }
    // }
}

