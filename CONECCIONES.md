# Termometro-con-memoria-y-LCD
Código para implementar un termómetro que registre la temperatura medida en la memoria eeprom del microcontrolador ATMEGA16
La conección del teclado es la siguiente
PORTB[7:4] a las columnas del teclada 
PORTB[7:4] a las entradas del teclado
PD0 al pin al pin EN de la LCD
PD1 al pin RS	de la LCD 
POTC[3:0] a los 4 bits más significativos del PORTC
