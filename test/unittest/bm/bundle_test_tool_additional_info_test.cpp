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

#include "iremote_broker.h"
#include "iremote_object.h"
#include "mock_bundle_installer_host.h"
#include "mock_bundle_mgr_host.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
class BundleTestToolAdditionalInfoTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleTestTool &cmd) const;

    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BundleTestToolAdditionalInfoTest::SetUp()
{
    optind = 0;
    MakeMockObjects();
    MockBundleMgrHost::SetBatchGetAdditionalInfoResult(ERR_OK);
    MockBundleMgrHost::SetBatchGetAdditionalInfoFailSecondBundle(false);
    MockBundleMgrHost::SetGetAdditionalInfoForAllUserResult(ERR_OK);
}

void BundleTestToolAdditionalInfoTest::TearDown()
{}

void BundleTestToolAdditionalInfoTest::MakeMockObjects()
{
    auto mgrHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleMgrHost());
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    auto installerHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleInstallerHost());
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void BundleTestToolAdditionalInfoTest::SetMockObjects(BundleTestTool &cmd) const
{
    cmd.bundleMgrProxy_ = mgrProxyPtr_;
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bundle_Test_Tool_Batch_Get_Additional_Info_0100
 * @tc.name: RunAsBatchGetAdditionalInfo
 * @tc.desc: Verify RunAsBatchGetAdditionalInfo splits bundle names by comma and outputs one JSON row per bundle.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Batch_Get_Additional_Info_0100,
    Function | MediumTest | TestSize.Level1)
{
    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("batchGetAdditionalInfo"),
        const_cast<char*>("-n"),
        const_cast<char*>("com.example.bundle.one,com.example.bundle.two"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsBatchGetAdditionalInfo();

    EXPECT_EQ(result, OHOS::ERR_OK);
    EXPECT_NE(cmd.resultReceiver_.find("[{"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("\"bundleName\":\"com.example.bundle.one\""), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("\"additionalInfo\":\"additionalInfoOne\""), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("\"bundleName\":\"com.example.bundle.two\""), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("\"additionalInfo\":\"additionalInfoTwo\""), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Batch_Get_Additional_Info_0200
 * @tc.name: RunAsBatchGetAdditionalInfo
 * @tc.desc: Verify RunAsBatchGetAdditionalInfo reports failure and passes the error code through when the IPC fails.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Batch_Get_Additional_Info_0200,
    Function | MediumTest | TestSize.Level1)
{
    MockBundleMgrHost::SetBatchGetAdditionalInfoResult(ERR_BUNDLE_MANAGER_INTERNAL_ERROR);

    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("batchGetAdditionalInfo"),
        const_cast<char*>("-n"),
        const_cast<char*>("com.example.bundle.one"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsBatchGetAdditionalInfo();

    EXPECT_EQ(result, ERR_BUNDLE_MANAGER_INTERNAL_ERROR);
    EXPECT_NE(cmd.resultReceiver_.find("batchGetAdditionalInfo failed"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find(std::to_string(ERR_BUNDLE_MANAGER_INTERNAL_ERROR)), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Batch_Get_Additional_Info_0300
 * @tc.name: RunAsBatchGetAdditionalInfo
 * @tc.desc: Verify RunAsBatchGetAdditionalInfo outputs errCode for the failed bundle and additionalInfo for the rest.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Batch_Get_Additional_Info_0300,
    Function | MediumTest | TestSize.Level1)
{
    MockBundleMgrHost::SetBatchGetAdditionalInfoFailSecondBundle(true);

    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("batchGetAdditionalInfo"),
        const_cast<char*>("-n"),
        const_cast<char*>("com.example.bundle.one,com.example.bundle.two"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsBatchGetAdditionalInfo();

    EXPECT_EQ(result, OHOS::ERR_OK);
    EXPECT_NE(cmd.resultReceiver_.find("\"additionalInfo\":\"additionalInfoOne\""), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("\"bundleName\":\"com.example.bundle.two\""), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("\"errCode\":" + std::to_string(ERR_BUNDLE_MANAGER_BUNDLE_NOT_EXIST)),
        std::string::npos);
    EXPECT_EQ(cmd.resultReceiver_.find("\"additionalInfo\":\"additionalInfoTwo\""), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Batch_Get_Additional_Info_0400
 * @tc.name: RunAsBatchGetAdditionalInfo
 * @tc.desc: Verify RunAsBatchGetAdditionalInfo prints usage and fails when no bundle name is specified.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Batch_Get_Additional_Info_0400,
    Function | MediumTest | TestSize.Level1)
{
    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("batchGetAdditionalInfo"),
        const_cast<char*>("-h"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsBatchGetAdditionalInfo();

    EXPECT_NE(result, OHOS::ERR_OK);
    EXPECT_NE(cmd.resultReceiver_.find("usage: bundle_test_tool batchGetAdditionalInfo"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Additional_Info_For_All_User_0100
 * @tc.name: RunAsGetAdditionalInfoForAllUser
 * @tc.desc: Verify RunAsGetAdditionalInfoForAllUser parses the bundle name option and returns the additional info.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Get_Additional_Info_For_All_User_0100,
    Function | MediumTest | TestSize.Level1)
{
    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getAdditionalInfoForAllUser"),
        const_cast<char*>("-n"),
        const_cast<char*>("com.example.bundle.one"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsGetAdditionalInfoForAllUser();

    EXPECT_EQ(result, OHOS::ERR_OK);
    EXPECT_NE(cmd.resultReceiver_.find("getAdditionalInfoForAllUser successfully"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find("additionalInfo: additionalInfoForAllUser"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Additional_Info_For_All_User_0200
 * @tc.name: RunAsGetAdditionalInfoForAllUser
 * @tc.desc: Verify RunAsGetAdditionalInfoForAllUser passes the permission error code through when the IPC is denied.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Get_Additional_Info_For_All_User_0200,
    Function | MediumTest | TestSize.Level1)
{
    MockBundleMgrHost::SetGetAdditionalInfoForAllUserResult(ERR_BUNDLE_MANAGER_PERMISSION_DENIED);

    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getAdditionalInfoForAllUser"),
        const_cast<char*>("-n"),
        const_cast<char*>("com.example.bundle.one"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsGetAdditionalInfoForAllUser();

    EXPECT_EQ(result, ERR_BUNDLE_MANAGER_PERMISSION_DENIED);
    EXPECT_NE(cmd.resultReceiver_.find("getAdditionalInfoForAllUser failed"), std::string::npos);
    EXPECT_NE(cmd.resultReceiver_.find(std::to_string(ERR_BUNDLE_MANAGER_PERMISSION_DENIED)), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Additional_Info_For_All_User_0300
 * @tc.name: RunAsGetAdditionalInfoForAllUser
 * @tc.desc: Verify RunAsGetAdditionalInfoForAllUser prints usage and fails when no option is specified.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Get_Additional_Info_For_All_User_0300,
    Function | MediumTest | TestSize.Level1)
{
    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getAdditionalInfoForAllUser"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsGetAdditionalInfoForAllUser();

    EXPECT_NE(result, OHOS::ERR_OK);
    EXPECT_NE(cmd.resultReceiver_.find("usage: bundle_test_tool getAdditionalInfoForAllUser"), std::string::npos);
    EXPECT_EQ(cmd.resultReceiver_.find("getAdditionalInfoForAllUser successfully"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Get_Additional_Info_For_All_User_0400
 * @tc.name: RunAsGetAdditionalInfoForAllUser
 * @tc.desc: Verify RunAsGetAdditionalInfoForAllUser rejects an empty bundle name locally instead of sending the IPC.
 */
HWTEST_F(BundleTestToolAdditionalInfoTest, Bundle_Test_Tool_Get_Additional_Info_For_All_User_0400,
    Function | MediumTest | TestSize.Level1)
{
    optind = 0;
    char *argv[] = {
        const_cast<char*>("bundle_test_tool"),
        const_cast<char*>("getAdditionalInfoForAllUser"),
        const_cast<char*>("-n"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    SetMockObjects(cmd);

    ErrCode result = cmd.RunAsGetAdditionalInfoForAllUser();

    EXPECT_EQ(result, OHOS::ERR_INVALID_VALUE);
    EXPECT_NE(cmd.resultReceiver_.find("usage: bundle_test_tool getAdditionalInfoForAllUser"), std::string::npos);
    EXPECT_EQ(cmd.resultReceiver_.find("getAdditionalInfoForAllUser successfully"), std::string::npos);
}
}  // namespace OHOS