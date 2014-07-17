#ifndef _LAMP_MANAGER_H_
#define _LAMP_MANAGER_H_
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

#include <Manager.h>
#include <PresetManager.h>
#include <Mutex.h>
#include <LampClients.h>

#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace lsf {

class LampsAndState {
  public:
    LampsAndState(LSFStringList lampList, LampState lampState, uint32_t period) :
        state(lampState), transitionPeriod(period) {
        for (LSFStringList::iterator it = lampList.begin(); it != lampList.end(); it++) {
            LSFString uniqueId = *it;
            if (std::find(lamps.begin(), lamps.end(), uniqueId) == lamps.end()) {
                lamps.push_back(uniqueId);
            }
        }
    }

    LSFStringList lamps;
    LampState state;
    uint32_t transitionPeriod;
};

class LampsAndPreset {
  public:
    LampsAndPreset(LSFStringList lampList, LSFString presetID, uint32_t period) :
        lamps(lampList), presetID(presetID), transitionPeriod(period) { }

    LSFStringList lamps;
    LSFString presetID;
    uint32_t transitionPeriod;
};

class LampsAndStateField {
  public:
    LampsAndStateField(LSFStringList lampList, LSFString fieldName, ajn::MsgArg arg, uint32_t period) :
        lamps(lampList), stateFieldName(fieldName), stateFieldValue(arg), transitionPeriod(period) { }

    LSFStringList lamps;
    LSFString stateFieldName;
    ajn::MsgArg stateFieldValue;
    uint32_t transitionPeriod;
};

class PulseLampsWithState {
  public:
    PulseLampsWithState(LSFStringList lampList, LampState fromLampState, LampState toLampState, uint32_t period, uint32_t duration, uint32_t numPulses) :
        lamps(lampList), fromState(fromLampState), toState(toLampState), period(period), duration(duration), numPulses(numPulses) { }

    LSFStringList lamps;
    LampState fromState;
    LampState toState;
    uint32_t period;
    uint32_t duration;
    uint32_t numPulses;
};

class PulseLampsWithPreset {
  public:
    PulseLampsWithPreset(LSFStringList lampList, LSFString fromPreset, LSFString toPreset, uint32_t period, uint32_t duration, uint32_t numPulses) :
        lamps(lampList), fromPreset(fromPreset), toPreset(toPreset), period(period), duration(duration), numPulses(numPulses) { }

    LSFStringList lamps;
    LSFString fromPreset;
    LSFString toPreset;
    uint32_t period;
    uint32_t duration;
    uint32_t numPulses;
};

typedef std::list<LampsAndState> LampsAndStateList;
typedef std::list<LampsAndPreset> LampsAndPresetList;
typedef std::list<LampsAndStateField> LampsAndStateFieldList;
typedef std::list<PulseLampsWithState> PulseLampsWithStateList;
typedef std::list<PulseLampsWithPreset> PulseLampsWithPresetList;

class LampManager : public Manager {
  public:
    friend class LampGroupManager;

    LampManager(ControllerService& controllerSvc, PresetManager& presetMgr);

    ~LampManager();

    /*
     * Start the Lamp Manager
     *
     * @param   None
     * @return  ER_OK if successful, error otherwise
     */
    QStatus Start(const char* keyStoreFileLocation);

    /*
     * Stop the Lamp Manager
     *
     * @param   None
     * @return  ER_OK if successful, error otherwise
     */
    void Stop(void);

    void Join(void);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetAllLampIDs
     *
     * @param message   The params
     */
    void GetAllLampIDs(ajn::Message& message);

    void GetAllLamps(LampNameMap& lamps) {
        lampClients.GetAllLamps(lamps);
    }

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampFaults
     *
     * @param message   The params
     */
    void GetLampFaults(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampFaults
     *
     * @param message   The params
     */
    void GetLampServiceVersion(ajn::Message& message);


    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.ClearLampFault
     *
     * @param message   The params
     */
    void ClearLampFault(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampSupportedLanguages
     *
     * @param message   The params
     */
    void GetLampSupportedLanguages(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampName
     *
     * @param message   The params
     */
    void GetLampManufacturer(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampName
     *
     * @param message   The params
     */
    void GetLampName(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.SetLampName
     * @param message   The params
     */
    void SetLampName(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampDetails
     *
     * @param message   The params
     */
    void GetLampDetails(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampParameters
     *
     * @param message   The params
     */
    void GetLampParameters(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampParametersField
     *
     * @param message   The params
     */
    void GetLampParametersField(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampState
     *
     * @param message   The params
     */
    void GetLampState(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.GetLampStateField
     *
     * @param message   The params
     */
    void GetLampStateField(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.TransitionLampState
     *
     * @param message   The params
     */
    void TransitionLampState(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.TransitionLampState
     *
     * @param message   The params
     */
    void PulseLampWithState(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.TransitionLampPreset
     *
     * @param message   The params
     */
    void PulseLampWithPreset(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.TransitionLampStateToPreset
     *
     * @param message   The params
     */
    void TransitionLampStateToPreset(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.TransitionLampStateField
     *
     * @param message   The params
     */
    void TransitionLampStateField(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.ResetLampState
     *
     * @param message   The params
     */
    void ResetLampState(ajn::Message& message);

    /**
     * Process an AllJoyn call to org.allseen.LSF.ControllerService.ResetLampStateField
     *
     * @param message   The params
     */
    void ResetLampStateField(ajn::Message& message);

    uint32_t GetControllerServiceLampInterfaceVersion(void);

    void ConnectToLamps(void) {
        lampClients.ConnectToLamps();
    }

    void DisconnectFromLamps(void) {
        lampClients.DisconnectFromLamps();
    }

  private:

    void ResetLampStateInternal(ajn::Message& message, LSFStringList lamps, bool groupOperation = false);

    void ResetLampStateFieldInternal(ajn::Message& message, LSFStringList lamps, LSFString stateFieldName, bool groupOperation = false);

    void ChangeLampStateAndField(ajn::Message& message,
                                 LampsAndStateList& transitionToStateComponent,
                                 LampsAndPresetList& transitionToPresetComponent,
                                 LampsAndStateFieldList& stateFieldComponent,
                                 PulseLampsWithStateList& pulseWithStateComponent,
                                 PulseLampsWithPresetList& pulseWithPresetComponent,
                                 bool groupOperation = false,
                                 bool sceneOperation = false);

    LampClients lampClients;
    PresetManager& presetManager;

};

}

#endif
