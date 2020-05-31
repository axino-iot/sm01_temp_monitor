/*******************************************************************
 *  Cloud connected temperature monitoring system.
 *******************************************************************
 * File:    main.c
 * Company: SciFlair Ltd.
 * Notes: 
 *    1) Compiler XC16 v1.25 was used to compile this project,  
 *       using another version might produce errors. 
 * 
 * Copyright (C)2020 SciFlair Ltd. - All Rights Reserved.
 * 
 * You may use, modify, copy and distribute these codes to be used 
 * solely on AXINO SM01 modules. 
 *   
 * Author            Date        Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * MFT               03/05/2020  Original release
 ********************************************************************/
#include <xc.h>
#include "sm01_lib/sm01.h"
#include <string.h>

#include "onewire.h"
#include "ds18b20.h"

//Application variables
static double tempc, tempf;
static char buffer[64] = {0};

static uint32_t lastUpdateTime = 0;
static uint16_t updateInterval = 120;
static bool updateReady = false;

//Main program begins here
int main(void)
{

    //Initialize SM01 hardware and all its required modules
    SM01_Init(SM01_MODULE_IN_USE, SM01_MODE_AUTO); 
    
    //Initialize Onewire communication
    InitOneWire();
    
    while(1)
    {
        //check the time in seconds since last update and set flag for update if  
        //update interval is past.
        if((SM01_UpTime() - lastUpdateTime) >  updateInterval)
        {
            lastUpdateTime = SM01_UpTime();
            updateReady = true;        
        }        
        
        if(SW_IO == 0 || updateReady == true) //publish to cloud if update is required, or switch on module is pressed 
        {
            //Check if cloud is connected
            if(EZCOM_IsConnected())
            {
                updateReady = false;

                printf("\r\nSending Message to Cloud...");
                
                //Read temperature value in C
                tempc = ReadTempC(); 
                //Convert to F
                tempf = 1.8*tempc+32.0;
                //Write it in a buffer as a JSON object
                sprintf(buffer,"{\"tempc\":%6.3f,\"tempf\":%6.3f}", tempc, tempf);
                
                //Send uplink message from device to Axino cloud
                if(EZCOM_Publish(1, buffer, false, EZCOM_QOS_1))
                        printf("\r\nMessage Sent: %s", buffer);
                else printf("\r\nMessage Failed!");
            }
            else printf("\r\nCloud is not connected");
            //wait 500ms for switch debouncing
            TickWait(500);
            //SM01_Sleep(5);
        }
    }
}

//call back function to receive downlink messages from cloud to device
bool EZCOM_OnReceive(uint32_t deviceid, uint32_t topic, uint8_t* msg, uint16_t msglen)
{
    if(topic == 2 && strncmp("interval=", (char*)msg,9) == 0)
    {
        updateInterval = atoi((char*)msg+9);
        if(updateInterval < 30) updateInterval = 30;        //limit max value to 30 seconds
        if(updateInterval > 3600) updateInterval = 3600;    //limit max value to 3600 seconds (1 hour) 
        printf("\r\nUpdate interval is set to %u Seconds", updateInterval);
    }
    else printf("\r\nUnknown Message");

    return true;
}
