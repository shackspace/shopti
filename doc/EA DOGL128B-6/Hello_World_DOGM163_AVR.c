//**********************************************************************
//
//  Sample Code for driving an ST7036 on the display series EA-DOGM 163
//	in 4-bit mode with a supply voltage of 5V
//
//  2010-06-18
//
//  Written by 
//	Dave the Brave (David [dot] Felkel [at] gmx [dot] de)
//
//	Tested with: AVR-GCC, ATTINY2313, EA DOGM163 S-A
//
//**********************************************************************

#define F_CPU 4000000

#define LCD_PORT	PORTB			//datapins are PB0 to PB4 (DB4 to DB7 on the DOG)
#define PIN_ENABLE 	PB6
#define PIN_RS 		PB7

#define CHARACTER_BUFFER_BASE_ADDRESS		0b10000000
#define CHARACTERS_PER_ROW 							16	

//instructions (see the ST7036 instruction set for further information)
#define INSTRUCTION_CLEAR_DISPLAY    		0b00000001
#define INSTRUCTION_FUNCTION_SET_INIT_0  	0b00110011
#define INSTRUCTION_FUNCTION_SET_INIT_1  	0b00110010
#define INSTRUCTION_FUNCTION_SET_INIT_2  	0b00101001
#define INSTRUCTION_INSTRUCTION_SET_0    	0b00101000
#define INSTRUCTION_INSTRUCTION_SET_1 		0b00101001
#define INSTRUCTION_BIAS_SET				0b00010101
#define INSTRUCTION_POWER_CONTROL 			0b01010011
#define INSTRUCTION_FOLLOWER_CONTROL		0b01101100
#define INSTRUCTION_CONTRAST_SET			0b01111111
#define INSTRUCTION_DISPLAY_ON				0b00001100
#define INSTRUCTION_ENTRY_MODE 				0b00000110

#include <avr/io.h>
#include <util/delay.h>

//writes 8 bit to the ST7036 in 2 nibbles (2 x 4bit)
void write(char data)
{
	char port_buffer = LCD_PORT;
	
	//write the high nibble first...
	char nibble = (data & 0xF0) >> 4;
	LCD_PORT |= nibble;
	LCD_PORT |= (1 << PIN_ENABLE);
	_delay_ms(1);
	
	LCD_PORT = port_buffer;
	_delay_ms(1);
	
	//...then the low nibble
	nibble = data & 0x0F;
	LCD_PORT |= nibble;
	LCD_PORT |= (1 << PIN_ENABLE);
	_delay_ms(1);
	
	LCD_PORT = port_buffer;
}

void write_instruction(char instruction)
{
	//RS low = instruction
	LCD_PORT &= ~(1 << PIN_RS);
	write(instruction);
}

void write_data(char data)
{
	//RS high = data
	LCD_PORT |= (1 << PIN_RS);
	write(data);
}

void set_cursor(char row, char column)
{
	write_instruction(CHARACTER_BUFFER_BASE_ADDRESS + row * CHARACTERS_PER_ROW + column);
}

void write_string(char* string)
{
	while (*string)
	{
		write_data(*string);
		*string++;
    }
}

void initialize_display()
{
	//wait for a short period, maybe the voltage needs to stabilize first
	_delay_ms(50);
	
	//initialize 4 bit mode
	write_instruction(INSTRUCTION_FUNCTION_SET_INIT_0);
	write_instruction(INSTRUCTION_FUNCTION_SET_INIT_1);
	write_instruction(INSTRUCTION_FUNCTION_SET_INIT_2);
	
	//initialize everything else
	//NOTE: In case you need an other initialization-routine (blinking cursor, 
	//double line height etc.) choose the appropriate instructions from 
	//the ST7036 datasheet and adjust the C-defines at the top of this file
	write_instruction(INSTRUCTION_BIAS_SET);
	write_instruction(INSTRUCTION_POWER_CONTROL);
	write_instruction(INSTRUCTION_FOLLOWER_CONTROL);
	write_instruction(INSTRUCTION_CONTRAST_SET);
	write_instruction(INSTRUCTION_INSTRUCTION_SET_0);
	write_instruction(INSTRUCTION_DISPLAY_ON);
	write_instruction(INSTRUCTION_CLEAR_DISPLAY);
	write_instruction(INSTRUCTION_ENTRY_MODE);
}

int main()
{
	//set LCD_PORT to OUTPUT and then ...
	DDRB = 0xFF;	

	//.. do fancy stuff with the lcd
	initialize_display();
		
	set_cursor(0, 0);
	write_string("Hello World!");
	
	set_cursor(1,0);
	write_string("Code written by:");
	
	set_cursor(2,0);
	write_string("Dave the Brave");
		
	while(1)
	{
		asm volatile ("nop");	//do nothing (in a very explicit way)
	}

	return 0;
}

