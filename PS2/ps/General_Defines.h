/*
* General_Defines.cpp
*
* Created: 29.03.2017 18:54:26
*  Author: Denis
*/


#define PWM_PIN 9


//tag for extern functions
#define _EXT

//tag for local functions
#define _LOC

//define 1 byte type
#define T_UBYTE unsigned char

//define 2 bytes type
#define T_UWORD unsigned short int

//Define structure for Temp Control
typedef struct TempControl
{
	double Kp;
	double Ki;
	double Kd;
	T_UBYTE Tset;
	double Tcurenta;
	T_UWORD Tinc;
	T_UWORD Tmem;
	T_UWORD Trac;
};

//TempControl PID_tempControl;

typedef enum T_E_PS_ERROR
{
	E_PS_ERROR,
	E_PS_SUCCESS
};