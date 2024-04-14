#include <SPI.h>
#include <Ethernet.h>

#define sinal A0
#define trig 3
#define eco 2

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 175 };

EthernetServer server(80);

int duracao = 0;
int distancia = 0;
boolean opcao = true;
char caracter;
char opca;
boolean opc = true;
boolean op = true;
boolean opp = true;
char o;
const int ledPin = 7;

String readString = String(30);
int status = 0;

void setup() {
  Ethernet.begin(mac, ip);
  server.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(eco, INPUT);
  pinMode(sinal, INPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
}

void loop() {
  duracao = pulseIn(eco, HIGH);
  distancia = duracao * 0.017175;

  Serial.print("\n== Valor da Distancia: \n ");
  Serial.print(distancia);
  Serial.println("cm ");
  Serial.print("\n ====== \n ");
  delay(500);

  int valorSensorChuva = analogRead(sinal);
  boolean estaMolhado = (valorSensorChuva < 650 && valorSensorChuva > 0);
  boolean estaProximo = (distancia < 20);  // Defina o limite de proximidade desejado

  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (readString.length() < 100) {
          readString += c;
        }
        if (c == '\n') {
          if (readString.indexOf("?") < 0) {
          } else {
            if (readString.indexOf("ledParam=1") > 0) {
              digitalWrite(ledPin, HIGH);
              status = 1;
            } else if (readString.indexOf("ledParam=0") > 0) {
              digitalWrite(ledPin, LOW);
              status = 0;
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head>");
            client.println("<link rel='icon' type='image/png' href='https://blogmasterwalkershop.com.br/arquivos/artigos/sub_wifi/logo_mws.png'/>");
            client.println("<title>Monitoramento apiario</title>");
            client.println("</head>");
            client.println("<body style=background-color:#ADD8E6>");
            client.println("<center><font color='blue'><h1>Monitoramento apiario</font></center></h1>");
            client.println("<h1><center>CONTROLE DE LED</center></h1>");

            // Botão para ligar o LED
            client.println("<center><form method='get' name='LED'><input type='hidden' name='ledParam' value='1'/><input type='submit' value='Ligar LED'/></form></center>");

            // Botão para desligar o LED
            client.println("<center><form method='get' name='LED'><input type='hidden' name='ledParam' value='0'/><input type='submit' value='Desligar LED'/></form></center>");

            // Botão para exibir o valor do sensor de proximidade
            client.println("<center><form method='get' name='SensorProximidade'><input type='hidden' name='sensorParam' value='1'/><input type='submit' value='Sensor de Proximidade'/></form></center>");

            // Botão para exibir o valor do sensor de chuva
            client.println("<center><form method='get' name='SensorChuva'><input type='hidden' name='sensorParam' value='2'/><input type='submit' value='Sensor de Chuva'/></form></center>");

            // Status do sensor de proximidade
            client.print("<center><h2>Status do Sensor de Proximidade: ");
            client.print(estaProximo ? "Próximo" : "Não Próximo");
            client.println("</h2></center>");

            // Status do sensor de chuva
            client.print("<center><h2>Status do Sensor de Chuva: ");
            client.print(estaMolhado ? "Molhado" : "Não Molhado");
            client.println("</h2></center>");

            client.println("<hr/>");
            client.println("<hr/>");
            client.println("</body>");
            client.println("</html>");
            readString = "";
            client.stop();
          }
        }
      }
    }
  }
} 
