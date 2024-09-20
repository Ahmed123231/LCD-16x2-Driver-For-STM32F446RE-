/*
 * LCD_prog.c
 *
 *  Created on: Sep 17, 2024
 *      Author: ahmed
 */


#include <stdint.h>
#include "GPIO_interface.h"
#include "RCC_interface.h"
#include "SysTick.h"
#include "LCD_Config.h"
#include "BIT_MATH.h"



PinConfig_t LCD_Pins[7];



PinConfig_t LCD_Pins[7]=
		{
				{LCD_CPORT,LCD_RS_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP},
				{LCD_CPORT,LCD_RW_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP},
				{LCD_CPORT,LCD_EN_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP},
				{LCD_DPORT,LCD_D4_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP},
				{LCD_DPORT,LCD_D5_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP},
				{LCD_DPORT,LCD_D6_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP},
				{LCD_DPORT,LCD_D7_PIN,OUTPUT,LOW,PUSH_PULL,PULLUP}

		};


void LCD_PinConfig(void){

		RCC_AHB1_EnableClk(LCD_RCC_DPORT);
		RCC_AHB1_EnableClk(LCD_RCC_CPORT);
	    uint8_t Local_iterator =0;
	    for(Local_iterator=0 ;Local_iterator <7 ;Local_iterator++)
	    {

	    	GPIO_u8PinInit(&LCD_Pins[Local_iterator]);


	    }



}





/************* Function to send the lower nibble of Data ****/
void LCD_voidSendLowerNibble(uint8_t copy_u8data) {

	/***** Array to Store Data Pins ****/
	    uint32_t array_pin[4] = {
	        LCD_D4_PIN,
	        LCD_D5_PIN,
	        LCD_D6_PIN,
	        LCD_D7_PIN
	    };

	    uint8_t Loc_u8_iterator = 0;
	    for (Loc_u8_iterator = 0; Loc_u8_iterator < 4; Loc_u8_iterator++) {
	        // Since PIN_HIGH is 0 and PIN_LOW is 1, we invert the logic
	    	PinVal_t pinValue = (GET_BIT(copy_u8data, Loc_u8_iterator)) ? PIN_HIGH : PIN_LOW;

	        //HAL_GPIO_WritePin(LCD_DPORT,array_pin[Loc_u8_iterator],pinValue);
	        GPIO_u8SetPinValue(LCD_DPORT,array_pin[Loc_u8_iterator],pinValue);
	    }
}

/****** Function to Set Enable Pin ************/
void LCD_voidSetEnable(void) {
    /******** SET EN PIN HIGH *********/
	//HAL_GPIO_WritePin(LCD_CPORT, LCD_EN_PIN, GPIO_PIN_SET);
	GPIO_u8SetPinValue(LCD_CPORT, LCD_EN_PIN,PIN_HIGH);
    //HAL_Delay(2);
    SysTick_delay_ms(2);
    /******** SET EN PIN LOW *********/
    //HAL_GPIO_WritePin(LCD_CPORT, LCD_EN_PIN, GPIO_PIN_RESET);
    GPIO_u8SetPinValue(LCD_CPORT, LCD_EN_PIN,PIN_LOW);
}

/************* LCD Command Send Function ************/
void LCD_voidSendCommand(uint8_t copy_u8command) {
    /* Clear RS PIN to Send Command */
	//HAL_GPIO_WritePin(LCD_CPORT, LCD_RS_PIN,GPIO_PIN_RESET);
	GPIO_u8SetPinValue(LCD_CPORT, LCD_RS_PIN,PIN_LOW);
    /* Clear RW PIN to Send Data */
	//HAL_GPIO_WritePin(LCD_CPORT, LCD_RW_PIN,GPIO_PIN_RESET);
	GPIO_u8SetPinValue(LCD_CPORT,LCD_RW_PIN,PIN_LOW);


    LCD_voidSendLowerNibble(copy_u8command >> 4);
    LCD_voidSetEnable();
    LCD_voidSendLowerNibble(copy_u8command);
    LCD_voidSetEnable();

}

/************* LCD Data Send Function ************/
void LCD_voidSendData(uint8_t copy_u8data) {
    /* SET RS PIN to Send Data */
	//HAL_GPIO_WritePin(LCD_CPORT, LCD_RS_PIN, GPIO_PIN_SET);
	GPIO_u8SetPinValue(LCD_CPORT, LCD_RS_PIN,PIN_HIGH);

    /* Clear RW PIN to Send Data */
	//HAL_GPIO_WritePin(LCD_CPORT, LCD_RW_PIN, GPIO_PIN_RESET);
	GPIO_u8SetPinValue(LCD_CPORT,LCD_RW_PIN,PIN_LOW);
    /*** Send Data ***/

    LCD_voidSendLowerNibble(copy_u8data >> 4);
    LCD_voidSetEnable();
    LCD_voidSendLowerNibble(copy_u8data);
    LCD_voidSetEnable();

}

/************* LCD Initialization Function 8-bit Mode ************/
void LCD_voidInit(void) {
	//HAL_Delay(40);
	SysTick_delay_ms(150);
    /* Send Function Set */

    LCD_voidSendLowerNibble(0b0010);
    LCD_voidSetEnable();
    LCD_voidSendLowerNibble(0b0010);
    LCD_voidSetEnable();
    LCD_voidSendLowerNibble(0b1000);
    LCD_voidSetEnable();

    /* Display on/off control */
    LCD_voidSendCommand(0b00001100);
    /* Display Clear */
    LCD_voidSendCommand(1);
    /* Entry Mode Set */
}

/************* LCD String Display Function ************/
void LCD_voidSendString(uint8_t *copy_u8data) {
	uint8_t i = 0;
    while (copy_u8data[i] != '\0') {
        LCD_voidSendData(copy_u8data[i]);
        copy_u8data++;
    }
}

/********** Function to Set Grid position on LCD ****/
void LCD_SetGridPos(uint8_t copy_u8column,uint8_t copy_u8row) {
	    uint8_t address = 0x0;
	    if (copy_u8row == 0) {
	        address = 0x80;
	    } else if (copy_u8row == 1) {
	        address = 0xC0;
	    }
	    if (copy_u8column < 16) {
	        address += copy_u8column;
	    }
	    LCD_voidSendCommand(address);
}

/************* LCD Number Display Function ************/
void LCD_voidSendNum(float copy_f32data) {
    uint8_t buffer[16];  // Buffer to hold the string representation of the number
    uint8_t i = 0;

    // Check if the number is negative
    if (copy_f32data < 0) {
        LCD_voidSendData('-');  // Send negative sign to LCD
        copy_f32data = -copy_f32data;  // Make the number positive
    }

    // Check if the number is a floating-point number or integer
    if (copy_f32data == (int)copy_f32data) {
        // If the number is an integer, print as an integer
        uint16_t int_part = (uint16_t)copy_f32data;
        if (int_part == 0) {
            buffer[i++] = '0';
        } else {
            // Convert integer part to string manually (reverse the digits)
            while (int_part != 0) {
                uint8_t digit = int_part % 10;  // Extract the least significant digit
                buffer[i++] = digit + '0';  // Convert digit to ASCII and store in buffer
                int_part /= 10;  // Move to the next digit
            }
        }
        // Send digits to LCD in reverse order
        while (i > 0) {
            LCD_voidSendData(buffer[--i]);
        }
    } else {
        // If it's a float, print as a float with six decimal places
        uint16_t int_part = (uint16_t)copy_f32data;  // Extract integer part
        float fractional_part = copy_f32data - int_part;  // Extract fractional part

        // Convert integer part manually as above
        if (int_part == 0) {
            buffer[i++] = '0';
        } else {
            while (int_part != 0) {
                uint8_t digit = int_part % 10;
                buffer[i++] = digit + '0';
                int_part /= 10;
            }
        }
        while (i > 0) {
            LCD_voidSendData(buffer[--i]);
        }

        // Print the decimal point
        LCD_voidSendData('.');

        // Convert fractional part to 6 decimal places (multiply by 1,000,000 and cast to integer)
        fractional_part *= 1000000;
        uint32_t fraction_as_int = (uint32_t)(fractional_part + 0.5);  // Round to nearest integer

        // Convert fractional part to string manually (6 digits)
        i = 0;
        buffer[i++] = (fraction_as_int / 100000) % 10 + '0';  // Hundred-thousands place
        buffer[i++] = (fraction_as_int / 10000) % 10 + '0';   // Ten-thousands place
        buffer[i++] = (fraction_as_int / 1000) % 10 + '0';    // Thousands place
        buffer[i++] = (fraction_as_int / 100) % 10 + '0';     // Hundreds place
        buffer[i++] = (fraction_as_int / 10) % 10 + '0';      // Tens place
        buffer[i++] = (fraction_as_int % 10) + '0';           // Ones place

        // Send fractional digits to LCD
        for (uint8_t j = 0; j < i; j++) {
            LCD_voidSendData(buffer[j]);
        }
    }
}
