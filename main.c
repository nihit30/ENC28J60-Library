// Ethernet Example
// Credits : Dr. Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL w/ ENC28J60
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// ENC28J60 Ethernet controller
//   MOSI (SSI2Tx) on PB7
//   MISO (SSI2Rx) on PB6
//   SCLK (SSI2Clk) on PB4
//   ~CS connected to PB1

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "enc28j60.h"
#include "wait.h"
#include "initHw.h"
#include "led.h"



//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

void main(void)
{
    uint8_t* udpData;
    uint8_t data[128];

    // init controller
    initHw();

    // init ethernet interface
    etherInit(ETHER_UNICAST | ETHER_BROADCAST | ETHER_HALFDUPLEX);
    etherSetIpAddress(10,219,128,2);

    // flash phy leds
    etherWritePhy(PHLCON, 0x0880);
    RED_LED = 1;
    waitMicrosecond(500000);
    etherWritePhy(PHLCON, 0x0990);
    RED_LED = 0;
    waitMicrosecond(500000);

    // message loop
    while (1)
    {
        if (etherKbhit())
        {
            if (etherIsOverflow())
            {
                RED_LED = 1;
                waitMicrosecond(100000);
                RED_LED = 0;
            }
            // get packet
            etherGetPacket(data, 128);
            // handle arp request
            if (etherIsArp(data))
            {
                etherSendArpResp(data);
                RED_LED = 1;
                GREEN_LED = 1;
                waitMicrosecond(50000);
                RED_LED = 0;
                GREEN_LED = 0;
            }
            // handle ip datagram
            if (etherIsIp(data))
            {
                if (etherIsIpUnicast(data))
                {
                    // handle icmp ping request
                    if (etherIsPingReq(data))
                    {
                      etherSendPingResp(data);
                      RED_LED = 1;
                      BLUE_LED = 1;
                      waitMicrosecond(50000);
                      RED_LED = 0;
                      BLUE_LED = 0;
                    }
                    // handle udp datagram
                    if (etherIsUdp(data))
                    {
                        udpData = etherGetUdpData(data);
                        if (udpData[0] == '1')
                            GREEN_LED = 1;
                        if (udpData[0] == '0')
                            GREEN_LED = 0;
                        etherSendUdpData(data, (uint8_t*)"Received", 9);
                        BLUE_LED = 1;
                        waitMicrosecond(100000);
                        BLUE_LED = 0;
                    }
                }
            }
        }
    }

}
