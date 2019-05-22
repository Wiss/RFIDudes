#include <LiquidCrystal_I2C.h>

// Programa para controlar temperatura en maquina vulcanizadora.
// Versión 0 : Se lee la temperatura y se envía al seríal para plotear y se imprime en la pantalla

//incluir librerias
#include <PID_v1.h>                     //se incluye libreria pid para controladores
#include "max6675.h"          //libreria de adc para sensor de temperatura
#include  <Wire.h>                      //libreria para protocolo I2C
#include  <LiquidCrystal_I2C.h>         //libreria para lcd por i2c
String s;

// inicializar sensores de temperatura
// CLK y DO para todos los spi

int thermoCLK = 13;     //SCK   tcCLK
int thermoDO = 12;     //MISO   const int tcSO = 50;

int tcCS[]  = {4, 5, 6, 7};
//int thermo1CS =4 ;
//int thermo2CS = 5;
//int thermo3CS = 6;
//int thermoUpCS = 7;

MAX6675 thermocouple[] = {
                          MAX6675(thermoCLK, tcCS[0], thermoDO),
                          MAX6675(thermoCLK, tcCS[1], thermoDO),
                          MAX6675(thermoCLK, tcCS[2], thermoDO),
                          MAX6675(thermoCLK, tcCS[3], thermoDO),
};

//MAX6675 tpar1(thermoCLK, thermo1CS, thermoDO);
//MAX6675 tpar2(thermoCLK, thermo2CS, thermoDO);
//MAX6675 tpar3(thermoCLK, thermo3CS, thermoDO);
//MAX6675 tparUp(thermoCLK, thermoUpCS, thermoDO);

//falta incluir sensor arriba
double temp[4];
double temp1, temp2, temp3, tempup;


// Variables PID
int pote;
double ventana = 5000;
unsigned long inicio;
double setpoint, out1, out2, out3, outup;
#define RELAY_PIN1 8
#define RELAY_PIN2 9
#define RELAY_PIN3 10
#define RELAY_PINup 11

double kp = 0.1;
double ki = 0.5;
double kd = 0;

PID pid1(&temp1, &out1, &setpoint, kp, ki, kd, DIRECT);
PID pid2(&temp2, &out2, &setpoint, kp, ki, kd, DIRECT);
PID pid3(&temp3, &out3, &setpoint, kp, ki, kd, DIRECT);
PID pidup(&tempup, &outup, &setpoint, kp, ki, kd, DIRECT);



//declarar pantalla LSD
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
String stringUp = "";
String stringDown = "";
//custom chars

byte arriba[8] = {
  0b00000100,
  0b00001110,
  0b00001110,
  0b00010101,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00000000,
};
byte abajo[8] = {
  0b00000000,
  0b00000100,
  0b00000100,
  0b00000100,
  0b00010101,
  0b00001110,
  0b00001110,
  0b00000100,
};



void setup() {
  // put your setup code here, to run once:
  //inicializar sensores de temperatura
  Serial.begin(9600);
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);
  pinMode(RELAY_PIN3, OUTPUT);
  pinMode(RELAY_PINup, OUTPUT);

  pid1.SetOutputLimits(0, 1);
  pid2.SetOutputLimits(0, 1);
  pid3.SetOutputLimits(0, 1);
  pidup.SetOutputLimits(0, 1);

  pid1.SetMode(AUTOMATIC);
  pid2.SetMode(AUTOMATIC);
  pid3.SetMode(AUTOMATIC);
  pidup.SetMode(AUTOMATIC);

  lcd.begin(16, 2);
  lcd.createChar (0, arriba);
  lcd.createChar (1, abajo);
  lcd.setCursor ( 0, 0 );
  // Imprimir otra cadena en esta posicion
  lcd.write((byte)1);
  lcd.print("Vulcanizadora");
  // Esperar un
  lcd.write((byte)0);

  lcd.setCursor ( 4, 1 );
  // Imprimir otra cadena en esta posicion
  lcd.print("RFIDUDES");
  // Esperar un
  delay(1000);
  inicio = millis();
  imprimir(temp1, temp2, 103.2, 104.2, setpoint);
  
  delay(500);
  
  for (int pin = 0; pin<=3; pin++){
      digitalWrite(tcCS[pin], LOW);
  }



  
}

void loop() {
  //Leer potenciometro y pasar a temperatura
  pote = analogRead(A0);
  setpoint = (double(pote) / 1023) ;
  lcd.setCursor ( 5, 0 );
  s = String(setpoint, 1);
  s = s.substring(0, 3);
  lcd.print(s);
  

  out1=setpoint;
  
  out2=setpoint;
  
  out3=setpoint;
  
  outup=setpoint;
  
  // loop ventana, es lo que sucede una vez cada por cada ciclo de trabajo, al inicio
  if (millis() - inicio > ventana) {

    //reinicializar el inicio de la ventana
    inicio = millis();



    // leer termocuplas

    for (int cs = 0; cs<=3; cs++){
      digitalWrite(tcCS[cs], LOW);
      delay(0);
      //Serial.print("temperatūras devējs Nr. ");
       //Serial.print(pin);
       //Serial.print(" ");
        //++pin;
        //Serial.println(thermocouple[cs].readCelsius());
        digitalWrite(tcCS[cs], HIGH);
        temp[cs] = thermocouple[cs].readCelsius();
        delay(0);
        }


  
    temp1 = temp[0];
    temp2 = temp[1];
    temp3 = temp[2];
    tempup = temp[3];
    //Calcular salida pid


    //imprimir temperaturas y setpoint a la pantalla

    imprimir(temp1, temp2, temp3, tempup, 100*setpoint);

    //
    Serial.print(temp1);
    Serial.print(",");
    Serial.print(temp2);
    Serial.print(",");
    Serial.print(temp3);
    Serial.print(",");
    Serial.print(tempup);
    Serial.print(",");
    Serial.println(setpoint);
      
    //     tempup=tparup.readCelsius();
  }
  if (out1 > (millis() - inicio) / ventana) digitalWrite(RELAY_PIN1, HIGH);
  else digitalWrite(RELAY_PIN1, LOW);

  if (out2 > (millis() - inicio) / ventana) digitalWrite(RELAY_PIN2, HIGH);
  else digitalWrite(RELAY_PIN2, LOW);

  if (out3 > (millis() - inicio) / ventana) digitalWrite(RELAY_PIN3, HIGH);
  else digitalWrite(RELAY_PIN3, LOW);

  if (outup > (millis() - inicio) / ventana) digitalWrite(RELAY_PINup, HIGH);
  else digitalWrite(RELAY_PINup, LOW);

  //


}


void imprimir(float t1, float t2, float t3, float t4, float ref) {
  lcd.setCursor ( 0, 0 );
  lcd.print(" ");
  s = String(ref, 1);
  s = s.substring(0, 3);

  lcd.print("REF=");
  lcd.print(s);
  lcd.print(" ");
  lcd.print("T");
  lcd.write((byte)0);
  lcd.print("=");
  s = String(t4, 1);
  s = s.substring(0, 3);
  lcd.print(s);
  lcd.setCursor ( 0, 1 );
  lcd.write((byte)1);
  lcd.print(" ");
  s = String(t1, 1);
  s = s.substring(0, 3);
  lcd.print(s);
  lcd.print("  ");
  s = String(t2, 1);
  s = s.substring(0, 3);
  lcd.print(s);
  lcd.print("  ");
  s = String(t3, 1);
  s = s.substring(0, 3);
  lcd.print(s);

}
