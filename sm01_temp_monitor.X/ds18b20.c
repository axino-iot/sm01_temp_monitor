
#include "DS18B20.h"

float ReadTempC()
{
    uint16_t i, result, crc;
    uint8_t temp[9] = {0};
    static float tempval;
      
    result = ResetBus();
 
    WriteByte(0xCC);
    WriteByte(0x44);
    
    TickWait(750);
    
    result = 0;
    while(!result)
    {
        result = ReadBit();
        TickWait_us(5);
    }
    result = ResetBus();
    
    WriteByte(0xCC);
    WriteByte(0xBE);
   
    for(i=0; i<9; i++)
    {
        temp[i]=ReadByte();
    }
    result = ResetBus();
        
    crc = calculateCRC(temp, 8);
    
    if(crc == temp[8])
        tempval = ConvertC(temp[0], temp[1]);
    else
    {
       printf("\r\ncrc failed, calculated = %u received=%u", crc, temp[8]); 
    }
    
    return(tempval);
    
}

float ConvertC(uint16_t lsb, uint16_t msb)
{
    uint16_t i, tempw = 0;
    float tempc, fraction = 0.0;

    float frac[] = {0.0625, 0.125, 0.25, 0.5};
    
    tempw = (msb<<8)+lsb;
    
    //calculate the whole part
    tempc = (tempw >> 4);
   
    //calculate the fractional part
    for(i = 0; i<4; i++)
    {
        fraction += frac[i] * (tempw & 0x1);
        tempw >>= 1;
    }
    
    return(tempw + fraction);
}

/* Calculates CRC based upon polynomial X^8 + X^5 + X^4 + 1.
Data is read starting at the least significant bit.
*/
uint16_t calculateCRC(uint8_t inByte[], uint8_t num_bytes)
{
  uint16_t crc = 0;
  uint8_t i,j, tempByte;
    
  for (i = 0; i < num_bytes; i++){
    tempByte = inByte[i];
    for (j = 0; j < 8; j++){
      if ((crc % 2) ^ (tempByte %2)){
        crc ^= 0b100011000;
      }
      else{
        crc ^= 0b000000000;
      }
      crc >>= 1;
      tempByte >>= 1;
    }  
  }
  return(crc);
}