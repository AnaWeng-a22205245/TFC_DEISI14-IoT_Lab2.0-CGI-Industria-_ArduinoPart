#include <WiFiS3.h>
#include <PubSubClient.h>

// Configurações de WiFi
const char* ssid = ""; // Substituir pelo SSID da sua rede WiFi
const char* password = ""; // Substituir pela senha da sua rede WiFi

// Configurações do Broker MQTT
const char* mqtt_server = ""; // Substituir pelo IP do seu broker MQTT

WiFiClient espClient;
PubSubClient client(espClient);

// Função para conectar ao WiFi
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

// Função para acender e apagar os LEDs (modoOK)
void modoOK() {
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  delay(2000);  // Mantém os LEDs acesos por 1 segundo
  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);

  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(7, LOW);
}

// Função para acender e apagar os LEDs (modoNOTOK)
void modoNOTOK() {
   digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  delay(2000);  // Mantém os LEDs acesos por 1 segundo
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);

  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(6, LOW);
}

void reconnect() {
  // Loop até que o cliente esteja conectado
  while (!client.connected()) {
    Serial.print("Tentar a conexão do MQTT... ||");
    // Tentando se conectar
    if (client.connect("ArduinoClient")) {
      Serial.println("... conectado ao MQTT");
      // Assinando o tópico
      client.subscribe("22205245/anawen/device/test"); // Substitua pelo seu tópico
      // Publicando uma mensagem no tópico "test/topic"
      //client.publish("22205245/anawen/device/test", "Olá do Arduino!");
    } else {
      Serial.print(" falhou , rc=");
      Serial.print(client.state());
      Serial.println(" a tentar novamente em 5 segundos");
      // Esperar 5 segundos antes de tentar novamente
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // Espera a conexão da porta serial (necessário para placas como o Leonardo)
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield não encontrado");
    while (true); // Não continua se o shield não for encontrado
  }

  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

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
