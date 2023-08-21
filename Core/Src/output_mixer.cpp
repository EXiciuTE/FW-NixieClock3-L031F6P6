/*
 * output_mixer.c
 *
 *  Created on: May 15, 2023
 *      Author: erikf
 *  @brief: class for managing all outgoing signals. Generates commands for the output handler and led driver
 */

#include <output_mixer.hpp>


/**
 * @brief Processes all data and generates commands for Output Handler and LED driver
 *
 * gets called every ms
 * gets data directly from respective .hpp files
 */
void run_output_mixer(uint8_t input){
	// get all data
	// compare if something changed
	// handle led animation state?
	// make sure that a minimum of tubes or points is active!

	/**
	 * menu 0: Clock
	 * menu 1: LED brightness - last 3 digits show value; first three are off
	 * 	change led with push
	 * menu 2: display/change one digit - points are on!
	 * 	change digit with push
	 * menu 3: set time - seconds are 0
	 * 	hours blink, change
	 * 	minutes blink, change
	 * press - safe time
	 */

	// Point 0 and 2 allways deactivated because not connected
//	set_point(0, false);
//	set_point(2, false);

	if(input==4){
		if(current_menu == 1 || current_menu==2 || current_menu==3){
			current_state++;
		}
		else{
			current_state=0;
			current_menu++;
		}
	}
	if(current_menu==4)
		current_menu=0;


	if(current_menu == 0){		//standard clock
		for(uint8_t i=0; i<6; i++){
			set_color(i,OFF,25);	//hotfix für Papa
		}
		if(data_to_RTC.new_data!=true){				//do not refresh tube with data from RTC before new data is written to RTC
													//without this, the old time will shine for a splitsecond when writing new time
			set_number(0, data_from_RTC.hours/10);
			set_number(1, data_from_RTC.hours%10);
			set_number(2, data_from_RTC.minutes/10);
			set_number(3, data_from_RTC.minutes%10);
			set_number(4, data_from_RTC.seconds/10);
			set_number(5, data_from_RTC.seconds%10);
			if((data_from_RTC.seconds %2) ==true){
				set_point(0, false);
				set_point(1, false);
				set_point(2, true);
				set_point(3, true);
			}
			else{
				set_point(0, true);
				set_point(1, true);
				set_point(2, false);
				set_point(3, false);
			}
		}
	}

	if(current_menu == 1){		//LED brightness control
		if(current_state==4){
			current_menu++;
			current_state=0;
		}
		switch(current_state){
			case 0: active_color = GREEN; break;
			case 1: active_color = RED; break;
			case 2: active_color = MAGENTA; break;
			case 3: active_color = ORANGE; break;
			default: break;
		}

		switch(input){
			case 0x1:	brightness+=10;	break;
			case 0x2:	brightness-=10;	break;
			default: break;
		}
		if(brightness==246 || brightness>110)
			brightness = 100;
		if(brightness==110)
			brightness = 0;

		for(uint8_t i=0; i<6; i++){
			if(brightness == 0)
				set_color(i,OFF,brightness);
			else
				set_color(i,active_color,brightness);
		}

		set_number(0, 0xA);
		set_number(1, 0xA);
		set_number(2, 0xA);
		set_number(board_size-3, brightness/100);
		set_number(board_size-2, (brightness/10)%10);
		set_number(board_size-1, brightness % 10);
	}

	if(current_menu == 2){	//time set function	- state 0 to 4
		if(current_state == 3 && board_size==4){
			current_state++;
			data_to_RTC.seconds = 0;
		}

		if(current_state == 4){
			current_menu=0;
			data_to_RTC.new_data = true;
		}
		if(current_state!=old_state){
			for(uint8_t i=0; i<6; i++){
				set_color(i,GREEN,25);
			}
			old_state=current_state;
		}

		if(current_state == 0){
			data_to_RTC.seconds = data_from_RTC.seconds;
			data_to_RTC.minutes = data_from_RTC.minutes;
			data_to_RTC.hours = data_from_RTC.hours;
			current_state++;
		}

		if(timeout(blink_1_timer)==true){	//500ms loop
			blink_1_timer = start_timer_ms(500);
			if(blink_state_1==true){
				set_color((current_state*2)-1,OFF,25);
				set_color((current_state*2)-2,OFF,25);
			}
			else{
				set_color((current_state*2)-1,GREEN,25);
				set_color((current_state*2)-2,GREEN,25);
			}
			blink_state_1 = !blink_state_1;
		}

		switch(current_state){
			case 0x1: number_value = data_to_RTC.hours; break;
			case 0x2: number_value = data_to_RTC.minutes; break;
			case 0x3: number_value = data_to_RTC.seconds; break;
			default: break;
		}

		switch(input){
			case 0x1:	number_value++;	break;
			case 0x2:	number_value--;	break;
			default: break;
		}

		if(((number_value==60) && ((current_state==3) || (current_state==2))) || (number_value==24 && current_state==1))
			number_value = 0;
		if(number_value==255){
			if(current_state==3 || current_state==2)
				number_value = 59;
			else
				number_value = 23;
		}

		switch(current_state){
			case 0x1: data_to_RTC.hours = number_value; break;
			case 0x2: data_to_RTC.minutes = number_value; break;
			case 0x3: data_to_RTC.seconds = number_value; break;
			default: break;
		}

		set_number(0, data_to_RTC.hours/10);
		set_number(1, data_to_RTC.hours%10);
		set_number(2, data_to_RTC.minutes/10);
		set_number(3, data_to_RTC.minutes%10);
		set_number(4, data_to_RTC.seconds/10);
		set_number(5, data_to_RTC.seconds%10);
	}

	// switch HV-enable pin by long press of button - change led to indicate state (RED = ON, GREEN = OFF)
	static bool tmp = false;
	if(input==0x8){		//Long Press
		flyback_state = !flyback_state;
		tmp = set_flyback_state(flyback_state);
	}
	if(tmp != true)
		set_color(0, RED , 1);	//hotfix für Papa

	// Tube Output-Data
	if(timeout(output_mixer_tube_timer)){
		output_mixer_tube_timer = start_timer_ms(TUBE_REFRESH_RATE_MS);
		set_output();
	}

	//	Time LED-Output
	if(timeout(output_mixer_led_timer)){
		output_mixer_led_timer = start_timer_ms(LED_REFRESH_RATE_MS);
//		for(uint8_t i=1; i<6; i++){
//			set_color(i,led_info[i][0],led_info[i][1]);
//		}
		send_data(true);
	}
}


