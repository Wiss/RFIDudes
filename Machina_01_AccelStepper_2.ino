// programa para hacer funcionar la machina
// V0.0 : Se definen los motores y la forma de acceder a cada uno
// V0.1 : se define el funcionamiento de los motores teniendo en cuenta la cantidad de pasos y una frecuencia de funcionamiento con función movmot
// V0.2 : Se utiliza librería AccelStepper para controlar motores. 
//         ->Problema con motor 8 y limit switch en último paso

// Disponibles para limit switch pines 22 al 31

#define LS_frnt 22 
#define LS_izq  23
#define LS_der  24
#define LS_tras 25

#include <AccelStepper.h>

int warning = 0;

long pasos_totales = 6;

int steps_retract = -40;
int steps_gancho  = 800;        // 0.75 mm/vuelta
int steps_gancho_corte = 132;   // 0.75 mm/vuelta. 2/3*200
int steps_cable = 300/8*200;    // 8 mm/vuelta. 
int steps_enrol = 200;          // una vuelta
int steps_meand = 600;          // tres vueltas
int steps_solid_enrol = steps_enrol*32/4; //steps_enrol*31.5/4; //Por cada vuelta del motor 4 y 5 se avanzan 31.5*2 mm. Entonces, el mot9 necesita 31.5*2/8 vueltas (8 mm por vuelta mot 9)
int steps_solid_meand = steps_enrol*10/2; //steps_meand*9.56/2;    //Por cada vuelta del motor 6 y 7 se avanzan ((15.56-6)*2)*2 mm. Entonces, el mot9 necesita 9.56*2*2/8 vueltas (8 mm por vuelta mot 9)


long  vel         = 1000; 
long  vel_cable   = vel;
long  vel_retract = vel/2; 
long  vel_gancho  = vel;
//long  vel_enrol   = vel/13;
float vel_solid_meand = vel;
float vel_solid_enrol = vel;     //steps_enrol*vel_enrol = steps_solid_enrol*vel_solid_enrol
float vel_enrol = vel_solid_enrol*steps_enrol/steps_solid_enrol;
float vel_meand = vel_solid_meand*steps_meand/steps_solid_meand;

int acel = vel;                 //Aceleración igual para todos los motores
long pasos_LS = 1000000;          //Número de pasos grande para while(!LS)

// pin enable
const int enable  = 30;
long ultimaVentana;
long lastmillis;


//motor 1, 1.7A
AccelStepper stepper1(1, 52, 53); // (Simple_MODE, STEP, DIR). Step siempre numero menor

//motor 2, 1.7A
AccelStepper stepper2(1, 50, 51);

//motor 3, 1.7A
AccelStepper stepper3(1, 48, 49);

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  pinMode(enable, OUTPUT);
  digitalWrite(enable, HIGH);

}

void loop() {

//Prueba

for(int i=0; i<=5; i++){
  ejecutar(i);
  delay(200);
}

//ejecutar(3);
delay(10000);
}

void ejecutar(int Paso) {
  
 /**************
 *** PASO 0 ***
 **************/
 
  if (Paso == 0){

//Setear motores
//Dirección deseada de motores se setea con signo dentro de steps en stepper.moveTo

    stepper8.setMaxSpeed(vel);       
    stepper8.setAcceleration(acel);
    stepper8.moveTo(pasos_LS);
    
    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(pasos_LS);  
             
//Motor 8 se mueve hasta limit switch frontal 
    
    while (!digitalRead(LS_frnt)){ 
      stepper8.run();      
      //delay(2);
      if(digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras)){
        Serial.println("error 88 break LS_frnt loop");
        warning = 88;
        break;
      }
    }

    Serial.println("Paso 0: salida primer while");
    delay(200);
    
    if(warning != 0){
      //Imprimir número de warning en serial o pantalla
      Serial.println("Error 88 break main loop");
      delay(200);
      exit(0);
    }
    
//Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch

    stepper8.setCurrentPosition(0);
    stepper8.setSpeed(vel_retract);
    stepper8.moveTo(steps_retract);
    while(true){
      //Serial.println("Paso 0: retract loop");
      //Serial.println(stepper8.currentPosition());
      stepper8.run();
      //delay(2);
      if(!stepper8.run()){
        Serial.println("Paso 0: Salida retract loop");
        break;  
      }
    }
    //Serial.println("PRUEBA3");    
//Motor 9 se mueve hasta limit switch izquierdo
    while (!digitalRead(LS_izq)){
      //Serial.println("Paso 0: Loop a LS izquierdo");
      stepper9.run();
      //delay(2);
      if(digitalRead(LS_der) || digitalRead(LS_tras) || digitalRead(LS_frnt)){
        warning = 88;
        break;
      }
    }
    if(warning != 0){
      Serial.println("error 88 break LS_izq loop");
      exit(0);
    }
//Motor 9 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    stepper9.setCurrentPosition(0);
    stepper9.setSpeed(vel_retract);
    stepper9.moveTo(steps_retract);
    while(true){
      //Serial.println("Paso 0: retract loop2");
      stepper9.run();
      //delay(2);
      if(!stepper9.run()){
        Serial.println("Paso 0: Salida retract loop2");
        break;  
      }
    }

  }//paso 0
  
/**************
 *** PASO 1 ***
 **************/
 
  if (Paso == 1){

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
    stepper2.moveTo(steps_gancho);    //CAMBIAR 
    
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(steps_gancho);    //CAMBIAR              
             
//Motor 8 se mueve hasta limit switch trasero    

    while (!digitalRead(LS_tras)){
      stepper8.run();
      //delay(2);
      if(digitalRead(LS_der) || digitalRead(LS_frnt) || digitalRead(LS_izq)){
        Serial.println("LS activated");
        warning = 1;
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }
    
//Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch

    stepper8.setCurrentPosition(0);
    stepper8.setSpeed(vel_retract);
    stepper8.moveTo(-steps_retract);

    while(true){
      stepper8.run();
      //delay(2);
      if(!stepper8.run()){
        Serial.println("Paso 0: Salida retract loop");
        break;  
      }
    }
  
//Motor 9 se mueve hasta limit switch derecho
    while (!digitalRead(LS_der)){
      stepper9.run();
      //delay(2);
      if(digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)){
        warning = 1;
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }
//Motor 9 se devuelve una cantidad determinada de pasos para no tocar el limit switch
 
    stepper9.setCurrentPosition(0);
    stepper9.setSpeed(vel_retract);
    stepper9.moveTo(-steps_retract);

    while(true){
      stepper9.run();
      //delay(2);
      if(!stepper9.run()){
        break;  
      }
    }
    
//Se abren ganchos

    while(true){
    if(digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)){
      warning = 1;
      break;
    }
    stepper2.run();
    delay(2);
    stepper3.run();
      if(!stepper2.run() && !stepper3.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

  }//paso 1

/**************
 *** PASO 2 ***
 **************/
 
  if (Paso == 2){
    
    stepper9.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    
    stepper9.setMaxSpeed(vel);
    stepper9.setAcceleration(acel);
    stepper9.moveTo(steps_cable);       //Dirección contraria a paso 0 
    
    stepper2.setMaxSpeed(vel_gancho);       
    stepper2.setAcceleration(acel);
    stepper2.moveTo(-steps_gancho);    //Dirección contraria a paso 0 (cerrar)
    
    stepper3.setMaxSpeed(vel_gancho);
    stepper3.setAcceleration(acel);
    stepper3.moveTo(-steps_gancho);    //Dirección contraria a paso 0    
    
//Cerrar gancho izquierdo para agarrar cable    
    delay(1000);
    while(true){
    if(digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)){
      warning = 2;
      break;
    }
    stepper3.run();
      if(!stepper3.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }
//Estirar cable hasta la izquierda

    while(true){
    if(digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)){
      warning = 2;
      break;
    }
    stepper9.run();
      if(!stepper9.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }
//Cerrar gancho derecho para cortar - agarrar cable

    while(true){
    if(digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt)){
      warning = 2;
      break;
    }
    stepper2.run();
      if(!stepper2.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

//Desactivar motores  
    
  }//paso 2


 /**************
 *** PASO 3 ***
 **************/
 
  if (Paso == 3){

    stepper4.setCurrentPosition(0);
    stepper5.setCurrentPosition(0);
    stepper8.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);
    
    stepper4.setMaxSpeed(vel_enrol);  //enrollador derecho
    stepper4.setAcceleration(acel);
    stepper4.moveTo(steps_enrol);     //giro horario
    
    stepper5.setMaxSpeed(vel_enrol);  //enrollador izquierdo
    stepper5.setAcceleration(acel);
    stepper5.moveTo(-steps_enrol);    //giro anti horario

    stepper8.setMaxSpeed(vel);       
    stepper8.setAcceleration(acel);
    stepper8.moveTo(pasos_LS);   

    stepper9.setAcceleration(acel);
    stepper9.setMaxSpeed(vel_solid_enrol);  //setMaxSpeed? REVISAR
    stepper9.moveTo(steps_solid_enrol);  //REVISAR SIGNO PARA DIRECCIÓN CORRECTA
    
//Motor 8 se mueve hasta limit switch frontal 
    while (!digitalRead(LS_frnt)){ 
      stepper8.run();
      //delay(2);
      if(digitalRead(LS_izq) || digitalRead(LS_der) || digitalRead(LS_tras)){
        warning = 3;
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

//Motor 4 y 5 enrollan 360 y 9 se mueve solidareamente

    while(true){
    if(digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || !digitalRead(LS_frnt)){
      warning = 3;
      break;
    }
    stepper4.run();
    stepper5.run();
    stepper9.run();
      if(!stepper4.run() && !stepper5.run() && !stepper9.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

//Desactivar motores  


  }//paso 3


 /**************
 *** PASO 4 ***
 **************/
 
  if (Paso == 4){
    
    stepper6.setCurrentPosition(0);
    stepper7.setCurrentPosition(0);
    stepper9.setCurrentPosition(0);

    stepper6.setMaxSpeed(vel_meand);  //meandros derecho
    stepper6.setAcceleration(acel);
    stepper6.moveTo(-steps_meand);     //REVISAR SENTIDO
    
    stepper7.setMaxSpeed(vel_meand);  //meandros izquierdo
    stepper7.setAcceleration(acel);
    stepper7.moveTo(steps_meand);     //REVISAR SENTIDO

      
    stepper9.setAcceleration(acel);
    stepper9.setMaxSpeed(vel_solid_meand);     //setSpeed? REVISAR
    stepper9.moveTo(steps_solid_meand);     //REVISAR SIGNO PARA DIRECCIÓN CORRECTA
        
 //Motor 6 y 7 generan meandros (3 revoluciones) y 9 se mueve solidareamente
    while(true){
    if(digitalRead(LS_der) || digitalRead(LS_izq) || digitalRead(LS_tras) || !digitalRead(LS_frnt)){
      warning = 4;
      Serial.println("warning 4");
      break;
    }
    stepper6.run();
    stepper7.run();
    stepper9.run();
      if(!stepper6.run() && !stepper7.run() && !stepper9.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

 //Motor 9 se mueve hasta limit switch derecho para terminar central loop 
    stepper9.moveTo(pasos_LS);     //REVISAR SENTIDO
    stepper9.setMaxSpeed(vel/2);
    while (!digitalRead(LS_der)){ 
       Serial.println("Fin paso 4 nouuu");
      stepper9.run();
      if(digitalRead(LS_izq) || digitalRead(LS_tras)){
         Serial.println("nottFin paso 4");
        warning = 4;
        break;
      }
    }
    if(warning != 0){
       Serial.println("noFin paso 4");
      exit(0);
    }

    Serial.println("Fin paso 4");

}//paso 4

 /**************
 *** PASO 5 ***
 **************/
 
  if (Paso == 55){
;
//Soldadura


}

 /**************
 *** PASO 6 ***
 **************/
 
  if (Paso == 5){

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
    stepper6.moveTo(steps_meand/3);     //REVISAR SENTIDO

    stepper7.setMaxSpeed(vel_meand);  //meandros derecho
    stepper7.setAcceleration(acel);
    stepper7.moveTo(-steps_meand/3);     //REVISAR SENTIDO

    stepper8.setMaxSpeed(vel_meand);  
    stepper8.setAcceleration(acel);
    stepper8.moveTo(-pasos_LS);     //REVISAR SENTIDO

    stepper9.setSpeed(vel_meand);  
    stepper9.setAcceleration(acel);
    stepper9.moveTo(steps_meand);     //REVISAR SENTIDO
    
    //Motor 6 y 7 vuelven a posicion inicial (1 revoluciones)
    while(digitalRead(LS_der) && digitalRead(LS_frnt)){
    if(digitalRead(LS_izq) || digitalRead(LS_tras)){
      warning = 6;
      break;
    }
    stepper6.run();
    stepper7.run();
      if(!stepper6.run() && !stepper7.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

    //Motor 8 se mueve hasta limit switch trasero    
    while (!digitalRead(LS_tras)){
      stepper8.run();
      if(digitalRead(LS_izq) || !digitalRead(LS_der)){
        warning = 6;
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }
    
    //Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    
    stepper8.setCurrentPosition(0);
    stepper8.setMaxSpeed(vel_retract); //SetSpeed?
    stepper8.moveTo(steps_retract);

    while(true){
      Serial.println("loop retract");
    stepper8.run();
    if(!stepper8.run()){
      break;  
    }
    }
    
//Se cierran ganchos

    while(true){
    if(digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || !digitalRead(LS_der)){
      warning = 6;
      break;
    }
    stepper2.run();
    stepper3.run();
      if(!stepper2.run() &&  !stepper3.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

//Se abre ganchos

    stepper2.setCurrentPosition(0);
    stepper2.moveTo(steps_gancho+steps_gancho_corte);
    
    stepper3.setCurrentPosition(0);
    stepper3.moveTo(steps_gancho+steps_gancho_corte);

    while(true){
    if(digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || !digitalRead(LS_der)){
      warning = 6;
      break;
    }
    stepper2.run();
    stepper3.run();  
      if(!stepper2.run() && !stepper3.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }
//Ganchos a posicion inicial

    stepper2.moveTo(steps_gancho); //REVISAR SENTIDOS
    stepper3.moveTo(steps_gancho); //REVISAR SENTIDOS
    while(true){
    if(digitalRead(LS_izq) || digitalRead(LS_tras) || digitalRead(LS_frnt) || !digitalRead(LS_der)){
      warning = 6;
      break;
    }
    stepper2.run();
    stepper3.run();
      if(!stepper2.run() && !stepper3.run()){
        break;
      }
    }
    if(warning != 0){
      exit(0);
    }

//Soltar LS_der
stepper9.moveTo(steps_retract*100);
while(digitalRead(LS_der)){
  stepper9.run();
}

}//paso 6



}//ejecutar
