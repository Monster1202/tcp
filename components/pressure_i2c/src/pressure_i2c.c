#include <stdbool.h>
#include <sys/param.h>
#include "esp_check.h"
#include "esp_log.h"
#include "pressure_i2c.h"
#include "driver/i2c.h"
#include "para_list.h"



static const char *TAG = "i2c-test";

// #define I2C_MASTER_SCL_IO           1//10      /*!< GPIO number used for I2C master clock */
// #define I2C_MASTER_SDA_IO           2//11      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define SENSOR_ADDR                 0x01        /*!< Slave address of the sensor */
#define REG_ADDR                    0x22       /*!< Register addresses of the register */

// #define PWR_MGMT_1_REG_ADDR         0x6B        /*!< Register addresses of the power managment register */
// #define RESET_BIT                   7

/**
 * @brief Read a sequence of bytes from a MPU9250 sensor registers
 */
static esp_err_t register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

/**
 * @brief Write a byte to a MPU9250 sensor register
 */
static esp_err_t register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);

    return ret;
}

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void pressure_read(void* arg)
{
    uint8_t data[2];
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");
    uint16_t pressure = 0;
    esp_err_t err_print = 0;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    while(1)
    {
        //2 in 8 registers 
        data[0]=0;
        data[1]=0;
        err_print = register_read(REG_ADDR, data, 2);
        if(err_print == -1)
            ESP_LOGI(TAG, "i2c_register_read_err_print: %d", err_print);
        //ESP_ERROR_CHECK(register_read(REG_ADDR, data, 2));
        pressure = data[0]+(uint16_t)(data[1]<<8);
        ESP_LOGI(TAG, "pressure = %d kpa", pressure);
        parameter_write_pressure(pressure);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    /* Demonstrate writing by reseting the MPU9250 */
    // ESP_ERROR_CHECK(register_write_byte(PWR_MGMT_1_REG_ADDR, 1 << RESET_BIT));

    // ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    // ESP_LOGI(TAG, "I2C unitialized successfully");
}

