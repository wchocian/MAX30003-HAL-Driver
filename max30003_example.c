/**
 ******************************************************************************
 * @file    max30003_example.c
 * @author  Wiktor Chocianowicz
 * @brief   MAX30003 Ultra-Low Power Biopotential (ECG) AFE driver example
 * 			functions - Source file
 * @note
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

#include "max30003.h"

/**
 * @brief  Handles the interrupts from MAX30003
 * @param hmax Pointer to MAX30003 handle
 */
void MAX30003_IRQHandler(MAX30003_HandleTypeDef *hmax) {
    uint32_t enabled_active;

    // 1. Read critical registers first
    MAX30003_GetInterruptStatus(hmax, &enabled_active);

    // 2. Handle enabled interrupts
    if(enabled_active & MAX30003_INT_EINT) MAX30003_ReadFIFO(hmax, NULL, MAX30003_FIFO_LENGTH); // Read FIFO to clear interrupt
    if(enabled_active & MAX30003_INT_EOVF) MAX30003_WriteReg(hmax, MAX30003_REG_FIFO_RST, MAX30003_FIFO_RST_D); // Clear FIFO overflow
    if(enabled_active & MAX30003_INT_FSTINT) {
    }
    if(enabled_active & MAX30003_INT_DCLOFFINT) {
    }
    if(enabled_active & MAX30003_INT_LONINT) {
    }
    if(enabled_active & MAX30003_INT_RRINT) {
    }
    if(enabled_active & MAX30003_INT_SAMP) {
    }
    if(enabled_active & MAX30003_INT_PLLINT) {
    }

    // 3. Optional: Log raw status for diagnostics
    // LogInterrupts(raw);
}

/**
 * @brief Handle ECG sample based on ETAG.
 * @param etag ETAG extracted from FIFO word.
 * @param ecg_sample ECG voltage data.
 */
void MAX30003_HandleETag(uint8_t etag, int32_t ecg_sample) {
    switch (etag) {
        case MAX30003_FIFO_ETAG_VALID:
            // Process valid ECG sample
            break;
        case MAX30003_FIFO_ETAG_FAST:
            // Fast mode (skip)
            break;
        case MAX30003_FIFO_ETAG_VALID_EOF:
            // Last sample (EOF)
            break;
        case MAX30003_FIFO_ETAG_OVERFLOW:
            // Handle overflow condition
            break;
        default:
            // Reserved/unknown ETAG
            break;
    }
}
/** 
 * @brief  Configure MAX30003 registers with default values
 * @param  hmax Pointer to MAX30003 handle
 * @retval HAL status
 */
HAL_StatusTypeDef MAX30003_ConfigureRegistersDefault(MAX30003_HandleTypeDef *hmax) {
    HAL_StatusTypeDef ret;
    
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_EN_INT, MAX30003_EN_INT_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_EN_INT2, MAX30003_EN_INT_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_MNGR_INT, MAX30003_MNGR_INT_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_MNGR_DYN, MAX30003_MNGR_DYN_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_GEN, MAX30003_CNFG_GEN_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_CAL, MAX30003_CNFG_CAL_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_EMUX, MAX30003_CNFG_EMUX_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_ECG, MAX30003_CNFG_ECG_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_RTOR1, MAX30003_CNFG_RTOR_DEFAULT_CONFIG)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_RTOR2, MAX30003_CNFG_RTOR2_DEFAULT_CONFIG)) != HAL_OK) return ret;

    return HAL_OK;
}

/**
 * @brief  Configure MAX30003 registers with custom values
 * @param  hmax Pointer to MAX30003 handle
 * @retval HAL status
 */
HAL_StatusTypeDef MAX30003_ConfigureRegisters(MAX30003_HandleTypeDef *hmax) {
    HAL_StatusTypeDef ret;

    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_EN_INT, MAX30003_EN_INT_DEFAULT_CONFIG
        | MAX30003_EN_INT_EINT_EN
        | MAX30003_EN_INT_EOVF_EN
        | MAX30003_EN_INT_FSTINT_DIS
        | MAX30003_EN_INT_DCLOFFINT_DIS
        | MAX30003_EN_INT_LONINT_DIS
        | MAX30003_EN_INT_RRINT_DIS
        | MAX30003_EN_INT_SAMP_DIS
        | MAX30003_EN_INT_PLLINT_DIS
        | MAX30003_EN_INT_INTB_TYPE_OPEN_DRAIN_125K_PULLUP)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_EN_INT2, MAX30003_EN_INT_DEFAULT_CONFIG
        | MAX30003_EN_INT_EINT_EN
        | MAX30003_EN_INT_EOVF_EN
        | MAX30003_EN_INT_FSTINT_DIS
        | MAX30003_EN_INT_DCLOFFINT_DIS
        | MAX30003_EN_INT_LONINT_DIS
        | MAX30003_EN_INT_RRINT_DIS
        | MAX30003_EN_INT_SAMP_DIS
        | MAX30003_EN_INT_PLLINT_DIS
        | MAX30003_EN_INT_INTB_TYPE_OPEN_DRAIN_125K_PULLUP)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_MNGR_INT, MAX30003_MNGR_INT_DEFAULT_CONFIG
        | MAX30003_MNGR_INT_EFIT_32 
        | MAX30003_MNGR_INT_CLR_FAST_DIS
        | MAX30003_MNGR_INT_CLR_RRINT_ON_STATUS_REGISTER_READ_BACK
        | MAX30003_MNGR_INT_CLR_SAMP_SELF_CLEAR
        | MAX30003_MNGR_INT_SAMP_IT_EVERY_SAMPLE)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_MNGR_DYN, MAX30003_MNGR_DYN_DEFAULT_CONFIG
        | MAX30003_MNGR_DYN_FAST_NORMAL_MODE
        | (0x3F << MAX30003_MNGR_DYN_FAST_TH_SHIFT))) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_GEN, MAX30003_CNFG_GEN_DEFAULT_CONFIG
        | MAX30003_CNFG_GEN_EN_ULP_LON_DIS
        | MAX30003_CNFG_GEN_FMSTR_512HZ_ECG_PROGGRESION
        | MAX30003_CNFG_GEN_EN_ECG_EN
        | MAX30003_CNFG_GEN_EN_DCLOFF_DIS
        | MAX30003_CNFG_GEN_DCLOFF_IPOL_ECGP_PULLUP
        | MAX30003_CNFG_GEN_DCLOFF_IMAG_0nA
        | MAX30003_CNFG_GEN_DCLOFF_VTH_VMID_PM_300
        | MAX30003_CNFG_GEN_EN_RBIAS_DIS
        | MAX30003_CNFG_GEN_RBIASV_100M
        | MAX30003_CNFG_GEN_RBIASP_DIS
        | MAX30003_CNFG_GEN_RBIASN_DIS)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_CAL, MAX30003_CNFG_CAL_DEFAULT_CONFIG
        | MAX30003_CNFG_CAL_EN_VCAL_DIS
        | MAX30003_CNFG_CAL_VMODE_UNIPOLAR
        | MAX30003_CNFG_CAL_VMAG_0_25mV
        | MAX30003_CNFG_CAL_FCAL_1Hz
        | MAX30003_CNFG_CAL_FIFTY_DUTY_50
        | (0x000 << MAX30003_CNFG_CAL_THIGH_SHIFT))) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_EMUX, MAX30003_CNFG_EMUX_DEFAULT_CONFIG
        | MAX30003_CNFG_EMUX_POL_NON_INVERTED
        | MAX30003_CNFG_EMUX_OPENP_INTERNALLY_ISOLATED
        | MAX30003_CNFG_EMUX_OPENN_INTERNALLY_ISOLATED
        | MAX30003_CNFG_EMUX_CALP_SEL_NONE
        | MAX30003_CNFG_EMUX_CALN_SEL_NONE)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_ECG, MAX30003_CNFG_ECG_DEFAULT_CONFIG
        | MAX30003_CNFG_ECG_RATE_512
        | MAX30003_CNFG_ECG_GAIN_80
        | MAX30003_CNFG_ECG_DHPF_EN
        | MAX30003_CNFG_ECG_DLPF_40)) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_RTOR1, MAX30003_CNFG_RTOR_DEFAULT_CONFIG
        | MAX30003_CNFG_RTOR_eWNDW_12
        | MAX30003_CNFG_RTOR_GAIN_DEFAULT
        | MAX30003_CNFG_RTOR_EN_RTOR_DIS
        | MAX30003_CNFG_RTOR_PAVG_DEFAULT
        | (0x3 << MAX30003_CNFG_RTOR_PTSF_SHIFT))) != HAL_OK) return ret;
    if((ret = MAX30003_WriteReg(hmax, MAX30003_REG_CNFG_RTOR2, MAX30003_CNFG_RTOR2_DEFAULT_CONFIG
        | (0x20 << MAX30003_CNFG_RTOR2_HOFF_SHIFT)
        | MAX30003_CNFG_RTOR2_RAVG_DEFAULT
        | (0x4 << MAX30003_CNFG_RTOR2_RHSF_SHIFT))) != HAL_OK) return ret;

    return HAL_OK;
}
