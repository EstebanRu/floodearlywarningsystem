// Configuramos los pines del sensor Trigger y Echo
const int PinTrig = 22;
const int PinEcho = 23;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;
const int numLecturas = 20;
const float distanciaVacio = 19.79;

float lecturas[numLecturas]; // Array para almacenar lecturas
int lecturaActual = 0;       // Lectura por la que vamos
float total = 0;             // Total de las que llevamos
float media = 0;             // Media de las medidas
bool primeraMedia = false;   // Para saber que ya hemos calculado por lo menos una

volatile int NumPulsos;      //variable para la cantidad de pulsos recibidos
int PinSensor = 2;           //Sensor conectado en el pin 2
float factor_conversion=7.5; //para convertir de frecuencia a caudal

unsigned long tiempoInicio;
const unsigned long intervalo = 500;

unsigned long tiempoInicio2;
const unsigned long intervalo2 = 1000;

void setup()
{
   tiempoInicio = millis();
   tiempoInicio2 = millis();
  Serial.begin(9600);
  pinMode(PinTrig, OUTPUT);
  pinMode(PinEcho, INPUT);

  // Inicializamos el array todo en 0
  for (int i = 0; i < numLecturas; i++) { lecturas[i] = 0; }
 pinMode(PinSensor, INPUT); 
 attachInterrupt(0,ContarPulsos,RISING); //(Interrupcion 0(Pin2),funcion,Flanco de subida)
  
}//setup

void loop()
{
  ObtenerFrecuencia();
  obtenerNivel();
}//loop

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



//---Función que se ejecuta en interrupción---------------
void ContarPulsos ()
{ 
  NumPulsos++;  //incrementamos la variable de pulsos
}


//---Función para obtener frecuencia de los pulsos--------
void ObtenerFrecuencia() 
{
  int frecuencia;
  NumPulsos = 0;        //Ponemos a 0 el número de pulsos
  
  interrupts();         //Habilitamos las interrupciones
  if (millis() - tiempoInicio2 >= intervalo2){
  noInterrupts();       //Desabilitamos las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
  

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
  
  tiempoInicio2 = millis();
  }
}

void obtenerNivel()
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
        float distanciaLleno = distanciaVacio - media;
    
        Serial.print(distanciaLleno);
        Serial.println(" cm");
        
        primeraMedia = false;
      }
        tiempoInicio = millis();
  } 
}
