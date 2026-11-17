// TP7 - Entorno Gráfico (Informática II - Ingeniería Electrónica)
// Control bidireccional de 2 entradas digitales (E1, E2) y 2 salidas LED (L1, L2)
// entre Arduino UNO y Processing por puerto serie USB.

#include <Arduino.h>

// Pines de hardware
constexpr uint8_t PIN_E1 = 2;   // Pulsador / llave 1 (INPUT_PULLUP -> GND)
constexpr uint8_t PIN_E2 = 3;   // Pulsador / llave 2 (INPUT_PULLUP -> GND)
constexpr uint8_t PIN_L1 = 9;   // LED 1 (con resistencia de 220 ohm a GND)
constexpr uint8_t PIN_L2 = 10;  // LED 2 (con resistencia de 220 ohm a GND)

// Temporización
constexpr unsigned long BAUDIOS = 9600;
constexpr unsigned long DEBOUNCE_MS = 30;
constexpr unsigned long HEARTBEAT_MS = 100;

struct EntradaDigital {
  uint8_t pin;
  bool estadoEstable;
  bool ultimaLecturaCruda;
  unsigned long ultimoCambioMs;
};

EntradaDigital entrada1 = {PIN_E1, false, false, 0};
EntradaDigital entrada2 = {PIN_E2, false, false, 0};

bool estadoL1 = false;
bool estadoL2 = false;
unsigned long ultimoEnvioMs = 0;

void actualizarSalidas() {
  digitalWrite(PIN_L1, estadoL1 ? HIGH : LOW);
  digitalWrite(PIN_L2, estadoL2 ? HIGH : LOW);
}

void enviarEstadoSerial() {
  Serial.print(entrada1.estadoEstable ? 1 : 0);
  Serial.print(',');
  Serial.print(entrada2.estadoEstable ? 1 : 0);
  Serial.print(',');
  Serial.print(estadoL1 ? 1 : 0);
  Serial.print(',');
  Serial.print(estadoL2 ? 1 : 0);
  Serial.print('\n');
}

// Lee una entrada con INPUT_PULLUP (LOW = presionado = true) y aplica antirrebote.
// Devuelve true si hubo flanco de subida (0 -> 1) confirmado.
bool leerEntradaConFlanco(EntradaDigital& entrada, unsigned long ahoraMs, bool& cambioEstado) {
  bool lecturaActual = (digitalRead(entrada.pin) == LOW);
  cambioEstado = false;

  if (lecturaActual != entrada.ultimaLecturaCruda) {
    entrada.ultimoCambioMs = ahoraMs;
    entrada.ultimaLecturaCruda = lecturaActual;
  }

  if ((ahoraMs - entrada.ultimoCambioMs) >= DEBOUNCE_MS) {
    if (lecturaActual != entrada.estadoEstable) {
      bool flancoSubida = (!entrada.estadoEstable && lecturaActual);
      entrada.estadoEstable = lecturaActual;
      cambioEstado = true;
      return flancoSubida;
    }
  }

  return false;
}

bool procesarComandosSerial() {
  bool huboCambio = false;

  while (Serial.available() > 0) {
    char cmd = static_cast<char>(Serial.read());
    switch (cmd) {
      case 'A':
        estadoL1 = true;
        huboCambio = true;
        break;
      case 'a':
        estadoL1 = false;
        huboCambio = true;
        break;
      case 'B':
        estadoL2 = true;
        huboCambio = true;
        break;
      case 'b':
        estadoL2 = false;
        huboCambio = true;
        break;
      default:
        break;
    }
  }

  if (huboCambio) {
    actualizarSalidas();
  }
  return huboCambio;
}

void setup() {
  Serial.begin(BAUDIOS);

  pinMode(PIN_E1, INPUT_PULLUP);
  pinMode(PIN_E2, INPUT_PULLUP);
  pinMode(PIN_L1, OUTPUT);
  pinMode(PIN_L2, OUTPUT);

  actualizarSalidas();
  enviarEstadoSerial();
}

void loop() {
  const unsigned long ahoraMs = millis();
  bool cambioE1 = false;
  bool cambioE2 = false;

  // Punto 3: al presionar E1 o E2 en Arduino, conmuta el LED correspondiente
  if (leerEntradaConFlanco(entrada1, ahoraMs, cambioE1)) {
    estadoL1 = !estadoL1;
    actualizarSalidas();
  }

  if (leerEntradaConFlanco(entrada2, ahoraMs, cambioE2)) {
    estadoL2 = !estadoL2;
    actualizarSalidas();
  }

  // Punto 2: comandos enviados desde Processing al hacer clic en L1 o L2
  const bool cambioSerial = procesarComandosSerial();

  // Punto 1 y 4: notificar estado actualizado a Processing
  if (cambioE1 || cambioE2 || cambioSerial || (ahoraMs - ultimoEnvioMs >= HEARTBEAT_MS)) {
    ultimoEnvioMs = ahoraMs;
    enviarEstadoSerial();
  }
}
