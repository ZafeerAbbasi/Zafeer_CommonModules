/*
# ##############################################################################
# File: adc.c                                                                  #
# Project: src                                                                 #
# Created Date: Saturday, October 28th 2023, 11:24:39 pm                       #
# Author: Zafeer Abbasi                                                        #
# ----------------------------------------------                               #
# Last Modified: Sunday, October 29th 2023, 12:48:25 pm                        #
# Modified By: Zafeer Abbasi                                                   #
# ----------------------------------------------                               #
# Copyright (c) 2023 Zafeer.A                                                  #
# ----------------------------------------------                               #
# HISTORY:                                                                     #
 */

/*##############################################################################################################################################*/
/*INCLUDES______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

#include "main.h"
#include "adc.h"

/*##############################################################################################################################################*/
/*FUNCTION DECLARATIONS_________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*GLOBALS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

ADC_HandleTypeDef hadc;

/*##############################################################################################################################################*/
/*DEFINES_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*EXTERNS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*TYPEDEFS/STRUCTS/ENUMS________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*FUNCTIONS_____________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

void ADC_Init( void )
{
    ADC_ChannelConfTypeDef sConfig = {0};

    // Configure ADC peripheral
    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; // Assuming you want to divide PCLK by 2
    hadc.Init.ScanConvMode = DISABLE; // Non-scan mode
    hadc.Init.ContinuousConvMode = DISABLE; // Continuous conversion disabled
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START; // Software trigger
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.NbrOfConversion = 1; // Only one conversion
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        // Initialization error handling
        Error_Handler();
    }

    // Configure for the ADC regular channel to be converted.
    sConfig.Channel = ADC_CHANNEL_0; // PA0
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; // Choose a proper sampling time
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        // Configuration error handling
        Error_Handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    __HAL_RCC_ADC1_CLK_ENABLE( );
    __HAL_RCC_GPIOA_CLK_ENABLE( );

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
    
}

/**
 * @brief Get the Joystick X, Y Value in a typedef joyStickVal_t
 * 
 * @return joyStickVal_t Struct containing X, Y Values
 */
joyStickVal_t getXYJoyStickVal( void )
{
    ADC_ChannelConfTypeDef sConfig = { 0 };
    uint32_t x_value = 0, y_value = 0;
    
    sConfig.Channel = ADC_CHANNEL_0; // PA0
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; // Choose a proper sampling time
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    // Read X-axis
    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    x_value = HAL_ADC_GetValue(&hadc);

    sConfig.Channel = ADC_CHANNEL_1; // PA0
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES; // Choose a proper sampling time
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    // Read Y-axis
    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
    y_value = HAL_ADC_GetValue(&hadc);

    return convertADCtoXY( x_value, y_value );

}

joyStickVal_t convertADCtoXY(uint16_t adcValueX, uint16_t adcValueY) 
{
    joyStickVal_t coordinate;

    // Convert X ADC reading to -1, 0, or 1
    if (adcValueX < ADC_THRESHOLD_LOW) {
        coordinate.x = -1;
    } else if (adcValueX > ADC_THRESHOLD_HIGH) {
        coordinate.x = 1;
    } else {
        coordinate.x = 0;
    }

    // Convert Y ADC reading to -1, 0, or 1
    if (adcValueY < ADC_THRESHOLD_LOW) {
        coordinate.y = 1;
    } else if (adcValueY > ADC_THRESHOLD_HIGH) {
        coordinate.y = -1;
    } else {
        coordinate.y = 0;
    }

    return coordinate;
}