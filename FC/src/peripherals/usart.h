#ifndef FC_PERIPHERALS_USART_H
#define FC_PERIPHERALS_USART_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void usart_setup(int num, unsigned int baud, bool dma);
void usart_disable(int num);
void usart_send(int num, const uint8_t *buf, size_t size);
void usart_receive(int num, uint8_t *buf, size_t size);
void *usart_dma_address(int num);

#endif

