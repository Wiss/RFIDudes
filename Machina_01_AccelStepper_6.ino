// programa para hacer funcionar la machina
// V0.0 : Se definen los motores y la forma de acceder a cada uno
// V0.1 : se define el funcionamiento de los motores teniendo en cuenta la cantidad de pasos y una frecuencia de funcionamiento con función movmot
// V0.2 : Se utiliza librería AccelStepper para controlar motores.
//         ->Falta incluir mensajes de error por serial

// Disponibles para limit switch pines 22 al 31

#define LS_frnt 22
#define LS_izq  23
#define LS_der  24
#define LS_tras 25

#include <AccelStepper.h>
#include  <Wire.h>                      //libreria para protocolo I2C
#include  <LiquidCrystal_I2C.h>         //libreria para lcd por i2c

int warning = 0;

long pasos_totales = 6;

int steps_retract = -100;
int steps_gancho_izq  = 1800;        // 1.25 mm/vuelta.
int steps_gancho_der  = 1800;        // 1.25 mm/vuelta.
int steps_gancho_corte = 200;   // 1.25 mm/vuelta.
int steps_cable = (300-104) / 8 * 200; // 8 mm/vuelta.   0.4 * 300 / 8 * 200
int steps_enrol = -245;          // una vuelta considerando reducción 
int steps_meand = 600;          // tres vueltas
int steps_meand_fin = 200;          // una vueltas
int steps_solid_enrol = 200 * 32 / 4; //steps_enrol*31.5/4; //Por cada vuelta del motor 4 y 5 se avanzan 31.5*2 mm. Entonces, el mot9 necesita 31.5*2/8 vueltas (8 mm por vuelta mot 9)
int steps_solid_meand = 0.85*(steps_meand * 9.56 / 2); //steps_meand*9.56/2;    //Por cada vuelta del motor 6 y 7 se avanzan ((15.56-6)*2)*2 mm. Entonces, el mot9 necesita 9.56*2*2/8 vueltas (8 mm por vuelta mot 9)


long  vel         = 1000;
long  vel_cable   = vel;
long  vel_retract = vel / 2;
long  vel_gancho  = vel;
//long  vel_enrol   = vel/13;
float vel_solid_meand = vel;
float vel_solid_enrol = vel;     //steps_enrol*vel_enrol = steps_solid_enrol*vel_solid_enrol
float vel_enrol = 0.5*(vel_solid_enrol * steps_enrol / steps_solid_enrol); // el factor de 0.5 es al ojo
float vel_meand = (vel_solid_meand * steps_meand / steps_solid_meand);

int acel = vel;                 //Aceleración igual para todos los motores
long pasos_LS = 1000000;          //Número de pasos grande para while(!LS)

// pin enable
const int enable  = 30;
long ultimaVentana;
long lastmillis;


//motor 1, 1.7A
AccelStepper stepper1(1, 52, 53); // (Simple_MODE, STEP, DIR). Step siempre numero menor

//motor 2, 1.7A
AccelStepper stepper2(1, 50, 51); // DERECHO

//motor 3, 1.7A
AccelStepper stepper3(1, 48, 49);  // IZQUIERDO

//motor 4, 2A
AccelStepper stepper4(1, 46, 47);

//motor 5, 2A
AccelStepper stepper5(1, 44, 45);

//motor 6, 2A
AccelStepper stepper6(1, 42, 43);

//motor 7, 2A
AccelStepper stepper7(1, 40, 41);

//motor 8, 2A
AccelStepper stepper8(1, 38, 39);

//motor 9, 2A
AccelStepper stepper9(1, 36, 37);

//motor 10, 1.7A
AccelStepper stepper10(1, 34, 35);


//declarar pantalla LSD
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
String lastUp, lastDown;
int tamanoString;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(enable, OUTPUT);
  digitalWrite(enable, HIGH);
  //  lcd.begin(16, 2);

  //  lcd.setCursor ( 3, 0 );
  // lcd.print("MACHINA 01");
  // Esperar un

  //  lcd.setCursor ( 4, 1 );
  // Imprimir otra cadena en esta posicion
  //  lcd.print("RFIDUDES");
  // Esperar un
  delay(5000);

  // setup para ajustar ganchos
  stepper2.setMaxSpeed(vel_gancho);
  stepper2.setAcceleration(acel);
  stepper2.moveTo(-0);    //DERECHO. positivo abre, negativo cierra
  stepper3.setMaxSpeed(vel_gancho);
  stepper3.setAcceleration(acel);
  stepper3.moveTo(-0);    //IZQUIERDO. positivo abre, negativo cierra
  
  // setup para ajustar motores 4 y 5 
  stepper4.setMaxSpeed(vel_enrol);
  stepper4.setAcceleration(acel);
  stepper4.moveTo(0);    //CAMBIAR
  stepper5.setMaxSpeed(vel_enrol);
  stepper5.setAcceleration(acel);
  stepper5.moveTo(0);    //CAMBIAR
  
  // setup para ajustar motores 9
  stepper9.setMaxSpeed(vel_cable);
  stepper9.setAcceleration(acel);
  stepper9.moveTo(-9*200/8);    // positivo izquierdo, negativo derecho. steps_cable

  // setup para ajustar motores 9
  stepper8.setMaxSpeed(vel);
  stepper8.setAcceleration(acel);
  stepper8.moveTo(0);    //negativo a tras, positivo a front 
  
  // setup para ajustar motores meandros (6 y 7)
  stepper6.setMaxSpeed(vel_meand);
  stepper6.setAcceleration(acel);
  stepper6.moveTo(-200);    //negativo. steps_meand. DERECHO
  stepper7.setMaxSpeed(vel_enrol);
  stepper7.setAcceleration(vel_meand);
  stepper7.moveTo(200);    //positivo. steps_meand. IZQUIERDO
 
  ejecutar(8);
}

void loop() {

  //Prueba
  //
  //for(int i=0; i<=6; i++){
  ////  imprimirOnceUp("paso "+String(i));
  //  ejecutar(i);
  //  delay(200);
  //}


  // para ajustar parametros/movimientos
  //stepper2.run(); // DER
  //stepper3.run(); // IZQ
  //stepper4.run(); // DER
  //stepper5.run(); // IZQ
  //stepper6.run(); //DER
  //stepper7.run(); // IZQ
  //stepper8.run(); //  Y
  //stepper9.run(); // X
}

void ejecutar(int Paso) {

  /**************
  *** PASO 0 ***
  **************/

  if (Paso == 0) {
    // lcd.setCursor ( 3, 0 );
    // Esperar un


    // Imprimir otra cadena en esta posicion
    //  imprimirOnceDown("homing");
    //Setear motores
    //Dirección deseada de motores se setea con signo dentro de steps en stepper.moveTo

    stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho
    stepper4.setAcceleration(acel);
    stepper4.moveTo(steps_enrol);     //giro 
    stepper5.setMaxSpeed(vel_enrol);  //enrollador izquierdo
    stepper5.setAcceleration(acel);
    stepper5.moveTo(-steps_enrol);    //giro 
    
    stepper8.setMaxSpeed(vel);
    stepper8.setAcceleration(acel);
    stepper8.moveTo(pasos_LS);

    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(pasos_LS);

    //Motor 8 se mueve hasta limit switch frontal

    while (!digitalRead(LS_frnt)) {
      stepper8.run();
      //delay(2);
      //      imprimirOnceDown("Moviendo motor8");
      if (digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras)) {
        //Serial.println("error 88 break LS_frnt loop");

        //        imprimirOnceDown("er88 LS_frntloop");
        warning = 88;
        break;
      }
    }
    //    imprimirOnceDown("out Primer While");
    //Serial.println("Paso 0: salida primer while");
    //    delay(200);

    if (warning != 0) {
      //Imprimir número de warning en serial o pantalla
      //Serial.println("Error 88 break main loop");
      //      imprimirOnceDown("er88 main loop");
      //     delay(200);
      exit(0);
    }

    //Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch

    stepper8.setCurrentPosition(0);
    stepper8.setSpeed(vel_retract);
    stepper8.moveTo(steps_retract);
    while (true) {
      //Serial.println("Paso 0: retract loop");
      //      imprimirOnceDown("retract loop");
      //Serial.println(stepper8.currentPosition());
      stepper8.run();
      //delay(2);
      if (!stepper8.run()) {
        //Serial.println("Paso 0: Salida retract loop");
        //        imprimirOnceDown("er88 retractLoop");
        break;
      }
    }
    //Serial.println("PRUEBA3");
    //Motor 9 se mueve hasta limit switch izquierdo
    while (!digitalRead(LS_izq)) {
      //      imprimirOnceDown("Loop LSizquierdo");
      //Serial.println("Paso 0: Loop a LS izquierdo");
      stepper9.run();
      //delay(2);
      if (digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_frnt)) {
        warning = 88;
        break;
      }
    }
    if (warning != 0) {
      //   imprimirOnceDown("er88 LS_izqLoop");
      //Serial.println("error 88 break LS_izq loop");
      exit(0);
    }
    //Motor 9 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    stepper9.setCurrentPosition(0);
    stepper9.setSpeed(vel_retract);
    stepper9.moveTo(steps_retract);
    while (true) {
      //      imprimirOnceDown("retract loop2");
      stepper9.run();
      //delay(2);
      if (!stepper9.run()) {
        //       imprimirOnceDown("Out retractLoop2");
        break;
      }
    }
    // motores 4 y 5 retornan una vuelta
        while (true) {
      stepper4.run();
      stepper5.run();
      if (!stepper4.run() && !stepper5.run()) {
        break;
      }
    }

  }//paso 0

  /**************
   *** PASO 1 ***
   **************/

  if (Paso == 1) {

    stepper8.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);

    stepper8.setMaxSpeed(vel);
    stepper8.setAcceleration(acel);
    stepper8.moveTo(-pasos_LS);       //Dirección contraria a paso 0

    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-pasos_LS);       //Dirección contraria a paso 0

    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel);
    stepper2.moveTo(steps_gancho_der);    //CAMBIAR

    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(steps_gancho_izq);    //CAMBIAR

    //Motor 8 se mueve hasta limit switch trasero

    while (!digitalRead(LS_tras)) {
      stepper8.run();
      //delay(2);
      if (digitalRead(LS_der) || digitalRead(LS_frnt) || digitalRead(LS_izq)) {
        warning = 1;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch

    stepper8.setCurrentPosition(0);
    stepper8.setSpeed(vel_retract);
    stepper8.moveTo(-steps_retract);

    while (true) {
      stepper8.run();
      if (!stepper8.run()) {
        break;
      }
    }

    //Motor 9 se mueve hasta limit switch derecho
    while (!digitalRead(LS_der)) {
      stepper9.run();
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)) {
        warning = 1;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    //Motor 9 se devuelve una cantidad determinada de pasos para no tocar el limit switch

    stepper9.setCurrentPosition(0);
    stepper9.setSpeed(vel_retract);
    stepper9.moveTo(-steps_retract);

    while (true) {
      stepper9.run();
      if (!stepper9.run()) {
        break;
      }
    }

    //Se abren ganchos

    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)) {
        warning = 1;
        break;
      }
      stepper2.run();
      //delay(2);
      stepper3.run();
      if (!stepper2.run() && !stepper3.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

  }//paso 1

  /**************
   *** PASO 2 ***
   **************/

  if (Paso == 2) {

    stepper9.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);

    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(steps_cable);       //Dirección contraria a paso 0


    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(-steps_gancho_izq);    //Dirección contraria a paso 0

    //Cerrar gancho izquierdo para agarrar cable
    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)) {
        warning = 2;
        break;
      }
      stepper3.run();
      if (!stepper3.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    //Estirar cable hasta la izquierda

    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)) {
        warning = 2;
        break;
      }
      stepper9.run();
      if (!stepper9.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
  }//paso 2

  /**************
   *** PASO 3 ***
   **************/
  if (Paso == 3) {

    stepper2.setCurrentPosition(0);

    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel);
    stepper2.moveTo(-steps_gancho_der);    //Dirección contraria a paso 0 (cerrar)

    //Cerrar gancho derecho para cortar - agarrar cable

    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)) {
        warning = 3;
        break;
      }
      stepper2.run();
      if (!stepper2.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Desactivar motores

  }//paso 3


  /**************
  *** PASO 4 ***
  **************/

  if (Paso == 4) {
    
    stepper8.setCurrentPosition(0);

    stepper8.setMaxSpeed(vel);
    stepper8.setAcceleration(acel);
    stepper8.moveTo(pasos_LS);

    //Motor 8 se mueve hasta limit switch frontal
    while (!digitalRead(LS_frnt)) {
      stepper8.run();
      //delay(2);
      if (digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras)) {
        warning = 4;
        break;
      }
    }
    
    if (warning != 0) {
      exit(0);
    }
    }//paso 4

  /**************
  *** PASO 5 ***
  **************/

  if (Paso == 5) {
    stepper4.setCurrentPosition(0);
    stepper5.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);

    stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho
    //stepper4.setSpeed(vel_enrol);
    stepper4.setAcceleration(acel);
    stepper4.moveTo(-steps_enrol);     //giro horario

    stepper5.setMaxSpeed(vel_enrol);  //enrollador izquierdo
    //stepper5.setSpeed(vel_enrol);
    stepper5.setAcceleration(acel);
    stepper5.moveTo(steps_enrol);    //giro anti horario

    stepper9.setMaxSpeed(vel_solid_enrol);  //setMaxSpeed? REVISAR
    //stepper9.setSpeed(vel_enrol);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-steps_solid_enrol);  
    
    //Motor 4 y 5 enrollan 360 y 9 se mueve solidareamente

    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || !digitalRead(LS_frnt)) {
        warning = 5;
        break;
      }
      stepper4.run();
      stepper5.run();
      stepper9.run();
      if (!stepper4.run() && !stepper5.run() && !stepper9.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Desactivar motores


  }//paso 5


  /**************
  *** PASO 6 ***
  **************/

  if (Paso == 6) {

    stepper6.setCurrentPosition(0);
    stepper7.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);

    stepper6.setMaxSpeed(vel_meand);  //meandros derecho
    stepper6.setAcceleration(acel);
    stepper6.moveTo(-steps_meand);     
    
    stepper7.setMaxSpeed(vel_meand);  //meandros izquierdo
    stepper7.setAcceleration(acel);
    stepper7.moveTo(steps_meand);     

    stepper9.setAcceleration(acel);
    stepper9.setMaxSpeed(vel_solid_meand);     //setSpeed? REVISAR
    stepper9.moveTo(-steps_solid_meand);     

    //Motor 6 y 7 generan meandros (3 revoluciones) y 9 se mueve solidareamente
    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || !digitalRead(LS_frnt)) {
        warning = 7;
        break;
      }
      stepper6.run();
      stepper7.run();
      stepper9.run();
      if (!stepper6.run() && !stepper7.run() && !stepper9.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
  }//paso 6

    /**************
  *** PASO 7 ***
  **************/

  if (Paso == 7) {
    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-pasos_LS);
    //Motor 9 se mueve hasta limit switch derecho para terminar central loop
    while (!digitalRead(LS_der)) {
      stepper9.run();
      if (digitalRead(LS_izq) || digitalRead(LS_tras)) {
        warning = 8;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }  
  }//paso 7

  /**************
  *** PASO 8 ***
  **************/

  if (Paso == 55) {
    ;
    //Soldadura


  }//paso 8

  /**************
  *** PASO 9 ***
  **************/

  if (Paso == 8) { // CAMBIAR A 9

    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper6.setCurrentPosition(0);
    stepper7.setCurrentPosition(0);
    stepper8.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);

    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel);
    stepper2.moveTo(-steps_gancho_corte);    //Dirección de cierre (-)

    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(-steps_gancho_corte);    //Dirección de cierre (-)

    stepper6.setMaxSpeed(vel_meand);  //meandros derecho
    stepper6.setAcceleration(acel);
    stepper6.moveTo(-steps_meand_fin);   //REVISAR SENTIDO

    stepper7.setMaxSpeed(vel_meand);  //meandros derecho
    stepper7.setAcceleration(acel);
    stepper7.moveTo(steps_mean_fin);   //REVISAR SENTIDO

    stepper8.setMaxSpeed(vel_meand);
    stepper8.setAcceleration(acel);
    stepper8.moveTo(-pasos_LS);     //REVISAR SENTIDO

    stepper9.setSpeed(vel_meand);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-steps_meand);     //REVISAR SENTIDO

    //Motor 6 y 7 vuelven a posicion inicial (1 revoluciones)
    while (digitalRead(LS_der) && digitalRead(LS_frnt)) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras)) {
        warning = 9;
        break;
      }
      stepper6.run();
      stepper7.run();
      if (!stepper6.run() && !stepper7.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Motor 8 se mueve hasta limit switch trasero
    while (!digitalRead(LS_tras)) {
      stepper8.run();
      if (digitalRead(LS_izq) || !digitalRead(LS_der)) {
        warning = 9;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch

    stepper8.setCurrentPosition(0);
    stepper8.setMaxSpeed(vel_retract); //SetSpeed?
    stepper8.moveTo(-steps_retract);

    while (true) {
      stepper8.run();
      if (!stepper8.run()) {
        break;
      }
    }

    //Se cierran ganchos

    while (true) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || !digitalRead(LS_der)) {
        warning = 9;
        break;
      }
      stepper2.run();
      stepper3.run();
      if (!stepper2.run() &&  !stepper3.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Se abre ganchos

    stepper2.setCurrentPosition(0);
    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel);
    stepper2.moveTo(steps_gancho_der + steps_gancho_corte);

    stepper3.setCurrentPosition(0);
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(steps_gancho_der + steps_gancho_corte);

    while (true) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || !digitalRead(LS_der)) {
        warning = 9;
        break;
      }
      stepper2.run();
      stepper3.run();
      if (!stepper2.run() && !stepper3.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    //Ganchos a posicion inicial
    stepper2.setCurrentPosition(0);
    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel);
    stepper2.moveTo(-steps_gancho_der); //REVISAR SENTIDOS

    stepper3.setCurrentPosition(0);
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(-steps_gancho_der); //REVISAR SENTIDOS
    while (true) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || !digitalRead(LS_der)) {
        warning = 9;
        break;
      }
      stepper2.run();
      stepper3.run();
      if (!stepper2.run() && !stepper3.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }

    //Soltar LS_der
    stepper9.moveTo(-steps_retract * 100);
    while (digitalRead(LS_der)) {
      stepper9.run();
    }

  }//paso 9



}//ejecutar

//void imprimirOnceDown( 2){
//if (!(lastDown.equals(strdown))){
// lcd.setCursor(0,1);
// lcd.print("                ");
// tamanoString=strdown.length();
// lcd.setCursor ((16-tamanoString)/2 , 1 );
// lcd.print(strdown);
// lastDown=strdown;
//}

//}

//void imprimirOnceUp( String strup){
//if (!lastUp.equals(strup)){
//  lcd.setCursor(0,0);
//lcd.print("                ");
//tamanoString=strup.length();
//lcd.setCursor ((16-tamanoString)/2 , 0 );
//lcd.print(strup);
//lastUp=strup;
//}
//}
