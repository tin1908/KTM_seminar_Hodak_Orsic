#include "htu21d.h"

extern I2C_HandleTypeDef hi2c1;

#define HTU21D_ADDR     (0x40 << 1)
#define TEMP_MEASURE_CMD 0xE3

float HTU21D_ReadTemperature(void)
{
    uint8_t cmd = TEMP_MEASURE_CMD;
    uint8_t rx[2];
    uint16_t raw;
    float temperature;

    // Send temperature measurement command
    HAL_I2C_Master_Transmit(&hi2c1, HTU21D_ADDR, &cmd, 1, HAL_MAX_DELAY);

    // HTU21D needs time to measure (~50 ms max)
    HAL_Delay(50);

    // Read 2 bytes
    HAL_I2C_Master_Receive(&hi2c1, HTU21D_ADDR, rx, 2, HAL_MAX_DELAY);

    raw = ((uint16_t)rx[0] << 8) | rx[1];

    // Clear status bits
    raw &= 0xFFFC;

    // Convert to temperature
    temperature = -46.85 + (175.72 * raw / 65536.0);

    return temperature;
}
