/*Librerias; Servo aún no la implementé*/
#include<Servo.h>
#include <ESP8266WiFi.h>

/*Variables de velocidad, tolerancia y declaración de prototipos de funciones*/
#define velocidad 255
double tolerancia = 5.00;
double medir_distancia();
void adelante();
void detener();
void derecha();
void atras();
void izquierda();
void servo_mov_der();
void servo_mov_izq();
void respuesta_web();
Servo mi_servo;

//Configuración de red
char ssid[] = "Wifi-Martinez";
const char *password = "1wds416s";

WiFiServer server(80); //Instanciamos el servidor en la variable server en el puerto especificado como argumento
WiFiClient client; //Instanciamos un cliente

//Función que recibe del cliente WiFi una cadena que contiene la solicitud como parte de la misma al tocar un botón
//al hacer esto la solicitud devuelve adelante, atras, etc.

void recepcion_serial(String recepcion) {

  //Buscamos si en la cadena recibida se encuentra, en alguna posición, el indice de la palabra buscada, de ser así accionamos la función de mover
  if (recepcion.indexOf("ADELANTE") > 0) {
    adelante();
  } else if (recepcion.indexOf("ATRAS") > 0) {

    atras();
  } else if (recepcion.indexOf("IZQUIERDA") > 0) {
    izquierda();
  } else if (recepcion.indexOf("DERECHA") > 0) {
    derecha();
  } else if (recepcion.indexOf("PARAR") > 0) {
    detener();
  }
}


void setup() {
  //inicio de puertos y pines
  Serial.begin(115200);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D1, OUTPUT); //TRIGGER
  pinMode(D2, INPUT); //ECHO
  mi_servo.attach(D3);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);

  Serial.printf("Conectando a: %s\n", ssid);

  //inicio de la conexión wifi con las credenciales anteriores
  WiFi.begin(ssid, password);

  // Intentamos que se conecte a la red wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando...");
    delay(2000);
  }

  //al conectarse devuelve la IP que se le asignó
  Serial.print("Conectado.  ");
  Serial.print(" Dirección IP del módulo: ");
  Serial.println(WiFi.localIP());
  //iniciamos el servidor
  server.begin();


}

void loop() {
//se comprueban las conexiones entrantes y se le asignan la variable cliente que es de tipo WiFiClient
  WiFiClient client = server.available();
  if (client) {
    //En el caso de que haya cliente
    Serial.println("Nuevo cliente conectado");
    //Obtenemos los datos de la solicitud y los guardamos en el string. readStringUntil('r') especifíca que la lectura se detendrá al recibir una 'r' sino quedaría en bucle
    String linea_cliente = client.readStringUntil('r');
    //enviamos la solicitud a la función 
    recepcion_serial(linea_cliente);
    Serial.println(linea_cliente);
    // Enviar la respuesta HTML al cliente
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head><meta charset='UTF-8'>");
    client.println("<meta http-equiv='X-UA-Compatible' content='IE=edge'>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    client.println("<style>");
    client.println("html {box-sizing: border-box;}*, *:before, *:after {box-sizing: inherit;}body {margin: 0;font-family:monospace;}");
    client.println(".botones{box-sizing: border-box;justify-items: center;align-items: center;}");
    client.println("*{border: none;}button{width: 100px;height: 100%;}");
    client.println(".contenedor {display: flex;justify-content: center;align-items: center;width: 50vw;height: 50vw;margin: 0 auto;}");
    client.println(".botones {text-align: center;display: grid;grid-template-columns: repeat(3, 1fr);grid-template-rows: repeat(3, 1fr);grid-column-gap: 5px;grid-row-gap: 5px;height: 37vh;justify-self: center;align-self: center;}");
    client.println("</style>");
    client.println("<title>ESP8266</title></head>");
    client.println("<body style='background-color: #050A30;'>");
    client.println("<h1 style='color:#D8A7B1; text-align: center;'>ESP8266 WiFi controller</h1>");
    client.println("<div class='contenedor'>");
    client.println("<div class= ' botones'>");
    client.println("<button onclick=location.href='./?IZQUIERDA' style='background-color: #B6E2D3; border-radius: 25px; color: white;grid-area: 2 / 1 / 3 / 2; '>IZQUIERDA</button>");
    client.println("<button onclick=location.href='./?ATRAS' style='background-color: #FAE8E0; border-radius: 25px; color: white;grid-area: 3 / 2 / 4 / 3;'>ATRAS</button>");
    client.println("<button onclick=location.href='./?DERECHA' style='background-color: #EF7C8E; border-radius: 25px; color: white;grid-area: 2 / 3 / 3 / 4; '>DERECHA</button>");
    client.println("<button onclick=location.href='./?PARAR' style='background-color: #673ab7; border-radius: 25px; color: white; grid-area: 2 / 2 / 3 / 3; '>PARAR</button>");
    client.println("<button onclick=location.href='./?ADELANTE' style='background-color: #D8A7B1; border-radius: 25px; color: white; grid-area: 1 / 2 / 2 / 3; '>ADELANTE</button>");
    client.println("</div>");
    client.println("</div>");
    client.println("</body>");
    client.println("</html>");

    // Finalizar la conexión con el cliente
    client.stop();
    Serial.println("Cliente desconectado");
  }
}

double medir_distancia() {
  double distancia = 0;
  double duracion = 0;
  digitalWrite(D1, HIGH);
  delayMicroseconds(10);
  digitalWrite(D1, LOW);
  duracion = pulseIn(D2, HIGH);
  distancia = duracion / 59;
  return distancia;
}
//funciones de movimiento
void adelante() {
  Serial.println("hacia adelante");
  analogWrite(D5, 0);
  analogWrite(D4, velocidad);
  analogWrite(D7, 0);
  analogWrite(D6, velocidad);
}

void atras() {
  Serial.println("retrocediendo");
  analogWrite(D5, velocidad);
  analogWrite(D4, 0);
  analogWrite(D7, velocidad);
  analogWrite(D6, 0);
}
void detener() {
  Serial.println("detenido");
  analogWrite(D5, 0);
  analogWrite(D4, 0);
  analogWrite(D7, 0);
  analogWrite(D6, 0);
}
void izquierda() {
  Serial.println("izquierda");
  analogWrite(D7, 0);
  analogWrite(D6, velocidad);
}
void derecha() {
  Serial.println("derecha");
  analogWrite(D5, 0);
  analogWrite(D4, velocidad);
}

void servo_mov_der() {
  double distancia = medir_distancia();
  for (int i = 0; i < 80; i++) {
    mi_servo.write(i);
    if (distancia >= tolerancia) {
      break;
    }
    Serial.println("moviendo +");
    delay(10);
  }
}

void servo_mov_izq() {
  //Medimos la distancia a cada ángulo del servo. Si la distancia medida es mayor a la tolerancia salimos del bucle
  double distancia = medir_distancia();
  for (int i = 80; i > 0; i--) {
    mi_servo.write(i);
    if (distancia >= tolerancia) {
      break;
    }
    Serial.println("moviendo -");
    delay(10);
  }

}
