/*
 * The Clear BSD License
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_debug_console.h"
#include "fsl_dma.h"
#include "fsl_spi.h"
#include "board.h"

#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_SPI_SLAVE SPI0
#define EXAMPLE_SPI_SLAVE_SSEL kSPI_Ssel0Assert

#define EXAMPLE_SPI_SLAVE_DMA_BASEADDR DMA0
#define EXAMPLE_SPI_SLAVE_TX_CHANNEL 7
#define EXAMPLE_SPI_SLAVE_RX_CHANNEL 6

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void SPI_DmaTxCallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);
static void SPI_DmaRxCallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds);
static void EXAMPLE_SlaveInit(void);
static void EXAMPLE_SlaveDMASetup(void);
static void EXAMPLE_SlaveStartDMATransfer(void);
static void EXAMPLE_TransferDataCheck(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
#define BUFFER_SIZE (64)
static uint8_t txBuffer[BUFFER_SIZE];
static uint8_t rxBuffer[BUFFER_SIZE];

dma_handle_t slaveTxHandle;
dma_handle_t slaveRxHandle;

static volatile bool slaveTxFinished = false;
static volatile bool slaveRxFinished = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
static void SPI_DmaTxCallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    slaveTxFinished = true;
}

static void SPI_DmaRxCallback(dma_handle_t *handle, void *param, bool transferDone, uint32_t tcds)
{
    slaveRxFinished = true;
}

int main(void)
{
    /* Initialize the boards */
    /* Enable clock of uart0. */
    CLOCK_EnableClock(kCLOCK_Uart0);
    /* Ser DIV of uart0. */
    CLOCK_SetClkDivider(kCLOCK_DivUsartClk,1U);    

    /* Enable clock of spi0. */
    CLOCK_EnableClock(kCLOCK_Spi0);
    
    BOARD_InitPins();
    BOARD_BootClockIRC12M();
    BOARD_InitDebugConsole();


    /* Print example information. */
    PRINTF("This is SPI dma transfer slave example!\n\r");
    PRINTF("\n\rSlave is working....\n\r");

    /* Initialize the slave SPI with configuration. */
    EXAMPLE_SlaveInit();

    /* Set up slave DMA configuration. */
    EXAMPLE_SlaveDMASetup();

    /* Slave start transfer with master. */
    EXAMPLE_SlaveStartDMATransfer();

    /* Check transfer data. */
    EXAMPLE_TransferDataCheck();

    /* De-initialize the DMA. */
    DMA_Deinit(EXAMPLE_SPI_SLAVE_DMA_BASEADDR);

    /* De-initialize the SPI. */
    SPI_Deinit(EXAMPLE_SPI_SLAVE);

    while (1)
    {
    }
}

static void EXAMPLE_SlaveInit(void)
{
    spi_slave_config_t slaveConfig;

    /* Default configuration for slave:
     * userConfig.enableSlave = true;
     * userConfig.polarity = kSPI_ClockPolarityActiveHigh;
     * userConfig.phase = kSPI_ClockPhaseFirstEdge;
     * userConfig.direction = kSPI_MsbFirst;
     * userConfig.dataWidth = kSPI_Data8Bits;
     * userConfig.sselPol = kSPI_SpolActiveAllLow;
     */
    SPI_SlaveGetDefaultConfig(&slaveConfig);
    SPI_SlaveInit(EXAMPLE_SPI_SLAVE, &slaveConfig);
}

static void EXAMPLE_SlaveDMASetup(void)
{
    /* DMA init */
    DMA_Init(EXAMPLE_SPI_SLAVE_DMA_BASEADDR);

    /* Enable channel and Create handle for RX channel. */
    DMA_EnableChannel(EXAMPLE_SPI_SLAVE_DMA_BASEADDR, EXAMPLE_SPI_SLAVE_RX_CHANNEL);
    DMA_CreateHandle(&slaveRxHandle, EXAMPLE_SPI_SLAVE_DMA_BASEADDR, EXAMPLE_SPI_SLAVE_RX_CHANNEL);
    DMA_SetCallback(&slaveRxHandle, SPI_DmaRxCallback, NULL);

    /* Enable channel and Create handle for TX channel. */
    DMA_EnableChannel(EXAMPLE_SPI_SLAVE_DMA_BASEADDR, EXAMPLE_SPI_SLAVE_TX_CHANNEL);
    DMA_CreateHandle(&slaveTxHandle, EXAMPLE_SPI_SLAVE_DMA_BASEADDR, EXAMPLE_SPI_SLAVE_TX_CHANNEL);
    DMA_SetCallback(&slaveTxHandle, SPI_DmaTxCallback, NULL);

    /* Set the channel priority. */
    DMA_SetChannelPriority(EXAMPLE_SPI_SLAVE_DMA_BASEADDR, EXAMPLE_SPI_SLAVE_TX_CHANNEL, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(EXAMPLE_SPI_SLAVE_DMA_BASEADDR, EXAMPLE_SPI_SLAVE_RX_CHANNEL, kDMA_ChannelPriority2);
}

static void EXAMPLE_SlaveStartDMATransfer(void)
{
    uint32_t i = 0U;
    dma_transfer_config_t slaveTxDmaConfig, slaveRxDmaConfig;

    /* Init Buffer*/
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        txBuffer[i] = i;
    }

    /* Prepare and start DMA RX transfer. */
    DMA_PrepareTransfer(&slaveRxDmaConfig, (void *)&EXAMPLE_SPI_SLAVE->RXDAT, rxBuffer, sizeof(uint8_t), BUFFER_SIZE,
                        kDMA_PeripheralToMemory, NULL);
    DMA_SubmitTransfer(&slaveRxHandle, &slaveRxDmaConfig);

    DMA_StartTransfer(&slaveRxHandle);

    /* Prepare and start DMA TX transfer. */
    DMA_PrepareTransfer(&slaveTxDmaConfig, txBuffer, (void *)&EXAMPLE_SPI_SLAVE->TXDAT, sizeof(uint8_t), BUFFER_SIZE,
                        kDMA_MemoryToPeripheral, NULL);
    DMA_SubmitTransfer(&slaveTxHandle, &slaveTxDmaConfig);

    DMA_StartTransfer(&slaveTxHandle);
}
static void EXAMPLE_TransferDataCheck(void)
{
    uint32_t i = 0U, err = 0U;

    /* Waiting for the transfer complete. */
    while (!(slaveTxFinished | slaveRxFinished))
    {
    }

    PRINTF("\n\rThe received data are:");
    /*Check if the data is right*/
    err = 0U;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        /* Print 16 numbers in a line */
        if ((i & 0x0FU) == 0U)
        {
            PRINTF("\n\r");
        }
        PRINTF("  0x%02X", rxBuffer[i]);
        /* Check if data matched. */
        if (txBuffer[i] != rxBuffer[i])
        {
            err++;
        }
    }

    if (err == 0)
    {
        PRINTF("\n\rSlave DMA transfer succeed!\n\r");
    }
    else
    {
        PRINTF("\n\rSlave DMA transfer faild!\n\r");
    }
}
