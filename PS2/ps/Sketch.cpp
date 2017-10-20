#include <Arduino.h>
#include <LiquidCrystal.h>
#include "Menu/menu.h"
#include "Read_Temp//read_temp.h"
#include <PID_v1.h>
#include "Sketch.h"

//variable for detect principal/menu screen
T_E_SCREEN screen;

T_E_BUTTON activeButton_menuStart = E_BUTTON_NONE;

float Temperature_Celsius;

//vafiable used to save Start Temperature
float initial_Temperature_Celsius;

T_UBYTE StartFlag;

//PID Parameters
double setpoint;
double input;
double output;

//variable for time(for every stage)
T_UBYTE stage_time = 60;

//Enum for state machine
typedef enum E_STATE_MACHINE_STAGE
{
	E_STAGE_HEATING,
	E_STAGE_MANTAIN,
	E_STAGE_COOLING
}T_E_STATE_MACHINE_STAGE;

//variable for state machine
T_E_STATE_MACHINE_STAGE e_sm_State = E_STAGE_HEATING;	//E_STAGE_HEATING - initial value

//PID object
PID ovenPID(&input, &output, &setpoint, get_PID_tempControl().Kp, get_PID_tempControl().Ki, get_PID_tempControl().Kd, DIRECT);

void setup() {
	// Initializes serial communication
	Serial.begin(9600);
	
	init_display();
	LCD_creteChar();
	screen = E_SCREEN_PRINCIPAL;
	//analogReference(INTERNAL);

	error_print();
	
	PID_Start();

	
	//Set Start flag for first EEPROM read
	StartFlag = 1;
	
}

void PID_Start(void)
{
	// Ramp up to minimum temperature
	setpoint = read_temp_celsius();
	initial_Temperature_Celsius =  read_temp_celsius();
	ovenPID.SetOutputLimits(0, 255);
	ovenPID.SetSampleTime(100);
	
	// Turn the PID on
	ovenPID.SetMode(AUTOMATIC);
	ovenPID.SetTunings(get_PID_tempControl().Kp, get_PID_tempControl().Ki, get_PID_tempControl().Kd);

	// Proceed to preheat stage
}

void TempControllerUpdate()
{
	input = Temperature_Celsius;
	ovenPID.Compute();
	analogWrite(PWM_PIN,output);
}

void Stage_State_Machine()
{
	switch (e_sm_State)
	{
		case E_STAGE_HEATING:
		{
			setpoint += (get_PID_tempControl().Tset - initial_Temperature_Celsius)/60.0;
			get_lcd().setCursor(0,1);
			get_lcd().print("Incalzire ");
			get_lcd().setCursor(11,1);
			get_lcd().print(stage_time);
			
			
			//change state after 1 minute
			if(stage_time == 0)
			{
				e_sm_State = E_STAGE_MANTAIN;
				stage_time = 60;
			}
			delay(700);
			stage_time--;			//increment the time
		}break;
		
		case E_STAGE_MANTAIN:
		{
			setpoint = get_PID_tempControl().Tset;
			get_lcd().setCursor(0,1);
			get_lcd().print("Mentinere");
			get_lcd().setCursor(11,1);
			get_lcd().print(stage_time);
			
			//change state after 1 minute
			if(stage_time == 0)
			{
				e_sm_State = E_STAGE_COOLING;
				stage_time = 60;
			}
			
			delay(700);
			stage_time--;			//increment the time
		}break;
		
		case E_STAGE_COOLING:
		{
			setpoint -= (get_PID_tempControl().Tset - initial_Temperature_Celsius)/60.0;

			get_lcd().setCursor(0,1);
			get_lcd().print("Racire   ");
			get_lcd().setCursor(11,1);
			get_lcd().print(stage_time);

			
			
			if(stage_time == 0)
			{
				e_sm_State = E_STAGE_HEATING;
				stage_time = 60;
			}
			
			delay(700);
			stage_time--;			//increment the time
		}break;
		
		default:
		{
			//do nothing
		}
	}
	
}


//Principal screen buttons
//|4|   |3|   |2|   |1|
//			        MENU

void loop() {

	Temperature_Celsius = read_temp_celsius();
	
	printCourrentTemperature(Temperature_Celsius);
	

	if(screen == E_SCREEN_PRINCIPAL)
	{
		Stage_State_Machine();		//call SM function
		
		TempControllerUpdate();	//Update PID Temperature Control

		draw_principalScreen(&activeButton_menuStart, &StartFlag);
	}
	
	switch (activeButton_menuStart)
	{
		case E_BUTTON_NONE:
		break;
		
		case E_BUTTON_1:
		{
			screen = E_SCREEN_MENU;
			mainMenuDraw();
			drawCursor();
			operateMainMenu(&screen, &activeButton_menuStart);
			
		}break;

		default:
		{
			//do nothing
		}
		break;
	}
}

