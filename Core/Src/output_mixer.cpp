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
	// Point 0 and 2 always deactivated because not connected

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

	//reset all outputs
	for(uint8_t i = 0; i<6; i++){
		set_point(i, false);
		set_number(i, 0xA);
		set_color(i, OFF, 25);
	}

	//manage menu - button pressen?
	//activate menu - enter submenu with data

	//menu system: 	default = 0 --> clock
	//				menu_select = 9 --> page to select menu 1-x ; 0=return to clock

	//enter menu
	if((current_menu == 0) && (input == 4) && (flyback_status==true)){
		input=0;
		current_menu = 9;
		selected_menu = 1;
	}
	//leave menu - without saving for menu 1 & 2
	if((input == 0x8) && (current_menu!=0) && (current_menu!=3)){
		current_menu = 0;
//		current_state = 0;
		old_state = 0;
		input = 0;
	}

	//"menu" standard clock
	if(current_menu == 0){
		submenu_0_display_time();
	}

	//menu time set
	if(current_menu == 1){
		submenu_1_set_time(input, new_selected_menu);
		new_selected_menu = false;
		input=0;
	}

	//menu date set
	if(current_menu == 2){
		submenu_2_set_date(input, new_selected_menu);
		new_selected_menu = false;
		input=0;
	}

	if(current_menu == 3){
		submenu_3_set_onoff(input, new_selected_menu);
		new_selected_menu = false;
		input=0;
	}

	//menu menu ;)
	if(current_menu == 9){
		new_selected_menu = submenu_9_menu_select(input);
		input=0;

	}

//	if(current_menu == 4){		//LED brightness control
//		if(current_state==4){
//			current_menu++;
//			current_state=0;
//		}
//		switch(current_state){
//			case 0: active_color = GREEN; break;
//			case 1: active_color = RED; break;
//			case 2: active_color = MAGENTA; break;
//			case 3: active_color = ORANGE; break;
//			default: break;
//		}
//
//		switch(input){
//			case 0x1:	brightness+=10;	break;
//			case 0x2:	brightness-=10;	break;
//			default: break;
//		}
//		if(brightness==246 || brightness>110)
//			brightness = 100;
//		if(brightness==110)
//			brightness = 0;
//
//		for(uint8_t i=0; i<6; i++){
//			if(brightness == 0)
//				set_color(i,OFF,brightness);
//			else
//				set_color(i,active_color,brightness);
//		}
//
//		set_number(0, 0xA);
//		set_number(1, 0xA);
//		set_number(2, 0xA);
//		set_number(board_size-3, brightness/100);
//		set_number(board_size-2, (brightness/10)%10);
//		set_number(board_size-1, brightness % 10);
//	}

	//############################ write data to flash ############################
	if(flash_write == true){
		bool temp = write_flash_new_data();
		if(temp == 0)
			flash_write = false;
	}


	//############################ run Output Handler  ############################

	static bool hv_on = false;
	static bool area_entered = false;	// variable only changes if area is entered/left - does occur once per area
	static bool area_left = false;
	bool inside_area = false;

	if(current_menu == 0){	//only check for time area, when not in the menu
		// switch HV-enable when time-area is entered/left
		// manual long press overwrites state until the next area is entered/left
		for(uint8_t i=0;i<8;i++){
			//if current day inside selected days for each time area
			if(data_from_RTC.day >= on_time[i][0] && data_from_RTC.day <= on_time[i][1]){
				if(	(data_from_RTC.hours == on_time[i][2] && data_from_RTC.minutes >= on_time[i][3] && on_time[i][2] != on_time[i][4]) ||
					(data_from_RTC.hours == on_time[i][4] && data_from_RTC.minutes <= on_time[i][5] && on_time[i][2] != on_time[i][4]) ||
					(on_time[i][2] == on_time[i][4] && data_from_RTC.hours == on_time[i][2] && data_from_RTC.minutes >= on_time[i][3] && data_from_RTC.minutes <= on_time[i][5]) ||
					(data_from_RTC.hours > on_time[i][2] && data_from_RTC.hours < on_time[i][4])){
						//currently inside of active time area
						if(area_entered == false){
							area_entered = true;
							area_left = false;
							flyback_status = true;
							hv_on = set_flyback_state(flyback_status);
						}
						inside_area = true;
						break;
				}
				inside_area = false;
			}
			else
				inside_area = false;

		}
		//check if we have to switch the hv off
		if(inside_area == false && area_left == false){
			area_entered = false;
			area_left = true;
			flyback_status = false;
			hv_on = set_flyback_state(flyback_status);
		}
	}
	if(inside_area == true)
		set_color(5, GREEN,10);
	else
		set_color(5, RED,10);

	// switch HV-enable pin by long press of button - change led to indicate state (RED = OFF, dark = ON)
	if(input==0x8){		//Long Press
		flyback_status = !flyback_status;
		hv_on = set_flyback_state(flyback_status);
	}
	if(hv_on != true)
		set_color(1, CYAN , 10);

	set_point(1, true);	//DEBUG
	// Tube Output-Data
	if(timeout(output_mixer_tube_timer)){
		output_mixer_tube_timer = start_timer_ms(TUBE_REFRESH_RATE_MS);
		set_output();
	}

	//	Time LED-Output
	if(timeout(output_mixer_led_timer)){
		output_mixer_led_timer = start_timer_ms(LED_REFRESH_RATE_MS);
		send_data(true);
	}
}

/**
 * @brief: default "menu" where the actual time from the RTC is displayed
 */
void submenu_0_display_time(void){
//	for(uint8_t i=0; i<6; i++){
//		set_color(i,OFF,25);	//hotfix für Papa
//	}
	if(data_to_RTC.new_data!=true){				//do not refresh tube with data from RTC before new data is written to RTC
												//without this, the old time will shine for a splitsecond when writing new time
		set_number(0, data_from_RTC.hours/10);
		set_number(1, data_from_RTC.hours%10);
		set_number(2, data_from_RTC.minutes/10);
		set_number(3, data_from_RTC.minutes%10);
		set_number(4, data_from_RTC.seconds/10);
		set_number(5, data_from_RTC.seconds%10);
		if((data_from_RTC.seconds %2) ==true){
			set_point(1, false);
			set_point(3, true);
		}
		else{
			set_point(1, true);
			set_point(3, false);
		}
	}
}

/**
 * @brief function to enter new time data - copies the current time, displays it, change it and tell the time handler to safe it to rtc
 * @param: enter input info 0x1=left; 0x2=right; 0x4=press; 0x8=long press
 * @param: new_entry set true, when menu is entered through menu select
 */
void submenu_1_set_time(uint8_t local_input, bool new_entry){
	uint8_t number_value = 0;
	static bool blink_state = false;
	static uint32_t blink_timer = 0;
	static uint8_t current_state = 0;

	if(new_entry == true){
		current_state = 1;
		data_to_RTC.seconds = data_from_RTC.seconds;
		data_to_RTC.minutes = data_from_RTC.minutes;
		data_to_RTC.hours = data_from_RTC.hours;
		blink_state = false;
	}

	//skip setting for seconds, they can't be shown anyway
	if(local_input==0x4){
		current_state++;
		local_input=0;
		blink_state = false;
	}

	if(current_state == 3 && board_size==4){
		current_state++;
		data_to_RTC.seconds = 0;
	}

	//leave setting and safe changes - go back to menu selection
	if(current_state == 4){
		current_menu = 9;
		data_to_RTC.new_data = 1;	//1 means new time data
		//TODO: add animation for safed data
	}

	//blink active digits
	if(blink_state==true){
		set_color((current_state*2)-1,OFF,25);
		set_color((current_state*2)-2,OFF,25);
	}
	else{
		set_color((current_state*2)-1,GREEN,25);
		set_color((current_state*2)-2,GREEN,25);
	}
	if(timeout(blink_timer)==true){	//500ms loop
		blink_timer = start_timer_ms(500);
		blink_state = !blink_state;
	}

	//copy time info into local variable for manipulation and checks
	switch(current_state){
		case 0x1: number_value = data_to_RTC.hours; break;
		case 0x2: number_value = data_to_RTC.minutes; break;
		case 0x3: number_value = data_to_RTC.seconds; break;
		default: break;
	}

	//change data according to input
	switch(local_input){
		case 0x1:	number_value++;	break;
		case 0x2:	number_value--;	break;
		default: break;
	}

	//correction for overrunning of valid value area
	if(((number_value==60) && ((current_state==3) || (current_state==2))) || (number_value==24 && current_state==1))
		number_value = 0;
	if(number_value==255){
		if(current_state==3 || current_state==2)
			number_value = 59;
		else
			number_value = 23;
	}

	//write manipulated value back to time struct
	switch(current_state){
		case 0x1: data_to_RTC.hours = number_value; break;
		case 0x2: data_to_RTC.minutes = number_value; break;
		case 0x3: data_to_RTC.seconds = number_value; break;
		default: break;
	}

	//display output
	set_number(0, data_to_RTC.hours/10);
	set_number(1, data_to_RTC.hours%10);
	set_number(2, data_to_RTC.minutes/10);
	set_number(3, data_to_RTC.minutes%10);
	set_number(4, data_to_RTC.seconds/10);
	set_number(5, data_to_RTC.seconds%10);
}

/**
 * @brief: function to set new date to RTC | day | date+month | year
 * @param: enter input info 0x1=left; 0x2=right; 0x4=press; 0x8=long press
 * @param: new_entry set true, when menu is entered through menu select
 */
void submenu_2_set_date(uint8_t local_input, bool new_entry){
	uint16_t value = 0;
	static bool blink_state = false;
	static uint32_t blink_timer = 0;
	static uint8_t current_state = 0;
	static uint16_t val_min[4]={0,0,0,1899};
	static uint16_t val_max[4]={8,32,13,2101};

	if(new_entry == true){
		current_state = 1;
		data_to_RTC.day = data_from_RTC.day;
		data_to_RTC.date = data_from_RTC.date;
		data_to_RTC.month = data_from_RTC.month;
		data_to_RTC.year = data_from_RTC.year;
		blink_state = false;
	}

	//skip setting for seconds, they can't be shown anyway
	if(local_input==0x4){
		current_state++;
		local_input=0;
		blink_state = false;
	}

	//leave setting and safe changes - go back to menu selection
	if(current_state == 5){
		current_menu = 9;
		data_to_RTC.new_data = 2;	//2 means new date data
		//TODO: add animation for safed data
	}

	//blink active digits
	uint16_t blink_color = 0;
	if(blink_state==true)
		blink_color = GREEN;
	else
		blink_color = 0;

	if(current_state==1)
		set_color(1,blink_color,25);
	if(current_state==2){
		set_color(0,blink_color,25);
		set_color(1,blink_color,25);
	}
	if(current_state==3){
		set_color(2,blink_color,25);
		set_color(3,blink_color,25);
	}
	if(current_state==4){
		set_color(0,blink_color,25);
		set_color(1,blink_color,25);
		set_color(2,blink_color,25);
		set_color(3,blink_color,25);
	}

	if(timeout(blink_timer)==true){	//500ms loop
		blink_timer = start_timer_ms(500);
		blink_state = !blink_state;
	}

	//copy time info into local variable for manipulation and checks
	switch(current_state){
		case 0x1: value = data_to_RTC.day; break;
		case 0x2: value = data_to_RTC.date; break;
		case 0x3: value = data_to_RTC.month; break;
		case 0x4: value = data_to_RTC.year; break;
		default: break;
	}

	//change data according to input
	switch(local_input){
		case 0x1:	value++;	break;
		case 0x2:	value--;	break;
		default: break;
	}

	//correction for overrunning of valid value area
	if(value==val_max[current_state-1])
		value = val_min[current_state-1]+1;
	if(value==val_min[current_state-1])
		value = val_max[current_state-1]-1;

	//write manipulated value back to time struct
	switch(current_state){
		case 0x1: data_to_RTC.day = value; break;
		case 0x2: data_to_RTC.date = value; break;
		case 0x3: data_to_RTC.month = value; break;
		case 0x4: data_to_RTC.year = value; break;
		default: break;
	}

	//display output
	if(current_state == 1){	//day
		set_number(1,data_to_RTC.day);
	}
	if(current_state == 2 || current_state == 3){	//date+month
		set_number(0,data_to_RTC.date/10);
		set_number(1,data_to_RTC.date%10);
		set_number(2,data_to_RTC.month/10);
		set_number(3,data_to_RTC.month%10);
	}
	if(current_state == 4){	//year
		set_number(0,data_to_RTC.year/1000);
		set_number(1,(data_to_RTC.year/100)%10);
		set_number(2,(data_to_RTC.year/10)%10);
		set_number(3,data_to_RTC.year%10);
	}
}

/**
 * @brief function to set different time-zones when the clock should be acitve
 * @param: enter input info 0x1=left; 0x2=right; 0x4=press; 0x8=long press
 * @param: new_entry set true, when menu is entered through menu select
 */
void submenu_3_set_onoff(uint8_t local_input, bool new_entry){
	uint8_t number_value = 0;
	static bool blink_state = false;
	static uint32_t blink_timer = 0;
	static uint8_t current_state = 0;
	static uint8_t current_substate = 0;
	static uint8_t otc[8][6] = {0};	//on time copy

	//current state: 0 - 8; each state contains 6 substates: start day, end day, start time hours/minutes, endtime hours/minutes

	if(new_entry == true){
		current_state = 0;
		current_substate = 0;
		blink_state = false;
		for(uint8_t i=0;i<8;i++){		//copy on_time to temporary register
			for(uint8_t j = 0; j<6; j++){
				otc[i][j] = on_time[i][j];
			}
		}
	}

	//handle menu control
	if(local_input == 0x4){
		current_substate++;
		if(current_substate==6){
			current_state++;
			current_substate = 0;
			for(uint8_t i=0;i<8;i++){		//copy temporary data back to on_time with every time profile changed
				for(uint8_t j = 0; j<6; j++){
					on_time[i][j] = otc[i][j];
				}
			}
		}
		local_input=0;
		blink_state = false;
	}

	if(local_input == 0x8){	//safe made changes and leave
		current_menu = 9;
		flash_write = true;
	}

	if(current_state == 8){
		current_state = 7;	//set state back to legal value to prevent writing in illegal memory space
		current_menu = 9;
		flash_write = true;
	}

	//menu function
	number_value = otc[current_state][current_substate];

	//blink active digits
	uint32_t blink_color = 0;
	uint32_t static_color = 0;

	if(current_substate == 0){
		static_color = RED;
		blink_color = GREEN;
	}
	if(current_substate == 1){
		static_color = GREEN;
		blink_color = RED;
	}
	else if(current_substate == 2 || current_substate == 3){
		static_color = blink_color = GREEN;
	}
	else if(current_substate > 3){	// current_substate == 4 || == 5
		static_color = blink_color = RED;
	}

	if(blink_state==!true)
		blink_color = 0;

	if(current_substate%2 == 0){	//when first two tubes display changed number
		set_color(0,blink_color,25);
		set_color(1,blink_color,25);
		set_color(2,static_color,25);
		set_color(3,static_color,25);
	}else{
		set_color(0,static_color,25);
		set_color(1,static_color,25);
		set_color(2,blink_color,25);
		set_color(3,blink_color,25);
	}

	if(timeout(blink_timer)==true){	//500ms loop
		blink_timer = start_timer_ms(500);
		blink_state = !blink_state;
	}

	//change data according to input
	switch(local_input){
		case 0x1:	number_value++;	break;
		case 0x2:	number_value--;	break;
		default: break;
	}

	if(current_substate == 0 || current_substate == 1){	//days
		if(number_value == 0)
			number_value = 7;
		if(number_value == 8)
			number_value = 1;
	}
	if(current_substate == 2 || current_substate == 4){ //hours
		if(number_value == 255)
			number_value = 23;
		if(number_value == 24)
			number_value = 0;
	}
	if(current_substate == 3 || current_substate == 5){ //minutes
		if(number_value == 255)
			number_value = 59;
		if(number_value == 60)
			number_value = 0;
	}

	otc[current_state][current_substate] = number_value;

	//display output
	if(current_substate == 0){
		set_number(1, number_value);
		set_number(3, otc[current_state][1]);
	}
	if(current_substate == 1){
		set_number(1, otc[current_state][0]);
		set_number(3, number_value);
	}
	if(current_substate == 2 || current_substate == 4){
		set_number(0, number_value/10);
		set_number(1, number_value%10);
		set_number(2, otc[current_state][current_substate+1]/10);
		set_number(3, otc[current_state][current_substate+1]%10);

	}
	if(current_substate == 3 || current_substate == 5){
		set_number(0, otc[current_state][current_substate-1]/10);
		set_number(1, otc[current_state][current_substate-1]%10);
		set_number(2, number_value/10);
		set_number(3, number_value%10);
	}
}

/**
 * @brief: function to select new setting option
 * @param: enter input info 0x1=left; 0x2=right; 0x4=press; 0x8=long press
 */
bool submenu_9_menu_select(uint8_t local_input){
	if(local_input == 0x1)
		selected_menu++;
	if(local_input == 0x2)
		selected_menu--;
	if(selected_menu == 255)
		selected_menu = 6;
	if(selected_menu == 7)
		selected_menu = 0;
	set_number(0, selected_menu);
	if(local_input == 0x4){
		current_menu = selected_menu;
		return true;
		//TODO: move current state to separat subfunctions
		//		current_state=0;	//reset state when entering new menu
	}
	else
		return false;
}

/**
 * @brief function to write new settings to flash, when input is complete
 * @return returns 0 if write was successful, else return 1
 */
bool write_flash_new_data(void){
	uint8_t temp = 1;
	uint8_t data = 0;
	uint32_t addr = UP_FLASH_ADDR;

	temp = HAL_FLASHEx_DATAEEPROM_Unlock();

	if(temp == 0){
		for(uint8_t i=0; i<50; i++){	//50 bytes to write in flash (8*6 time area; 2 led+points)
			if(i<48)
				data = on_time[i/6][i%6];
			else
				data = misc_setting[i-48];
			temp = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, addr, data);
			addr++;
		}
	}
	HAL_FLASHEx_DATAEEPROM_Lock();
	return temp;
}
