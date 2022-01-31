/*
 *	LCD interface example
 *	Uses routines from delay.c
 *	This code will interface to a standard LCD controller
 *	like the Hitachi HD44780. It uses it in 4 bit mode, with
 *	the hardware connected as follows (the standard 14 pin 
 *	LCD connector is used):
 *	
 *	PORTD bits 0-3 are connected to the LCD data bits 4-7 (high nibble)
 *	PORTC bit 1 is connected to LCD RS input (register select)
 *  RW input (read/write) connected to ground
 *	PORTC bit 0 is connected to the LCD EN bit (enable)
 *	
 *	
 */
#include <avr/io.h>
#include <util/delay.h>


void lcd_write(unsigned char c);

/*Limpiar y regresar a casa el LCD*/

void lcd_clear(void);

/* Escribir una cadena de caracteres en la LCD */

void lcd_puts(const char * s);

/* Escribir un caracter en la LCD */

void lcd_putch(char c);


/*
 * Ir a posición específica
 */

void lcd_goto(unsigned char pos);
	
/* inicializar LCD - establecer en 4 bits */

void lcd_init();

#define LCD_EN	PD0
#define LCD_RS	PD1
#define LCD_DATA	PORTC


void LCD_STROBE(){
	
	PORTD |= (1<<LCD_EN);
	PORTD &= ~(1<<LCD_EN);
}
/*Escribir un byte en el LCD en modo 4 bits*/

void lcd_write(unsigned char c){
	
	_delay_ms(1);
	LCD_DATA = ((c >> 4) & 0x0F);
	LCD_STROBE();
	LCD_DATA = (c & 0x0F);
	LCD_STROBE();	
}

/*Limpiar y regresar a casa el LCD*/

void lcd_clear(void){
	
	PORTD &= ~(1<<LCD_RS);
	lcd_write(0x01);
	_delay_ms(2);
}

/* Escribir una cadena de caracteres en la LCD */

void lcd_puts(const char * s)
{
	PORTD |= (1<<LCD_RS);	// Escribir datos
	while(*s)
		lcd_write(*s++);
}

/* Escribir un caracter en la LCD */

void lcd_putch(char c)
{
	PORTD |= (1<<LCD_RS);	// Escribir datos
	lcd_write( c );
}


/*
 * Ir a posición específica
 */

void lcd_goto(unsigned char pos)
{
	PORTD &= ~(1<<LCD_RS);	//Escribir dirección
	lcd_write(0x80+pos);
}
	
/* inicializar LCD - establecer en 4 bits */

void lcd_init() {
	
	char init_value;
	init_value = 0x3;
	
	DDRD = 0xFF;
	DDRC = 0xFF;
	PORTD &= ~(1<<LCD_RS);
	PORTD &= ~(1<<LCD_EN);
	
	_delay_ms(15);				// Esperar 15ms despues de energizar
	LCD_DATA	 = init_value;
	LCD_STROBE();
	_delay_ms(10);
	LCD_STROBE();
	_delay_ms(10);
	LCD_STROBE();
	_delay_ms(10);
	LCD_DATA = 2;	// Four bit mode
	LCD_STROBE();
	
	lcd_write(0x28); // Set interface length
//	lcd_write(0xC); // Display On, Cursor Off, Blink Off
	lcd_write(0xF); // Display On, Cursor On, Cursor Blink
	lcd_clear();	// Clear screen
	lcd_write(0x6); // Set entry Mode
}