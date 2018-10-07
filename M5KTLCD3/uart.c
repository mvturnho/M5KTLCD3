/*
 * uart.c
 *
 *  Created on: Oct 7, 2018
 *      Author: mvturnho
 */

#include "M5KTLCD3.h"

#define EX_UART_NUM UART_NUM_1

#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

volatile uint8_t ui8_received_package_flag = 0;
volatile uint8_t ui8_rx_buffer[22];
volatile uint8_t ui8_rx_counter = 0;
volatile uint8_t ui8_tx_buffer[11];
volatile uint8_t ui8_tx_counter = 0;
volatile uint8_t ui8_i;
volatile uint8_t ui8_checksum;
static uint16_t ui16_crc_rx;
static uint16_t ui16_crc_tx;
static uint8_t ui8_lcd_variable_id = 0;
static uint8_t ui8_master_comm_package_id = 0;
static uint8_t ui8_slave_comm_package_id = 0;
volatile uint8_t ui8_byte_received;
volatile uint8_t ui8_state_machine = 0;
volatile uint8_t ui8_uart_received_first_package = 0;

// Both definition are same and valid
//static uart_isr_handle_t *handle_console;
static intr_handle_t handle_console;

static void IRAM_ATTR uart_intr_handle(void *arg) {
	uint16_t rx_fifo_len, status;
	uint16_t i = 0;

	status = UART0.int_st.val; // read UART interrupt Status

	rx_fifo_len = UART0.status.rxfifo_cnt; // read number of bytes in UART buffer

	while (rx_fifo_len) {
		ui8_rx_buffer[i++] = UART0.fifo.rw_byte; // read all bytes
		rx_fifo_len--;
	}

	// after reading bytes from buffer clear UART interrupt status
	uart_clear_intr_status(EX_UART_NUM,
			UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);

	// a test code or debug code to indicate UART receives successfully,
	// you can redirect received byte as echo also
	uart_write_bytes(EX_UART_NUM, (const char*) "RX Done", 7);
}

void uart2_init(void) {
	/* Configure parameters of an UART driver,
	 * communication pins and install the driver */
	uart_config_t uart_config = { .baud_rate = 115200, .data_bits =
			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE };
	ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
	ESP_ERROR_CHECK(
			uart_set_pin(EX_UART_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
	ESP_ERROR_CHECK(
			uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));
	// release the pre registered UART handler/subroutine
	ESP_ERROR_CHECK(uart_isr_free(EX_UART_NUM));

	// register new UART ISR subroutine
	ESP_ERROR_CHECK(
			uart_isr_register(EX_UART_NUM,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

}

