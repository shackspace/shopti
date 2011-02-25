/* ---------------------------------------------------------------------

   Sample code for driving ST7036 on ELECTRONIC ASSEMBLY's DOG-Series
   (tested on ATmega8, EA DOGM163, AVR-GCC)
   *** NO FREE SUPPORT ON THIS PIECE OF CODE ***
   if you need an offer: mailto: programmer@demmel-m.de
   
   --------------------------------------------------------------------- */

//Serieller Betrieb von EA DOGModulen mit dem ST7036

#define ST7036_SI		As_(PORTD, 5)
#define ST7036_CLK	As_(PORTC, 4)
#define ST7036_CSB	As_(PORTD, 2)
#define ST7036_RS		As_(PORTD, 3)
#define ST7036_RESET	As_(PORTB, 1)

/* ein Byte in das ???-Register des ST7036 senden */
void ST7036_write_byte( char data )
{
signed char	u8_zahl = 8;
char c_data;
	// Chip-Select auf log.0
	Clear_Bit( ST7036_CSB );
	c_data = data;

	do
	{
		_delay_loop_2(6);
		if ( c_data & 0x80 )		// oberstes Bit von c_data betrachten
			Set_Bit(ST7036_SI);		// und Datenleitung entsprechend setzen
		else
			Clear_Bit(ST7036_SI);

		_delay_loop_2(5);			// einen Clockpuls erzeugen
		Clear_Bit(ST7036_CLK);
		_delay_loop_2(6);
		Set_Bit(ST7036_CLK);

		c_data = c_data << 1;
		u8_zahl --;

	} while (u8_zahl > 0);

	// Chip-Select wieder auf log.1
	warte_ms( 2 );
	Set_Bit( ST7036_CSB );
}

/* ein Byte in das Control-Register des KS0073 senden */
void ST7036_write_command_byte( char data )
{
	Clear_Bit( ST7036_RS );
	_delay_loop_2( 1 );
	ST7036_write_byte( data );
}

/* ein Byte in das Daten-Register des KS0073 senden */
void ST7036_write_data_byte( char data )
{
	Set_Bit( ST7036_RS );
	_delay_loop_2( 7 );
	ST7036_write_byte( data );
}

/* Reset durchführen */
void ST7036_reset(void)
{
#ifdef ST7036_RESET
	Clear_Bit(lcdReset);	// Hardware-Reset log.0 an den ST7036 anlegen
	warte_ms( 100 );
	Set_Bit(lcdReset);
#endif
}


/* ein Byte in das Daten-Register des KS0073 senden */
void ST7036_init(void)
{

Set_Bit(ST7036_CLK);
Set_Bit(ST7036_CSB);

//ST7036_reset();


	warte_ms(50);		// mehr als 40ms warten

//	ST7036_write_command_byte( 0x38 );	// Function set; 8 bit Datenlänge, 2 Zeilen

	warte_us(50);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x39 );	// Function set; 8 bit Datenlänge, 2 Zeilen, Instruction table 1
	warte_us(50);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x1d );	// Bias Set; BS 1/5; 3 zeiliges Display /1d
	warte_us(50);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x7c );	// Kontrast C3, C2, C1 setzen /7c
	warte_us(50);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x50 );	// Booster aus; Kontrast C5, C4 setzen /50
	warte_us(50);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x6c );	// Spannungsfolger und Verstärkung setzen /6c
	warte_ms( 500 );	// mehr als 200ms warten !!!

	ST7036_write_command_byte( 0x0f );	// Display EIN, Cursor EIN, Cursor BLINKEN /0f
	warte_us(50);		// mehr als 26,3µs warten

	ST7036_write_command_byte( 0x01 );	// Display löschen, Cursor Home
	warte_ms(400);		//

	ST7036_write_command_byte( 0x06 );	// Cursor Auto-Increment
	warte_us(50);		// mehr als 26,3µs warten

}


void ST7036_putsf( PGM_P string )
{
unsigned char zahl;
zahl = 0;
while (string[zahl] != 0x00)
	{
	ST7036_write_data_byte( string[zahl]);
	zahl++;
	}
}

void ST7036_puts( char * string )
{
unsigned char zahl;
zahl = 0;
while (string[zahl] != 0x00)
	{
	_delay_loop_2(50000);
	ST7036_write_data_byte( string[zahl] );
	string ++;
	}
}

void ST7036_putc( char zeichen )
{
	ST7036_write_data_byte( zeichen );
}

// positioniert den Cursor auf x/y.
// 0/0 ist links oben, 2/15 ist rechts unten
void ST7036_goto_xy(unsigned char xwert, unsigned char ywert)
{
//_delay_loop_2(50000);
ST7036_write_command_byte(0x80 + ywert*0x20 + xwert);
//_delay_loop_2(50000);
Set_Bit(ST7036_RS);
}


void ST7036_clearline( unsigned char zeile )
{
unsigned char zahl;
ST7036_goto_xy( 0, zeile );
for (zahl=1; zahl<20; zahl++) ST7036_write_data_byte( ' ' );
}

void ST7036_clear( void )
{
	ST7036_clearline( 0 );
	ST7036_clearline( 1 );
	ST7036_clearline( 2 );
}
