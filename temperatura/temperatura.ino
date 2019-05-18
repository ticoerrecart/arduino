/*
  temperatyra y display
*/

//pines para 1er digito
/*int a = 1;
int b = 2;
int c = 3;
int d = 4;
int e = 5;
int f = 6;
int g = 7;*/

//pines para 2do digito
/*int a2 = 8;
int b2 = 9;
int c2 = 10;
int d2 = 11;
int e2 = 12;
int f2 = 13;
int g2 = 14;*/


int pines[2][7] = {{1,2,3,4,5,6,7}, //1er digito ->posicion 0
                   {8,9,10,11,12,13,14} //2do digito ->posicion 1
                  };

//para este display, el cero prende la barra!!
int seven_seg_digits[10][7] = { {0,0,0,0,0,0,1},  // = 0
                                 {1,0,0,1,1,1,1},  // = 1
                                 {0,0,1,0,0,1,0},  // = 2
                                 {0,0,0,0,1,1,0},  // = 3
                                 {1,0,0,1,1,0,0},  // = 4
                                 {0,1,0,0,1,0,0},  // = 5
                                 {0,1,0,0,0,0,0},  // = 6
                                 {0,0,0,1,1,1,1},  // = 7
                                 {0,0,0,0,0,0,0},  // = 8
                                 {0,0,0,1,1,0,0}   // = 9
                                };


//temperatura
float tempC; // Variable para almacenar el valor obtenido del sensor (0 a 1023)
int pinLM35 = 15; // Variable del pin de entrada del sensor (A1)
int tempCOut = 0;

void pintarNumero(int numero){
  if(numero>=10){
    int unidad = numero%10;
    int decena = floor(numero /10);
    for(int j=0;j<7;j++){
      digitalWrite(pines[0][j], seven_seg_digits[decena][j]);
    }

    for(int j=0;j<7;j++){
      digitalWrite(pines[1][j], seven_seg_digits[unidad][j]);
    }
  }else{
   apagarDigito(0);
   for(int j=0;j<7;j++){
    digitalWrite(pines[1][j], seven_seg_digits[numero][j]);
   } 
  }
  
}

void apagarDigito(int digito){
  for(int j=0;j<7;j++){
    digitalWrite(pines[digito][j], 1);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  //ojo que al habilitar el puero serial, el pin Digital D2(2) queda titilando!!
  //Serial.begin(9600);
  //inicializo display
  for(int i=0;i<7;i++){
    pinMode(pines[0][i], OUTPUT);
    pinMode(pines[1][i], OUTPUT);
    digitalWrite(pines[0][i], 1);
    digitalWrite(pines[1][i], 1);
  }

  //// Cambiamos referencia de las entradas analógicas para temperatura
  //analogReference(INTERNAL);
  
  /*pinMode(a, OUTPUT);
  digitalWrite(a, 1);  
  pinMode(b, OUTPUT);
  digitalWrite(b, 1);  
  pinMode(c, OUTPUT);
  digitalWrite(c, 1);
  pinMode(d, OUTPUT);
  digitalWrite(d, 1);
  pinMode(e, OUTPUT);
  digitalWrite(e, 1);
  pinMode(f, OUTPUT);
  digitalWrite(f, 1);
  pinMode(g, OUTPUT);
  digitalWrite(g, 1);*/
  /*pinMode(a2, OUTPUT);
  digitalWrite(a2, 1);  
  pinMode(b2, OUTPUT);
  digitalWrite(b2, 1);  
  pinMode(c2, OUTPUT);
  digitalWrite(c2, 1);
  pinMode(d2, OUTPUT);
  digitalWrite(d2, 1);
  pinMode(e2, OUTPUT);
  digitalWrite(e2, 1);
  pinMode(f2, OUTPUT);
  digitalWrite(f2, 1);
  pinMode(g2, OUTPUT);
  digitalWrite(g2, 1); */
  
}



// the loop function runs over and over again forever
void loop() {
  // Con analogRead leemos el sensor, que es un valor de 0 a 1023
  tempC = analogRead(pinLM35); 
   
  // Calculamos la temperatura con la fórmula
  tempC = ((5.0 * tempC * 100.0)/1024.0)-5;//le resto 5 porque es el error q me da respecto del termometro analog.
  
  if(tempCOut==0 || tempCOut!=round(tempC)){
    tempCOut = round(tempC);
    pintarNumero(tempCOut); 
  }
  
  // Esperamos un tiempo para repetir el loop
  delay(3000);

  //Serial.println(tempC);
//pintarNumero(21);
  /*for (int i=0;i<=99;i++){
      pintarNumero(i);
      delay(500);
  }*/
}
