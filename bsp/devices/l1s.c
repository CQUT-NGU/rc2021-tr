/**
 * *****************************************************************************
 * @file         bsp_l1s.c
 * @brief        l1s of boards
 * @author       NGU
 * @date         20210707
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "l1s.h"

l1s_t l1s;

uint8_t l1s0_buf8[2][BUFSIZ_L1S0];
uint8_t *l1s0_buf = *l1s0_buf8;
uint8_t l1s1_buf8[2][BUFSIZ_L1S1];
uint8_t *l1s1_buf = *l1s1_buf8;
// uint8_t l1s2_buf8[2][BUFSIZ_L1S2];
// uint8_t *l1s2_buf = *l1s2_buf8;
// uint8_t l1s3_buf8[2][BUFSIZ_L1S3];
// uint8_t *l1s3_buf = *l1s3_buf8;

static uint8_t l1s_send_start[] = {
    0xA5,
    0x5A,
    0x04,
    0x00,
    0xFB,
};

static uint8_t l1s_send_stop[] = {
    0xA5,
    0x5A,
    0x05,
    0x00,
    0xFA,
};

static uint8_t BCC(uint8_t *dat, uint16_t len)
{
    uint8_t bcc = 0;
    uint8_t *end = dat + len;
    while (dat != end)
    {
        bcc ^= *dat++;
    }
    return bcc;
}

void l1s_dma_init(UART_HandleTypeDef *huart)
{
    /* DMA Enable Receiver */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    /* Enable UART */
    __HAL_UART_ENABLE(huart);
    /* Enalbe idle interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

    do
    {
        /* Disable the specified DMA Stream */
        __HAL_DMA_DISABLE(huart->hdmarx);
    } while (huart->hdmarx->Instance->CR & DMA_SxCR_EN);
    /* Clear all flag */
    BSP_DMA_CLEAR_FLAG(huart->hdmarx);

    /* DMA stream x peripheral address register */
    huart->hdmarx->Instance->PAR = (uint32_t) & (huart->Instance->DR);
}

void l1s_config(UART_HandleTypeDef *huart)
{
    HAL_UART_Transmit(huart, (void *)"iSET:1,0", sizeof("iSET:1,0") - 1, 0xFFFF);
    HAL_UART_Transmit(huart, (void *)"iSET:2,40", sizeof("iSET:2,40") - 1, 0xFFFF);
    HAL_UART_Transmit(huart, (void *)"iSET:4,115200", sizeof("iSET:4,115200") - 1, 0xFFFF);
    HAL_UART_Transmit(huart, (void *)"iSET:7,20", sizeof("iSET:7,20") - 1, 0xFFFF);
    HAL_UART_Transmit(huart, (void *)"iSET:8,0", sizeof("iSET:8,0") - 1, 0xFFFF);
}

void l1s_start(UART_HandleTypeDef *huart)
{
    HAL_UART_Transmit(huart, l1s_send_start, sizeof(l1s_send_start), 0xFFFF);
}

void l1s_stop(UART_HandleTypeDef *huart)
{
    HAL_UART_Transmit(huart, l1s_send_stop, sizeof(l1s_send_stop), 0xFFFF);
}

void l1s_init(void)
{
    l1s_config(&huart_l1s0);
    l1s_config(&huart_l1s1);
    // l1s_config(&huart_l1s2);
    // l1s_config(&huart_l1s3);

    l1s_dma_init(&huart_l1s0);
    l1s_dma_init(&huart_l1s1);
    // l1s_dma_init(&huart_l1s2);
    // l1s_dma_init(&huart_l1s3);

    usart_dma_rx(&huart_l1s0, l1s0_buf8[0] + 1, l1s0_buf8[1] + 1, BUFSIZ_L1S0 - 1);
    usart_dma_rx(&huart_l1s1, l1s1_buf8[0] + 1, l1s1_buf8[1] + 1, BUFSIZ_L1S1 - 1);
    // usart_dma_rx(&huart_l1s2, l1s2_buf8[0] + 1, l1s2_buf8[1] + 1, BUFSIZ_L1S2 - 1);
    // usart_dma_rx(&huart_l1s3, l1s3_buf8[0] + 1, l1s3_buf8[1] + 1, BUFSIZ_L1S3 - 1);

    l1s_start(&huart_l1s0);
    l1s_start(&huart_l1s1);
    // l1s_start(&huart_l1s2);
    // l1s_start(&huart_l1s3);
}

static void l1s_irq(l1s_dis_t *dis, l1s_e flag, uint8_t *buf, uint8_t len)
{
    if (len < 8)
    {
        return;
    }

    if (buf[0] == 0xB4 && buf[1] == 0x69 && BCC(buf, 7) == buf[7])
    {
        dis->data = 0;
        dis->error = 0;
        if (buf[2] & 0x80)
        {
            dis->error |= buf[5];
            dis->error <<= 8;
            dis->error |= buf[6];
            l1s.flag = (int8_t)(l1s.flag & ~flag);
        }
        else
        {
            dis->data |= buf[3];
            dis->data <<= 8;
            dis->data |= buf[4];
            dis->data <<= 8;
            dis->data |= buf[5];
            dis->data <<= 8;
            dis->data |= buf[6];
            l1s.flag = (int8_t)(l1s.flag | flag);
        }
    }
}

/**
 * @brief This function handles USART global interrupt.
*/
void L1S0_IRQHandler(void)
{
    if (huart_l1s0.Instance->SR & UART_FLAG_RXNE) /*!< USART Status register*/
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s0); /* Clears the UART PE pending l1s0_flag */
    }
    else if (huart_l1s0.Instance->SR & UART_FLAG_IDLE)
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s0); /* Clears the UART PE pending l1s0_flag */

        /* Disable the specified DMA Stream */
        __HAL_DMA_DISABLE(huart_l1s0.hdmarx);

        /* DMA stream x configuration register */
        if ((huart_l1s0.hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s0_buf = l1s0_buf8[0];
            l1s0_buf[0] = (uint8_t)(BUFSIZ_L1S0 - 1 - huart_l1s0.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s0.hdmarx->Instance->NDTR = BUFSIZ_L1S0 - 1;

            /* Set memory buffer 1 */
            huart_l1s0.hdmarx->Instance->CR |= DMA_SxCR_CT;

            /* Enable the specified DMA Stream */
            __HAL_DMA_ENABLE(huart_l1s0.hdmarx);
        }
        else
        {
            /* Current memory buffer used is Memory 1 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s0_buf = l1s0_buf8[1];
            l1s0_buf[0] = (uint8_t)(BUFSIZ_L1S0 - 1 - huart_l1s0.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s0.hdmarx->Instance->NDTR = BUFSIZ_L1S0 - 1;

            /* Set memory buffer 0 */
            huart_l1s0.hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
        }

        /* Enable the specified DMA Stream */
        __HAL_DMA_ENABLE(huart_l1s0.hdmarx);

        l1s_irq(&l1s.dis0, FLAG_L1S0, l1s0_buf + 1, *l1s0_buf);
    }
    else if (huart_l1s0.Instance->SR & UART_FLAG_TC)
    {
        __HAL_UART_CLEAR_FLAG(&huart_l1s0, UART_FLAG_TC);
    }
}

/**
 * @brief This function handles USART global interrupt.
*/
void L1S1_IRQHandler(void)
{
    if (huart_l1s1.Instance->SR & UART_FLAG_RXNE) /*!< USART Status register*/
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s1); /* Clears the UART PE pending l1s1_flag */
    }
    else if (huart_l1s1.Instance->SR & UART_FLAG_IDLE)
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s1); /* Clears the UART PE pending l1s1_flag */

        /* Disable the specified DMA Stream */
        __HAL_DMA_DISABLE(huart_l1s1.hdmarx);

        /* DMA stream x configuration register */
        if ((huart_l1s1.hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s1_buf = l1s1_buf8[0];
            l1s1_buf[0] = (uint8_t)(BUFSIZ_L1S1 - 1 - huart_l1s1.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s1.hdmarx->Instance->NDTR = BUFSIZ_L1S1 - 1;

            /* Set memory buffer 1 */
            huart_l1s1.hdmarx->Instance->CR |= DMA_SxCR_CT;

            /* Enable the specified DMA Stream */
            __HAL_DMA_ENABLE(huart_l1s1.hdmarx);
        }
        else
        {
            /* Current memory buffer used is Memory 1 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s1_buf = l1s1_buf8[1];
            l1s1_buf[0] = (uint8_t)(BUFSIZ_L1S1 - 1 - huart_l1s1.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s1.hdmarx->Instance->NDTR = BUFSIZ_L1S1 - 1;

            /* Set memory buffer 0 */
            huart_l1s1.hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
        }

        /* Enable the specified DMA Stream */
        __HAL_DMA_ENABLE(huart_l1s1.hdmarx);

        l1s_irq(&l1s.dis1, FLAG_L1S1, l1s1_buf + 1, *l1s1_buf);
    }
    else if (huart_l1s1.Instance->SR & UART_FLAG_TC)
    {
        __HAL_UART_CLEAR_FLAG(&huart_l1s1, UART_FLAG_TC);
    }
}

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
