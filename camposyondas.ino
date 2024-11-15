#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>

const char* ssid = "cubeta de ondas";
const char* password = "3,141592";

WebServer server(80);               // Servidor HTTP en el puerto 80
WebSocketsServer webSocket = WebSocketsServer(81); // Servidor WebSocket en el puerto 81

// Pines de los potenciómetros
const int pot1Pin = 34;
const int pot2Pin = 35;

const int pinLed = 5;

const int pin2=18; // entrada 2 del l293D
const int pin7=19; // Entrada 7 del l293D
const int pin1=21;

int pwm1; // Variable del PWM 1
int pwm2; // Variable del PWM 2

int pot1Value = 0;
int pot2Value = 0;

const int freq = 5000;
const int pwmChannel = 0;
const int resolution = 8; 

// Declaración de la función webSocketEvent
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
// Función para enviar datos al PCF8574

void setup() {
  Serial.begin(115200);

  // Configuración de los pines de los potenciómetros
  pinMode(pinLed, OUTPUT);

  pinMode(pin2,OUTPUT);
  pinMode(pin7,OUTPUT);
  pinMode(pin1,OUTPUT);

  // Configuración del punto de acceso
  WiFi.softAP(ssid, password);
  Serial.println("Punto de acceso iniciado");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Contraseña: ");
  Serial.println(password);
  Serial.print("IP del punto de acceso: ");
  Serial.println(WiFi.softAPIP());

  // Inicialización de I2C
  Wire.begin(); // usa los pines por defecto de la ESP32 (SDA=21, SCL=22)

  // Inicialización del servidor HTTP
  server.on("/", handleRoot);
  server.begin();

  // Inicialización del servidor WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  digitalWrite(pin1,HIGH);
}
unsigned long t1 = 0;
bool estadoDelLed = false;
void loop() {
  // Leer y mapear valores de los potenciómetros
  int newPot1Value = map(analogRead(pot1Pin), 0, 4095, 1, 255);
  int newPot2Value = analogRead(pot2Pin);

  // Actualizar si los valores han cambiado
  if (newPot1Value != pot1Value || newPot2Value != pot2Value) {
    pot1Value = newPot1Value;
    pot2Value = newPot2Value;

    // Enviar los valores actualizados al cliente WebSocket
    String json = "{\"pot1\": " + String(pot1Value) + ", \"pot2\": " + String(pot2Value) + "}";
    webSocket.broadcastTXT(json);

  }
  /*if(millis() < t1 + pot1Value){
    estadoDelLed = !estadoDelLed;
    t1 = millis();
  }*/
  
  digitalWrite(pinLed, LOW);
  delay(newPot1Value);
  digitalWrite(pinLed, HIGH);
  delay(newPot1Value);


  
  // Almacenando valor del potenciometro
  pwm1 = map(newPot2Value,0,4095,10,155);

  analogWrite(pin2,pwm1);
  digitalWrite(pin7, LOW);

  server.handleClient();
  webSocket.loop();
}

// Función para servir la página HTML
void handleRoot() {
  String html = R"rawliteral(
   <!DOCTYPE html>
    <html lang="es">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width,initial-scale=1"/>
      <title>Cubeta de ondas</title>
      <style>
        *{
          margin: 0; padding: 0;
          font-family: monospace;
        }
        .padre{
          margin: auto;
          width: 70%;
        }
      
        @media screen and (max-width:700px) {
            .padre {
                width: 100%;
            }
        }
        h1{
          padding: 10px; font-size: 34px; color: #fff;
          background: #007fff;
          text-align: center;
          margin-bottom: 20px;
        }
        .label{
          display: flex;
          flex-direction: column;
          align-items: center;
          margin-bottom: 20px;
        }
        h2{
          margin: 10px;
        }
        a{
          text-decoration: none;
          color: #000;
          font-size: 16px;
          font-weight: 800;
        }
        li{
          margin-bottom: 10px;
        }
        .label h2{
          color: #007fff;
          font-weight: 700;
        }
        span{
          border: 3px solid #007fff;
          border-radius: 6px;
          padding: 0px 15px;
          font-size: 30px;
          margin-bottom: 50px;
        }
        footer{
          width: 100%;
          background: #003264;
          padding: 10px;
          color: #fff;
        }
      </style>
    </head>
    <body>
      <div class="padre">

        <h1>Cubeta de ondas</h1>
        <div class="label">
          <h2>LUZ</h2>
          <span id="pot1Value">0 Hz</span>
        </div>
        <div class="label">
          <h2>ONDAS</h2>
          <span id="pot2Value">0 Hz</span>
        </div>
        
        <footer>
          <h2>Integrantes</h2>
          <ul>
            <li>
              <a href="mailto:juliethy.fallam@uqvirtual.edu.co?Subject=Cubeta%20de%20agua">Julieth Falla Murillo</a>
            </li>
            <li>
              <a href="mailto:bayrond.cortesf@uqvirtual.edu.co?Subject=Cubeta%20de%20agua">Bayron Daniel Cortes Falla</a>
            </li>
            <li>
              <a href="mailto:diegoa.camposl@uqvirtual.edu.co?Subject=Cubeta%20de%20agua">Diego Armando Campos</a>
            </li>
            <li>
              <a href="mailto:david.francoo@uqvirtual.edu.co?Subject=Cubeta%20de%20agua">David Francisco Ospina</a>
            </li>
            <li>
              <a href="mailto:santiago.jaramillov@uqvirtual.edu.co?Subject=Cubeta%20de%20agua">Santiago Jaramillo Valencia</a>
            </li>
            <li>
              <a href="mailto:ejlozanom@uqvirtual.edu.co?Subject=Cubeta%20de%20agua">Elu Jesus Lozano Mendoza</a>
            </li>
          </ul>
          
          <h2>link del repositorio</h2>
        </footer>
      </div>
      <script>
        const ws = new WebSocket('ws://' + window.location.hostname + ':81');
        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            let pot1Value = parseInt(data.pot1, 10); // Convertir a entero con base 10
            let hzluz = (1 / ((pot1Value/1000) * 2)).toFixed(6); // Limitar a 2 decimales
            document.getElementById("pot1Value").textContent = hzluz + ' Hz';
            document.getElementById("pot2Value").textContent = parseFloat(data.pot2).toFixed(2) + ' Hz'; // Limitar a 2 decimales
        };
        ws.onopen = function() {
          console.log("Conectado al servidor WebSocket");
        };
        ws.onclose = function() {
          console.log("Desconectado del servidor WebSocket");
        };
      </script>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Definición de la función webSocketEvent
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_DISCONNECTED) {
    Serial.printf("[%u] Desconectado!\n", num);
  } else if (type == WStype_CONNECTED) {
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Conectado desde %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
  }
}