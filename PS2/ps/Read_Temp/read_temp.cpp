/*
* read_temp.cpp
*
* Created: 12.04.2017 17:52:07
*  Author: Denis
*/

#include "read_temp.h"
#include "../General_Defines.h"
#include <Arduino.h>

//float static temperaturaC2 = 0;

_EXT float read_temp_celsius()
{
	int val;
	float suma=0;
	float static temperaturaC2 = 0;
	float vectortemperatura[510];
	
	int i;
	
	for ( i = 1 ; i <= 500; i++)
	{
		
	val = analogRead(LM35_PIN);
	float mv = ( val/1024.0)*5000;
	float temperatureC = mv/10;
	suma = suma + temperatureC;
	vectortemperatura[i] = temperatureC;
	}
	temperaturaC2 = suma / 500;
	
	int j;
	float aux;
	
	
	//Serial.print("TEMPRATURE = ");
	//Serial.print(temperaturaC2);
	
	return temperaturaC2;
}