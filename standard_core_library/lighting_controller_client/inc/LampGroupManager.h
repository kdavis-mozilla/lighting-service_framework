#ifndef _LAMP_GROUP_MANAGER_H_
#define _LAMP_GROUP_MANAGER_H_

/**
 * @file
 * This file provides definitions for the Lamp Group Manager
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

#include <list>

#include <Manager.h>
#include <ControllerClientDefs.h>


namespace lsf {

class ControllerClient;

/**
 * Abstract base class implemented by User Application Developers.
 * The callbacks defined in this class allow the User Application
 * to be informed when Lamp Groups specific specific AllJoyn method
 * replies or signals are received from the Lighting Controller
 * Service
 */
class LampGroupManagerCallback {
  public:

    /**
     * Destructor
     */
    virtual ~LampGroupManagerCallback() { }

    /**
     * Indicates that a reply has been received for the method call GetAllLampGroupIDs method call
     *
     * @param responseCode   The response code
     * @param lampGroupIDs   Lamp Group IDs
     */
    virtual void GetAllLampGroupIDsReplyCB(const LSFResponseCode& responseCode, const LSFStringList& lampGroupIDs) { }

    /**
     * Indicates that a reply has been received for the method call GetLampGroupName method call
     *
     * @param responseCode   The response code
     * @param lampGroupID    The Lamp Group ID
     * @param lampGroupName  The Lamp Group Name
     */
    virtual void GetLampGroupNameReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID, const LSFString& language, const LSFString& lampGroupName) { }

    /**
     * Indicates that a reply has been received for the SetLampGroupName method call
     *
     * @param responseCode   The response code
     * @param lampGroupID    The Lamp Group ID
     */
    virtual void SetLampGroupNameReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID, const LSFString& language) { }

    /**
     * Indicates that the signal LampGroupsNameChanged has been received
     *
     * @param lampGroupIDs    The Lamp Group IDs
     */
    virtual void LampGroupsNameChangedCB(const LSFStringList& lampGroupIDs) { }

    /**
     * Indicates that a reply has been received for the CreateLampGroup method call
     *
     * @param responseCode   The response code
     * @param lampGroupID    The Lamp Group ID
     */
    virtual void CreateLampGroupReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     *  Indicates that the signal LampGroupsCreated has been received
     *
     *  @param lampGroupIDs   The Lamp Group IDs
     */
    virtual void LampGroupsCreatedCB(const LSFStringList& lampGroupIDs) { }

    /**
     * Indicates that a reply has been received for the GetLampGroup method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @parma lampGroup       The Lamp Group
     */
    virtual void GetLampGroupReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID, const LampGroup& lampGroup) { }

    /**
     * Indicates that a reply has been received for the DeleteLampGroup method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     */
    virtual void DeleteLampGroupReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     *  Indicates that the signal LampGroupsDeleted has been received
     *
     *  @param lampGroupIDs   The Lamp Group IDs
     */
    virtual void LampGroupsDeletedCB(const LSFStringList& lampGroupIDs) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupState method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     */
    virtual void TransitionLampGroupStateReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupState method call
     *
     * @param responseCode    The response code
     * @param lampID          The LampGroup ID
     */
    virtual void PulseLampGroupWithStateReplyCB(const LSFResponseCode& responseCode, const LSFString& lampID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupState method call
     *
     * @param responseCode    The response code
     * @param lampID          The LampGroup ID
     */
    virtual void StrobeLampGroupWithStateReplyCB(const LSFResponseCode& responseCode, const LSFString& lampID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupState method call
     *
     * @param responseCode    The response code
     * @param lampID          The LampGroup ID
     */
    virtual void CycleLampGroupWithStateReplyCB(const LSFResponseCode& responseCode, const LSFString& lampID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupPreset method call
     *
     * @param responseCode    The response code
     * @param lampID          The LampGroup ID
     */
    virtual void PulseLampGroupWithPresetReplyCB(const LSFResponseCode& responseCode, const LSFString& lampID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupPreset method call
     *
     * @param responseCode    The response code
     * @param lampID          The LampGroup ID
     */
    virtual void StrobeLampGroupWithPresetReplyCB(const LSFResponseCode& responseCode, const LSFString& lampID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupPreset method call
     *
     * @param responseCode    The response code
     * @param lampID          The LampGroup ID
     */
    virtual void CycleLampGroupWithPresetReplyCB(const LSFResponseCode& responseCode, const LSFString& lampID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void TransitionLampGroupStateOnOffFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void TransitionLampGroupStateHueFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void TransitionLampGroupStateSaturationFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void TransitionLampGroupStateBrightnessFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void TransitionLampGroupStateColorTempFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the ResetLampGroupState method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     */
    virtual void ResetLampGroupStateReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the ResetLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void ResetLampGroupStateOnOffFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the ResetLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void ResetLampGroupStateHueFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the ResetLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void ResetLampGroupStateSaturationFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the ResetLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void ResetLampGroupStateBrightnessFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the ResetLampGroupStateField method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     * @param stateFieldName
     */
    virtual void ResetLampGroupStateColorTempFieldReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that a reply has been received for the UpdateLampGroup method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     */
    virtual void UpdateLampGroupReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }

    /**
     * Indicates that the signal LampGroupsUpdated has been received
     *
     * @param lampGroupIDs    The Lamp Group IDs
     */
    virtual void LampGroupsUpdatedCB(const LSFStringList& lampGroupIDs) { }

    /**
     * Indicates that a reply has been received for the TransitionLampGroupStateToPreset method call
     *
     * @param responseCode    The response code
     * @param lampGroupID     The Lamp Group ID
     */
    virtual void TransitionLampGroupStateToPresetReplyCB(const LSFResponseCode& responseCode, const LSFString& lampGroupID) { }
};

/**
 * This class exposes the APIs that the User Application can use to manage
 * Lamp Groups
 */
class LampGroupManager : public Manager {

    friend class ControllerClient;

  public:

    /**
     * Constructor
     */
    LampGroupManager(ControllerClient& controller, LampGroupManagerCallback& callback);

    /**
     * Get the IDs of all the Lamp Groups
     * Response in LampGroupManagerCallback::GetAllLampGroupIDsReplyCB
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus GetAllLampGroupIDs(void);

    /**
     * Get the name of a Lamp Group
     * Response in LampGroupManagerCallback::GetLampGroupNameCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus GetLampGroupName(const LSFString& lampGroupID, const LSFString& language = LSFString("en"));

    /**
     * Set the name of the specified Lamp Group
     * Response in LampGroupManagerCallback::SetLampGroupNameReplyCB
     *
     * @param lampGroupID The Lamp Group ID
     * @param lampGroupName        The Lamp Group Name
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus SetLampGroupName(const LSFString& lampGroupID, const LSFString& lampGroupName, const LSFString& language = LSFString("en"));

    /**
     * Create a new Lamp Group
     * Response in LampGroupManagerCallback::CreateLampGroupReplyCB
     *
     * @param lampGroup   Lamp Group
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus CreateLampGroup(const LampGroup& lampGroup);

    /**
     * Modify a Lamp Group
     * Response in LampGroupManagerCallback::UpdateLampGroupReplyCB
     *
     * @param lampGroupID  The Lamp Group ID
     * @param lampGroup    Lamp Group
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus UpdateLampGroup(const LSFString& lampGroupID, const LampGroup& lampGroup);

    /**
     * Get the information about a Lamp Group
     * Response in LampGroupManagerCallback::GetLampGroupReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus GetLampGroup(const LSFString& lampGroupID);

    /**
     * Delete a Lamp Group
     * Response in LampGroupManagerCallback::DeleteLampGroupReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus DeleteLampGroup(const LSFString& lampGroupID);

    /**
     * Transition a Lamp Group to a new state
     * Response in LampGroupManagerCallback::TransitionLampGroupStateReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param lampGroupState The new state
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupState(const LSFString& lampGroupID, const LampState& lampGroupState, const uint32_t& transitionPeriod = 0);

    /**
     * Transition the LampGroup to a given state
     * Response in LampGroupManagerCallback::TransitionLampGroupStateReplyCB
     *
     * @param lampGroupID    The LampGroup id
     * @param lampGroupState The new LampGroup state
     */
    ControllerClientStatus PulseLampGroupWithState(const LSFString& lampGroupID, const LampState& fromLampGroupState, const LampState& toLampGroupState, const uint32_t& period, const uint32_t& duration, const uint32_t& numPulses);

    /**
     * Transition the LampGroup to a given state
     * Response in LampGroupManagerCallback::TransitionLampGroupStateReplyCB
     *
     * @param lampGroupID    The LampGroup id
     * @param lampGroupState The new LampGroup state
     */
    ControllerClientStatus StrobeLampGroupWithState(const LSFString& lampGroupID, const LampState& fromLampGroupState, const LampState& toLampGroupState, const uint32_t& period, const uint32_t& numStrobes);

    /**
     * Transition the LampGroup to a given state
     * Response in LampGroupManagerCallback::TransitionLampGroupStateReplyCB
     *
     * @param lampGroupID    The LampGroup id
     * @param lampGroupState The new LampGroup state
     */
    ControllerClientStatus CycleLampGroupWithState(const LSFString& lampGroupID, const LampState& lampGroupStateA, const LampState& lampGroupStateB, const uint32_t& period, const uint32_t& duration, const uint32_t& numCycles);

    /**
     * Transition the LampGroup to a given state
     * Response in LampGroupManagerCallback::TransitionLSFStringReplyCB
     *
     * @param lampGroupID    The LampGroup id
     * @param lampGroupPreset The new LampGroup state
     */
    ControllerClientStatus PulseLampGroupWithPreset(const LSFString& lampGroupID, const LSFString& fromPresetID, const LSFString& toPresetID, const uint32_t& period, const uint32_t& duration, const uint32_t& numPulses);

    /**
     * Transition the LampGroup to a given state
     * Response in LampGroupManagerCallback::TransitionLSFStringReplyCB
     *
     * @param lampGroupID    The LampGroup id
     * @param presetID The new LampGroup state
     */
    ControllerClientStatus StrobeLampGroupWithPreset(const LSFString& lampGroupID, const LSFString& fromPresetID, const LSFString& toPresetID, const uint32_t& period, const uint32_t& numStrobes);

    /**
     * Transition the LampGroup to a given state
     * Response in LampGroupManagerCallback::TransitionLSFStringReplyCB
     *
     * @param lampGroupID    The LampGroup id
     * @param presetID The new LampGroup state
     */
    ControllerClientStatus CycleLampGroupWithPreset(const LSFString& lampGroupID, const LSFString& presetIdA, const LSFString& presetIdB, const uint32_t& period, const uint32_t& duration, const uint32_t& numCycles);

    /**
     * Transition a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::TransitionLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupStateOnOffField(const LSFString& lampGroupID, const bool& onOff) {
        LSFString name("OnOff");
        return TransitionLampGroupStateBooleanField(lampGroupID, name, onOff);
    }

    /**
     * Transition a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::TransitionLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupStateHueField(const LSFString& lampGroupID, const uint32_t& hue, const uint32_t& transitionPeriod = 0) {
        LSFString name("Hue");
        return TransitionLampGroupStateIntegerField(lampGroupID, name, hue, transitionPeriod);
    }

    /**
     * Transition a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::TransitionLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupStateSaturationField(const LSFString& lampGroupID, const uint32_t& saturation, const uint32_t& transitionPeriod = 0) {
        LSFString name("Saturation");
        return TransitionLampGroupStateIntegerField(lampGroupID, name, saturation, transitionPeriod);
    }

    /**
     * Transition a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::TransitionLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupStateBrightnessField(const LSFString& lampGroupID, const uint32_t& brightness, const uint32_t& transitionPeriod = 0) {
        LSFString name("Brightness");
        return TransitionLampGroupStateIntegerField(lampGroupID, name, brightness, transitionPeriod);
    }

    /**
     * Transition a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::TransitionLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupStateColorTempField(const LSFString& lampGroupID, const uint32_t& colorTemp, const uint32_t& transitionPeriod = 0) {
        LSFString name("ColorTemp");
        return TransitionLampGroupStateIntegerField(lampGroupID, name, colorTemp, transitionPeriod);
    }

    /**
     * Transition a Lamp Group to a given Preset
     * Response in LampGroupManagerCallback::TransitionLampGroupStateToPresetReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param presetID   The Preset ID
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus TransitionLampGroupStateToPreset(const LSFString& lampGroupID, const LSFString& presetID, const uint32_t& transitionPeriod = 0);

    /**
     * Reset a Lamp Group's state
     * Response in LampGroupManagerCallback::ResetLampGroupStateReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus ResetLampGroupState(const LSFString& lampGroupID);

    /**
     * Reset a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::ResetLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus ResetLampGroupStateOnOffField(const LSFString& lampGroupID) {
        LSFString name("OnOff");
        return ResetLampGroupStateField(lampGroupID, name);
    }

    /**
     * Reset a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::ResetLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus ResetLampGroupStateHueField(const LSFString& lampGroupID) {
        LSFString name("Hue");
        return ResetLampGroupStateField(lampGroupID, name);
    }

    /**
     * Reset a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::ResetLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus ResetLampGroupStateSaturationField(const LSFString& lampGroupID) {
        LSFString name("Saturation");
        return ResetLampGroupStateField(lampGroupID, name);
    }

    /**
     * Reset a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::ResetLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus ResetLampGroupStateBrightnessField(const LSFString& lampGroupID) {
        LSFString name("Brightness");
        return ResetLampGroupStateField(lampGroupID, name);
    }

    /**
     * Reset a Lamp Group's field to a given value
     * Response in LampGroupManagerCallback::ResetLampGroupStateFieldReplyCB
     *
     * @param lampGroupID    The Lamp Group ID
     * @param stateFieldName The name of the state stateFieldName to transition
     * @param value          The new value of the state stateFieldName
     * @return
     *      - CONTROLLER_CLIENT_OK if successful
     *      - An error status otherwise
     *
     */
    ControllerClientStatus ResetLampGroupStateColorTempField(const LSFString& lampGroupID) {
        LSFString name("ColorTemp");
        return ResetLampGroupStateField(lampGroupID, name);
    }

  private:

    ControllerClientStatus TransitionLampGroupStateIntegerField(const LSFString& lampGroupID, const LSFString& stateFieldName, const uint32_t& value, const uint32_t& transitionPeriod = 0);
    ControllerClientStatus TransitionLampGroupStateBooleanField(const LSFString& lampGroupID, const LSFString& stateFieldName, const bool& value);
    ControllerClientStatus ResetLampGroupStateField(const LSFString& lampGroupID, const LSFString& stateFieldName);

    /**
     * Handler for the signal LampGroupsNameChanged
     */
    void LampGroupsNameChanged(LSFStringList& idList) {
        callback.LampGroupsNameChangedCB(idList);
    }

    /**
     * Handler for the signal LampGroupsCreated
     */
    void LampGroupsCreated(LSFStringList& idList) {
        callback.LampGroupsCreatedCB(idList);
    }

    /**
     * Handler for the signal LampGroupsDeleted
     */
    void LampGroupsDeleted(LSFStringList& idList) {
        callback.LampGroupsDeletedCB(idList);
    }

    /**
     * Handler for the signal LampGroupsUpdated
     */
    void LampGroupsUpdated(LSFStringList& idList) {
        callback.LampGroupsUpdatedCB(idList);
    }
    /**
     * Method Reply Handler for the signal GetAllLampGroupIDs
     */
    void GetAllLampGroupIDsReply(LSFResponseCode& responseCode, LSFStringList& idList) {
        callback.GetAllLampGroupIDsReplyCB(responseCode, idList);
    }

    /**
     * Method Reply Handler for the signal GetLampGroupName
     */
    void GetLampGroupNameReply(LSFResponseCode& responseCode, LSFString& lsfId, LSFString& language, LSFString& lsfName) {
        callback.GetLampGroupNameReplyCB(responseCode, lsfId, language, lsfName);
    }

    /**
     * Method Reply Handler for the signal SetLampGroupName
     */
    void SetLampGroupNameReply(LSFResponseCode& responseCode, LSFString& lsfId, LSFString& language) {
        callback.SetLampGroupNameReplyCB(responseCode, lsfId, language);
    }

    /**
     * Method Reply Handler for the signal CreateLampGroup
     */
    void CreateLampGroupReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.CreateLampGroupReplyCB(responseCode, lsfId);
    }

    /**
     * Method Reply Handler for the signal UpdateLampGroup
     */
    void UpdateLampGroupReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.UpdateLampGroupReplyCB(responseCode, lsfId);
    }

    /**
     * Method Reply Handler for the signal GetLampGroup
     */
    void GetLampGroupReply(ajn::Message& message);

    /**
     * Method Reply Handler for the signal DeleteLampGroup
     */
    void DeleteLampGroupReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.DeleteLampGroupReplyCB(responseCode, lsfId);
    }

    /**
     * Method Reply Handler for the signal ResetLampGroupFieldState
     */
    void ResetLampGroupFieldStateReply(LSFResponseCode& responseCode, LSFString& lsfId, LSFString& lsfName);

    /**
     * Method Reply Handler for the signal ResetLampGroupState
     */
    void ResetLampGroupStateReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.ResetLampGroupStateReplyCB(responseCode, lsfId);
    }

    /**
     * Method Reply Handler for the signal TransitionLampGroupState
     */
    void TransitionLampGroupStateReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.TransitionLampGroupStateReplyCB(responseCode, lsfId);
    }

    void PulseLampGroupWithStateReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.PulseLampGroupWithStateReplyCB(responseCode, lsfId);
    }

    void StrobeLampGroupWithStateReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.StrobeLampGroupWithStateReplyCB(responseCode, lsfId);
    }

    void CycleLampGroupWithStateReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.CycleLampGroupWithStateReplyCB(responseCode, lsfId);
    }

    void PulseLampGroupWithPresetReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.PulseLampGroupWithPresetReplyCB(responseCode, lsfId);
    }

    void StrobeLampGroupWithPresetReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.StrobeLampGroupWithPresetReplyCB(responseCode, lsfId);
    }

    void CycleLampGroupWithPresetReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.CycleLampGroupWithPresetReplyCB(responseCode, lsfId);
    }

    /**
     * Method Reply Handler for the signal TransitionLampGroupStateField
     */
    void TransitionLampGroupStateFieldReply(LSFResponseCode& responseCode, LSFString& lsfId, LSFString& lsfName);

    /**
     * Method Reply Handler for the signal TransitionLampGroupStateToPreset
     */
    void TransitionLampGroupStateToPresetReply(LSFResponseCode& responseCode, LSFString& lsfId) {
        callback.TransitionLampGroupStateToPresetReplyCB(responseCode, lsfId);
    }

    /**
     * Callback used to send Lamp Group specific signals and method replies
     * to the User Application
     */
    LampGroupManagerCallback&   callback;
};


}

#endif
