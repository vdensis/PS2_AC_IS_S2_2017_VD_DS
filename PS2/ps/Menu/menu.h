/*
* menu.h
*
* Created: 29.03.2017 18:50:53
*  Author: Denis
*/

#include "../EEPROM_Save/EEPROMsave_Structure.h"
#include <EEPROM.h>

//define max value for analog read buttons value
#define D_MAX_BUTTON_VALUE 850

//Define delay for button press
#define BT_PRESSED_DELAY 300

//Define Address in EEPROM for error flag
#define EEPROM_ERROR_FLAG 255

//Declare Buttons name enumeration
typedef enum T_E_BUTTON
{
	E_BUTTON_NONE,
	E_BUTTON_1,
	E_BUTTON_2,
	E_BUTTON_3,
	E_BUTTON_4
};


//Declare principal LCD screen
typedef enum T_E_SCREEN
{
	E_SCREEN_PRINCIPAL,
	E_SCREEN_MENU
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//									Define local functions											//
//////////////////////////////////////////////////////////////////////////////////////////////////////

_LOC T_E_BUTTON getButton(int analogValue);

_LOC void menuKp();

_LOC void menuKi();

_LOC void menuKd();

_LOC void menuTempSet();

_LOC void menuTempCurent();

_LOC void menuTinc();

_LOC void menuTmem();

_LOC void menuTrac();

_LOC void factoryReset();

//////////////////////////////////////////////////////////////////////////////////////////////////////
//									Define extern functions											//
//////////////////////////////////////////////////////////////////////////////////////////////////////
extern _EXT void init_display();

extern _EXT void LCD_creteChar();

extern _EXT void mainMenuDraw();

extern _EXT void drawCursor();

extern _EXT void operateMainMenu(T_E_SCREEN* screen, T_E_BUTTON* activeButton_menuStart);

extern _EXT T_E_BUTTON readMenuButton();

extern _EXT void draw_principalScreen(T_E_BUTTON *activeButton_menuStart, T_UBYTE *startFlag);

extern _EXT void error_print();

extern _EXT void printCourrentTemperature(float courentTemp);

extern _EXT TempControl get_PID_tempControl();

extern _EXT LiquidCrystal get_lcd();

//////////////////////////////////////////////////////////////////////////////////////////////////////
