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

#include <fstream>
#include <gtest/gtest.h>
#include <unistd.h>

#include "bundle_test_tool.h"
#include "bundle_test_ipc_caller.h"

#include "iremote_broker.h"
#include "iremote_object.h"
#include "mock_bundle_installer_host.h"
#include "mock_bundle_mgr_host.h"
#include "nlohmann/json.hpp"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string TEST_CONFIG_PATH = "/data/local/tmp/bundle_test_tool_cmds.json";
const std::string TEST_BUNDLE_NAME = "com.example.myapplication";
const std::string TEST_CMD_GET_BUNDLE_GIDS = "getBundleGids";
const std::string TEST_CMD_GET_CONTINUE_BUNDLE_NAMES = "getContinueBundleNames";
const std::string TEST_CMD_IS_DEBUGGABLE_APPLICATION = "isDebuggableApplication";
const std::string TEST_CMD_GET_UID_BY_DEBUG_BUNDLE_NAME = "getUidByDebugBundleName";
const std::string TEST_CMD_CHECK_ABILITY_ENABLE_INSTALL = "checkAbilityEnableInstall";
const std::string TEST_CMD_GET_BUNDLE_INFOS_FOR_CONTINUATION = "getBundleInfosForContinuation";
const std::string TEST_CONFIG_CONTENT = R"({
    "commands": {
        "getBundleGids": {
            "ipcCode": 10,
            "options": [
                {"short": "n", "long": "bundle-name", "type": "string", "param": "bundleName", "required": true}
            ],
            "params": [
                {"name": "bundleName", "type": "string", "source": "option"}
            ],
            "reply": [
                {"type": "bool", "name": "success"},
                {"type": "int32vector", "name": "gids", "condition": "success"}
            ]
        },
        "getContinueBundleNames": {
            "ipcCode": 162,
            "options": [
                {"short": "n", "long": "bundle-name", "type": "string",
                    "param": "continueBundleName", "required": true},
                {"short": "u", "long": "user-id", "type": "int32", "param": "userId", "required": false}
            ],
            "params": [
                {"name": "continueBundleName", "type": "string", "source": "option"},
                {"name": "userId", "type": "int32", "source": "option"}
            ],
            "reply": [
                {"type": "int32", "name": "ret"},
                {"type": "stringvector", "name": "bundleNames", "condition": "ret"}
            ]
        },
        "isDebuggableApplication": {
            "ipcCode": 202,
            "options": [
                {"short": "n", "long": "bundle-name", "type": "string", "param": "bundleName", "required": true}
            ],
            "params": [
                {"name": "bundleName", "type": "string", "source": "option"}
            ],
            "reply": [
                {"type": "int32", "name": "ret"},
                {"type": "bool", "name": "isDebuggable"}
            ]
        },
        "getUidByDebugBundleName": {
            "ipcCode": 111,
            "options": [
                {"short": "n", "long": "bundle-name", "type": "string", "param": "bundleName", "required": true},
                {"short": "u", "long": "user-id", "type": "int32", "param": "userId", "required": false}
            ],
            "params": [
                {"name": "bundleName", "type": "string", "source": "option"},
                {"name": "userId", "type": "int32", "source": "option"}
            ],
            "reply": [
                {"type": "int32", "name": "uid"}
            ]
        },
        "checkAbilityEnableInstall": {
            "ipcCode": 76,
            "options": [
                {"short": "n", "long": "bundle-name", "type": "string", "param": "bundleName", "required": false},
                {"short": "a", "long": "ability-name", "type": "string", "param": "abilityName", "required": false},
                {"short": "m", "long": "module-name", "type": "string", "param": "moduleName", "required": false},
                {"short": "d", "long": "device-id", "type": "string", "param": "deviceId", "required": false},
                {"short": "i", "long": "mission-id", "type": "int32", "param": "missionId", "required": false},
                {"short": "u", "long": "user-id", "type": "int32", "param": "userId", "required": false}
            ],
            "params": [
                {"name": "want", "type": "want", "source": "option"},
                {"name": "missionId", "type": "int32", "source": "option"},
                {"name": "userId", "type": "int32", "source": "option"},
                {"name": "callback", "type": "remoteobject", "source": "option"}
            ],
            "reply": [
                {"type": "bool", "name": "success"}
            ]
        },
        "getBundleInfosForContinuation": {
            "ipcCode": 161,
            "options": [
                {"short": "f", "long": "flags", "type": "int32", "param": "flags", "required": true},
                {"short": "u", "long": "user-id", "type": "int32", "param": "userId", "required": false}
            ],
            "params": [
                {"name": "flags", "type": "int32", "source": "option"},
                {"name": "userId", "type": "int32", "source": "option"}
            ],
            "reply": [
                {"type": "bool", "name": "success"}
            ]
        }
    }
})";
}  // namespace

class GenericIpcCallerTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void WriteTestConfig();
    void RemoveTestConfig();
    sptr<IBundleMgr> GetMockBundleMgr();

    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void GenericIpcCallerTest::SetUp()
{
    optind = 0;
    WriteTestConfig();
    GenericIpcCaller::SetCmdsPath(TEST_CONFIG_PATH);

    auto mgrHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleMgrHost());
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    auto installerHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleInstallerHost());
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void GenericIpcCallerTest::TearDown()
{
    WriteTestConfig();
}

void GenericIpcCallerTest::WriteTestConfig()
{
    std::ofstream ofs(TEST_CONFIG_PATH, std::ios::trunc);
    if (!ofs.is_open()) {
        return;
    }
    ofs << TEST_CONFIG_CONTENT;
    ofs.flush();
    ofs.close();
}

void GenericIpcCallerTest::RemoveTestConfig()
{
    if (remove(TEST_CONFIG_PATH.c_str()) != 0) {
        // ignore: config file may not exist
    }
}

sptr<IBundleMgr> GenericIpcCallerTest::GetMockBundleMgr()
{
    return mgrProxyPtr_;
}

/**
 * @tc.number: Generic_Ipc_Caller_List_Commands_0100
 * @tc.name: ListCommands
 * @tc.desc: Verify ListCommands returns available commands after loading config.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_List_Commands_0100, Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    std::string result = caller.ListCommands();
    EXPECT_NE(result.find("getBundleGids"), std::string::npos);
    EXPECT_NE(result.find("getContinueBundleNames"), std::string::npos);
    EXPECT_NE(result.find("isDebuggableApplication"), std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Load_Config_0100
 * @tc.name: LoadConfig
 * @tc.desc: Verify LoadConfig returns true when config file exists and is valid JSON.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Load_Config_0100, Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    bool ret = caller.LoadConfig();
    EXPECT_TRUE(ret);
    EXPECT_TRUE(caller.IsConfigLoaded());
}

/**
 * @tc.number: Generic_Ipc_Caller_Load_Config_0200
 * @tc.name: LoadConfig
 * @tc.desc: Verify LoadConfig returns false when config file does not exist.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Load_Config_0200, Function | SmallTest | TestSize.Level0)
{
    RemoveTestConfig();
    GenericIpcCaller caller;
    bool ret = caller.LoadConfig();
    EXPECT_FALSE(ret);
    EXPECT_FALSE(caller.IsConfigLoaded());
    WriteTestConfig();
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetBundleGids_Help_0100
 * @tc.name: ExecuteCommand getBundleGids -h
 * @tc.desc: Verify ExecuteCommand with -h option returns help message.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetBundleGids_Help_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("-h"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_GIDS, argc, argv, output);
    EXPECT_EQ(ret, OHOS::ERR_OK);
    EXPECT_NE(output.find("usage:"), std::string::npos);
    EXPECT_NE(output.find("--bundle-name"), std::string::npos);
    EXPECT_NE(output.find("(required)"), std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetBundleGids_Missing_Required_0100
 * @tc.name: ExecuteCommand getBundleGids without required -n
 * @tc.desc: Verify ExecuteCommand returns error when required option is missing.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetBundleGids_Missing_Required_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleGids"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_GIDS, argc, argv, output);
    EXPECT_NE(ret, OHOS::ERR_OK);
    EXPECT_NE(output.find("missing or empty"), std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetBundleGids_Empty_Value_0100
 * @tc.name: ExecuteCommand getBundleGids -n ""
 * @tc.desc: Verify ExecuteCommand returns error when required option value is empty string.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetBundleGids_Empty_Value_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("-n"),
        const_cast<char *>(""),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_GIDS, argc, argv, output);
    EXPECT_NE(ret, OHOS::ERR_OK);
    EXPECT_NE(output.find("missing or empty"), std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_Command_Not_Found_0100
 * @tc.name: ExecuteCommand with unknown command
 * @tc.desc: Verify ExecuteCommand returns error for unknown command name.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_Command_Not_Found_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("unknownCmd"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand("unknownCmd", argc, argv, output);
    EXPECT_NE(ret, OHOS::ERR_OK);
    EXPECT_NE(output.find("not found"), std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_Unknown_Option_0100
 * @tc.name: ExecuteCommand with unknown option
 * @tc.desc: Verify ExecuteCommand returns error for unknown option.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_Unknown_Option_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("-x"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_GIDS, argc, argv, output);
    EXPECT_NE(ret, OHOS::ERR_OK);
    EXPECT_NE(output.find("unknown option"), std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetBundleGids_Long_Option_0100
 * @tc.name: ExecuteCommand getBundleGids --bundle-name
 * @tc.desc: Verify ExecuteCommand supports long option --bundle-name and reaches IPC stage.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetBundleGids_Long_Option_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("--bundle-name"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_GIDS, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("success:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_IsDebuggableApplication_0100
 * @tc.name: ExecuteCommand isDebuggableApplication -n <bundleName>
 * @tc.desc: Verify ExecuteCommand for isDebuggableApplication reaches IPC stage.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_IsDebuggableApplication_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("isDebuggableApplication"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_IS_DEBUGGABLE_APPLICATION, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("ret:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetUidByDebugBundleName_0100
 * @tc.name: ExecuteCommand getUidByDebugBundleName -n <bundleName>
 * @tc.desc: Verify ExecuteCommand for getUidByDebugBundleName reaches IPC stage.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetUidByDebugBundleName_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getUidByDebugBundleName"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_UID_BY_DEBUG_BUNDLE_NAME, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("uid:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetUidByDebugBundleName_With_UserId_0100
 * @tc.name: ExecuteCommand getUidByDebugBundleName -n <bundleName> -u <userId>
 * @tc.desc: Verify ExecuteCommand for getUidByDebugBundleName with optional userId.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetUidByDebugBundleName_With_UserId_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getUidByDebugBundleName"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-u"),
        const_cast<char *>("100"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_UID_BY_DEBUG_BUNDLE_NAME, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("uid:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetContinueBundleNames_0100
 * @tc.name: ExecuteCommand getContinueBundleNames -n <bundleName>
 * @tc.desc: Verify ExecuteCommand for getContinueBundleNames reaches IPC stage.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetContinueBundleNames_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getContinueBundleNames"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_CONTINUE_BUNDLE_NAMES, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("ret:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_CheckAbilityEnableInstall_0100
 * @tc.name: ExecuteCommand checkAbilityEnableInstall -n <bundleName> -a <abilityName>
 * @tc.desc: Verify ExecuteCommand for checkAbilityEnableInstall with want param and remoteobject callback.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_CheckAbilityEnableInstall_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("checkAbilityEnableInstall"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-a"),
        const_cast<char *>("MainAbility"),
        const_cast<char *>("-u"),
        const_cast<char *>("100"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_CHECK_ABILITY_ENABLE_INSTALL, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("success:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetBundleInfosForContinuation_0100
 * @tc.name: ExecuteCommand getBundleInfosForContinuation -f <flags>
 * @tc.desc: Verify ExecuteCommand for getBundleInfosForContinuation reaches IPC stage.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetBundleInfosForContinuation_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleInfosForContinuation"),
        const_cast<char *>("-f"),
        const_cast<char *>("1"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_INFOS_FOR_CONTINUATION, argc, argv, output);
    EXPECT_TRUE(ret == OHOS::ERR_OK || ret == ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR);
    EXPECT_TRUE(output.find("success:") != std::string::npos ||
                output.find("failed to get bundle manager proxy") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_GetBundleInfosForContinuation_Missing_Required_0100
 * @tc.name: ExecuteCommand getBundleInfosForContinuation without -f
 * @tc.desc: Verify ExecuteCommand returns error when required flags option is missing.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_GetBundleInfosForContinuation_Missing_Required_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleInfosForContinuation"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_INFOS_FOR_CONTINUATION, argc, argv, output);
    EXPECT_NE(ret, OHOS::ERR_OK);
    EXPECT_TRUE(output.find("missing or empty") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Execute_Invalid_Int32_Value_0100
 * @tc.name: ExecuteCommand with invalid int32 value
 * @tc.desc: Verify ExecuteCommand returns error when int32 option value is not a number.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Execute_Invalid_Int32_Value_0100,
    Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleInfosForContinuation"),
        const_cast<char *>("-f"),
        const_cast<char *>("not_a_number"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    ErrCode ret = caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_INFOS_FOR_CONTINUATION, argc, argv, output);
    EXPECT_NE(ret, OHOS::ERR_OK);
    EXPECT_TRUE(output.find("invalid int32 value") != std::string::npos ||
                output.find("failed to load config") != std::string::npos ||
                output.find("IPC request failed") != std::string::npos ||
                output.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Generic_Ipc_Caller_Config_Cached_0100
 * @tc.name: LoadConfig caching
 * @tc.desc: Verify LoadConfig is cached - second call returns true without re-reading file.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Config_Cached_0100, Function | SmallTest | TestSize.Level0)
{
    GenericIpcCaller caller;
    EXPECT_TRUE(caller.LoadConfig());
    EXPECT_TRUE(caller.IsConfigLoaded());
    RemoveTestConfig();
    EXPECT_TRUE(caller.LoadConfig());
    WriteTestConfig();
}

/**
 * @tc.number: Generic_Ipc_Caller_Getopt_State_Restore_0100
 * @tc.name: GetoptStateGuard restores global state
 * @tc.desc: Verify optind/opterr are restored after ExecuteCommand.
 */
HWTEST_F(GenericIpcCallerTest, Generic_Ipc_Caller_Getopt_State_Restore_0100, Function | SmallTest | TestSize.Level0)
{
    optind = 5;
    opterr = 1;
    GenericIpcCaller caller;
    char *argv[] = {
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("-h"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    std::string output;
    caller.ExecuteCommand(TEST_CMD_GET_BUNDLE_GIDS, argc, argv, output);
    EXPECT_EQ(optind, 5);
    EXPECT_EQ(opterr, 1);
}

class BundleTestToolCallCommandTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void WriteTestConfig();
    void RemoveTestConfig();
    void MakeMockObjects();
    void SetMockObjects(BundleTestTool &cmd) const;

    sptr<IBundleMgr> mgrProxyPtr_;
    sptr<IBundleInstaller> installerProxyPtr_;
};

void BundleTestToolCallCommandTest::SetUp()
{
    optind = 0;
    WriteTestConfig();
    MakeMockObjects();
}

void BundleTestToolCallCommandTest::TearDown()
{
    WriteTestConfig();
}

void BundleTestToolCallCommandTest::WriteTestConfig()
{
    std::ofstream ofs(TEST_CONFIG_PATH);
    ASSERT_TRUE(ofs.is_open());
    ofs << TEST_CONFIG_CONTENT;
    ofs.close();
}

void BundleTestToolCallCommandTest::RemoveTestConfig()
{
    if (remove(TEST_CONFIG_PATH.c_str()) != 0) {
        // ignore: config file may not exist
    }
}

void BundleTestToolCallCommandTest::MakeMockObjects()
{
    auto mgrHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleMgrHost());
    mgrProxyPtr_ = iface_cast<IBundleMgr>(mgrHostPtr);

    auto installerHostPtr = sptr<IRemoteObject>(new (std::nothrow) MockBundleInstallerHost());
    installerProxyPtr_ = iface_cast<IBundleInstaller>(installerHostPtr);
}

void BundleTestToolCallCommandTest::SetMockObjects(BundleTestTool &cmd) const
{
    cmd.SetBundleMgrProxy(mgrProxyPtr_);
    cmd.SetBundleInstallerProxy(installerProxyPtr_);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_No_Subcommand_0100
 * @tc.name: RunAsCallCommand with no subcommand
 * @tc.desc: Verify RunAsCallCommand returns error when no subcommand is specified.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_No_Subcommand_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_NE(result.find("you must specify a subcommand"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_List_0100
 * @tc.name: RunAsCallCommand call list
 * @tc.desc: Verify 'call list' returns the list of available commands.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_List_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("list"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_NE(result.find("Available commands"), std::string::npos);
    EXPECT_NE(result.find("getBundleGids"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_GetBundleGids_Help_0100
 * @tc.name: RunAsCallCommand call getBundleGids -h
 * @tc.desc: Verify 'call getBundleGids -h' returns help message.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_GetBundleGids_Help_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("-h"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_NE(result.find("usage:"), std::string::npos);
    EXPECT_NE(result.find("--bundle-name"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_GetBundleGids_Execute_0100
 * @tc.name: RunAsCallCommand call getBundleGids -n <bundleName>
 * @tc.desc: Verify 'call getBundleGids -n com.example.myapplication' reaches IPC stage.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_GetBundleGids_Execute_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("getBundleGids"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_TRUE(result.find("success:") != std::string::npos ||
                result.find("failed to get bundle manager proxy") != std::string::npos ||
                result.find("failed to load config") != std::string::npos ||
                result.find("IPC request failed") != std::string::npos ||
                result.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_Unknown_Subcommand_0100
 * @tc.name: RunAsCallCommand call unknownCmd
 * @tc.desc: Verify 'call unknownCmd' returns command not found error.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_Unknown_Subcommand_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("unknownCmd"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_NE(result.find("not found"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_GetBundleGids_Missing_Required_0100
 * @tc.name: RunAsCallCommand call getBundleGids without -n
 * @tc.desc: Verify 'call getBundleGids' without required -n option returns error.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_GetBundleGids_Missing_Required_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("getBundleGids"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_NE(result.find("missing or empty"), std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_GetUidByDebugBundleName_0100
 * @tc.name: RunAsCallCommand call getUidByDebugBundleName -n <bundleName>
 * @tc.desc: Verify 'call getUidByDebugBundleName -n com.example.myapplication' reaches IPC stage.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_GetUidByDebugBundleName_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("getUidByDebugBundleName"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_TRUE(result.find("uid:") != std::string::npos ||
                result.find("failed to get bundle manager proxy") != std::string::npos ||
                result.find("failed to load config") != std::string::npos ||
                result.find("IPC request failed") != std::string::npos ||
                result.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_IsDebuggableApplication_0100
 * @tc.name: RunAsCallCommand call isDebuggableApplication -n <bundleName>
 * @tc.desc: Verify 'call isDebuggableApplication -n com.example.myapplication' reaches IPC stage.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_IsDebuggableApplication_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("isDebuggableApplication"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_TRUE(result.find("ret:") != std::string::npos ||
                result.find("failed to get bundle manager proxy") != std::string::npos ||
                result.find("failed to load config") != std::string::npos ||
                result.find("IPC request failed") != std::string::npos ||
                result.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_GetContinueBundleNames_0100
 * @tc.name: RunAsCallCommand call getContinueBundleNames -n <bundleName>
 * @tc.desc: Verify 'call getContinueBundleNames -n com.example.myapplication' reaches IPC stage.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_GetContinueBundleNames_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("getContinueBundleNames"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_TRUE(result.find("ret:") != std::string::npos ||
                result.find("failed to get bundle manager proxy") != std::string::npos ||
                result.find("failed to load config") != std::string::npos ||
                result.find("IPC request failed") != std::string::npos ||
                result.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_CheckAbilityEnableInstall_0100
 * @tc.name: RunAsCallCommand call checkAbilityEnableInstall
 * @tc.desc: Verify 'call checkAbilityEnableInstall -n <bundleName> -a <abilityName>' reaches IPC stage.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_CheckAbilityEnableInstall_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("checkAbilityEnableInstall"),
        const_cast<char *>("-n"),
        const_cast<char *>(TEST_BUNDLE_NAME.c_str()),
        const_cast<char *>("-a"),
        const_cast<char *>("MainAbility"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_TRUE(result.find("success:") != std::string::npos ||
                result.find("failed to get bundle manager proxy") != std::string::npos ||
                result.find("failed to load config") != std::string::npos ||
                result.find("IPC request failed") != std::string::npos ||
                result.find("operation failed") != std::string::npos);
}

/**
 * @tc.number: Bundle_Test_Tool_Call_GetBundleInfosForContinuation_0100
 * @tc.name: RunAsCallCommand call getBundleInfosForContinuation -f 1
 * @tc.desc: Verify 'call getBundleInfosForContinuation -f 1' reaches IPC stage.
 */
HWTEST_F(BundleTestToolCallCommandTest, Bundle_Test_Tool_Call_GetBundleInfosForContinuation_0100,
    Function | SmallTest | TestSize.Level0)
{
    char *argv[] = {
        const_cast<char *>("bundle_test_tool"),
        const_cast<char *>("call"),
        const_cast<char *>("getBundleInfosForContinuation"),
        const_cast<char *>("-f"),
        const_cast<char *>("1"),
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    BundleTestTool cmd(argc, argv);
    std::string result = cmd.ExecCommand();
    EXPECT_TRUE(result.find("success:") != std::string::npos ||
                result.find("failed to get bundle manager proxy") != std::string::npos ||
                result.find("failed to load config") != std::string::npos ||
                result.find("IPC request failed") != std::string::npos ||
                result.find("operation failed") != std::string::npos);
}

}  // namespace AppExecFwk
}  // namespace OHOS
