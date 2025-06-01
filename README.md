# Practica2_Instrumentaci-n_Grupo5
Este proyecto diseñado para monitorear y visualizar en tiempo real las condiciones climáticas de un entorno utilizando un Arduino Uno. Emplea varios sensores para medir la temperatura, humedad, luz ambiental, velocidad del viento y calidad del aire. 
![image](https://github.com/user-attachments/assets/43fccaac-2b59-4f20-958a-c29d7551b0bb)

# COMPONENTES
-Arduino Uno: Es el microcontrolador principal que gestiona todo el sistema y controla la lectura de sensores, el procesamiento de datos y la activación de actuadores.
-Sensor DHT22: Se utiliza para medir la temperatura y la humedad del ambiente con buena precisión.
-Sensor LDR (resistencia dependiente de luz): Permite medir la intensidad de luz ambiental. Se conecta como divisor de voltaje en una entrada analógica.
-Sensor de viento (analógico): Detecta la velocidad del viento mediante una señal analógica que se interpreta con analogRead().
-Sensor de calidad del aire (tipo MQ): Mide la calidad del aire en forma de concentración de partículas, también mediante señal analógica.
-Servo motor SG90: Representa una válvula que se abre o se cierra dependiendo de la temperatura. Es controlado por señal PWM desde un pin digital.
-Pantalla LCD I2C 20x4: Muestra en tiempo real las mediciones de todos los sensores, así como el estado del sistema (por ejemplo, "Calentando", "Enfriando").
-Registro de desplazamiento 74HC595: Permite controlar varios LEDs con pocos pines del Arduino, activando cada uno por turnos según el nivel de luz ambiental.
-8 LEDs: Funcionan como una barra indicadora del nivel de luz. Se encienden en secuencia según la cantidad de luz medida por el sensor LDR.
-Resistencias: Utilizadas principalmente para limitar la corriente en los LEDs y en el divisor de voltaje del LDR.
-Cables y protoboard (opcional): Utilizados para realizar todas las conexiones entre componentes de manera ordenada y segura durante el prototipado.

# CONEXIONES DEL SISTEMA
A continuación, las conexiones clave entre los componentes y el Arduino Uno:
# Sensores
DHT22:
Señal → Pin digital 7
VCC → 5V
GND → GND
LDR:
Señal → A0 (con resistencia en divisor de voltaje)
Sensor de viento:
Señal → A1
Sensor de calidad del aire:
Señal → A2
# LCD I2C 20x4
SDA → A4
SCL → A5
VCC → 5V
GND → GND
# Registro 74HC595 (para LEDs)
DATA_PIN (DS) → Pin 10
CLOCK_PIN (SHCP) → Pin 8
LATCH_PIN (STCP) → Pin 9
Q0–Q7 del 74HC595 → Ánodos de los LEDs (con resistencias)
LEDs → GND (cátodos)
# Servo SG90
Señal → Pin 6
VCC → 5V (preferible fuente externa con GND común)
GND → GND


# FUNCIONAMIENTO DEL SISTEMA
El sistema funciona como una estación de monitoreo climático inteligente, basada en Arduino. Cada cierto intervalo (2 segundos), el sistema realiza las siguientes acciones:
-Toma lecturas de sensores:
Temperatura y humedad con el sensor DHT22.
Nivel de luz con un sensor LDR.
Velocidad del viento y calidad del aire con sensores analógicos.
-Procesa los datos:
Convierte las lecturas analógicas en valores comprensibles (porcentajes o velocidades).
Calcula el nivel de luz y lo representa con 8 LEDs, encendidos progresivamente según la intensidad.
-Controla el sistema de climatización:
Si la temperatura es mayor a 28 °C, activa el servo en posición de "enfriamiento".
Si es menor a 22 °C, lo pone en modo "calentamiento".
Entre ambos valores, se mantiene en una "zona muerta", sin acción.
-Muestra la información:
Imprime todos los datos en una pantalla LCD I2C 20x4.
También los envía al monitor serial para visualización en el PC.

El sistema reacciona automáticamente a los cambios de temperatura y registra los cambios de estado (por ejemplo, de "calentando" a "enfriando"), informando al usuario.

