// programa para hacer funcionar la machina
// V0.0 : Se definen los motores y la forma de acceder a cada uno
// V0.1 : se define el funcionamiento de los motores teniendo en cuenta la cantidad de pasos y una frecuencia de funcionamiento con función movmot
// V0.2 : Se utiliza librería AccelStepper para controlar motores.
//         ->Falta incluir mensajes de error por serial

// Disponibles para limit switch pines 22 al 31

// MODIFICAR SetCurrentPosition() en motores 2 y 3 (Ganchos) !!

#define LS_cort_der 24  // Limit switch para ajustar nivel cero gancho corte derecho 
#define LS_cort_izq 25  // Limit switch para ajustar nivel cero gancho corte izquierdo
#define LS_der  26 // Limit switch derecho
#define LS_tras 27  // Limit switch trasero
#define LS_frnt 28  // Limit switch frontal 
#define LS_izq  29  // Limit switch izquierdo
#define LS_niv_der 30  //Limit switch para ajustar nivel motor derecho 
#define LS_niv_izq 31  //Limit switch para ajustar nivel motor izquierdo

#include <AccelStepper.h>
#include  <Wire.h>                      //libreria para protocolo I2C
#include  <LiquidCrystal_I2C.h>         //libreria para lcd por i2c

int warning = 0;

long pasos_totales = 9;  // numero de pasos que debe realizar la máquina para construir una antena 

//* pasos generales asociados a motores stepers
int steps_retract = -100;    //se devuelve una cantidad determinada de pasos para no tocar el limit switch
int steps_feed = -200;        // al implementar alimentador de cable
int steps_retract_mot9 = 12 / 8 * 200 ; // parametro utilizado para separarse el limit switch derecho al momento de soltar la antena
int steps_retract_mot8 = -30 / 8 * 200 ; // parametro utilizado para separarse el limit switch derecho al momento de soltar la antena
int steps_retract_gancho = 20;       // cantidad de pasos que se devuelve el gancho (al cortar) luego de hacer contacto con el limitswitch
int steps_gancho_izq  = 1500;        // 1.25 mm/vuelta.
int steps_gancho_der  = 1400;        // 1.25 mm/vuelta.
int steps_gancho_corte_der = 60;   // 1.25 mm/vuelta.
int steps_gancho_corte_izq = 60;   // 1.25 mm/vuelta.
int steps_cable = (191 + 10)/ 8 * 200 ; // 8 mm/vuelta. Distancia recorrida por motor 8 (eje X). Define el largo del cable 
int steps_enrol = 600;          // una vuelta considerando reducción 
int steps_retract_niv_der = 50; // pasos que se devuelve luego de tocar el LS para nivelar gancho der
int steps_retract_niv_izq = 50; // pasos que se devuelve luego de tocar el LS para nivelar gancho izq
int pasos = 0; // pasos comodin
int k = 0; // contador de pasos
int pasos_k = 0; // activar condición asociada a contador de pasos

//* Pasos meandros
// con 1 reductora
//int steps_meand = 3 * (200 * 4.6) + 2;          // tres vueltas. 3 x (steps 1 vuelta), donde step 1 vuelta = 200 * 4.6
//int steps_meand_fin = (200 * 4.6) + 1;          // una vueltas
//int steps_solid_meand = (steps_meand * 9.56 / 4)* 1/4.6; //steps_meand*9.56/2;    //Por cada vuelta del motor 6 y 7 se avanzan..
//..((15.56-6)*2)*2 mm. Entonces, el mot9 necesita  9.56*2*2/8 vueltas (8 mm por vuelta mot 9)
// Anterior y basado en pruebas empiricas
//int steps_solid_meand = (1.12 * (steps_meand * 8 / 4) * 1/4.6) / 4.6;  //steps_meand*8/2;    //Por cada vuelta del motor 6 y 7 se avanzan ..
//((13-5)*2)*2 mm. Entonces, el mot9 necesita  8*2*2/8 vueltas. 

// con 2 reductoras
int steps_meand = (3 * (200 * 4.6) + 2) * 4.6;          // tres vueltas. 3 x (steps 1 vuelta), donde step 1 vuelta = 200 * 4.6 (4.6 es por la reductora)
int steps_meand_fin = ((200 * 4.6) + 1) * 4.6;          // una vueltas
// con 2 reductoras
int steps_solid_meand = (steps_meand * 9.56 / 4)* 1/4.6 * 1/4.6; //steps_meand*9.56/2;    //Por cada vuelta del motor 6 y 7 se avanzan..
//((15.56-6)*2)*2 mm. Entonces, el mot9 necesita  9.56*2*2/8 vueltas (8 mm por vuelta mot 9)

//* Pasos end-loads
int steps_solid_enrol = 0.99 * (200 * 63 / 4); //steps_enrol*63/4; //Por cada vuelta del motor 4 y 5 se avanzan 63*2 mm. 
// ..Entonces, el mot9 necesita 63*2/8 vueltas (8 mm por vuelta mot 9)
// - (5 * steps_meand / 100 * 200 / 8);
int steps_enrol_demas = 50; // 1/20 de vuelta del motor 4 o 5 aprox. Giro adicional de gancho para obtener end-loads completos

//velocidades
long  vel         = 1000;
long  vel_acople = vel/3;  // velocidad de acoplamiento asociado al paso 4
long  vel_cable   = vel;  
long  vel_retract = vel / 2;  // velocidad retracción luego de tocar limit switch (general)
long  vel_retract_mot8 = vel / 5;  // velocidad específica mot 9 retracción luego de tocar limit switch
long  vel_retract_mot9 = vel / 5;  // velocidad específica mot 9 retracción luego de tocar limit switch
long  vel_gancho  = vel;    // velocidad asociada al corte realizado por los ganchos
long  vel_feed   = vel/4;   // velocidad de alimentación del cable de cobre

// relaciones velocidad enrolamiento end-loads
float vel_solid_enrol = vel;     //steps_enrol*vel_enrol = steps_solid_enrol*vel_solid_enrol
float vel_enrol = (vel_solid_enrol * steps_enrol / steps_solid_enrol); 
// float vel_enrol = (vel_solid_enrol * 200 / steps_solid_enrol); // 200 en lugar de 245 (?)
// relacione velocidad meandros
// con reductora
float vel_meand = vel; // aqui esta la restriccion de mayor velocidad para la machina dado el REDUCTOR
float vel_solid_meand = 0.75*(vel_meand * steps_solid_meand / steps_meand);  // CON reductora  // ESTABA EN 0.69 1/agosto

//aceleraciones
int acel = 2*vel;                 //Aceleración igual para todos los motores
int acel_gancho = 3*vel;                 //Aceleración igual para todos los motores
long pasos_LS = 1000000;          //Número de pasos grande para while(!LS)
int acel_feed = 2*vel_feed; 

// pin enable
const int enable  = 53;
long ultimaVentana;
long lastmillis;


// MOTORES: (Simple_MODE, STEP, DIR). Step siempre numero menor

//motor 1, 1.7A
AccelStepper stepper1(1, 46, 47); // FEEDER
//motor 2, 1.7A
AccelStepper stepper2(1, 42, 43); // GANCHO DERECHO
//motor 3, 1.7A
AccelStepper stepper3(1, 38, 39);  // GANCHO IZQUIERDO
//motor 4, 2A
AccelStepper stepper4(1, 34, 35); // ENRROLLADOR DERECHO
//motor 5, 2A
AccelStepper stepper5(1, 48, 49); // ENRROLLADOR IZQUIERDO
//motor 6, 2A
AccelStepper stepper6(1, 44, 45); // MEANDROS DERECHO
//motor 7, 2A
AccelStepper stepper7(1, 40, 41); // MEANDROS IZQUIERDO
//motor 8, 2A
AccelStepper stepper8(1, 36, 37); // EJE Y
//motor 9, 2A
AccelStepper stepper9(1, 32, 33); // EJE X
//motor 10, 1.7A
//AccelStepper stepper10(1, 34, 35); 
//AccelStepper stepper10(1, 34, 35); 


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
  delay(100);

  // setup para ajustar feeder manualmente
  stepper1.setMaxSpeed(vel_feed);
  stepper1.setAcceleration(acel_feed);
  stepper1.moveTo(steps_feed);    // Valor de steps negativo: alimenta machina.

  // setup para ajustar ganchos manualmente
  stepper2.setMaxSpeed(vel_gancho);
  stepper2.setAcceleration(acel_gancho);
  //stepper2.moveTo(steps_gancho_der);    // DERECHO. positivo abre, negativo cierra. steps_gancho_der
  //stepper2.moveTo(steps_gancho_corte_der);
  //
  stepper2.moveTo(-200);
  
  stepper3.setMaxSpeed(vel_gancho);
  stepper3.setAcceleration(acel_gancho);
  //stepper3.moveTo(steps_gancho_izq);    // IZQUIERDO. positivo abre, negativo cierra. steps_gancho_izq
  //stepper3.moveTo(steps_gancho_corte_izq);
  //stepper3.moveTo(200); 
 
  // setup para ajustar motores 4 y 5  manualmente
  stepper4.setMaxSpeed(vel_enrol);
  stepper4.setAcceleration(acel);
  stepper4.moveTo(3*steps_enrol);    // DERECHO. dirección en funcionamiento normal: positiva. 
  
  stepper5.setMaxSpeed(vel_enrol);
  stepper5.setAcceleration(acel);
  stepper5.moveTo(-3*steps_enrol);    // IZQUIERDO. dirección en funcionamiento normal: negativa. 
  
  // setup para ajustar motores meandros (6 y 7) manualmente
  stepper6.setMaxSpeed(vel_meand);
  stepper6.setAcceleration(acel);
  stepper6.moveTo(-steps_meand/3*1.);    //dirección en funcionamiento normal: negativa. steps_meand. DERECHO
  //stepper6.moveTo(-40);
  
  stepper7.setMaxSpeed(vel_meand);
  stepper7.setAcceleration(acel);
  stepper7.moveTo(steps_meand/3*1.);    //dirección en funcionamiento normal: positiva. steps_meand. IZQUIERDO

  // setup para ajustar motores 8 manualmente
  stepper8.setMaxSpeed(vel_retract_mot8);
  stepper8.setAcceleration(acel);
  stepper8.moveTo(-10 / 8 * 200 );    //negativo a tras, positivo a front. Y -> 8mm/vuelta

  // setup para ajustar motor 9 manualmente
  stepper9.setMaxSpeed(vel_cable);
  stepper9.setAcceleration(acel);
  stepper9.moveTo(1*200/8);    // positivo izquierdo (abre), negativo derecho (cierra). steps_cable. X -> 8mm/vuelta. steps_cable
  //stepper9.moveTo(steps_solid_enrol)

  // Pasos para generar una antena
    
  //ejecutar(0); // HOME
  //calibrar_niv(4,-1);   // NIVEL GANCHO DERECHO. 1 DIRECCCION DE FUNCIONAMIENTO NORMAL (-1) LO CONTRARIO
  //calibrar_niv(5,-1);   //  NIVEL GANCHO 1 DIRECCCION DE FUNCIONAMIENTO NORMAL (-1) LO CONTRARIO
  //calibrar_cort0(2); // PROFUNDIDAD CORTE 0 GANCHO IZQUIERDO 
  //calibrar_cort0(3);  // PROFUNDIDAD CORTE 0 GANCHO DERECHO
  
  //ejecutar(1); // ABRE GANCHOS
  //ejecutar(2); // CORTE GANCHO IZQUIERDO Y ESTIRAMIENTO CABLE
  //ejecutar(3); // CORTE GANCHO DERECHO
  //MOVER EJE X (AUMENTA TENSION)
  //ejecutar(4); // ACOPLE PIEZA TRASERA Y GANCHOS
  //ejecutar(5); // CONSTRUYE END-LOADS
  //ejecutar(6); // CONSTRUYE MEANDROS
  //ejecutar(7); // CENTRAL LOOP
  //ejecutar(8); // TERMINACIONES, APERTURA GANCHOS Y ENTREGA DE ANTENA
  //ejecutar(9); // CIERRE GANCHO



}

void loop() {

  //Prueba
  //
  //for(int i=0; i<=8; i++){
    ////  imprimirOnceUp("paso "+String(i));
  //if(i == 0){
  //ejecutar(0); // HOME
  //calibrar_niv(4,-1);   // NIVEL GANCHO DERECHO. 1 DIRECCCION DE FUNCIONAMIENTO NORMAL (-1) LO CONTRARIO
  //calibrar_niv(5,-1);   //  NIVEL GANCHO 1 DIRECCCION DE FUNCIONAMIENTO NORMAL (-1) LO CONTRARIO
  //calibrar_cort0(2); // PROFUNDIDAD CORTE 0 GANCHO IZQUIERDO 
  //calibrar_cort0(3);  // PROFUNDIDAD CORTE 0 GANCHO DERECHO
  //else
   // ejecutar(i);
   // delay(200);
  //}
  //}


  // para ajustar parametros/movimientos
  //if (digitalRead(LS_frnt) || digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras)) {
        //Serial.println("error 88 break LS_frnt loop");
  //}
  
  //stepper1.run(); // FEEDER
  //stepper2.run(); // GANCHO DER
  //stepper3.run(); // GANCHO IZQ
  //stepper4.run(); // GIRO DER
  //stepper5.run(); // GIRO IZQ
  //stepper6.run(); // MEANDRO DER
  //stepper7.run(); // MEANDRO IZQ
  //stepper8.run(); //  EJE Y
  //stepper9.run(); // EJE X
}

  /************************
  *** CALIBRACION NIVEL ***
  ************************/

void calibrar_niv(int mot, int dir) {
  // dir = 1 funcionamiento nomarl, d = -1 lo contrario
  // mot indica el motor a calibrar. 4 gancho der, 5 gancho izq

  //Motor 4 se gira hasta limit switch respectivo
  if(mot == 4){
    pasos = pasos_LS*dir;
    stepper4.setCurrentPosition(0);
    stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho. dirección en funcionamiento normal: positiva. 
    stepper4.setAcceleration(acel);
    stepper4.moveTo(pasos);    //giro
    
    k = 0;
    while (!digitalRead(LS_niv_der) || (digitalRead(LS_niv_der) && k < pasos_k)) {
      k = k + 1;
      stepper4.run();
      if (digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)|| digitalRead(LS_frnt)) {
        warning = 88;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    stepper4.setCurrentPosition(0);
    stepper4.setSpeed(vel_retract);
    stepper4.moveTo(-dir*steps_retract_niv_der);
    
  while (true) {
      stepper4.run();
      if (!stepper4.run()) {
        break;
      }
    }
  }
  
   //Motor 5 se gira hasta limit switch respectivo
  if(mot == 5){
    pasos = -pasos_LS*dir;
    stepper5.setCurrentPosition(0);
    stepper5.setMaxSpeed(vel_enrol);  //enrollador derecho. dirección en funcionamiento normal: negativa. 
    stepper5.setAcceleration(acel);  
    stepper5.moveTo(pasos);    //giro

    k = 0;
    while (!digitalRead(LS_niv_izq) || (digitalRead(LS_niv_izq) && k < pasos_k)) {
      k = k + 1;
      stepper5.run();
      if (digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)|| digitalRead(LS_frnt)) {
        warning = 88;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    stepper5.setCurrentPosition(0);
    stepper5.setSpeed(vel_retract);
    stepper5.moveTo(dir*steps_retract_niv_der);
    
  while (true) {
      stepper5.run();
      if (!stepper5.run()) {
        break;
      }
    } 
    }
}

  /**********************************
  *** CALIBRACION CORTE (NIVEL 0) ***
  ***********************************/

void calibrar_corte(int mot) {
  // mot indica el motor a calibrar. 2 gancho der, 3 gancho izq
    if(mot == 2){
    // motor 2 entra hasta limitswitch y luego sale hasta llegar a cero
    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel_gancho);
    stepper2.moveTo(-pasos_LS);   
      while (!digitalRead(LS_cort_der)) {
        stepper2.run();
        if (digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_izq)) {
          warning = 88;
          break;
          }
       }
      if (warning != 0) {
      exit(0);
      }
    stepper2.setCurrentPosition(0);
    stepper2.setSpeed(vel_retract);
    stepper2.moveTo(steps_retract_gancho);
    
  while (true) {
      stepper2.run();
      if (!stepper2.run()) {
        break;
      }
    }
  }


    if(mot == 3){
    // motor 3 entra hasta limitswitch y luego sale hasta llegar a cero
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel_gancho);
    stepper3.moveTo(-pasos_LS);   
      while (!digitalRead(LS_cort_izq)) {
        stepper3.run();
        if (digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_izq)) {
          warning = 88;
          break;
          }
       }
      if (warning != 0) {
      exit(0);
      }
    stepper3.setCurrentPosition(0);
    stepper3.setSpeed(vel_retract);
    stepper3.moveTo(steps_retract_gancho);
    
  while (true) {
      stepper3.run();
      if (!stepper3.run()) {
        break;
      }
    }
  }

}

void ejecutar(int Paso) {

  /**************
  *** PASO 0 ***
  **************/

  if (Paso == 0) {
    Serial.print(Paso);
    // lcd.setCursor ( 3, 0 );
    // Esperar un


    // Imprimir otra cadena en esta posicion
    //  imprimirOnceDown("homing");
    //Setear motores
    //Dirección deseada de motores se setea con signo dentro de steps en stepper.moveTo

    //stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho
    //stepper4.setAcceleration(acel);
    //stepper4.moveTo(-steps_enrol);     //giro 
    //stepper5.setMaxSpeed(vel_enrol);  //enrollador izquierdo
    //stepper5.setAcceleration(acel);
    //stepper5.moveTo(steps_enrol);    //giro 
    
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
      if (digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
      if (digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    //    while (true) {
    //  stepper4.run();
    //  stepper5.run();
    //  if (!stepper4.run() && !stepper5.run()) {
     //   break;
     // }
   // }


    // motor 3 entra hasta limitswitch y luego sale hasta llegar a cero
//    stepper3.setMaxSpeed(vel_gancho);
//    stepper3.setAcceleration(acel_gancho);
//    stepper3.moveTo(-pasos_LS);    
//
//  while (!digitalRead(LS_cort_izq)) {
//      stepper3.run();
//      if (digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_izq) || digitalRead(LS_cort_der)) {
//        warning = 88;
//        break;
//      }
//    }
//    if (warning != 0) {
//      exit(0);
//    }
//    stepper3.setCurrentPosition(0);
//    stepper3.setSpeed(vel_retract);
//    stepper3.moveTo(steps_retract_gancho);
//    
//    while (true) {
//      stepper3.run();
//      if (!stepper3.run()) {
//        break;
//      }
 //   }


  }//paso 0

  /**************
   *** PASO 1 ***
   **************/

  if (Paso == 1) {
    Serial.print(Paso);

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
    stepper2.setAcceleration(acel_gancho);
    stepper2.moveTo(steps_gancho_der);    //CAMBIAR

    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel_gancho);
    stepper3.moveTo(steps_gancho_izq);    //CAMBIAR

    //Motor 8 se mueve hasta limit switch trasero
    while (!digitalRead(LS_tras)) {
      stepper8.run();
      //delay(2);
      if (digitalRead(LS_der) || digitalRead(LS_frnt) || digitalRead(LS_izq) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    Serial.print(Paso);

    stepper9.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);

    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(steps_cable);       //Dirección contraria a paso 0


    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel_gancho);
    stepper3.moveTo(-steps_gancho_izq);    //Dirección contraria a paso 0

    //Cerrar gancho izquierdo para agarrar cable
    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    Serial.print(Paso);

    stepper2.setCurrentPosition(0);
    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel_gancho);
    stepper2.moveTo(-steps_gancho_der);    //Dirección contraria a paso 0 (cerrar)

    //Cerrar gancho derecho para cortar - agarrar cable
    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    Serial.print(Paso);
    
    stepper8.setCurrentPosition(0);
    stepper8.setMaxSpeed(vel_acople);
    stepper8.setAcceleration(acel);
    stepper8.moveTo(pasos_LS);

    //Motor 8 se mueve hasta limit switch frontal
    while (!digitalRead(LS_frnt)) {
      stepper8.run();
      //delay(2);
      if (digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    Serial.print(Paso);
    
    stepper4.setCurrentPosition(0);
    stepper5.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);

    stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho
    stepper4.setAcceleration(acel);
    stepper4.moveTo(steps_enrol + steps_enrol_demas);     //(+)--> normal

    stepper5.setMaxSpeed(vel_enrol);  //enrollador izquierdo
    stepper5.setAcceleration(acel);
    stepper5.moveTo(-steps_enrol - steps_enrol_demas);    //(-)--> normal

    stepper9.setMaxSpeed(vel_solid_enrol); 
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-steps_solid_enrol);  
    
    //Motor 4 y 5 enrollan 360 (y un poco mas: steps_enrol_demas) y 9 se mueve solidareamente
    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || !digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    Serial.print(Paso);

    stepper6.setCurrentPosition(0);
    stepper7.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);

    stepper6.setMaxSpeed(vel_meand);  //meandros derecho.  (+)--> normal
    stepper6.setAcceleration(acel);
    stepper6.moveTo(-steps_meand);     
    
    stepper7.setMaxSpeed(vel_meand);  //meandros izquierdo. (+)--> normal
    stepper7.setAcceleration(acel);
    stepper7.moveTo(steps_meand);     

    stepper9.setAcceleration(acel);
    stepper9.setMaxSpeed(vel_solid_meand);    
    stepper9.moveTo(-steps_solid_meand);     

    //Motor 6 y 7 generan meandros (3 revoluciones) y 9 se mueve solidareamente
    while (true) {
      if (digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || !digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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
    Serial.print(Paso);
    
    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-pasos_LS);
    //Motor 9 se mueve hasta limit switch derecho para terminar central loop
    while (!digitalRead(LS_der)) {
      stepper9.run();
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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

  if (Paso == 8) { 
    Serial.print(Paso);

    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper6.setCurrentPosition(0);
    stepper7.setCurrentPosition(0);
    stepper8.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);
    stepper5.setCurrentPosition(0);
    
    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel_gancho);
    stepper2.moveTo(-steps_gancho_corte_der);    //Dirección de cierre (-)
   
    stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho
    stepper4.setAcceleration(acel);
    stepper4.moveTo(-pasos_LS);     //(-) --> funcionamiento de calibración (sentido contrario a funcionamiento normal) 

    stepper5.setMaxSpeed(vel_enrol);  //enrollador izquierdo
    stepper5.setAcceleration(acel);
    stepper5.moveTo(pasos_LS);    //(+) --> funcionamiento de calibración (sentido contrario a funcionamiento normal)
    
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel_gancho);
    stepper3.moveTo(-steps_gancho_corte_izq);    //Dirección de cierre (-)

    stepper6.setMaxSpeed(vel_meand);  //meandros derecho
    stepper6.setAcceleration(acel);
    stepper6.moveTo(-steps_meand_fin);   //Dirección de funcionamiento normal (-)

    stepper7.setMaxSpeed(vel_meand);  //meandros derecho
    stepper7.setAcceleration(acel);
    stepper7.moveTo(steps_meand_fin);   //Dirección de funcionamiento normal (+)

    stepper8.setMaxSpeed(vel_meand);
    stepper8.setAcceleration(acel);
    stepper8.moveTo(-pasos_LS);     

    stepper9.setSpeed(vel_meand);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(-steps_meand);    

    //Motor 6 y 7 vuelven a posicion inicial (1 revoluciones)
    while (digitalRead(LS_der) && digitalRead(LS_frnt)) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)) {
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

    //Motor 9 se devuelve una cantidad determinada de pasos
    stepper9.setCurrentPosition(0);
    stepper9.setSpeed(vel_retract_mot9);
    stepper9.moveTo(steps_retract_mot9);

    while (true) {
      stepper9.run();
      if (!stepper9.run()) {
        break;
      }
    }

    //Motor 8 se mueve hacia atrás steps_retract_mot8 pasos
    stepper8.setCurrentPosition(0);
    stepper8.setSpeed(vel_retract_mot8);
    stepper8.moveTo(steps_retract_mot8);

    while (true) {
      stepper8.run();
      if (!stepper8.run()) {
        break;
      }
      if (digitalRead(LS_izq) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der) || digitalRead(LS_tras)) { // || !digitalRead(LS_der)
        warning = 9;
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    
    //Gancho derecho a posicion inicial (horizontal en cero)
    while (!digitalRead(LS_niv_der)) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)  || digitalRead(LS_cort_izq) ||
      digitalRead(LS_cort_der)) {
        warning = 9;
        break;
      }
      stepper4.run();
      if (!stepper4.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    
    //Gancho izquierdo a posicion inicial (horizontal en cero)
    while (!digitalRead(LS_niv_izq)) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)  || digitalRead(LS_cort_izq) 
      || digitalRead(LS_cort_der)) {
        warning = 9;
        break;
      }
      stepper5.run();
      if (!stepper5.run()) {
        break;
      }
    }
    if (warning != 0) {
      exit(0);
    }
    
    //Se cierran ganchos
    while (true) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)
      || !digitalRead(LS_niv_der) || !digitalRead(LS_niv_izq)) { //|| !digitalRead(LS_der)
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
    stepper2.moveTo(steps_gancho_der + steps_gancho_corte_der);

    stepper3.setCurrentPosition(0);
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(steps_gancho_der + steps_gancho_corte_izq);

    while (true) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der)
       || !digitalRead(LS_niv_der) || !digitalRead(LS_niv_izq)){ // || !digitalRead(LS_der)
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


  }//paso 8

  /**************
   *** PASO 9 ***
   **************/

  if (Paso == 9) {
    Serial.print(Paso);
    
    //Ganchos a posicion inicial
    stepper2.setCurrentPosition(0);
    stepper2.setMaxSpeed(vel_gancho);
    stepper2.setAcceleration(acel);
    stepper2.moveTo(-steps_gancho_der); 

    stepper3.setCurrentPosition(0);
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(-steps_gancho_der); 

    // se cierran ganchos
    while (true) {
      if (digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || digitalRead(LS_cort_izq) || digitalRead(LS_cort_der) || 
      digitalRead(LS_der) || !digitalRead(LS_niv_der) || !digitalRead(LS_niv_izq)){
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
