bool mov1Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont1 < nSteps && mot1) {
    if (millis() - ultiVent1 < periodon ) {
      digitalWrite(step1, HIGH);
      return false;
    }
    else if (millis() - ultiVent1 <  periodo) {
      digitalWrite(step1, LOW);
      return false;
    }
    else {
      ultiVent1 = millis();
      cont1++;
      return false;
    }
  }
  else {
    cont1 = 0;
    mot1 = false;
    return true;
  }
}

bool mov2Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont2 < nSteps && mot2) {
    if (millis() - ultiVent2 < periodon ) {
      digitalWrite(step2, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent2 <  periodo) {
      digitalWrite(step2, LOW);
      return false;
    }
    else {
      ultiVent2 = millis();
      cont2++;
      return false;
    }
  }
  else {
    cont2 = 0;
    mot2 = false;
    return true;
  }
}

bool mov3Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont3 < nSteps && mot3) {
    if (millis() - ultiVent3 < periodon ) {
      digitalWrite(step3, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent3 <  periodo) {
      digitalWrite(step3, LOW);
      return false;
    }
    else {
      ultiVent3 = millis();
      cont3++;
      return false;
    }
  }
  else {
    cont3 = 0;
    mot3 = false;
    return true;
  }
}

bool mov4Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont4 < nSteps && mot4) {
    if (millis() - ultiVent4 < periodon ) {
      digitalWrite(step4, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent4 <  periodo) {
      digitalWrite(step4, LOW);
      return false;
    }
    else {
      ultiVent4 = millis();
      cont4++;
      return false;
    }
  }
  else {
    cont4 = 0;
    mot4 = false;
    return true;
  }
}

bool mov5Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont5 < nSteps && mot5) {
    if (millis() - ultiVent5 < periodon ) {
      digitalWrite(step5, HIGH);
      return false;
    }
    else if (millis() - ultiVent5 <  periodo) {
      digitalWrite(step5, LOW);
      return false;
    }
    else {
      ultiVent5 = millis();
      cont5++;
      return false;
    }
  }
  else {
    cont5 = 0;
    mot5 = false;
    return true;
  }
}

bool mov6Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont6 < nSteps && mot6) {
    if (millis() - ultiVent6 < periodon ) {
      digitalWrite(step6, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent6 <  periodo) {
      digitalWrite(step6, LOW);
      return false;
    }
    else {
      ultiVent6 = millis();
      cont6++;
      return false;
    }
  }
  else {
    cont6 = 0;
    mot6 = false;
    return true;
  }
}

bool mov7Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont7 < nSteps && mot7) {
    if (millis() - ultiVent7 < periodon ) {
      digitalWrite(step7, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent7 <  periodo) {
      digitalWrite(step7, LOW);
      return false;
    }
    else {
      ultiVent7 = millis();
      cont7++;
      return false;
    }
  }
  else {
    cont7 = 0;
    mot7 = false;
    return true;
  }
}

bool mov8Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont8 < nSteps && mot8) {
    if (millis() - ultiVent8 < periodon ) {
      digitalWrite(step8, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent8 <  periodo) {
      digitalWrite(step8, LOW);
      return false;
    }
    else {
      ultiVent8 = millis();
      cont8++;
      return false;
    }
  }
  else {
    cont8 = 0;
    mot8 = false;
    return true;
  }
}


bool mov9Mot(int nSteps , long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (cont9 < nSteps && mot9) {
    if (millis() - ultiVent9 < periodon ) {
      digitalWrite(step9, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent9 <  periodo) {
      digitalWrite(step9, LOW);
      return false;
    }
    else {
      ultiVent9 = millis();
      cont9++;
      return false;
    }
  }
  else {
    cont9 = 0;
    mot9 = false;
    return true;
  }
}

bool mov1Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if ( mot1) {
    if (millis() - ultiVent1 < periodon ) {
      digitalWrite(step1, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent1 <  periodo) {
      digitalWrite(step1, LOW);
      return false;
    }
    else {
      ultiVent1 = millis();
   
      return false;
    }
  }
  
}

bool mov2Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot2) {
    if (millis() - ultiVent2 < periodon ) {
      digitalWrite(step2, HIGH);
      return false;
    }
    else if (millis() - ultiVent2 <  periodo) {
      digitalWrite(step2, LOW);
      return false;
    }
    else {
      ultiVent2 = millis();
 
      return false;
    }
  }
}

bool mov3Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot3) {
    if (millis() - ultiVent3 < periodon ) {
      digitalWrite(step3, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent3 <  periodo) {
      digitalWrite(step3, LOW);
      return false;
    }
    else {
      ultiVent3 = millis();
  
      return false;
    }
  }
  
}

bool mov4Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot4) {
    if (millis() - ultiVent4 < periodon ) {
      digitalWrite(step4, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent4 <  periodo) {
      digitalWrite(step4, LOW);
      return false;
    }
    else {
      ultiVent4 = millis();
  
      return false;
    }
  }
  
}

bool mov5Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot5) {
    if (millis() - ultiVent5 < periodon ) {
      digitalWrite(step5, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent5 <  periodo) {
      digitalWrite(step5, LOW);
      return false;
    }
    else {
      ultiVent5 = millis();
  
      return false;
    }
  }
  
}

bool mov6Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot6) {
    if (millis() - ultiVent6 < periodon ) {
      digitalWrite(step6, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent6 <  periodo) {
      digitalWrite(step6, LOW);
      return false;
    }
    else {
      ultiVent6 = millis();
  
      return false;
    }
  }
  
}

bool mov7Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot7) {
    if (millis() - ultiVent7 < periodon ) {
      digitalWrite(step7, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent7 <  periodo) {
      digitalWrite(step7, LOW);
      return false;
    }
    else {
      ultiVent7 = millis();
  
      return false;
    }
  }
  
}

bool mov8Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot8) {
    if (millis() - ultiVent8 < periodon ) {
      digitalWrite(step8, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent8 <  periodo) {
      digitalWrite(step8, LOW);
      return false;
    }
    else {
      ultiVent8 = millis();
  
      return false;
    }
  }
  
}

bool mov9Mot( long periodo) {
  // esta funcion evalua los tiempos y sube y baja las salidas digitales asociadas al motor uno hasta que se cumplen
  // los nSteps, entonces retorna True y reinicializa la variable cont1 (numero de pasos)
  // nota1: se debe dejar de llamar desde afuera dado que al reiniciar el numero de pasos entonces quedaria funcionando hasta el infinito

  if (mot9) {
    if (millis() - ultiVent9 < periodon ) {
      digitalWrite(step9, HIGH);
      return false;
     
    }
    else if (millis() - ultiVent9 <  periodo) {
      digitalWrite(step9, LOW);
      return false;
    }
    else {
      ultiVent9 = millis();
  
      return false;
    }
  }
  
}