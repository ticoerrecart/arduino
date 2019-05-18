/*
  Creado: Luis del Valle (ldelvalleh@programarfacil.com)
  https://programarfacil.com
*/
 
// Declaracion de variables globales
float tempC; // Variable para almacenar el valor obtenido del sensor (0 a 1023)
int pinLM35 = 15; // Variable del pin de entrada del sensor (A1)
 
void setup() {
  // Cambiamos referencia de las entradas analógicas
  analogReference(INTERNAL);
  
  // Configuramos el puerto serial a 9600 bps
  Serial.begin(9600);
}
 
void loop() {
  // Con analogRead leemos el sensor, recuerda que es un valor de 0 a 1023
  tempC = analogRead(pinLM35); 
   
  // Calculamos la temperatura con la fórmula
  tempC = (1.1 * tempC * 100.0)/1024.0; 
 
  // Envia el dato al puerto serial
  Serial.print(tempC);
  // Salto de línea
  Serial.print("\n");
  
  // Esperamos un tiempo para repetir el loop
  delay(1000);
}
