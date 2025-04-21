/**
 ******************************************************************************
 * @file    max30003_example.h
 * @author  Wiktor Chocianowicz
 * @brief   MAX30003 Ultra-Low Power Biopotential (ECG) AFE driver example -
 *          Header file
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

#ifndef INC_MAX30003_EXAMPLE_H_
#define INC_MAX30003_EXAMPLE_H_

#include "max30003.h"

void MAX30003_IRQHandler(MAX30003_HandleTypeDef *hmax);

void MAX30003_HandleETag(uint8_t etag, int32_t ecg_sample);

HAL_StatusTypeDef MAX30003_ConfigureRegistersDefault(MAX30003_HandleTypeDef *hmax);

HAL_StatusTypeDef MAX30003_ConfigureRegisters(MAX30003_HandleTypeDef *hmax);

#endif /* INC_MAX30003_EXAMPLE_H_ */
