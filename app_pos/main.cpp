#include "driverlib.h"
#include "delay.h"
#include "./ri_signal/pid/pid.h"
#include "timer_a.h"
// #include "lcd.h"
// #include "lcd_print.h"
// #include "mcu."
#include "./ri_signal/filter/formular/ax_b.h"
#include "app_motor.h"


extern void init_CLOCK(void);
extern void init_GPIO(void);
extern void init_ADC12(void);
extern void init_PWM_TIMER_A0(void);

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
static int idle_down_counter = 10;

unsigned long adc_result[2] ={0, 0};
Motor my_motor = Motor();
signalx::PIDController my_pid_main = signalx::PIDController(1, 0, 0, 999,999);

// signalx::Ax_B my_filter(1, 0);

// 返回值： 0==没按下  1= 首次按下， 2= 保持按下
int check_single_key(int io_num){
	static int last_pressed_io_num = -1;
	static int key_pressed_counter = 0;

	// int level = gpio_get_level(GPIO_NUM_0);
	int level = 1;
	if (level == 1 ){
		if (last_pressed_io_num == 9){
			// 这个按钮刚才就被按下了
			key_pressed_counter++;
			if (key_pressed_counter >= 50){
				return 2;
			}
		} else {
			// 首次检测到这个按钮被按下
			last_pressed_io_num = 9;
			key_pressed_counter = 0;
			return 1;
		}
	}
	return 0;
}

// 返回值： 1,2,3,4 = 键值press,   5 = enter is hold.
int check_keys(){
	int result = check_single_key(1);
	if (result ==1){
		return 1;
	}else if (result ==2){
		return 5;
	}

	result = check_single_key(2);
	if (result ==1){
		return 2;
	}else if (result ==2){
		return 6;
	}
	result = check_single_key(3);
	if (result ==1){
		return 3;
	}else if (result ==2){
		return 7;
	}
	result = check_single_key(4);
	if (result ==1){
		return 4;
	}else if (result ==2){
		return 8;
	}
	return 0;
}

void to_state(Enum_State new_state){
	// printf("g_state is chaning from:  %s  to:  %s", AppMain::GetStateName(g_state), AppMain::GetStateName(new_state));
	g_state = new_state;
}

void state_machine(){
	static int force;
	switch (g_state){
		case IDLE:
			idle_down_counter--;
			if (idle_down_counter == 0){
				g_state = WORKING;
			}
			break;
		case WORKING:
			switch (check_keys()){
			case 5:
				// 进入标定 Motor
				g_state = MOTOR_CAL_MOVE_TO_MAX;
				break;
			case 6:
				// 进入标定 PID
				g_state = PID_CAL_ON_KP_RESET;
				break;
			default:
				// 显示当前开度

				// PID 控制
				force = my_pid_main.FeedBack(my_motor.PV);
				my_motor.SetForce(force);
				break;
			} 
			break;
		case MOTOR_CAL_MOVE_TO_MAX:
			if (my_motor.GetVelocity() < 800){
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
    // my_filter.Reset();
    // my_filter.SetKnownPoint(0, 0);
    // my_filter.SetKnownPoint(4096, 1.0);

    init_CLOCK();
    init_GPIO();
    init_ADC12();
    init_PWM_TIMER_A0();
    // LCD_init();
    // LCD_clear_home();

    // LCD_goto(0, 0);
    // LCD_putstr("CH0:");

    // LCD_goto(0, 1);
    // LCD_putstr("CH1:");


    while(1)
    {   

        Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1, adc_result[1]);
        Timer_A_setCompareValue(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2, adc_result[1]+ 1000);
        state_machine();

        // 休眠，等待中断唤醒

        // delay_ms(100);
    };
}
