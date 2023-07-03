// Configuramos los pines del sensor Trigger y Echo
const int PinTrig = 22;
const int PinEcho = 23;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;

// Número de muestras
const int numLecturas = 20;

// Distancia a los 100 ml y vacío
const float distancia100 = 2.15;
const float distanciaVacio = 19.79;

float lecturas[numLecturas]; // Array para almacenar lecturas
int lecturaActual = 0; // Lectura por la que vamos
float total = 0; // Total de las que llevamos
float media = 0; // Media de las medidas
bool primeraMedia = false; // Para saber que ya hemos calculado por lo menos una


unsigned long tiempoInicio;
const unsigned long intervalo = 500;

void setup()
{
   tiempoInicio = millis();
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
}
void loop()
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
    float cantidadLiquido = distanciaLleno * 100 / distancia100;

    Serial.print(distanciaLleno);
    Serial.println(" cm");
    
    //Serial.print(cantidadLiquido);
    //Serial.println(" ml");
    primeraMedia = false;
  }
    tiempoInicio = millis();
  }
 
}

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
