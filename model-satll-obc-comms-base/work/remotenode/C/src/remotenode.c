/* This code is ignored/never invoked, since partition_2 is just a dummy partition that is not deployed anywhere */
#include "remotenode.h"
#include <stdio.h>
#include <string.h>

// Temp: For transmitting log strings over UART
void transmit_log_info(const char* pString);


void remotenode_startup(void)
{
}


void remotenode_PI_uart_byte_tx(const asn1SccT_UInt8 *pByte){

}

void remotenode_PI_can_tx( const asn1SccRaw_CAN_Frame * pFrame)
{
}


/* TEMP */

void transmit_log_info(const char* pString)
{
    // Iterate over characters in the buffer
    unsigned int charBuff;             // Under the hood TASTE is defaulting to using four bytes for a T-Uint8. Using an unsigned long here to match that default.
    for (size_t i=0; i<strlen(pString); i++) {
        charBuff = pString[i];
        //remotenode_RI_uart_byte_rx(&charBuff);
    } 
}

void remotenode_PI_uart_tx_encoded(const asn1SccT_UInt8 *pByte)
{
    (void)pByte;
}

/* END TEMP */
