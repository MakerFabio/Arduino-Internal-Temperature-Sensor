/* read_signature_row 
 * this function reads and print the ATmega328P signature bytes (10 of them , not sure how many are really signature bytes)
 * they should contain also the internal temperature sensor calibration data
 * ATmega328P device signature is 0x1E, 0x95, 0x0F
*/ 
#include <avr/boot.h>
#define SIG_BYTECOUNT 6
uint8_t sr[SIG_BYTECOUNT] = {0};
void read_signature_row()
{   
  char buff[100];// used for printf
  
  for(int i=0; i<SIG_BYTECOUNT; i++)// print HEX
  {
    sr[i] = boot_signature_byte_get(i);
  }

  // print signature in hex format
  for(int i=0; i<SIG_BYTECOUNT; i++){
    sprintf(buff, " %02X",sr[i]);// print HEX format
    Serial.print(buff);
  }  
  Serial.println();  
  
  // print signature hex showing also array index
  for(int i=0; i<SIG_BYTECOUNT; i++){// print HEX
    sprintf(buff, "s%d=%02X ", i, sr[i]);// print HEX format with index
    Serial.print(buff);
  }
  Serial.println();  
}

// readTempNoCal(): reads internal temperature sensor with no calibration
// it uses 2 points line formula extrapolated from the typical value datasheet
// WARNING: accuracy can be much worse than +-10 degree becasue calibration is not performed!!
int readTempNoCal()
{
  char buff[100];// used for printf
  int temp;
  int aRead ;  // Read internal temperature sensor value (connected to ADC channel #8)

  ADMUX  = 0b11001000;   // set ADC REFERENCE to internal 1.1V and set ADC channel to 8 (temp)
  ADCSRA = 0b10000111;
  
  // burn some readings after ADMUX change
  for (int i = 0; i<10; i++){
    ADCSRA |= 0x40;
    while (ADCSRA & 0x40);     // The ADC start conversion bit remains HIGH until the ongoing A/D conversion is finished
  }
  aRead = ADCL;              // Read the ADC result (lower byte first)
  aRead |= (ADCH << 8);      // Read higher 2 bits (bit-9, bit-8) of the ADC result

  temp = (((long int)aRead *781)/1000)-250; // default value would be -250 but with 1.1 REF voltage, in the datasheet s sspecified to use 1.1V , but is not possible having a vref connected externally 
  
  sprintf(buff, "aRead=%d tempNoCal=%d", aRead, temp);
  Serial.println(buff);

  return temp;
}

// calculate internal temperature sensor with the datasheet formula
// WARNING: this funciton is not working at the moment, becasue there i didn't find correct signaturebyte interpretation for TS_OFFSET and TS_GAIN
// PREREQUISITE: void read_signature_row() has to be called in advance to read tdevice signature bytes
int readTempCal()
{
  char buff[100];// used for printf
  int tmp;
  long int aRead ;         // Read internal temperature sensor value (connected to ADC channel #8)
  unsigned int sOff, sGain;// index for sOff and sGain on the signature byte array
  int tempCorrSign;
  int ts_offset;         //TS_OFFSET is the signed twos complement temperature sensor offset reading
  unsigned int ts_gain;  //TS_GAIN is the unsigned fixed point 8-bit temperature sensor gain factor in 1/128th units stored in the signature row.

  ADMUX  = 0b11001000;   // set ADC REFERENCE to internal 1.1V, set ADC channel to 8 (temp)
  ADCSRA = 0b10000111;
  
  // burn some readings after ADMUX change
  for (int i = 0; i<10; i++){
    ADCSRA |= 0x40;
    while (ADCSRA & 0x40);     // The ADC start conversion bit remains HIGH until the ongoing A/D conversion is finished
  }
  
  aRead = ADCL;              // Read the ADC result (lower byte first)
  aRead |= (ADCH << 8);      // Read higher 2 bits (bit-9, bit-8) of the ADC result

  sOff = 3;// select 
  sGain = 5; 
  ts_offset=(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr1=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);
  
  sOff = 5;
  sGain = 3; 
  ts_offset=(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr2=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

  sOff = 1;
  sGain = 5; 
  ts_offset=(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr3=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

  sOff = 5;
  sGain = 1; 
  ts_offset=(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr4=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

  sOff = 3;
  sGain = 1; 
  ts_offset=(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr5=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

  sOff = 1;
  sGain = 3; 
  ts_offset=(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr6=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

// inverted offset, all prints
  sOff = 3;
  sGain = 5; 
  ts_offset=-(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr7=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);
  
  sOff = 5;
  sGain = 3; 
  ts_offset=-(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr 8=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);


  sOff = 1;
  sGain = 5; 
  ts_offset=-(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr 9=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

  sOff = 5;
  sGain = 1; 
  ts_offset=-(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr 10=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);


  sOff = 3;
  sGain = 1; 
  ts_offset=-(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr 11=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);


  sOff = 1;
  sGain = 3; 
  ts_offset=-(int8_t)sr[sOff];
  ts_gain=sr[sGain];
  tempCorrSign =(((aRead - (273 + 100 - ts_offset)) * 128) / ts_gain) + 25;
  sprintf(buff, "ts_off=%d(s%d) ts_gain=%d(s%d) tempCorr 12=%02d ", ts_offset, sOff, ts_gain, sGain, tempCorrSign);
  Serial.println(buff);

  return tempCorrSign;
 }

// ********************  FUNCTION DEFINITIONs *********************
void setup() 
{
  // Setup fo the parameters for SPI communication, set CSN and EN pins as outputs and set EN to High  
  Serial.begin(9600);   // sets the baud rate for communication with the computer to 9600 bauds
  Serial.println();    // optional: indent at the firss line 

  read_signature_row(); // read signature bytes
  }

// loop: MAIN LOOP
void loop() 
{ 
  int    temp;

  temp = readTempNoCal();
  temp = readTempCal();

  delay(1000);
}
