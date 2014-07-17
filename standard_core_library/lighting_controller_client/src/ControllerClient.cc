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

#include <algorithm>

#include <alljoyn/about/AnnounceHandler.h>
#include <alljoyn/about/AnnouncementRegistrar.h>
#include <alljoyn/Status.h>
#include <qcc/Debug.h>

#include <ControllerClient.h>

using namespace qcc;
using namespace ajn;

#define QCC_MODULE "CONTROLLER_CLIENT"

namespace lsf {

/**
 * Controller Service Object Path
 */

/**
 * Handler class for some standard AllJoyn signals and callbacks
 */
class ControllerClient::ControllerClientBusHandler :
    public SessionListener, public BusAttachment::JoinSessionAsyncCB,
    public services::AnnounceHandler {

  public:
    /**
     * Constructor
     */
    ControllerClientBusHandler(ControllerClient& client) : controllerClient(client) { }

    /**
     * Destructor
     */
    virtual ~ControllerClientBusHandler() { }

    /**
     * Session Lost signal handler
     */
    virtual void SessionLost(SessionId sessionId, SessionLostReason reason);

    /**
     * JoinSession callback handler
     */
    virtual void JoinSessionCB(QStatus status, SessionId sessionId, const SessionOpts& opts, void* context);

    /**
     * Announce signal handler
     */
    virtual void Announce(uint16_t version, uint16_t port, const char* busName, const ObjectDescriptions& objectDescs, const AboutData& aboutData);

    virtual void SessionMemberRemoved(SessionId sessionId, const char* uniqueName);

  private:

    /**
     * Reference to the Controller Client instance
     */
    ControllerClient& controllerClient;
};


void ControllerClient::ControllerClientBusHandler::JoinSessionCB(QStatus status, SessionId sessionId, const SessionOpts& opts, void* context)
{
    controllerClient.bus.EnableConcurrentCallbacks();
    QCC_DbgPrintf(("%s", __func__));
    controllerClient.OnSessionJoined(status, sessionId, context);
}

void ControllerClient::ControllerClientBusHandler::SessionLost(SessionId sessionId, SessionLostReason reason)
{
    controllerClient.bus.EnableConcurrentCallbacks();
    QCC_DbgPrintf(("SessionLost(%u)", sessionId));
    controllerClient.OnSessionLost(sessionId);
}

void ControllerClient::ControllerClientBusHandler::SessionMemberRemoved(SessionId sessionId, const char* uniqueName)
{
    controllerClient.bus.EnableConcurrentCallbacks();
    QCC_DbgPrintf(("SessionMemberRemoved(%u,%s)", sessionId, uniqueName));
    controllerClient.OnSessionMemberRemoved(sessionId, uniqueName);
}

void ControllerClient::ControllerClientBusHandler::Announce(
    uint16_t version,
    SessionPort port,
    const char* busName,
    const ObjectDescriptions& objectDescs,
    const AboutData& aboutData)
{
    QCC_DbgPrintf(("%s", __func__));

    AboutData::const_iterator ait;
    const char* deviceID = NULL;
    const char* deviceName = NULL;

    controllerClient.bus.EnableConcurrentCallbacks();

    ObjectDescriptions::const_iterator oit = objectDescs.find(ControllerServiceObjectPath);
    if (oit != objectDescs.end()) {
        QCC_DbgPrintf(("%s: About Data Dump", __func__));
        for (ait = aboutData.begin(); ait != aboutData.end(); ait++) {
            QCC_DbgPrintf(("%s: %s", ait->first.c_str(), ait->second.ToString().c_str()));
        }

        ait = aboutData.find("DeviceId");
        if (ait == aboutData.end()) {
            QCC_LogError(ER_FAIL, ("%s: DeviceId missing in About Announcement", __func__));
            return;
        }
        ait->second.Get("s", &deviceID);

        ait = aboutData.find("DeviceName");
        if (ait == aboutData.end()) {
            QCC_LogError(ER_FAIL, ("%s: DeviceName missing in About Announcement", __func__));
            return;
        }
        ait->second.Get("s", &deviceName);

        uint64_t rank;
        ait = aboutData.find("Rank");
        if (ait == aboutData.end()) {
            QCC_LogError(ER_FAIL, ("%s: Rank missing in About Announcement", __func__));
            return;
        }
        ait->second.Get("t", &rank);

        bool isLeader;
        ait = aboutData.find("IsLeader");
        if (ait == aboutData.end()) {
            QCC_LogError(ER_FAIL, ("%s: IsLeader missing in About Announcement", __func__));
            return;
        }
        ait->second.Get("b", &isLeader);

        QCC_DbgPrintf(("%s: Received Announce: busName=%s port=%u deviceID=%s deviceName=%s rank=%llu isLeader=%d", __func__,
                       busName, port, deviceID, deviceName, rank, isLeader));
        if (isLeader) {
            controllerClient.OnAnnounced(port, busName, deviceID, deviceName, rank);
        } else {
            QCC_DbgPrintf(("%s: Received a non-leader announcement", __func__));
        }
    }
}

static const char* interfaces[] =
{
    ControllerServiceInterfaceName,
    ControllerServiceLampInterfaceName,
    ControllerServiceLampGroupInterfaceName,
    ControllerServicePresetInterfaceName,
    ControllerServiceSceneInterfaceName,
    ControllerServiceMasterSceneInterfaceName,
    ConfigServiceInterfaceName,
    AboutInterfaceName
};

ControllerClient::ControllerClient(
    ajn::BusAttachment& bus,
    ControllerClientCallback& clientCB) :
    bus(bus),
    busHandler(new ControllerClientBusHandler(*this)),
    callback(clientCB),
    controllerServiceManagerPtr(NULL),
    lampManagerPtr(NULL),
    lampGroupManagerPtr(NULL),
    presetManagerPtr(NULL),
    sceneManagerPtr(NULL),
    masterSceneManagerPtr(NULL)
{
    ClearCurrentLeader();
    QStatus status = services::AnnouncementRegistrar::RegisterAnnounceHandler(bus, *busHandler, interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    QCC_DbgPrintf(("services::AnnouncementRegistrar::RegisterAnnounceHandler: %s\n", QCC_StatusText(status)));
}

ControllerClient::~ControllerClient()
{
    services::AnnouncementRegistrar::UnRegisterAnnounceHandler(bus, *busHandler, interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    delete busHandler;
}

uint32_t ControllerClient::GetVersion(void)
{
    QCC_DbgPrintf(("%s", __func__));
    return CONTROLLER_CLIENT_VERSION;
}

void ControllerClient::ClearCurrentLeader()
{
    currentLeader.busName.clear();
    currentLeader.rank = 0;
    currentLeader.port = 0;
    currentLeader.sessionId = 0;
    currentLeader.deviceID.clear();
    currentLeader.deviceName.clear();
}

void ControllerClient::OnSessionMemberRemoved(ajn::SessionId sessionId, const char* uniqueName)
{
    controllerLock.Lock();
    if ((currentLeader.busName == uniqueName) && (currentLeader.sessionId == sessionId)) {
        OnSessionLost(sessionId);
    } else {
        QCC_DbgPrintf(("%s: Ignoring spurious OnSessionMemberRemoved", __func__));
    }
    controllerLock.Unlock();
}


void ControllerClient::OnSessionLost(ajn::SessionId sessionID)
{
    QCC_DbgPrintf(("OnSessionLost(%u)\n", sessionID));

    LSFString deviceName;
    LSFString deviceID;

    deviceName.clear();
    deviceID.clear();

    controllerLock.Lock();
    if (currentLeader.sessionId == sessionID) {
        deviceName = currentLeader.deviceName;
        deviceID = currentLeader.deviceID;
        ClearCurrentLeader();
    }
    controllerLock.Unlock();

    if (!deviceID.empty()) {
        callback.DisconnectedFromControllerServiceCB(deviceID, deviceName);
    }
}

void ControllerClient::SignalWithArgDispatcher(const ajn::InterfaceDescription::Member* member, const char* sourcePath, ajn::Message& message)
{
    bus.EnableConcurrentCallbacks();

    QCC_DbgPrintf(("%s: Received Signal %s", __func__, message->GetMemberName()));

    SignalDispatcherMap::iterator it = signalHandlers.find(message->GetMemberName());
    if (it != signalHandlers.end()) {
        SignalHandlerBase* handler = it->second;

        size_t numInputArgs;
        const MsgArg* inputArgs;
        message->GetArgs(numInputArgs, inputArgs);

        LSFStringList idList;

        MsgArg* idArgs;
        size_t numIds;
        inputArgs[0].Get("as", &numIds, &idArgs);

        for (size_t i = 0; i < numIds; ++i) {
            char* tempId;
            idArgs[i].Get("s", &tempId);
            idList.push_back(LSFString(tempId));
        }

        handler->Handle(idList);
    }
}

void ControllerClient::SignalWithoutArgDispatcher(const ajn::InterfaceDescription::Member* member, const char* sourcePath, ajn::Message& message)
{
    bus.EnableConcurrentCallbacks();

    QCC_DbgPrintf(("%s: Received Signal %s", __func__, message->GetMemberName()));

    NoArgSignalDispatcherMap::iterator it = noArgSignalHandlers.find(message->GetMemberName());
    if (it != noArgSignalHandlers.end()) {
        NoArgSignalHandlerBase* handler = it->second;
        handler->Handle();
    }
}

void ControllerClient::OnSessionJoined(QStatus status, ajn::SessionId sessionId, void* context)
{
    ErrorCodeList errorList;

    bus.EnableConcurrentCallbacks();

    ControllerEntry* joined = static_cast<ControllerEntry*>(context);

    LSFString deviceName;
    deviceName.clear();

    if (joined) {
        QCC_DbgPrintf(("%s: sessionId= %u status=%s\n", __func__, sessionId, QCC_StatusText(status)));
        controllerLock.Lock();

        /*
         * Check to see if this is a response from a Controller Service that is in our ignore list
         */
        if (ignoreList.find(joined->deviceID) != ignoreList.end()) {
            QCC_DbgPrintf(("%s: Ignoring Join Session response from Controller Service %s", __func__, joined->deviceID.c_str()));
            ignoreList.erase(joined->deviceID);
        } else {
            if (joined->deviceID == currentLeader.deviceID) {
                QCC_DbgPrintf(("%s: Response from the current leader", __func__));
                if (status == ER_OK) {
                    currentLeader.proxyObject = ProxyBusObject(bus, currentLeader.busName.c_str(), ControllerServiceObjectPath, sessionId);
                    currentLeader.proxyObject.IntrospectRemoteObject();
                    currentLeader.sessionId = sessionId;
                    AddMethodHandlers();
                    AddSignalHandlers();
                }
                /*
                 * This is to account for the case when the device name of Controller Service changes when a
                 * Join Session with the Controller Service is in progress
                 */
                deviceName = currentLeader.deviceName;
            }
        }
        controllerLock.Unlock();

        if (!deviceName.empty()) {
            if (ER_OK == status) {
                callback.ConnectedToControllerServiceCB(joined->deviceID, deviceName);
            } else {
                callback.ConnectToControllerServiceFailedCB(joined->deviceID, deviceName);
            }
        }

        delete joined;
    }
}

void ControllerClient::OnAnnounced(SessionPort port, const char* busName, const char* deviceID, const char* deviceName, uint64_t rank)
{
    QCC_DbgPrintf(("%s: port=%u, busName=%s, deviceID=%s, deviceName=%s, rank=%lu", __func__, port, busName, deviceID, deviceName, rank));

    bool nameChanged = false;
    bool connectToNewLeader = false;
    LSFString oldDeviceId;
    LSFString oldDeviceName;
    SessionId oldSessionId = 0;

    oldDeviceId.clear();
    oldDeviceName.clear();

    controllerLock.Lock();
    // if the name of the current CS has changed...
    if (deviceID == currentLeader.deviceID) {
        if (deviceName != currentLeader.deviceName) {
            currentLeader.deviceName = deviceName;
            if (currentLeader.sessionId != 0) {
                nameChanged = true;
            }
            QCC_DbgPrintf(("%s: Current leader name change", __func__));
        }
    } else {
        if (rank > currentLeader.rank) {
            connectToNewLeader = true;

            ControllerEntry entry;
            entry.port = port;
            entry.deviceID = deviceID;
            entry.deviceName = deviceName;
            entry.rank = rank;
            entry.busName = busName;
            entry.sessionId = 0;

            if (!currentLeader.busName.empty()) {
                if (currentLeader.sessionId == 0) {
                    QCC_DbgPrintf(("%s: Pushed current leader in ignore list", __func__));
                    ignoreList.insert(currentLeader.deviceID);
                } else {
                    oldSessionId = currentLeader.sessionId;
                    oldDeviceId = currentLeader.deviceID;
                    oldDeviceName = currentLeader.deviceName;
                }
            }
            currentLeader = entry;
            QCC_DbgPrintf(("%s: Updated new current leader entry", __func__));
        } else {
            QCC_DbgPrintf(("%s: Ignoring a lower ranking leader accouncement than current leader", __func__));
        }
    }
    controllerLock.Unlock();

    if (nameChanged) {
        controllerServiceManagerPtr->callback.ControllerServiceNameChangedCB();
    } else if (connectToNewLeader) {
        if (oldSessionId) {
            QCC_DbgPrintf(("%s: Tearing down old session", __func__));
            bus.LeaveSession(oldSessionId);
            callback.DisconnectedFromControllerServiceCB(oldDeviceId, oldDeviceName);
        }

        QCC_DbgPrintf(("%s: Joining session with the new leader", __func__));
        SessionOpts opts;
        opts.isMultipoint = true;
        ControllerEntry* context = new ControllerEntry();
        context->port = port;
        context->deviceID = deviceID;
        context->deviceName = deviceName;
        context->rank = rank;
        context->busName = busName;
        context->sessionId = 0;
        QStatus status = bus.JoinSessionAsync(busName, port, busHandler, opts, busHandler, context);
        if (status != ER_OK) {
            QCC_LogError(status, ("%s: JoinSessionAsync failed", __func__));
            delete context;
            callback.ConnectToControllerServiceFailedCB(deviceID, deviceName);
        }
    }
}

ControllerClientStatus ControllerClient::MethodCallAsyncHelper(
    const char* ifaceName,
    const char* methodName,
    ajn::MessageReceiver* handler,
    ajn::MessageReceiver::ReplyHandler callback,
    const ajn::MsgArg* args,
    size_t numArgs,
    void* context)
{
    ControllerClientStatus status = CONTROLLER_CLIENT_OK;
    controllerLock.Lock();

    if (currentLeader.sessionId) {
        QStatus ajStatus = currentLeader.proxyObject.MethodCallAsync(
            ifaceName,
            methodName,
            handler,
            callback,
            args,
            numArgs,
            context);
        if (ajStatus != ER_OK) {
            status = CONTROLLER_CLIENT_ERR_FAILURE;
            QCC_LogError(ajStatus, ("%s method call to Controller Service failed", methodName));
        }
    } else {
        // this is no longer available
        status = CONTROLLER_CLIENT_ERR_NOT_CONNECTED;
    }

    controllerLock.Unlock();
    return status;
}

ControllerClientStatus ControllerClient::MethodCallAsyncForReplyWithResponseCodeAndListOfIDs(
    const char* ifaceName,
    const char* methodName,
    const ajn::MsgArg* args,
    size_t numArgs)
{
    QCC_DbgPrintf(("%s: Method Call=%s", __func__, methodName));
    LSFString* methodNameContext = new LSFString(methodName);
    ControllerClientStatus status = MethodCallAsyncHelper(
        ifaceName,
        methodName,
        this,
        static_cast<ajn::MessageReceiver::ReplyHandler>(&ControllerClient::HandlerForMethodReplyWithResponseCodeAndListOfIDs),
        args,
        numArgs,
        (void*)methodNameContext);
    if (status != CONTROLLER_CLIENT_OK) {
        delete methodNameContext;
    }
    return status;
}

void ControllerClient::HandlerForMethodReplyWithResponseCodeAndListOfIDs(Message& message, void* context)
{
    if (context) {
        QCC_DbgPrintf(("%s: Method Reply for %s:%s", __func__, ((LSFString*)context)->c_str(), (MESSAGE_METHOD_RET == message->GetType()) ? message->ToString().c_str() : "ERROR"));
        bus.EnableConcurrentCallbacks();

        if (message->GetType() == ajn::MESSAGE_METHOD_RET) {
            MethodReplyWithResponseCodeAndListOfIDsDispatcherMap::iterator it = methodReplyWithResponseCodeAndListOfIDsHandlers.find(*((LSFString*)context));
            if (it != methodReplyWithResponseCodeAndListOfIDsHandlers.end()) {
                MethodReplyWithResponseCodeAndListOfIDsHandlerBase* handler = it->second;

                size_t numInputArgs;
                const MsgArg* inputArgs;
                message->GetArgs(numInputArgs, inputArgs);

                LSFStringList idList;
                LSFResponseCode responseCode;

                inputArgs[0].Get("u", &responseCode);

                MsgArg* idArgs;
                size_t numIds;
                inputArgs[1].Get("as", &numIds, &idArgs);

                for (size_t i = 0; i < numIds; ++i) {
                    char* tempId;
                    idArgs[i].Get("s", &tempId);
                    idList.push_back(LSFString(tempId));
                }

                handler->Handle(responseCode, idList);
            }
        } else {
            ErrorCodeList errorList;
            errorList.push_back(ERROR_ALLJOYN_METHOD_CALL_TIMEOUT);
            callback.ControllerClientErrorCB(errorList);
        }

        delete ((LSFString*)context);
    } else {
        QCC_LogError(ER_FAIL, ("%s: Received a NULL context in method reply", __func__));
    }
}

ControllerClientStatus ControllerClient::MethodCallAsyncForReplyWithResponseCodeIDAndName(
    const char* ifaceName,
    const char* methodName,
    const ajn::MsgArg* args,
    size_t numArgs)
{
    QCC_DbgPrintf(("%s: Method Call=%s", __func__, methodName));
    LSFString* methodNameContext = new LSFString(methodName);
    ControllerClientStatus status = MethodCallAsyncHelper(
        ifaceName,
        methodName,
        this,
        static_cast<ajn::MessageReceiver::ReplyHandler>(&ControllerClient::HandlerForMethodReplyWithResponseCodeIDAndName),
        args,
        numArgs,
        (void*)methodNameContext);
    if (status != CONTROLLER_CLIENT_OK) {
        delete methodNameContext;
    }
    return status;
}

void ControllerClient::HandlerForMethodReplyWithResponseCodeIDAndName(Message& message, void* context)
{
    if (context) {
        QCC_DbgPrintf(("%s: Method Reply for %s:%s", __func__, ((LSFString*)context)->c_str(), (MESSAGE_METHOD_RET == message->GetType()) ? message->ToString().c_str() : "ERROR"));
        bus.EnableConcurrentCallbacks();

        if (message->GetType() == ajn::MESSAGE_METHOD_RET) {
            MethodReplyWithResponseCodeIDAndNameDispatcherMap::iterator it = methodReplyWithResponseCodeIDAndNameHandlers.find(*((LSFString*)context));
            if (it != methodReplyWithResponseCodeIDAndNameHandlers.end()) {
                MethodReplyWithResponseCodeIDAndNameHandlerBase* handler = it->second;

                size_t numInputArgs;
                const MsgArg* inputArgs;
                message->GetArgs(numInputArgs, inputArgs);

                char* uniqueId;
                char* name;
                LSFResponseCode responseCode;

                inputArgs[0].Get("u", &responseCode);
                inputArgs[1].Get("s", &uniqueId);
                inputArgs[2].Get("s", &name);

                LSFString lsfId = LSFString(uniqueId);
                LSFString lsfName = LSFString(name);

                handler->Handle(responseCode, lsfId, lsfName);
            }
        } else {
            ErrorCodeList errorList;
            errorList.push_back(ERROR_ALLJOYN_METHOD_CALL_TIMEOUT);
            callback.ControllerClientErrorCB(errorList);
        }

        delete ((LSFString*)context);
    } else {
        QCC_LogError(ER_FAIL, ("%s: Received a NULL context in method reply", __func__));
    }
}

ControllerClientStatus ControllerClient::MethodCallAsyncForReplyWithResponseCodeAndID(
    const char* ifaceName,
    const char* methodName,
    const ajn::MsgArg* args,
    size_t numArgs)
{
    QCC_DbgPrintf(("%s: Method Call=%s", __func__, methodName));
    LSFString* methodNameContext = new LSFString(methodName);
    ControllerClientStatus status = MethodCallAsyncHelper(
        ifaceName,
        methodName,
        this,
        static_cast<ajn::MessageReceiver::ReplyHandler>(&ControllerClient::HandlerForMethodReplyWithResponseCodeAndID),
        args,
        numArgs,
        (void*)methodNameContext);
    if (status != CONTROLLER_CLIENT_OK) {
        delete methodNameContext;
    }
    return status;
}

void ControllerClient::HandlerForMethodReplyWithResponseCodeAndID(Message& message, void* context)
{
    if (context) {
        QCC_DbgPrintf(("%s: Method Reply for %s:%s", __func__, ((LSFString*)context)->c_str(), (MESSAGE_METHOD_RET == message->GetType()) ? message->ToString().c_str() : "ERROR"));
        bus.EnableConcurrentCallbacks();

        if (message->GetType() == ajn::MESSAGE_METHOD_RET) {
            MethodReplyWithResponseCodeAndIDDispatcherMap::iterator it = methodReplyWithResponseCodeAndIDHandlers.find(*((LSFString*)context));
            if (it != methodReplyWithResponseCodeAndIDHandlers.end()) {
                MethodReplyWithResponseCodeAndIDHandlerBase* handler = it->second;

                size_t numInputArgs;
                const MsgArg* inputArgs;
                message->GetArgs(numInputArgs, inputArgs);

                char* id;
                LSFResponseCode responseCode;

                inputArgs[0].Get("u", &responseCode);
                inputArgs[1].Get("s", &id);

                LSFString lsfId = LSFString(id);

                handler->Handle(responseCode, lsfId);
            }
        } else {
            ErrorCodeList errorList;
            errorList.push_back(ERROR_ALLJOYN_METHOD_CALL_TIMEOUT);
            callback.ControllerClientErrorCB(errorList);
        }

        delete ((LSFString*)context);
    } else {
        QCC_LogError(ER_FAIL, ("%s: Received a NULL context in method reply", __func__));
    }
}

ControllerClientStatus ControllerClient::MethodCallAsyncForReplyWithUint32Value(
    const char* ifaceName,
    const char* methodName,
    const ajn::MsgArg* args,
    size_t numArgs)
{
    QCC_DbgPrintf(("%s: Method Call=%s", __func__, methodName));
    LSFString* methodNameContext = new LSFString(methodName);
    ControllerClientStatus status = MethodCallAsyncHelper(
        ifaceName,
        methodName,
        this,
        static_cast<ajn::MessageReceiver::ReplyHandler>(&ControllerClient::HandlerForMethodReplyWithUint32Value),
        args,
        numArgs,
        (void*)methodNameContext);
    if (status != CONTROLLER_CLIENT_OK) {
        delete methodNameContext;
    }
    return status;
}

void ControllerClient::HandlerForMethodReplyWithUint32Value(Message& message, void* context)
{
    if (context) {
        QCC_DbgPrintf(("%s: Method Reply for %s:%s", __func__, ((LSFString*)context)->c_str(), (MESSAGE_METHOD_RET == message->GetType()) ? message->ToString().c_str() : "ERROR"));
        bus.EnableConcurrentCallbacks();

        if (message->GetType() == ajn::MESSAGE_METHOD_RET) {
            MethodReplyWithUint32ValueDispatcherMap::iterator it = methodReplyWithUint32ValueHandlers.find(*((LSFString*)context));
            if (it != methodReplyWithUint32ValueHandlers.end()) {
                MethodReplyWithUint32ValueHandlerBase* handler = it->second;

                size_t numInputArgs;
                const MsgArg* inputArgs;
                message->GetArgs(numInputArgs, inputArgs);

                uint32_t value;
                inputArgs[0].Get("u", &value);
                handler->Handle(value);
            } else {
                QCC_LogError(ER_FAIL, ("%s: Did not find handler", __func__));
            }
        } else {
            ErrorCodeList errorList;
            errorList.push_back(ERROR_ALLJOYN_METHOD_CALL_TIMEOUT);
            callback.ControllerClientErrorCB(errorList);
        }

        delete ((LSFString*)context);
    } else {
        QCC_LogError(ER_FAIL, ("%s: Received a NULL context in method reply", __func__));
    }
}

ControllerClientStatus ControllerClient::MethodCallAsyncForReplyWithResponseCodeIDLanguageAndName(
    const char* ifaceName,
    const char* methodName,
    const ajn::MsgArg* args,
    size_t numArgs)
{
    QCC_DbgPrintf(("%s: Method Call=%s", __func__, methodName));
    LSFString* methodNameContext = new LSFString(methodName);
    ControllerClientStatus status = MethodCallAsyncHelper(
        ifaceName,
        methodName,
        this,
        static_cast<ajn::MessageReceiver::ReplyHandler>(&ControllerClient::HandlerForMethodReplyWithResponseCodeIDLanguageAndName),
        args,
        numArgs,
        (void*)methodNameContext);
    if (status != CONTROLLER_CLIENT_OK) {
        delete methodNameContext;
    }
    return status;
}

void ControllerClient::HandlerForMethodReplyWithResponseCodeIDLanguageAndName(Message& message, void* context)
{
    if (context) {
        QCC_DbgPrintf(("%s: Method Reply for %s:%s", __func__, ((LSFString*)context)->c_str(), (MESSAGE_METHOD_RET == message->GetType()) ? message->ToString().c_str() : "ERROR"));
        bus.EnableConcurrentCallbacks();

        if (message->GetType() == ajn::MESSAGE_METHOD_RET) {
            MethodReplyWithResponseCodeIDLanguageAndNameDispatcherMap::iterator it = methodReplyWithResponseCodeIDLanguageAndNameHandlers.find(*((LSFString*)context));
            if (it != methodReplyWithResponseCodeIDLanguageAndNameHandlers.end()) {
                MethodReplyWithResponseCodeIDLanguageAndNameHandlerBase* handler = it->second;

                size_t numInputArgs;
                const MsgArg* inputArgs;
                message->GetArgs(numInputArgs, inputArgs);

                LSFResponseCode responseCode;
                char* id;
                char* lang;
                char* name;

                inputArgs[0].Get("u", &responseCode);
                inputArgs[1].Get("s", &id);
                inputArgs[2].Get("s", &lang);
                inputArgs[3].Get("s", &name);

                LSFString lsfId = LSFString(id);
                LSFString language = LSFString(lang);
                LSFString lsfName = LSFString(name);

                handler->Handle(responseCode, lsfId, language, lsfName);
            } else {
                QCC_LogError(ER_FAIL, ("%s: Did not find handler", __func__));
            }
        } else {
            ErrorCodeList errorList;
            errorList.push_back(ERROR_ALLJOYN_METHOD_CALL_TIMEOUT);
            callback.ControllerClientErrorCB(errorList);
        }

        delete ((LSFString*)context);
    } else {
        QCC_LogError(ER_FAIL, ("%s: Received a NULL context in method reply", __func__));
    }
}

void ControllerClient::Reset(void)
{
    QCC_DbgPrintf(("%s", __func__));

    LSFString deviceName;
    LSFString deviceID;
    SessionId sessionId = 0;

    deviceName.clear();
    deviceID.clear();

    controllerLock.Lock();
    sessionId = currentLeader.sessionId;
    deviceName = currentLeader.deviceName;
    deviceID = currentLeader.deviceID;
    ClearCurrentLeader();
    controllerLock.Unlock();

    if (sessionId) {
        bus.LeaveSession(sessionId);
        callback.DisconnectedFromControllerServiceCB(deviceID, deviceName);
    }

    QStatus status = services::AnnouncementRegistrar::UnRegisterAnnounceHandler(bus, *busHandler, interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    if (ER_OK == status) {
        status = services::AnnouncementRegistrar::RegisterAnnounceHandler(bus, *busHandler, interfaces, sizeof(interfaces) / sizeof(interfaces[0]));
    }

    if (status != ER_OK) {
        ErrorCodeList errors;
        errors.push_back(ERROR_IRRECOVERABLE);
        callback.ControllerClientErrorCB(errors);
    }
}

void ControllerClient::AddMethodHandlers()
{
    //methodReplyWithResponseCodeAndListOfIDsHandlers.clear();

    if (controllerServiceManagerPtr) {
        AddNoArgSignalHandler("ControllerServiceLightingReset", controllerServiceManagerPtr, &ControllerServiceManager::ControllerServiceLightingReset);

        AddMethodReplyWithUint32ValueHandler("GetControllerServiceVersion", controllerServiceManagerPtr, &ControllerServiceManager::GetControllerServiceVersionReply);
        AddMethodReplyWithUint32ValueHandler("LightingResetControllerService", controllerServiceManagerPtr, &ControllerServiceManager::LightingResetControllerServiceReply);
    }

    if (lampManagerPtr) {
        AddSignalHandler("LampsNameChanged", lampManagerPtr, &LampManager::LampsNameChanged);
        AddSignalHandler("LampsStateChanged", lampManagerPtr, &LampManager::LampsStateChanged);

        AddMethodReplyWithResponseCodeAndListOfIDsHandler("GetAllLampIDs", lampManagerPtr, &LampManager::GetAllLampIDsReply);

        AddMethodReplyWithResponseCodeIDAndNameHandler("TransitionLampStateField", lampManagerPtr, &LampManager::TransitionLampStateFieldReply);
        AddMethodReplyWithResponseCodeIDAndNameHandler("ResetLampStateField", lampManagerPtr, &LampManager::ResetLampStateFieldReply);
        AddMethodReplyWithResponseCodeIDAndNameHandler("SetLampName", lampManagerPtr, &LampManager::SetLampNameReply);

        AddMethodReplyWithResponseCodeIDLanguageAndNameHandler("GetLampName", lampManagerPtr, &LampManager::GetLampNameReply);
        AddMethodReplyWithResponseCodeIDLanguageAndNameHandler("GetLampManufacturer", lampManagerPtr, &LampManager::GetLampManufacturerReply);

        AddMethodReplyWithResponseCodeAndIDHandler("ResetLampState", lampManagerPtr, &LampManager::ResetLampStateReply);
        AddMethodReplyWithResponseCodeAndIDHandler("TransitionLampState", lampManagerPtr, &LampManager::TransitionLampStateReply);
        AddMethodReplyWithResponseCodeAndIDHandler("TransitionLampStateToPreset", lampManagerPtr, &LampManager::TransitionLampStateToPresetReply);
        AddMethodReplyWithResponseCodeAndIDHandler("PulseLampWithState", lampManagerPtr, &LampManager::PulseLampWithStateReply);
        AddMethodReplyWithResponseCodeAndIDHandler("PulseLampWithPreset", lampManagerPtr, &LampManager::PulseLampWithPresetReply);
    }

    if (lampGroupManagerPtr) {
        AddSignalHandler("LampGroupsNameChanged", lampGroupManagerPtr, &LampGroupManager::LampGroupsNameChanged);
        AddSignalHandler("LampGroupsCreated", lampGroupManagerPtr, &LampGroupManager::LampGroupsCreated);
        AddSignalHandler("LampGroupsUpdated", lampGroupManagerPtr, &LampGroupManager::LampGroupsUpdated);
        AddSignalHandler("LampGroupsDeleted", lampGroupManagerPtr, &LampGroupManager::LampGroupsDeleted);

        AddMethodReplyWithResponseCodeAndListOfIDsHandler("GetAllLampGroupIDs", lampGroupManagerPtr, &LampGroupManager::GetAllLampGroupIDsReply);

        AddMethodReplyWithResponseCodeIDLanguageAndNameHandler("GetLampGroupName", lampGroupManagerPtr, &LampGroupManager::GetLampGroupNameReply);
        AddMethodReplyWithResponseCodeIDAndNameHandler("TransitionLampGroupStateField", lampGroupManagerPtr, &LampGroupManager::TransitionLampGroupStateFieldReply);
        AddMethodReplyWithResponseCodeIDAndNameHandler("ResetLampGroupStateField", lampGroupManagerPtr, &LampGroupManager::ResetLampGroupStateFieldReply);

        AddMethodReplyWithResponseCodeIDAndNameHandler("SetLampGroupName", lampGroupManagerPtr, &LampGroupManager::SetLampGroupNameReply);
        AddMethodReplyWithResponseCodeAndIDHandler("ResetLampGroupState", lampGroupManagerPtr, &LampGroupManager::ResetLampGroupStateReply);
        AddMethodReplyWithResponseCodeAndIDHandler("TransitionLampGroupState", lampGroupManagerPtr, &LampGroupManager::TransitionLampGroupStateReply);
        AddMethodReplyWithResponseCodeAndIDHandler("TransitionLampGroupStateToPreset", lampGroupManagerPtr, &LampGroupManager::TransitionLampGroupStateToPresetReply);
        AddMethodReplyWithResponseCodeAndIDHandler("CreateLampGroup", lampGroupManagerPtr, &LampGroupManager::CreateLampGroupReply);
        AddMethodReplyWithResponseCodeAndIDHandler("UpdateLampGroup", lampGroupManagerPtr, &LampGroupManager::UpdateLampGroupReply);
        AddMethodReplyWithResponseCodeAndIDHandler("DeleteLampGroup", lampGroupManagerPtr, &LampGroupManager::DeleteLampGroupReply);

        AddMethodReplyWithResponseCodeAndIDHandler("PulseLampGroupWithState", lampGroupManagerPtr, &LampGroupManager::PulseLampGroupWithStateReply);
        AddMethodReplyWithResponseCodeAndIDHandler("PulseLampGroupWithPreset", lampGroupManagerPtr, &LampGroupManager::PulseLampGroupWithPresetReply);
    }

    if (presetManagerPtr) {
        AddNoArgSignalHandler("DefaultLampStateChanged", presetManagerPtr, &PresetManager::DefaultLampStateChanged);
        AddSignalHandler("PresetsNameChanged", presetManagerPtr, &PresetManager::PresetsNameChanged);
        AddSignalHandler("PresetsCreated", presetManagerPtr, &PresetManager::PresetsCreated);
        AddSignalHandler("PresetsUpdated", presetManagerPtr, &PresetManager::PresetsUpdated);
        AddSignalHandler("PresetsDeleted", presetManagerPtr, &PresetManager::PresetsDeleted);

        AddMethodReplyWithResponseCodeAndListOfIDsHandler("GetAllPresetIDs", presetManagerPtr, &PresetManager::GetAllPresetIDsReply);

        AddMethodReplyWithResponseCodeIDLanguageAndNameHandler("GetPresetName", presetManagerPtr, &PresetManager::GetPresetNameReply);

        AddMethodReplyWithResponseCodeIDAndNameHandler("SetPresetName", presetManagerPtr, &PresetManager::SetPresetNameReply);
        AddMethodReplyWithResponseCodeAndIDHandler("CreatePreset", presetManagerPtr, &PresetManager::CreatePresetReply);
        AddMethodReplyWithResponseCodeAndIDHandler("UpdatePreset", presetManagerPtr, &PresetManager::UpdatePresetReply);
        AddMethodReplyWithResponseCodeAndIDHandler("DeletePreset", presetManagerPtr, &PresetManager::DeletePresetReply);

        AddMethodReplyWithUint32ValueHandler("SetDefaultLampState", presetManagerPtr, &PresetManager::SetDefaultLampStateReply);
    }

    if (sceneManagerPtr) {
        AddSignalHandler("ScenesNameChanged", sceneManagerPtr, &SceneManager::ScenesNameChanged);
        AddSignalHandler("ScenesCreated", sceneManagerPtr, &SceneManager::ScenesCreated);
        AddSignalHandler("ScenesUpdated", sceneManagerPtr, &SceneManager::ScenesUpdated);
        AddSignalHandler("ScenesDeleted", sceneManagerPtr, &SceneManager::ScenesDeleted);
        AddSignalHandler("ScenesApplied", sceneManagerPtr, &SceneManager::ScenesApplied);

        AddMethodReplyWithResponseCodeAndListOfIDsHandler("GetAllSceneIDs", sceneManagerPtr, &SceneManager::GetAllSceneIDsReply);

        AddMethodReplyWithResponseCodeIDLanguageAndNameHandler("GetSceneName", sceneManagerPtr, &SceneManager::GetSceneNameReply);

        AddMethodReplyWithResponseCodeIDAndNameHandler("SetSceneName", sceneManagerPtr, &SceneManager::SetSceneNameReply);
        AddMethodReplyWithResponseCodeAndIDHandler("CreateScene", sceneManagerPtr, &SceneManager::CreateSceneReply);
        AddMethodReplyWithResponseCodeAndIDHandler("UpdateScene", sceneManagerPtr, &SceneManager::UpdateSceneReply);
        AddMethodReplyWithResponseCodeAndIDHandler("DeleteScene", sceneManagerPtr, &SceneManager::DeleteSceneReply);
        AddMethodReplyWithResponseCodeAndIDHandler("ApplyScene", sceneManagerPtr, &SceneManager::ApplySceneReply);
    }

    if (masterSceneManagerPtr) {
        AddSignalHandler("MasterScenesNameChanged", masterSceneManagerPtr, &MasterSceneManager::MasterScenesNameChanged);
        AddSignalHandler("MasterScenesCreated", masterSceneManagerPtr, &MasterSceneManager::MasterScenesCreated);
        AddSignalHandler("MasterScenesUpdated", masterSceneManagerPtr, &MasterSceneManager::MasterScenesUpdated);
        AddSignalHandler("MasterScenesDeleted", masterSceneManagerPtr, &MasterSceneManager::MasterScenesDeleted);
        AddSignalHandler("MasterScenesApplied", masterSceneManagerPtr, &MasterSceneManager::MasterScenesApplied);

        AddMethodReplyWithResponseCodeAndListOfIDsHandler("GetAllMasterSceneIDs", masterSceneManagerPtr, &MasterSceneManager::GetAllMasterSceneIDsReply);

        AddMethodReplyWithResponseCodeIDLanguageAndNameHandler("GetMasterSceneName", masterSceneManagerPtr, &MasterSceneManager::GetMasterSceneNameReply);

        AddMethodReplyWithResponseCodeIDAndNameHandler("SetMasterSceneName", masterSceneManagerPtr, &MasterSceneManager::SetMasterSceneNameReply);
        AddMethodReplyWithResponseCodeAndIDHandler("CreateMasterScene", masterSceneManagerPtr, &MasterSceneManager::CreateMasterSceneReply);
        AddMethodReplyWithResponseCodeAndIDHandler("UpdateMasterScene", masterSceneManagerPtr, &MasterSceneManager::UpdateMasterSceneReply);
        AddMethodReplyWithResponseCodeAndIDHandler("DeleteMasterScene", masterSceneManagerPtr, &MasterSceneManager::DeleteMasterSceneReply);
        AddMethodReplyWithResponseCodeAndIDHandler("ApplyMasterScene", masterSceneManagerPtr, &MasterSceneManager::ApplyMasterSceneReply);
    }
}

void ControllerClient::AddSignalHandlers()
{
    const InterfaceDescription* controllerServiceInterface = currentLeader.proxyObject.GetInterface(ControllerServiceInterfaceName);
    const InterfaceDescription* controllerServiceLampInterface = currentLeader.proxyObject.GetInterface(ControllerServiceLampInterfaceName);
    const InterfaceDescription* controllerServiceLampGroupInterface = currentLeader.proxyObject.GetInterface(ControllerServiceLampGroupInterfaceName);
    const InterfaceDescription* controllerServicePresetInterface = currentLeader.proxyObject.GetInterface(ControllerServicePresetInterfaceName);
    const InterfaceDescription* controllerServiceSceneInterface = currentLeader.proxyObject.GetInterface(ControllerServiceSceneInterfaceName);
    const InterfaceDescription* controllerServiceMasterSceneInterface = currentLeader.proxyObject.GetInterface(ControllerServiceMasterSceneInterfaceName);

    const SignalEntry signalEntries[] = {
        { controllerServiceInterface->GetMember("ControllerServiceLightingReset"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithoutArgDispatcher) },
        { controllerServiceLampInterface->GetMember("LampsNameChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceLampInterface->GetMember("LampsStateChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceLampGroupInterface->GetMember("LampGroupsNameChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceLampGroupInterface->GetMember("LampGroupsCreated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceLampGroupInterface->GetMember("LampGroupsUpdated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceLampGroupInterface->GetMember("LampGroupsDeleted"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServicePresetInterface->GetMember("DefaultLampStateChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithoutArgDispatcher) },
        { controllerServicePresetInterface->GetMember("PresetsNameChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServicePresetInterface->GetMember("PresetsCreated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServicePresetInterface->GetMember("PresetsUpdated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServicePresetInterface->GetMember("PresetsDeleted"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceSceneInterface->GetMember("ScenesNameChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceSceneInterface->GetMember("ScenesCreated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceSceneInterface->GetMember("ScenesUpdated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceSceneInterface->GetMember("ScenesDeleted"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceSceneInterface->GetMember("ScenesApplied"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceMasterSceneInterface->GetMember("MasterScenesNameChanged"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceMasterSceneInterface->GetMember("MasterScenesCreated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceMasterSceneInterface->GetMember("MasterScenesUpdated"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceMasterSceneInterface->GetMember("MasterScenesDeleted"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) },
        { controllerServiceMasterSceneInterface->GetMember("MasterScenesApplied"), static_cast<MessageReceiver::SignalHandler>(&ControllerClient::SignalWithArgDispatcher) }
    };

    for (size_t i = 0; i < (sizeof(signalEntries) / sizeof(SignalEntry)); ++i) {
        bus.RegisterSignalHandler(
            this,
            signalEntries[i].handler,
            signalEntries[i].member,
            ControllerServiceObjectPath);
    }
}

}
