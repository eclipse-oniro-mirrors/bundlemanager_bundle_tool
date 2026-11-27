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

#include <gtest/gtest.h>

#include <new>
#include <unistd.h>
#include <vector>

#define private public
#define protected public
#include "bundle_test_tool.h"
#undef protected
#undef private

#include "bundle_constants.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "mock_bundle_installer_host.h"
#include "mock_bundle_mgr_host.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
class BundleTestToolCacheStatTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleTestTool &cmd) const;

    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BundleTestToolCacheStatTest::SetUp()
{
    optind = 0;
    MakeMockObjects();
    MockBundleMgrHost::SetGetBundleInfosReturn(true);
    MockBundleMgrHost::SetGetBundleStatsFailSecondBundle(false);
    MockBundleMgrHost::SetGetExtensionSandboxDataDirResult(ERR_OK, "");
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Dir_0100
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Verify the command prints the directory returned by BMS.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Dir_0100,
    Function | MediumTest | TestSize.Level1)
{
    const std::string dir = "+extension-entry-inputMethodExtAbility+com.example.bundle.one";
    MockBundleMgrHost::SetGetExtensionSandboxDataDirResult(ERR_OK, dir);
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getExtensionSandboxDataDir"),
        const_cast<char*>("-e"),
        const_cast<char*>("inputMethodExtAbility"),
    };
    BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), ERR_OK);
    EXPECT_NE(cmd.resultReceiver_.find("getExtensionSandboxDataDir successfully"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find(dir), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Dir_0200
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Verify missing extension name prints parameter error.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Dir_0200,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getExtensionSandboxDataDir"),
        const_cast<char*>("-e"),
        const_cast<char*>(""),
    };
    BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), ERR_BUNDLE_MANAGER_PARAM_ERROR);
    EXPECT_EQ(cmd.resultReceiver_.find("usage: bundle_test_tool getExtensionSandboxDataDir"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("getExtensionSandboxDataDir failed\nerrCode is 8521225\n"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Dir_0300
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Verify query failures print the returned error code.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Dir_0300,
    Function | MediumTest | TestSize.Level1)
{
    constexpr ErrCode QUERY_FAILED = ERR_BUNDLE_MANAGER_EXTENSION_SANDBOX_DIR_NOT_EXIST;
    MockBundleMgrHost::SetGetExtensionSandboxDataDirResult(QUERY_FAILED, "");
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getExtensionSandboxDataDir"),
        const_cast<char*>("-e"),
        const_cast<char*>("missingExtension"),
    };
    BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), QUERY_FAILED);
    EXPECT_NE(cmd.resultReceiver_.find("getExtensionSandboxDataDir failed"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find(std::to_string(QUERY_FAILED)), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0400
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Reject app index options; BMS selects the caller's own instance.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0400,
    Function | MediumTest | TestSize.Level1)
{
    for (const char *option : {"-a", "--app-index"}) {
        optind = 0;
        char *argv[] = {
            const_cast<char*>("bundle_test_tool"),
            const_cast<char*>("getExtensionSandboxDataDir"),
            const_cast<char*>("-e"),
            const_cast<char*>("inputMethodExtAbility"),
            const_cast<char*>(option),
            const_cast<char*>("1"),
        };
        BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
        SetMockObjects(cmd);
        EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), OHOS::ERR_INVALID_VALUE);
    }
}

namespace {
class ExtensionSandboxUidHost : public MockBundleMgrHost {
public:
    ErrCode GetExtensionSandboxDataDir(const std::string &extensionName, std::string &dir) override
    {
        observedUid = geteuid();
        ++callCount;
        dir = "test-directory";
        return queryResult;
    }
    uid_t observedUid = 0;
    int32_t callCount = 0;
    ErrCode queryResult = ERR_OK;
};
} // namespace

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0500
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Parse short and long caller UID options.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0500,
    Function | MediumTest | TestSize.Level1)
{
    for (const char *option : {"-i", "--uid"}) {
        optind = 0;
        char *argv[] = {const_cast<char*>("bundle_test_tool"), const_cast<char*>("getExtensionSandboxDataDir"),
            const_cast<char*>("-e"), const_cast<char*>("inputMethodExtAbility"),
            const_cast<char*>(option), const_cast<char*>("20010000")};
        BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
        std::string extensionName;
        int32_t callerUid = Constants::INVALID_UID;
        bool showHelp = false;
        EXPECT_EQ(cmd.ParseGetExtensionSandboxDataDirArgs(extensionName, callerUid, showHelp), ERR_OK);
        EXPECT_EQ(callerUid, 20010000);
        EXPECT_EQ(extensionName, "inputMethodExtAbility");
        EXPECT_FALSE(showHelp);
    }
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0600
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Reject malformed or out-of-range UIDs without calling BMS.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0600,
    Function | MediumTest | TestSize.Level1)
{
    for (const char *value : {"", "-1", "abc", "10000x", "2147483648"}) {
        optind = 0;
        char *argv[] = {const_cast<char*>("bundle_test_tool"), const_cast<char*>("getExtensionSandboxDataDir"),
            const_cast<char*>("-e"), const_cast<char*>("inputMethodExtAbility"),
            const_cast<char*>("-i"), const_cast<char*>(value)};
        BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
        sptr<ExtensionSandboxUidHost> host = new ExtensionSandboxUidHost();
        cmd.bundleMgrProxy_ = host;
        const uid_t originalUid = geteuid();
        EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), OHOS::ERR_INVALID_VALUE);
        EXPECT_EQ(host->callCount, 0);
        EXPECT_EQ(geteuid(), originalUid);
    }
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0700
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Preserve the current caller UID when the option is omitted.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0700,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {const_cast<char*>("bundle_test_tool"), const_cast<char*>("getExtensionSandboxDataDir"),
        const_cast<char*>("-e"), const_cast<char*>("inputMethodExtAbility")};
    BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
    sptr<ExtensionSandboxUidHost> host = new ExtensionSandboxUidHost();
    cmd.bundleMgrProxy_ = host;
    const uid_t originalUid = geteuid();
    EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), ERR_OK);
    EXPECT_EQ(host->observedUid, originalUid);
    EXPECT_EQ(host->callCount, 1);
    EXPECT_EQ(geteuid(), originalUid);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0800
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Switch caller UID and restore it on query success and failure.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0800,
    Function | MediumTest | TestSize.Level1)
{
    if (geteuid() != Constants::ROOT_UID) {
        GTEST_SKIP() << "Switching caller UID requires root";
    }
    const uid_t originalUid = geteuid();
    for (ErrCode queryResult : std::vector<ErrCode>{ERR_OK, ERR_BUNDLE_MANAGER_EXTENSION_SANDBOX_DIR_NOT_EXIST}) {
        optind = 0;
        char *argv[] = {const_cast<char*>("bundle_test_tool"), const_cast<char*>("getExtensionSandboxDataDir"),
            const_cast<char*>("-e"), const_cast<char*>("inputMethodExtAbility"),
            const_cast<char*>("-i"), const_cast<char*>("10000")};
        BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
        sptr<ExtensionSandboxUidHost> host = new ExtensionSandboxUidHost();
        host->queryResult = queryResult;
        cmd.bundleMgrProxy_ = host;
        EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), queryResult);
        EXPECT_EQ(host->observedUid, static_cast<uid_t>(Constants::BASE_APP_UID));
        EXPECT_EQ(host->callCount, 1);
        ASSERT_EQ(geteuid(), originalUid);
    }
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0900
 * @tc.name: GetExtensionSandboxDataDir
 * @tc.desc: Reject a missing UID argument before calling BMS.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Extension_Sandbox_Data_Dir_0900,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {const_cast<char*>("bundle_test_tool"), const_cast<char*>("getExtensionSandboxDataDir"),
        const_cast<char*>("-e"), const_cast<char*>("inputMethodExtAbility"), const_cast<char*>("-i")};
    BundleTestTool cmd(sizeof(argv) / sizeof(argv[0]), argv);
    sptr<ExtensionSandboxUidHost> host = new ExtensionSandboxUidHost();
    cmd.bundleMgrProxy_ = host;
    EXPECT_EQ(cmd.RunAsGetExtensionSandboxDataDir(), OHOS::ERR_INVALID_VALUE);
    EXPECT_EQ(host->callCount, 0);
}

void BundleTestToolCacheStatTest::TearDown()
{}

void BundleTestToolCacheStatTest::MakeMockObjects()
{
    auto mgrHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleMgrHost());
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    auto installerHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleInstallerHost());
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void BundleTestToolCacheStatTest::SetMockObjects(BundleTestTool &cmd) const
{
    cmd.bundleMgrProxy_ = mgrProxyPtr_;
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Each_Bundle_Cache_Stat_0100
 * @tc.name: GetEachBundleCacheStat
 * @tc.desc: Verify GetEachBundleCacheStat returns all bundle cache sizes when all stats succeed.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Each_Bundle_Cache_Stat_0100,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getEachBundleCacheStat"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    std::string msg;
    bool ret = cmd.GetEachBundleCacheStat(100, msg);

    EXPECT_TRUE(ret);
    EXPECT_NE(msg.find("bundleName: com.example.bundle.one, appIndex: 0, cache size: 100"), std::string::npos);
    EXPECT_NE(msg.find("bundleName: com.example.bundle.two, appIndex: 1, cache size: 200"), std::string::npos);
    EXPECT_NE(msg.find("success count: 2"), std::string::npos);
    EXPECT_NE(msg.find("fail count: 0"), std::string::npos);
    EXPECT_NE(msg.find("total cache size: 300"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Each_Bundle_Cache_Stat_0200
 * @tc.name: GetEachBundleCacheStat
 * @tc.desc: Verify GetEachBundleCacheStat reports failure when GetBundleInfos fails.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Each_Bundle_Cache_Stat_0200,
    Function | MediumTest | TestSize.Level1)
{
    MockBundleMgrHost::SetGetBundleInfosReturn(false);

    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getEachBundleCacheStat"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    std::string msg;
    bool ret = cmd.GetEachBundleCacheStat(100, msg);

    EXPECT_FALSE(ret);
    EXPECT_NE(msg.find("error: get bundle infos failed"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Each_Bundle_Cache_Stat_0300
 * @tc.name: GetEachBundleCacheStat
 * @tc.desc: Verify GetEachBundleCacheStat keeps partial result when one bundle stats query fails.
 */
HWTEST_F(BundleTestToolCacheStatTest, Bundle_Test_Tool_Get_Each_Bundle_Cache_Stat_0300,
    Function | MediumTest | TestSize.Level1)
{
    MockBundleMgrHost::SetGetBundleStatsFailSecondBundle(true);

    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getEachBundleCacheStat"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    std::string msg;
    bool ret = cmd.GetEachBundleCacheStat(100, msg);

    EXPECT_FALSE(ret);
    EXPECT_NE(msg.find("bundleName: com.example.bundle.one, appIndex: 0, cache size: 100"), std::string::npos);
    EXPECT_NE(msg.find("bundleName: com.example.bundle.two, appIndex: 1, error: get bundle stats failed"),
        std::string::npos);
    EXPECT_NE(msg.find("success count: 1"), std::string::npos);
    EXPECT_NE(msg.find("fail count: 1"), std::string::npos);
    EXPECT_NE(msg.find("total cache size: 100"), std::string::npos);
}
}  // namespace OHOS
