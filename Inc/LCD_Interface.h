/*
 * LCD_Interface.h
 *
 *  Created on: Sep 17, 2024
 *      Author: Ahmed Atef
 */

#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_



void LCD_PinConfig(void);
void LCD_voidSendLowerNibble(uint8_t copy_u8data);
void LCD_voidSetEnable(void) ;
void LCD_voidSendCommand(uint8_t copy_u8command);
void LCD_voidSendData(uint8_t copy_u8data) ;
void LCD_voidInit(void);
void LCD_voidSendString(uint8_t *copy_u8data);
void LCD_SetGridPos(uint8_t copy_u8column,uint8_t copy_u8row);
void LCD_voidSendNum(float copy_f32data);




#endif /* LCD_INTERFACE_H_ */
