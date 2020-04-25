  //Importar Librerias
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_SSD1306.h> 
#include <AccelStepper.h>

//Declaracion volatiles
volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;

//Declaracion variables
bool menu = true;
int respiracionesMinuto = 0;
int presion = 0;
int volumen = 0;
bool mostrarTemperatura = false;
bool pulsacion = false;
int anterior = 0;

//Declaracion Constantes
#define presionMinima 0
#define presionMaxima 12
#define respiracionesMaximas 30
#define respiracionesMinimas 10
//#define volumenMaximo 100
//#define volumenMinimo 50
#define voltajeMinimo 0.5
#define voltajeMaximo 4.5
#define an 1023

#define motorInterfaceType 1

//declaracion de pines
#define pulsePin A0
#define presionPin A1
#define potenciometroRespiraciones A3
#define potenciometroPresion A4
#define potenciometroVolumne A5
#define bufferPinPulso 6
#define dirPin 8
#define stepPin 9
#define pinRojo 49
#define pinAzul 53
#define pinVerde 51

//Rango
  
  int rangoRespiraciones = an / (respiracionesMaximas - respiracionesMinimas);
//  int rangoVolumen = an / (volumenMaximo - volumenMinimo);
  int rangoPresion = an / (presionMaxima - presionMinima);


//Inicio de Elementos
LiquidCrystal_I2C lcd(0x27, 16, 2); //
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  pinMode(bufferPinPulso, OUTPUT);
  pinMode(pinRojo, OUTPUT);
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAzul, OUTPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  attachInterrupt(digitalPinToInterrupt(2), IntTemperatura, RISING);
  stepper.setMaxSpeed(1200);
  mlx.begin();
  interruptSetup();
  menu = true;
}

void loop() {  
    sensorPresion(); 
    //delay(3000);
}

void menuu(){
  presion = analogRead(potenciometroPresion);
  presion = presionMinima + calculoPotenciometro(presion, rangoPresion);
  respiracionesMinuto = analogRead(potenciometroRespiraciones);
  respiracionesMinuto = respiracionesMinimas + calculoPotenciometro(respiracionesMinuto,rangoRespiraciones);
  //volumen = analogRead(potenciometroVolumen);
  //volumen = volumenMinimo + calculoPotenciometro(volumen, rangoVolumen);  
}

int calculoPotenciometro(int valor, int rango){
  valor = 1023 - valor;
  int val = -1;
  int pos = 0;
 while (val < 0){
      if (((pos * rango) >= valor) && ( valor < ((pos+1) * rango)))
        val = pos;
      pos++;
  }  
  return val;   
}

void pulso(){
    int pulso = analogRead(pulsePin);
    pulsos(pulso);
    Serial.println(pulso);
}

void pulsos(int pulso) {
  if (pulso >= 515) {
    if ((anterior > pulso) && pulsacion) {
      pulsacion = false;
      digitalWrite(bufferPinPulso, HIGH);
    }
    anterior = pulso;
  } else {
    digitalWrite(bufferPinPulso, LOW);
    pulsacion = true;
  }
  lcd.setCursor(1, 0);
  lcd.print("BPM= ");
  lcd.print(BPM);
  lcd.print("    ");
  if (QS == true) {
    QS = false;
  }
}

void IntTemperatura() {
  if(menu){
    menu = false;
  }else{
   mostrarTemperatura = true;   
  }  
}

void temperatura() {
  double temp = mlx.readObjectTempC();
  lcd.setCursor(1, 1);
  lcd.print("Temp = ");
  lcd.print(temp);
  if (temp > 30) {
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinRojo, HIGH);
  } else {
    digitalWrite(pinRojo, LOW);
    digitalWrite(pinVerde, HIGH);
  }

  mostrarTemperatura = false;
}

void sensorPresion() {
  float lecturaSensor = analogRead(presionPin); //Declaramos la entrada de Voltaje
  float voltaje = lecturaSensor * (5.0 / 1023.0);

  if (voltaje < voltajeMinimo) {
    Serial.println("Presion bajo de cero");
    Serial.println(voltaje);
  } else if (voltaje > voltajeMaximo) {
    Serial.println("presion muy alta");
  } else {
    float presionIngresadaBares = presionMinima + ((voltaje - voltajeMinimo) / (voltajeMaximo - voltajeMinimo)) * (presionMaxima - presionMinima);
    float presionIngresadaPascales = presionIngresadaBares*1019.716 ;
    Serial.println("Bares:");
    Serial.println(presionIngresadaBares);
    Serial.println("KiloPascales:");
    Serial.println(presionIngresadaPascales);
  }
  
  delay(2000);
}
void adelante(int pasos, int velocidad, int tiempoSiguienteCiclo) {
  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != pasos)
  {
    stepper.setSpeed(velocidad);
    stepper.runSpeed();
  }
}
void atras (int pasos, int velocidad, int tiempoSiguienteCiclo) {
  stepper.setCurrentPosition(0);
  while (stepper.currentPosition() != -pasos)
  {
    stepper.setSpeed(-velocidad);
    stepper.runSpeed();
  }
}
