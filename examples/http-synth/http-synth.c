/*
 * Copyright (C) 2016, Michael LeMay. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "sys/log.h"

#include "httpd.h"
#include "httpd-cgi.h"
#include "startup.h"
#include "uart.h"

PROCESS(http_synth_process, "HTTP-accessible synthesizer");

AUTOSTART_PROCESSES(&http_synth_process);

static struct ctimer timer;
#define MIDI_CHANNEL 0

/* See section 4 of the datasheet for the dream SAM2695 synthesizer for a
 * listing of other available programs/instruments:
 * https://www.dream.fr/pdf/Serie2000/SAM_Datasheets/SAM2695.pdf
 */

#define MIDI_PROG_CELESTA 9

#define MIDI_PROG_ALERT MIDI_PROG_CELESTA

#define MIDI_PITCH_MIDDLE_C 60

#define MIDI_PITCH_ALERT MIDI_PITCH_MIDDLE_C
#define MIDI_VELOCITY_ALERT 100

#define MIDI_VELOCITY_OFF 0

/*---------------------------------------------------------------------------*/
static void
midi_note(unsigned pitch, unsigned velocity)
{
  assert(0 < pitch && pitch < 128);
  assert(velocity < 128);

  quarkX1000_uart_tx(QUARK_X1000_UART_0, 0x90 | MIDI_CHANNEL);
  quarkX1000_uart_tx(QUARK_X1000_UART_0, pitch);
  quarkX1000_uart_tx(QUARK_X1000_UART_0, velocity);
}
/*---------------------------------------------------------------------------*/
static void
midi_prog_change(unsigned prog)
{
  quarkX1000_uart_tx(QUARK_X1000_UART_0, 0xC0 | MIDI_CHANNEL);
  quarkX1000_uart_tx(QUARK_X1000_UART_0, prog);
}
/*---------------------------------------------------------------------------*/
static void
stop_alert(void *data)
{
  midi_note(MIDI_PITCH_ALERT, MIDI_VELOCITY_OFF);
}
/*---------------------------------------------------------------------------*/
static void
play_alert(void *data)
{
  midi_prog_change(MIDI_PROG_ALERT);
  midi_note(MIDI_PITCH_ALERT, MIDI_VELOCITY_ALERT);

  printf("Played note.\n");

  if(!ctimer_expired(&timer)) {
    ctimer_stop(&timer);
  }
  ctimer_set(&timer, CLOCK_SECOND * 2, stop_alert, NULL);
}
/*---------------------------------------------------------------------------*/
static
PT_THREAD(synth_thread(struct httpd_state *s, char *ptr))
{
  PSOCK_BEGIN(&s->sout);

  play_alert(NULL);

  PSOCK_SEND_STR(&s->sout, "Played note.");

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
static const char synth_name[] HTTPD_STRING_ATTR = "synth";
HTTPD_CGI_CALL(synth, synth_name, synth_thread);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(http_synth_process, ev, data)
{
  PROCESS_BEGIN();

  httpd_init();
  httpd_cgi_add(&synth);

  ctimer_set(&timer, CLOCK_SECOND / 2, play_alert, NULL);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
KERN_STARTUP_FUNC(http_synth_kern_init)
{
  quarkX1000_uart_init_port(QUARK_X1000_UART_0, 31250);
}
/*---------------------------------------------------------------------------*/
