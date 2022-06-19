#include "usart.h"

#include "dev-info.h"
#include "gpio.h"

dev_info devInfo;

const uint8_t idn_cmd[CMD_IDN_SIZE] = {0x49, 0x44, 0x4E}; // IDN
const uint8_t temp_cmd[CMD_TEMP_SIZE] = {0x54, 0x45, 0x4D, 0x50}; // TEMP

const char info_cmd[CMD_IDN_INFO_SIZE] = {	
	'T', 'h', 'e', 'r', 'm', 'o', 'c', 'o', 'u', 'p', 'l', 'e', ',',
	'S', 'T', 'M', '3', '2', 'F', '1', ',',
	's', '/', 'n', '1', ',',
	'v', 'e', 'r', '1', '.', '0', '\r', '\n'
};

void dev_init (dev_info * _dev_info)
{
	_dev_info->sys_time = 0;

	_dev_info->value_thermocouple[1] = 0;
	_dev_info->value_thermocouple[0] = 0;

	_dev_info->f_ready_thermocouple = FLAG_THERMOCOUPLE_WAIT;
	
	_dev_info->i_push_in = 0;
	_dev_info->i_pop_in = 0;
	
	_dev_info->i_push_out = 0;
	_dev_info->i_pop_out = 0;

	_dev_info->payload_size = 0;
}

uint16_t get_sys_time (dev_info * _dev_info)
{
	return _dev_info->sys_time;
}

void set_sys_time (dev_info * _dev_info, uint16_t value)
{
	_dev_info->sys_time = value;
}

// *IDN?/r/n

#define HEAD_SYMBOL (uint8_t)0x2A // *
#define END_SYMBOL (uint8_t)0x3F // ?

void processing_input_buffer (dev_info * _dev_info)
{
	if (_dev_info->i_push_in != _dev_info->i_pop_in)
	{
		uint8_t byte = _dev_info->in_buf[_dev_info->i_pop_in];

		switch (_dev_info->stage)
		{
			case 0:
			{
				if (byte == HEAD_SYMBOL)
				{
					_dev_info->stage = 1;
				}

				inc_pop_in_buf ();

				break;
			}
			case 1:
			{
				if (byte == END_SYMBOL)
				{
					_dev_info->stage = 2;
				}
				else
				{
					_dev_info->in_payload[_dev_info->payload_size] = byte;

					_dev_info->payload_size++;
					if (_dev_info->payload_size == SIZE_PAYLOAD_IN)
					{
						_dev_info->payload_size = 0;
					}
				}

				inc_pop_in_buf ();

				break;
			}
			case 2:
			{
				if (_dev_info->payload_size == CMD_IDN_SIZE)
				{
					for (uint8_t i = 0; i < CMD_IDN_SIZE; i++)
					{
						if (idn_cmd[i] == _dev_info->in_payload[i])
						{
							_dev_info->payload_size--;
						}
					}

					if (!_dev_info->payload_size)
					{
						send ((uint8_t *)info_cmd, CMD_IDN_INFO_SIZE);
					}
					else
					{
						_dev_info->payload_size = 0;
					}

					_dev_info->stage = 0;
				}
				else if (_dev_info->payload_size == CMD_TEMP_SIZE)
				{
					for (uint8_t i = 0; i < CMD_TEMP_SIZE; i++)
					{
						if (temp_cmd[i] == _dev_info->in_payload[i])
						{
							_dev_info->payload_size--;
						}
					}

					if (!_dev_info->payload_size)
					{
						uint8_t data[4];
						
						data[0] = _dev_info->value_thermocouple[0];
						data[1] = _dev_info->value_thermocouple[1];
						data[2] = 0xd; // '\r'
						data[3] = 0xa; // '\n'

						send ((uint8_t *)data, 4);
					}
					else
					{
						_dev_info->payload_size = 0;
					}

					_dev_info->stage = 0;
				}
				else
				{
					_dev_info->stage = 0;
					_dev_info->payload_size = 0;
				}

				break;
			}

		}
	}
}

void inc_pop_in_buf (void)
{
	devInfo.i_pop_in++;

	if (devInfo.i_pop_in == SIZE_BUF_IN)
	{
		devInfo.i_pop_in = 0;
	}
}
