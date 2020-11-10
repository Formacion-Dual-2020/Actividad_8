//###########################################################################
//
// FILE:    Example_2837xSSci_Echoback.c
//
// TITLE:   SCI Echoback.
//
//! \addtogroup cpu01_example_list
//! <h1>SCI Echoback (sci_echoback)</h1>
//!
//!  This test receives and echo-backs data through the SCI-A port.
//!
//!  The PC application 'hyperterminal' or another terminal
//!  such as 'putty' can be used to view the data from the SCI and
//!  to send information to the SCI.  Characters received
//!  by the SCI port are sent back to the host.
//!
//!  \b Running \b the \b Application
//!  -# Configure hyperterminal or another terminal such as putty:
//!
//!  For hyperterminal you can use the included hyperterminal configuration
//!  file SCI_96.ht.
//!  To load this configuration in hyperterminal
//!    -# Open hyperterminal
//!    -# Go to file->open
//!    -# Browse to the location of the project and
//!       select the SCI_96.ht file.
//!  -# Check the COM port.
//!  The configuration file is currently setup for COM1.
//!  If this is not correct, disconnect (Call->Disconnect)
//!  Open the File-Properties dialogue and select the correct COM port.
//!  -# Connect hyperterminal Call->Call
//!  and then start the 2837xS SCI echoback program execution.
//!  -# The program will print out a greeting and then ask you to
//!  enter a character which it will echo back to hyperterminal.
//!
//!  \note If you are unable to open the .ht file, or you are using
//!  a different terminal, you can open a COM port with the following settings
//!  -  Find correct COM port
//!  -  Bits per second = 9600
//!  -  Date Bits = 8
//!  -  Parity = None
//!  -  Stop Bits = 1
//!  -  Hardware Control = None
//!
//!  \b Watch \b Variables \n
//!  - LoopCount - the number of characters sent
//!
//! \b External \b Connections \n
//!  Connect the SCI-A port to a PC via a transceiver and cable.
//!  - GPIO28 is SCI_A-RXD (Connect to Pin3, PC-TX, of serial DB9 cable)
//!  - GPIO29 is SCI_A-TXD (Connect to Pin2, PC-RX, of serial DB9 cable)
//!
//
//###########################################################################
// $TI Release: F2837xS Support Library v3.10.00.00 $
// $Release Date: Tue May 26 17:16:51 IST 2020 $
// $Copyright:
// Copyright (C) 2014-2020 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "F28x_Project.h"

//
// Globals
//
Uint16 LoopCount;

//
// Function Prototypes
//
void scia_echoback_init(void);
void scia_fifo_init(void);
void scia_xmit(int a);
void scia_msg(char *msg);

void scic_echoback_init(void);
void scic_fifo_init(void);
void scic_xmit(int a);
void scic_msg(char *msg);

//
// Main
//
int iLongitud=0;
void main(void)
{
    Uint16 ReceivedChar;
    char *msg_hola,*msg_como,*msg_estas,*msg_bien,*msg_fin,*msg_r;
    int i=0;

//
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xS_SysCtrl.c file.
//
   InitSysCtrl();

//
// Step 2. Initialize GPIO:
// This example function is found in the F2837xS_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
//
   InitGpio();

//
// For this example, only init the pins for the SCI-A port.
//  GPIO_SetupPinMux() - Sets the GPxMUX1/2 and GPyMUX1/2 register bits
//  GPIO_SetupPinOptions() - Sets the direction and configuration of the GPIOS
// These functions are found in the F2837xS_Gpio.c file.
//

   //Config SCIC
   GPIO_SetupPinMux(90, GPIO_MUX_CPU1, 6);
   GPIO_SetupPinOptions(90, GPIO_INPUT, GPIO_PUSHPULL);
   GPIO_SetupPinMux(89, GPIO_MUX_CPU1, 6);
   GPIO_SetupPinOptions(89, GPIO_OUTPUT, GPIO_ASYNC);

   //Config SCIA
   GPIO_SetupPinMux(84, GPIO_MUX_CPU1, 5);
   GPIO_SetupPinOptions(84, GPIO_INPUT, GPIO_PUSHPULL);
   GPIO_SetupPinMux(85, GPIO_MUX_CPU1, 5);
   GPIO_SetupPinOptions(85, GPIO_OUTPUT, GPIO_ASYNC);

//
// Step 3. Clear all __interrupts and initialize PIE vector table:
// Disable CPU __interrupts
//
   DINT;

//
// Initialize PIE control registers to their default state.
// The default state is all PIE __interrupts disabled and flags
// are cleared.
// This function is found in the F2837xS_PieCtrl.c file.
//
   InitPieCtrl();

//
// Disable CPU __interrupts and clear all CPU __interrupt flags:
//
   IER = 0x0000;
   IFR = 0x0000;

//
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the __interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xS_DefaultIsr.c.
// This function is found in F2837xS_PieVect.c.
//
   InitPieVectTable();

//
// Step 4. User specific code:


//

   scia_fifo_init();       // Initialize the SCI FIFO
   scia_echoback_init();   // Initialize SCI for echoback

   scic_fifo_init();       // Initialize the SCI FIFO
   scic_echoback_init();   // Initialize SCI for echoback



   /*

A.1 Escribe Â«Hola 0Â» al scia y al scic

B.1 Recibe Â«Hola 0Â» y lo manda a scia, responde Â«Â¿cÃ³mo 0Â» a scia y scic

A.2 Recibe Â«Â¿cÃ³mo 0Â» y lo manda a scia, responde Â«estÃ¡s? 0Â» a scia y scic

B.2 Recibe Â«estÃ¡s? 0Â» y lo manda a scia, responde Â«Bien 0Â» a scia y scic


A.3 Recibe Â«Bien 0Â» y lo manda a scia.

0 A.1 B.1 A.2 B.2 A.3
.
.
9

A.4 EnvÃ­a Â«finÂ» a scia
B.3 EnvÃ­a Â«finÂ» a scia
    */

   msg_hola="Hola 0";
   msg_como="Â¿CÃ³mo 0";
   msg_estas="estÃ¡s? 0";
   msg_bien="Bien. 0";
   msg_fin="Fin.";

   int iTotalLetras=0, iCoincidenFLAG=1, NumMsg=0,irepeticion=0;


   for(irepeticion=0;irepeticion<10;irepeticion++){
       /*Lo que hace es:
        *            (1)─Hola
        * (2)─¿Cómo
        *            (3)─estás?
        * (4)─Bien.
        *───────────────again x9;
        * ─Fin       ─Fin.
        * a) Espera hasta que le envíen un mensaje igual a (1).
        * b)      Compara cada vez al recibido;
        * c) Cuando recibe exactamente (1) lo envía por SCIA;
        * d) Envía mensaje (2).
        * e) Espera hasta recibir un mensaje igual a (3);
        * f)      Comprobará cadad vez al recibido;
        * g) Cuando recibe exactamente (3) lo envía por SCIA;
        * h) Ahora envía (4);
        ──────────────────Repite otra nueve veces;
        * k) Finalmente envía Fin. por SCIA
        */
        //a)
        while(iCoincidenFLAG){ iCoincidenFLAG=0;
            //b)
            scic_rcv_msg(msg_r);
            for(i=0;i<iTotalLetras;i++){       if( (msg_r[i] == msg_hola[i])&&(!iCoincidenFLAG) ){ }else{ iCoincidenFLAG=1; }       }
            //c)
            if(!iCoincidenFLAG){ scia_msg(msg_r);}else{}
           }
            //d)
            scic_msg(msg_como); scia_msg(msg_como); iCoincidenFLAG=1;
            //e)
            while(iCoincidenFLAG){ iCoincidenFLAG=0;
                //f)
                scic_rcv_msg(msg_r);
                for(i=0;i<iTotalLetras;i++){       if( (msg_r[i] == msg_estas[i])&&(!iCoincidenFLAG) ){ }else{ iCoincidenFLAG=1; }       }
                //g)
                if(!iCoincidenFLAG){ scia_msg(msg_r);}else{}
            }
            //h)
            scic_msg(msg_bien); scia_msg(msg_bien);
   }
   //k)
   scia_msg(msg_fin);  // __  y yaka bamos
}





// Función para recibir mensajes
void scic_rcv_msg(char *str)
{
    iLongitud = 0;      // Indice para el string

    do
        while(ScicRegs.SCIFFRX.bit.RXFFST == 0)         // Esperar a que el buffer reciba por lo menos un byte.
            ;
    while ((str[iLongitud++] = ScicRegs.SCIRXBUF.all) != '0');      // Asignar el valor en el buffer al string, comparar con el
                                                            // caracter de fin de mensaje ('0') y sumar 1 al índice al terminar.

    str[iLongitud - 1] = '\0';                                  // Asignar el caracter nulo al final del string.
}



//
//  scia_echoback_init - Test 1,SCIA  DLB, 8-bit word, baud rate 0x000F,
//                       default, 1 STOP bit, no parity
//
void scia_echoback_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    SciaRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                    // No parity,8 char bits,
                                    // async mode, idle-line protocol
    SciaRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                    // Disable RX ERR, SLEEP, TXWAKE
    SciaRegs.SCICTL2.all = 0x0003;
    SciaRegs.SCICTL2.bit.TXINTENA = 1;
    SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

    //
    // SCIA at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    //
    SciaRegs.SCIHBAUD.all = 0x0002;
    SciaRegs.SCILBAUD.all = 0x008B;

    SciaRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset
}

void scic_echoback_init()
{
    //
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    //

    ScicRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                    // No parity,8 char bits,
                                    // async mode, idle-line protocol
    ScicRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                    // Disable RX ERR, SLEEP, TXWAKE
    ScicRegs.SCICTL2.all = 0x0003;
    ScicRegs.SCICTL2.bit.TXINTENA = 1;
    ScicRegs.SCICTL2.bit.RXBKINTENA = 1;

    //
    // SCIA at 9600 baud
    // @LSPCLK = 50 MHz (200 MHz SYSCLK) HBAUD = 0x02 and LBAUD = 0x8B.
    // @LSPCLK = 30 MHz (120 MHz SYSCLK) HBAUD = 0x01 and LBAUD = 0x86.
    //
    ScicRegs.SCIHBAUD.all = 0x0002;
    ScicRegs.SCILBAUD.all = 0x008B;

    ScicRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset
}

//
// scia_xmit - Transmit a character from the SCI
//
void scia_xmit(int a)
{
    while (SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF.all =a;
}

void scic_xmit(int a)
{
    while (ScicRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScicRegs.SCITXBUF.all =a;
}

//
// scia_msg - Transmit message via SCIA
//
void scia_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scia_xmit(msg[i]);
        i++;
    }
}


void scic_msg(char * msg)
{
    int i;
    i = 0;
    while(msg[i] != '\0')
    {
        scic_xmit(msg[i]);
        i++;
    }
}

//
// scia_fifo_init - Initialize the SCI FIFO
//
void scia_fifo_init()
{
    SciaRegs.SCIFFTX.all = 0xE040;
    SciaRegs.SCIFFRX.all = 0x2044;
    SciaRegs.SCIFFCT.all = 0x0;
}

void scic_fifo_init()
{
    ScicRegs.SCIFFTX.all = 0xE040;
    ScicRegs.SCIFFRX.all = 0x2044;
    ScicRegs.SCIFFCT.all = 0x0;
}

//
// End of file
//