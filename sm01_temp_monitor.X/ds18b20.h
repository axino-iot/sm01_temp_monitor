/* 
 * File:   DS18B20.h
 * Author: MFT
 *
 * Created on 14 July 2019, 14:24
 */

#ifndef DS18B20_H
#define	DS18B20_H

#include "onewire.h"

#ifdef	__cplusplus
extern "C" {
#endif

//Converts register values to floating point temperature values 
float ConvertC(uint16_t lsb, uint16_t msb);

//Calculates CRC to validate the data read from sensor
uint16_t calculateCRC(uint8_t inByte[], uint8_t num_bytes);

//Read the centigrade temperature according to the data sheet.
float ReadTempC();


#ifdef	__cplusplus
}
#endif

#endif	/* DS18B20_H */

