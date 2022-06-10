#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include "TimerOne.h"

#define NUM_LEDS 3
#define DATA_PIN 8

#define PIN_SW1 9
#define PIN_SW2 10
#define PIN_SW3 11

#define ESTADO_BOTON_ESPERA 0
#define ESTADO_BOTON_CONFIRMACION 1
#define ESTADO_BOTON_LIBERACION 2

#define ESTADO_MAQUINA_ESPERA 0
#define ESTADO_MAQUINA_SELECCION 1
#define ESTADO_MAQUINA_SIRVIENDO_A 2
#define ESTADO_MAQUINA_SIRVIENDO_B 3
#define ESTADO_MAQUINA_SIRVIENDO_C 4
#define ESTADO_MAQUINA_EXIT 5
#define ESTADO_MAQUINA_LIMPIEZA 6
#define ESTADO_MAQUINA_ERROR_A 7
#define ESTADO_MAQUINA_ERROR_B 8

#define ESTADO_BOTON_INICIAL ESTADO_BOTON_ESPERA

#define DELAY_MS_BOTON 25
#define DELAY_MS_SERVIR 5000
#define DELAY_MS_EXIT 2000
#define DELAY_MS_ERROR 3000
#define DELAY_S_LIMPIAR 10

int estado_FSM_boton1, estado_FSM_boton2, estado_FSM_boton3, estado_FSM_maquina;
int ms_boton1, ms_boton2, ms_boton3, ms, sec;
int flag_boton1, flag_boton2, flag_boton3;
int limpieza, num;

Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ----------------------------------------------------------------------------------------------------- */

void setup() {
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);
  pinMode(PIN_SW3, INPUT_PULLUP);

  estado_FSM_boton1 = ESTADO_BOTON_INICIAL;
  estado_FSM_maquina = ESTADO_MAQUINA_LIMPIEZA;

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("\n");

  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.setPixelColor(1, pixels.Color(255, 0, 0));
  pixels.setPixelColor(2, pixels.Color(255, 0, 0));
  pixels.show();

  lcd.begin();
  // lcd.init();
  lcd.backlight();

  // Inicializo el Timer One
  Timer1.initialize(1000);
  Timer1.attachInterrupt(ISR_Timer);
  ms = 0;
}


void loop() {
  maquinaEstadosBoton1();
  maquinaEstadosBoton2();
  maquinaEstadosBoton3();
  maquinaEstadoMaquina();
}

/* ----------------------------------------------------------------------------------------------------- */

void maquinaEstadoMaquina(void) {
  switch (estado_FSM_maquina) {

    case ESTADO_MAQUINA_LIMPIEZA:
      if (digitalRead(PIN_SW1) == 0) {
        limpieza = 1;
      }
      if (limpieza == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Limpiando...");
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));
        pixels.setPixelColor(1, pixels.Color(0, 0, 255));
        pixels.setPixelColor(2, pixels.Color(0, 0, 255));
        pixels.show();
        if (ms > 1000) {
          sec++;
          ms = 0;
        }
        if (sec == DELAY_S_LIMPIAR) {
          lcd.clear();
          pixels.setPixelColor(0, pixels.Color(255, 0, 0));
          pixels.setPixelColor(1, pixels.Color(255, 0, 0));
          pixels.setPixelColor(2, pixels.Color(255, 0, 0));
          pixels.show();
          estado_FSM_maquina = ESTADO_MAQUINA_ESPERA;
          flag_boton1 = 0;
        }
      }
      else {
        estado_FSM_maquina = ESTADO_MAQUINA_ESPERA;
        ms = 0;
      }
      break;

    case ESTADO_MAQUINA_ESPERA:
      lcd.setCursor(0, 0);
      lcd.print("Pase su tarjeta");
      num = Serial1.parseInt();
      if (num == 1) {
        flag_boton1 = 0;
        flag_boton2 = 0;
        flag_boton3 = 0;
        estado_FSM_maquina = ESTADO_MAQUINA_SELECCION;
        lcd.clear();
      }
      if (num == 2) {
        ms = 0;
        estado_FSM_maquina = ESTADO_MAQUINA_ERROR_A;
        lcd.clear();
      }
      break;

    case ESTADO_MAQUINA_SELECCION:
      lcd.setCursor(0, 0);
      lcd.print("Eliga una bebida");
      if (flag_boton1 == 1) {
        flag_boton1 = 0;
        lcd.clear();
        estado_FSM_maquina = ESTADO_MAQUINA_SIRVIENDO_A;
        ms = 0;
      }
      if (flag_boton2 == 1) {
        flag_boton2 = 0;
        lcd.clear();
        estado_FSM_maquina = ESTADO_MAQUINA_SIRVIENDO_B;
        ms = 0;
      }
      if (flag_boton3 == 1) {
        flag_boton3 = 0;
        lcd.clear();
        estado_FSM_maquina = ESTADO_MAQUINA_SIRVIENDO_C;
        ms = 0;
      }
      break;

    case ESTADO_MAQUINA_SIRVIENDO_A:
      lcd.setCursor(0, 0);
      lcd.print("Sirviendo...");
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();
      if (ms > DELAY_MS_SERVIR) {
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        pixels.show();
        lcd.clear();
        estado_FSM_maquina = ESTADO_MAQUINA_EXIT;
        ms = 0;
      }
      break;

    case ESTADO_MAQUINA_SIRVIENDO_B:
      lcd.setCursor(0, 0);
      lcd.print("Sirviendo...");
      pixels.setPixelColor(1, pixels.Color(0, 255, 0));
      pixels.show();
      if (ms > DELAY_MS_SERVIR) {
        pixels.setPixelColor(1, pixels.Color(255, 0, 0));
        pixels.show();
        lcd.clear();
        estado_FSM_maquina = ESTADO_MAQUINA_EXIT;
        ms = 0;
      }
      break;

    case ESTADO_MAQUINA_SIRVIENDO_C:
      lcd.setCursor(0, 0);
      lcd.print("Sirviendo...");
      pixels.setPixelColor(2, pixels.Color(0, 255, 0));
      pixels.show();
      if (ms > DELAY_MS_SERVIR) {
        pixels.setPixelColor(2, pixels.Color(255, 0, 0));
        pixels.show();
        lcd.clear();
        estado_FSM_maquina = ESTADO_MAQUINA_EXIT;
        ms = 0;
      }
      break;

    case ESTADO_MAQUINA_EXIT:
      lcd.setCursor(0, 0);
      lcd.print("Muchas Gracias");
      if (ms > DELAY_MS_EXIT) {
        estado_FSM_maquina = ESTADO_MAQUINA_ESPERA;
        lcd.clear();
        flag_boton1 = 0;
        flag_boton2 = 0;
        flag_boton3 = 0;
        ms = 0;
        num = 0;
        Serial1.println("4");
        Serial1.println("0");
      }
      break;

    case ESTADO_MAQUINA_ERROR_A:
      lcd.setCursor(0, 0);
      lcd.print("Tarjeta no");
      lcd.setCursor(0, 1);
      lcd.print("registrada");
      if (ms >= DELAY_MS_ERROR) {
        estado_FSM_maquina = ESTADO_MAQUINA_ERROR_B;
        lcd.clear();
        ms = 0;
      }
      break;

    case ESTADO_MAQUINA_ERROR_B:
      lcd.setCursor(0, 0);
      lcd.print("o saldo");
      lcd.setCursor(0, 1);
      lcd.print("insuficiente");
      if (ms >= DELAY_MS_ERROR) {
        estado_FSM_maquina = ESTADO_MAQUINA_ESPERA;
        flag_boton1 = 0;
        flag_boton2 = 0;
        flag_boton3 = 0;
        Serial1.println("4");
        Serial1.println("0");
        lcd.clear();
      }
      break;
  }
}

void maquinaEstadosBoton1(void) {
  char estado_boton;

  switch (estado_FSM_boton1) {
    case ESTADO_BOTON_ESPERA:
      estado_boton = digitalRead(PIN_SW1);

      if (estado_boton == 0) {
        ms_boton1 = 0;
        estado_FSM_boton1 = ESTADO_BOTON_CONFIRMACION;
      }
      break;

    case ESTADO_BOTON_CONFIRMACION:
      estado_boton = digitalRead(PIN_SW1);

      if (estado_boton == 0 && ms_boton1 >= DELAY_MS_BOTON) {
        estado_FSM_boton1 = ESTADO_BOTON_LIBERACION;
      }

      if (estado_boton == 1 && ms_boton1 < DELAY_MS_BOTON) {
        estado_FSM_boton1 = ESTADO_BOTON_ESPERA;
      }
      break;

    case ESTADO_BOTON_LIBERACION:
      estado_boton = digitalRead(PIN_SW1);

      if (estado_boton == 1) {
        flag_boton1 = 1;
        estado_FSM_boton1 = ESTADO_BOTON_ESPERA;
      }
      break;
  }
}

void maquinaEstadosBoton2(void) {
  char estado_boton;

  switch (estado_FSM_boton2) {
    case ESTADO_BOTON_ESPERA:
      estado_boton = digitalRead(PIN_SW2);

      if (estado_boton == 0) {
        ms_boton2 = 0;
        estado_FSM_boton2 = ESTADO_BOTON_CONFIRMACION;
      }
      break;

    case ESTADO_BOTON_CONFIRMACION:
      estado_boton = digitalRead(PIN_SW2);

      if (estado_boton == 0 && ms_boton2 >= DELAY_MS_BOTON) {
        estado_FSM_boton2 = ESTADO_BOTON_LIBERACION;
      }

      if (estado_boton == 1 && ms_boton2 < DELAY_MS_BOTON) {
        estado_FSM_boton2 = ESTADO_BOTON_ESPERA;
      }
      break;

    case ESTADO_BOTON_LIBERACION:
      estado_boton = digitalRead(PIN_SW2);

      if (estado_boton == 1) {
        flag_boton2 = 1;
        estado_FSM_boton2 = ESTADO_BOTON_ESPERA;
      }
      break;
  }
}

void maquinaEstadosBoton3(void) {
  char estado_boton;

  switch (estado_FSM_boton3) {
    case ESTADO_BOTON_ESPERA:
      estado_boton = digitalRead(PIN_SW3);

      if (estado_boton == 0) {
        ms_boton3 = 0;
        estado_FSM_boton3 = ESTADO_BOTON_CONFIRMACION;
      }
      break;

    case ESTADO_BOTON_CONFIRMACION:
      estado_boton = digitalRead(PIN_SW3);

      if (estado_boton == 0 && ms_boton3 >= DELAY_MS_BOTON) {
        estado_FSM_boton3 = ESTADO_BOTON_LIBERACION;
      }

      if (estado_boton == 1 && ms_boton3 < DELAY_MS_BOTON) {
        estado_FSM_boton3 = ESTADO_BOTON_ESPERA;
      }
      break;

    case ESTADO_BOTON_LIBERACION:
      estado_boton = digitalRead(PIN_SW3);

      if (estado_boton == 1) {
        flag_boton3 = 1;
        estado_FSM_boton3 = ESTADO_BOTON_ESPERA;
      }
      break;
  }
}

void ISR_Timer(void) {
  ms++;
  ms_boton1++;
  ms_boton2++;
  ms_boton3++;
}
