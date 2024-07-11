
#include "driverlib.h"

// 返回值： 0==没按下  1= 首次按下， 2= 保持按下
int check_key_P21(){
	static int key_pressed_counter = 0;
	int level = GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN1);  // P2.1 是开发板上的按钮
	if (level == 0 ){
        if (key_pressed_counter == 0){
			// 首次检测到这个按钮被按下
			key_pressed_counter++;
			return 1;
        }else{
            // 这个按钮刚才就被按下了
            key_pressed_counter++;
            if (key_pressed_counter >= 50){
                return 2;
            }
        }
    } else {
        // 这个按钮没有被按下
        key_pressed_counter = 0;
    }
	return 0;
}
int check_key_P22(){
	static int key_pressed_counter = 0;
	int level = GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN2);  
	if (level == 0 ){
        if (key_pressed_counter == 0){
			// 首次检测到这个按钮被按下
			key_pressed_counter++;
			return 1;
        }else{
            // 这个按钮刚才就被按下了
            key_pressed_counter++;
            if (key_pressed_counter >= 50){
                return 2;
            }
        }
    } else {
        // 这个按钮没有被按下
        key_pressed_counter = 0;
    }
	return 0;
}
int check_key_P23(){
	static int key_pressed_counter = 0;
	int level = GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN3);  
	if (level == 0 ){
        if (key_pressed_counter == 0){
			// 首次检测到这个按钮被按下
			key_pressed_counter++;
			return 1;
        }else{
            // 这个按钮刚才就被按下了
            key_pressed_counter++;
            if (key_pressed_counter >= 50){
                return 2;
            }
        }
    } else {
        // 这个按钮没有被按下
        key_pressed_counter = 0;
    }
	return 0;
}
int check_key_P24(){
	static int key_pressed_counter = 0;
	int level = GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN4);  
	if (level == 0 ){
        if (key_pressed_counter == 0){
			// 首次检测到这个按钮被按下
			key_pressed_counter++;
			return 1;
        }else{
            // 这个按钮刚才就被按下了
            key_pressed_counter++;
            if (key_pressed_counter >= 50){
                return 2;
            }
        }
    } else {
        // 这个按钮没有被按下
        key_pressed_counter = 0;
    }
	return 0;
}

// 返回值： 1,2,3,4 = 键值press,   5 = enter is hold.
int check_keys(){
    int result = check_key_P21();
	if (result == 1){
		return 1;
	}else if (result ==2){
		return 5;
	}
    result = check_key_P22();
	if (result == 1){
		return 2;
	}else if (result ==2){
		return 6;
	}
    result = check_key_P23();
	if (result == 1){
		return 3;
	}else if (result ==2){
		return 7;
	}
    result = check_key_P24();
	if (result == 1){
		return 4;
	}else if (result ==2){
		return 8;
	}
    
	return 0;
}

