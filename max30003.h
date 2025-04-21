/**
 ******************************************************************************
 * @file    max30003.h
 * @author  Wiktor Chocianowicz
 * @brief   MAX30003 Ultra-Low Power Biopotential (ECG) AFE driver - Header file
 *
 * @details This header defines register addresses, constants, and macros
 *          required to interface with the MAX30003 ECG AFE via SPI.
 *
 * @note    Reference: MAX30003 Datasheet (Rev 3, Sept 2021)
 * 
 * MIT License
 * 
 * Copyright (c) 2025 Wiktor Chocianowicz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************
 */

#ifndef INC_MAX30003_H_
#define INC_MAX30003_H_

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/**
 * @brief MAX30003 device handle structure
 */
typedef struct {
    SPI_HandleTypeDef *hspi;     /**< SPI handle */
    GPIO_TypeDef *cs_port;       /**< Chip Select GPIO port */
    uint16_t cs_pin;             /**< Chip Select GPIO pin */
} MAX30003_HandleTypeDef;

/************************************************
 * MAX30003 Interrupt masks
 ***********************************************/

#define MAX30003_INT_NONE         0x000000     /**< No interrupt */
#define MAX30003_INT_EINT         (1 << 23)    /**< ECG FIFO threshold reached */
#define MAX30003_INT_EOVF         (1 << 22)    /**< ECG FIFO overflow */
#define MAX30003_INT_FSTINT       (1 << 21)    /**< Fast recovery mode active */
#define MAX30003_INT_DCLOFFINT    (1 << 20)    /**< DC lead-off detected */
#define MAX30003_INT_LONINT       (1 << 11)    /**< Lead-on detected (ULP mode) */
#define MAX30003_INT_RRINT        (1 << 10)    /**< R-to-R interval detected */
#define MAX30003_INT_SAMP         (1 << 9)     /**< Sample synchronization pulse */
#define MAX30003_INT_PLLINT       (1 << 8)     /**< PLL lock lost */

#define MAX30003_SPI_TIMEOUT      	100	/**< SPI transaction timeout in milliseconds */
#define MAX30003_FIFO_LENGTH		32	/**< FIFO length */

/************************************************
 * MAX30003 Register Map
 ***********************************************/

/* Status & Control Registers */
#define MAX30003_REG_STATUS       0x01  /**< Status register (RO) */
#define MAX30003_REG_EN_INT       0x02  /**< Interrupt enable */
#define MAX30003_REG_EN_INT2      0x03  /**< Interrupt enable 2 */
#define MAX30003_REG_MNGR_INT     0x04  /**< Interrupt manager */
#define MAX30003_REG_MNGR_DYN     0x05  /**< Dynamic manager */
#define MAX30003_REG_SW_RST       0x08  /**< Software reset (WO) */
#define MAX30003_REG_SYNCH        0x09  /**< Synchronization (WO) */
#define MAX30003_REG_FIFO_RST     0x0A  /**< FIFO reset (WO) */
#define MAX30003_REG_INFO         0x0F  /**< Info register (RO) */

/* Configuration Registers */
#define MAX30003_REG_CNFG_GEN     0x10  /**< General config */
#define MAX30003_REG_CNFG_CAL     0x12  /**< Calibration config */
#define MAX30003_REG_CNFG_EMUX    0x14  /**< Electrode MUX config */
#define MAX30003_REG_CNFG_ECG     0x15  /**< ECG config */
#define MAX30003_REG_CNFG_RTOR1   0x1D  /**< R-to-R detection config 1 */
#define MAX30003_REG_CNFG_RTOR2   0x1E  /**< R-to-R detection config 2 */

/* Miscellaneous */
#define MAX30003_REG_NO_OP        0x00  /**< No operation */
#define MAX30003_REG_NO_OP_END    0x7F  /**< End No-op */

/************************************************
 * FIFO Commands & Tags
 ************************************************/
#define MAX30003_FIFO_CMD_ECG_BURST  0x20  /**< ECG burst FIFO read */
#define MAX30003_FIFO_CMD_ECG        0x21  /**< ECG single FIFO read */
#define MAX30003_FIFO_CMD_RTOR       0x25  /**< R-to-R interval read */

/* FIFO ETAG values */
#define MAX30003_FIFO_ETAG_VALID         0x00  /**< Valid sample */
#define MAX30003_FIFO_ETAG_FAST          0x01  /**< Fast recovery */
#define MAX30003_FIFO_ETAG_VALID_EOF     0x02  /**< Valid sample, EOF */
#define MAX30003_FIFO_ETAG_FAST_EOF      0x03  /**< Fast recovery, EOF */
#define MAX30003_FIFO_ETAG_EMPTY         0x06  /**< FIFO empty */
#define MAX30003_FIFO_ETAG_OVERFLOW      0x07  /**< FIFO overflow */

/* FIFO Tag Bit Masks */
#define MAX30003_ETAG_MASK               0x07		/**< 8bit ETAG mask */
#define MAX30003_ETAG_SHIFT              3			/**< Right shift for ETAG data bits */
#define MAX30003_ECG_VOLTAGE_DATA_MASK   0x3FFFF	/**< 32bit ECG data mask */
#define MAX30003_ECG_VOLTAGE_DATA_SHIFT  6			/**< Right shift for ECG data bits */

/************************************************
 * Register Bit Masks
 ************************************************/

/* EN_INT (0x02 and 0x03) */

#define MAX30003_EN_INT_EINT_DIS                           (0 << 23)
#define MAX30003_EN_INT_EINT_EN                            (1 << 23)    /**< ECG FIFO Interrupt. Indicates that ECG records meeting/exceeding the ECG FIFO Interrupt Threshold (EFIT) are available for readback. Remains active until ECG FIFO is read back to the extent required to clear the EFIT condition. */

#define MAX30003_EN_INT_EOVF_DIS                           (0 << 22)
#define MAX30003_EN_INT_EOVF_EN                            (1 << 22)    /**< ECG FIFO Overflow. Indicates that the ECG FIFO has overflown and the data record has been corrupted. Remains active until a FIFO Reset (recommended) or SYNCH operation is issued.*/

#define MAX30003_EN_INT_FSTINT_DIS                         (0 << 21)
#define MAX30003_EN_INT_FSTINT_EN                          (1 << 21)    /**< ECG Fast Recovery Mode. Issued when the ECG Fast Recovery Mode is engaged (either manually or automatically). Status and Interrupt Clear behavior is defined by CLR_FAST, see MNGR_INT for details.*/

#define MAX30003_EN_INT_DCLOFFINT_DIS                      (0 << 20)
#define MAX30003_EN_INT_DCLOFFINT_EN                       (1 << 20)    /**< DC Lead-Off Detection Interrupt. Indicates that the MAX30003 has determined it is in an ECG leads off condition (as selected in CNFG_GEN) for more than 115ms. Remains active as long as the leads-off condition persists, then held until cleared by STATUS read back (32nd SCLK).*/

#define MAX30003_EN_INT_LONINT_DIS                         (0 << 11)
#define MAX30003_EN_INT_LONINT_EN                          (1 << 11)    /**< Ultra-Low Power (ULP) Leads-On Detection Interrupt. Indicates that the MAX30003 has determined it is in a leads-on condition (as selected in CNFG_GEN). LONINT is asserted whenever EN_ULP_LON[1:0] in register CNFG_GEN is set to either 0b01 or 0b10 to indicate that the ULP leads on detection mode has been enabled. The STATUS register has to be read back once after ULP leads on detection mode has been activated to clear LONINT and enable leads on detection. LONINT remains active while the leads-on condition persists, then held until cleared by STATUS read back (32nd SCLK).*/

#define MAX30003_EN_INT_RRINT_DIS                          (0 << 10)
#define MAX30003_EN_INT_RRINT_EN                           (1 << 10)    /**< ECG R to R Detector R Event Interrupt. Issued when the R to R detector has identified a new R event. Clear behavior is defined by CLR_RRINT[1:0]; see MNGR_INT for details.*/

#define MAX30003_EN_INT_SAMP_DIS                           (0 << 9)
#define MAX30003_EN_INT_SAMP_EN                            (1 << 9)     /**< Sample Synchronization Pulse. Issued on the ECG base-rate sampling instant, for use in assisting µC monitoring and synchronizing other peripheral operations and data, generally recommended for use as a dedicated interrupt. Frequency is selected by SAMP_IT[1:0], see MNGR_INT for details. Clear behavior is defined by CLR_SAMP, see MNGR_INT for details.*/

#define MAX30003_EN_INT_PLLINT_DIS                         (0 << 8)
#define MAX30003_EN_INT_PLLINT_EN                          (1 << 8)     /**< PLL Unlocked Interrupt. Indicates that the PLL has not yet achieved or has lost its phase lock. PLLINT will only be asserted when the PLL is powered up and active (ECG and/or BIOZ Channel enabled). Remains asserted while the PLL unlocked condition persists, then held until cleared by STATUS read back (32nd SCLK).*/

#define MAX30003_EN_INT_INTB_TYPE_3S                       (0x0 << 0)   /**< Disabled (Three-state)*/
#define MAX30003_EN_INT_INTB_TYPE_CMOS                     (0x1 << 0)   /**< CMOS Driver*/
#define MAX30003_EN_INT_INTB_TYPE_OPEN_DRAIN               (0x2 << 0)   /**< Open-Drain N/nMOS Driver*/
#define MAX30003_EN_INT_INTB_TYPE_OPEN_DRAIN_125K_PULLUP   (0x3 << 0)   /**< Open-Drain N/nMOS Driver with Internal 125kΩ Pullup Resistance*/

#define MAX30003_EN_INT_DEFAULT_CONFIG                     (0x000003 << 0)   /**< Default EN_INT register config*/


/* MNGR_INT (0x04) */

#define MAX30003_MNGR_INT_EFIT_1                                    (0x00 << 19) /**< ECG FIFO Interrupt Threshold = 1*/
#define MAX30003_MNGR_INT_EFIT_2                                    (0x01 << 19) /**< ECG FIFO Interrupt Threshold = 2*/
#define MAX30003_MNGR_INT_EFIT_3                                    (0x02 << 19) /**< ECG FIFO Interrupt Threshold = 3*/
#define MAX30003_MNGR_INT_EFIT_4                                    (0x03 << 19) /**< ECG FIFO Interrupt Threshold = 4*/
#define MAX30003_MNGR_INT_EFIT_5                                    (0x04 << 19) /**< ECG FIFO Interrupt Threshold = 5*/
#define MAX30003_MNGR_INT_EFIT_6                                    (0x05 << 19) /**< ECG FIFO Interrupt Threshold = 6*/
#define MAX30003_MNGR_INT_EFIT_7                                    (0x06 << 19) /**< ECG FIFO Interrupt Threshold = 7*/
#define MAX30003_MNGR_INT_EFIT_8                                    (0x07 << 19) /**< ECG FIFO Interrupt Threshold = 8*/
#define MAX30003_MNGR_INT_EFIT_9                                    (0x08 << 19) /**< ECG FIFO Interrupt Threshold = 9*/
#define MAX30003_MNGR_INT_EFIT_10                                   (0x09 << 19) /**< ECG FIFO Interrupt Threshold = 10*/
#define MAX30003_MNGR_INT_EFIT_11                                   (0x0A << 19) /**< ECG FIFO Interrupt Threshold = 11*/
#define MAX30003_MNGR_INT_EFIT_12                                   (0x0B << 19) /**< ECG FIFO Interrupt Threshold = 12*/
#define MAX30003_MNGR_INT_EFIT_13                                   (0x0C << 19) /**< ECG FIFO Interrupt Threshold = 13*/
#define MAX30003_MNGR_INT_EFIT_14                                   (0x0D << 19) /**< ECG FIFO Interrupt Threshold = 14*/
#define MAX30003_MNGR_INT_EFIT_15                                   (0x0E << 19) /**< ECG FIFO Interrupt Threshold = 15*/
#define MAX30003_MNGR_INT_EFIT_16                                   (0x0F << 19) /**< ECG FIFO Interrupt Threshold = 16*/
#define MAX30003_MNGR_INT_EFIT_17                                   (0x10 << 19) /**< ECG FIFO Interrupt Threshold = 17*/
#define MAX30003_MNGR_INT_EFIT_18                                   (0x11 << 19) /**< ECG FIFO Interrupt Threshold = 18*/
#define MAX30003_MNGR_INT_EFIT_19                                   (0x12 << 19) /**< ECG FIFO Interrupt Threshold = 19*/
#define MAX30003_MNGR_INT_EFIT_20                                   (0x13 << 19) /**< ECG FIFO Interrupt Threshold = 20*/
#define MAX30003_MNGR_INT_EFIT_21                                   (0x14 << 19) /**< ECG FIFO Interrupt Threshold = 21*/
#define MAX30003_MNGR_INT_EFIT_22                                   (0x15 << 19) /**< ECG FIFO Interrupt Threshold = 22*/
#define MAX30003_MNGR_INT_EFIT_23                                   (0x16 << 19) /**< ECG FIFO Interrupt Threshold = 23*/
#define MAX30003_MNGR_INT_EFIT_24                                   (0x17 << 19) /**< ECG FIFO Interrupt Threshold = 24*/
#define MAX30003_MNGR_INT_EFIT_25                                   (0x18 << 19) /**< ECG FIFO Interrupt Threshold = 25*/
#define MAX30003_MNGR_INT_EFIT_26                                   (0x19 << 19) /**< ECG FIFO Interrupt Threshold = 26*/
#define MAX30003_MNGR_INT_EFIT_27                                   (0x1A << 19) /**< ECG FIFO Interrupt Threshold = 27*/
#define MAX30003_MNGR_INT_EFIT_28                                   (0x1B << 19) /**< ECG FIFO Interrupt Threshold = 28*/
#define MAX30003_MNGR_INT_EFIT_29                                   (0x1C << 19) /**< ECG FIFO Interrupt Threshold = 29*/
#define MAX30003_MNGR_INT_EFIT_30                                   (0x1D << 19) /**< ECG FIFO Interrupt Threshold = 30*/
#define MAX30003_MNGR_INT_EFIT_31                                   (0x1E << 19) /**< ECG FIFO Interrupt Threshold = 31*/
#define MAX30003_MNGR_INT_EFIT_32                                   (0x1F << 19) /**< ECG FIFO Interrupt Threshold = 32*/

#define MAX30003_MNGR_INT_CLR_FAST_DIS                              (0 << 6) /**< FSTINT remains active until the FAST mode is disengaged (manually or automatically), then held until cleared by STATUS read back (32nd SCLK)*/
#define MAX30003_MNGR_INT_CLR_FAST_EN                               (1 << 6) /**< FSTINT remains active until cleared by STATUS read back (32nd SCLK), even if the MAX30003 remains in FAST recovery mode. Once cleared, FSTINT will not be re-asserted until FAST mode is exited and re-entered, either manually or automatically*/

#define MAX30003_MNGR_INT_CLR_RRINT_ON_STATUS_REGISTER_READ_BACK    (0x0 << 4) /**< Clear RRINT on STATUS Register Read Back*/
#define MAX30003_MNGR_INT_CLR_RRINT_ON_RTOR_REGISTER_READ_BACK      (0x1 << 4) /**< Clear RRINT on RTOR Register Read Back*/
#define MAX30003_MNGR_INT_CLR_RRINT_SELF_CLEAR                      (0x2 << 4) /**< Self-Clear RRINT after one ECG data rate cycle, approximately 2ms to 8ms*/

#define MAX30003_MNGR_INT_CLR_SAMP_SELF_CLEAR_ON_STATUS_READBACK    (0 << 2) /**< Clear SAMP on STATUS Register Read Back (recommended for debug/evaluation only)*/
#define MAX30003_MNGR_INT_CLR_SAMP_SELF_CLEAR                       (1 << 2) /**< Self-clear SAMP after approximately one-fourth of one data rate cycle*/

#define MAX30003_MNGR_INT_SAMP_IT_EVERY_SAMPLE                      (0x0 << 0) /**< Sample Synchronization Pulse (SAMP) Frequency. Issued every sample instant*/
#define MAX30003_MNGR_INT_SAMP_IT_EVERY_2ND_SAMPLE                  (0x1 << 0) /**< Sample Synchronization Pulse (SAMP) Frequency. Issued every 2nd sample instant*/
#define MAX30003_MNGR_INT_SAMP_IT_EVERY_4TH_SAMPLE                  (0x2 << 0) /**< Sample Synchronization Pulse (SAMP) Frequency. Issued every 4th sample instant*/
#define MAX30003_MNGR_INT_SAMP_IT_EVERY_16TH_SAMPLE                 (0x3 << 0) /**< Sample Synchronization Pulse (SAMP) Frequency. Issued every 16th sample instant*/

#define MAX30003_MNGR_INT_DEFAULT_CONFIG                            (0x780004 << 0)   /**< Default MNGR_INT register config*/

/* MNGR_DYN (0x05) */

#define MAX30003_MNGR_DYN_FAST_NORMAL_MODE        (0x0 << 22) /**< Normal Mode (Fast Recovery Mode Disabled)*/
#define MAX30003_MNGR_DYN_FAST_MANUAL_MODE        (0x1 << 22) /**< Manual Fast Recovery Mode Enable (remains active until disabled)*/
#define MAX30003_MNGR_DYN_FAST_AUTOMATIC_MODE     (0x2 << 22) /**< Automatic Fast Recovery Mode Enable (Fast Recovery automatically activated when/while ECG outputs are saturated, using FAST_TH)*/

#define MAX30003_MNGR_DYN_FAST_TH_SHIFT           16                                        /**< Shift for MNGR_DYN FAST_TH functionality. Automatic Fast Recovery Threshold: If FAST[1:0] = 10 and the output of an ECG measurement exceeds the symmetric thresholds defined by 2048*FAST_TH for more than 125ms, the Fast Recovery mode will be automatically engaged and remain active for 500ms. For example, the default value (FAST_TH = 0x3F) corresponds to an ECG output upper threshold of 0x1F800, and an ECG output lower threshold of 0x20800*/
#define MAX30003_MNGR_DYN_FAST_TH_MASK            0x3F                                      /**< Mask for MNGR_DYN FAST_TH bits*/
#define MAX30003_MNGR_DYN_FAST_TH_DEFAULT         (0x3F << MAX30003_MNGR_DYN_FAST_TH_SHIFT) /**< Default value*/

#define MAX30003_MNGR_DYN_DEFAULT_CONFIG          (0x3F0000 << 0) /**< Default MNGR_DYN register config*/

/* SW_RST (0x08) */

#define MAX30003_SW_RST_D           (0x000000 << 0) /**< Reset*/

/* SYNCH (0x09) */

#define MAX30003_SYNCH_D            (0x000000 << 0) /**< Synch*/

/* FIFO_RST (0x0A) */

#define MAX30003_FIFO_RST_D         (0x000000 << 0) /**< FIFO Reset*/

/* CNFG_GEN (0x10) */

#define MAX30003_CNFG_GEN_EN_ULP_LON_DIS                (0x0 << 22) /**< ULP Lead-On Detection disabled*/
#define MAX30003_CNFG_GEN_EN_ULP_LON_EN                 (0x1 << 22) /**< ECG ULP Lead-On Detection enabled*/

#define MAX30003_CNFG_GEN_FMSTR_512HZ_ECG_PROGGRESION   (0x0 << 20) /**< FMSTR = 32768Hz, TRES = 15.26µs (512Hz ECG progressions)*/
#define MAX30003_CNFG_GEN_FMSTR_500HZ_ECG_PROGGRESION   (0x1 << 20) /**< FMSTR = 32000Hz, TRES = 15.63µs (500Hz ECG progressions)*/
#define MAX30003_CNFG_GEN_FMSTR_200HZ_ECG_PROGGRESION   (0x2 << 20) /**< FMSTR = 32000Hz, TRES = 15.63µs (200Hz ECG progressions)*/
#define MAX30003_CNFG_GEN_FMSTR_199HZ_ECG_PROGGRESION   (0x3 << 20) /**< FMSTR = 31968.78Hz, TRES = 15.64µs (199.8049Hz ECG progressions)*/

#define MAX30003_CNFG_GEN_EN_ECG_DIS                    (0 << 19) /**< ECG Channel disabled*/
#define MAX30003_CNFG_GEN_EN_ECG_EN                     (1 << 19) /**< ECG Channel enabled*/

#define MAX30003_CNFG_GEN_EN_DCLOFF_DIS                 (0x0 << 12) /**< DC Lead-Off Detection disabled*/
#define MAX30003_CNFG_GEN_EN_DCLOFF_EN                  (0x1 << 12) /**< DCLOFF Detection applied to the ECGP/N pins*/

#define MAX30003_CNFG_GEN_DCLOFF_IPOL_ECGP_PULLUP       (0 << 11) /**< ECGP - Pullup, ECGN – Pulldown*/
#define MAX30003_CNFG_GEN_DCLOFF_IPOL_ECGP_PULLDOWN     (1 << 11) /**< ECGP - Pulldown, ECGN – Pullup*/

#define MAX30003_CNFG_GEN_DCLOFF_IMAG_0nA               (0x0 << 8) /**< DC Lead-Off Current 0nA (Disable and Disconnect Current Sources)*/
#define MAX30003_CNFG_GEN_DCLOFF_IMAG_5nA               (0x1 << 8) /**< DC Lead-Off Current 5nA*/
#define MAX30003_CNFG_GEN_DCLOFF_IMAG_10nA              (0x2 << 8) /**< DC Lead-Off Current 10nA*/
#define MAX30003_CNFG_GEN_DCLOFF_IMAG_20nA              (0x3 << 8) /**< DC Lead-Off Current 20nA*/
#define MAX30003_CNFG_GEN_DCLOFF_IMAG_50nA              (0x4 << 8) /**< DC Lead-Off Current 50nA*/
#define MAX30003_CNFG_GEN_DCLOFF_IMAG_100nA             (0x5 << 8) /**< DC Lead-Off Current 100nA*/

#define MAX30003_CNFG_GEN_DCLOFF_VTH_VMID_PM_300        (0x0 << 6) /**< VMID ± 300mV*/
#define MAX30003_CNFG_GEN_DCLOFF_VTH_VMID_PM_400        (0x1 << 6) /**< VMID ± 400mV*/
#define MAX30003_CNFG_GEN_DCLOFF_VTH_VMID_PM_450        (0x2 << 6) /**< VMID ± 450mV*/
#define MAX30003_CNFG_GEN_DCLOFF_VTH_VMID_PM_500        (0x3 << 6) /**< VMID ± 500mV*/

#define MAX30003_CNFG_GEN_EN_RBIAS_DIS                  (0x0 << 4) /**< Resistive Bias disabled*/
#define MAX30003_CNFG_GEN_EN_RBIAS_EN                   (0x1 << 4) /**< ECG Resistive Bias enabled if EN_ECG is also enabled*/

#define MAX30003_CNFG_GEN_RBIASV_50M                    (0x0 << 2) /**< RBIAS = 50MΩ*/
#define MAX30003_CNFG_GEN_RBIASV_100M                   (0x1 << 2) /**< RBIAS = 100MΩ*/
#define MAX30003_CNFG_GEN_RBIASV_200M                   (0x2 << 2) /**< RBIAS = 200MΩ*/

#define MAX30003_CNFG_GEN_RBIASP_DIS                    (0 << 1) /**< ECGP is not resistively connected to VMID*/
#define MAX30003_CNFG_GEN_RBIASP_EN                     (1 << 1) /**< ECGP is connected to VMID through a resistor (selected by RBIASV)*/

#define MAX30003_CNFG_GEN_RBIASN_DIS                    (0 << 0) /**< ECGN is not resistively connected to VMID*/
#define MAX30003_CNFG_GEN_RBIASN_EN                     (1 << 0) /**< ECGN is connected to VMID through a resistor (selected by RBIASV)*/

#define MAX30003_CNFG_GEN_DEFAULT_CONFIG                (0x000004 << 0) /**< Default CNFG_GEN register config*/

/* CNFG_CAL (0x12) */

#define MAX30003_CNFG_CAL_EN_VCAL_DIS                   (0 << 22) /**< Calibration sources and modes disabled*/
#define MAX30003_CNFG_CAL_EN_VCAL_EN                    (1 << 22) /**< Calibration sources and modes enabled*/

#define MAX30003_CNFG_CAL_VMODE_UNIPOLAR                (0 << 21) /**< Unipolar, sources swing between VMID ± VMAG and VMID*/
#define MAX30003_CNFG_CAL_VMODE_BIPOLAR                 (1 << 21) /**< Bipolar, sources swing between VMID + VMAG and VMID - VMAG*/

#define MAX30003_CNFG_CAL_VMAG_0_25mV                   (0 << 20) /**< VMAG = 0.25mV*/
#define MAX30003_CNFG_CAL_VMAG_0_50mV                   (1 << 20) /**< VMAG = 0.50mV*/

#define MAX30003_CNFG_CAL_FCAL_256Hz                    (0x0 << 12) /**< Calibration Source Frequency = FMSTR/128 (Approximately 256Hz)*/
#define MAX30003_CNFG_CAL_FCAL_64Hz                     (0x1 << 12) /**< Calibration Source Frequency = FMSTR/512 (Approximately 64Hz)*/
#define MAX30003_CNFG_CAL_FCAL_16Hz                     (0x2 << 12) /**< Calibration Source Frequency = FMSTR/2048 (Approximately 16Hz)*/
#define MAX30003_CNFG_CAL_FCAL_4Hz                      (0x3 << 12) /**< Calibration Source Frequency = FMSTR/8192 (Approximately 4Hz)*/
#define MAX30003_CNFG_CAL_FCAL_1Hz                      (0x4 << 12) /**< Calibration Source Frequency = FMSTR/2^15 (Approximately 1Hz)*/
#define MAX30003_CNFG_CAL_FCAL_1_4Hz                    (0x5 << 12) /**< Calibration Source Frequency = FMSTR/2^17 (Approximately 1/4Hz)*/
#define MAX30003_CNFG_CAL_FCAL_1_16Hz                   (0x6 << 12) /**< Calibration Source Frequency = FMSTR/2^19 (Approximately 1/16Hz)*/
#define MAX30003_CNFG_CAL_FCAL_1_64Hz                   (0x7 << 12) /**< Calibration Source Frequency = FMSTR/2^21 (Approximately 1/64Hz)*/

#define MAX30003_CNFG_CAL_FIFTY_DUTY_SELECT             (0 << 11) /**< Use CAL_THIGH to select time high for VCALP and VCALN*/
#define MAX30003_CNFG_CAL_FIFTY_DUTY_50                 (1 << 11) /**< THIGH = 50% (CAL_THIGH[10:0] are ignored)*/

#define MAX30003_CNFG_CAL_THIGH_SHIFT                   0                                        /**< Shift for THIGH. Calibration Source Time High Selection. THIGH = THIGH[10:0] x CAL_RES*/
#define MAX30003_CNFG_CAL_THIGH_MASK                    0x7FF                                    /**< Mask for THIGH bits*/
#define MAX30003_CNFG_CAL_THIGH_DEFAULT                 (0x000 << MAX30003_CNFG_CAL_THIGH_SHIFT) /**< Default value*/

#define MAX30003_CNFG_CAL_DEFAULT_CONFIG                (0x004800 << 0) /**< Default CNFG_CAL register config*/

/* CNFG_EMUX (0x14) */

#define MAX30003_CNFG_EMUX_POL_NON_INVERTED                 (0 << 23) /**< ECG Input Polarity Non-inverted*/
#define MAX30003_CNFG_EMUX_POL_INVERTED                     (1 << 23) /**< ECG Input Polarity Inverted*/

#define MAX30003_CNFG_EMUX_OPENP_INTERNALLY_CONNECTED       (0 << 21) /**< ECGP is internally connected to the ECG AFE Channel*/
#define MAX30003_CNFG_EMUX_OPENP_INTERNALLY_ISOLATED        (1 << 21) /**< ECGP is internally isolated from the ECG AFE Channel*/

#define MAX30003_CNFG_EMUX_OPENN_INTERNALLY_CONNECTED       (0 << 20) /**< ECGN is internally connected to the ECG AFE Channel*/
#define MAX30003_CNFG_EMUX_OPENN_INTERNALLY_ISOLATED        (1 << 20) /**< ECGN is internally isolated from the ECG AFE Channel*/

#define MAX30003_CNFG_EMUX_CALP_SEL_NONE                    (0x0 << 18) /**< No calibration signal applied*/
#define MAX30003_CNFG_EMUX_CALP_SEL_IN_TO_VMID              (0x1 << 18) /**< Input is connected to VMID*/
#define MAX30003_CNFG_EMUX_CALP_SEL_IN_TO_VCALP             (0x2 << 18) /**< Input is connected to VCALP (only available if CAL_EN_VCAL = 1)*/
#define MAX30003_CNFG_EMUX_CALP_SEL_IN_TO_VCALN             (0x3 << 18) /**< Input is connected to VCALN (only available if CAL_EN_VCAL = 1)*/

#define MAX30003_CNFG_EMUX_CALN_SEL_NONE                    (0x0 << 16) /**< No calibration signal applied*/
#define MAX30003_CNFG_EMUX_CALN_SEL_IN_TO_VMID              (0x1 << 16) /**< Input is connected to VMID*/
#define MAX30003_CNFG_EMUX_CALN_SEL_IN_TO_VCALP             (0x2 << 16) /**< Input is connected to VCALP (only available if CAL_EN_VCAL = 1)*/
#define MAX30003_CNFG_EMUX_CALN_SEL_IN_TO_VCALN             (0x3 << 16) /**< Input is connected to VCALN (only available if CAL_EN_VCAL = 1)*/

#define MAX30003_CNFG_EMUX_DEFAULT_CONFIG                   (0x300000 << 0) /**< Default CNFG_EMUX register config*/

/* CNFG_ECG (0x15) */

#define MAX30003_CNFG_ECG_DEFAULT      (0x000000 << 0)

#define MAX30003_CNFG_ECG_RATE_512     (0x0 << 22) /**< FMSTR = 00/01/10/11. ECG Data Rate = 512/500/RESERVED/RESERVED sps*/
#define MAX30003_CNFG_ECG_RATE_256     (0x1 << 22) /**< FMSTR = 00/01/10/11. ECG Data Rate = 256/250/RESERVED/RESERVED sps*/
#define MAX30003_CNFG_ECG_RATE_128     (0x2 << 22) /**< FMSTR = 00/01/10/11. ECG Data Rate = 128/125/200/199.8 sps*/

#define MAX30003_CNFG_ECG_GAIN_20      (0x0 << 16) /**< ECG Channel Gain = 20V/V*/
#define MAX30003_CNFG_ECG_GAIN_40      (0x1 << 16) /**< ECG Channel Gain = 40V/V*/
#define MAX30003_CNFG_ECG_GAIN_80      (0x2 << 16) /**< ECG Channel Gain = 80V/V*/
#define MAX30003_CNFG_ECG_GAIN_160     (0x3 << 16) /**< ECG Channel Gain = 160V/V*/

#define MAX30003_CNFG_ECG_DHPF_DIS     (0 << 14) /**< ECG Channel Digital High-Pass Filter Cutoff Frequency = Bypass (DC)*/
#define MAX30003_CNFG_ECG_DHPF_EN      (1 << 14) /**< ECG Channel Digital High-Pass Filter Cutoff Frequency = 0.50Hz*/

#define MAX30003_CNFG_ECG_DLPF_BYPASS  (0x0 << 12) /**< ECG Channel Digital Low-Pass Filter Cutoff Frequency = Bypass (Decimation only, no FIR filter applied)*/
#define MAX30003_CNFG_ECG_DLPF_40      (0x1 << 12) /**< ECG Channel Digital Low-Pass Filter Cutoff Frequency = approximately 40Hz (Except for 125 and 128sps settings)*/
#define MAX30003_CNFG_ECG_DLPF_100     (0x2 << 12) /**< ECG Channel Digital Low-Pass Filter Cutoff Frequency = approximately 100Hz (Available for 512, 256, 500, and 250sps ECG Rate selections only)*/
#define MAX30003_CNFG_ECG_DLPF_150     (0x3 << 12) /**< ECG Channel Digital Low-Pass Filter Cutoff Frequency = approximately 150Hz (Available for 512 and 500sps ECG Rate selections only)*/

#define MAX30003_CNFG_ECG_DEFAULT_CONFIG  (0x805000 << 0) /**< Default CNFG_ECG register config*/

/* CNFG_RTOR (0x1D & 0x1E) */

#define MAX30003_CNFG_RTOR_eWNDW_6             (0x0 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 6 * 8ms = 48ms*/
#define MAX30003_CNFG_RTOR_eWNDW_8             (0x1 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 8 * 8ms = 64ms*/
#define MAX30003_CNFG_RTOR_eWNDW_10            (0x2 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 10 * 8ms = 80ms*/
#define MAX30003_CNFG_RTOR_eWNDW_12            (0x3 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 12 * 8ms = 96*/
#define MAX30003_CNFG_RTOR_eWNDW_14            (0x4 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 14 * 8ms = 112*/
#define MAX30003_CNFG_RTOR_eWNDW_16            (0x5 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 16 * 8ms = 128*/
#define MAX30003_CNFG_RTOR_eWNDW_18            (0x6 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 18 * 8ms = 144*/
#define MAX30003_CNFG_RTOR_eWNDW_20            (0x7 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 20 * 8ms = 160*/
#define MAX30003_CNFG_RTOR_eWNDW_22            (0x8 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 22 * 8ms = 176*/
#define MAX30003_CNFG_RTOR_eWNDW_24            (0x9 << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 24 * 8ms = 192*/
#define MAX30003_CNFG_RTOR_eWNDW_26            (0xA << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 26 * 8ms = 208*/
#define MAX30003_CNFG_RTOR_eWNDW_28            (0xB << 20) /**< R to R Window Averaging (Window Width = RTOR_WNDW[3:0] * 8ms) = 28 * 8ms = 224*/

#define MAX30003_CNFG_RTOR_GAIN_DEFAULT        (0xF << 16) /**< R to R Gain = AUTO*/
#define MAX30003_CNFG_RTOR_GAIN_1              (0x0 << 16) /**< R to R Gain = 1*/
#define MAX30003_CNFG_RTOR_GAIN_2              (0x1 << 16) /**< R to R Gain = 2*/
#define MAX30003_CNFG_RTOR_GAIN_4              (0x2 << 16) /**< R to R Gain = 4*/
#define MAX30003_CNFG_RTOR_GAIN_8              (0x3 << 16) /**< R to R Gain = 8*/
#define MAX30003_CNFG_RTOR_GAIN_16             (0x4 << 16) /**< R to R Gain = 16*/
#define MAX30003_CNFG_RTOR_GAIN_32             (0x5 << 16) /**< R to R Gain = 32*/
#define MAX30003_CNFG_RTOR_GAIN_64             (0x6 << 16) /**< R to R Gain = 64*/
#define MAX30003_CNFG_RTOR_GAIN_128            (0x7 << 16) /**< R to R Gain = 128*/
#define MAX30003_CNFG_RTOR_GAIN_256            (0x8 << 16) /**< R to R Gain = 256*/
#define MAX30003_CNFG_RTOR_GAIN_512            (0x9 << 16) /**< R to R Gain = 512*/
#define MAX30003_CNFG_RTOR_GAIN_1024           (0xA << 16) /**< R to R Gain = 1024*/
#define MAX30003_CNFG_RTOR_GAIN_2048           (0xB << 16) /**< R to R Gain = 2048*/
#define MAX30003_CNFG_RTOR_GAIN_4096           (0xC << 16) /**< R to R Gain = 4096*/
#define MAX30003_CNFG_RTOR_GAIN_8192           (0xD << 16) /**< R to R Gain = 8192*/
#define MAX30003_CNFG_RTOR_GAIN_16384          (0xE << 16) /**< R to R Gain = 16384*/
#define MAX30003_CNFG_RTOR_GAIN_AUTO           (0xF << 16) /**< R to R Gain = AUTO*/

#define MAX30003_CNFG_RTOR_EN_RTOR_DIS         (0 << 15) /**< RTOR Detection disabled*/
#define MAX30003_CNFG_RTOR_EN_RTOR_EN          (1 << 15) /**< RTOR Detection enabled if EN_ECG is also enabled*/

#define MAX30003_CNFG_RTOR_PAVG_DEFAULT       (0x2 << 12) /**< R to R Peak Averaging Weight Factor = 8*/
#define MAX30003_CNFG_RTOR_PAVG_2             (0x0 << 12) /**< R to R Peak Averaging Weight Factor = 2*/
#define MAX30003_CNFG_RTOR_PAVG_4             (0x1 << 12) /**< R to R Peak Averaging Weight Factor = 4*/
#define MAX30003_CNFG_RTOR_PAVG_8             (0x2 << 12) /**< R to R Peak Averaging Weight Factor = 8*/
#define MAX30003_CNFG_RTOR_PAVG_16            (0x3 << 12) /**< R to R Peak Averaging Weight Factor = 16*/

#define MAX30003_CNFG_RTOR_PTSF_SHIFT         8                                      /**< Shift for PTSF bits*/
#define MAX30003_CNFG_RTOR_PTSF_MASK          0xF                                    /**< Mask for PTSF bits*/
#define MAX30003_CNFG_RTOR_PTSF_DEFAULT       (0x3 << MAX30003_CNFG_RTOR_PTSF_SHIFT) /**< Default PTSF value*/

#define MAX30003_CNFG_RTOR2_HOFF_SHIFT        16                                       /**< Shift for PTSF bits*/
#define MAX30003_CNFG_RTOR2_HOFF_MASK         0x3F                                     /**< Mask for PTSF bits*/
#define MAX30003_CNFG_RTOR2_HOFF_DEFAULT      (0x20 << MAX30003_CNFG_RTOR2_HOFF_SHIFT) /**< Default PTSF value*/

#define MAX30003_CNFG_RTOR2_RAVG_DEFAULT       (0x2 << 12) /**< R to R Interval Averaging Weight Factor = 8*/
#define MAX30003_CNFG_RTOR2_RAVG_2             (0x0 << 12) /**< R to R Interval Averaging Weight Factor = 2*/
#define MAX30003_CNFG_RTOR2_RAVG_4             (0x1 << 12) /**< R to R Interval Averaging Weight Factor = 4*/
#define MAX30003_CNFG_RTOR2_RAVG_8             (0x2 << 12) /**< R to R Interval Averaging Weight Factor = 8*/
#define MAX30003_CNFG_RTOR2_RAVG_16            (0x3 << 12) /**< R to R Interval Averaging Weight Factor = 16*/

#define MAX30003_CNFG_RTOR2_RHSF_SHIFT         8                                      /**< Shift for RHSF bits*/
#define MAX30003_CNFG_RTOR2_RHSF_MASK          0x7                                    /**< Mask for RHSF bits*/
#define MAX30003_CNFG_RTOR2_RHSF_DEFAULT       (0x4 << MAX30003_CNFG_RTOR2_RHSF_SHIFT) /**< Default RHSF value*/

#define MAX30003_CNFG_RTOR_DEFAULT_CONFIG      (0x3F2300 << 0) /**< Default CNFG_RTOR register config*/
#define MAX30003_CNFG_RTOR2_DEFAULT_CONFIG     (0x202400 << 0) /**< Default CNFG_RTOR2 register config*/

/************************************************
 * Function Prototypes
 ************************************************/

#ifdef __cplusplus
extern "C" {
#endif

HAL_StatusTypeDef MAX30003_Init(MAX30003_HandleTypeDef *hmax,
                                SPI_HandleTypeDef *hspi,
                                GPIO_TypeDef *cs_port,
                                uint16_t cs_pin);

HAL_StatusTypeDef MAX30003_ReadReg(MAX30003_HandleTypeDef *hmax,
                                    uint8_t reg, uint32_t *data);

HAL_StatusTypeDef MAX30003_WriteReg(MAX30003_HandleTypeDef *hmax,
                                    uint8_t reg, uint32_t data);

HAL_StatusTypeDef MAX30003_ReadFIFO(MAX30003_HandleTypeDef *hmax,
                                    uint32_t *fifo_data, uint8_t count);

uint8_t MAX30003_ExtractETag(uint32_t fifo_data);

uint32_t MAX30003_ExtractECGData(uint32_t fifo_data);

HAL_StatusTypeDef MAX30003_GetInterruptStatus(MAX30003_HandleTypeDef *hmax, 
    uint32_t *enabled_active);

#ifdef __cplusplus
}
#endif

#endif /* INC_MAX30003_H_ */
