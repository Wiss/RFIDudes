// programa para hacer funcionar la machina
// V0.0 : Se definen los motores y la forma de acceder a cada uno
//input limit swithc pruea
//const int limit1 = 22;
// V0.1 : se define el funcionamiento de los motores teniendo en cuenta la cantidad de pasos y una frecuencia de funcionamiento

int limit1state = 0;

// Disponibles para limit switch pines 22 al 31

#define LS_izq 22
#define LS_frnt 23 
#define LS_der 24
#define LS_tras 25


bool warning = false;

uint8_t PasosTotales = 6;
uint8_t  Paso;

uint16_t Periodo = 10; 
uint16_t Periodon = Periodo/2;
uint8_t  periodoRetract = Periodo*2;
uint8_t periodoCable;
uint8_t periodo_enrol;
uint8_t periodo_meandl;

uint16_t steps_gancho;
uint16_t steps_gancho_corte;
uint16_t steps_cable;
uint16_t steps_enrol;
uint16_t steps_meand;


// pin enable
const int enable  = 52;
long ultimaVentana;
long lastmillis;


//motor 1
const int step1 = 50;
const int dir1  = 51;
bool mot1 = false;
long cont1;
long ultiVent1;

//motor 2
const int step2;
const int dir2;
bool mot2 = false;
long cont2;
long ultiVent2;

//motor 3
const int step3;
const int dir3;
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

//motor 7
const int step7 ;
const int dir7 ;
bool mot7 = false;
long cont7;
long ultiVent7;

//motor 8
const int step8 = 48;
const int dir8  = 49;
bool mot8 = false;
long cont8;
long ultiVent8;

//motor 9
const int step9 = 46;
const int dir9  = 47;
bool mot9 = false;
long cont9;
long ultiVent9;

void setup() {
  // put your setup code here, to run once:

  pinMode(LS_der, INPUT);
  pinMode(LS_izq, INPUT);
  pinMode(LS_tras, INPUT);
  pinMode(LS_frnt, INPUT);

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

  pinMode(step7, OUTPUT);
  pinMode(dir7, OUTPUT);

  pinMode(step8, OUTPUT);
  pinMode(dir8, OUTPUT);    

  pinMode(step9, OUTPUT);
  pinMode(dir9, OUTPUT);
  
  pinMode(enable, OUTPUT);
  digitalWrite(enable, HIGH);

}

void loop() {
/*
  for(int i = 0 ; i <= PasosTotales ; i++){ 
    if(LS_izq == HIGH || LS_tras == HIGH){
        warning = 9;
        break;
      }
    ejecutar(i);  
  }
*/
  
//Prueba

ejecutar(0);
delay(10000);

  
}

void ejecutar(int Paso) {
  
 /**************
 *** PASO 0 ***
 **************/
 
  if (Paso == 0){

//Inicializar motores
    mot8 = true;
    mot9 = true;


//Seteo dirección deseada de motores involucrados
    digitalWrite(dir8, HIGH); //eje Y
    digitalWrite(dir9, HIGH); //eje X

               
//Motor 8 se mueve hasta limit switch frontal  
    while (digitalRead(LS_frnt)){ 
      mov8Mot(Periodo);
      if(LS_izq == HIGH || LS_der == HIGH || LS_tras== HIGH){
        warning = 88;
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    digitalWrite(dir8, LOW); //dirección opuesta a la definida anteriormente
    //mot8 = true;
    cont8 = 0;
    while(true){
    mov8Mot(10,periodoRetract);
    if(mot8 = false){
      break;  
    }
    }
    
//Motor 9 se mueve hasta limit switch izquierdo
    while (!digitalRead(LS_izq)){
      mov9Mot(Periodo);
      if(LS_der == HIGH || LS_tras == HIGH || LS_frnt== HIGH){
        warning = 88;
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Motor 9 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    digitalWrite(dir9, LOW);
    //mot9 = true;
    cont9 = 0;
    while(true){
      mov9Mot(10,periodoRetract); 
      if(mot9 = false){
        break;  
      }
    }

    

    mot8 = false;
    mot9 = false;
  }
  
/**************
 *** PASO 1 ***
 **************/
 
  if (Paso == 1){

//Inicializar motores
    mot8 = true;
    mot9 = true;
    mot2 = true;
    mot3 = true;
    //mot10 = true;  //Motor soldadura

    
//Seteo dirección deseada de motores involucrados
    digitalWrite(dir8, LOW); //eje Y
    digitalWrite(dir9, LOW); //eje X
    digitalWrite(dir2, LOW); //Gancho derecho
    digitalWrite(dir3, LOW); //Gancho izquierdo
               
//Motor 8 se mueve hasta limit switch trasero    
    while (!digitalRead(LS_tras)){ //&& mov9Mot()
      mov8Mot(periodo8);
      if(LS_izq == HIGH || LS_der == HIGH || LS_frnt== HIGH){
        warning = 1;
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    digitalWrite(dir8, HIGH); //dirección opuesta a la definida anteriormente
    //mot8 = true;
    cont8 = 0;
    while(true){
    mov8Mot(10,periodo_retract);
    if(mot8 = false){
      break;  
    }
    }
    
//Motor 9 se mueve hasta limit switch derecho
    while (!digitalRead(LS_der)){
      mov9Mot(periodo9);
      if(LS_izq == HIGH || LS_tras == HIGH || LS_frnt== HIGH){
        warning = 1;
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Motor 9 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    digitalWrite(dir9, HIGH);
    //mot9 = true;
    cont9 = 0;
    while(true){
      mov9Mot(10,periodo_retract); 
      if(mot9 = false){
        break;  
      }
    }
    //mov8mot(10,periodoRetract);   esta demas(?)
    
//Se abren ganchos

    while(true){
    if(LS_der == HIGH || LS_izq == HIGH || LS_tras == HIGH || LS_frnt== HIGH){
      warning = 1;
      break;
    }
    mov2mot(steps_gancho, periodo2);
    mov3mot(steps_gancho, periodo3);
      if(mot2 == false && mot3 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }
    mot8 = false;
    mot9 = false;
    mot2 = false;
    mot3 = false;
  }

/**************
 *** PASO 2 ***
 **************/
 
  if (Paso == 2){
    mot9 = true;
    mot2 = true;
    mot3 = true;
    
//Cerrar gancho izquierdo para agarrar cable   

    digitalWrite(dir3, HIGH); 
    while(true){
    if(LS_der == HIGH || LS_izq == HIGH || LS_tras == HIGH || LS_frnt== HIGH){
      warning = 2;
      break;
    }
    mov3mot(steps_gancho, periodo3);
      if(mot3 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Estirar cable hasta la izquierda

    while(true){
    if(LS_der == HIGH || LS_izq == HIGH || LS_tras == HIGH || LS_frnt== HIGH){
      warning = 2;
      break;
    }
    mov9Mot(steps_cable, periodoCable);
      if(mot9 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Cerrar gancho derecho para cortar - agarrar cable

    digitalWrite(dir2, HIGH); 
    while(true){
    if(LS_der == HIGH || LS_izq == HIGH || LS_tras == HIGH || LS_frnt== HIGH){
      warning = 2;
      break;
    }
    mov2mot(steps_gancho, periodo2);
      if(mot2 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }

//Desactivar motores  

    mot9 = false;
    mot2 = false;
    mot3 = false;
    
  }//paso 2


 /**************
 *** PASO 3 ***
 **************/
 
  if (Paso == 3){
    mot8 = true;
    mot4 = true;
    mot5 = true;
    //Seteo dirección deseada de motores involucrados
    digitalWrite(dir4, LOW); //Enrollador derecho
    digitalWrite(dir5, LOW); //Enrollador izquierdo

//Motor 8 se mueve hasta limit switch frontal 
    while (!digitalRead(LS_frnt)){ 
      mov8Mot(periodo8);
      if(LS_izq == HIGH || LS_der == HIGH || LS_tras== HIGH){
        warning = 3;
        break;
      }
    }
    if(warning != 0){
      break;
    }

//Motor 4 y 5 enrollan 360 y 9 se mueve solidareamente
    digitalWrite(dir9, LOW); 
    while(true){
    if(LS_der == HIGH || LS_izq == HIGH || LS_tras == HIGH){
      warning = 3;
      break;
    }
    mov4mot(steps_enrol, periodo_enrol);
    mov5mot(steps_enrol, periodo_enrol);
    perido_solid_enrol = periodo_enrol*4/31.5; //steps_enrol*periodo_enrol = steps_solid_enrol*periodo_solid_enrol
    steps_solid_enrol = steps_enrol*31.5/4; //Por cada vuelta del motor 4 y 5 se avanzan 31.5*2 mm. Entonces, el mot9 necesita 31.5*2/8 vueltas (8 mm por vuelta mot 9)
    mov9Mot(steps_solid_enrol, periodo_solid_enrol);
      if(mot4 == false && mot5 == false && mot9 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }

//Desactivar motores  

    mot8 = false;
    mot4 = false;
    mot5 = false;
    
  }//paso 3


 /**************
 *** PASO 4 ***
 **************/
 
  if (Paso == 4){

//Inicializar motores
    mot6 = true;
    mot7 = true;
    mot9 = true;
    
//Seteo dirección deseada de motores involucrados
    digitalWrite(dir6, LOW); //Meandros derecho
    digitalWrite(dir7, LOW); //Meandros izquierdo
    //Motor 6 y 7 generan meandros (3 revoluciones) y 9 se mueve solidareamente
    //digitalWrite(dir9, LOW); //redundante
    while(true){
    if(LS_der == HIGH || LS_izq == HIGH || LS_tras == HIGH){
      warning = 4;
      break;
    }
    mov6mot(steps_meand, periodo_meand);
    mov7mot(steps_meand, periodo_meand);
    perido_solid_meand = (periodo_meand*2/9.56)*1.01; //steps_meand*periodo_meand = steps_solid_meand*periodo_solid_meand. Agregar un factor de seguridad
    steps_solid_meand = steps_meand*9.56/2; //Por cada vuelta del motor 6 y 7 se avanzan ((15.56-6)*2)*2 mm. Entonces, el mot9 necesita 9.56*2*2/8 vueltas (8 mm por vuelta mot 9)
    mov9Mot(steps_solid_meand, periodo_solid_meand);
      if(mot6 == false && mot7 == false && mot9 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }

    //Motor 9 se mueve hasta limit switch derecho para terminar central loop 
    while (!digitalRead(LS_der)){ 
      mov9Mot(periodo9);
      if(LS_izq == HIGH || LS_frnt== HIGH){
        warning = 1;
        break;
      }
    }
    if(warning != 0){
      break;
    }

 //Desactivar motores
    mot6 = false;
    mot7 = false;
    mot9 = false;

}
 /**************
 *** PASO 5 ***
 **************/
 
  if (Paso == 5){

//Inicializar motores


}

 /**************
 *** PASO 6 ***
 **************/
 
  if (Paso == 6){

//Inicializar motores
    mot2 = true;
    mot3 = true;
    mot6 = true;
    mot7 = true;
    mot8 = true;
    mot9 = true;
    
    //Motor 6 y 7 vuelven a posicion inicial (1 revoluciones)
    while(true){
    if(LS_izq == HIGH || LS_tras == HIGH){
      warning = 6;
      break;
    }
    mov6mot(steps_meand/3, periodo_meand);
    mov7mot(steps_meand/3, periodo_meand);
      if(mot6 == false && mot7 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }

    digitalWrite(dir8, LOW); //eje Y
    //Motor 8 se mueve hasta limit switch trasero    
    while (!digitalRead(LS_tras)){ //&& mov9Mot()
      mov8Mot(periodo8);
      if(LS_izq == HIGH || LS_tras == HIGH){
        warning = 6;
        break;
      }
    }
    if(warning != 0){
      break;
    }
    //Motor 8 se devuelve una cantidad determinada de pasos para no tocar el limit switch
    digitalWrite(dir8, HIGH); //dirección opuesta a la definida anteriormente
    //mot8 = true;
    cont8 = 0;
    while(true){
    mov8Mot(10,periodoRetract);
    if(mot8 = false){
      break;  
    }
    }
    

//Se cierran ganchos

    while(true){
    if(LS_izq == HIGH || LS_tras == HIGH){
      warning = 1;
      break;
    }
    mov2mot(steps_gancho_corte, periodo2);
    mov3mot(steps_gancho_corte, periodo3);
      if(mot2 == false && mot3 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }

//Se abre ganchos
    digitalWrite(dir2, LOW); //Gancho derecho
    digitalWrite(dir3, LOW); //Gancho izquierdo
    while(true){
    if(LS_izq == HIGH || LS_tras == HIGH){
      warning = 1;
      break;
    }
    mov2mot(steps_gancho+steps_gancho_corte, periodo2);
    mov3mot(steps_gancho+steps_gancho_corte, periodo3);
      if(mot2 == false && mot3 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }
//Ganchos a posicion inicial
    digitalWrite(dir2, HIGH); //Gancho derecho
    digitalWrite(dir3, HIGH); //Gancho izquierdo
    while(true){
    if(LS_izq == HIGH || LS_tras == HIGH){
      warning = 1;
      break;
    }
    mov2mot(steps_gancho, periodo2);
    mov3mot(steps_gancho, periodo3);
      if(mot2 == false && mot3 == false){
        break;
      }
    }
    if(warning != 0){
      break;
    }

 //Desactivar motores
    mot2 = false;
    mot3 = false;
    mot6 = false;
    mot7 = false;
    mot8 = false;
    mot9 = false;

}//paso 6

}//ejecutar
