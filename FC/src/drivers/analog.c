#include "drivers/analog.h"
#include "peripherals/adc.h"
#include "peripherals/dma.h"
#include "peripherals/timer.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"
#include <stdint.h>

#define ADC_DMA 1
#define ADC_DMA_IRQ (DMA1_Channel1_IRQn + ADC_DMA - 1)

#define ADC_TIMER 4
#define ADC_TIMER_CH 4

static const uint8_t chans[ANALOG_READINGS_COUNT] = { 
	13, 1, 9, // Roll Pitch Yaw gyros
	12, 14, 15 // Accel X Y and Z
};

static volatile union {
	struct analog_readings readings;
	uint16_t raw[ANALOG_READINGS_COUNT];
} data;

static analog_update_handler update_handler;

static void analog_dma_handler();

void analog_setup(analog_update_handler handler) {
	update_handler = handler;
	
	dma_configure(ADC_DMA, DMA_DIR_PERIPHERAL_TO_MEM, DMA_PRIORITY_HIGH, 16, 16, 
		DMA_OPTION_MEMORY_INCREMENT | DMA_OPTION_CIRCULAR | DMA_OPTION_INTERRUPT);
	dma_start(ADC_DMA, data.raw, adc_dma_address(), ANALOG_READINGS_COUNT);

	timer_channel_setup_oc(ADC_TIMER, ADC_TIMER_CH, TIMER_OC_MODE_PWM_1, NULL);
	timer_channel_set_ccr(ADC_TIMER, ADC_TIMER_CH, 1000);

	nvic_register_handler(ADC_DMA_IRQ, analog_dma_handler);
	nvic_enable_interrupt(ADC_DMA_IRQ);
	
	adc_scan(chans, ANALOG_READINGS_COUNT, ADC_TRIGGER_TIM4_CC4);
}

struct analog_readings analog_get_readings() {
	return data.readings;
}

static void analog_dma_handler() {
	if (update_handler)
		update_handler(data.readings);
		
	dma_clear_interrupt(ADC_DMA);
}

