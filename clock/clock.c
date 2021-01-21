// #define F_CPU 921600UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define kSetTCCR2 ((1 << CS22) | (1 << CS20)) // overflow at 1 Hz
#define kStayAwakeSeconds (30)

typedef struct {
  unsigned char second;
  unsigned char minute;
  unsigned char hour;
  unsigned char day;
} clock_t;

static volatile clock_t t;
static volatile unsigned char stayAwakeTimer;

static void init(void) {
  _delay_ms(1000);           // start up delay to allow xtal to stabilize
  stayAwakeTimer = kStayAwakeSeconds;
  DDRD |= (1 << 6);          // led as output

  ASSR |= (1 << AS2);        // enable asynchronous clock
  TCNT2 = 0;                 // reset counter / timer 2
  TCCR2 = kSetTCCR2;         // initialise prescaler for 1 Hz overflow
  while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {}

  TIMSK |= (1 << TOIE2);    // Set 8-bit Timer/Counter2 Overflow Interrupt Enable
  sei();
  set_sleep_mode(SLEEP_MODE_PWR_SAVE);
  sleep_enable();           // Enabling sleep mode
}

int main(void) {
  init();                 // Initialize registers and configure RTC.

  while (1) {

    if(stayAwakeTimer == 0){
      sleep_mode();           // Enter sleep mode.
      TCCR2 = kSetTCCR2;  // Write dummy value to control register
      while (ASSR & ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))) {}

      PORTD ^= (1 << 6);
    }

  }
  return 0;
}

ISR(TIMER2_OVF_vect) {
  if(stayAwakeTimer) stayAwakeTimer--;
  if (++t.second == 60)
  {
    t.second = 0;
    if (++t.minute == 60) {
      t.minute = 0;
      if (++t.hour == 24) {
        t.hour = 0;
        t.day++;
      }
    }
  }
}
