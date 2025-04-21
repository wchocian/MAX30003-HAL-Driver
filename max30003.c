/**
 ******************************************************************************
 * @file    max30003.c
 * @author  Wiktor Chocianowicz
 * @brief   MAX30003 Ultra-Low Power Biopotential (ECG) AFE driver - Source file
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

#include "max30003.h"

/**
 * @brief Extract ETAG from 24-bit FIFO word.
 * @param fifo_data Raw FIFO 24-bit word.
 * @return ETAG value (0–7).
 */
uint8_t MAX30003_ExtractETag(uint32_t fifo_data) {
    return (fifo_data >> MAX30003_ETAG_SHIFT) & MAX30003_ETAG_MASK;
}

/**
 * @brief Extract ECG voltage sample from FIFO word.
 * @param fifo_data Raw FIFO 24-bit word.
 * @return ECG voltage (18-bit).
 */
uint32_t MAX30003_ExtractECGData(uint32_t fifo_data) {
    return (fifo_data >> MAX30003_ECG_VOLTAGE_DATA_SHIFT) & MAX30003_ECG_VOLTAGE_DATA_MASK;
}

/**
 * @brief Initialize MAX30003 communication handle.
 * @param hmax Pointer to device handle.
 * @param hspi SPI handle.
 * @param cs_port GPIO port for CS pin.
 * @param cs_pin GPIO pin for CS.
 * @return HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef MAX30003_Init(MAX30003_HandleTypeDef *hmax,
                                SPI_HandleTypeDef *hspi,
                                GPIO_TypeDef *cs_port,
                                uint16_t cs_pin) {
    if (hspi == NULL)
        return HAL_ERROR;

    hmax->hspi = hspi;
    hmax->cs_port = cs_port;
    hmax->cs_pin = cs_pin;

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
    return HAL_OK;
}

/**
 * @brief Perform SPI transmit/receive operation.
 * @param hmax Device handle.
 * @param tx_data Pointer to data to transmit.
 * @param rx_data Pointer to buffer for received data.
 * @param size Number of bytes to exchange.
 * @return HAL_OK on success.
 */
static HAL_StatusTypeDef MAX30003_SPI_TransmitReceive(MAX30003_HandleTypeDef *hmax,
                                                    uint8_t *tx_data,
                                                    uint8_t *rx_data,
                                                    uint16_t size) {
    HAL_GPIO_WritePin(hmax->cs_port, hmax->cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(hmax->hspi, tx_data, rx_data, size, MAX30003_SPI_TIMEOUT);
    HAL_GPIO_WritePin(hmax->cs_port, hmax->cs_pin, GPIO_PIN_SET);
    return status;
}

/**
 * @brief Read 24-bit register value.
 * @param hmax Device handle.
 * @param reg Register address.
 * @param data Pointer to output value.
 * @return HAL_OK on success.
 */
HAL_StatusTypeDef MAX30003_ReadReg(MAX30003_HandleTypeDef *hmax,
                                uint8_t reg, uint32_t *data) {
    uint8_t tx_buf[4] = { (reg << 1) | 0x01 };
    uint8_t rx_buf[4] = {0};

    HAL_StatusTypeDef status = MAX30003_SPI_TransmitReceive(hmax, tx_buf, rx_buf, 4);

    if (status == HAL_OK) {
        *data = ((uint32_t)rx_buf[1] << 16) |
                ((uint32_t)rx_buf[2] << 8) |
                rx_buf[3];
    }

    return status;
}

/**
 * @brief Write 24-bit value to a register.
 * @param hmax Device handle.
 * @param reg Register address.
 * @param data 24-bit data to write.
 * @return HAL_OK on success.
 */
HAL_StatusTypeDef MAX30003_WriteReg(MAX30003_HandleTypeDef *hmax,
                                    uint8_t reg, uint32_t data) {
    uint8_t tx_buf[4] = {
        (reg << 1) & 0xFE,
        (data >> 16) & 0xFF,
        (data >> 8) & 0xFF,
        data & 0xFF
    };

    HAL_GPIO_WritePin(hmax->cs_port, hmax->cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(hmax->hspi, tx_buf, 4, MAX30003_SPI_TIMEOUT);
    HAL_GPIO_WritePin(hmax->cs_port, hmax->cs_pin, GPIO_PIN_SET);

    return status;
}

/**
 * @brief Read ECG samples from FIFO.
 * @param hmax Device handle.
 * @param fifo_data Output buffer for FIFO data.
 * @param count Number of samples to read.
 * @return HAL_OK on success.
 */
HAL_StatusTypeDef MAX30003_ReadFIFO(MAX30003_HandleTypeDef *hmax,
                                    uint32_t *fifo_data, uint8_t count) {
    uint8_t tx_buf[4] = {0};
    uint8_t rx_buf[4] = {0};
    HAL_StatusTypeDef status = HAL_OK;

    tx_buf[0] = ((count > 1 ? MAX30003_FIFO_CMD_ECG_BURST : MAX30003_FIFO_CMD_ECG) << 1) | 0x01;

    for (uint8_t i = 0; i < count && status == HAL_OK; ++i) {
        status = MAX30003_SPI_TransmitReceive(hmax, tx_buf, rx_buf, 4);
        fifo_data[i] = ((uint32_t)rx_buf[1] << 16) |
                    ((uint32_t)rx_buf[2] << 8) |
                    rx_buf[3];
        if (i == 0) tx_buf[0] = 0x00;  // Continue burst without command byte
    }

    return status;
}

/**
 * @brief  Gets enabled+active interrupts
 * @param  hmax Pointer to MAX30003 handle
 * @param[out] enabled_active Interrupts that are both enabled and active
 * @retval HAL status
 */
HAL_StatusTypeDef MAX30003_GetInterruptStatus(
    MAX30003_HandleTypeDef *hmax,
    uint32_t *enabled_active
) {
    uint32_t raw_status = 0;
    uint32_t en_int_reg = 0;
    HAL_StatusTypeDef ret;
    
    // Read STATUS (0x01) and EN_INT (0x02)
    if((ret = MAX30003_ReadReg(hmax, MAX30003_REG_STATUS, &raw_status)) != HAL_OK) return ret;
    if((ret = MAX30003_ReadReg(hmax, MAX30003_REG_EN_INT, &en_int_reg)) != HAL_OK) return ret;

    // Mask STATUS with EN_INT to get enabled+active interrupts
    *enabled_active = raw_status & en_int_reg & 0xF00F00;
    
    return HAL_OK;
}

