/*
 * Copyright (c) 2021-2026 Huawei Device Co., Ltd.
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

#include "mock_bundle_mgr_host.h"

using namespace OHOS::AAFwk;
namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string MODULE_NAME = "moduleName";
const std::string FIRST_BUNDLE_NAME = "com.example.bundle.one";
const std::string SECOND_BUNDLE_NAME = "com.example.bundle.two";
constexpr int32_t CACHE_STATS_INDEX = 4;
constexpr int64_t CACHE_SIZE_ONE = 100;
constexpr int64_t CACHE_SIZE_TWO = 200;
const std::string ADDITIONAL_INFO_ONE = "additionalInfoOne";
const std::string ADDITIONAL_INFO_TWO = "additionalInfoTwo";
const std::string ADDITIONAL_INFO_ALL_USER = "additionalInfoForAllUser";

bool g_getBundleInfosResult = true;
bool g_getBundleStatsFailSecondBundle = false;
ErrCode g_batchGetAdditionalInfoResult = ERR_OK;
bool g_batchGetAdditionalInfoFailSecondBundle = false;
ErrCode g_getAdditionalInfoForAllUserResult = ERR_OK;
} // namespace

void MockBundleMgrHost::SetGetBundleInfosReturn(bool result)
{
    g_getBundleInfosResult = result;
}

void MockBundleMgrHost::SetGetBundleStatsFailSecondBundle(bool enable)
{
    g_getBundleStatsFailSecondBundle = enable;
}

void MockBundleMgrHost::SetBatchGetAdditionalInfoResult(ErrCode result)
{
    g_batchGetAdditionalInfoResult = result;
}

void MockBundleMgrHost::SetBatchGetAdditionalInfoFailSecondBundle(bool enable)
{
    g_batchGetAdditionalInfoFailSecondBundle = enable;
}

void MockBundleMgrHost::SetGetAdditionalInfoForAllUserResult(ErrCode result)
{
    g_getAdditionalInfoForAllUserResult = result;
}

bool MockBundleMgrHost::DumpInfos(
    const DumpFlag flag, const std::string &bundleName, int32_t userId, std::string &result)
{
    APP_LOGD("enter");
    APP_LOGD("flag: %{public}d", flag);
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    if (bundleName.size() > 0) {
        result = bundleName + "\n";
    } else {
        result = "OK";
    }
    return true;
}

ErrCode MockBundleMgrHost::CleanBundleCacheFiles(const std::string &bundleName,
    const sptr<ICleanCacheCallback> cleanCacheCallback, int32_t userId, int32_t appIndex)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    cleanCacheCallback->OnCleanCacheFinished(true);
    return ERR_OK;
}

bool MockBundleMgrHost::CleanBundleDataFiles(const std::string &bundleName, const int userId,
    const int appIndex, const int callerUid)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    return true;
}

ErrCode MockBundleMgrHost::SetApplicationEnabled(const std::string &bundleName, bool isEnable, int32_t userId,
    bool killProcess)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    APP_LOGD("isEnable: %{public}d", isEnable);
    return ERR_OK;
}

ErrCode MockBundleMgrHost::SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnable, int32_t userId)
{
    APP_LOGD("enter");
    APP_LOGD("abilityName: %{public}s", abilityInfo.name.c_str());
    APP_LOGD("isEnable: %{public}d", isEnable);
    return ERR_OK;
}

bool MockBundleMgrHost::GetBundleArchiveInfo(const std::string &hapFilePath, const BundleFlag flag,
    BundleInfo &bundleInfo)
{
    bundleInfo.moduleNames.emplace_back(MODULE_NAME);
    return true;
}

bool MockBundleMgrHost::GetBundleInfos(int32_t flags, std::vector<BundleInfo> &bundleInfos, int32_t userId)
{
    if (!g_getBundleInfosResult) {
        return false;
    }
    bundleInfos.clear();
    BundleInfo first;
    first.name = FIRST_BUNDLE_NAME;
    first.appIndex = 0;
    bundleInfos.emplace_back(first);

    BundleInfo second;
    second.name = SECOND_BUNDLE_NAME;
    second.appIndex = 1;
    bundleInfos.emplace_back(second);
    return true;
}

bool MockBundleMgrHost::GetBundleStats(const std::string &bundleName, int32_t userId,
    std::vector<int64_t> &bundleStats, int32_t appIndex, uint32_t statFlag)
{
    if (g_getBundleStatsFailSecondBundle && bundleName == SECOND_BUNDLE_NAME) {
        return false;
    }

    bundleStats = { 0, 0, 0, 0, 0 };
    if (bundleName == FIRST_BUNDLE_NAME) {
        bundleStats[CACHE_STATS_INDEX] = CACHE_SIZE_ONE;
    } else {
        bundleStats[CACHE_STATS_INDEX] = CACHE_SIZE_TWO;
    }
    return true;
}

ErrCode MockBundleMgrHost::GetBundleStatsAsync(const std::string &bundleName, int32_t userId, int32_t appIndex,
    uint32_t statFlag, const sptr<IBundleStatsCallback> &callback)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    if (callback == nullptr) {
        return ERR_BUNDLE_MANAGER_PARAM_ERROR;
    }
    if (g_getBundleStatsFailSecondBundle && bundleName == SECOND_BUNDLE_NAME) {
        callback->OnGetBundleStatsFinished(ERR_BUNDLE_MANAGER_INTERNAL_ERROR, {});
        return ERR_OK;
    }

    std::vector<int64_t> bundleStats = { 0, 0, 0, 0, 0 };
    if (bundleName == FIRST_BUNDLE_NAME) {
        bundleStats[CACHE_STATS_INDEX] = CACHE_SIZE_ONE;
    } else {
        bundleStats[CACHE_STATS_INDEX] = CACHE_SIZE_TWO;
    }
    callback->OnGetBundleStatsFinished(ERR_OK, bundleStats);
    return ERR_OK;
}

ErrCode MockBundleMgrHost::BatchGetAdditionalInfo(const std::vector<std::string> &bundleNames,
    std::vector<BundleAdditionalInfo> &additionalInfos)
{
    APP_LOGD("enter");
    if (bundleNames.empty()) {
        return ERR_BUNDLE_MANAGER_PARAM_ERROR;
    }
    if (g_batchGetAdditionalInfoResult != ERR_OK) {
        return g_batchGetAdditionalInfoResult;
    }

    additionalInfos.clear();
    for (const auto &bundleName : bundleNames) {
        BundleAdditionalInfo info;
        info.bundleName = bundleName;
        if (g_batchGetAdditionalInfoFailSecondBundle && bundleName == SECOND_BUNDLE_NAME) {
            info.errCode = ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST;
        } else {
            info.additionalInfo = bundleName == SECOND_BUNDLE_NAME ? ADDITIONAL_INFO_TWO : ADDITIONAL_INFO_ONE;
        }
        additionalInfos.emplace_back(info);
    }
    return ERR_OK;
}

ErrCode MockBundleMgrHost::GetAdditionalInfoForAllUser(const std::string &bundleName, std::string &additionalInfo)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        return ERR_BUNDLE_MANAGER_PARAM_ERROR;
    }
    if (g_getAdditionalInfoForAllUserResult != ERR_OK) {
        return g_getAdditionalInfoForAllUserResult;
    }
    additionalInfo = ADDITIONAL_INFO_ALL_USER;
    return ERR_OK;
}
}  // namespace AppExecFwk
}  // namespace OHOS