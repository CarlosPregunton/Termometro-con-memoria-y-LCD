/* 
		R3 -> PA7, R2 -> PA6, R1 -> PA5, R0 -> PA4 		Renglones conectados a los 4 bits mas significativos del pueto A
				   C2 -> PA2, C1 -> PA1, C0 -> PA0,    Columnas conectadas a los 3 bits menos significativos del puerto A
*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <mylib/lcd_c.h>
#include <mylib/eeprom_interna.h>

#define TECLA_PRT 	PORTB
#define TECLA_DDR		DDRB 
#define TECLA_PIN		PINB

unsigned char teclado[4][4] = {
	{1,2,3,'/'},
	{4,5,6,'*'},
	{7,8,9,'-'},
	{'#',0,'*','+'}};

	unsigned char col_pos, ren_pos;

	char buscar_tecla()
	{
			_delay_ms(20);
			TECLA_DDR = 0xF0;           /* PORTB[7:4] como salida y PORTB[7:4] como entrada */
			TECLA_PRT = 0xFF;

			/* Revisar renglones*/
			TECLA_PRT = 0xEF;            /* revisar si se ha presionado una tecla en el primer renglón */
			asm("NOP");
			col_pos = (TECLA_PIN & 0x0F);
			if(col_pos != 0x0F)
			{
				ren_pos = 0;
				goto tabla;
			}

			TECLA_PRT = 0xDF;		/* revisar si se ha presionado una tecla en el segundo renglón */
			asm("NOP");
			col_pos = (TECLA_PIN & 0x0F);
			if(col_pos != 0x0F)
			{
				ren_pos = 1;
				goto tabla;
			}
				
			TECLA_PRT = 0xBF;		/* revisar si se ha presionado una tecla en el tercer renglón */
			asm("NOP");
			col_pos = (TECLA_PIN & 0x0F);
			if(col_pos != 0x0F)
			{
				ren_pos = 2;
				goto tabla;
			}

			TECLA_PRT = 0x7F;		/* revisar si se ha presionado una tecla en el cuarto renglón */
			asm("NOP");
			col_pos = (TECLA_PIN & 0x0F);
			if(col_pos != 0x0F)
			{
				ren_pos = 3;
				goto tabla;
			}
			
						col_pos = (TECLA_PIN & 0x0F);
						if(col_pos == 0x0F)
						{
							return 'z';
						}
			
		   tabla:
			if(col_pos == 0x0E)
			return(teclado[ren_pos][0]);
			else if(col_pos == 0x0D)
			return(teclado[ren_pos][1]);
			else if(col_pos == 0x0B)
			return(teclado[ren_pos][2]);
			else {
			_delay_ms(20);
			return 'z';	
			}
		}
		
		unsigned int direccion_eeprom = 0;
		char imp_direccion_eeprom[5];
		int main(void)
		{
			/********Configuración de E/S Digitales **************/
			lcd_init();
			DDRA = 0xFE;	//PA0 como entrada analógica
			
			sei();													//abilitar el bit de interrupciones globales
			
			/***********Configuración del ADC *********************/
			ADCSRA |= (1<<ADEN)|(0<<ADIE)|(0<<ADATE);				//Habilitar ADC, AutoTriger e sin interrupción por ADC
			ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);				//CLK_ADC = 8MHz/16Hz = 0.5MHz
			ADMUX |= (1<<REFS1)|(1<<REFS0)|(1<<ADLAR);				//Referencia interna, entrada ADC0 y just a la Izquierda
			
			/**********Configuración del temporizador*************/
			
			TIMSK |= (1<<TOIE1);					//Habilitar la interrupción por desbordamiento del timer 1
			TCCR1A &= (~(1<<WGM10))&(~(1<<WGM11));	//Modo simple
			TCCR1B &= (~(1<<WGM12))&(~(1<<WGM13));
			TCCR1B |= (1<<CS10)|(1<<CS12);			//Prescaler = 1024
			TCCR1B &= (~(1<<CS11));
			TCNT1 = 57722;															//Habilitar interrupciones
		
			unsigned char k = 5;
			unsigned char num_alm[3] = {0,0,0};
			char imp_num[6];
			unsigned int num = 0;
			char imp_adc[5];
			char imp_eeprom[5];
			unsigned char eeprom_val;
			int adc_val;
			
			while(1)
			{
				k = buscar_tecla();
				
				if((k != 'z') & (k != '*')){
				
				num_alm[0] = num_alm[1];
				num_alm[1] = num_alm[2];
				num_alm[2] = k;
				num = num_alm[0]*100 + num_alm[1]*10 + num_alm[2];
				sprintf(imp_num,"%i",num);
				}
				
				if(k == '#'){
					
				num_alm[0] = 0;
				num_alm[1] = 0;
				num_alm[2] = 0;
				num = num_alm[0]*100 + num_alm[1]*10 + num_alm[2];
				sprintf(imp_num,"%i",num);
				lcd_clear();
				}
				
				if(k == '*'){
					
				eeprom_val = EEPROM_leer(num);
				sprintf(imp_eeprom,"%i",eeprom_val);
					
				}
				
				ADCSRA |= (1<<ADSC);								//Iniciar la conversión
				while(!(ADCSRA & (1<<ADIF)));
				adc_val = 1.003921569*ADCH;	
				sprintf(imp_adc,"%i",adc_val);
				
				lcd_clear();
				lcd_goto(0x00);
				lcd_puts("Dir:");
				lcd_puts(imp_num);
				
				lcd_goto(0x08);
				lcd_puts("T=");
				lcd_puts(imp_adc);
				
				lcd_goto(0x40);
				lcd_puts("Alm:");
				lcd_puts(imp_eeprom);
				
				sprintf(imp_direccion_eeprom,"%i",direccion_eeprom);
				lcd_goto(0x48);
				lcd_puts("t:");
				lcd_puts(imp_direccion_eeprom);
				_delay_ms(100);
			}
		}
		
		ISR(TIMER1_OVF_vect){
			
		//	if(cont_segundos == 60){
				
				TCNT1 = 57722;
				EEPROM_escribir(direccion_eeprom,ADCH);
				direccion_eeprom++;
				if(direccion_eeprom > 500)
				direccion_eeprom = 0;
				
		//		cont_segundos = 0;
		//	}
		//	cont_segundos++;		
		}

		/*Calculo de la temporización
		1MHz/1024 = 976.5625Hz
		P_tempotizador = 1/976.5625Hz = 1.024ms
		
		Para ocurrir un desbordamiento cada 1s se debe contar hasta
		1s/1.024ms = 977 conteos
		Se debe iniciar el conteo en:
		65535 - 977 = 64558
		
		Para ocurrir un desbordamiento cada 60s se debe contar hasta
		60s/1.024ms = 58,594 conteos	
		Se debe iniciar el conteo en:
		65535 - 58594 = 6941
		*/

		


