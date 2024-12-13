/*
 * IoT ESP8266 Based Mood Lamp (RGB LED) Controller Program
 * https://descubrearduino.com
 */
extern "C" {
#include "user_interface.h"
}
ADC_MODE(ADC_TOUT);
//ADC_MODE(ADC_VCC);
 
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include "index.h"
#include "wificonfig.h"



//SSID and Password of your WiFi router
//const char* ssid = "";
//const char* password = "";
String ssid;
String password;

const IPAddress direccionAP = IPAddress(192, 168, 66, 1);
const IPAddress SUBNET = IPAddress(255, 255, 255, 0);
String NOMBRERED = "RGBControl";
String PASWRED = "laluzpapa";
  
ESP8266WebServer server(80);
//LED Connections
//const int BlueLED = 16; // D0
//const int RedLED = 5;    // D1 
//const int GreenLED = 4; // D2
const int BlueLED = 5; // D1
const int RedLED = 4;    // D2
const int GreenLED = 0; // D3

int vectr2[] = {255,255,255,255, 255,0  ,0  ,0  ,0  ,0  ,0  ,75 ,125,180,255};
int vectg2[] = {0  ,75 ,125,180, 255,255,255,255,255,255,0  ,0  ,0  ,0  ,0  };
int vectb2[] = {0  ,0  ,0  ,0  ,0   ,0  ,75 ,125,180,255,255,255,255,255,255};
int DIMENSIONVECT2=15;


#define T1V  ESP8266_REG(0x604) //(RO) Current Value
//=======================================================================
//                    Parametros globales
//=======================================================================
// Intensidad
float brillo=1;
int bandera=0;
int recuperarband;
int r, g, b;
//bool derivr, derivg, derivb=true;
int banderaslash=0;
bool estadoflash;
bool auxflash=false;
int contflash;

void variacionlenta(void);
void variacionfuerte(void);
void flashaso(void);

bool banderaip = false;
bool banderaserver = false;
bool reconfwifi = false;
//=======================================================================
//                    handles main page
//=======================================================================
void handleRoot() {
  Serial.println("Root Page Requested");
  server.send(200, "text/html", MAIN_page);
}

/*
void handleRoot() {
 Serial.println("You called root page");
 String s = MAIN_page; //Read HTML contents    // what a wasted RAM
 //Serial.println(s);    // you better commented this
 server.send(200, "text/html", s); //Send web page <----------------------Line 46
}
 */


//=======================================================================
//                    Handle Set Color
//=======================================================================
void handleForm() {
  //Saperate Colors are sent through javascript
  bandera=0;
  String red = server.arg("r");
  String green = server.arg("g");
  String blue = server.arg("b");
  //int r = red.toInt() * brillo;
  //int g = green.toInt() * brillo;
  //int b = blue.toInt() * brillo;
  r = red.toInt() * brillo;
  g = green.toInt() * brillo;
  b = blue.toInt() * brillo;
  Serial.print("Red:");Serial.println(r);
  Serial.print("Green:");Serial.println(g);
  Serial.print("Blue:");Serial.println(b);
  
  //PWM Correction 8-bit to 10-bit
  r = r * 4; 
  g = g * 4;
  b = b * 4;
  //for ULN2003 or Common Cathode RGB Led not needed
  /*
  r = 1024 - r;
  g = 1024 - g;
  b = 1024 - b;
  */
  //ESP supports analogWrite All IOs are PWM
  analogWrite(RedLED,r);
  analogWrite(GreenLED,g);
  analogWrite(BlueLED,b);
  server.sendHeader("Location", "/");
  server.send(302, "text/html", "Updated-- Press Back Button");
 
  delay(500);  
}


void handleinten(){
   
  String inten = server.arg("I");
  float aux = inten.toInt(); 
  aux = aux/10;
  if(aux >= 0 && aux <= 1)
    brillo = aux;
    
    /*
    brillo=brillo-0.1;
    if(brillo<=0)
      brillo=1;
    String inten = (String) brillo;
    */
  server.send(302, "text/html", "<p>Configurado en I= "+ inten +"</p>");
 }

void arcoiris(){
    bandera=2;
    r=0;
    g=0;
    b=240;
    banderaslash=0;
    server.sendHeader("Location", "/");
    server.send(302, "text/html", "<p>Modific en SLASH</p>");
  }

void mortalflash(){
    bandera=3;
    server.sendHeader("Location", "/");
    server.send(302, "text/html", "<p>Modific en FLASH</p>");
  }

  
void handleflash(){
  recuperarband=bandera; 
  bandera=4;
  estadoflash=false;
  contflash=0;
  server.sendHeader("Location", "/");
  server.send(302, "text/html", "<p>FLAAAAAAAAASH</p>");

 }

void handlevariado(){
  bandera=6;
  server.sendHeader("Location", "/");
  server.send(302, "text/html", "<p>Variando</p>");
  }

  void handledesvan(){
    bandera=7;
    server.sendHeader("Location", "/");
    server.send(302, "text/html",  "<p>ZZZz</p>");
  }

  void handleVECfijo(){
    bandera=0;
    Serial.print("Indice: "); 
    
    int index = server.arg("ind").toInt();
    Serial.println(index); 
    if(index==15){
      analogWrite(RedLED,  1024);
      analogWrite(GreenLED, 1024);
      analogWrite(BlueLED, 1024);
        Serial.println("BLANCO");
    }else if(index==16){
      analogWrite(RedLED,  0);
      analogWrite(GreenLED, 0);
      analogWrite(BlueLED, 0);
      Serial.println("NEGRO");
    }
    else{
    analogWrite(RedLED,  4* vectr2[index]);
    analogWrite(GreenLED,4* vectg2[index]);
    analogWrite(BlueLED, 4* vectb2[index]);
    Serial.println(vectr2[index]);
    Serial.println(vectg2[index]);
    Serial.println(vectb2[index]);
    }
  server.sendHeader("Location", "/");
  server.send(302, "text/html", "<p> Actualizado </p>");
}

//=======================================================================
//                    WIFI
//=======================================================================

void conectarstation(){
  //WiFi.mode(WIFI_STA);
  //WiFi.setAutoConnect(true)
   if(wifi_station_set_auto_connect(1)==false){
      Serial.println("FALLLLOO"); 
   }
   int n = WiFi.scanNetworks();
   if(n>0){
      for(int i=0;i<n;i++){
        if(WiFi.SSID(i)==ssid){
          i=n;
          WiFi.begin(ssid,password,true);
          WiFi.persistent(false);

          if(reconfwifi){
            writeString(0,ssid);
            writeString(128,password);
            
            }
         //if(WiFi.status()!=WL_CONNECTED)
         // WL_CONNECTED after successful connection is established
         //WL_NO_SSID_AVAIL in case configured SSID cannot be reached
         //WL_CONNECT_FAILED if connection failed
         //WL_CONNECT_WRONG_PASSWORD if password is incorrect
         //WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
         //WL_DISCONNECTED if module is not configured in station mode
         //-1 on timeout
        }
      }
    
   }
  //WiFi.begin(ssid, password);     //Connect to your WiFi router
}

void handlewificonf(){
    ssid = server.arg("ssid");
    password = server.arg("pass");
    String texto = "\n<!DOCTYPE html>\n<html>\n <p> Configurando ssid: "+ ssid +" pass: "+ password +"</p>\n</html>\n\n\n";
    server.send(200, "text/html",texto);
    reconfwifi = true;
    conectarstation();
}

void handlewifiweb(){ 
  server.send(200, "text/html", wificonfig);
  }

void handlesendwconf(){
  String text0 = "<!DOCTYPE html> <html><p> Estado actual: </p>";
  String text1 = "SSID:  " + WiFi.SSID() + "<br>";
  String text2 = " IP de la STATION:  " + WiFi.localIP().toString() + "</html>";
  server.send(200, "text/html",text0 + text1 + text2);
}

void handlereq() {
    server.send(200, "text/html","");
  }
 
//=======================================================================
//                    SETUP
//=======================================================================
WiFiEventHandler gotIpEventHandler;
//MDNS dnslocal;
DNSServer dnsServer;

void setup(){
  Serial.begin(115200);   //Start serial connection  
  EEPROM.begin(512);
  pinMode(RedLED,OUTPUT);
  pinMode(GreenLED,OUTPUT);
  pinMode(BlueLED,OUTPUT);

  //-----------------------------------------------------------------------------
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(direccionAP,direccionAP,SUBNET);
  WiFi.softAP(NOMBRERED);

  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(53, "luces.local", direccionAP);
 
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event){      
    //WiFi.softAP(NOMBRERED, PASWRED);     
     //Serial.print("Nueva IP: ");
     //Serial.print(WiFi.localIP());
   banderaip=true;
   if(banderaserver){
     MDNS.begin("luces");
     MDNS.addService("http", "tcp", 80);
     Serial.println("MDNS server started");
  }
    });

  ssid = read_String(0);
  password = read_String(128);

  int n = WiFi.scanNetworks();
   if(n>0){
      bool flagexist=false;
      for(int i=0;i<n;i++){
        if(WiFi.SSID(i)==ssid){
          i=n;
          flagexist=true;
          WiFi.begin(ssid,password,true);
        }
      }
      if(flagexist==false)
        WiFi.disconnect();    
   }
    
  //conectarstation();
  /*
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");  
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
*/
  
//-----------------------------------------------------------------------------
 
  
  server.on("/", handleRoot);  //Associate handler function to path
  server.on("/setRGB",handleForm);
  server.on("/setFLASH",mortalflash);
  server.on("/setSLASH",arcoiris);
  server.on("/setLUZ",handleinten);
  server.on("/flasheada",handleflash);
  server.on("/variado",handlevariado);
  server.on("/desvanecimiento",handledesvan);
  server.on("/indx",handleVECfijo);
  server.on("/wificonfig", handlewifiweb);
  server.on("/WCONF", handlewificonf);
  server.on("/getWCONF", handlesendwconf);
  server.on("/seeconfig", handlesendwconf);
  server.on("/req", handlereq);
  server.onNotFound(handleRoot)
  server.begin();                           //Start server
  
  banderaserver=true;
  Serial.println("HTTP server started");
  analogWriteFreq(100);
  //analogWriteFreq(40000);

  
  analogWrite(RedLED,150);
  analogWrite(GreenLED,150);
  analogWrite(BlueLED,150);
  Serial.println(T2V);
  T1C=0b10000010;


  bandera=6;

  if(banderaip){
     //MDNS.begin("esp8266", WiFi.localIP());
     MDNS.begin("luces");
     MDNS.addService("http", "tcp", 80);
      Serial.println("MDNS server started");
  }
}

//=======================================================================
//                    MEMORIA EEPROM
//=======================================================================


void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}


String read_String(char add)
{
  int i;
  char data[128]; //Max 127 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<127)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}
//=======================================================================
//                    LOOP
//=======================================================================
unsigned int  tinicial=0;
unsigned int  tactual=0;
int contador=0;
int tiempo=0;
bool resetinicial=true;


void loop(){
  dnsServer.processNextRequest();
  MDNS.update();
  server.handleClient();
  //if(tiempo>8000){  //se traba en 8578, seguro por desbordamiento del timer (2 a la 27)
  //    ESP.restart();
  //  }
  // Carga de valores para medir tiempo
  if(resetinicial){
    tinicial = T2V;
    resetinicial=false;   
  }
  tactual = T2V;
  if(tactual<tinicial)
    tinicial=0;
 // Funciones de chequeo de tiempos
  if(T2V>20131653)
    T2V=0;
 
  if( (tactual-tinicial)>12500 ){
      resetinicial=true;
      contador++;
      if(bandera==4){//el flashaso
        flashaso();
      }
      if(bandera==7){
          desvanvariado();
      }
      if(contador>=20){ //5 y (tactual-tinicial)>50000 =>aprox 1 seg
        contador=0;
        tiempo++;
                        //Serial.print(tiempo);
                        //Serial.print(" - ");
                        //Serial.println(T2V);
        if(bandera==2){
          variacionlenta();
        }
        if(bandera==3){
          variacionfuerte();
        }
        if(bandera==6){
          variacionvariada();
        }   
              
           
     }
      
    }
    // Sensado de tension
    //float voltaje = analogRead(A0);
    //Serial.print(voltaje);
}




void variacionlenta(){
  
    if( banderaslash==0){
        r=r+10;
        if(r>=230){
           banderaslash++;
        }
      }
    else if(banderaslash==1){
      b=b-10;
      if(b<=10){
           banderaslash++;
      }
    }
    else if( banderaslash==2){
        g=g+10;
        if(g>=230){
           banderaslash++;
        }
      }
    else if(banderaslash==3){
      r=r-10;
      if(r<=10){
           banderaslash++;
      }
    }

    else if( banderaslash==4){
        b=b+10;
        if(b>=230){
           banderaslash++;
        }
      }
    else if(banderaslash==5){
      g=g-10;
      if(g<=10){
           banderaslash++;
           banderaslash=0;
      }
    }

   /*
  Serial.print("Red:");Serial.println(r);
  Serial.print("Green:");Serial.println(g);
  Serial.print("Blue:");Serial.println(b);
  analogWrite(RedLED,r * 4 * brillo);
  analogWrite(GreenLED,g * 4 * brillo);
  analogWrite(BlueLED,b  * 4 * brillo);*/
  }

int vectr[] = {255,255,0  ,255,0  ,0  };
int vectg[] = {0  ,255,0  ,0  ,255,255};
int vectb[] = {0  ,0  ,255,255,0  ,255};
int indexvect = 0;
int DIMENSIONVECT = 6;

void variacionfuerte(){
  indexvect++;
  if(indexvect>=DIMENSIONVECT)
     indexvect=0;
  r = vectr[indexvect];
  g = vectg[indexvect];
  b = vectb[indexvect];
  /*
  Serial.print("Red:");Serial.println(r);
  Serial.print("Green:");Serial.println(g);
  Serial.print("Blue:");Serial.println(b);
  */
  analogWrite(RedLED,r * 4 * brillo);
  analogWrite(GreenLED,g * 4 * brillo);
  analogWrite(BlueLED,b  * 4 * brillo);
}


void flashaso(){
  if(auxflash==true){//Duplicador de tiempo
    auxflash=true;
    return;
    }
  else
    auxflash=false;
    
  if(estadoflash==false){
        r=255;
        b=150;
        g=255;
        analogWrite(RedLED,r * 4 * brillo);
        analogWrite(GreenLED,g * 4 * brillo);
        analogWrite(BlueLED,b  * 4 * brillo);
        estadoflash=true;
    }
  else{
        r=0;
        b=0;
        g=0;
        analogWrite(RedLED,r * 4 * brillo);
        analogWrite(GreenLED,g * 4 * brillo);
        analogWrite(BlueLED,b  * 4 * brillo);
        estadoflash=false;
    contflash++;
    }
    if(contflash>=15)//Cantidad de flasheasos
      bandera=recuperarband;
}

void variacionvariada(){
  indexvect++;
  if(indexvect>=DIMENSIONVECT2)
     indexvect=0;
  r = vectr2[indexvect];
  g = vectg2[indexvect];
  b = vectb2[indexvect];
  /*
  Serial.print("Red:");Serial.println(r);
  Serial.print("Green:");Serial.println(g);
  Serial.print("Blue:");Serial.println(b);
  */
  analogWrite(RedLED,r * 4 * brillo);
  analogWrite(GreenLED,g * 4 * brillo);
  analogWrite(BlueLED,b  * 4 * brillo);
}

int contdesvan = 1;
int vectfact[] = {20, 20, 19, 19, 18, 18, 17, 17, 16, 16, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 4, 4, 3, 3, 2, 1, 0}; 
int VECTFLENG = 30;
 void desvanvariado(){
    contdesvan++;
    if(contdesvan>=VECTFLENG){  //Posiblemente haya que limitar a menos el 10
     contdesvan=0;
     indexvect++; 
    }
    if(indexvect>=DIMENSIONVECT2)
      indexvect=0;
    r = vectr2[indexvect]*(0.05*vectfact[contdesvan]);
    g = vectg2[indexvect]*(0.05*vectfact[contdesvan]);
    b = vectb2[indexvect]*(0.05*vectfact[contdesvan]);
  analogWrite(RedLED,  r * 4 );
  analogWrite(GreenLED,g * 4 );
  analogWrite(BlueLED, b * 4 );
} 
