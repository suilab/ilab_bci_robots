

/*
  Copyright (c) 2014 José Carlos Nieto, https://menteslibres.net/xiam

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * Aknowledgements:
 *
 * I used these shources while learning how the IR sensors and emitters work:
 *  http://learn.adafruit.com/ir-sensor/
 *  https://github.com/shirriff/Arduino-IRremote
 *  http://en.wikipedia.org/wiki/Apple_Remote
 *  http://techdocs.altium.com/display/ADRR/NEC+Infrared+Transmission+Protocol
 *
 * The code used for emitting signals was taken from the Arduino-IRremote
 * library, with some minor modifications.
 *  https://github.com/shirriff/Arduino-IRremote
 *
 * */

#ifndef IRDEBUG
#define IRDEBUG

// This is the pin number used by the IR sensor.
#ifndef IRDEBUG_RECV_PIN
  #define IRDEBUG_RECV_PIN           2
#endif

// This is the pin number used by the IR led.
#ifndef IRDEBUG_OUTP_PIN
  #define IRDEBUG_OUTP_PIN           9
#endif

// Maximum burst/space length (in milliseconds).
#ifndef IRDEBUG_PULSE_MAX_LENGTH
  // This is, by default, tuned for the Apple Remote, which maximum burst is
  // 9000 long.
  #define IRDEBUG_PULSE_MAX_LENGTH  9500
#endif

// Minimum burst/space length (in milliseconds). Tune it if you're having odd reads.
#ifndef IRDEBUG_PULSE_MIN_LENGTH
  // This is, by default, tuned for the Apple Remote, which minimum burst is
  // 560 long.
  #define IRDEBUG_PULSE_MIN_LENGTH  300
#endif

// Maximum signal size.
#ifndef IRDEBUG_SIGNAL_BUFSIZE
  #define IRDEBUG_SIGNAL_BUFSIZE     128
#endif

// System's clock.
#ifdef F_CPU
  #define SYSCLOCK F_CPU
#else
  #define SYSCLOCK 16000000
#endif

// Timer macros taken from https://github.com/shirriff/Arduino-IRremote
#define TIMER_ENABLE_PWM     (TCCR2A |= _BV(COM2B1))
#define TIMER_DISABLE_PWM    (TCCR2A &= ~(_BV(COM2B1)))
#define TIMER_DISABLE_INTR   (TIMSK2 = 0)
#define TIMER_CONFIG_KHZ(val) ({ \
  const uint8_t pwmval = SYSCLOCK / 2000 / (val); \
  TCCR2A = _BV(WGM20); \
  TCCR2B = _BV(WGM22) | _BV(CS20); \
  OCR2A = pwmval; \
  OCR2B = pwmval / 3; \
})

// Signal buffer.
unsigned int irdebug_signal[IRDEBUG_SIGNAL_BUFSIZE];
unsigned int irdebug_signal_i;
unsigned int irdebug_signal_l;

unsigned long irdebug_time;
unsigned long irdebug_curr;
unsigned int irdebug_diff;

bool irdebug_capturing;
bool irdebug_signal_on;
bool irdebug_initialized = false;

// Enables output for the IRDEBUG_OUTP_PIN at the desired frequency.
void irdebug_enable_irout(int);

// Sends a pulse burst.
void irdebug_send_mark(unsigned int);

// Disables burst and waits.
void irdebug_send_space(unsigned int);

// Advances buffer to the next signal's slot.
void irdebug_next_burst();

// Starts a signal capture.
void irdebug_start_capture();

// Stops a signal capture
void irdebug_stop_capture();

// Initializes irdebug.
void irdebug_init();

// Copies buffer to a signal pointer.
void irdebug_dump_signal(unsigned int *);

// Prints signal on code compatible form.
void irdebug_print_signal(unsigned int *);

// Sends a raw signal.
void irdebug_send(unsigned int *, int);

// Attempts to capture a signal.
bool irdebug_capture_signal(unsigned int *, int);

// Returns true if the signal B matches signal A within error range.
bool irdebug_match_signal(unsigned int *, unsigned int *, unsigned int);

void irdebug_next_burst() {
  // Is this signal long enough?
  // Serial.print("i = ");
  // Serial.println(irdebug_signal_i);
  if (irdebug_signal_l > IRDEBUG_PULSE_MIN_LENGTH) {
    // Saving current burst/space length to the current buffer's index.
    irdebug_signal[irdebug_signal_i] = irdebug_signal_l;
    // Setting next signal's size to 0 and advancing one step.
    irdebug_signal_l = 0;
    irdebug_signal_i++;
  }
}

void irdebug_start_capture() {
  // Setting capture flag.
  irdebug_capturing = true;
  // Starting on zeroes.
  irdebug_signal_l  = 0;
  irdebug_signal_i  = 0;
}

void irdebug_stop_capture() {
  // Marking the end of the signal with a zero.
  irdebug_signal[irdebug_signal_i] = 0;
  // Setting capture flag to not capturing.
  irdebug_capturing = false;
  // Resetting time.
  irdebug_time = 0;
}

void irdebug_init() {
  if (irdebug_initialized == false) {
    irdebug_initialized = true;
    irdebug_signal_i    = 0;
    pinMode(IRDEBUG_RECV_PIN, INPUT);
    // This is used here just to reset values to their default value.
    irdebug_stop_capture();
  }
}

void irdebug_dump_signal(unsigned int *signal) {
  unsigned int i;
  for (i = 0; i < irdebug_signal_i; i++) {
    signal[i] = irdebug_signal[i];
  };
  signal[irdebug_signal_i] = 0;
}

void irdebug_print_signal(unsigned int *signal) {
  unsigned int i;
  Serial.println("unsigned int signal[] = {");
  for (i = 0; signal[i] > 0; i++) {
    if (i%2 == 0) {
      Serial.print("\t");
      Serial.print(signal[i]);
      Serial.print(", ");
    } else {
      Serial.print(signal[i]);
      Serial.println(",");
    }
  }
  Serial.println("0};");
}

// Taken from https://github.com/shirriff/Arduino-IRremote
void irdebug_send(unsigned int *buf, int khz) {
  unsigned int i;
  irdebug_enable_irout(khz);
  for (i = 0; buf[i] > 0; i++) {
    if (i%2 == 0) {
      irdebug_send_mark(buf[i]);
    } else {
      irdebug_send_space(buf[i]);
    }
  }
  irdebug_send_space(0);
}

void irdebug_send_mark(unsigned int ms) {
  TIMER_ENABLE_PWM;
  delayMicroseconds(ms);
}

void irdebug_send_space(unsigned int ms) {
  TIMER_DISABLE_PWM;
  delayMicroseconds(ms);
}

void irdebug_enable_irout(int khz) {
  TIMER_DISABLE_INTR;
  pinMode(IRDEBUG_OUTP_PIN, OUTPUT);
  digitalWrite(IRDEBUG_OUTP_PIN, LOW);
  TIMER_CONFIG_KHZ(khz);
}

bool irdebug_capture_signal(unsigned int *signal, int max_length) {
  bool captured = false;

  irdebug_init();

  irdebug_curr = micros();

  if (irdebug_time > 0) {

    irdebug_signal_on = (digitalRead(IRDEBUG_RECV_PIN) == LOW);

    irdebug_diff = irdebug_curr - irdebug_time;

    if (irdebug_capturing == false) {
      if (irdebug_signal_on) {
        irdebug_start_capture();
      }
    } else {
      if (irdebug_signal_on) {
        if (irdebug_signal_i%2 != 0) {
          irdebug_next_burst();
        }
      } else {
        if (irdebug_signal_i%2 == 0) {
          irdebug_next_burst();
        }
      }
      if (irdebug_signal_i < max_length) {
        irdebug_signal_l = irdebug_signal_l + irdebug_diff;
        if (irdebug_signal_l > IRDEBUG_PULSE_MAX_LENGTH) {
          irdebug_stop_capture();
          irdebug_dump_signal(signal);
          captured = true;
        }
      } else {
        irdebug_stop_capture();
      }
    }
  }

  irdebug_time = irdebug_curr;
  return captured;
}

bool irdebug_match_signal(unsigned int *a, unsigned int *b, unsigned int error) {
  unsigned int i;
  unsigned int diff;

  for (i = 0; i < IRDEBUG_SIGNAL_BUFSIZE; i++) {
    if (a[i] == 0 || b[i] == 0) {
      break;
    }

    if (a[i] > b[i]) {
      diff = a[i] - b[i];
    } else {
      diff = b[i] - a[i];
    }

    if (diff > error) {
      return false;
    }
  }

  return true;
}

#endif

//#include <irdebug.h>

#define HEXBUG_SPIDER_LEAD          1800, 450
#define HEXBUG_SPIDER_CONTROL_CODE  1800, 900
#define HEXBUG_SPIDER_B0            350,  550
#define HEXBUG_SPIDER_B1            350, 1450
#define HEXBUG_SPIDER_STOP          350, 0

#define HEXBUG_SPIDER_H0    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H1    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H2    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H3    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H4    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H5    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H6    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H7    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0
#define HEXBUG_SPIDER_H8    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_H9    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_HA    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_HB    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_HC    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_HD    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_HE    HEXBUG_SPIDER_B0, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1
#define HEXBUG_SPIDER_HF    HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1

#define HEXBUG_SPIDER_CHANNEL 'A'

#if HEXBUG_SPIDER_CHANNEL == 'A'
  #define HEXBUG_SPIDER_CONTROLLER_ID      HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B1

  #define BUTTON_RIGHT      HEXBUG_SPIDER_H4, HEXBUG_SPIDER_H6
  #define BUTTON_LEFT       HEXBUG_SPIDER_H3, HEXBUG_SPIDER_H8
  #define BUTTON_FORWARD    HEXBUG_SPIDER_H1, HEXBUG_SPIDER_HC
  #define BUTTON_BACKWARD   HEXBUG_SPIDER_H2, HEXBUG_SPIDER_HA
#else
  #define HEXBUG_SPIDER_CONTROLLER_ID      HEXBUG_SPIDER_B1, HEXBUG_SPIDER_B0

  #define BUTTON_RIGHT      HEXBUG_SPIDER_H4, HEXBUG_SPIDER_H7
  #define BUTTON_LEFT       HEXBUG_SPIDER_H3, HEXBUG_SPIDER_H9
  #define BUTTON_FORWARD    HEXBUG_SPIDER_H1, HEXBUG_SPIDER_HD
  #define BUTTON_BACKWARD   HEXBUG_SPIDER_H2, HEXBUG_SPIDER_HB
#endif

#define HEXBUG_SPIDER_RIGHT      0
#define HEXBUG_SPIDER_LEFT       1
#define HEXBUG_SPIDER_FORWARD    2
#define HEXBUG_SPIDER_BACKWARD   3

#define HEXBUG_SPIDER_SIGNALS (HEXBUG_SPIDER_BACKWARD + 1)

#define HEXBUG_SPIDER_KHZ    38

#define HEXBUG_SPIDER_SIGNAL(code) { HEXBUG_SPIDER_LEAD, HEXBUG_SPIDER_CONTROL_CODE, code, HEXBUG_SPIDER_CONTROLLER_ID, HEXBUG_SPIDER_STOP }

static unsigned int hexbug_spider_signal[HEXBUG_SPIDER_SIGNALS][26] = {
  HEXBUG_SPIDER_SIGNAL(BUTTON_RIGHT),
  HEXBUG_SPIDER_SIGNAL(BUTTON_LEFT),
  HEXBUG_SPIDER_SIGNAL(BUTTON_FORWARD),
  HEXBUG_SPIDER_SIGNAL(BUTTON_BACKWARD)
};

void hexbug_spider_send(int code) {
  irdebug_send(hexbug_spider_signal[code], HEXBUG_SPIDER_KHZ);
}

void hexbug_spider_left() {
  hexbug_spider_send(HEXBUG_SPIDER_LEFT);
}

void hexbug_spider_right() {
  hexbug_spider_send(HEXBUG_SPIDER_RIGHT);
}

void hexbug_spider_forward() {
  hexbug_spider_send(HEXBUG_SPIDER_FORWARD);
}

void hexbug_spider_backward() {
  hexbug_spider_send(HEXBUG_SPIDER_BACKWARD);
}
