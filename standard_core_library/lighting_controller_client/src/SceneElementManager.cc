/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
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

#include <SceneElementManager.h>
#include <ControllerClient.h>

#include <qcc/Debug.h>

#include <utility>

#define QCC_MODULE "SCENE_ELEMENT_MANAGER"

using namespace lsf;
using namespace ajn;

SceneElementManager::SceneElementManager(ControllerClient& controllerClient, SceneElementManagerCallback& callback) :
    Manager(controllerClient),
    callback(callback)
{
    controllerClient.sceneElementManagerPtr = this;
}

ControllerClientStatus SceneElementManager::GetAllSceneElementIDs(void)
{
    QCC_DbgPrintf(("%s", __func__));
    return controllerClient.MethodCallAsyncForReplyWithResponseCodeAndListOfIDs(
               ControllerServiceSceneElementInterfaceName,
               "GetAllSceneElementIDs");
}

ControllerClientStatus SceneElementManager::GetSceneElementName(const LSFString& sceneElementID, const LSFString& language)
{
    QCC_DbgPrintf(("%s: sceneElementID=%s", __func__, sceneElementID.c_str()));
    MsgArg args[2];
    args[0].Set("s", sceneElementID.c_str());
    args[1].Set("s", language.c_str());
    return controllerClient.MethodCallAsyncForReplyWithResponseCodeIDLanguageAndName(
               ControllerServiceSceneElementInterfaceName,
               "GetSceneElementName",
               args,
               2);
}

ControllerClientStatus SceneElementManager::SetSceneElementName(const LSFString& sceneElementID, const LSFString& sceneElementName, const LSFString& language)
{
    QCC_DbgPrintf(("%s: sceneElementID=%s sceneElementName=%s language=%s", __func__, sceneElementID.c_str(), sceneElementName.c_str(), language.c_str()));

    MsgArg args[3];
    args[0].Set("s", sceneElementID.c_str());
    args[1].Set("s", sceneElementName.c_str());
    args[2].Set("s", language.c_str());

    return controllerClient.MethodCallAsyncForReplyWithResponseCodeIDAndName(
               ControllerServiceSceneElementInterfaceName,
               "SetSceneElementName",
               args,
               3);
}

ControllerClientStatus SceneElementManager::CreateSceneElement(uint32_t& trackingID, const SceneElement& sceneElement, const LSFString& sceneElementName, const LSFString& language)
{
    QCC_DbgPrintf(("%s", __func__));
    MsgArg args[5];
    sceneElement.Get(&args[0], &args[1], &args[2]);
    args[3].Set("s", sceneElementName.c_str());
    args[4].Set("s", language.c_str());

    return controllerClient.MethodCallAsyncForReplyWithResponseCodeIDAndTrackingID(
               trackingID,
               ControllerServiceSceneElementInterfaceName,
               "CreateSceneElement",
               args,
               5);
}

ControllerClientStatus SceneElementManager::UpdateSceneElement(const LSFString& sceneElementID, const SceneElement& sceneElement)
{
    QCC_DbgPrintf(("%s: sceneElementID=%s", __func__, sceneElementID.c_str()));
    MsgArg args[4];
    args[0].Set("s", sceneElementID.c_str());
    sceneElement.Get(&args[1], &args[2], &args[3]);

    return controllerClient.MethodCallAsyncForReplyWithResponseCodeAndID(
               ControllerServiceSceneElementInterfaceName,
               "UpdateSceneElement",
               args,
               4);
}

ControllerClientStatus SceneElementManager::GetSceneElement(const LSFString& sceneElementID)
{
    QCC_DbgPrintf(("%s: sceneElementID=%s", __func__, sceneElementID.c_str()));
    MsgArg arg;
    arg.Set("s", sceneElementID.c_str());

    return controllerClient.MethodCallAsync(
               ControllerServiceSceneElementInterfaceName,
               "GetSceneElement",
               this,
               &SceneElementManager::GetSceneElementReply,
               &arg,
               1);
}

ControllerClientStatus SceneElementManager::ApplySceneElement(const LSFString& sceneElementID)
{
    QCC_DbgPrintf(("%s: sceneElementID=%s", __func__, sceneElementID.c_str()));
    MsgArg arg;
    arg.Set("s", sceneElementID.c_str());

    return controllerClient.MethodCallAsyncForReplyWithResponseCodeAndID(
               ControllerServiceSceneElementInterfaceName,
               "ApplySceneElement",
               &arg,
               1);
}

void SceneElementManager::GetSceneElementReply(Message& message)
{
    QCC_DbgPrintf(("%s", __func__));
    size_t numArgs;
    const MsgArg* args;
    message->GetArgs(numArgs, args);

    if (controllerClient.CheckNumArgsInMessage(numArgs, 5) != LSF_OK) {
        return;
    }

    LSFResponseCode responseCode = static_cast<LSFResponseCode>(args[0].v_uint32);
    LSFString sceneElementID = static_cast<LSFString>(args[1].v_string.str);
    SceneElement sceneElement(args[2], args[3], args[4]);

    callback.GetSceneElementReplyCB(responseCode, sceneElementID, sceneElement);
}

ControllerClientStatus SceneElementManager::DeleteSceneElement(const LSFString& sceneElementID)
{
    QCC_DbgPrintf(("%s: sceneElementID=%s", __func__, sceneElementID.c_str()));
    MsgArg arg;
    arg.Set("s", sceneElementID.c_str());

    return controllerClient.MethodCallAsyncForReplyWithResponseCodeAndID(
               ControllerServiceSceneElementInterfaceName,
               "DeleteSceneElement",
               &arg,
               1);
}

ControllerClientStatus SceneElementManager::GetSceneElementDataSet(const LSFString& sceneElementID, const LSFString& language)
{
    ControllerClientStatus status = CONTROLLER_CLIENT_OK;

    status = GetSceneElement(sceneElementID);

    if (CONTROLLER_CLIENT_OK == status) {
        status = GetSceneElementName(sceneElementID, language);
    }

    return status;
}
