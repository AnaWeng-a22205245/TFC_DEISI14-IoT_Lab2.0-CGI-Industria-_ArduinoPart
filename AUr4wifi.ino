#include <WiFiS3.h>
#include <PubSubClient.h>
#include <Stepper.h>  // Inclui a biblioteca para o motor de passo

// Configurações de WiFi
const char* ssid = "ASUS——W 3236"; // Substitua pelo SSID da sua rede WiFi
const char* password = "4680(N5ii"; // Substitua pela senha da sua rede WiFi

// Configurações do Broker MQTT
const char* mqtt_server = "10.10.0.57"; // Substitua pelo IP do seu broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

// Configurações do motor de passo
const int stepsPerRevolution = 2048; // Número de passos por revolução (para 28BYJ-48)
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;
Stepper myStepper(stepsPerRevolution, motorPin1, motorPin3, motorPin2, motorPin4); // Inicializa a biblioteca Stepper

// Pino do microswitch
const int switchPin = 2;  // pino do microswitch

// Variável para monitorar o estado anterior do microswitch
bool lastSwitchState = HIGH;

// Função para ligar ao WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("Endereço IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Falha na conexão WiFi");
  }
}

// Função callback para lidar com mensagens recebidas
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (messageTemp == "1") {
    modoOK();
  } else if (messageTemp == "2") {
    modoNOTOK();
  }

  // Publicar uma mensagem de resposta
  client.publish("22205245/anawen/device/recebeu", "Mensagem recebida com sucesso");
}

// Função para acender e apagar os LEDs (modoOK) e mover o motor de passo
void modoOK() {
  delay(2000);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  delay(2000);  // Mantém os LEDs acesos por 2 segundos
  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);

  // Movimentar o motor de passo para frente e para trás
  Serial.println("Movimentação do motor do modoOK");

  delay(3000); 
  // Define a velocidade do motor (15 RPM)
  myStepper.setSpeed(15);

  // Mover o motor -135 graus (768 passos)
  myStepper.step(768);  // Gira o motor -135 graus
  Serial.println("Motor move para frente");

  delay(500);

   // Mover o motor -135 graus (768 passos)
  myStepper.step(-768);  // Gira o motor -135 graus
  Serial.println("Motor move volta para trás");

  delay(500);
  // Verifica se o microswitch foi pressionado
  int currentSwitchState = digitalRead(switchPin);
  if (currentSwitchState == LOW) {
    Serial.println("Microswitch pressionado - Motor para");
  }

  digitalWrite(5, LOW);
  digitalWrite(7, LOW);
}

// Função para acender e apagar os LEDs (modoNOTOK) sem mover o motor de passo
void modoNOTOK() {
  delay(2000);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  delay(2000);  // Mantém os LEDs acesos por 2 segundos
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);

  // Simular a verificação do microswitch
  Serial.println("Simulação de verificação do microswitch para modoNOTOK");

 delay(1000);
  // Verifica se o microswitch foi pressionado
  int currentSwitchState = digitalRead(switchPin);
  if (currentSwitchState == LOW) {
    Serial.println("Microswitch pressionado - Ação parada");
  }


  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
}

void reconnect() {
  // Loop até que o cliente esteja ligado com sucesso
  while (!client.connected()) {
    Serial.print("Tentando a conexão do MQTT... ||");
    // Tentando se conectar
    if (client.connect("ArduinoClient")) {
      Serial.println("... conectado ao MQTT");
      // Assinando o tópico
      client.subscribe("22205245/anawen/device/test"); 
    } else {
      Serial.print(" falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      // Esperar 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Espera a conexão da porta serial
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield não encontrado");
    while (true); // Não continua se o shield não for encontrado
  }

  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); // Configura o pino do microswitch

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
