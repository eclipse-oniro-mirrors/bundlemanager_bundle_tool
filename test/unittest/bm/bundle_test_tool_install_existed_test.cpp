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

#define private public
#define protected public
#include "bundle_test_tool.h"
#undef protected
#undef private

#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "mock_bundle_installer_host.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
const std::string TEST_BUNDLE_NAME = "com.example.install.existed";
constexpr int32_t TEST_USER_ID = 101;
}

class InstallExistedBundleInstallerHost : public MockBundleInstallerHost {
public:
    ErrCode InstallExisted(const std::string &bundleName, int32_t userId) override
    {
        called_ = true;
        bundleName_ = bundleName;
        userId_ = userId;
        return result_;
    }

    bool called_ = false;
    std::string bundleName_;
    int32_t userId_ = 0;
    ErrCode result_ = ERR_OK;
};

class BundleTestToolInstallExistedTest : public testing::Test {
public:
    void SetUp() override
    {
        optind = 0;
        installerHost_ = sptr<InstallExistedBundleInstallerHost>(
            new (std::nothrow) InstallExistedBundleInstallerHost());
        ASSERT_NE(installerHost_, nullptr);
        installerProxy_ = iface_cast<IBundleInstaller>(installerHost_->AsObject());
        ASSERT_NE(installerProxy_, nullptr);
    }

    void TearDown() override
    {
        optind = 0;
    }

    void SetInstallerProxy(BundleTestTool &command) const
    {
        command.bundleInstallerProxy_ = installerProxy_;
    }

    sptr<InstallExistedBundleInstallerHost> installerHost_;
    sptr<IBundleInstaller> installerProxy_;
};

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0100
 * @tc.name: ParseInstallExistedOptions
 * @tc.desc: The InstallExisted test command uses a non-EDM root uid by default.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0100,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    std::string bundleName;
    std::string permissionMode;
    int32_t userId = 0;
    int32_t euid = 0;

    ErrCode result = command.ParseInstallExistedOptions(bundleName, userId, euid, permissionMode);

    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(bundleName, TEST_BUNDLE_NAME);
    EXPECT_EQ(userId, TEST_USER_ID);
    EXPECT_EQ(euid, Constants::ROOT_UID);
    EXPECT_EQ(permissionMode, "install_bundle");
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0200
 * @tc.name: ParseInstallExistedOptions
 * @tc.desc: The InstallExisted test command accepts an explicitly specified EDM uid.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0200,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
        const_cast<char *>("-e"),
        const_cast<char *>("3057"),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    std::string bundleName;
    std::string permissionMode;
    int32_t userId = 0;
    int32_t euid = 0;

    ErrCode result = command.ParseInstallExistedOptions(bundleName, userId, euid, permissionMode);

    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(euid, Constants::EDM_UID);
    EXPECT_EQ(permissionMode, "manage_edm_policy");
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0250
 * @tc.name: ParseInstallExistedOptions
 * @tc.desc: The caller permission can be explicitly overridden for negative permission tests.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0250,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
        const_cast<char *>("-e"),
        const_cast<char *>("3057"),
        const_cast<char *>("-p"),
        const_cast<char *>("none"),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    std::string bundleName;
    std::string permissionMode;
    int32_t userId = 0;
    int32_t euid = 0;

    ErrCode result = command.ParseInstallExistedOptions(bundleName, userId, euid, permissionMode);

    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(euid, Constants::EDM_UID);
    EXPECT_EQ(permissionMode, "none");
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0260
 * @tc.name: ParseInstallExistedOptions
 * @tc.desc: INSTALL_BUNDLE and MANAGE_EDM_POLICY can be granted together.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0260,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
        const_cast<char *>("-e"),
        const_cast<char *>("3057"),
        const_cast<char *>("-p"),
        const_cast<char *>("both"),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    std::string bundleName;
    std::string permissionMode;
    int32_t userId = 0;
    int32_t euid = 0;

    ErrCode result = command.ParseInstallExistedOptions(bundleName, userId, euid, permissionMode);

    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(euid, Constants::EDM_UID);
    EXPECT_EQ(permissionMode, "both");
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0275
 * @tc.name: ParseInstallExistedOptions
 * @tc.desc: An unsupported caller permission mode is rejected.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0275,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
        const_cast<char *>("-p"),
        const_cast<char *>("invalid_permission"),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    std::string bundleName;
    std::string permissionMode;
    int32_t userId = 0;
    int32_t euid = 0;

    ErrCode result = command.ParseInstallExistedOptions(bundleName, userId, euid, permissionMode);

    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0300
 * @tc.name: RunAsInstallExistedCommand
 * @tc.desc: The command forwards bundle name and user id to IBundleInstaller and reports success.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0300,
    Function | MediumTest | TestSize.Level1)
{
    std::string currentEuid = std::to_string(geteuid());
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
        const_cast<char *>("-e"),
        const_cast<char *>(currentEuid.c_str()),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    SetInstallerProxy(command);

    ErrCode result = command.RunAsInstallExistedCommand();

    EXPECT_EQ(result, ERR_OK);
    EXPECT_TRUE(installerHost_->called_);
    EXPECT_EQ(installerHost_->bundleName_, TEST_BUNDLE_NAME);
    EXPECT_EQ(installerHost_->userId_, TEST_USER_ID);
    EXPECT_EQ(command.resultReceiver_, "install existed app successfully\n");
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0400
 * @tc.name: RunAsInstallExistedCommand
 * @tc.desc: The command returns and prints the exact error reported by IBundleInstaller.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0400,
    Function | MediumTest | TestSize.Level1)
{
    installerHost_->result_ = ERR_APPEXECFWK_INSTALL_EXISTED_ENTERPRISE_BUNDLE_NOT_ALLOWED;
    std::string currentEuid = std::to_string(geteuid());
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
        const_cast<char *>("-e"),
        const_cast<char *>(currentEuid.c_str()),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    SetInstallerProxy(command);

    ErrCode result = command.RunAsInstallExistedCommand();

    EXPECT_EQ(result, ERR_APPEXECFWK_INSTALL_EXISTED_ENTERPRISE_BUNDLE_NOT_ALLOWED);
    EXPECT_TRUE(installerHost_->called_);
    EXPECT_NE(command.resultReceiver_.find(std::to_string(result)), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Install_Existed_0500
 * @tc.name: RunAsInstallExistedCommand
 * @tc.desc: Missing required options are rejected without invoking IBundleInstaller.
 */
HWTEST_F(BundleTestToolInstallExistedTest, Bundle_Test_Tool_Install_Existed_0500,
    Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("installExisted"),
        const_cast<char *>("-u"),
        const_cast<char *>("101"),
    };
    BundleTestTool command(sizeof(argv) / sizeof(argv[0]), argv);
    SetInstallerProxy(command);

    ErrCode result = command.RunAsInstallExistedCommand();

    EXPECT_EQ(result, ERR_INVALID_VALUE);
    EXPECT_FALSE(installerHost_->called_);
    EXPECT_NE(command.resultReceiver_.find("usage: bundle_test_tool installExisted"), std::string::npos);
}
}  // namespace OHOS
