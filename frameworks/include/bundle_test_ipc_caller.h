/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_BUNDLE_TOOL_INCLUDE_BUNDLE_TEST_IPC_CALLER_H
#define FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_BUNDLE_TOOL_INCLUDE_BUNDLE_TEST_IPC_CALLER_H

#include <map>
#include <memory>
#include <string>

#include "app_log_wrapper.h"
#include "errors.h"
#include "iremote_object.h"
#include "message_parcel.h"
#include "refbase.h"

namespace OHOS {
namespace AAFwk {
class Want;
}

namespace AppExecFwk {

class GenericIpcCaller : public RefBase {
public:
    GenericIpcCaller();
    ~GenericIpcCaller() override;

    std::string ListCommands();
    ErrCode ExecuteCommand(const std::string& name, int argc, char **argv, std::string &output);
    bool LoadConfig();
    bool IsConfigLoaded() const { return configLoaded_; }
    static void SetCmdsPath(const std::string &path) { cmdsPath_ = path; }

private:
    struct Impl;
    bool configLoaded_ = false;
    std::unique_ptr<Impl> impl_;
    bool WriteWantParam(AAFwk::Want &want,
        const std::map<std::string, std::string> &parsedValues, MessageParcel &data);
    static std::string cmdsPath_;

    DISALLOW_COPY_AND_MOVE(GenericIpcCaller);
};

}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_BUNDLEMANAGER_BUNDLE_FRAMEWORK_BUNDLE_TOOL_INCLUDE_BUNDLE_TEST_IPC_CALLER_H
