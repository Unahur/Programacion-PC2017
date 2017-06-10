/*
Released under Creative Commons Attribution 4.0
by Gabriel Molina 2017
https://creativecommons.org/licenses/by/4.0/
Attribution means you can use it however you like as long you
mention that it's base on my stuff.
I'll be pleased if you'd do it by sharing https://github.com/Unahur/Programacion-PC2017
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);

/*
 * URL principal
 */
void handleRoot() {
  String message = "<html><head></head><body style='font-family: sans-serif; font-size: 12px'>";
  message += "Selecciona la salida y presiona on/off para generar la correspondiente url:<br><br>";
  message += "<form id='data' action='/rf' target='response'>";
  message += "Pin de salida D<input name='D' type='number' value='6' style='width:40px' min='0' max='8'> ";
  message += "<input name='on' type='hidden' value='1'>";
  message += "<script>function send(on){ document.getElementsByName('on')[0].value = on; document.getElementById('data').submit();}; function loaded(){ var a = document.getElementById('target'); a.innerHTML = a.href = document.getElementById('response').contentWindow.location; };</script>";
  message += "<input type='button' value='on' onclick='send(1)'><input type='button' value='off' onclick='send(0)'>";
  message += "</form><br>";
  message += "<iframe id='response' name='response' style='display:none' onload='loaded()'></iframe>URL construida y llamada: <a href='#' id='target'>...</a><br><br>";
  message += "Que te diviertas! -<a href='https://github.com/Unahur/Programacion-PC2017'>Programacion PC 2017</a></body></html>";
  server.send(200, "text/html", message);
}

/* 
 * Método para manejar las URL malformadas
 * Devuelve el método solicitado y los parámetros enviados 
 */
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/* 
 * Método para obtener los argumentos enviados por parámetro
 * Recibe un string
 * Devuelve un entero
 */ 
int getArgValue(String name)
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

/* 
 * Método para manejar las salidas del dispositivo
 * No recibe parámetros
 * Devuelve un OK y la salida accionada
 */
void handleRf()
{
  const int pinNumbers[] = {D0, D1, D2, D3, D4, D5, D6, D7, D8};
  int pin = getArgValue("D");
  int on = getArgValue("on");
  String out = "rf D";
  out += pin;
  out += " ";
  out += on;
  pinMode(pinNumbers[pin], OUTPUT);
  if (on == 1)
    digitalWrite(pinNumbers[pin], 1);
   else
    digitalWrite(pinNumbers[pin], 0);
  server.send(200, "text/plain", out);  
}

/* 
 * Método principal donde inicializamos el dispositivo y realizamos la conexión a la red 
 */
void setup(void){
  // Inicializamos el puerto serie para monitorear la conexión del dispositivo
  // a la red WIFI mediante las credenciales proporcionadas.
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Esperando la conexión
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Imprimimos la conexión establecida
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Creamos las rutas de acesso a nuestros métodos
  server.on("/", handleRoot);
  server.on("/rf", handleRf);
  server.onNotFound(handleNotFound);

  // Iniciamos el servidor web
  server.begin();
  Serial.println("HTTP server started");
}

/* 
 * Método para recibir las peticiones a través del servidor web en forma cíclica
 */
void loop(void)
{
  server.handleClient();
}
