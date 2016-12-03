//==========================================================================================================
//	Davy Huang
//	James Wang
//
//

//==========================================================================================================
//	included dependencies
#include "msp.h"
#include <driverlib.h>
#include <grlib.h>
#include "Crystalfontz128x128_ST7735.h"
#include <stdio.h>

//==========================================================================================================
//	Global Variables
Graphics_Context g_sContext;
static uint16_t resultsBuffer[2];
	//	ADC results buffer for Joystick

volatile int Score_A;
volatile int Score_B;
	//	Score variables

volatile int Position_A_X;
volatile int Position_A_Y;
	//	Position variable for A
volatile int Position_B_X;
volatile int Position_B_Y;
	//	Position variable for A
	//	Range from 20 to 120 in the Y
	//	Range from 10 to 110 in the X

volatile int delay_max=1;
	//	Max delay for Joystick to be pushed before it records a push and set speed

volatile int delay_x;
volatile int delay_y;
	//	actual delay variable that will be iterated to record changing push
volatile int delay_x_In;
volatile int delay_y_In;
	//	actual delay variable that will be iterated to record changing push
volatile int state;






//==========================================================================================================
	//	UART EXAMPLE
const eUSCI_UART_Config uartConfig =
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
    78,                                     // BRDIV = 78
    2,                                       // UCxBRF = 2
    0,                                       // UCxBRS = 0
    EUSCI_A_UART_NO_PARITY,                  // No Parity
    EUSCI_A_UART_LSB_FIRST,                  // MSB First
    EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
    EUSCI_A_UART_MODE,                       // UART mode
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

	volatile char Position_A_X_Out;
	volatile char Position_A_Y_Out;
		//	Output bytes for UART
	volatile char Position_B_X_In;
	volatile char Position_B_Y_In;
		//	Inputs byte for UART
	volatile char receiveByte;
	volatile char receiveByte_last;



//==========================================================================================================
//	Main variable
void main(void){

	//==========================================================================================================
	//	UART EXAMPLE
	/* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
	GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

	/* Setting DCO (clock) to 12MHz */
	CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
	/* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);
    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);
    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

/*
	    MAP_UART_initModule(EUSCI_A0_MODULE, &uartConfig);


	    MAP_UART_enableModule(EUSCI_A0_MODULE);

	    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
*/

	//==========================================================================================================
	//	Initialize variables for Display
		Score_A=0;
		Score_B=0;
			// Set score for A and B to 0

		delay_x=delay_max;
		delay_y=delay_max;
		delay_x_In=delay_max;
		delay_y_In=delay_max;
			//	Set delay to max

		Position_A_X=70;
		Position_A_Y=10;
			//	Starting position for A
		Position_B_X=70;
		Position_B_Y=10;
					//	Starting position for A

		Position_A_X_Out='S';
		Position_A_Y_Out='S';
			// 'S'-Stationary, F-'Forward', B-'Backward'

		Position_B_X_In='S';
		Position_B_Y_In='S';

		receiveByte='Z';
		receiveByte_last='S';
		state=0;
	//==========================================================================================================
	//	Initialize Timers, clocks, ects. See default comments
    /* Halting WDT and disabling master interrupts */
		MAP_WDT_A_holdTimer();
		MAP_Interrupt_disableMaster();

		/* Set the core voltage level to VCORE1 */
		MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);

		/* Set 2 flash wait states for Flash bank 0 and 1*/
		MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
		MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

		/* Initializes Clock System */
		MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
		MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );
		MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );
		MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1 );
		MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

		/* Initializes display */
		Crystalfontz128x128_Init();

		/* Set default screen orientation */
		Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

		/* Initializes graphics context */
		Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
		Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
		Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
		GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
		Graphics_clearDisplay(&g_sContext);


		/* Configures Pin 6.0 and 4.4 as ADC input */
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

		/* Initializing ADC (ADCOSC/64/8) */
		MAP_ADC14_enableModule();
		MAP_ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

		/* Configuring ADC Memory (ADC_MEM0 - ADC_MEM1 (A15, A9)  with repeat)
			 * with internal 2.5v reference */
		MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);
		MAP_ADC14_configureConversionMemory(ADC_MEM0,
				ADC_VREFPOS_AVCC_VREFNEG_VSS,
				ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);

		MAP_ADC14_configureConversionMemory(ADC_MEM1,
				ADC_VREFPOS_AVCC_VREFNEG_VSS,
				ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

		/* Enabling the interrupt when a conversion on channel 1 (end of sequence)
		 *  is complete and enabling conversions */
		MAP_ADC14_enableInterrupt(ADC_INT1);

		/* Enabling Interrupts */
		MAP_Interrupt_enableInterrupt(INT_ADC14);
		MAP_Interrupt_enableMaster();

		/* Setting up the sample timer to automatically step through the sequence
		 * convert.
		 */
		MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

		/* Triggering the start of the sample */
		MAP_ADC14_enableConversion();
		MAP_ADC14_toggleConversionTrigger();

    while(1)
    {
        MAP_PCM_gotoLPM0();


        //==========================================================================================================
        	//	UART EXAMPLE

    }
}


/* This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM1. This signals the end of conversion and the results array is
 * grabbed and placed in resultsBuffer */
void ADC14_IRQHandler(void){
	//==========================================================================================================
	//	Interrupt called every Analog to Digital Conversion
		uint64_t status;
		status = MAP_ADC14_getEnabledInterruptStatus();
		MAP_ADC14_clearInterruptFlag(status);
			//	Enable interrupt and clear flag

		/* ADC_MEM1 conversion completed */
		if(status & ADC_INT1)
		{
			//==========================================================================================================
			//	1. Gets Joystick Position data and stores it in resultBuffer
			resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
			resultsBuffer[1] = ADC14_getResult(ADC_MEM1);

			//==========================================================================================================
			//	2. Gets Joystick clicked data and iterates the Score_X accordingly
				if (!(P4IN & GPIO_PIN1)){
					Score_A=Score_A+1;
						//	Iterates the score upwards
					if(Score_A==999){
						Score_A=0;
							//	Reset if over 999. SUBSTITUE WITH A KILL SCREEN
					}
				}
				char string_score[15];
					//	Char string to store score info
				sprintf(string_score, "A: %03d B: %03d", Score_A, Score_B);
					//	%03d converts the decimal into a 3 digit character
					//	ex: 2 = 002, 12=012, 123=123
				//==========================================================================================================
				//	3.	Prints to screen
				Graphics_drawStringCentered(&g_sContext,
												(int8_t *)string_score,
												AUTO_STRING_LENGTH,
												64,
												10,
												OPAQUE_TEXT);
				//==========================================================================================================
				//	4. Initialize the max ranges the circle A can be in
					int X_Max=120;
					int X_Min=8;
					int Y_Max=120;
					int Y_Min=20;

					//==========================================================================================================
						// 4a.	Reset A if out of range
					if(Position_A_X>X_Max){
						Position_A_X=X_Max;
					}
					if(Position_A_X<X_Min){
						Position_A_X=X_Min;
					}
					if(Position_A_Y>Y_Max){
						Position_A_Y=Y_Max;
					}
					if(Position_A_Y<Y_Min){
						Position_A_Y=Y_Min;
					}

					//==========================================================================================================
						// 4b.	Reset B if out of range
					if(Position_B_X>X_Max){
						Position_B_X=X_Max;
					}
					if(Position_B_X<X_Min){
						Position_B_X=X_Min;
					}
					if(Position_B_Y>Y_Max){
						Position_B_Y=Y_Max;
					}
					if(Position_B_Y<Y_Min){
						Position_B_Y=Y_Min;
					}

					//==========================================================================================================
						// 4c.	Joystick parameters
					int X_zero=8165;
					int Y_zero=8253;
						//	Position Joystick is in without any movement. Can be calibrated
					int threshold_x=1000;
					int threshold_y=1000;
						//	Threshold the Joystick needs to be pushed to be outside to be considered pushed

					//==========================================================================================================
					//	5. Checks if X joystick is in the PUSHED range or the UNPUSHED range

						//	Resets delay counter inside the UNPUSHED range
						if(X_zero-threshold_x<resultsBuffer[0]<X_zero+threshold_x){
							delay_x=delay_max;
							Position_A_X_Out='S';
								//	Outputs stationary
						}

						//	Inside the positive PUSH range
						if(resultsBuffer[0]>X_zero+threshold_x){
							delay_x=delay_x-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_x==0){
							Position_A_X=Position_A_X+1;
								//	Iterate forward in X position
							Position_A_X_Out='F';
								//	Output forward
							delay_x=delay_max;
							}
						}

						//	Inside the negative PUSH range
						if(resultsBuffer[0]<X_zero-threshold_x){
							delay_x=delay_x-1;
								//	Iterates delay downward when is the PUSHED threshold
							if(delay_x==0){
							Position_A_X=Position_A_X-1;
								//	Iterates downward in X position
							Position_A_X_Out='B';
							delay_x=delay_max;
							}
						}

						//==========================================================================================================
						//	6. Checks if Y joystick is in the PUSHED range or the UNPUSHED range
						//	Resets delay counter inside the UNPUSHED range
						if(Y_zero-threshold_y<resultsBuffer[1]<Y_zero+threshold_y){
							delay_y=delay_max;
							Position_A_Y_Out='S';
								//	Output stationary
						}
						if(resultsBuffer[1]>Y_zero+threshold_y){
							delay_y=delay_y-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_y==0){
							Position_A_Y=Position_A_Y-1;
								//	Iterates forward in Y position
							Position_A_Y_Out='F';
								//	Outputs forward
							delay_y=delay_max;
							}
						}
						if(resultsBuffer[1]<Y_zero-threshold_y){
							delay_y=delay_y-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_y==0){
							Position_A_Y=Position_A_Y+1;
								//	Iterates downward in Y position
							Position_A_Y_Out='B';
								//	Outputs backward
							delay_y=delay_max;
							}
						}
						//==========================================================================================================
						//	7. Does calculations for X UART

						if(Position_B_X_In=='S'){
							delay_x_In=delay_max;
						}
						if(Position_B_X_In=='B'){
							delay_x_In=delay_x_In-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_x_In==0){
								Position_B_X=Position_B_X-1;
									//	Iterates backward in X position
								delay_x_In=delay_max;
							}
						}
						if(Position_B_X_In=='F'){
							delay_x_In=delay_x_In-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_x_In==0){
								Position_B_X=Position_B_X+1;
									//	Iterates forward in X position
								delay_x_In=delay_max;
							}
						}
						//==========================================================================================================
							//	8. Does calculations for Y UART
						if(Position_B_Y_In=='S'){
							delay_y_In=delay_max;
						}
						if(Position_B_Y_In=='B'){
							delay_y_In=delay_y_In-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_y_In==0){
								Position_B_Y=Position_B_Y+1;
									//	Iterates downward in Y position
								delay_y_In=delay_max;
							}
						}
						if(Position_B_Y_In=='F'){
							delay_y_In=delay_y_In-1;
								//	Iterates delay downward when in the PUSHED threshold
							if(delay_y_In==0){
								Position_B_Y=Position_B_Y-1;
									//	Iterates forward in Y position
								delay_y_In=delay_max;
							}
						}




						//==========================================================================================================
						//	9. Draws the circles in the new calculated positions
							//==========================================================================================================
							//	a. Player A circle
							Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
							Graphics_fillCircle(&g_sContext,
													Position_A_X,
													Position_A_Y,
													5);
								//	Clears the previous spaces within 5 pixels as white

							Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
							Graphics_fillCircle(&g_sContext,
													Position_A_X,
													Position_A_Y,
													2);
								//	Draws the circle

							//==========================================================================================================
								//	b. Player B circle
							Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
							Graphics_fillCircle(&g_sContext,
													Position_B_X,
													Position_B_Y,
													5);
								//	Clears the previous spaces within 5 pixels as white

							Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
							Graphics_fillCircle(&g_sContext,
													Position_B_X,
													Position_B_Y,
													2);
								//	Draws the circle



							Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);




    }
}

void euscia2_isr(void){
	//==========================================================================================================
		//	Inputs will be S/F/B -> X/Y -> S/F/B -> X/Y

	receiveByte = UCA0RXBUF;
	//	Record current byte

	//==========================================================================================================
		//	Sets assigns previous byte based on current byte
	if(receiveByte=='X'){
		Position_B_X_In=receiveByte_last;
	}
	else if(receiveByte=='Y'){
		Position_B_Y_In=receiveByte_last;
	}
	else{
	}
	receiveByte_last=receiveByte;
		//	Record last byte

	//==========================================================================================================
		//	Outputs byte in loop S/F/B -> X/Y -> S/F/B -> X/Y

	if(state==0){
		MAP_UART_transmitData(EUSCI_A2_BASE, Position_A_X_Out);
		state=state+1;
	}
	else if(state==1){
		MAP_UART_transmitData(EUSCI_A2_BASE, 'X');
		state=state+1;
	}
	else if(state==2){
		MAP_UART_transmitData(EUSCI_A2_BASE, Position_A_Y_Out);
		state=state+1;
	}
	else{
		MAP_UART_transmitData(EUSCI_A2_BASE, 'Y');
		state=0;
	}
}
