#include "onewire.h"

//Citation: https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/126.html

// 'tick' values
uint16_t A,B,C,D,E,F,G,H,I,J;


void InitOneWire(void)
{
    //define GPIO as an output
    GPIO_Mode(GPIO1W, OUTPUT);  
    //Drive it low so 0 is written to the latch.
    //Now GPIO1W_TRIS = 0 will activate the latch
    //and GPIO1W_TRIS = 1 shall activate the pin.
    GPIO_Write(GPIO1W, 0);    

    GPIO1W_TRIS = 1;  // Releases the bus

    // Adjust tick values depending on speed
    // Standard Speed
        A = 6 - 5;
        B = 64 -5;
        C = 60 -5;
        D = 10 -5 ;
        E = 9 -5;
        F = 55 -5 ;
        G = 0;
        H = 480 -5;
        I = 70 -5;
        J = 410 -5;

}

void WriteBit(uint16_t bit)
{
        if (bit)
        {
                // Write '1' bit
                GPIO1W_TRIS = 0; // Drives DQ low
                TickWait_us(A);
                GPIO1W_TRIS = 1;  //Releases the bus 
                TickWait_us(B); // Complete the time slot and 10us recovery
        }
        else
        {
                // Write '0' bit
                GPIO1W_TRIS = 0; // Drives DQ low
                TickWait_us(C);
                GPIO1W_TRIS = 1; // Releases the bus
                TickWait_us(D);
        }
}

// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
uint16_t ReadBit(void)
{
        uint16_t result;

        GPIO1W_TRIS = 0; // Drives DQ low
        TickWait_us(A);
        GPIO1W_TRIS = 1; // Releases the bus
        TickWait_us(E);
        result = GPIO1W_IN & 0x01; // Sample the bit value from the slave
        TickWait_us(F); // Complete the time slot and 10us recovery

        return result;
}

//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void WriteByte(uint16_t databyte)
{
        uint16_t loop;

        // Loop to write each bit in the byte, LS-bit first
        for (loop = 0; loop < 8; loop++)
        {
                WriteBit(databyte & 0x01);

                // shift the data byte for the next bit
                databyte >>= 1;
        }
}


// Read 1-Wire data byte and return it
//
uint16_t ReadByte(void)
{
        int16_t loop, result=0;

        for (loop = 0; loop < 8; loop++)
        {
                // shift the result to get it ready for the next bit
                result >>= 1;

                // if result is one, then set MS bit
                if (ReadBit())
                        result |= 0x80;
        }
        return result;
}

//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)
//
uint16_t ResetBus(void)
{
        uint16_t result;

        TickWait_us(G);
        GPIO1W_TRIS = 0; // Drives DQ low
        TickWait_us(H);
        GPIO1W_TRIS = 1; // Releases the bus
        TickWait_us(I);
        result = GPIO1W_IN ^ 0x01; // Sample for presence pulse from slave
        TickWait_us(J); // Complete the reset sequence recovery
        return result; // Return sample presence pulse result
}

// Write a 1-Wire data byte and return the sampled result.
//
uint16_t TouchByte(int data)
{
        uint16_t loop, result=0;

        for (loop = 0; loop < 8; loop++)
        {
                // shift the result to get it ready for the next bit
                result >>= 1;

                // If sending a '1' then read a bit else write a '0'
                if (data & 0x01)
                {
                        if (ReadBit())
                                result |= 0x80;
                }
                else
                        WriteBit(0);

                // shift the data byte for the next bit
                data >>= 1;
        }
        return result;
}

//-----------------------------------------------------------------------------
// Write a block 1-Wire data bytes and return the sampled result in the same
// buffer.
//
void WriteBlock(uint8_t *data, uint16_t data_len)
{
        int loop;

        for (loop = 0; loop < data_len; loop++)
        {
                data[loop] = TouchByte(data[loop]);
        }
}
