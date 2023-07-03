//Paquete para la comunicación bluetooth
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
#endif

BluetoothSerial SerialBT;

// Configuramos los pines del sensor Trigger y Echo
const int PinTrig = 22;
const int PinEcho = 23;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;
const int numLecturas = 8;
const float distanciaVacio = 19.79;


float distanciaLleno=0;
float caudal_L_m=0;

  
float lecturas[numLecturas]; // Array para almacenar lecturas
int lecturaActual = 0;       // Lectura por la que vamos
float total = 0;             // Total de las que llevamos
float media = 0;             // Media de las medidas
bool primeraMedia = false;   // Para saber que ya hemos calculado por lo menos una

volatile int NumPulsos;      //variable para la cantidad de pulsos recibidos
int PinSensor = 4;           //Sensor conectado en el pin 2
float factor_conversion=7.5; //para convertir de frecuencia a caudal

unsigned long tiempoInicio;
const unsigned long intervalo = 250;

unsigned long tiempoInicio2;
const unsigned long intervalo2 = 2000;

unsigned long tiempoInicio3;
const unsigned long intervalo3 = 2000;

/////////////VARIABLES PARA ENVIAR INFORMACIÓN A LA DB
float NivelBT;  //Variable para guardad el nivel del río que será enviada a la app
float CaudalBT; //Variable para guardar el caudal del río que será enviada a la app
//NOTA: Para el prototipo definir un delay que lea y envíe los datos cada 2s

void IRAM_ATTR ContarPulsos()
{
  NumPulsos++; // Incrementamos la variable de pulsos
}



void setup()
{
  tiempoInicio=tiempoInicio2=tiempoInicio3 = millis();
  SerialBT.begin("AquaSentinelDevice");
  Serial.begin(115200);
  pinMode(PinTrig, OUTPUT);
  pinMode(PinEcho, INPUT);

  // Inicializamos el array todo en 0
  for (int i = 0; i < numLecturas; i++) 
  { lecturas[i] = 0; } 
 pinMode(PinSensor, INPUT_PULLUP);
 //attachInterrupt(digitalPinToInterrupt(PinSensor), ContarPulsos, RISING); // Asociamos la interrupción al pin correspondiente
 
}//setup
//loop///////////////////////////////////////////////////////
void loop()
{  
  NivelBT=obtenerNivel();  //Nivel enviado a la aplicación
  CaudalBT=0;//ObtenerFrecuencia(); //Caudal enviado a la aplicación
   if (millis() - tiempoInicio3 >= intervalo3) {
    //-----Enviamos por el puerto serie---------------//
  SerialBT.print(NivelBT);
  SerialBT.print(";");
  SerialBT.print(CaudalBT);
  SerialBT.print(";");
      tiempoInicio3 = millis();
    }
  
}
//loop///////////////////////////////////////////////////////

// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(PinTrig, LOW);
  delayMicroseconds(2);

  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(PinTrig, HIGH);
  delayMicroseconds(10);

  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(PinTrig, LOW);
}



//---Función para obtener frecuencia de los pulsos--------
float ObtenerFrecuencia() 
{
  int frecuencia;
         //Ponemos a 0 el número de pulsos
  
  interrupts();         //Habilitamos las interrupciones
  if (millis() - tiempoInicio2 >= intervalo2){
  noInterrupts();       //Desabilitamos las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
   NumPulsos = 0;

   caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  float caudal_L_h=caudal_L_m*60; //calculamos el caudal en L/h

  //-----Enviamos por el puerto serie---------------
  Serial.print ("FrecuenciaPulsos: "); 
  Serial.print (frecuencia,0); 
  Serial.print ("Hz\tCaudal: "); 
  Serial.print (caudal_L_m,3); 
  Serial.print (" L/m\t"); 
  Serial.print (caudal_L_h,3); 
  Serial.println ("L/h");
  
  tiempoInicio2 = millis();
 
  }
   return caudal_L_m;
}


float obtenerNivel()
{
   if (millis() - tiempoInicio >= intervalo) {
      // Eliminamos la última medida
      total = total - lecturas[lecturaActual];
      iniciarTrigger();
      // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
      unsigned long tiempo = pulseIn(PinEcho, HIGH);
      // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
      // por eso se multiplica por 0.000001
      float distancia = tiempo * 0.000001 * VelSon / 2.0;
      // Almacenamos la distancia en el array
      lecturas[lecturaActual] = distancia;
      // Añadimos la lectura al total
      total = total + lecturas[lecturaActual];
    
      // Avanzamos a la siguiente posición del array
      lecturaActual = lecturaActual + 1;
      // Comprobamos si hemos llegado al final del array
      if (lecturaActual >= numLecturas)
      {
        primeraMedia = true;
        lecturaActual = 0;
      }
    
      // Calculamos la media
      media = total / numLecturas;
    
      // Solo mostramos si hemos calculado por lo menos una media
      if (primeraMedia)
      {
         distanciaLleno = distanciaVacio - media;
    
        Serial.print(distanciaLleno);
        Serial.println(" cm");

        primeraMedia = false;
      }   
        tiempoInicio = millis();
  }
  return distanciaLleno;
}
