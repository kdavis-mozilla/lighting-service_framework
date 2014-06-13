#ifndef _OEM_CODE_H_
#define _OEM_CODE_H_
/**
 * @file OEMCode.h
 * @defgroup oem_code The OEM APIs used by the Lamp Service
 * @{
 */
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

#ifdef ONBOARDING_SERVICE
#include <alljoyn/onboarding/OnboardingManager.h>
#endif

/**
 * OEM-specific initialization
 *
 * @param  None
 * @return None
 */
void OEM_Initialize(void);

/**
 * Return a string that describes the current active faults
 *
 * @param  None
 * @return String describing the current active faults or NULL if there are no active faults
 */
const char* OEM_GetFaultsText(void);

/**
 * OEM-defined default state
 * This function is called when the lamp boots from the factory state.
 * It will set the initial LampState values
 *
 * @param  state The lamp state to set
 * @return None
 */
void OEM_InitialState(LampState* state);

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
 * @param   msg   The msg to serialize data into
 * @return  Status of the operation
 */
AJ_Status OEM_GetLampParameters(AJ_Message* msg);

/**
 * Return the Lamp's current energy usage in milliwatts
 *
 * @param   None
 * @return  The Lamp's current energy usage in milliwatts
 */
uint32_t OEM_GetEnergyUsageMilliwatts(void);

/**
 * Get the Lamp's current Brightness in Lumens
 *
 * @param   None
 * @return  The Lamp's current Brightness in Lumens
 */
uint32_t OEM_GetBrightnessLumens(void);

#ifdef ONBOARDING_SERVICE

/**
 * The default settings for the Onboarding service
 */
extern AJOBS_Settings OEM_OnboardingSettings;

#endif

/**
 * Change the lamp's on/off state in response
 * to the setting of Lamp State Interface onOff property.
 * The current state should be retrieved from the NVRAM,
 * updated and the new state should be saved back in the NVRAM.
 *
 * @param  onoff On or off
 * @return Status of the operation
 */
LampResponseCode OEM_SetLampOnOff(uint8_t onoff);

/**
 * Change the lamp's hue in response
 * to the setting of Lamp State Interface hue property.
 * The current state should be retrieved from the NVRAM,
 * updated and the new state should be saved back in the NVRAM.
 *
 * @param  hue   The hue
 * @return Status of the operation
 */
LampResponseCode OEM_SetLampHue(uint32_t hue);

/**
 * Change the lamp's brightness in response
 * to the setting of Lamp State Interface brightness property.
 * The current state should be retrieved from the NVRAM,
 * updated and the new state should be saved back in the NVRAM.
 *
 * @param  brightness  The brightness
 * @return Status of the operation
 */
LampResponseCode OEM_SetLampBrightness(uint32_t brightness);

/**
 * Change the lamp's saturation in response
 * to the setting of Lamp State Interface saturation property.
 * The current state should be retrieved from the NVRAM,
 * updated and the new state should be saved back in the NVRAM.
 *
 * @param  saturation The saturation
 * @return Status of the operation
 */
LampResponseCode OEM_SetLampSaturation(uint32_t saturation);

/**
 * Change the lamp's color temperature in response
 * to the setting of Lamp State Interface colorTemp property.
 * The current state should be retrieved from the NVRAM,
 * updated and the new state should be saved back in the NVRAM.
 *
 * @param   colorTemp     The color temperature
 * @return  Status of the operation
 */
LampResponseCode OEM_SetLampColorTemp(uint32_t colorTemp);

/**
 * This function needs to implemented by the OEM to support the Transition Effect
 *
 * @param  newState          New state of the Lamp to transition to
 * @param  timestamp         Timestamp of when to start the transition.
 * @param  transitionPeriod  The time period to transition over
 * @return Status of the operation
 */
LampResponseCode OEM_TransitionLampState(LampState* newState, uint64_t timestamp, uint32_t transitionPeriod);

/*
 * This function needs to implemented by the OEM to support the Pulse Effect
 * @param  fromState        Specifies the LampState that the Lamp needs to be in when starting a pulse
 * @param  toState          End state of a pulse
 * @param  period           Period of the pulse (in ms). Period refers to the time duration between the start of two pulses
 * @param  duration         The duration of a single pulse (in ms). This must be less than the period
 * @param  numPulses        Number of pulses
 * @param  timestamp        Time stamp of when to start applying the pulse effect from
 *
 * @return Status of the operation
 */
LampResponseCode OEM_ApplyPulseEffect(LampState* fromState, LampState* toState, uint32_t period, uint32_t duration, uint32_t numPulses, uint64_t timestamp);


/**
 * Serialize all active fault codes into a message.
 * Fault codes are unsigned 32-bit values that are
 * defined by the OEM.
 *
 * @param msg   The message to serialize into
 * @return      Status of the operation
 */
LampResponseCode OEM_GetLampFaults(AJ_Message* msg);

/**
 * Clear the lamp fault with the given code. Fault codes are unsigned
 * 32-bit values that are defined by the OEM.
 * This will be invoked when the ClearLampFault interface method is
 * invoked. The response code is defined by the OEM.
 *
 * @param fault The fault code to clear
 * @return      Status of the operation
 */
LampResponseCode OEM_ClearLampFault(LampFaultCode fault);

/**
 * Serialize the Lamp's details
 *
 * @param msg   The msg to serialize data into
 * @return      Status of the operation
 */
LampResponseCode LAMP_MarshalDetails(AJ_Message* msg);

/**
 * This struct holds all fields of the Lamp's Details.
 */
typedef struct {
    LampMake lampMake;                     /**< The make of the lamp */
    LampModel lampModel;                   /**< The model of the lamp */
    DeviceType deviceType;                 /**< The type of device */
    LampType lampType;                     /**< The type of lamp */
    BaseType baseType;                     /**< The make of the lamp base */
    uint32_t deviceLampBeamAngle;          /**< The beam angle */
    uint8_t deviceDimmable;                /**< Is the Lamp dimmable */
    uint8_t deviceColor;                   /**< Does the lamp support color */
    uint8_t variableColorTemp;             /**< variable color temperature? */
    uint8_t deviceHasEffects;              /**< Are effects available? */
    uint32_t deviceMinVoltage;             /**< Minimum voltage */
    uint32_t deviceMaxVoltage;             /**< Maximum voltage */
    uint32_t deviceWattage;                /**< Lamp Wattage */
    uint32_t deviceIncandescentEquivalent; /**< Incandescent equivalent */
    uint32_t deviceMaxLumens;              /**< Maximum light output in Lumens */
    uint32_t deviceMinTemperature;         /**< Minimum supported color temperature */
    uint32_t deviceMaxTemperature;         /**< Maximum supported color temperature */
    uint32_t deviceColorRenderingIndex;    /**< Lamp's Color Rendering Index */
} LampDetailsStruct;


/**
 * A global struct to hold this Lamp's details.
 */
extern LampDetailsStruct LampDetails;

/**
 * The about icon MIME type
 */
extern const char* aboutIconMimetype;

/**
 * The about icon raw data.
 * Note that when showing an icon, the About client may choose to use either
 * this raw image OR fetch the image pointed to by aboutIconUrl.
 */
extern const uint8_t aboutIconContent[];

/**
 * The about icon size
 */
extern const size_t aboutIconSize;

/**
 * The about icon URL
 * Note that when showing an icon, the About client may choose to use either
 * fetch it from this URL or use the raw data in aboutIconContent.
 */
extern const char* aboutIconUrl;

/**
 * The routing node prefix to discover. If this is set to NULL, the Lamp Service will
 * discover the default prefix org.alljoyn.BusNode
 */
extern const char* routingNodePrefix;

/**
 * @}
 */
#endif
