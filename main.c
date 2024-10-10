/*
 * EE 690/615 Embedded Systems Lab No. 07
 * Group 02 :
 * 210020009 - Ganesh Panduranga Karamsetty
 * 210020036 - Rishabh Pomaje
 * Program to setup the UART module on the muC and communicate with another muC.
*/
#define CLOCK_HZ    16000000

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

void CLK_enable( void );
void PORT_F_init( void );
void PORT_E_init( void );
void UART5_setup( void );
void UART7_setup( void );
void UART_Tx( unsigned char );
unsigned char UART_Rx( void );

int main(void)

{
    // Enable all the required Clocks
    CLK_enable();
    // Setup Port F
    PORT_F_init();
    // Setup Port E to interface with the UART
    PORT_E_init();
    // Setup UART Module 05
    UART5_setup();
    // Setup UART Module 07
    UART7_setup();
//    int counter = 0 ;
//    int time_limit = 160000 ;
    while(1){
        // Check if there is any Rx message in FIFO
        unsigned char rxData = UART_Rx() ;
        // GPIO_PORTF_DATA_R = |_|_|_|SW1|G|B|R|SW2|
        if ((UART5_RSR_R & (0x02)) != 0){
            GPIO_PORTF_DATA_R = 0x2 ;
            UART7_ECR_R = 0xFF ;
        }
        else{
            if (rxData == 0xAA){
                GPIO_PORTF_DATA_R = 0x8 ;
            }
            else if (rxData == 0xF0){
                GPIO_PORTF_DATA_R = 0x4 ;
            }
        }

        // Now send if any message
        unsigned char status_byte = GPIO_PORTF_DATA_R;
        // Check if SW1 is pressed
        if (((~status_byte) & 0x10) != 0){
            UART_Tx(0xF0);  // Transmit 0xF0 if SW1 is pressed
            // Debouncing :
//            while (counter < time_limit){
//                counter += 1 ;  // Debouncing
//            }
//            counter = 0 ;
        }
        // Check if SW2 is pressed
        if (((~status_byte) & 0x01) != 0){
            UART_Tx(0xAA);  // Transmit 0xAA if SW2 is pressed
            // Debouncing :
//            while (counter < time_limit){
//                counter += 1 ;  // Debouncing
//            }
//            counter = 0 ;
        }
    }
}

void CLK_enable( void )
{
    // Setting up the UART clocks
    SYSCTL_RCGCUART_R |= (1 << 7) | (1 << 5) ;                             // Enabling the clock to UART module 7 and 5
    SYSCTL_RCGCGPIO_R |= 0x00000010 ;                           // Enable clock to GPIO_E
    SYSCTL_RCGCGPIO_R |= 0x00000020 ;                           // Enable clock to GPIO_F
}

void UART_Tx( unsigned char data )
{
    while((UART7_FR_R & (1 << 3)) != 0){
         ;// Check for BUSY bit and wait for Tx FIFO to become free
    }
    UART7_DR_R = data ; // Place the Tx msg in the Data Register
}

unsigned char UART_Rx( void )
{
    if((UART7_FR_R & 0x40) != 0){
        unsigned char rxData = UART7_DR_R ;
        return rxData ; // Read the Rx msg from the Data Register
    }
    else{
        return 0x00 ;
    }
}

void UART5_setup( void )
{
    UART5_CTL_R = 0x00 ;                                        // Disabling the UART
    // Calculations for Baud Rate Divisor
    int UARTSysClk = CLOCK_HZ ;                                 // Clock connected to the UART module
    int clk_div = 16 ;                                          // Clock divider depending on the communication rate
    int baud_rate = 9600 ;                                      // Baud rate for communication

    float BRD = (1.0 * UARTSysClk) / (clk_div * baud_rate) ;    // Baud rate divisor (BRD)
    int BRDI = BRD ;                                            // Integer part of BRD
    BRD = BRD - BRDI ;                                          // Extracting the fractional part of BRD
    int BRDF = 64 * BRD + 0.5 ;                                 // Fractional part of the BRD to write to the register

    // Continue to configure the UART
    UART5_IBRD_R = BRDI ;                                       // Integer part of the BRD
    UART5_FBRD_R = BRDF ;                                       // Fractional part of the BRD
    UART5_LCRH_R |= (1 << 6) | (1 << 5) | (1 << 1) ;            // |7-SPS|6,5-WLEN|4-FEN|3-STP2|2-EPS|1-PEN|0-BRK|
    UART5_CC_R = 0x00 ;                                         // Clock source of the register
    UART5_ECR_R = 0xFF ;
    UART5_CTL_R |= (1 << 9) | (1 << 0) ;               // |9-RXE|8-TXE|7-LBE|6-.|5-HSE|4-EOT|3-SMART|2-SIRLP|1-SIREN|0-UARTEN|
}

void UART7_setup( void )
{
    UART7_CTL_R = 0x00 ;                                        // Disabling the UART
    // Calculations for Baud Rate Divisor
    int UARTSysClk = CLOCK_HZ ;                                 // Clock connected to the UART module
    int clk_div = 16 ;                                          // Clock divider depending on the communication rate
    int baud_rate = 9600 ;                                      // Baud rate for communication

    float BRD = (1.0 * UARTSysClk) / (clk_div * baud_rate) ;    // Baud rate divisor (BRD)
    int BRDI = BRD ;                                            // Integer part of BRD
    BRD = BRD - BRDI ;                                          // Extracting the fractional part of BRD
    int BRDF = 64 * BRD + 0.5 ;                                 // Fractional part of the BRD to write to the register

    // Continue to configure the UART
    UART7_IBRD_R = BRDI ;                                       // Integer part of the BRD
    UART7_FBRD_R = BRDF ;                                       // Fractional part of the BRD
    UART7_LCRH_R |= (1 << 6) | (1 << 5) | (1 << 1) ;            // |7-SPS|6,5-WLEN|4-FEN|3-STP2|2-EPS|1-PEN|0-BRK|
    UART7_CC_R = 0x00 ;                                         // Clock source of the register
    UART7_ECR_R = 0xFF ;
    UART7_CTL_R |= (1 << 9) | (1 << 8) | (1 << 0) ;             // |9-RXE|8-TXE|7-LBE|6-.|5-HSE|4-EOT|3-SMART|2-SIRLP|1-SIREN|0-UARTEN|
}

void PORT_E_init( void )
{
    GPIO_PORTE_LOCK_R = 0x4C4F434B ;                            // Unlock commit register
    GPIO_PORTE_CR_R = 0xF1 ;                                    // Make PORTE0 configurable
    GPIO_PORTE_DEN_R = 0x33 ;                                   // Set PORTE pins 4 pin
    GPIO_PORTE_DIR_R = 0x22 ;                                   // Set PORTE4 pin as input user switch pin
    GPIO_PORTE_PUR_R = 0x22 ;                                   // Pull Up Register
    GPIO_PORTE_AFSEL_R = 0x33 ;                                 // Alternate function select for the PE0 and PE1
    GPIO_PORTE_PCTL_R = 0x110011 ;                                  // Selecting the peripheral for the AFSEL
}

void PORT_F_init( void )
{
    GPIO_PORTF_LOCK_R = 0x4C4F434B ;                            // Unlock commit register
    GPIO_PORTF_CR_R = 0xF1 ;                                    // Make PORTF0 configurable
    GPIO_PORTF_DEN_R = 0x1F ;                                   // Set PORTF pins 4 pin as Digital
    GPIO_PORTF_DIR_R = 0x0E ;                                   // Set PORTF4 pin as input user switch pin
    GPIO_PORTF_PUR_R = 0x11 ;                                   // Pull up register
    GPIO_PORTF_DATA_R = 0x00 ;                                  // Clearing previous data
}
