/*
* menu.c
*
* Created: 29.03.2017 18:50:43
*  Author: Denis
*/

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "menu.h"
#include "../Sketch.h"


//set the LCD pins
extern LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Menu items - in a string vector
String menuItems[] = {"Kp", "Ki", "Kd", "Temp Setata", "T inc", "T Mem", "T Rac","Factory Reset"};

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b10101, // * * *
	0b01110, //  ***
	0b00100  //   *
};

byte upArrow[8] = {
	0b00100, //   *
	0b01110, //  ***
	0b10101, // * * *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100, //   *
	0b00100  //   *
};

byte menuCursor[8] = {
	B01000, //  *
	B00100, //   *
	B00010, //    *
	B00001, //     *
	B00010, //    *
	B00100, //   *
	B01000, //  *
	B00000  //
};

// Navigation button variables
int readKey;
int savedDistance = 0;

// Menu control variables
int menuPage = 0;
int maxMenuPages = 6;       //change if you add more items in menu
int cursorPosition = 0;

//Error flag variable
T_E_PS_ERROR e_saveError;

TempControl PID_tempControl;

_EXT void init_display()
{
	// Initializes and clears the LCD screen
	lcd.begin(16, 2);
	lcd.clear();
}

_EXT void LCD_creteChar()
{
	// Creates the byte for the 3 custom characters
	lcd.createChar(0, menuCursor);
	lcd.createChar(1, upArrow);
	lcd.createChar(2, downArrow);
}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
_EXT void mainMenuDraw(){
	
	lcd.clear();
	lcd.setCursor(1, 0);
	lcd.print(menuItems[menuPage]);
	lcd.setCursor(1, 1);
	lcd.print(menuItems[menuPage + 1]);
	
	if (menuPage == 0)
	{
		lcd.setCursor(15, 1);
		lcd.write(byte(2));	 //write \/
	}
	else if ((menuPage > 0) && (menuPage < maxMenuPages))
	{
		lcd.setCursor(15, 0);
		lcd.write(byte(1));   //write /\
		
		lcd.setCursor(15, 1);
		lcd.write(byte(2));   //write \/

	}
	else if (menuPage == maxMenuPages)
	{
		lcd.setCursor(15, 0);
		lcd.write(byte(1));   //end of menu => write /\
		
	}
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
_EXT void drawCursor() {
	for (int x = 0; x < 2; x++)
	{
		lcd.setCursor(0, x);
		lcd.print(" ");     // Erases current cursor
	}
	
	if (menuPage % 2 == 0) {
		if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
			lcd.setCursor(0, 0);
			lcd.write(byte(0));
		}
		if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
			lcd.setCursor(0, 1);
			lcd.write(byte(0));
		}
	}
	if (menuPage % 2 != 0) {
		if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
			lcd.setCursor(0, 1);
			lcd.write(byte(0));
		}
		if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
			lcd.setCursor(0, 0);
			lcd.write(byte(0));
		}
	}
}

_EXT void operateMainMenu(T_E_SCREEN* screen, T_E_BUTTON* activeButton_menuStart) {
	int activeButton = 0;
	while (activeButton == 0) {
		
	//	Serial.println("Loop_operateMainMenu");
		T_E_BUTTON button;
		readKey = analogRead(A0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		
		switch (button)
		{
			case E_BUTTON_NONE: // When button returns as 0 there is no action taken
			break;
			
			case E_BUTTON_1:  // This case will execute if the "forward" button is pressed
			{
			//	Serial.println("OK");
				
				button = E_BUTTON_NONE;
				
				switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
					
					case 0:
					{
						menuKp();
					}break;
					
					case 1:
					{
						menuKi();
					}break;
					
					case 2:
					{
						menuKd();
					}break;
					
					case 3:
					{
						menuTempSet();
					}break;
					
					case 4:
					{
						menuTinc();
					}break;
					
					case 5:
					{
						menuTmem();
					}break;
					
					case 6:
					{
						menuTrac();
					}break;
					
					case 7:
					{
						factoryReset();
					}
				}
				
				activeButton = 1;
				mainMenuDraw();
				drawCursor();
				

			}break;
			case E_BUTTON_2:
			{
				button = E_BUTTON_NONE;
				if (menuPage == 0) {
					cursorPosition = cursorPosition - 1;
					cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
				}
				if (menuPage % 2 == 0 && cursorPosition % 2 == 0) {
					menuPage = menuPage - 1;
					menuPage = constrain(menuPage, 0, maxMenuPages);
				}

				if (menuPage % 2 != 0 && cursorPosition % 2 != 0) {
					menuPage = menuPage - 1;
					menuPage = constrain(menuPage, 0, maxMenuPages);
				}

				cursorPosition = cursorPosition - 1;
				cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

				mainMenuDraw();
				drawCursor();
				activeButton = 1;
			//	Serial.println("UP");

			}break;
			
			case 3:
			{
				button = E_BUTTON_NONE;
				if (menuPage % 2 == 0 && cursorPosition % 2 != 0) {
					menuPage = menuPage + 1;
					menuPage = constrain(menuPage, 0, maxMenuPages);
				}

				if (menuPage % 2 != 0 && cursorPosition % 2 == 0) {
					menuPage = menuPage + 1;
					menuPage = constrain(menuPage, 0, maxMenuPages);
				}

				cursorPosition = cursorPosition + 1;
				cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
				mainMenuDraw();
				drawCursor();
				activeButton = 1;
				
			//	Serial.println("Down");
			}break;
			case 4:
			{
				*screen = E_SCREEN_PRINCIPAL;
				*activeButton_menuStart = E_BUTTON_NONE;
				
			//	Serial.println("PRINCIPAL SCREEN BUTTON_NONE");
				activeButton = 1;
			}break;
		}
	}
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
_LOC T_E_BUTTON getButton(int analogValue) {
	T_E_BUTTON button = E_BUTTON_NONE;
	
	if ((analogValue < 850) && (analogValue > 700))
	{
		button = E_BUTTON_1; // OK
	}
	else if ((analogValue < 650) && (analogValue > 500))
	{
		button = E_BUTTON_2; // UP
	}
	else if ((analogValue < 450) && (analogValue > 350))
	{
		button = E_BUTTON_3; // DOWN
	}
	else if ((analogValue > 150) && (analogValue < 250)) {
		
		button = E_BUTTON_4; // BACK
	}
	return button;
}

_LOC void menuKp() { // Function executes when you select the 2nd item from main menu
	int activeButton = 0;
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Kp = ");
	lcd.setCursor(5, 0);
	lcd.print(PID_tempControl.Kp);
	lcd.setCursor(0, 1);
	lcd.print("BACK  -  +  SAVE");
	

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Kp in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				lcd.print("SALVAT!");
				
				EEPROM.write(255, 1);		//No error
				
				TempControllerUpdate();
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
				
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Kp += 1;
				lcd.setCursor(5,0);
				lcd.print(PID_tempControl.Kp);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Kp -= 1;
				lcd.setCursor(5,0);
				lcd.print(PID_tempControl.Kp);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void menuKi() { // Function executes when you select the 2nd item from main menu
	int activeButton = 0;
	
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Ki = ");
	lcd.setCursor(5,0);
	lcd.print(PID_tempControl.Ki);
	lcd.setCursor(0,1);
	lcd.print("BACK  -  +  SAVE");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Ki in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				lcd.print("SALVAT!");
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				
				TempControllerUpdate();
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Ki += 1;
				lcd.setCursor(5,0);
				lcd.print(PID_tempControl.Ki);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Ki -= 1;
				lcd.setCursor(5,0);
				lcd.print(PID_tempControl.Ki);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void menuKd() { // Function executes when you select the 3rd item from main menu
	int activeButton = 0;
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Kd = ");
	lcd.setCursor(5,0);
	lcd.print(PID_tempControl.Kd);
	lcd.setCursor(0,1);
	lcd.print("BACK  -  +  SAVE");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Kd in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				
				TempControllerUpdate();
				lcd.print("SALVAT!");
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
				
				
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Kd += 1;
				lcd.setCursor(5,0);
				lcd.print(PID_tempControl.Kd);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Kd -= 1;
				lcd.setCursor(5,0);
				lcd.print(PID_tempControl.Kd);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void menuTempSet() { // Function executes when you select the 4th item from main menu
	int activeButton = 0;
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Tset = ");
	lcd.setCursor(7,0);
	lcd.print(PID_tempControl.Tset);
	lcd.setCursor(0,1);
	lcd.print("BACK  -  +  SAVE");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Tset in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				
				TempControllerUpdate();
				lcd.print("SALVAT!");
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Tset += 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Tset);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Tset -= 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Tset);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void menuTinc() {
	int activeButton = 0;
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Tinc = ");
	lcd.setCursor(7,0);
	lcd.print(PID_tempControl.Tinc);
	lcd.setCursor(0,1);
	lcd.print("BACK  -  +  SAVE");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Tinc in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				
				lcd.clear();
				lcd.setCursor(5,0);
				lcd.print("SALVAT!");
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				
			
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Tinc += 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Tinc);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Tinc -= 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Tinc);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void menuTmem() {
	int activeButton = 0;
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Tmem = ");
	lcd.setCursor(7,0);
	lcd.print(PID_tempControl.Tmem);
	lcd.setCursor(0,1);
	lcd.print("BACK  -  +  SAVE");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Tmem in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				lcd.print("SALVAT!");
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Tmem += 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Tmem);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Tmem -= 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Tmem);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void menuTrac() {
	int activeButton = 0;
	eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
	EEPROM.write(EEPROM_ERROR_FLAG, 0);		//Write error to EEPROM_ERROR_FLAG address in EEPROM

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Trac = ");
	lcd.setCursor(7,0);
	lcd.print(PID_tempControl.Trac);
	lcd.setCursor(0,1);
	lcd.print("BACK  -  +  SAVE");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save Trac in EEPROM
			{
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				lcd.print("SALVAT!");
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
			
			case E_BUTTON_2:
			{
				PID_tempControl.Trac += 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Trac);
			}break;
			
			case E_BUTTON_3:
			{
				PID_tempControl.Trac -= 1;
				lcd.setCursor(7,0);
				lcd.print(PID_tempControl.Trac);
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "back" button is pressed
			{
				EEPROM.write(EEPROM_ERROR_FLAG, 1);		//No error
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}

_LOC void factoryReset()
{
	int activeButton = 0;

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("Resetati valori?");
	lcd.setCursor(0, 1);
	lcd.print("NU            DA");

	while (activeButton == 0) {
		T_E_BUTTON button;
		readKey = analogRead(0);
		if (readKey < D_MAX_BUTTON_VALUE) {
			delay(BT_PRESSED_DELAY);
			readKey = analogRead(0);
		}
		button = getButton(readKey);
		switch (button) {
			case E_BUTTON_1://Save All parameters in EEPROM
			{
				PID_tempControl.Kd = 20;
				PID_tempControl.Ki = 20;
				PID_tempControl.Kp = 20;
				PID_tempControl.Tcurenta = 20;
				PID_tempControl.Tinc = 20;
				PID_tempControl.Tmem = 20;
				PID_tempControl.Trac = 20;
				PID_tempControl.Tset = 20;
				
				eepromWrite(0,&PID_tempControl,sizeof(PID_tempControl));
				lcd.clear();
				lcd.setCursor(5,0);
				lcd.print("RESETAT!");
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
			
			case E_BUTTON_4:  // This case will execute if the "NU" button is pressed
			{
				lcd.clear();
				lcd.setCursor(5,0);
				lcd.print("ANULAT!");
				delay(1500);
				button = E_BUTTON_NONE;
				activeButton = 1;
			}break;
		}
	}
}


_EXT T_E_BUTTON readMenuButton()
{
	T_E_BUTTON button;
	readKey = analogRead(A0);
	if (readKey < D_MAX_BUTTON_VALUE) {
		delay(BT_PRESSED_DELAY);
		readKey = analogRead(0);
	}
	button = getButton(readKey);
	return button;
}


_EXT void draw_principalScreen(T_E_BUTTON *activeButton_menuStart, T_UBYTE *startFlag)
{

	//read EEPROM first time(one time, for principal screen)
	if (*startFlag == 1)
	{
		eepromRead(0,&PID_tempControl,sizeof(PID_tempControl));
		PID_Start();
		*startFlag = 0;
		//Serial.println("START PROGRAM; READ EEPROM 1 TIME");
	}

	*activeButton_menuStart = readMenuButton();
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Ts:");
	lcd.setCursor(3,0);
	lcd.print(PID_tempControl.Tset);
}

_EXT void printCourrentTemperature(float courentTemp)
{
	lcd.setCursor(7,0); 
	lcd.print("Tc:");
	lcd.print(courentTemp);	
}

_EXT void error_print()
{
	if ((T_E_PS_ERROR)EEPROM.read(EEPROM_ERROR_FLAG) == E_PS_ERROR)
	{
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Salvare anter.");
		lcd.setCursor(0,1);
		lcd.print("esuata!");
		delay(1000);
		EEPROM.write(EEPROM_ERROR_FLAG, 1);		//delete error flag (set to Success)
	}
}


_EXT TempControl get_PID_tempControl()
{
	return PID_tempControl;
}


_EXT LiquidCrystal get_lcd()
{
	return lcd;
}