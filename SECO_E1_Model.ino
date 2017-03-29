#include <DueTimer.h>

const uint32_t  CPRD            = 50*84;          //  f = 20kHz -> T = 50 us -> T*MCK = 50 us * 84 MHz
const uint32_t  channel_0       = 0;              // PWM channel 0
const uint32_t  channel_1       = 1;              // PWM Channel 1
const uint32_t  pwmPin35        = 35;             // PWM output pin
const uint32_t  pwmPin37        = 37;             // PWM output pin
const EPioType  peripheralId_0  = PIO_PERIPH_B;   //  Peripheral id of PWM pin
const EPioType  peripheralId_1  = PIO_PERIPH_B;   //  Peripheral id of PWM pin

const byte      encPin1         = 3;              // Encoder input pin 1
const byte      encPin2         = 7;              // Encoder input pin 2
volatile int    nPulse          = 0;              //  Number of pulses read

const int       nTotalRep       = 5;              //  Number of repetitions for each voltage value
const int       nTotalSample    = 1200;           //  Number of samples for each repetition
int             nRep            = 0;              //  Variable to save the number of repetition
int             nSample         = 0;              //  Variable to save the number of sample
int             voltModel       = 1;              //  Variable to save PWM Voltage
double          posM[nTotalSample];               //  Array to save the values in each repetition

int             print_result    = false;          //  Variable to indicate to print the results
int             fin             = false;          //  Variable to indicate when the program has finished

/*
 * ************************************************************************************************************
 * -------------------------------------   Hardware PWM Functions   -------------------------------------------
 * ************************************************************************************************************
 */
 /*
 * @function    setupPWM(uint32_t channel, uint16_t duty, uint32_t pwmPin)
 * @param       uint32_t channel        Channel of PWM
 * @param       uint32_t pwmPin         Pin of PWM
 * @param       uint32_t peripheralId   Peripheral of PWM pin
 * @return      void
 * @description Initialization of PWM
 */
void setupPWM(uint32_t channel, uint32_t pwmPin, EPioType peripheralId)
{
  uint32_t clka       = 0;    //  Not used
  uint32_t clkb       = 0;    //  Not used
  uint32_t mck        = 0;    //  Master clock without prescaler
  uint32_t prescaler  = 0;    //  No prescaler
  uint16_t duty       = 0;    //  0%
  uint32_t alignment  = 0;    //  0 left, 1 center
  uint32_t polarity   = 0;    //  0 low level at beginning, 1 high level at beginning
  uint16_t timeH      = 0;
  uint16_t timeL      = 0;

  //  Enable PWM peripheral
  pmc_enable_periph_clk(ID_PWM);    //  PWM ID Register (ID_PWM) -> ../system/CMSIS/Device/ATMEL/Include/sam3x8e.h

  //  Configure PWM
  //  PWM Base address (PWM) -> ../system/CMSIS/Device/ATMEL/Include/sam3x8e.h
  PWMC_ConfigureClocks(clka, clkb, mck ) ;
  PWMC_ConfigureChannel(  PWM,  channel,  prescaler,  alignment,  polarity ) ;
  PWMC_SetPeriod(  PWM,  channel, CPRD ) ;
  PWMC_SetDutyCycle(  PWM,  channel,  duty ) ;
  PWMC_SetDeadTime(  PWM,  channel,  timeH,  timeL ) ;

  //  Configure PWM pin
  PIO_Configure(
    g_APinDescription[pwmPin].pPort,
    peripheralId,
    g_APinDescription[pwmPin].ulPin,
    g_APinDescription[pwmPin].ulPinConfiguration);
}

/*
 * @function    setVoltage(double v, uint32_t channel)
 * @param       double    v  Voltage to set
 * @param       uint32_t  channel PWM channel
 * @return      void
 * @description Set DC voltage of PWM
 */
void setVoltage(double v, uint32_t channel)
{
  if (v >= 0 && v <= 9)
  {
    uint16_t duty;
    duty = (uint16_t)((v * (double)CPRD)/9);
    PWMC_SetDutyCycle(  PWM,  channel,  duty ) ;
    PWMC_EnableChannel(  PWM,  channel ) ;
  }
}

/*
 * ************************************************************************************************************
 * ----------------------------------------   Encoder Functions   ---------------------------------------------
 * ************************************************************************************************************
 */
 /*
 * @function    get_nPulse()
 * @return      int Number of pulses save in the variable
 * @description Getter of nPulse variable
 */
int get_nPulse()
{
  return nPulse;
}

/*
 * @function    newEncPulse()
 * @return      void
 * @description Add new pulse
 */
void newEncPulse ()
{
  nPulse++;
}

/*
 * @function    setupEncInt(int encPin1, int encPin2)
 * @param       int encPin1 Pin of enconder
 * @param       int encPin2 Pin of enconder
 * @return      void
 * @description Set the pins of encoder as input and create interrupts for them, using newEncPulse as callback
 */
void setupEncInt(int encPin1, int encPin2)
{
  pinMode(encPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(encPin1), newEncPulse, CHANGE);

  pinMode(encPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(encPin2), newEncPulse, CHANGE);
}

/*
 * ************************************************************************************************************
 * --------------------------------------   Motor Model Functions   -------------------------------------------
 * ************************************************************************************************************
 */
/*
 * @function    samplePosition()
 * @return      void
 * @description Save the position in the array. Also control the PWM voltage depending on the number of samples and repetitions
 */
void samplePosition()
{
  int nPulse = get_nPulse();
  
  if (nSample < (nTotalSample/2)) 
  {
    setVoltage(voltModel, channel_0);
    setVoltage(0, channel_1);

    posM[nSample] += nPulse;
    nSample++; 
  }
  else if (nSample >= 600 && nSample < 1200) 
  {
    setVoltage(0, channel_0);
    setVoltage(0, channel_1);

    posM[nSample] += nPulse;
    nSample++;
  }
  else if (nSample == nTotalSample)
  {
    nRep++;
    nPulse = 0;
    if (nRep < nTotalRep)
    {
      nSample = 0;
    }
    else if (nRep == nTotalRep)
    {
      print_result  = true;
      nRep++;
      nSample++;
    }
  }
}

/*
 * @function    meanValueDoubleArray(double* a)
 * @param       double* a Pointer to double array
 * @return      void
 * @description Calculate mean value
 */
void meanValueDoubleArray(double* a)
{
  int i;
  for (i = 0; i<(sizeof(a)/sizeof(double)); i++)
  {
    a[i] = a[i]/((double)nTotalRep);
  }
}

/*
 * @function    initDoubleArray(double* a)
 * @param       double* a Pointer to double array
 * @return      void
 * @description Initialization of array
 */
void initDoubleArray(double* a)
{
  int i;
  for(i = 0; i<(sizeof(a)/sizeof(double)); i++)
  {
    a[i] = 0.0;
  }
}

/*
 * ************************************************************************************************************
 * -------------------------------------------   Main Program   -----------------------------------------------
 * ************************************************************************************************************
 */
void setup() 
{
  Serial.begin(115200);
  setupPWM(channel_0, pwmPin35, peripheralId_0);
  setupPWM(channel_1, pwmPin37, peripheralId_1);

  setupEncInt(encPin1, encPin2);

  initDoubleArray(posM);
  
  Timer3.attachInterrupt(samplePosition).setPeriod(1000);    //  Cada 1000 us se lanza la función readDecoder()
  Timer3.start();
  
  setVoltage(voltModel, channel_0);
  setVoltage(0, channel_1);

}

void loop() 
{
  int i;
  for (voltModel = 1; voltModel < 10; voltModel++)
  {
    while (!print_result)
    {
      
    } 
      meanValueDoubleArray(posM);
      Serial.print("VOLTAJE = ");
      Serial.println(voltModel);
      Serial.print("0.00");
      Serial.print(" ");
      Serial.println("0.00");
      for (i=0; i<nTotalSample; i++)
      {
        Serial.print((double)(i+1));
        Serial.print(" ");
        Serial.println(posM[i]);
      }
      Serial.println();
      print_result = false; 
      nSample = 0;
      nRep = 0;
      initDoubleArray(posM);
      delay(1000);

      if (voltModel == 9)
      {
        fin = true;
      }
  }  
  while(fin){}
}


