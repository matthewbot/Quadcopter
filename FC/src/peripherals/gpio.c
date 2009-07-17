#include "peripherals/gpio.h"
#include "stm32f10x.h"

#define CNF_ANALOG 0
#define CNF_INPUT_FLOAT 1
#define CNF_INPUT_PULL 2

#define CNF_PUSH_PULL 0
#define CNF_OPEN_DRAIN 1
#define CNF_AF_PUSH_PULL 2
#define CNF_AF_OPEN_DRAIN 3

#define MODE_INPUT 0
#define MODE_OUTPUT_10MHZ 1
#define MODE_OUTPUT_2MHZ 2
#define MODE_OUTPUT_50MHZ 3

#define PINLO(num, cnf, mode) (((cnf) << 2) | (mode)) << ((num)*4)
#define PINHI(num, cnf, mode) PINLO(num-8, cnf, mode)

static GPIO_TypeDef *const gpios[] = { GPIOA, GPIOB, GPIOC, GPIOD };

void gpio_init() {
	AFIO->MAPR = AFIO_MAPR_SWJ_CFG_DISABLE | // disable JTAG debugging (we use LEDs for cheaperness :D)
	             AFIO_MAPR_TIM2_REMAP_PARTIALREMAP2 | // Remap Timer 2 (We need CH3 on PB10)
	             AFIO_MAPR_TIM3_REMAP_FULLREMAP | // Remap Timer 3 (We need it on PC6-PC9)
	             AFIO_MAPR_USART3_REMAP_PARTIALREMAP; // Remap USART3 (Need on PC10-PC11)

	GPIOA->ODR = (1 << 0); // Pin 0 needs pull up resisters due to button)

	GPIOA->CRL = PINLO(0, CNF_INPUT_PULL, MODE_INPUT) | // Button
	             PINLO(1, CNF_ANALOG, MODE_INPUT) | // Pitch gyro (ADC12_IN1)
	             PINLO(2, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Future (USART2_TX)
	             PINLO(3, CNF_INPUT_FLOAT, MODE_INPUT) | // Future (USART2_RX)
	             PINLO(4, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Future (SPI1_NSS)
	             PINLO(5, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Future (SPI1_SCK)
	             PINLO(6, CNF_INPUT_FLOAT, MODE_INPUT) | // Future (SPI1_MISO)
	             PINLO(7, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ); // Future (SPI1_MOSI)
	             
	GPIOA->CRH = PINHI(8, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Altimeter MCLK (TIM1_CH1)
	             PINHI(9, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // PC USB (USART1_TX)
	             PINHI(10, CNF_INPUT_FLOAT, MODE_INPUT) | // PC USB (USART1_RX)
	             PINHI(13, CNF_OPEN_DRAIN, MODE_OUTPUT_2MHZ) | // LED1 Red
	             PINHI(14, CNF_OPEN_DRAIN, MODE_OUTPUT_2MHZ) | // LED2 Yellow
	             PINHI(15, CNF_OPEN_DRAIN, MODE_OUTPUT_2MHZ); // LED3 Green
	             
	GPIOB->ODR = (1 << 4); // Pin 4 needs pull up resisters due to button)
	             
	GPIOB->CRL = PINLO(0, CNF_ANALOG, MODE_INPUT) | // Pot 2 (ADC12_IN8)
	             PINLO(1, CNF_ANALOG, MODE_INPUT) | // Pot 3 (ADC12_IN9)
	             PINLO(3, CNF_OPEN_DRAIN, MODE_OUTPUT_2MHZ) | // LED 4 (Blue)
	             PINLO(4, CNF_INPUT_PULL, MODE_INPUT) | // Button 2
	             PINLO(5, CNF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Compass reset (TODO, how to configure!)
                 PINLO(6, CNF_AF_OPEN_DRAIN, MODE_OUTPUT_2MHZ) | // I2C clock
                 PINLO(7, CNF_AF_OPEN_DRAIN, MODE_OUTPUT_2MHZ);  // I2C data
                 
    GPIOB->CRH = PINHI(8, CNF_INPUT_FLOAT, MODE_INPUT) | // Compass DRDY
                 PINHI(9, CNF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Altimeter XCLR
                 PINHI(10, CNF_INPUT_FLOAT, MODE_INPUT) | // PPM Input (TIM2_CH3)
                 PINHI(11, CNF_INPUT_FLOAT, MODE_INPUT) | // US signal (TIM2_CH4)
                 PINHI(12, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Compass (SPI2_NSS)
                 PINHI(13, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // Compass (SPI2_SCK)
                 PINHI(14, CNF_INPUT_FLOAT, MODE_INPUT) | // Compass (SPI2_MISO)
                 PINHI(15, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ); // Compass (SPI2_MOSI)
                 
	GPIOC->CRL = PINLO(0, CNF_ANALOG, MODE_INPUT) | // Accel X (ADC12_IN10)
	             PINLO(1, CNF_ANALOG, MODE_INPUT) | // Accel Y (ADC12_IN11)
	             PINLO(2, CNF_ANALOG, MODE_INPUT) | // Accel Z (ADC12_IN12)
	             PINLO(3, CNF_ANALOG, MODE_INPUT) | // Roll Gyro (ADC12_IN13)
	             PINLO(4, CNF_ANALOG, MODE_INPUT) | // Yaw Gyro (ADC12_IN14)
	             PINLO(5, CNF_ANALOG, MODE_INPUT) | // Pot 1 (ADC12_IN15)
	             PINLO(6, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // North ESC (TIM3_CH1)
	             PINLO(7, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ); // South ESC (TIM3_CH2)
	             
	GPIOC->CRH = PINHI(8, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // East ESC (TIM3_CH3)
	             PINHI(9, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // West ESC (TIM3_CH4)
	             PINHI(10, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // XBC (USART3_TX)
	             PINHI(11, CNF_AF_PUSH_PULL, MODE_OUTPUT_2MHZ) | // XBC (USART3_RX)
	             PINHI(12, CNF_OPEN_DRAIN, MODE_OUTPUT_2MHZ) | // LED
	             PINHI(13, CNF_INPUT_FLOAT, MODE_INPUT) | // Future
	             PINHI(14, CNF_INPUT_FLOAT, MODE_INPUT) | // OSC32_IN
	             PINHI(15, CNF_INPUT_FLOAT, MODE_INPUT); // OSC32_OUT           
}

void gpio_output(enum gpio_port port, int pin, bool val) {
	gpios[port]->BSRR = 1 << (val ? pin : pin+16);
}

bool gpio_input(enum gpio_port port, int pin) {
	return (gpios[port]->IDR >> pin) & 1;
}
