/*
 * EEPROMsave_Structure.h
 *
 * Created: 29.03.2017 20:16:49
 *  Author: Denis
 */ 
#include "../General_Defines.h"


#ifndef EEPROMSAVE_STRUCTURE_H_
#define EEPROMSAVE_STRUCTURE_H_

_EXT extern void eepromRead(T_UWORD addr, TempControl *output, T_UWORD length);

_EXT extern void eepromWrite(T_UWORD addr, TempControl *input, T_UWORD length);

#endif /* EEPROMSAVE_STRUCTURE_H_ */