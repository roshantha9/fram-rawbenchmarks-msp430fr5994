/*
 * main.c
 *
 *  Created on: Nov 27, 2018
 *      Author: Rosh
 *
 *  Notes:
 *  - FRAM vs. SRAM speed/energy test
 *
 */



#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "driverlib.h"


// general utilities



/*******************************************************
 * Globals
 *******************************************************/
#define CLK_SPEED_8MHz      1
#define CLK_SPEED_16MHz     0

#define BUFF_SIZE 1536

// buffers in SRAM
unsigned char SRAM_Buff1[BUFF_SIZE] = {0};
unsigned char SRAM_Buff2[BUFF_SIZE] = {0};

// buffers in FRAM

#pragma PERSISTENT(FRAM_Buff1)
unsigned char FRAM_Buff1[BUFF_SIZE] = {0};
#pragma PERSISTENT(FRAM_Buff2)
unsigned char FRAM_Buff2[BUFF_SIZE] = {0};


volatile uint8_t value3=0;



/*******************************************************
 * FUNC DEFS
 *******************************************************/
// benchmarks

void _buffer_populate(volatile uint8_t *Buff_ptr, uint8_t data_byte);

// setup related
//void uart_setup(void);
void clock_setup2(void);

// helpers
void _delay(uint32_t d);

// debug


/*************************************************************************************
 * SETUP
 *************************************************************************************/
void clock_setup2(void){

#if CLK_SPEED_8MHz
    //Set DCO Frequency to 8MHz
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    //configure MCLK, SMCLK to be source by DCOCLK
    CS_initClockSignal(CS_MCLK,  CS_DCOCLK_SELECT,  CS_CLOCK_DIVIDER_1); //16mhz
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT,  CS_CLOCK_DIVIDER_1); // 16mhz

#endif

#if CLK_SPEED_16MHz

    FRCTL0 = FRCTLPW | NWAITS_1; // above 16Mhz needs FRAM wait states
    CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);
    CS_initClockSignal (CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal (CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

#endif

    __bis_SR_register(GIE);

    //Verify if the Clock settings are as expected
    volatile uint32_t clockValue;
    clockValue = CS_getMCLK();
    clockValue = CS_getACLK();
    clockValue = CS_getSMCLK();
    if(clockValue);

    _delay(1000);

}


/*************************************************************************************
 * buffer related
 *************************************************************************************/
void _buffer_populate(volatile uint8_t *Buff_ptr, uint8_t data_byte){
    uint32_t i;
    for(i=0; i < BUFF_SIZE; i++){
        Buff_ptr[i] = data_byte;
    }
}


/*************************************************************************************
 * DEBUG
 *************************************************************************************/



/*************************************************************************************
 * MAIN
 *************************************************************************************/
void main(void)
{
    /* mandatory init stuff */
    WDTCTL = WDTPW | WDTHOLD;     //Stop WDT
    PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings

    uint16_t i, j;
    uint8_t value1=0;
    uint8_t value2=0;

    clock_setup2();


    //_delay(1000000);

    /* initialize the buffers in SRAM */
    //_buffer_populate(SRAM_Buff1, (uint8_t)0xFF); // buff init
    //_buffer_populate(SRAM_Buff2, (uint8_t)0xFF); // buff init

    _delay(1000000);


    /* -------------- fram write -------------- */
    __no_operation();
    for(i=0; i<100; i++){
        for (j=0;j<BUFF_SIZE;j++){
            FRAM_Buff1[j]=(uint8_t)j;
            FRAM_Buff2[j]=(uint8_t)j;
        }
    }
    __no_operation();

    _delay(1000000);


    /* -------------- fram read -------------- */
    __no_operation();
    for(i=0; i<100; i++){
        for (j=0;j<BUFF_SIZE;j++){
            value1 = FRAM_Buff1[j];
            value2 = FRAM_Buff2[j];
        }
    }
    __no_operation();
    value3 = value1 + value2;
    if(value3);

    _delay(1000000);


    /* -------------- sram write -------------- */
    __no_operation();
    for(i=0; i<100; i++){
        for (j=0;j<BUFF_SIZE;j++){
            SRAM_Buff1[j]=(uint8_t)j;
            SRAM_Buff2[j]=(uint8_t)j;
        }
    }
    __no_operation();

    _delay(1000000);


    /* -------------- sram read -------------- */
    __no_operation();
    for(i=0; i<100; i++){
        for (j=0;j<BUFF_SIZE;j++){
            value1 = SRAM_Buff1[j];
            value2 = SRAM_Buff2[j];
        }
    }
    __no_operation();
    value3 = value1 + value2;
    if(value3);
    _delay(1000000);


}



/*************************************************************************************
 * BENCHMARKING
 *************************************************************************************/





/*************************************************************************************
 * HELPER FUNCTIONS
 *************************************************************************************/
void _delay(uint32_t d){
    uint32_t i;
    for (i=0;i<d;i++){__no_operation();}
}

