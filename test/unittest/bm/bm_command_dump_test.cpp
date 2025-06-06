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
#include "mock_bundle_installer_host.h"
#include "mock_bundle_mgr_host.h"

using namespace testing::ext;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
class BmCommandDumpTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void MakeMockObjects();
    void SetMockObjects(BundleManagerShellCommand &cmd) const;

    std::string cmd_ = "dump";
    std::string shared_ = "dump-shared";
    std::string sharedDependencies_ = "dump-shared-dependencies";
    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BmCommandDumpTest::SetUpTestCase()
{}

void BmCommandDumpTest::TearDownTestCase()
{}

void BmCommandDumpTest::SetUp()
{
    // reset optind to 0
    optind = 0;

    // make mock objects
    MakeMockObjects();
}

void BmCommandDumpTest::TearDown()
{}

void BmCommandDumpTest::MakeMockObjects()
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

void BmCommandDumpTest::SetMockObjects(BundleManagerShellCommand &cmd) const
{
    // set the mock mgr proxy
    cmd.bundleMgrProxy_ = mgrProxyPtr_;

    // set the mock installer proxy
    cmd.bundleInstallerProxy_ = installerProxyPtr_;
}

/**
 * @tc.number: Bm_Command_Dump_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0100, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump xxx" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0200, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -x" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0300, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -xxx" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0400, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump --x" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0500, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0600
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump --xxx" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0600, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -h" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0700, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump --help" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0800, Function | MediumTest | TestSize.Level1)
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

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_0900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -a" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_0900, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-a"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "OK");
}

/**
 * @tc.number: Bm_Command_Dump_1000
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump --all" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1000, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("--all"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "OK");
}

/**
 * @tc.number: Bm_Command_Dump_1100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1100, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_1200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name>" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1200, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_BUNDLE_NAME + "\n");
}

/**
 * @tc.number: Bm_Command_Dump_1300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -s" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1300, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_1400
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name> -s" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1400, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>("-s"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_BUNDLE_NAME + "\n");
}

/**
 * @tc.number: Bm_Command_Dump_1500
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name> -u" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1500, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>("-u"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_1700
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name> -d" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1700, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>("-d"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_1800
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name> -d <device-id>" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1800, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>("-d"),
        const_cast<char*>(DEFAULT_DEVICE_TIME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_DUMP_FAILED + "\n");
}

/**
 * @tc.number: Bm_Command_Dump_1900
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -n <bundle-name> -XXX <user-id>" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_1900, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>("-XXX"),
        const_cast<char*>(DEFAULT_USER_ID.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_2100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -xx <bundle-name>" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_2100, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-xx"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Dump_2200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump -d <device-id>" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Dump_2200, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(cmd_.c_str()),
        const_cast<char*>("-d"),
        const_cast<char*>(DEFAULT_DEVICE_TIME.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;
    BundleManagerShellCommand cmd(argc, argv);
    // set the mock objects
    SetMockObjects(cmd);
    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n" + HELP_MSG_DUMP);
}

/**
 * @tc.number: Bm_Command_Shared_0001
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump-shared" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Shared_0001, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(shared_.c_str()),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_NO_OPTION + "\n" + HELP_MSG_DUMP_SHARED);
}

/**
 * @tc.number: Bm_Command_Shared_0002
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump-shared -n" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Shared_0002, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(shared_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), STRING_REQUIRE_CORRECT_VALUE + HELP_MSG_DUMP_SHARED);
}

/**
 * @tc.number: Bm_Command_Shared_0003
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump-shared -xxx" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Shared_0003, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(shared_.c_str()),
        const_cast<char*>("-xxx"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP_SHARED);
}

/**
 * @tc.number: Bm_Command_Shared_0005
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump-shared -h" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Shared_0005, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(shared_.c_str()),
        const_cast<char*>("-h"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), HELP_MSG_DUMP_SHARED);
}

/**
 * @tc.number: Bm_Command_Shared_0006
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm dump-shared -n <bundle-name>" command.
 */
HWTEST_F(BmCommandDumpTest, Bm_Command_Shared_0006, Function | MediumTest | TestSize.Level1)
{
    char *argv[] = {
        const_cast<char*>(TOOL_NAME.c_str()),
        const_cast<char*>(shared_.c_str()),
        const_cast<char*>("-n"),
        const_cast<char*>(STRING_BUNDLE_NAME.c_str()),
        const_cast<char*>("-t"),
        const_cast<char*>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]) - 1;

    BundleManagerShellCommand cmd(argc, argv);

    // set the mock objects
    SetMockObjects(cmd);

    EXPECT_EQ(cmd.ExecCommand(), "error: unknown option.\n" + HELP_MSG_DUMP_SHARED);
}

} // namespace OHOS