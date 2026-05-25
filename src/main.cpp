/*
#include <Arduino.h>

#define PIN_BRUSHLESS_MOTOR 18
#define CHANNEL_0 0
#define RESOLUTION_BITS 16
#define FREQ_HZ 50
#define PWM_MAX ((1 << RESOLUTION_BITS) - 1)

float rapport_cyclique = 5; // Valeur de départ = 5%

// Convertit un pourcentage (0 à 100) en valeur de rapport cyclique PWM
uint32_t pourcentageEnRapportCyclique(float pourcentage)
{
  pourcentage = constrain(pourcentage, 0, 100);
  // Serial.printf("pourcentage %f\n", pourcentage);
  pourcentage = (pourcentage / 100.0) * PWM_MAX;
  return pourcentage;
}

void move_brushless(int rapport_cyclique)
{
  // On contraint ici entre 5% et 10% pour rester dans la plage valide de l’ESC
  rapport_cyclique = constrain(rapport_cyclique, 5, 10);
  ledcWrite(CHANNEL_0, pourcentageEnRapportCyclique(rapport_cyclique));
  Serial.printf("Commande moteur : %d%%\n", rapport_cyclique);
}

void init_esc(bool _active_sequence)
{
  if (_active_sequence)
  {
    Serial.println("Mise au min des gaz (5%)");
    move_brushless(5);
    delay(5000);

    Serial.println("Mise au max des gaz (10%)");
    move_brushless(10);
    delay(2000);

    Serial.println("Retour au min des gaz (5%)");
    move_brushless(5);
    delay(2000);

    Serial.println("Calibrage terminé !");
  }
}

void setup()
{
  Serial.begin(115200);
  ledcSetup(CHANNEL_0, FREQ_HZ, RESOLUTION_BITS);
  ledcAttachPin(PIN_BRUSHLESS_MOTOR, CHANNEL_0);
  init_esc(true);
}

void loop()
{
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == 'p') // Augmente la vitesse
    {
      rapport_cyclique += 1;
    }
    else if (c == 'm') // Diminue la vitesse
    {
      rapport_cyclique -= 1;
    }
    else if (c == 's') // Stoppe le moteur (retour au min)
    {
      rapport_cyclique = 5;
      Serial.println("Moteur stoppé (5%)");
    }
    rapport_cyclique = constrain(rapport_cyclique, 5, 10);
    // Appliquer le rapport cyclique contraint
    move_brushless(rapport_cyclique);
  }
}
*/

// ---------------- Code non tester encore mais a l'air bien fonction
#include <Arduino.h>

#define PIN_BRUSHLESS_MOTOR 18

#define CHANNEL_0 0
#define PWM_FREQ 50
#define PWM_RESOLUTION 16

#define PWM_MAX ((1 << PWM_RESOLUTION) - 1)

// Plage standard ESC
#define ESC_MIN_US 1000
#define ESC_MAX_US 2000

// Période à 50 Hz = 20 ms = 20000 µs
#define PWM_PERIOD_US 20000

// Valeur actuelle
int throttle_us = ESC_MIN_US;

///////////////////////////////////////////////////////////
// Convertit des microsecondes en valeur PWM ESP32
///////////////////////////////////////////////////////////
uint32_t usToDutyCycle(int pulse_us)
{
  pulse_us = constrain(pulse_us, ESC_MIN_US, ESC_MAX_US);

  float duty = (float)pulse_us / PWM_PERIOD_US;

  return (uint32_t)(duty * PWM_MAX);
}

///////////////////////////////////////////////////////////
// Envoi commande ESC
///////////////////////////////////////////////////////////
void move_brushless(int pulse_us)
{
  pulse_us = constrain(pulse_us, ESC_MIN_US, ESC_MAX_US);

  ledcWrite(CHANNEL_0, usToDutyCycle(pulse_us));

  Serial.printf("Commande ESC : %d us\n", pulse_us);
}

///////////////////////////////////////////////////////////
// Calibration ESC
///////////////////////////////////////////////////////////
void init_esc(bool active_sequence)
{
  if (active_sequence)
  {
    Serial.println("ESC -> minimum");

    move_brushless(ESC_MIN_US);
    delay(5000);

    Serial.println("ESC -> maximum");

    move_brushless(ESC_MAX_US);
    delay(2000);

    Serial.println("ESC -> minimum");

    move_brushless(ESC_MIN_US);
    delay(2000);

    Serial.println("Calibration terminee");
  }
}

///////////////////////////////////////////////////////////
// SETUP
///////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);

  ledcSetup(CHANNEL_0, PWM_FREQ, PWM_RESOLUTION);

  ledcAttachPin(PIN_BRUSHLESS_MOTOR, CHANNEL_0);

  init_esc(true);

  Serial.println("Commandes :");
  Serial.println("p = + vitesse");
  Serial.println("m = - vitesse");
  Serial.println("s = stop");
}

///////////////////////////////////////////////////////////
// LOOP
///////////////////////////////////////////////////////////
void loop()
{
  if (Serial.available())
  {
    char c = Serial.read();

    if (c == 'p')
    {
      throttle_us += 25;
    }
    else if (c == 'm')
    {
      throttle_us -= 25;
    }
    else if (c == 's')
    {
      throttle_us = ESC_MIN_US;
    }

    throttle_us = constrain(throttle_us, ESC_MIN_US, ESC_MAX_US);

    move_brushless(throttle_us);
  }
}