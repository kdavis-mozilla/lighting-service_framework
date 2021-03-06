#ifndef CONTROLLER_SERVICE_MANAGER_INIT_H
#define CONTROLLER_SERVICE_MANAGER_INIT_H
/**
 * \defgroup ControllerServiceManagerInit
 * ControllerServiceManagerInit code
 */
/**
 * \ingroup ControllerService
 */
/**
 * @file
 * This file provides definitions for initializing the ControllerServiceManager
 */
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

#include "ControllerService.h"

namespace lsf {

OPTIONAL_NAMESPACE_CONTROLLER_SERVICE

/**
 * Used to initialize the ControllerServiceManager
 */
ControllerServiceManager* InitializeControllerServiceManager(
    const std::string& factoryConfigFile,
    const std::string& configFile,
    const std::string& lampGroupFile,
    const std::string& presetFile,
    const std::string& transitionEffectFile,
    const std::string& pulseEffectFile,
    const std::string& sceneElementsFile,
    const std::string& sceneFile,
    const std::string& sceneWithSceneElementsFile,
    const std::string& masterSceneFile);

OPTIONAL_NAMESPACE_CLOSE

} //lsf

#endif
