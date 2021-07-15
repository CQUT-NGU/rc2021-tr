/**
 * *****************************************************************************
 * @file         ctrl_l1s.c
 * @brief        contrl l1s
 * @author       NGU
 * @date         20210707
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

#include "ctrl_l1s.h"

#if L1S_TOTAL > 0
extern UART_HandleTypeDef huart_l1s0;
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
extern UART_HandleTypeDef huart_l1s1;
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
extern UART_HandleTypeDef huart_l1s2;
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
extern UART_HandleTypeDef huart_l1s3;
#endif /* L1S_TOTAL */

l1s_t l1s = {
#if L1S_TOTAL > 0
    .dis0.error = L1S_ERROR_NORUN,
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    .dis1.error = L1S_ERROR_NORUN,
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    .dis2.error = L1S_ERROR_NORUN,
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    .dis3.error = L1S_ERROR_NORUN,
#endif /* L1S_TOTAL */
};

#if L1S_TOTAL > 0
uint8_t l1s0_buf8[2][BUFSIZ_L1S0];
uint8_t *l1s0_buf = *l1s0_buf8;
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
uint8_t l1s1_buf8[2][BUFSIZ_L1S1];
uint8_t *l1s1_buf = *l1s1_buf8;
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
uint8_t l1s2_buf8[2][BUFSIZ_L1S2];
uint8_t *l1s2_buf = *l1s2_buf8;
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
uint8_t l1s3_buf8[2][BUFSIZ_L1S3];
uint8_t *l1s3_buf = *l1s3_buf8;
#endif /* L1S_TOTAL */

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

static void l1s_irq(l1s_dis_t *dis, int8_t flag, uint8_t *buf, uint8_t len)
{
    if (len < 8)
    {
        return;
    }

    if (buf[0] == 0xB4 && buf[1] == 0x69 && BCC(buf, 7) == buf[7])
    {
        dis->raw = 0;
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
            dis->raw |= buf[3];
            dis->raw <<= 8;
            dis->raw |= buf[4];
            dis->raw <<= 8;
            dis->raw |= buf[5];
            dis->raw <<= 8;
            dis->raw |= buf[6];
            l1s.flag = (int8_t)(l1s.flag | flag);
        }
        dis->data = (int32_t)(dis->raw - dis->base);
    }
}

void l1s_config(UART_HandleTypeDef *huart)
{
    HAL_UART_Transmit(huart, (uint8_t *)(uint32_t) "iSET:1,0", sizeof("iSET:1,0") - 1, 0xFFFF);
    HAL_Delay(10);
    HAL_UART_Transmit(huart, (uint8_t *)(uint32_t) "iSET:2,40", sizeof("iSET:2,40") - 1, 0xFFFF);
    HAL_Delay(10);
    HAL_UART_Transmit(huart, (uint8_t *)(uint32_t) "iSET:4,115200", sizeof("iSET:4,115200") - 1, 0xFFFF);
    HAL_Delay(10);
    HAL_UART_Transmit(huart, (uint8_t *)(uint32_t) "iSET:7,20", sizeof("iSET:7,20") - 1, 0xFFFF);
    HAL_Delay(10);
    HAL_UART_Transmit(huart, (uint8_t *)(uint32_t) "iSET:8,0", sizeof("iSET:8,0") - 1, 0xFFFF);
    HAL_Delay(10);
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
#if L1S_TOTAL > 0
    l1s_config(&huart_l1s0);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    l1s_config(&huart_l1s1);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    l1s_config(&huart_l1s2);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    l1s_config(&huart_l1s3);
#endif /* L1S_TOTAL */

#if L1S_TOTAL > 0
    usart_dma_rx_init(&huart_l1s0);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    usart_dma_rx_init(&huart_l1s1);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    usart_dma_rx_init(&huart_l1s2);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    usart_dma_rx_init(&huart_l1s3);
#endif /* L1S_TOTAL */

#if L1S_TOTAL > 0
    usart_dma_rx(&huart_l1s0, l1s0_buf8[0] + 1, l1s0_buf8[1] + 1, BUFSIZ_L1S0 - 1);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    usart_dma_rx(&huart_l1s1, l1s1_buf8[0] + 1, l1s1_buf8[1] + 1, BUFSIZ_L1S1 - 1);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    usart_dma_rx(&huart_l1s2, l1s2_buf8[0] + 1, l1s2_buf8[1] + 1, BUFSIZ_L1S2 - 1);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    usart_dma_rx(&huart_l1s3, l1s3_buf8[0] + 1, l1s3_buf8[1] + 1, BUFSIZ_L1S3 - 1);
#endif /* L1S_TOTAL */

#if L1S_TOTAL > 0
    l1s_start(&huart_l1s0);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    l1s_start(&huart_l1s1);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    l1s_start(&huart_l1s2);
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    l1s_start(&huart_l1s3);
#endif /* L1S_TOTAL */
}

void l1s_check(void)
{
#if L1S_TOTAL > 0
    if (l1s.dis0.error == L1S_ERROR_NORUN)
    {
        l1s_start(&huart_l1s0);
    }
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    if (l1s.dis1.error == L1S_ERROR_NORUN)
    {
        l1s_start(&huart_l1s1);
    }
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    if (l1s.dis2.error == L1S_ERROR_NORUN)
    {
        l1s_start(&huart_l1s2);
    }
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    if (l1s.dis3.error == L1S_ERROR_NORUN)
    {
        l1s_start(&huart_l1s3);
    }
#endif /* L1S_TOTAL */
}

void l1s_cli(void)
{
#if L1S_TOTAL > 0
    if (l1s.dis0.error == L1S_ERROR_NONE && l1s.dis0.raw > 0)
    {
        l1s.dis0.base = l1s.dis0.raw;
    }
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 1
    if (l1s.dis1.error == L1S_ERROR_NONE && l1s.dis1.raw > 0)
    {
        l1s.dis1.base = l1s.dis1.raw;
    }
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 2
    if (l1s.dis2.error == L1S_ERROR_NONE && l1s.dis2.raw > 0)
    {
        l1s.dis2.base = l1s.dis2.raw;
    }
#endif /* L1S_TOTAL */
#if L1S_TOTAL > 3
    if (l1s.dis3.error == L1S_ERROR_NONE && l1s.dis3.raw > 0)
    {
        l1s.dis3.base = l1s.dis3.raw;
    }
#endif /* L1S_TOTAL */
}

/**
 * @brief This function handles USART global interrupt.
*/
#if L1S_TOTAL > 0
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
#endif /* L1S_TOTAL */

#if L1S_TOTAL > 1
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
#endif /* L1S_TOTAL */

#if L1S_TOTAL > 2
void L1S2_IRQHandler(void)
{
    if (huart_l1s2.Instance->SR & UART_FLAG_RXNE) /*!< USART Status register*/
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s2); /* Clears the UART PE pending l1s2_flag */
    }
    else if (huart_l1s2.Instance->SR & UART_FLAG_IDLE)
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s2); /* Clears the UART PE pending l1s2_flag */

        /* Disable the specified DMA Stream */
        __HAL_DMA_DISABLE(huart_l1s2.hdmarx);

        /* DMA stream x configuration register */
        if ((huart_l1s2.hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s2_buf = l1s2_buf8[0];
            l1s2_buf[0] = (uint8_t)(BUFSIZ_L1S2 - 1 - huart_l1s2.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s2.hdmarx->Instance->NDTR = BUFSIZ_L1S2 - 1;

            /* Set memory buffer 1 */
            huart_l1s2.hdmarx->Instance->CR |= DMA_SxCR_CT;

            /* Enable the specified DMA Stream */
            __HAL_DMA_ENABLE(huart_l1s2.hdmarx);
        }
        else
        {
            /* Current memory buffer used is Memory 1 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s2_buf = l1s2_buf8[1];
            l1s2_buf[0] = (uint8_t)(BUFSIZ_L1S2 - 1 - huart_l1s2.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s2.hdmarx->Instance->NDTR = BUFSIZ_L1S2 - 1;

            /* Set memory buffer 0 */
            huart_l1s2.hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
        }

        /* Enable the specified DMA Stream */
        __HAL_DMA_ENABLE(huart_l1s2.hdmarx);

        l1s_irq(&l1s.dis2, FLAG_L1S2, l1s2_buf + 1, *l1s2_buf);
    }
    else if (huart_l1s2.Instance->SR & UART_FLAG_TC)
    {
        __HAL_UART_CLEAR_FLAG(&huart_l1s2, UART_FLAG_TC);
    }
}
#endif /* L1S_TOTAL */

#if L1S_TOTAL > 3
void L1S3_IRQHandler(void)
{
    if (huart_l1s3.Instance->SR & UART_FLAG_RXNE) /*!< USART Status register*/
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s3); /* Clears the UART PE pending l1s3_flag */
    }
    else if (huart_l1s3.Instance->SR & UART_FLAG_IDLE)
    {
        __HAL_UART_CLEAR_PEFLAG(&huart_l1s3); /* Clears the UART PE pending l1s3_flag */

        /* Disable the specified DMA Stream */
        __HAL_DMA_DISABLE(huart_l1s3.hdmarx);

        /* DMA stream x configuration register */
        if ((huart_l1s3.hdmarx->Instance->CR & DMA_SxCR_CT) == RESET)
        {
            /* Current memory buffer used is Memory 0 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s3_buf = l1s3_buf8[0];
            l1s3_buf[0] = (uint8_t)(BUFSIZ_L1S3 - 1 - huart_l1s3.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s3.hdmarx->Instance->NDTR = BUFSIZ_L1S3 - 1;

            /* Set memory buffer 1 */
            huart_l1s3.hdmarx->Instance->CR |= DMA_SxCR_CT;

            /* Enable the specified DMA Stream */
            __HAL_DMA_ENABLE(huart_l1s3.hdmarx);
        }
        else
        {
            /* Current memory buffer used is Memory 1 */

            /* Receive data length, length = set_data_length - remain_length */
            l1s3_buf = l1s3_buf8[1];
            l1s3_buf[0] = (uint8_t)(BUFSIZ_L1S3 - 1 - huart_l1s3.hdmarx->Instance->NDTR);

            /* Reset set_data_lenght */
            huart_l1s3.hdmarx->Instance->NDTR = BUFSIZ_L1S3 - 1;

            /* Set memory buffer 0 */
            huart_l1s3.hdmarx->Instance->CR &= ~(DMA_SxCR_CT);
        }

        /* Enable the specified DMA Stream */
        __HAL_DMA_ENABLE(huart_l1s3.hdmarx);

        l1s_irq(&l1s.dis2, FLAG_L1S3, l1s3_buf + 1, *l1s3_buf);
    }
    else if (huart_l1s3.Instance->SR & UART_FLAG_TC)
    {
        __HAL_UART_CLEAR_FLAG(&huart_l1s3, UART_FLAG_TC);
    }
}
#endif /* L1S_TOTAL */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
