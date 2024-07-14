#include "adc12_a.h"
#include "driverlib.h"
#include "delay.h"
#include "./ri_signal/pid/pid.h"
#include "./ri_signal/filter/formular/ax_b.h"
#include "gpio.h"
#include "intrinsics.h"
#include "timer_a.h"
#include "app_motor.h"


extern void init_CLOCK(void);
extern void init_GPIO(void);
extern void init_ADC_REF(void);
extern void init_ADC12(void);
// extern void init_PWM_TIMER_A0(void);
extern void init_PWM_TIMER_B0(void);
extern void init_timer_T1A1(void);

// extern int check_keys();
volatile bool g_timer_10ms_flag = true;

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
static int idle_down_counter = 50;

unsigned long adc_result[2] ={0, 0};
Motor my_motor = Motor();
signalx::PIDController my_pid_main = signalx::PIDController(1, 0, 0, 999,999);


void to_state(Enum_State new_state){
	// printf("g_state is chaning from:  %s  to:  %s", AppMain::GetStateName(g_state), AppMain::GetStateName(new_state));
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

				// PID 控制
				force = my_pid_main.FeedBack(my_motor.PV);
				my_motor.SetForce(force);
                g_state =  MOTOR_CAL_MOVE_TO_MAX;
				break;
			} 
			break;
		case MOTOR_CAL_MOVE_TO_MAX:
			if (my_motor.GetVelocity() < 50){
				// 没有达到预期速度，增加力度
				my_motor.ForceUp(10);
			}else{
				// 开始正确方向移动了， 记住 这时候的 force 值
				my_motor.SaveForce_ForwardMin();
				to_state(MOTOR_CAL_MOVING_TO_MAX);
			}
			break;
		case MOTOR_CAL_MOVING_TO_MAX:
			if (my_motor.GetVelocity() < 50){
				// 到达开度最大位置
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
				to_state(IDLE);
			}
			break;
		default:
			break;
		}
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

    __enable_interrupt();

    while(1)
    {   
        if (g_timer_10ms_flag){
            // 10ms 进入一次
            g_timer_10ms_flag = false;

            // 是否要判断  is_busy() ?
            adc_result[0] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_0);
            adc_result[1] = ADC12_A_getResults(ADC12_A_BASE, ADC12_A_MEMORY_1);

            my_motor.SetAdc(adc_result[0]);
            state_machine();
            // 启动 ADC 多通道 单次转换。
            ADC12_A_startConversion(ADC12_A_BASE, ADC12_A_MEMORY_0, ADC12_A_SEQOFCHANNELS);
        }
    }
}


#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
{
    static int ms_counter = 0;
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    ms_counter++;
    if (ms_counter >=10){
        // 每 10ms 进入一次
        g_timer_10ms_flag = true;
        ms_counter = 0;
    }
}

