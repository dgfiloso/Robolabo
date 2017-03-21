#include <DueTimer.h>

const uint32_t CPRD = 50*84;     //  Frecuencia 20kHz -> Periodo 50 us -> Usando MCK este campo es: T*MCK = 50 us * 84 MHz

const uint32_t channel_0 = 0;   // PWM channel 0
uint16_t duty_cycle_0 = 0;       //  De 0 a CPRD
const uint32_t pwmPin35 = 35; // PWM output pin

const uint32_t channel_1 = 1;   // PWM Channel 1
uint16_t duty_cycle_1 = 0;       //  De 0 a CPRD
const uint32_t pwmPin37 = 37; // PWM output pin
  
const byte encPin1 = 3; // Encoder input pin 1
const byte encPin2 = 7; // Encoder input pin 2
volatile int pulsos = 0;
volatile int sentido = true;

//  Tal y como esta hecho, solo se pueden usar pines del periférico B
void setupPWM(uint32_t channel, uint16_t duty, uint32_t pwmPin){
  
  uint32_t clka = 0;    //  No se usa
  uint32_t clkb = 0;    //  No se usa
  uint32_t mck = 0;     //  Master clock sin prescalado
  //  Explicado en la página 1044 del datasheet
  uint32_t prescaler = 0;
  uint32_t alignment = 0;   //  0 izquierda, 1 centro
  uint32_t polarity = 0;    //  0 comienza en nivel bajo, 1 comienza en nivel alto

  uint16_t timeH = 0;
  uint16_t timeL = 0;

  //  Habilitamos el periférico de PWM
  pmc_enable_periph_clk(ID_PWM);    //  ID del PWM, se encuentra en ../system/CMSIS/Device/ATMEL/Include/sam3x8e.h

  //  La constante PWM es la dirección base del PWM que se encuentra definida en ../system/CMSIS/Device/ATMEL/Include/sam3x8e.h
  PWMC_ConfigureClocks(clka, clkb, mck ) ;
  PWMC_ConfigureChannel(  PWM,  channel,  prescaler,  alignment,  polarity ) ;
  PWMC_SetPeriod(  PWM,  channel, CPRD ) ; 
  PWMC_SetDutyCycle(  PWM,  channel,  duty ) ;
  PWMC_SetDeadTime(  PWM,  channel,  timeH,  timeL ) ;

//  PWMC_EnableChannel(  PWM,  channel ) ;

  //  Configuramos el puerto del PWM
  PIO_Configure(
    g_APinDescription[pwmPin].pPort,
    PIO_PERIPH_B,
    g_APinDescription[pwmPin].ulPin,
    g_APinDescription[pwmPin].ulPinConfiguration);
}

void readEncoder () {
  pulsos++;
}

int setupEncInt(int encPin1, int encPin2) {
  pinMode(encPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(encPin1), readEncoder, CHANGE);

  pinMode(encPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(encPin2), readEncoder, CHANGE);
}

int setVoltage(double v, uint32_t channel) {
  if (v >= 0 && v <= 9) {
    uint16_t duty;
    duty = (uint16_t)((v * (double)CPRD)/9);
    PWMC_SetDutyCycle(  PWM,  channel,  duty ) ;
    PWMC_EnableChannel(  PWM,  channel ) ;
    return 0;
  } else {
    return 1;
  }
}

void setPosition(double degree) {
  int direc = 0;
  int numPulses;
  
  if (degree < 0) {
    direc = 1;
  }
  
  numPulses = (int)((degree*(double)1800)/360);
  Serial.print("numPulses");
  Serial.println(numPulses);
 if ( setVoltage(!direc,channel_0) ) {
  Serial.println("#Error: Voltaje fuera de rango [0,9]");
 }
 if ( setVoltage(direc,channel_1) ) {
  Serial.println("#Error: Voltaje fuera de rango [0,9]");
 }
 while(pulsos < numPulses){
  
 }
 if ( setVoltage(0,channel_0) ) {
  Serial.println("#Error: Voltaje fuera de rango [0,9]");
 }
 if ( setVoltage(0,channel_1) ) {
  Serial.println("#Error: Voltaje fuera de rango [0,9]");
 }
}

void setup() {

  Serial.begin(9600);
  setupPWM(channel_0, duty_cycle_0, pwmPin35);
  setupPWM(channel_1, duty_cycle_1, pwmPin37);

  setupEncInt(encPin1, encPin2);

    setPosition(90);
    
//  Timer3.attachInterrupt(readDecoder).setPeriod(1000);    //  Cada 1000 us se lanza la función readDecoder()
//   Timer3.start();
}

void loop() {


//  if ( setVoltage(0,channel_0) ) {
//    Serial.println("#Error: Voltaje fuera de rango [0,9]");
//  }
//  if ( setVoltage(4,channel_1) ) {
//    Serial.println("#Error: Voltaje fuera de rango [0,9]");
//  }
//
//  delay(5000);
//
//    if (sentido) {
//      if ( setVoltage(1,channel_0) ) {
//        Serial.println("#Error: Voltaje fuera de rango [0,9]");
//      }
//      if ( setVoltage(0,channel_1) ) {
//        Serial.println("#Error: Voltaje fuera de rango [0,9]");
//      }
//    } else {
//      if ( setVoltage(0,channel_0) ) {
//        Serial.println("#Error: Voltaje fuera de rango [0,9]");
//      }
//      if ( setVoltage(1,channel_1) ) {
//        Serial.println("#Error: Voltaje fuera de rango [0,9]");
//      }
//    }
//  
//  Serial.print(pulsos);
//  Serial.print(" ");
//  Serial.println(sentido);
  
//
//  delay(5000);

//  if ( setVoltage(0,channel_0) ) {
//    Serial.println("#Error: Voltaje fuera de rango [0,9]");
//  }
//  if ( setVoltage(0,channel_1) ) {
//    Serial.println("#Error: Voltaje fuera de rango [0,9]");
//  }
//
//  delay(5000);
//  double tension;
//  String sentido;
//  
//  Serial.println("Indique la tension");
//  while(!Serial.available()) {
//    tension = Serial.read();
//  }
//  Serial.println("Indique el sentido");
//  while(!Serial.available()) {
//    sentido = Serial.readString();
//  }
//  
//  if ( sentido == "derecha") {
//    if ( setVoltage(0,channel_0) ) {
//      Serial.println("#Error: Voltaje fuera de rango [0,9]");
//    }
//    if ( setVoltage(tension,channel_1) ) {
//      Serial.println("#Error: Voltaje fuera de rango [0,9]");
//    }
//  } else if ( sentido == "izquierda" ) {
//    if ( setVoltage(tension,channel_0) ) {
//      Serial.println("#Error: Voltaje fuera de rango [0,9]");
//    }
//    if ( setVoltage(0,channel_1) ) {
//      Serial.println("#Error: Voltaje fuera de rango [0,9]");
//    }
//  } else {
//    Serial.println("#Error: Sentido no definido, solo derecha o izquierda");
//  }
//
//  tension = 0;
//  sentido = "";
}


