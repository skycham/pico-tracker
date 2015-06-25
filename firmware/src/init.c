/*
 * Board init functions
 * Copyright (C) 2015  Richard Meadows <richardeoin>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "samd20.h"
#include "hw_config.h"
#include "system/system.h"
#include "system/port.h"
#include "system/events.h"
#include "system/extint.h"
#include "gps.h"
#include "si_trx.h"
#include "watchdog.h"
#include "xosc.h"
#include "timer.h"

/**
 * Initialises the status LED. SHOULD TURN ON THE LED
 */
static inline void led_reset(void)
{
  port_pin_set_config(LED0_PIN,
		      PORT_PIN_DIR_OUTPUT,	/* Direction */
		      PORT_PIN_PULL_NONE,	/* Pull */
		      false);			/* Powersave */
  port_pin_set_output_level(LED0_PIN, 0);	/* LED is active low */
}

/**
 * Power Management
 */
void powermananger_init(void)
{
  system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBA,
//			      PM_APBAMASK_EIC | /* EIC is used now */
//			      PM_APBAMASK_RTC | /* RTC is used now */
    0);
}

/**
 * Internal initialisation
 * =============================================================================
 */
void init(timepulse_callback_t callback)
{
  /**
   * Reset to get the system in a safe state
   * --------------------------------------------------------------------------
   */
  led_reset();
  si_trx_shutdown();

  /* If the reset was caused by the internal watchdog... */
  if (PM->RCAUSE.reg & PM_RCAUSE_WDT) {
    /* External hardware is in an undefined state. Wait here for the
       external watchdog to trigger an external reset */

    while (1);
  }

  /**
   * Internal initialisation
   * ---------------------------------------------------------------------------
   */

  /* Clock up to 14MHz with 0 wait states */
  system_flash_set_waitstates(SYSTEM_WAIT_STATE_1_8V_14MHZ);

  /* Up the clock rate to 4MHz */
  system_clock_source_osc8m_set_config(SYSTEM_OSC8M_DIV_2, /* Prescaler */
				       false,		   /* Run in Standby */
				       false);		   /* Run on Demand */

  /* Restart the GCLK Module */
  system_gclk_init();
  system_events_init();
  system_extint_init();

  /* Remember the HW watchdog has been running since reset */
  //watchdog_init();

  /* Configure Sleep Mode */
  //system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
  system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2); /* Disable CPU, AHB and APB */

  /* Configure the Power Manager */
  //powermananger_init();

  /**
   * System initialisation
   * ---------------------------------------------------------------------------
   */

  /* Enable the xosc on gclk1 */
  xosc_init();

  /* GPS init */
//  gps_init();

  /* Enable timer interrupt and event channel */
  timepulse_extint_init();
  timepulse_set_callback(callback);

  /* Initialise Si4060 interface */
  si_trx_init();
}