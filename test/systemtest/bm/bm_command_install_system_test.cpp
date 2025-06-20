/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include <thread>
#include "bundle_command.h"
#include "tool_system_test.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
const std::string STRING_BUNDLE_PATH = "/data/test/resource/bm/test_one.hap";
const std::string STRING_BUNDLE_PATH_INVALID = STRING_BUNDLE_PATH + ".invalid";
const std::string STRING_BUNDLE_NAME = "com.test.bundlename.one";
const std::string INSTALL_FALSE = "error: install file path invalid.";
const std::string ERR_CODE = "code:9568269\n";
}  // namespace

class BmCommandInstallSystemTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BmCommandInstallSystemTest::SetUpTestCase()
{}

void BmCommandInstallSystemTest::TearDownTestCase()
{}

void BmCommandInstallSystemTest::SetUp()
{
    // reset optind to 0
    optind = 0;
}

void BmCommandInstallSystemTest::TearDown()
{}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0100
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0100, Function | MediumTest | TestSize.Level0)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // uninstall the bundle
    std::string command = "bm uninstall -n " + STRING_BUNDLE_NAME;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);
    EXPECT_PRED2(ToolSystemTest::IsSubSequence, commandResult, STRING_UNINSTALL_BUNDLE_OK + "\n");
}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0200
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path>" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0200, Function | MediumTest | TestSize.Level0)
{
    // install an invalid bundle
    std::string command = "bm install -p " + STRING_BUNDLE_PATH_INVALID;
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_INSTALL_BUNDLE_NG + "\n" + ERR_CODE + INSTALL_FALSE + "\n");
}

/**
 * @tc.number: Bm_Command_Install_SystemTest_0300
 * @tc.name: ExecCommand
 * @tc.desc: Verify the "bm install -p <bundle-path> -r" command.
 */
HWTEST_F(BmCommandInstallSystemTest, Bm_Command_Install_SystemTest_0300, Function | MediumTest | TestSize.Level1)
{
    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);

    // install a valid bundle
    ToolSystemTest::InstallBundle(STRING_BUNDLE_PATH, true);

    // install the same bundle
    std::string command = "bm install -p " + STRING_BUNDLE_PATH + " -r";
    std::string commandResult = ToolSystemTest::ExecuteCommand(command);

    EXPECT_EQ(commandResult, STRING_INSTALL_BUNDLE_OK + "\n");

    // uninstall the bundle
    ToolSystemTest::UninstallBundle(STRING_BUNDLE_NAME);
}
} // OHOS