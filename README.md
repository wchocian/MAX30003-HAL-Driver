# MAX30003-HAL-Driver

A Hardware Abstraction Layer (HAL) driver for the MAX30003 ultra-low power
bio-sensor module (ECG and pacemaker detection), designed for STM32
microcontrollers using the STM32 HAL framework.

## Overview

This repository provides a platform-independent HAL driver for interfacing with
the MAX30003 bio-sensor module through SPI communication. The driver abstracts
low-level register operations and provides easy-to-use APIs for integrating ECG
and bioelectrical signal monitoring capabilities into STM32-based projects.

## Datasheet

The library was created based off of the datasheet below.

[MAX30003 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/max30003.pdf)

## Features

- Full register map implementation.
- SPI interface abstraction.
- Platform-independent design (easily portable to other MCUs).
- Example `.c` and `.h` files.

## Installation

1. Clone the repository:

```bash
git clone https://github.com/wchocian/MAX30003-HAL-Driver.git
```

2. Move source and header files to your project's directory.

```bash
cp MAX30003-HAL-Driver/max30003*.c Project-Path-Src/
cp MAX30003-HAL-Driver/max30003*.h Project-Path-Inc/
```

3. Include header file in your project:

```c
#include "max30003.h"
```

## Usage

Typically you would start by creating a `MAX30003_HandleTypeDef` variable and
assigning it values using `MAX30003_Init()`.

```c
MAX30003_HandleTypeDef hmax;

MAX30003_Init(&hmax, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin); /// Initialization of MAX30003
```

where

```c
HAL_StatusTypeDef MAX30003_Init(MAX30003_HandleTypeDef *hmax,
        SPI_HandleTypeDef *hspi,
        GPIO_TypeDef *cs_port,
        uint16_t cs_pin);
```

Then you can configure registers like so:

```c
MAX30003_WriteReg(hmax, MAX30003_REG_EN_INT, MAX30003_EN_INT_DEFAULT_CONFIG
        | MAX30003_EN_INT_EINT_EN
        | MAX30003_EN_INT_EOVF_EN
        | MAX30003_EN_INT_FSTINT_DIS
        | MAX30003_EN_INT_DCLOFFINT_DIS
        | MAX30003_EN_INT_LONINT_DIS
        | MAX30003_EN_INT_RRINT_DIS
        | MAX30003_EN_INT_SAMP_DIS
        | MAX30003_EN_INT_PLLINT_DIS
        | MAX30003_EN_INT_INTB_TYPE_OPEN_DRAIN_125K_PULLUP);
```

where

```c
HAL_StatusTypeDef MAX30003_WriteReg(MAX30003_HandleTypeDef *hmax,
                                    uint8_t reg, uint32_t data);
```

Then you can use `MAX30003_ReadReg()` and `MAX30003_ReadFIFO()` to further obrain data and read registers.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements
- STMicroelectronics for STM32 HAL framework.
