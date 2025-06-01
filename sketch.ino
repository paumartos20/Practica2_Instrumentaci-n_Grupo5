#include <Wire.h>                    
#include <LiquidCrystal_I2C.h>       // Librería para manejar pantallas LCD por I2C
#include <DHT.h>                     // Librería para el sensor de temperatura y humedad DHT
#include <Servo.h>                   

// ----- Pines y constantes -----
#define DHTPIN 7                     // Pin digital donde está conectado el sensor DHT22
#define DHTTYPE DHT22                // Tipo de sensor DHT que se está utilizando
#define LDR_PIN A0                   // Pin analógico para sensor de luz (LDR)
#define VIENTO_PIN A1                // Pin analógico para sensor de viento
#define AIRE_PIN A2                  // Pin analógico para sensor de calidad de aire

#define LATCH_PIN 9                  // Pin de latch para el registro de desplazamiento 74HC595
#define CLOCK_PIN 8                  // Pin de reloj para el 74HC595
#define DATA_PIN 10                  // Pin de datos para el 74HC595

#define SERVO_PIN 6                  // Pin PWM para controlar el servo SG90

// ----- Objetos -----
DHT dht(DHTPIN, DHTTYPE);            // Instancia del sensor DHT
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Instancia del LCD con dirección I2C 0x27, tamaño 20x4
Servo servoValvula;                  // Objeto para controlar el servo

// ----- Variables -----
byte ledNivel = 0;                   // Nivel de iluminación en LEDs (0 a 8)
float tempUltima = 0.0;              // Última temperatura leída
float tempAnterior = 0.0;            // Temperatura anterior a la última
String estadoActual = "Zona muerta"; // Estado actual del sistema (según temperatura)
String estadoAnterior = "Zona muerta"; // Estado anterior (para comparar cambios)

// ----- Función de actualización LEDs (iluminación) -----
void actualizarIluminacion(byte nivel) {
  byte salida = 0;
  for (int i = 0; i < nivel; i++) {
    salida |= (1 << i);              // Enciende los primeros "nivel" LEDs (usa desplazamiento de bits)
  }
  digitalWrite(LATCH_PIN, LOW);      // Comienza la transmisión de datos al 74HC595
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, salida); // Envia los bits al registro de desplazamiento
  digitalWrite(LATCH_PIN, HIGH);     // Finaliza la transmisión, actualiza la salida
}

void setup() {
  Serial.begin(9600);                // Inicia comunicación por monitor serial
  dht.begin();                       // Inicia sensor DHT
  lcd.init();                        // Inicializa el LCD
  lcd.backlight();                   // Enciende la luz de fondo del LCD

  // Pines del 74HC595
  pinMode(LATCH_PIN, OUTPUT);        // Configura pin de latch como salida
  pinMode(CLOCK_PIN, OUTPUT);        // Configura pin de reloj como salida
  pinMode(DATA_PIN, OUTPUT);         // Configura pin de datos como salida

  // Servo
  servoValvula.attach(SERVO_PIN);    // Asocia el servo al pin correspondiente
  servoValvula.write(90);            // Posición inicial del servo (zona muerta)

  lcd.setCursor(0, 0);               
  lcd.print("Monitorizacion Clima"); // Mensaje de bienvenida en LCD
  delay(2000);                       
  lcd.clear();                       
}

void loop() {
  delay(2000);                       // Espera de 2 segundos entre lecturas

  // ----- Lecturas -----
  float temperatura = dht.readTemperature();        // Lee temperatura del DHT
  float humedad = dht.readHumidity();               // Lee humedad del DHT
  int ldrValor = analogRead(LDR_PIN);               // Lectura analógica de LDR
  int vientoValor = analogRead(VIENTO_PIN);         // Lectura analógica de sensor de viento
  int aireValor = analogRead(AIRE_PIN);             // Lectura analógica de calidad del aire

  float luz = map(ldrValor, 0, 1023, 0, 100);        // Convierte valor de LDR a porcentaje de luz
  float viento = map(vientoValor, 0, 1023, 0, 150);  // Convierte lectura de viento a velocidad (km/h aprox.)
  float calidad = map(aireValor, 0, 1023, 0, 100);   // Convierte lectura de aire a calidad (porcentaje)

  // ----- Control iluminación con LEDs -----
  ledNivel = map(luz, 0, 100, 0, 8);                 // Convierte porcentaje de luz a número de LEDs
  actualizarIluminacion(ledNivel);                  // Actualiza visualización de iluminación con LEDs

  // ----- Control de temperatura -----
  tempAnterior = tempUltima;                        // Guarda la última temperatura
  tempUltima = temperatura;                         // Actualiza la temperatura actual

  if (temperatura > 28.0) {
    servoValvula.write(180);                        // Gira el servo para enfriar
    estadoActual = "Enfriando";                     
  } else if (temperatura < 22.0) {
    servoValvula.write(0);                          // Gira el servo para calentar
    estadoActual = "Calentando";
  } else {
    servoValvula.write(90);                         // Zona neutra (sin acción)
    estadoActual = "Zona muerta";
  }

  // ----- Mostrar datos en LCD -----
  lcd.clear();                                       // Limpia pantalla

  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperatura, 1);                         // Muestra temperatura con 1 decimal
  lcd.print((char)223);                              // Carácter del símbolo de grados
  lcd.print("C Hum:");
  lcd.print(humedad, 0);                             // Muestra humedad sin decimales
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Luz:");
  lcd.print(luz, 0);                                 // Porcentaje de luz
  lcd.print("% Vnt:");
  lcd.print(viento, 0);                              // Velocidad del viento

  lcd.setCursor(0, 2);
  lcd.print("Aire:");
  lcd.print(calidad, 0);                             // Calidad del aire
  lcd.print("% LEDs:");
  lcd.print(ledNivel);                               // Nivel de LEDs encendidos

  lcd.setCursor(0, 3);
  if (estadoActual != estadoAnterior && estadoActual != "Zona muerta") {
    lcd.print(estadoActual);                         // Muestra nuevo estado
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(estadoActual);                         // Muestra transición de temperatura
    lcd.setCursor(0, 1);
    lcd.print("De ");
    lcd.print(tempAnterior, 1);
    lcd.print((char)223);
    lcd.print(" a ");
    lcd.print(tempUltima, 1);
    lcd.print((char)223);
    delay(3000);
  } else {
    lcd.print("Estado: ");
    lcd.print(estadoActual);                         // Muestra estado actual si no hay cambio
  }

  // ----- Serial Monitor -----
  Serial.print("Temp: "); Serial.print(temperatura); Serial.print(" C | ");
  Serial.print("Hum: "); Serial.print(humedad); Serial.print(" % | ");
  Serial.print("Luz: "); Serial.print(luz); Serial.print(" % | ");
  Serial.print("Viento: "); Serial.print(viento); Serial.print(" km/h | ");
  Serial.print("Calidad: "); Serial.print(calidad); Serial.print(" % | ");
  Serial.print("LEDs: "); Serial.print(ledNivel); Serial.print(" | ");
  Serial.print("Servo: "); Serial.println(estadoActual); // Muestra todos los datos por consola

  estadoAnterior = estadoActual;                     // Actualiza el estado anterior
}



