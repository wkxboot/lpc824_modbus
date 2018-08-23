/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v4.1
processor: LPC824
package_id: LPC824M201JDH20
mcu_data: ksdk2_0
processor_version: 4.0.1
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_swm.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '3', peripheral: USART0, signal: RXD, pin_signal: PIO0_13/ADC_10}
  - {pin_num: '20', peripheral: USART0, signal: TXD, pin_signal: PIO0_14/ACMP_I3/ADC_2}
  - {pin_num: '19', peripheral: USART1, signal: RXD, pin_signal: PIO0_0/ACMP_I1}
  - {pin_num: '6', peripheral: USART1, signal: TXD, pin_signal: PIO0_4/ADC_11}
  - {pin_num: '4', peripheral: SPI1, signal: MISO, pin_signal: PIO0_12}
  - {pin_num: '9', peripheral: SPI1, signal: MOSI, pin_signal: PIO0_11/I2C0_SDA}
  - {pin_num: '10', peripheral: SPI1, signal: SCK, pin_signal: PIO0_10/I2C0_SCL}
  - {pin_num: '11', peripheral: SPI1, signal: SSEL0, pin_signal: PIO0_15}
  - {pin_num: '12', peripheral: GPIO, signal: 'PIO0, 1', pin_signal: PIO0_1/ACMP_I2/CLKIN}
  - {pin_num: '8', peripheral: SWD, signal: SWDIO, pin_signal: SWDIO/PIO0_2}
  - {pin_num: '7', peripheral: SWD, signal: SWCLK, pin_signal: SWCLK/PIO0_3}
  - {pin_num: '10', peripheral: I2C0, signal: SCL, pin_signal: PIO0_10/I2C0_SCL}
  - {pin_num: '9', peripheral: I2C0, signal: SDA, pin_signal: PIO0_11/I2C0_SDA}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M0P */
void BOARD_InitPins(void)
{
    /* Enables clock for switch matrix.: 0x01u */
    CLOCK_EnableClock(kCLOCK_Swm);

    /* USART0_TXD connect to P0_14 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_TXD, kSWM_PortPin_P0_14);

    /* USART0_RXD connect to P0_13 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_RXD, kSWM_PortPin_P0_13);

    /* USART1_TXD connect to P0_4 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART1_TXD, kSWM_PortPin_P0_4);

    /* USART1_RXD connect to P0_0 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART1_RXD, kSWM_PortPin_P0_0);

    /* SPI1_SCK connect to P0_10 */
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_SCK, kSWM_PortPin_P0_10);

    /* SPI1_MOSI connect to P0_11 */
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_MOSI, kSWM_PortPin_P0_11);

    /* SPI1_MISO connect to P0_12 */
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_MISO, kSWM_PortPin_P0_12);

    /* SPI1_SSEL0 connect to P0_15 */
    SWM_SetMovablePinSelect(SWM0, kSWM_SPI1_SSEL0, kSWM_PortPin_P0_15);

    /* SWCLK connect to P0_3 */
    SWM_SetFixedPinSelect(SWM0, kSWM_SWCLK, true);

    /* SWDIO connect to P0_2 */
    SWM_SetFixedPinSelect(SWM0, kSWM_SWDIO, true);

    /* I2C0_SDA connect to P0_11 */
    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SDA, true);

    /* I2C0_SCL connect to P0_10 */
    SWM_SetFixedPinSelect(SWM0, kSWM_I2C0_SCL, true);

    /* Disable clock for switch matrix. */
    CLOCK_DisableClock(kCLOCK_Swm);
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
