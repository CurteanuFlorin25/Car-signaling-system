#include <avr/io.h>
#include <util/delay.h>

unsigned int push0 = 0;
unsigned int push1 = 0;
unsigned int push2 = 0;

unsigned int lastPush0 = 0;
unsigned int lastPush1 = 0;
unsigned int lastPush2 = 0;

#define SISTEM_INACTIV 0
#define SEMNALIZARE_DREAPTA 1
#define SEMNALIZARE_STANGA 2
#define AVARII 3

int stare = SISTEM_INACTIV;
int stare_anterioara = SISTEM_INACTIV;

void initializare()
{
  DDRD |= 0b00001100;
  DDRB &= ~(0b00111000);
}

void citeste_butoane()
{
  push0 = (PINB & 0b00001000) ? 1 : 0;
  push1 = (PINB & 0b00010000) ? 1 : 0;
  push2 = (PINB & 0b00100000) ? 1 : 0;
}

void setup()
{
  initializare();
}

void loop()
{
  citeste_butoane();

  // Buton AVARII (toggle)
  if (push2 && !lastPush2)
  {
    if (stare == AVARII)
      stare = stare_anterioara;  // Revine la starea de dinainte
    else {
      stare_anterioara = stare;  // Salvează starea curentă
      stare = AVARII;
    }
  }

  // Buton STANGA (toggle) – doar dacă nu suntem în AVARII
  if (push0 && !lastPush0 && stare != AVARII)
  {
    if (stare == SEMNALIZARE_STANGA)
      stare = SISTEM_INACTIV;
    else
      stare = SEMNALIZARE_STANGA;
  }

  // Buton DREAPTA (toggle) – doar dacă nu suntem în AVARII
  if (push1 && !lastPush1 && stare != AVARII)
  {
    if (stare == SEMNALIZARE_DREAPTA)
      stare = SISTEM_INACTIV;
    else
      stare = SEMNALIZARE_DREAPTA;
  }

  // Salvăm starea butoanelor pentru toggle corect
  lastPush0 = push0;
  lastPush1 = push1;
  lastPush2 = push2;

  // Execuția semnalizării
  switch (stare)
  {
    case SISTEM_INACTIV:
      PORTD &= ~(0b00001100);  // Oprire LED-uri
      _delay_ms(100);          // Delay minim pentru stabilitate
      break;

    case SEMNALIZARE_DREAPTA:
      PORTD |= 0b00000100;     // LED dreapta ON (D2)
      _delay_ms(250);
      PORTD &= ~(0b00000100);  // LED dreapta OFF
      _delay_ms(250);
      break;

    case SEMNALIZARE_STANGA:
      PORTD |= 0b00001000;     // LED stanga ON (D3)
      _delay_ms(250);
      PORTD &= ~(0b00001000);  // LED stanga OFF
      _delay_ms(250);
      break;

    case AVARII:
      PORTD |= 0b00001100;     // Ambele LED-uri ON
      _delay_ms(250);
      PORTD &= ~(0b00001100);  // Ambele LED-uri OFF
      _delay_ms(250);
      break;
  }
}

int main()
{
  setup();
  while (1)
  {
    loop();
  }
}
