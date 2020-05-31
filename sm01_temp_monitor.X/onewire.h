/* 
 * File:   onewire.h
 * Author: MFT
 *
 * Created on 14 July 2019, 12:20
 * 
 */

#ifndef ONEWIRE_H
#define	ONEWIRE_H

#include "sm01_lib/sm01.h"

//One wire pin must to be pulled high with 4.7k ohm resister for it to work
//GPIO1W,GPIO1W_TRIS,GPIO1W_IN should be defined as given in sm01.h
#define GPIO1W GPIO14
#define GPIO1W_TRIS GPIO14_TRIS
#define GPIO1W_IN GPIO14_IN

#ifdef	__cplusplus
extern "C" {
#endif

    // Initialize 1-Wire bus
    void InitOneWire(void);
    
    // Write a bit to 1-Wire bus.
    void WriteBit(uint16_t bit);
    
    // Read a bit from the 1-Wire bus and return it. 
    uint16_t ReadBit(void);
    
    // Write 1-Wire data byte
    void WriteByte(uint16_t databyte);
    
    // Read 1-Wire data byte and return it
    uint16_t ReadByte(void);
    
    //Generate a 1-Wire reset, return 1 if no presence detect was found,
    // return 0 otherwise.
    uint16_t ResetBus(void);
    
    // Write a 1-Wire data byte and return the sampled result.
    uint16_t TouchByte(int data);
    
    // Write a block 1-Wire data bytes and return the sampled result in the same
    // buffer.
    void WriteBlock(uint8_t *data, uint16_t data_len);

#ifdef	__cplusplus
}
#endif

#endif	/* ONEWIRE_H */

