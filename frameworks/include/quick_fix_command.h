/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_BM_INCLUDE_QUICK_FIX_COMMAND_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_BM_INCLUDE_QUICK_FIX_COMMAND_H

#include <vector>

#include "quick_fix_info.h"

namespace OHOS {
namespace AppExecFwk {
class QuickFixCommand {
public:
static int32_t ApplyQuickFix(const std::vector<std::string> &quickFixFiles, std::string &resultInfo,
    bool isDebug = false, bool isReplace = false);
static int32_t GetApplyedQuickFixInfo(const std::string &bundleName, std::string &resultInfo);
static std::string GetQuickFixInfoString(const AAFwk::ApplicationQuickFixInfo &quickFixInfo);
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_BM_INCLUDE_QUICK_FIX_COMMAND_H