#ifndef _OEM_CODE_H_
#define _OEM_CODE_H_
/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <LampService.h>
#include <LampState.h>
#include <LampResponseCodes.h>
#include <LampValues.h>

/**
 * Get the Firmware version
 *
 * @param  None
 * @return  The firmware version
 */
uint32_t OEM_GetFirmwareVersion(void);

/**
 * Get the hardware version
 *
 * @param  None
 * @return  The hardware version
 */
uint32_t OEM_GetHardwareVersion(void);

/**
 * OEM-specific initialization
 *
 * @param  None
 * @return None
 */
void OEM_Initialize(void);

/**
 * Restart the Lamp
 *
 * @param  None
 * @return None
 */
void OEM_Restart(void);

/**
 * Reset the Lamp to factory settings
 *
 * @param  None
 * @return None
 */
void OEM_FactoryReset(void);

/**
 * Serialize the Lamp's current real-time parameters
 *
 * @param msg   The msg to serialize data into
 * @return      AJ_OK if no errors occured
 */
AJ_Status OEM_GetLampParameters(AJ_Message* msg);

/**
 * Return the current power draw, in milliamps
 *
 * @return  The power draw
 */
uint32_t OEM_GetPowerDraw();

/**
 * Get the current light output
 *
 * @return the current light output
 */
uint32_t OEM_GetOutput();

/**
 * Get the remaining life
 *
 * @return  The remaining life span
 */
uint32_t OEM_GetRemainingLife();


/**
 * Change the lamp state
 *
 * @param newState  New state of the Lamp
 * @param timestamp Timestamp of when to transition.
 * @return          LAMP_OK if no errors occured
 */
LampResponseCode OEM_TransitionLampState(LampState* newState, uint32_t timestamp);

/**
 * Serialize all active fault codes into a message.
 *
 * @param msg   The message to serialize into
 * @return      The LampResponseCode; LAMP_OK if no errors occured
 */
LampResponseCode OEM_GetLampFaults(AJ_Message* msg);

/**
 * Clear the lamp fault with the given code
 *
 * @param fault The fault code to clear
 * @return      LAMP_OK if the fault is valid and successfully cleared
 */
LampResponseCode OEM_ClearLampFault(LampFaultCode fault);

/**
 * Serialize the Lamp's details
 *
 * @param msg   The msg to serialize data into
 * @return      LAMP_OK if no errors occured
 */
LampResponseCode LAMP_MarshalDetails(AJ_Message* msg);

extern const char* deviceProductName;
extern const char* deviceManufactureName;

/**
 * This struct holds all fields of the Lamp's Details.
 */
typedef struct {
    const LampMake lampMake;
    const LampModel lampModel;

    const DeviceType deviceType;
    const LampType lampType;
    const BaseType baseType;


    const uint32_t deviceLampBeamAngle; /**< The beam angle */
    const uint8_t deviceDimmable;         /**< Dimmable? */
    const uint8_t deviceColor;            /**< color */
    const uint8_t variableColorTemperature; /**< variable color temperature? */
    const uint8_t deviceHasEffects;       /**< Are effects available? */

    const uint32_t deviceVoltage;       /**< voltage */
    const uint32_t deviceWattage;       /**< wattage */
    const uint32_t deviceWattageEquivalent; /**< Incandescent wattage equivalent */
    const uint32_t deviceMaxOutput;     /**< maximum output */
    const uint32_t deviceMinTemperature;    /**< lowest possible color temperature */
    const uint32_t deviceMaxTemperature;    /**< highest possible color temperature */
    const uint32_t deviceColorRenderingIndex; /**< rendering index */
    const uint32_t deviceLifespan;      /**< estimated lifespan */

} LampDetails_t;


extern LampDetails_t LampDetails;

#endif
