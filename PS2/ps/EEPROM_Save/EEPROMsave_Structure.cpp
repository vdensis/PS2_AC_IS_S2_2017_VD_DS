/*
 * EEPROMsave_Structure.cpp
 *
 * Created: 29.03.2017 20:16:34
 *  Author: Denis
 */ 

#include <EEPROM.h>
#include <Arduino.h>  // for type definitions
#include "EEPROMsave_Structure.h"


_EXT void eepromRead(T_UWORD addr, TempControl *output, T_UWORD length)
{
	T_UBYTE* src;
	T_UBYTE* dst;
	src = (T_UBYTE*)addr;
	dst = (T_UBYTE*)output;
	for (T_UWORD i = 0; i < length; i++) {
		*dst++ = eeprom_read_byte(src++);
	}
}

_EXT void eepromWrite(T_UWORD addr, TempControl *input, T_UWORD length)
{
	T_UBYTE* src;
	T_UBYTE* dst;
	src = (T_UBYTE*)input;
	dst = (T_UBYTE*)addr;
	for (T_UWORD i = 0; i < length; i++) {
		eeprom_write_byte(dst++, *src++);
	}
}



//EEPROM memory:	----------------------------------------------------------------------------
//					|____Kp____|_____Ki____|_____Kd_____|TSet|__Tcurenta__|_Tinc_|_Tmem_|_Trac_|
//					|1  2  3  4| 5  6  7  8| 9  10 11 12| 13 |14 15 16 17 |18 19 | 20 21| 22 23|
//					----------------------------------------------------------------------------
