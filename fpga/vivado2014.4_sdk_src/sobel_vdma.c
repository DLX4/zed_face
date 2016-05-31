/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
void print(char *str);

int main()
{
    init_platform();

    print("Hello World\n\r");

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x0, 0x4); //reset   MM2S VDMA Control Register
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x0, 0x8); //gen-lock
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x5C,   0x04000000);   //MM2S Start Addresses

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x54, 640*3);//MM2S HSIZE Register---buffer length
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x58, 0x01000780);//S2MM FRMDELAY_STRIDE Register 1920*3=5760 对齐之后为8192=0x2000

    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x0, 0x03);//MM2S VDMA Control Register

    //Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, 480);//MM2S_VSIZE    启动传输





    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xAC, 0x08000000);//S2MM Start Addresses
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA4, 640*3);
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA8, 0x01000780);//S2MM Frame Delay and Stride
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x30, 0x3);//S2MM VDMA Control Register



    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0x50, 480);//MM2S_VSIZE    启动传输
    Xil_Out32(XPAR_AXI_VDMA_0_BASEADDR + 0xA0, 480);//S2MM Vertical Size  start an S2MM transfer

    cleanup_platform();
    return 0;
}


