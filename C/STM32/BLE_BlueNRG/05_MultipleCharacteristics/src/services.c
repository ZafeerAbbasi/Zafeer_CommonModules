/*
# ##############################################################################
# File: services.c                                                             #
# Project: src                                                                 #
# Created Date: Tuesday, October 24th 2023, 9:41:38 pm                         #
# Author: Zafeer Abbasi                                                        #
# ----------------------------------------------                               #
# Last Modified: Saturday, October 28th 2023, 2:18:58 pm                       #
# Modified By: Zafeer Abbasi                                                   #
# ----------------------------------------------                               #
# Copyright (c) 2023 Zafeer.A                                                  #
# ----------------------------------------------                               #
# HISTORY:                                                                     #
 */

/*##############################################################################################################################################*/
/*INCLUDES______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include "services.h"
#include "main.h"

/*##############################################################################################################################################*/
/*FUNCTION DECLARATIONS_________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*GLOBALS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

/**
 * @brief Number of CUSTOM Services: 2
 * 1. Health Service
 * 2. Weather Service
 * 
 * Health Service has 2 characteristics:
 *  1. BPM ( Beats Per Minute )
 *  2. Weight of the person
 * 
 * Weather Service has 2 characteristics:
 *  1. Temperature 
 *  2. Humidity
 */

/**
 * @brief Custom Health Service ( UUID's are randomly Generated )
 * As a custom, Service and Characteristic UUID's should be very similar. Only differences are in the 13th Byte
 */
const uint8_t HEALTH_SERVICE_UUID[ 16 ]     = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe0,0xf2,0x73,0xd9};
const uint8_t HEALTH_BPM_CHAR_UUID[ 16 ]    = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe1,0xf2,0x73,0xd9};
const uint8_t HEALTH_WEIGHT_CHAR_UUID[ 16 ] = {0x66,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0xe2,0xf2,0x73,0xd9};
uint16_t healthServiceHdl, healthBPMCharHdl, healthWeightCharHdl, healthBPMCharValueHdl, healthWeightCharValueHdl; 

/**
 * @brief Custom Weather Service ( UUID's are randomly Generated )
 * As a custom, Service and Characteristic UUID's should be very similar. Only differences are in the 13th Byte
 */
const uint8_t WEATHER_SERVICE_UUID[ 16 ]    = {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x00,0xf2,0x73,0xd9};
const uint8_t WEATHER_TEMP_CHAR_UUID[ 16 ]  = {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x01,0xf2,0x73,0xd9};
const uint8_t WEATHER_HUM_CHAR_UUID[ 16 ]   = {0x67,0x9a,0x0c,0x20,0x00,0x08,0x96,0x9e,0xe2,0x11,0x9e,0xb1,0x02,0xf2,0x73,0xd9};
uint16_t weatherServiceHdl, weatherTmpCharHdl, weatherHumCharHdl, weatherTmpCharValueHdl, weatherHumCharValueHdl;

/**
 * @brief Flags and User Connection Handle
 * 
 */
uint8_t     FLAG_CONNECTED              = FALSE;
uint8_t     FLAG_SET_CONNECTABLE        = TRUE;
uint8_t     FLAG_NOTIFICATION_ENABLED   = FALSE;

uint16_t    usrConnectionHdl            = 0;

/*Example Sensor Values*/
int32_t BPM         = 85;
int32_t WEIGHT      = 90;
int32_t TEMPERATURE = 20;
int32_t HUMIDITY    = 80;

/*##############################################################################################################################################*/
/*DEFINES_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*EXTERNS_______________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*TYPEDEFS/STRUCTS/ENUMS________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/



/*##############################################################################################################################################*/
/*FUNCTIONS_____________________________________________________________________________________________________________________________________*/
/*##############################################################################################################################################*/

/**
 * @brief Add All the Custom Defined Services
 * 
 * @return tBleStatus Status of Operation
 */
tBleStatus service_AddServices( void )
{
    /* Declare Local Service / Characteristic UUID's */
    Service_UUID_t  healthServiceUUID, weatherServiceUUID;
    Char_UUID_t     healthBPMCharUUID, healthWeightCharUUID, weatherTmpCharUUID, weatherHumCharUUID;

    /*Copy Global UUID's to local variables 
    memcpy( Destination,                            Source,                     size                                )*/
    memcpy( healthServiceUUID.Service_UUID_128,     HEALTH_SERVICE_UUID,        sizeof( HEALTH_SERVICE_UUID )       );
    memcpy( weatherServiceUUID.Service_UUID_128,    WEATHER_SERVICE_UUID,       sizeof( WEATHER_SERVICE_UUID )      );
    memcpy( healthBPMCharUUID.Char_UUID_128,        HEALTH_BPM_CHAR_UUID,       sizeof( HEALTH_BPM_CHAR_UUID )      );
    memcpy( healthWeightCharUUID.Char_UUID_128,     HEALTH_WEIGHT_CHAR_UUID,    sizeof( HEALTH_WEIGHT_CHAR_UUID )   );
    memcpy( weatherTmpCharUUID.Char_UUID_128,       WEATHER_TEMP_CHAR_UUID,     sizeof( WEATHER_TEMP_CHAR_UUID )    );
    memcpy( weatherHumCharUUID.Char_UUID_128,       WEATHER_HUM_CHAR_UUID,      sizeof( WEATHER_HUM_CHAR_UUID )     );

    /*Add Services*/
    aci_gatt_add_service( 
        UUID_TYPE_128, 
        &healthServiceUUID, 
        PRIMARY_SERVICE, 
        7, 
        &healthServiceHdl 
    );
    aci_gatt_add_service( 
        UUID_TYPE_128, 
        &weatherServiceUUID, 
        PRIMARY_SERVICE, 
        7, 
        &weatherServiceHdl 
    );

    /*Add Characteristics*/
    aci_gatt_add_char(
        healthServiceHdl,
        UUID_TYPE_128,
        &healthBPMCharUUID,
        4,
        CHAR_PROP_READ,
        ATTR_PERMISSION_NONE,
        GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
        0,
        0,
        &healthBPMCharHdl
    );
    aci_gatt_add_char(
        healthServiceHdl,
        UUID_TYPE_128,
        &healthWeightCharUUID,
        4,
        CHAR_PROP_READ,
        ATTR_PERMISSION_NONE,
        GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
        0,
        0,
        &healthWeightCharHdl
    );
    aci_gatt_add_char(
        weatherServiceHdl,
        UUID_TYPE_128,
        &weatherTmpCharUUID,
        4,
        CHAR_PROP_READ,
        ATTR_PERMISSION_NONE,
        GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
        0,
        0,
        &weatherTmpCharHdl
    );
    aci_gatt_add_char(
        weatherServiceHdl,
        UUID_TYPE_128,
        &weatherHumCharUUID,
        4,
        CHAR_PROP_READ, 
        ATTR_PERMISSION_NONE,
        GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
        0,
        0,
        &weatherHumCharHdl
    );

    /*Update Characteristic Value Handle*/
    healthBPMCharValueHdl       = healthBPMCharHdl      + 1;
    healthWeightCharValueHdl    = healthWeightCharHdl   + 1;
    weatherTmpCharValueHdl      = weatherTmpCharHdl     + 1;
    weatherHumCharValueHdl      = weatherHumCharHdl     + 1;

    
    return BLE_STATUS_SUCCESS;
}

/**
 * @brief Update Characteristic Value
 * 
 * @param charValueHdl Characteristic Value Handle for the Characteristic to be Updated
 * @param newData New Value for the Characteristic
 */
void service_UpdateData( uint16_t charValueHdl, int32_t newData )
{
    tBleStatus ret;
    newData = swap_int32( newData );

    if( charValueHdl == healthBPMCharValueHdl )
    {
        
        /*Update BPM Characteristic Value*/
        ret = aci_gatt_update_char_value(
            healthServiceHdl,
            healthBPMCharHdl,
            0,
            4,
            ( uint8_t * )&newData
        );
        if( ret != BLE_STATUS_SUCCESS )
        {
            printf( " BPM CHARACTERISTIC VALUE UPDATE FAILED ... \r\n " );
        }
        
    }
    if( charValueHdl == healthWeightCharValueHdl )
    {

        /*Update Weight Characteristic Value*/
        ret = aci_gatt_update_char_value(
            healthServiceHdl,
            healthWeightCharHdl,
            0,
            4,
            ( uint8_t * )&newData
        );
        if( ret != BLE_STATUS_SUCCESS )
        {
            printf( " WEIGHT CHARACTERISTIC VALUE UPDATE FAILED ... \r\n " );
        }
        
    }
    if( charValueHdl == weatherTmpCharValueHdl )
    {

        /*Update Temp Characteristic Value*/
        ret = aci_gatt_update_char_value(
            weatherServiceHdl,
            weatherTmpCharHdl,
            0,
            4,
            ( uint8_t * )&newData
        );
        if( ret != BLE_STATUS_SUCCESS )
        {
            printf( " TEMPERATURE CHARACTERISTIC VALUE UPDATE FAILED ... \r\n " );
        }
        
    }
    if( charValueHdl == weatherHumCharValueHdl )
    {

        /*Update Humidity Characteristic Value*/
        ret = aci_gatt_update_char_value(
            weatherServiceHdl,
            weatherHumCharHdl,
            0,
            4,
            ( uint8_t * )&newData
        );
        if( ret != BLE_STATUS_SUCCESS )
        {
            printf( " HUMIDITY CHARACTERISTIC VALUE UPDATE FAILED ... \r\n " );
        }
        
    }
}

/*Converting from Big Endian to Little Endian*/
int32_t swap_int32(int32_t val) 
{
    return ((val << 24) & 0xFF000000) |
           ((val <<  8) & 0x00FF0000) |
           ((val >>  8) & 0x0000FF00) |
           ((val >> 24) & 0x000000FF);
}

/*Custom CallBack Implementations------------------------------------------------------------------------------*/

/**
 * @brief Custom Connection Complete CallBack. Used in GAP ( Generic Access Profile )
 * 
 * @param handle Connection Handle
 */
void GAP_customConnectionCompleteCB( uint16_t handle )
{
    FLAG_CONNECTED = TRUE;
    usrConnectionHdl = handle;
    printf( " Connection Complete ... \r\n " );
    HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET );
}

/**
 * @brief Custom Disconnection Complete CallBack. 
 * 
 */
void GAP_customDisconnectionCompleteCB( void )
{
    FLAG_CONNECTED = FALSE;
    usrConnectionHdl = 0;
    FLAG_SET_CONNECTABLE = TRUE;
    printf( " Disconnection Complete ... \r\n " );
    HAL_GPIO_WritePin( LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET );
}

/**
 * @brief Custom Read Request CallBack.
 * 
 * @param handle Characteristic Value Attribute Handle
 * 
 * Context:
 * Attribute handle, which is passed by the function: aci_gatt_read_permit_req_event is the Characteristic Value Handle.
 * The Handle which we delcared ourselved ( e.g. weatherHumCharHdl ) is the Characteristic Declaration Handle.
 * 
 * --------------------------------Characteristic Declaration Handle:-----------------------------------------------------
 * The Characteristic Handle is commonly referred to as the handle of the Characteristic Declaration Attribute.
 * This is a metadata attribute, which in its value, describes various properties of the characteristic, such as if it's
 * readable, writable, it's UUID and it's Value Handle. 
 * 
 * --------------------------------Characteristic Value Handle:-----------------------------------------------------------
 * The value for this handle is also found in the Characteristic Declaration Handle's value. The Characteristic Value 
 * Handle is commonly referred to as the handle of the Characteristic Value Attribute. This Attribute actually contains 
 * the value or data payload for the Characteristic in its attribute value.
 * 
 * ------------------Relation between Characteristic Declaration Handle and Characteristic:-------------------------------
 * In many BLE Stacks, including BlueNRG, the handle of the Characteristic Value Attribute is generally ONE more than 
 * the handle of the corresponding Characteristic Declaration Handle. 
 * 
 * The function: aci_gatt_read_permit_req_event has a parameter: Attribute Handle. This is the same parameter that needs
 * to be passed in to this function. The Attribute Handle parameter of the aci_gatt_read_permit_req_event is the 
 * Characteristic Value Handle, not the Characteristic Declaration Handle ( e.g. of Characteristic Declaration Handle 
 * can be the ones we defined ourselved, like weatherHumCharHdl ).
 * 
 * -----Why is the Attribute Handle parameter of aci_gatt_read_permit_req_event a Characteristic Value Handle-------------
 * ----------------------------instead of Characteristic Declaration Handle?----------------------------------------------
 * In a typical use-case scenario, the central device is usually interested in reading the value of a characteristic, 
 * not its metadata. Therefore, the read request generally pertains to the "Characteristic Value" attribute.
 */
void GATT_readRqstCB( uint16_t charValueHdl )
{
    if( charValueHdl == healthBPMCharValueHdl )
    {
        
        printf( " BPM READ REQUEST RECEIEVED ...            VALUE TO BE SENT: %ld \r\n ", BPM );
        service_UpdateData( charValueHdl, BPM );
        
    }
    if( charValueHdl == healthWeightCharValueHdl )
    {

        printf( " WEIGHT READ REQUEST RECEIEVED ...         VALUE TO BE SENT: %ld \r\n ", WEIGHT );
        service_UpdateData( charValueHdl, WEIGHT );
        
    }
    if( charValueHdl == weatherTmpCharValueHdl )
    {

        printf( " TEMPERATURE READ REQUEST RECEIEVED ...    VALUE TO BE SENT: %ld \r\n ", TEMPERATURE );
        service_UpdateData( charValueHdl, TEMPERATURE );
        
    }
    if( charValueHdl == weatherHumCharValueHdl )
    {

        printf( " HUMIDITY READ REQUEST RECEIEVED ...       VALUE TO BE SENT: %ld \r\n ", HUMIDITY );
        service_UpdateData( charValueHdl, HUMIDITY );
        
    }

    /*If Connection is valid, allow read request*/
    if( usrConnectionHdl != 0 )
    {
        aci_gatt_allow_read( usrConnectionHdl );
    }
    
}

/*Register Custom CallBacks---------------------------------------------------------------------------------*/

void hci_le_connection_complete_event
									(uint8_t Status,
									uint16_t Connection_Handle,
									uint8_t Role,
									uint8_t Peer_Address_Type,
									uint8_t Peer_Address[6],
									uint16_t Conn_Interval,
									uint16_t Conn_Latency,
									uint16_t Supervision_Timeout,
									uint8_t Master_Clock_Accuracy
									)
{
    GAP_customConnectionCompleteCB( Connection_Handle );
}

void hci_le_disconnection_complete_event
										(uint8_t Status,
										 uint16_t Connection_Handle,
										 uint8_t Reason
										)
{
    GAP_customDisconnectionCompleteCB( );
}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
    GATT_readRqstCB( Attribute_Handle );
}

/*User Event Receive Function Implementation*/
void APP_userEvtRx( void * pData )
{
    uint32_t i;

    hci_spi_pckt *hciPckt = ( hci_spi_pckt * )pData;

    /*Process Event Packet*/
    if( hciPckt->type == HCI_EVENT_PKT )
    {
        /*Process Event Packet*/
        /*Get Data from Packet*/
        hci_event_pckt *eventPckt = ( hci_event_pckt * )hciPckt->data;

        if( eventPckt->evt == EVT_LE_META_EVENT )
        {
            
            /*Process Meta Data Event*/
            /*Get Meta Data*/
            evt_le_meta_event *metaDataEvt = ( void * )eventPckt->data;

            /*Loop through all the possible Meta events and process the specific event which occured*/
            for( i = 0; i < HCI_LE_META_EVENTS_COUNT; i++ )
            {
                if( metaDataEvt->subevent == hci_le_meta_events_table[ i ].evt_code )
                {
                    hci_le_meta_events_table[ i ].process( ( void * )metaDataEvt->data );
                }
            }
            
        } 
        else if( eventPckt->evt == EVT_VENDOR )
        {
            
            /*Process Vendor Event*/
            /*Get Vendor ( BlueNRG ) Data*/
            evt_blue_aci *blueNRGEvt = ( void * )eventPckt->data;

            /*Loop through all the possible Vendor Events and and process the specific event which occured*/
            for( i = 0; i < HCI_VENDOR_SPECIFIC_EVENTS_COUNT; i++ )
            {
                if( blueNRGEvt->ecode == hci_vendor_specific_events_table[ i ].evt_code )
                {
                    hci_vendor_specific_events_table[ i ].process( ( void * )blueNRGEvt->data );
                }
            }
            
        } 
        else
        {

            /*Process Normal Event*/
            /*Loop through all normal events and process the event which occured*/
            for( i = 0; i < HCI_EVENTS_COUNT; i++ )
            {
                if( eventPckt->evt == hci_events_table[ i ].evt_code )
                {
                    hci_events_table[ i ].process( ( void * )eventPckt->data );
                }
            }
            
        }
    }
    
    /*Process other events*/
    
}