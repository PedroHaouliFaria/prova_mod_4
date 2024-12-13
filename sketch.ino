#include <WiFi.h>
#include <HTTPClient.h>

#define led_verde 41 // Pino utilizado para controle do led verde
#define led_vermelho 40 // Pino utilizado para controle do led vermelho
#define led_amarelo 9 // Pino utilizado para controle do led amarelo

const int buttonPin = 18;  // Número do pino do botão
const int ldrPin = 4;  // Número do pino do sensor fotoresistor

int buttonState = 0;  // Variável para leitura do estado do botão
int lastButtonState = LOW; // Estado anterior do botão
unsigned long lastDebounceTime = 0; // Tempo da última mudança de estado do botão
unsigned long debounceDelay = 50; // Tempo de debounce em milissegundos

int threshold = 600;

bool isRed = false; // Variável para acompanhar se o LED vermelho está aceso

void setup() {
  // Configuração inicial dos pinos para controle dos LEDs como saídas
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(led_amarelo, OUTPUT);

  // Configuração dos pinos de entrada
  pinMode(buttonPin, INPUT); // Inicializa o pino do botão como entrada
  pinMode(ldrPin, INPUT); // Inicializa o pino do LDR como entrada

  // Inicializa todos os LEDs como desligados
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);
  digitalWrite(led_amarelo, LOW);

  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECTED) { // Verifica a conexão WiFi
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!");

  // Verifica o estado do botão
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

  if (WiFi.status() == WL_CONNECTED) { // Se o ESP32 estiver conectado à internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do resultado da requisição HTTP

    if (httpResponseCode > 0) {
      Serial.print("Código de resposta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Código de erro: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Desconectado");
  }
}

void loop() {
  int ldrStatus = analogRead(ldrPin);

  int reading = digitalRead(buttonPin); // Leitura atual do botão

  // Verifica se houve uma mudança no estado do botão
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Atualiza o tempo da última mudança de estado
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Se o tempo decorrido desde a última mudança de estado é maior que o tempo de debounce
    if (reading != buttonState) {
      buttonState = reading;

      // Se o botão estiver pressionado e o LED vermelho estiver aceso
      if (buttonState == HIGH && isRed) {
        delay(1000); // Aguardar 1 segundo após o pressionamento do botão
        digitalWrite(led_vermelho, LOW);
        isRed = false; // Define que o LED vermelho está apagado
        digitalWrite(led_verde, HIGH);
        delay(3000); // Verde por 3 segundos
        digitalWrite(led_verde, LOW);
      }
    }
  }

  // Atualiza o estado anterior do botão
  lastButtonState = reading;

  if (ldrStatus <= threshold) {
    Serial.print("Está escuro, modo noturno ativado, pisque o LED amarelo");
    Serial.println(ldrStatus);
    digitalWrite(led_amarelo, HIGH);
    delay(500);
    digitalWrite(led_amarelo, LOW);
    delay(500);
  } else {
    Serial.print("Está claro, modo convencional ativado");
    Serial.println(ldrStatus);

    // Alterna entre os LEDs verde, amarelo e vermelho
    digitalWrite(led_verde, HIGH);
    delay(3000); // Verde por 3 segundos
    digitalWrite(led_verde, LOW);

    digitalWrite(led_amarelo, HIGH);
    delay(2000); // Amarelo por 2 segundos
    digitalWrite(led_amarelo, LOW);

    digitalWrite(led_vermelho, HIGH);
    isRed = true; // Define que o LED vermelho está aceso
    delay(5000); // Vermelho por 5 segundos

    digitalWrite(led_vermelho, LOW);
    isRed = false; // Define que o LED vermelho está apagado
  }
}
