/**
 * SAM D20/D21/R21 Serial Peripheral Interface Driver
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef SERCOM_H_INCLUDED
#define SERCOM_H_INCLUDED

#include "system/system.h"
#include "system/clock.h"
#include "sercom/sercom_pinout.h"

#if (SERCOM0_GCLK_ID_SLOW == SERCOM1_GCLK_ID_SLOW &&	\
     SERCOM0_GCLK_ID_SLOW == SERCOM2_GCLK_ID_SLOW &&	\
     SERCOM0_GCLK_ID_SLOW == SERCOM3_GCLK_ID_SLOW)
#  define SERCOM_GCLK_ID SERCOM0_GCLK_ID_SLOW
#else
#  error "SERCOM modules must share the same slow GCLK channel ID."
#endif

#if (0x1ff >= REV_SERCOM)
#  define FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_1
#elif (0x2ff >= REV_SERCOM)
#  define FEATURE_SERCOM_SYNCBUSY_SCHEME_VERSION_2
#else
#  error "Unknown SYNCBUSY scheme for this SERCOM revision"
#endif

/**
 * Status Codes
 */
enum sercom_status_t {
  SERCOM_STATUS_OK = 1,
  SERCOM_STATUS_BAUDRATE_UNAVAILABLE,
  SERCOM_STATUS_TIMEOUT,
  SERCOM_STATUS_PACKET_COLLISION,
  SERCOM_STATUS_BUSY,
  SERCOM_STATUS_DENIED,
  SERCOM_STATUS_BAD_FORMAT,
  SERCOM_STATUS_BAD_DATA,
  SERCOM_STATUS_OVERFLOW,
  SERCOM_STATUS_IO,
  SERCOM_STATUS_INVALID_ARG,
  SERCOM_STATUS_ABORTED,
  SERCOM_STATUS_UNSUPPORTED_DEV
};

/**
 * Select sercom asynchronous operation mode
 */
enum sercom_asynchronous_operation_mode {
  SERCOM_ASYNC_OPERATION_MODE_ARITHMETIC = 0,
  SERCOM_ASYNC_OPERATION_MODE_FRACTIONAL,
};

/**
 * Select number of samples per bit
 */
enum sercom_asynchronous_sample_num {
  SERCOM_ASYNC_SAMPLE_NUM_3 = 3,
  SERCOM_ASYNC_SAMPLE_NUM_8 = 8,
  SERCOM_ASYNC_SAMPLE_NUM_16 = 16,
};

/**
 * Type used for registering interrupt handlers
 */
typedef void (*sercom_handler_t)(Sercom* const sercom_instance,
				 uint8_t instance_index);

uint8_t _sercom_get_sercom_inst_index(Sercom *const sercom_instance);

void _sercom_set_handler(Sercom* const sercom_instance,
			 const sercom_handler_t interrupt_handler);

void _sercom_set_gclk_generator(const enum gclk_generator generator_source);

enum sercom_status_t _sercom_get_sync_baud_val(const uint32_t baudrate,
					       const uint32_t external_clock,
					       uint16_t *const baudval);

enum sercom_status_t _sercom_get_async_baud_val(const uint32_t baudrate,
						const uint32_t peripheral_clock,
						uint16_t *const baudval,
						enum sercom_asynchronous_operation_mode mode,
						enum sercom_asynchronous_sample_num sample_num);

uint32_t _sercom_get_default_pad(Sercom *const sercom_module,
				 const uint8_t pad);

#endif /* SERCOM_H_INCLUDED */
