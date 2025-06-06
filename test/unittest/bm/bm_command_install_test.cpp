/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#define private public
#include "bundle_command.h"
#undef private
#include "bundle_constants.h"
#include "bundle_installer_interface.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "mock_bundle_mgr_host.h"
#include "mock_bundle_installer_host.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
class BmCommandInstallTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleManagerShellCommand &cmd) const;

    std::string cmd_ = "install";
    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BmCommandInstallTest::SetUpTestCase()
{}

void BmCommandInstallTest::TearDownTestCase()
{}

void BmCommandInstallTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void BmCommandInstallTest::TearDown()
{}

void BmCommandInstallTest::MakeMockObjects()
{
    // mock a mgr host
    auto mgrHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleMgrHost());
    // mock a mgr proxy
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    // mock a installer host
    auto installerHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleInstallerHost());
    // mock a installer proxy
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void BmCommandInstallTest::SetMockObjects(BundleManagerShellCommand &cmd) const
{
    // set the mock mgr proxy
    cmd.bundleMgrProxy_ = mgrProxyPtr_;

    // set the mock installer proxy
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bm_Command_Install_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0100, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0200, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -x" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0300, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-x"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0400, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --x" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0500, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--x"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0600, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --h" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0700, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-h"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --help" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0800, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--help"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_0900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_0900, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_1000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -r" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1000, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-r"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(
        cmd.ExecCommand(), "error: you must specify a bundle path with '-p' or '--bundle-path'.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_1100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1100, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -r" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1200, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>("-r"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -r -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1300, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-r"),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1600, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1700, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_INSTALL_PATH1.c_str()),
        const_cast<char*>(STRING_BUNDLE_INSTALL_PATH2.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1800, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_BUNDLE_INSTALL_PATH2.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_1900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install --bundle-path <bundle-path> <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_1900, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--bundle-path"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2000, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--bundle-path"),
        const_cast<char*>(STRING_BUNDLE_INSTALL_PATH1.c_str()),
        const_cast<char*>(STRING_BUNDLE_INSTALL_PATH2.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2100, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--bundle-path"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_BUNDLE_INSTALL_PATH2.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -r" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2300, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>("-r"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -r -p <bundle-path> <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2400, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-r"),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_2600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p -r <bundle-path> <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2600, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>("-r"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_2700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -u xxx" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2700, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>("-u"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_2800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> <bundle-path> -u" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_2800, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>("-u"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_3000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -w" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3000, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>("-w"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_3100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -w <waitting-time>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3100, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>("-w"),
        const_cast<char*>(DEFAULT_WAIT_TIME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_3200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -XXX <user-id>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3200, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(STRING_OTHER_BUNDLE_PATH.c_str()),
        const_cast<char*>("-XXX"),
        const_cast<char*>(DEFAULT_USER_ID.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_3300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -w <waitting-time>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3300, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>("-w"),
        const_cast<char*>(MINIMUMT_WAIT_TIME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: option requires a correct value.\n");
}

/**
 * @tc.number: Bm_Command_Install_3400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -w <waitting-time>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3400, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-p"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>("-w"),
        const_cast<char*>(MAXIMUM_WAIT_TIME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: option requires a correct value.\n");
}

/**
 * @tc.number: Bm_Command_Install_3500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3500, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-cc"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_INSTALL);
}

/**
 * @tc.number: Bm_Command_Install_3600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3600, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_3700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3700, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--shared-bundle-dir-path"),
        const_cast<char*>(STRING_BUNDLE_PATH.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_INSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_3800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3800, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("-r"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_3900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_3900, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("--replace"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_4000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_4000, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("-p"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_4100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_4100, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("--bundle-path"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_4200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_4200, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("-u"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_4300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_4300, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("--user-id"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_4400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_4400, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("-w"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}

/**
 * @tc.number: Bm_Command_Install_4500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -s <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallTest, Bm_Command_Install_4500, Function | MediumTest | TestSize.Level1)
{
    // install a bundle
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>("--waitting-time"),
        const_cast<char*>("xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE);
}
} // OHOS