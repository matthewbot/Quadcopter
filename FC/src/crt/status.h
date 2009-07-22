#ifndef FC_CRT_STATUS_H
#define FC_CRT_STATUS_H

#define BLINK_COUNT_NMI 1
#define BLINK_COUNT_HARDFAULT 2
#define BLINK_COUNT_FAULT 3 // fault handled by nvic - message will be printed on USART1
#define BLINK_COUNT_UNHANDLED 4 // message on USART1

void status_init();
void status_on();
void status_off();

__attribute__ ((noreturn))
void status_blink_halt(int count);

#endif
