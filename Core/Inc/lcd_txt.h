#ifndef		__LCDTXT_H
#define		__LCDTXT_H

#include "stm32f1xx_hal.h"


/*------------- Define LCD Use -----------------*/

/*Note: Comment which not use */

#define LCD16xN //For lcd16x2 or lcd16x4
//#define LCD20xN //For lcd20x4

/*------------- Define For Connection -----------------*/

#define RS_PORT		GPIOA
#define RS_PIN		GPIO_PIN_9

#define EN_PORT		GPIOA
#define EN_PIN		GPIO_PIN_8

#define D7_PORT		GPIOB
#define D7_PIN		GPIO_PIN_12

#define D6_PORT		GPIOB
#define D6_PIN		GPIO_PIN_13

#define D5_PORT		GPIOB
#define D5_PIN		GPIO_PIN_14

#define D4_PORT		GPIOB
#define D4_PIN		GPIO_PIN_15


/*------------ Declaring Private Macro -----------------*/

#define PIN_LOW(PORT,PIN)	HAL_GPIO_WritePin(PORT,PIN,GPIO_PIN_RESET);
#define PIN_HIGH(PORT,PIN)	HAL_GPIO_WritePin(PORT,PIN,GPIO_PIN_SET);

/*------------ Declaring Function Prototype -------------*/
void LCD_Init(void);
void LCD_Write(uint8_t type,uint8_t data);
void LCD_Puts(uint8_t x, uint8_t y, int8_t *string);
void LCD_Clear_RC(uint8_t row, uint8_t col, uint8_t length);
void LCD_Clear(void);
#endif

