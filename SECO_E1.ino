uint32_t clka = 0;    //  No se usa
uint32_t clkb = 0;    //  No se usa
uint32_t mck = 0;     //  Master clock sin prescalado
uint32_t pwmPin = 8; // PWM output pin
uint32_t ul_channel = g_APinDescription[pwmPin].ulPWMChannel;   // Set PWM channel
//  Explicado en la página 1044 del datasheet
uint32_t prescaler = clka;
uint32_t alignment = 0;   //  0 izquierda, 1 centro
uint32_t polarity = 0;    //  0 comienza en nivel bajo, 1 comienza en nivel alto
uint32_t period = 100;      //  Frecuencia 20kHz -> Periodo 50 us -> Usando MCK este campo es: T*MCK
uint16_t duty = 50;
uint16_t timeH = 0;
uint16_t timeL = 0;

void setup() {

  //  Configuramos el puerto del PWM
  PIO_Configure(
   g_APinDescription[pwmPin].pPort,
   g_APinDescription[pwmPin].ulPinType,
   g_APinDescription[pwmPin].ulPin,
   g_APinDescription[pwmPin].ulPinConfiguration);
   
  //  Configuramos el PWM
  pmc_enable_periph_clk(ID_PWM);    //  ID del PWM, se encuentra en ../system/CMSIS/Device/ATMEL/Include/sam3x8e.h

  //  La constante PWM es la dirección base del PWM que se encuentra definida en ../system/CMSIS/Device/ATMEL/Include/sam3x8e.h
  PWMC_ConfigureClocks(clka, clkb, mck ) ;
  PWMC_ConfigureChannel(  PWM,  ul_channel,  prescaler,  alignment,  polarity ) ;
  PWMC_SetPeriod(  PWM,  ul_channel, period ) ; 
  PWMC_SetDutyCycle(  PWM,  ul_channel,  duty ) ;
  PWMC_SetDeadTime(  PWM,  ul_channel,  timeH,  timeL ) ;

  PWMC_EnableChannel(  PWM,  ul_channel ) ;



}

void loop() {
  // put your main code here, to run repeatedly:

}
