/*
 * C_P14_EEPROM_INTERNA.c
 *
 * Created: 08/03/2021 22:42:49
 *  Author: pc
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void EEPROM_escribir(unsigned int Direccion, unsigned char Dato){
	/*Esperar termino de escritura anterior*/
	while(EECR & (1<<EEWE));

	/*Establecer dato y direccion*/
	EEAR = Direccion;
	EEDR = Dato;

	/*Escribir uno a EEMWE*/
	EECR |= (1<<EEMWE);

	/*Empezar la escritura de la eeprom*/
	EECR |= (1<<EEWE);	
}

unsigned char EEPROM_leer(unsigned int Direccion){
	/*Esperar termino de escritura anterior*/
	while(EECR & (1<<EEWE));
	
	/*Establecer dato y direccion*/
	EEAR = Direccion;
	
	/*Empezar la lectura de la eeprom*/
	EECR |= (1<<EERE);
	
	/*Retornar dato del registro de datos */
	return EEDR;
}