#include "lcd_txt.h"



/*--------------- Initialize LCD ------------------*/
void LCD_Init(void)
{
	
	HAL_Delay(30);
	
	PIN_LOW(D4_PORT,D4_PIN);
	PIN_HIGH(D5_PORT,D5_PIN);
	PIN_LOW(D6_PORT,D6_PIN);
	PIN_LOW(D7_PORT,D7_PIN);
	PIN_LOW(RS_PORT,RS_PIN);
	
	PIN_HIGH(EN_PORT,EN_PIN);
	PIN_LOW(EN_PORT,EN_PIN);
	
	LCD_Write(0,0x28);
	LCD_Write(0,0x0c);
	LCD_Write(0,0x06);
	LCD_Write(0,0x01);
}

/*--------------- Write To LCD ---------------*/
void LCD_Write(uint8_t type,uint8_t data)
{
	HAL_Delay(2);
	if(type)
	{
		PIN_HIGH(RS_PORT,RS_PIN);
	}else
	{
		PIN_LOW(RS_PORT,RS_PIN);
	}
	
	//Send High Nibble
	if(data&0x80)
	{
		PIN_HIGH(D7_PORT,D7_PIN);
	}else
	{
		PIN_LOW(D7_PORT,D7_PIN);
	}
	
	if(data&0x40)
	{
		PIN_HIGH(D6_PORT,D6_PIN);
	}else
	{
		PIN_LOW(D6_PORT,D6_PIN);
	}
	
	if(data&0x20)
	{
		PIN_HIGH(D5_PORT,D5_PIN);
	}else
	{
		PIN_LOW(D5_PORT,D5_PIN);
	}
	
	if(data&0x10)
	{
		PIN_HIGH(D4_PORT,D4_PIN);
	}else
	{
		PIN_LOW(D4_PORT,D4_PIN);
	}
	PIN_HIGH(EN_PORT,EN_PIN);
	PIN_LOW(EN_PORT,EN_PIN);
	

	//Send Low Nibble
	if(data&0x08)
	{
		PIN_HIGH(D7_PORT,D7_PIN);
	}else
	{
		PIN_LOW(D7_PORT,D7_PIN);
	}
	
	if(data&0x04)
	{
		PIN_HIGH(D6_PORT,D6_PIN);
	}else
	{
		PIN_LOW(D6_PORT,D6_PIN);
	}
	
	if(data&0x02)
	{
		PIN_HIGH(D5_PORT,D5_PIN);
	}else
	{
		PIN_LOW(D5_PORT,D5_PIN);
	}
	
	if(data&0x01)
	{
		PIN_HIGH(D4_PORT,D4_PIN);
	}else
	{
		PIN_LOW(D4_PORT,D4_PIN);
	}
	PIN_HIGH(EN_PORT,EN_PIN);
	PIN_LOW(EN_PORT,EN_PIN);
}

void LCD_Puts(uint8_t x, uint8_t y, int8_t *string)
{
	//Set Cursor Position
	#ifdef LCD16xN	//For LCD16x2 or LCD16x4
	switch(x)
	{
		case 0: //Row 0
			LCD_Write(0,0x80+0x00+y);
			break;
		case 1: //Row 1
			LCD_Write(0,0x80+0x40+y);
			break;
		case 2: //Row 2
			LCD_Write(0,0x80+0x10+y);
			break;
		case 3: //Row 3
			LCD_Write(0,0x80+0x50+y);
			break;
	}
	#endif
	
	#ifdef LCD20xN	//For LCD20x4
	switch(x)
	{
		case 0: //Row 0
			LCD_Write(0,0x80+0x00+y);
			break;
		case 1: //Row 1
			LCD_Write(0,0x80+0x40+y);
			break;
		case 2: //Row 2
			LCD_Write(0,0x80+0x14+y);
			break;
		case 3: //Row 3
			LCD_Write(0,0x80+0x54+y);
			break;
	}
	#endif
	
	while(*string)
	{
		LCD_Write(1,*string);
		string++;
	}
}

void LCD_Clear_RC(uint8_t row, uint8_t col, uint8_t length)
{
    // Create a string of spaces with the specified length
    char spaces[length + 1];
    for (uint8_t i = 0; i < length; i++)
    {
        spaces[i] = ' ';
    }
    spaces[length] = '\0'; // Null-terminate the string

    // Use lcd_puts to write the spaces to the specified position
    LCD_Puts(row, col, (int8_t *)spaces);
}

void LCD_Clear(void)
{
	LCD_Write(0,0x01);
}




