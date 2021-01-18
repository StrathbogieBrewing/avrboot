#define F_CPU 8000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

typedef struct {
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char date;
  unsigned char month;
  unsigned char year;
} time;

time t;

static void init(void) {
  // Wait for external clock crystal to stabilize;
  // for (uint8_t i = 0; i < 0x40; i++) {
  //   for (int j = 0; j < 0xFFFF; j++)
  //     ;
  // }

  _delay_ms(1000);
  // DDRB = 0xFF;											//Configure all eight pins of
  // port B as outputs
  // TIMSK &= ~((1 << TOIE0); // Make sure TC0 interrupts are disabled
  ASSR |= (1 << AS2);
      // set Timer/counter0 to be asynchronous from the CPU clock
                  // with a second external clock (32,768kHz)driving it.
  // TCNT2 = 0; // Reset timer
  // OCR2 = 127;

  TCCR2 = ((1 << CS22) | (1 << CS20) );

  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) |
                 (1 << TCR2UB))) {}// Wait until TC0 is updated

  TIMSK |= (1 << TOIE2); // Set 8-bit Timer/Counter0 Overflow Interrupt Enable
  sei();                 // Set the Global Interrupt Enable Bit
  set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Selecting power save mode as the sleep
                                       // mode to be used
  sleep_enable(); // Enabling sleep mode
}

int main(void) {

  init(); // Initialize registers and configure RTC.

  while (1) {

    DDRD |= (1 << 6);
    PORTD |= (1 << 6);

    sleep_mode(); // Enter sleep mode.
    TCCR2 = ((1 << CS22) | (1 << CS20) ); // Write dummy value to control register
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {}
      // Wait until TC0 is updated

    PORTD &= ~(1 << 6);

    sleep_mode(); // Enter sleep mode.
    TCCR2 = ((1 << CS22) | (1 << CS20) ); // Write dummy value to control register
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {}
      // Wait until TC0 is updated

  }
  return 0;
}

static char not_leap(void) // check for leap year
{
  if (!(t.year % 100)) {
    return (char)(t.year % 400);
  } else {
    return (char)(t.year % 4);
  }
}

ISR(TIMER2_OVF_vect) {
  if (++t.second == 60) // keep track of time, date, month, and year
  {
    t.second = 0;
    if (++t.minute == 60) {
      t.minute = 0;
      if (++t.hour == 24) {
        t.hour = 0;
        if (++t.date == 32) {
          t.month++;
          t.date = 1;
        } else if (t.date == 31) {
          if ((t.month == 4) || (t.month == 6) || (t.month == 9) ||
              (t.month == 11)) {
            t.month++;
            t.date = 1;
          }
        } else if (t.date == 30) {
          if (t.month == 2) {
            t.month++;
            t.date = 1;
          }
        } else if (t.date == 29) {
          if ((t.month == 2) && (not_leap())) {
            t.month++;
            t.date = 1;
          }
        }
        if (t.month == 13) {
          t.month = 1;
          t.year++;
        }
      }
    }
  }
  // PORTB = ~(((t.second & 0x01) | t.minute << 1) | t.hour << 7);
}
