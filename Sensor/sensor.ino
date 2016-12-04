// Programa : Alarme com sensor de gas teste com butano: (apenas para prototipo)
// Autor : Grupo Izone

// Definicoes dos pinos dos leds e buzzer
int pin_led_verm = 8;
int pin_led_verde = 9;
int pin_buzzer = 10;
int pin_d0 = 7;
int pin_a0 = A2;

int nivel_sensor = 300;

void setup()
{
  // Define os pinos de leitura do sensor como entrada
  pinMode(pin_d0, INPUT);
  pinMode(pin_a0, INPUT);
  // Define pinos leds e buzzer como saida
  pinMode(pin_led_verm, OUTPUT);
  pinMode(pin_led_verde, OUTPUT);
  pinMode(pin_buzzer, OUTPUT);
  // Inicializa a serial
  Serial.begin(9600);
}

void loop()
{
  // Le os dados do pino digital D0 do sensor
  int valor_digital = digitalRead(pin_d0);
  // Le os dados do pino analogico A0 do sensor
  int valor_analogico = analogRead(pin_a0);
  // Mostra os dados no serial monitor
  Serial.print("Pino D0 : ");
  Serial.print(valor_digital);
  Serial.print(" Pino A0 : ");
  Serial.println(valor_analogico);
  // Verifica o nivel de butano detectado
  if (valor_analogico > nivel_sensor)
  {
    // Liga o buzzer e o led vermelho, e
    // desliga o led verde
    digitalWrite(pin_led_verm, HIGH);
    digitalWrite(pin_led_verde, LOW);
    digitalWrite(pin_buzzer, HIGH);

  }
  else
  {
    // Desliga o buzzer e o led vermelho, e
    // liga o led verde
    digitalWrite(pin_led_verm, LOW);
    digitalWrite(pin_led_verde, HIGH);
    digitalWrite(pin_buzzer, LOW);
  }
  delay(100);
}
