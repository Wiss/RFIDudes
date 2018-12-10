// programa para hacer funcionar la machina
// V0.0 : Se definen los motores y la forma de acceder a cada uno
//input limit swithc pruea
//const int limit1 = 22;
// V0.1 : se define el funcionamiento de los motores teniendo en cuenta la cantidad de pasos y una frecuencia de funcionamiento


const int limit1 = 22;
int limit1state = 0;
bool mecambie = 0;
String Mot;
String Step;
String Dir;


// pin enable
const int enable  = 52;
const long Periodon = 10 / 2;
long ultimaVentana;
long lastmillis;


//motor 1
const int step1 = 50;
const int dir1  = 51;
bool mot1 = false;
long cont1;
long ultiVent1;

//motor 2
const int step2 = 48;
const int dir2  = 49;
bool mot2 = false;
long cont2;
long ultiVent2;

//motor 3
const int step3 = 46;
const int dir3  = 47;
bool mot3 = false;
long cont3;
long ultiVent3;

//motor 4
const int step4 = 44;
const int dir4  = 45;
bool mot4 = false;
long cont4;
long ultiVent4;


//motor 5
const int step5 = 42;
const int dir5  = 43;
bool mot5 = false;
long cont5;
long ultiVent5;

//motor 6
const int step6 = 40;
const int dir6  = 41;
bool mot6 = false;
long cont6;
long ultiVent6;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //  pinMode(limit1, INPUT);

  pinMode(step1, OUTPUT);
  pinMode(dir1, OUTPUT);

  pinMode(step2, OUTPUT);
  pinMode(dir2, OUTPUT);

  pinMode(step3, OUTPUT);
  pinMode(dir3, OUTPUT);

  pinMode(step4, OUTPUT);
  pinMode(dir4, OUTPUT);

  pinMode(step5, OUTPUT);
  pinMode(dir5, OUTPUT);

  pinMode(step6, OUTPUT);
  pinMode(dir6, OUTPUT);

  pinMode(enable, OUTPUT);
  pinMode(limit1, OUTPUT);
  digitalWrite(enable, HIGH);
  digitalWrite(dir6, LOW);
  mot6 = true;
  mot1 = true;

}

void loop() {
  limit1state = digitalRead(limit1);
  if (limit1state == HIGH) {

    if (~mecambie) {
      int estadoanterior = digitalRead(dir6);
      if (estadoanterior == HIGH) {
        digitalWrite(dir6, LOW );
      }
      else {
        digitalWrite(dir6, HIGH );
      }
      mot6 = false;
      mecambie = true;
    }

  }
  else if (mecambie) {
    Serial.println("apretao");
    mecambie = false;
    mot6 = true;
  }
  else {
    mov6Mot(10);
  }


}

//void ejecutar() {
//  if (millis() - ultimaVentana < Periodon ) {
//    if (mot1) {
//      digitalWrite(step1, HIGH);
//    }
//    if (mot2) {
//      digitalWrite(step2, HIGH);
//    }
//    if (mot3) {
//      digitalWrite(step3, HIGH);
//    }
//    if (mot4) {
//      digitalWrite(step4, HIGH);
//    }
//    if (mot5) {
//      digitalWrite(step5, HIGH);
//    }
//    if (mot6) {
//      digitalWrite(step6, HIGH);
//    }
//
//  }
//  else if (millis() - ultimaVentana <  Periodo) {
//
//
//    if (mot1) {
//      digitalWrite(step1, LOW);
//    }
//    if (mot2) {
//      digitalWrite(step2, LOW);
//    }
//    if (mot3) {
//      digitalWrite(step3, LOW);
//    }
//    if (mot4) {
//      digitalWrite(step4, LOW);
//    }
//    if (mot5) {
//      digitalWrite(step5, LOW);
//    }
//    if (mot6) {
//      digitalWrite(step6, LOW);
//    }
//  }
//  else {
//    ultimaVentana = millis();
//  }
//}
//
//
//






