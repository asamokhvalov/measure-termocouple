#ifndef DEV_INGO_H
#define DEV_INGO_H

#include "stm32f1xx.h"

#define MAX_SYS_TICK_MS (uint16_t)65000

// timing measure in ms
typedef enum {
	PERIOD_MEASURE_TEMPERATURE = (uint16_t)200, // conversation time
}
PERIOD_TIMING;

/* ---------------------------------------- */

typedef enum {
	STAGE_MEASUREMENT = 0x00,
	STAGE_CALCULATION = 0x01,
	STAGE_SENDING = 0x02,
	STAGE_CHANGE_DIV_MUL = 0x03
}
STATUS_DEVICE;

typedef enum {
	CAPTURE_DATA_SIZE = 50,
}
ADC_ARRAY_DATA_SIZE;

typedef enum {
	PROCESS_DATA_FREE = 0,
	PROCESS_DATA_BUSY = 1
}
FLAG_PROCESS_DATA;

typedef enum
{
	SIZE_BUF_IN = (uint8_t)100,
	SIZE_PAYLOAD_IN = (uint8_t)50,
}
SIZE_BUFFER_INPUT;

typedef enum
{
	SIZE_BUF_OUT = (uint8_t)100,
	SIZE_PAYLOAD_OUT = (uint8_t)50,
}
SIZE_BUFFER_OUTPUT;

typedef enum
{
	FLAG_THERMOCOUPLE_WAIT = (uint8_t)0,
	FLAG_THERMOCOUPLE_READY = (uint8_t)1,
}
FLAG_THERMOCOUPLE;

#define CMD_IDN_SIZE	(uint8_t)3
#define CMD_TEMP_SIZE	(uint8_t)4
#define CMD_IDN_INFO_SIZE	(uint8_t)34

typedef struct __attribute__((packed)) _dev_info 
{
	volatile uint16_t sys_time;

	uint8_t stage;

	volatile uint8_t value_thermocouple[2];
	volatile uint8_t f_ready_thermocouple;
	
	volatile uint8_t in_buf[SIZE_BUF_IN];
	volatile uint8_t i_push_in;
	volatile uint8_t i_pop_in;
	
	volatile uint8_t out_buf[SIZE_BUF_OUT];
	volatile uint8_t i_push_out;
	volatile uint8_t i_pop_out;

	volatile uint8_t in_payload[SIZE_PAYLOAD_IN];
	volatile uint8_t payload_size;
}
dev_info;

extern dev_info devInfo;

void dev_init (dev_info * _dev_info);

uint16_t get_sys_time (dev_info * _dev_info);
void set_sys_time (dev_info * _dev_info, uint16_t value);

static void inc_pop_in_buf (void);
void processing_input_buffer (dev_info * _dev_info);

#endif
