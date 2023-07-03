//Paquete para la comunicación bluetooth
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run 'make menuconfig' to and enable it
#endif

BluetoothSerial SerialBT;

//VARIABLES/PINES ULTRASONIDO 
//Configuramos los pines del sensor Trigger y Echo
const int PinTrig = 7;
const int PinEcho = 6;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;

// Número de muestras
const int numLecturas = 100;

// Distancia a los 100 ml y vacío
const float distancia100 = 2.15;
const float distanciaVacio = 11.41;

float lecturas[numLecturas]; // Array para almacenar lecturas
int lecturaActual = 0; // Lectura por la que vamos
float total = 0; // Total de las que llevamos
float media = 0; // Media de las medidas
bool primeraMedia = false; // Para saber que ya hemos calculado por lo menos una

/////////////VARIABLES/PINES/FUNCIONES FLUJO
volatile int NumPulsos; //variable para la cantidad de pulsos recibidos
int PinSensor = 2;    //Sensor conectado en el pin 2
float factor_conversion=7.5; //para convertir de frecuencia a caudal

/////////////VARIABLES PARA ENVIAR INFORMACIÓN A LA DB
float NivelBT;  //Variable para guardad el nivel del río que será enviada a la app
float CaudalBT; //Variable para guardar el caudal del río que será enviada a la app
//NOTA: Para el prototipo definir un delay que lea y envíe los datos cada 2s

//---Función que se ejecuta en interrupción---------------
void ContarPulsos ()
{ 
  NumPulsos++;  //incrementamos la variable de pulsos
} 

//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuencia() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Desabilitamos las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  //Iniciamos el módulo bluetooth de la ESP32
  SerialBT.begin("AquaSentinelDevice");
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);
  // Ponemos el pin Trig en modo salida
  pinMode(PinTrig, OUTPUT);
  // Ponemos el pin Echo en modo entrada
  pinMode(PinEcho, INPUT);

  // Inicializamos el array
  for (int i = 0; i < numLecturas; i++)
  {
    lecturas[i] = 0;
  }

//PARA FLUJO
  pinMode(PinSensor, INPUT); 
  attachInterrupt(0,ContarPulsos,RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
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
    float distanciaLleno = distanciaVacio - media;
    float cantidadLiquido = distanciaLleno * 100 / distancia100;

    Serial.print(media);
    Serial.println(" cm");
    
    Serial.print(cantidadLiquido);
    Serial.println(" ml");
  }

  delay(2000);

  /////////////FLUJO
  
  float frecuencia=ObtenerFrecuencia(); //obtenemos la Frecuencia de los pulsos en Hz
  float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  float caudal_L_h=caudal_L_m*60; //calculamos el caudal en L/h

  //-----Enviamos por el puerto serie---------------
  Serial.print ("FrecuenciaPulsos: "); 
  Serial.print (frecuencia,0); 
  Serial.print ("Hz\tCaudal: "); 
  Serial.print (caudal_L_m,3); 
  Serial.print (" L/m\t"); 
  Serial.print (caudal_L_h,3); 
  Serial.println ("L/h");

  //-----Enviamos por el puerto serie---------------
  NivelBT=media/100;  //Nivel enviado a la aplicación
  CaudalBT=(caudal_L_m,3); //Caudal enviado a la aplicación
  SerialBT.print(NivelBT);
  SerialBT.print(";");
  SerialBT.print(CaudalBT);
  SerialBT.print(";");

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Método que inicia la secuencia del Trigger para comenzar a medir ULTRASONIDO
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
