/**
 * *****************************************************************************
 * @file         mpu6500.c/h
 * @brief        mpu6500
 * @author       ngu
 * @date         20210101
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "mpu6500.h"

/* Private includes ----------------------------------------------------------*/
#include "bsp.h"
#include "cc.h"
#include "ist8310reg.h"
#include "main.h"
#include "mpu6500reg.h"

#include <string.h>

extern SPI_HandleTypeDef hspi5;

/* Private define ------------------------------------------------------------*/

#undef hspi
#define hspi         hspi5
#define MPU_NSS_PIN  GPIO_PIN_6
#define MPU_NSS_PORT GPIOF

/* Private macro -------------------------------------------------------------*/

#if !USED_OS
#define MPU_DELAY osDelay
#else
#define MPU_DELAY delay_ms
#endif /* USED_OS */
#define MPU_NSS_LOW  MPU_NSS_PORT->BSRR = (uint32_t)MPU_NSS_PIN << 16U
#define MPU_NSS_HIGH MPU_NSS_PORT->BSRR = MPU_NSS_PIN

/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

static uint8_t buff_mpu[14]; /*!< buffer to save MPU6500 raw data */
static uint8_t buff_ist[6];  /*!< buffer to save IST8310 raw data */
static uint8_t tx;           /*!< Transmit */
static uint8_t rx;           /*!< Receive */

mpu_t mpu;
imu_t imu;

/* Private user code ---------------------------------------------------------*/

/**
 * @brief        write a byte of data to specified register
 * @param[in]    reg:  the address of register to be written
 * @param[in]    data: the data to be written
*/
static void mpu_write_byte(uint8_t const reg, uint8_t const data)
{
    MPU_NSS_LOW;

    tx = reg & 0x7FU;
    (void)HAL_SPI_TransmitReceive(&hspi, &tx, &rx, 1U, 55U);

    tx = data;
    (void)HAL_SPI_TransmitReceive(&hspi, &tx, &rx, 1U, 55U);

    MPU_NSS_HIGH;
}

/**
 * @brief        read a byte of data from specified register
 * @param[in]    reg: the address of register to be read
 * @return       uint8_t rx
*/
static uint8_t mpu_read_byte(uint8_t const reg)
{
    MPU_NSS_LOW;

    tx = reg | 0x80U;

    (void)HAL_SPI_TransmitReceive(&hspi, &tx, &rx, 1U, 55U);
    (void)HAL_SPI_TransmitReceive(&hspi, &tx, &rx, 1U, 55U);

    MPU_NSS_HIGH;

    return rx;
}

/**
 * @brief        read bytes of data from specified register
 * @param[in]    reg: address from where data is to be written
 * @param[out]   buf: the buffer of receiver
 * @param[in]    len: the length of len
*/
static void mpu_read_bytes(uint8_t const reg, uint8_t *buf, uint8_t len)
{
    MPU_NSS_LOW;

    tx = reg | 0x80U;

    (void)HAL_SPI_TransmitReceive(&hspi, &tx, &rx, 1U, 55U);
    (void)HAL_SPI_TransmitReceive(&hspi, &tx, buf, len, 55U);

    MPU_NSS_HIGH;
}

/**
 * @brief        write IST8310 register through MPU6500's I2C master
 * @param[in]    addr: the address to be written of IST8310's register
 * @param[in]    data: data to be written
*/
static void ist_write_byte(uint8_t addr, uint8_t data)
{
    /*!< turn off slave 1 at first */
    mpu_write_byte(MPU6500_I2C_SLV1_CTRL, 0x00U);
    MPU_DELAY(2U);

    mpu_write_byte(MPU6500_I2C_SLV1_REG, addr);
    MPU_DELAY(2U);

    mpu_write_byte(MPU6500_I2C_SLV1_DO, data);
    MPU_DELAY(2U);

    /*!< turn on slave 1 with one byte transmitting */
    mpu_write_byte(MPU6500_I2C_SLV1_CTRL, 0x80U | 0x01U);

    /*!< wait longer to ensure the data is transmitted from slave 1 */
    MPU_DELAY(10U);
}

/**
 * @brief        write IST8310 register through MPU6500's I2C Master
 * @param[in]    addr: the address to be read of IST8310's register
 * @return       uint8_t
*/
static uint8_t ist_read_byte(uint8_t addr)
{
    uint8_t ret;

    mpu_write_byte(MPU6500_I2C_SLV4_REG, addr);
    MPU_DELAY(10U);

    mpu_write_byte(MPU6500_I2C_SLV4_CTRL, 0x80U);
    MPU_DELAY(10U);

    ret = mpu_read_byte(MPU6500_I2C_SLV4_DI);

    /*!< turn off slave4 after read */
    mpu_write_byte(MPU6500_I2C_SLV4_CTRL, 0x00U);
    MPU_DELAY(10U);

    return ret;
}

/**
 * @brief        initialize the MPU6500 I2C Slave 0 for I2C reading
 * @param[in]    id: slave device address, Address[6:0]
 * @param[in]    reg: the address of base register
 * @param[in]    len: the length of read data
*/
static void mpu_i2c_config(uint8_t id, uint8_t reg, uint8_t len)
{
    /**
     * configure the device address of the IST8310
     * use slave1, auto transmit single measure mode
    */
    mpu_write_byte(MPU6500_I2C_SLV1_ADDR, id);
    MPU_DELAY(2U);

    mpu_write_byte(MPU6500_I2C_SLV1_REG, IST8310_R_CONFA);
    MPU_DELAY(2U);

    mpu_write_byte(MPU6500_I2C_SLV1_DO, IST8310_ODR_MODE);
    MPU_DELAY(2U);

    /*!< use slave0,auto read data */
    mpu_write_byte(MPU6500_I2C_SLV0_ADDR, 0x80U | id);
    MPU_DELAY(2U);

    mpu_write_byte(MPU6500_I2C_SLV0_REG, reg);
    MPU_DELAY(2U);

    /*!< every eight mpu6500 internal samples one i2c master read */
    mpu_write_byte(MPU6500_I2C_SLV4_CTRL, 0x03U);
    MPU_DELAY(2U);

    /*!< enable slave 0 and 1 access delay */
    mpu_write_byte(MPU6500_I2C_MST_DELAY_CTRL, 0x01U | 0x02U);
    MPU_DELAY(2U);

    /*!< enable slave 1 auto transmit */
    mpu_write_byte(MPU6500_I2C_SLV1_CTRL, 0x80U | 0x01U);

    /*!< Wait 6ms (minimum waiting time for 16 times internal average setup) */
    MPU_DELAY(6U);

    /*!< enable slave 0 with len bytes reading */
    mpu_write_byte(MPU6500_I2C_SLV0_CTRL, 0x80U | len);
    MPU_DELAY(2U);
}

/**
 * @brief        Initializes the IST8310 device
*/
uint8_t ist_init(void)
{
    /*!< Enable iic master mode */
    mpu_write_byte(MPU6500_USER_CTRL, 0x30U);
    MPU_DELAY(10U);

    /*!< Enable iic 400khz */
    mpu_write_byte(MPU6500_I2C_MST_CTRL, 0x0DU);
    MPU_DELAY(10U);

    /*!< Turn on slave 1 for ist write and slave 4 to ist read */
    mpu_write_byte(MPU6500_I2C_SLV1_ADDR, IST8310_ADDRESS);
    MPU_DELAY(10U);

    mpu_write_byte(MPU6500_I2C_SLV4_ADDR, 0x80U | IST8310_ADDRESS);
    MPU_DELAY(10U);

    /* IST8310_R_CONFB 0x01 = device rst */
    ist_write_byte(IST8310_R_CONFB, 0x01U);
    MPU_DELAY(10U);

    if (IST8310_DEVICE_ID_A != ist_read_byte(IST8310_WHO_AM_I))
    {
        return 1U;
    }

    /* Soft reset */
    ist_write_byte(IST8310_R_CONFB, 0x01U);
    MPU_DELAY(10U);

    /* Config as ready mode to access register */
    ist_write_byte(IST8310_R_CONFA, 0x00U);
    if (ist_read_byte(IST8310_R_CONFA) != 0x00U)
    {
        return 2U;
    }
    MPU_DELAY(10U);

    /* Normal state, no int */
    ist_write_byte(IST8310_R_CONFB, 0x00U);
    if (ist_read_byte(IST8310_R_CONFB) != 0x00U)
    {
        return 3U;
    }
    MPU_DELAY(10U);

    /* Config low noise mode, x,y,z axis 16 time 1 avg */
    ist_write_byte(IST8310_AVGCNTL, 0x24U); /* 100100 */
    if (ist_read_byte(IST8310_AVGCNTL) != 0x24U)
    {
        return 4U;
    }
    MPU_DELAY(10U);

    /* Set/Reset pulse duration setup,normal mode */
    ist_write_byte(IST8310_PDCNTL, 0xC0U);
    if (ist_read_byte(IST8310_PDCNTL) != 0xC0U)
    {
        return 5U;
    }
    MPU_DELAY(10U);

    /* Turn off slave1 & slave 4 */
    mpu_write_byte(MPU6500_I2C_SLV1_CTRL, 0x00U);
    MPU_DELAY(10U);

    mpu_write_byte(MPU6500_I2C_SLV4_CTRL, 0x00U);
    MPU_DELAY(10U);

    /* Configure and turn on slave 0 */
    mpu_i2c_config(IST8310_ADDRESS, IST8310_R_XL, 0x06U);
    MPU_DELAY(100U);

    return 0U;
}

/**
 * @brief        get the data of IST8310
 * @param[out]   buff the buffer to save the data of IST8310
*/
void ist_data_update(uint8_t *buff)
{
    mpu_read_bytes(MPU6500_EXT_SENS_DATA_00, buff, 6U);
}

/**
 * @brief        get the data of imu
*/
void mpu_data_update(void)
{
    mpu_read_bytes(MPU6500_ACCEL_XOUT_H, buff_mpu, 14U);

    mpu.ax = buff_mpu[0] << 8 | buff_mpu[1];
    mpu.ay = buff_mpu[2] << 8 | buff_mpu[3];
    mpu.az = buff_mpu[4] << 8 | buff_mpu[5];

    mpu.temp = buff_mpu[6] << 8 | buff_mpu[7];

    mpu.gx = ((buff_mpu[8] << 8 | buff_mpu[9]) - mpu.gx_offset);
    mpu.gy = ((buff_mpu[10] << 8 | buff_mpu[11]) - mpu.gy_offset);
    mpu.gz = ((buff_mpu[12] << 8 | buff_mpu[13]) - mpu.gz_offset);

    ist_data_update(buff_ist);

    memcpy(&mpu.mx, buff_ist, 6U);

    memcpy(&imu.ax, &mpu.ax, 6U * sizeof(int16_t));

    imu.temp = 21 + mpu.temp / 333.87f;
    /* 2000dps -> rad/s */
    imu.wx = mpu.gx / 16.384f / 57.3f;
    imu.wy = mpu.gy / 16.384f / 57.3f;
    imu.wz = mpu.gz / 16.384f / 57.3f;
}

/**
 * @brief        set imu 6500 gyroscope measure range
 * @param[in]    fsr: range(0,±250dps;1,±500dps;2,±1000dps;3,±2000dps)
*/
static void mpu_set_gyro_fsr(uint8_t fsr)
{
    mpu_write_byte(MPU6500_GYRO_CONFIG, fsr << 3U);
}

/**
 * @brief        set imu 6050/6500 accelerate measure range
 * @param        fsr: range(0,±2g;1,±4g;2,±8g;3,±16g)
*/
static void mpu_set_accel_fsr(uint8_t fsr)
{
    mpu_write_byte(MPU6500_ACCEL_CONFIG, fsr << 3U);
}

/**
 * @brief        get the offset data of MPU6500
*/
void mpu_offset_call(void)
{
    for (int i = 0; i < 300; i++)
    {
        mpu_read_bytes(MPU6500_ACCEL_XOUT_H, buff_mpu, 14U);

        mpu.ax_offset += buff_mpu[0] << 8 | buff_mpu[1];
        mpu.ay_offset += buff_mpu[2] << 8 | buff_mpu[3];
        mpu.az_offset += buff_mpu[4] << 8 | buff_mpu[5];

        mpu.gx_offset += buff_mpu[8] << 8 | buff_mpu[9];
        mpu.gy_offset += buff_mpu[10] << 8 | buff_mpu[11];
        mpu.gz_offset += buff_mpu[12] << 8 | buff_mpu[13];

        MPU_DELAY(5);
    }

    mpu.ax_offset = mpu.ax_offset / 300;
    mpu.ay_offset = mpu.ay_offset / 300;
    mpu.az_offset = mpu.az_offset / 300;
    mpu.gx_offset = mpu.gx_offset / 300;
    mpu.gy_offset = mpu.gx_offset / 300;
    mpu.gz_offset = mpu.gz_offset / 300;
}

/**
 * @brief        initialize imu mpu6500 and magnet meter ist3810
*/
void mpu_device_init(void)
{
    MPU_DELAY(100);

    (void)mpu_read_byte(MPU6500_WHO_AM_I);

    uint8_t data[10][2] = {
        {MPU6500_PWR_MGMT_1, 0x80},     /* Reset Device */
        {MPU6500_PWR_MGMT_1, 0x03},     /* Clock Source - Gyro-Z */
        {MPU6500_PWR_MGMT_2, 0x00},     /* Enable Acc & Gyro */
        {MPU6500_CONFIG, 0x04},         /* LPF 41Hz */
        {MPU6500_GYRO_CONFIG, 0x18},    /* +-2000dps */
        {MPU6500_ACCEL_CONFIG, 0x10},   /* +-8G */
        {MPU6500_ACCEL_CONFIG_2, 0x02}, /* Enable LowPassFilter  Set Acc LPF */
        {MPU6500_USER_CTRL, 0x20},
    }; /* Enable AUX */

    for (uint8_t i = 0; i < 10; i++)
    {
        mpu_write_byte(data[i][0], data[i][1]);
        MPU_DELAY(1U);
    }

    mpu_set_gyro_fsr(3U);
    mpu_set_accel_fsr(2U);

    ist_init();
    mpu_offset_call();
}

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
