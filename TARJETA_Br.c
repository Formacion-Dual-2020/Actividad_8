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

A.1 Escribe «Hola 0» al scia y al scic

B.1 Recibe «Hola 0» y lo manda a scia, responde «¿cómo 0» a scia y scic

A.2 Recibe «¿cómo 0» y lo manda a scia, responde «estás? 0» a scia y scic

B.2 Recibe «estás? 0» y lo manda a scia, responde «Bien 0» a scia y scic


A.3 Recibe «Bien 0» y lo manda a scia.

0 A.1 B.1 A.2 B.2 A.3
.
.
9

A.4 Envía «fin» a scia
B.3 Envía «fin» a scia
    */

   msg_hola="Hola 0";
   msg_como="¿Cómo 0";
   msg_estas="estás? 0";
   msg_bien="Bien. 0";
   msg_fin="Fin.";

   int iTotalLetras=0, iCoincidenFLAG=0, NumMsg=0,irepeticion=0;





   for(irepeticion=0;irepeticion<10;irepeticion++){                     //Clears received string length


 //La tarjeta B primero recibe caracter la palabra «Hola 0» por caracter, el contador de caracteres es "iTotalLetras";
        iTotalLetras=0; iCoincidenFLAG=0;
        while(iCoincidenFLAG){    //Dentro del bucle hasta que el mensaje sea «¿Cómo 0»
                iCoincidenFLAG=0;
           while(ScicRegs.SCIFFRX.bit.RXFFST == 0); ReceivedChar = ScicRegs.SCIRXBUF.all;  //Recibe cada letra;

           if(ReceivedChar != '0'){ msg_r[iTotalLetras] = ReceivedChar; iTotalLetras++;}   // Y la va guardando;
           else{
//Terminó  de recibir la cadena y ahora la va a comparar;


//Si las letras coinciden, "iCoincide" sigue siendo 0. Si no, será igual a 1;

    for(i=0;i<iTotalLetras;i++){       if( (msg_r[i] == msg_hola[i])&&(!iCoincidenFLAG) ){ }else{ iCoincidenFLAG=1; }       }
//Terminó de comparar la cadena recibida con la que debería haber recibido;

//Si coincidieron, la envía a scia y continua, pero si no, no;
            if(!iCoincidenFLAG){ scia_msg(msg_r);}else{}                    //Hace el reenvio del primer mensaje;
				iTotalLetras=0;
           } //Ye se recibió el primer mensaje. Si fue «¿Cómo 0» sale del if, pero si no sale;
       }

        //Envia su  primer respuesta;
              scic_msg(msg_como); scia_msg(msg_como);



//Entonces debería recibir el mensaje «estas? 0», y se quedará en el sigueinte bucle hasta que eso sea;
       while(iCoincidenFLAG){            iCoincidenFLAG=0;

       while(ScicRegs.SCIFFRX.bit.RXFFST == 0); ReceivedChar = ScicRegs.SCIRXBUF.all;  //Recibe cada letra;

       if(ReceivedChar != '0'){ msg_r[iTotalLetras] = ReceivedChar; iTotalLetras++;}   // Y la va guardando;
       else{
//Terminó de recibir la cadena y ahora la va a comparar;


//Si las letras coinciden, "iCoincide" sigue siendo 0. Si no, será igual a 1;

               for(i=0;i<iTotalLetras;i++){       if( (msg_r[i] == msg_estas[i])&&(!iCoincidenFLAG) ){ }else{ iCoincidenFLAG=1; }       }
//Terminó de comparar la cadena recibida con la que debería haber recibido;


//Si coincidieron, la envía a scia y continua, pero si no, no;
           if(!iCoincidenFLAG){ scia_msg(msg_r);}else{}                    //Hace el reenvio del primer mensaje;
				iTotalLetras=0;
			}	//Ye se recibió el primer mensaje, si fue   «¿Cómo 0»;

        }//Ye se recibió el  mensaje. Si fue «estas? 0» sale del if, pero si no, no sale;


       //Ahora debe enviar el segundo mensaje el bien, para ir terminando la frase;
       scic_msg(msg_bien); scia_msg(msg_bien);

//Hasta este punto ya se envió y recibió la frase completa.

       //CREO QUE AQUÍ NO LLEVA DELAY PORQUE ESTA TARJETA NO MANDA EL PRIMER MENSAJE, sino que ya se puede ir a esperar
       //a que la tarjeta A vuelva a comenzar la frase;

   }// Entonces se completan las 10 repeticiones de la frase;

//Y luego ambas tarjetas debe mostrar el mensaje «Fin»
   scia_msg(msg_fin);  // __  y yaka bamos


   /*
   LoopCount = 0;
   for(;;)
   {
       while(ScicRegs.SCIFFRX.bit.RXFFST == 0) { } // wait for empty state

       //Logica para guardar la palabra recibida - ivan
       ReceivedChar = ScicRegs.SCIRXBUF.all;    //Se guarda el caracter contenido en el buffer en ReceivedChar
       if(ReceivedChar != '0'){                 //Si el caracter es diferente de '0'
           msg_r[i] = ReceivedChar;             //este mismo se guarda en la posición i de msg_r
           i++;                                 //Se utiliza una iteración para ir llenando los espacio de msg_r
       }
       else {                                   //Cuando el valor del caracter es '0', se deja de escribir en la cadena msg_r
           //Transmit message to SCIA
           scia_msg(msg_r);                     //se escribe la palabra recibida en la consola de CCS
           i = 0;                               //Se reinicia el contador para cuando se reciba otra palabra
       }
       // - ivan

       LoopCount++;
   }//*/
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
