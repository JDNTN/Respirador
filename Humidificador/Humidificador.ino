//Librerias
#include <OneWire.h>                
#include <DallasTemperature.h>
#include "DHT.h"

//Constantes
#define DHTPIN 2
#define DHTTYPE DHT11

//Declaracion de Variables
OneWire ourWire(3);
DHT humedadTemp(DHTPIN, DHTTYPE);
DallasTemperature tempAgua(&ourWire);

void setup() {
  Serial.begin(9600); 
  humedadTemp.begin();
  tempAgua.begin();
}

void loop() {  
  tempAgua.requestTemperatures();
  double aguaTemp = tempAgua.getTempCByIndex(0);
  double humedad = humedadTemp.readHumidity();
  double tempAm = humedadTemp.readTemperature();
  String data = "Temperatura Aire ";
  data += tempAm;
  data += " Humedad Aire: ";
  data += humedad;
  data += "Temperatura Agua ";
  data += aguaTemp;
}