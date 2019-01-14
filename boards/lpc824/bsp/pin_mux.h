/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
    kPIN_MUX_DirectionInput = 0U,        /* Input direction */
    kPIN_MUX_DirectionOutput = 1U,       /* Output direction */
    kPIN_MUX_DirectionInputOrOutput = 2U /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*!
 * @brief Hysteresis.: Enable */
#define PIO10_HYS_ENABLE 0x01u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO10_MODE_PULL_UP 0x02u
/*!
 * @brief Hysteresis.: Enable */
#define PIO2_HYS_ENABLE 0x01u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO2_MODE_PULL_UP 0x02u

/*! @name PIO0_1 (number 12), AD_SYNC
  @{ */
#define BOARD_INITPINS_AD_SYNC_CTRL_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_AD_SYNC_CTRL_SIGNAL PIO0                        /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_AD_SYNC_CTRL_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_AD_SYNC_CTRL_GPIO_PIN 1U                        /*!<@brief PIO0 pin index: 1 */
#define BOARD_INITPINS_AD_SYNC_CTRL_PORT 0U                            /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_AD_SYNC_CTRL_PIN 1U                             /*!<@brief 0U pin index: 1 */
#define BOARD_INITPINS_AD_SYNC_CTRL_CHANNEL 1                          /*!<@brief GPIO PIO0 channel: 1 */
#define BOARD_INITPINS_AD_SYNC_CTRL_PIN_NAME PIO0_1                    /*!<@brief Pin name */
#define BOARD_INITPINS_AD_SYNC_CTRL_LABEL "AD_SYNC"                    /*!<@brief Label */
#define BOARD_INITPINS_AD_SYNC_CTRL_NAME "AD_SYNC_CTRL"                /*!<@brief Identifier name */
#define BOARD_INITPINS_AD_SYNC_CTRL_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                       /* @} */

/*! @name PIO0_23 (number 1), LED
  @{ */
#define BOARD_INITPINS_LED_CTRL_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_LED_CTRL_SIGNAL PIO0                        /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_LED_CTRL_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_LED_CTRL_GPIO_PIN 23U                       /*!<@brief PIO0 pin index: 23 */
#define BOARD_INITPINS_LED_CTRL_PORT 0U                            /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_LED_CTRL_PIN 23U                            /*!<@brief 0U pin index: 23 */
#define BOARD_INITPINS_LED_CTRL_CHANNEL 23                         /*!<@brief GPIO PIO0 channel: 23 */
#define BOARD_INITPINS_LED_CTRL_PIN_NAME PIO0_23                   /*!<@brief Pin name */
#define BOARD_INITPINS_LED_CTRL_LABEL "LED"                        /*!<@brief Label */
#define BOARD_INITPINS_LED_CTRL_NAME "LED_CTRL"                    /*!<@brief Identifier name */
#define BOARD_INITPINS_LED_CTRL_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                   /* @} */

/*! @name PIO0_17 (number 2), RWE_485
  @{ */
#define BOARD_INITPINS_RWE_485_CTRL_PERIPHERAL GPIO                    /*!<@brief Device name: GPIO */
#define BOARD_INITPINS_RWE_485_CTRL_SIGNAL PIO0                        /*!<@brief GPIO signal: PIO0 */
#define BOARD_INITPINS_RWE_485_CTRL_GPIO GPIO                          /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_RWE_485_CTRL_GPIO_PIN 17U                       /*!<@brief PIO0 pin index: 17 */
#define BOARD_INITPINS_RWE_485_CTRL_PORT 0U                            /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_RWE_485_CTRL_PIN 17U                            /*!<@brief 0U pin index: 17 */
#define BOARD_INITPINS_RWE_485_CTRL_CHANNEL 17                         /*!<@brief GPIO PIO0 channel: 17 */
#define BOARD_INITPINS_RWE_485_CTRL_PIN_NAME PIO0_17                   /*!<@brief Pin name */
#define BOARD_INITPINS_RWE_485_CTRL_LABEL "RWE_485"                    /*!<@brief Label */
#define BOARD_INITPINS_RWE_485_CTRL_NAME "RWE_485_CTRL"                /*!<@brief Identifier name */
#define BOARD_INITPINS_RWE_485_CTRL_DIRECTION kPIN_MUX_DirectionOutput /*!<@brief Direction */
                                                                       /* @} */
#define  BOARD_HX711_DOUT_GPIO       GPIO                                  
#define  BOARD_HX711_DOUT_PORT       0U    
#define  BOARD_HX711_DOUT_PIN        11      

#define  BOARD_HX711_SCLK_GPIO       GPIO                                  
#define  BOARD_HX711_SCLK_PORT       0U    
#define  BOARD_HX711_SCLK_PIN        10   

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M0P */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
