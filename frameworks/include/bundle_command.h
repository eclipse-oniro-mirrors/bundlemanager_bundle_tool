/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_APPEXECFWK_STANDARD_TOOLS_BM_INCLUDE_BUNDLE_COMMAND_H
#define FOUNDATION_APPEXECFWK_STANDARD_TOOLS_BM_INCLUDE_BUNDLE_COMMAND_H

#include "shell_command.h"
#include "bundle_mgr_interface.h"
#include "bundle_installer_interface.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string TOOL_NAME = "bm";

const std::string HELP_MSG = "usage: bm <command> <options>\n"
                             "These are common bm commands list:\n"
                             "  help         list available commands\n"
                             "  install      install a bundle with options\n"
                             "  uninstall    uninstall a bundle with options\n"
                             "  install-plugin install a plugin with options\n"
                             "  uninstall-plugin  uninstall a plugin with option\n"
                             "  dump         dump the bundle info\n"
                             "  get          obtain device udid\n"
                             "  quickfix     quick fix, including query and install\n"
                             "  compile      Compile the software package\n"
                             "  copy-ap      Copy software ap file to /data/local/pgo\n"
                             "  dump-overlay dump overlay info of the specific overlay bundle\n"
                             "  dump-target-overlay dump overlay info of the specific target bundle\n"
                             "  dump-dependencies dump dependencies by given bundle name and module name\n"
                             "  dump-shared dump inter-application shared library information by bundle name\n";

const std::string ENABLE_DISABLE_HELP_MSG = "  enable       enable the bundle\n"
                                            "  disable      disable the bundle\n";

const std::string CLEAN_HELP_MSG = "  clean        clean the bundle data\n";

const std::string HELP_MSG_COMPILE =
    "usage: bm compile [-m mode] [-r reset] (bundle-name | -a)\n"
    "options list:\n"
    "  -h, --help                           list available commands.\n"
    "  -m, --mode <mode-name>               select partial or full mode.\n"
    "  -r, --reset                          clear bundle configuration file data.\n"
    "  -a, --all                            compile or reset all software packages.\n";

const std::string HELP_MSG_COPY_AP =
    "usage: bm copy-ap (bundle-name | -a)\n"
    "options list:\n"
    "  -h, --help                           list available commands.\n"
    "  -n, --bundle-name <bundle-name>      copy ap by bundle name\n"
    "  -a, --all                            copy all software packages.\n";

const std::string HELP_MSG_INSTALL =
    "usage: bm install <options>\n"
    "options list:\n"
    "  -h, --help                                                     list available commands\n"
    "  -p, --bundle-path <file-path>                                  install a hap or hsp by a specified path\n"
    "  -p, --bundle-path <file-path> <file-path> ...                  install one bundle by some hap or hsp paths\n"
    "  -p, --bundle-path <bundle-direction>                           install one bundle by a direction,\n"
    "                                                                    under which are some hap or hsp files\n"
    "  -r -p <bundle-file-path>                                       replace an existing bundle\n"
    "  -r --bundle-path <bundle-file-path>                            replace an existing bundle\n"
    "  -s, --shared-bundle-dir-path <shared-bundle-dir-path>          install inter-application hsp files\n"
    "  -w, --waitting-time <waitting-time>                            specify waitting time for installation,\n"
    "                                                                    the minimum waitting time is 180s,\n"
    "                                                                    the maximum waitting time is 600s\n";

const std::string HELP_MSG_UNINSTALL =
    "usage: bm uninstall <options>\n"
    "options list:\n"
    "  -h, --help                           list available commands\n"
    "  -n, --bundle-name <bundle-name>      uninstall a bundle by bundle name\n"
    "  -m, --module-name <module-name>      uninstall a module by module name\n"
    "  -k, --keep-data                      keep the user data after uninstall\n"
    "  -s, --shared                         uninstall inter-application shared library\n"
    "  -v, --version                        uninstall a inter-application shared library by versionCode\n";

const std::string HELP_MSG_UNINSTALL_SHARE =
    "usage: bm uninstall-shared <options>\n"
    "options list:\n"
    "  -h, --help                           list available commands\n"
    "  -n, --bundle-name <bundle-name>      uninstall a share library by bundle name\n"
    "  -v, --version <versionCode>          uninstall a share library by versionCode\n";

const std::string HELP_MSG_DUMP =
    "usage: bm dump <options>\n"
    "options list:\n"
    "  -h, --help                           list available commands\n"
    "  -a, --all                            list all bundles in system\n"
    "  -g, --debug-bundle                   list debug bundles in system\n"
    "  -n, --bundle-name <bundle-name>      list the bundle info by a bundle name\n"
    "  -s, --shortcut-info                  list the shortcut info\n"
    "  -d, --device-id <device-id>          specify a device id\n"
    "  -l, --label                          list the label info\n";

const std::string HELP_MSG_CLEAN =
    "usage: bm clean <options>\n"
    "options list:\n"
    "  -h, --help                                      list available commands\n"
    "  -n, --bundle-name  <bundle-name>                bundle name\n"
    "  -c, --cache                                     clean bundle cache files by bundle name\n"
    "  -d, --data                                      clean bundle data files by bundle name\n"
    "  -i, --app-index <app-index>                     specify a app index\n";

const std::string HELP_MSG_ENABLE =
    "usage: bm enable <options>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       enable bundle by bundle name\n"
    "  -a, --ability-name <ability-name>      enable ability by ability name\n";

const std::string HELP_MSG_DISABLE =
    "usage: bm disable <options>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       disable bundle by bundle name\n"
    "  -a, --ability-name <ability-name>      disable ability by ability name\n";

const std::string HELP_MSG_GET =
    "usage: bm get <options>\n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -u, --udid                             obtain udid of the current device\n";

const std::string HELP_MSG_QUICK_FIX =
    "usage: bm quickfix <options>\n"
    "options list:\n"
    "-h, --help                                   list available commands\n"
    "-q, --query                                  indicates query quickfix, used with -b or --bundle-name\n"
    "-r, --remove                                 indicates remove quickfix, used with -b or --bundle-name\n"
    "-b, --bundle-name <bundle-name>              query quickfix status and information by a specified bundle name\n"
    "-d, --debug                                  apply a quickfix debug mode\n"
    "-a, --apply                                  indicates apply quickfix, used with -f or --file-path\n"
    "-t, --target <target-path>                   indicates a target path to apply quickfix\n"
    "-o, --overwrite                              apply a quickfix in overwrite mode\n"
    "-f, --file-path <file-path>                  apply a quickfix file by a specified path\n"
    "-f, --file-path <file-path> <file-path> ...  apply some quickfix files of one bundle\n"
    "-f, --file-path <bundle-direction>           apply quickfix files by direction, under which are quickfix files\n";

const std::string HELP_MSG_OVERLAY =
    "usage: bm dump-overlay <options>\n"
    "options list:\n"
    "  -h, --help                                         list available commands\n"
    "  -b, --bundle-name <bundle-name>                    bundle name of the overlay bundle\n"
    "  -m, --module-name <module-name>                    module name of the overlay bundle\n"
    "  -t, --target-module-name <target-module-name>      target module name of overlay bundle\n";

const std::string HELP_MSG_OVERLAY_TARGET =
    "usage: bm dump-target-overlay <options>\n"
    "options list:\n"
    "  -h, --help                                         list available commands\n"
    "  -b, --bundle-name <bundle-name>                    bundle name of the target overlay bundle\n"
    "  -m, --module-name <module-name>                    module name of the target overlay bundle\n";

const std::string HELP_MSG_DUMP_SHARED =
    "usage: bm dump-shared <options>\n"
    "eg:bm dump-shared -n <bundle-name> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -a, --all                              list all inter-application shared library name in system\n"
    "  -n, --bundle-name  <bundle-name>       dump inter-application shared library information by bundleName\n";

const std::string HELP_MSG_DUMP_SHARED_DEPENDENCIES =
    "usage: bm dump-dependencies <options>\n"
    "eg:bm dump-dependencies -n <bundle-name> -m <module-name> \n"
    "options list:\n"
    "  -h, --help                             list available commands\n"
    "  -n, --bundle-name  <bundle-name>       dump dependencies by bundleName and moduleName\n"
    "  -m, --module-name  <module-name>       dump dependencies by bundleName and moduleName\n";
const std::string HELP_MSG_INSTALL_PLUGIN =
    "usage: bm install-plugin <options>\n"
    "options list:\n"
    "  -h, --help                                                              list available commands\n"
    "  -n  --host-bundle-name <host-bundle_name> -p --plugin-path <file-path>  install a plugin by hsp path and host bundle name\n";

const std::string HELP_MSG_UNINSTALL_PLUGIN =
    "usage: bm uninstall-plugin <options>\n"
    "options list:\n"
    "  -h, --help                                                              list available commands\n"
    "  -n  --host-bundle-name <host-bundle_name> -p --plugin-bundle-name <plugin-bundle-name>  uninstall a plugin by plugin bundle name and host bundle name\n";

const std::string HELP_MSG_NO_INSTALL_PLUGIN_OPTION =
    "error: you must specify a plugin path with '-p' or '--plugin-path' \n"
        "and a host bundle name with '-n' or '--host-bundle-name' \n";

const std::string HELP_MSG_NO_UNINSTALL_PLUGIN_OPTION =
    "error: you must specify a plugin bundle name with '-p' or '--plugin-bundle-name' \n"
        "and a host bundle name with '-n' or '--host-bundle-name' \n";

const std::string STRING_INCORRECT_OPTION = "error: incorrect option";
const std::string HELP_MSG_NO_BUNDLE_PATH_OPTION =
    "error: you must specify a bundle path with '-p' or '--bundle-path'.";

const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name'.";

const std::string STRING_INSTALL_BUNDLE_OK = "install bundle successfully.";
const std::string STRING_INSTALL_BUNDLE_NG = "error: failed to install bundle.";

const std::string STRING_UNINSTALL_BUNDLE_OK = "uninstall bundle successfully.";
const std::string STRING_UNINSTALL_BUNDLE_NG = "error: failed to uninstall bundle.";

const std::string HELP_MSG_NO_DATA_OR_CACHE_OPTION =
    "error: you must specify '-c' or '-d' for 'bm clean' option.";
const std::string STRING_CLEAN_CACHE_BUNDLE_OK = "clean bundle cache files successfully.";
const std::string STRING_CLEAN_CACHE_BUNDLE_NG = "error: failed to clean bundle cache files.";

const std::string STRING_CLEAN_DATA_BUNDLE_OK = "clean bundle data files successfully.";
const std::string STRING_CLEAN_DATA_BUNDLE_NG = "error: failed to clean bundle data files.";

const std::string STRING_ENABLE_BUNDLE_OK = "enable bundle successfully.";
const std::string STRING_ENABLE_BUNDLE_NG = "error: failed to enable bundle.";

const std::string STRING_DISABLE_BUNDLE_OK = "disable bundle successfully.";
const std::string STRING_DISABLE_BUNDLE_NG = "error: failed to disable bundle.";

const std::string STRING_GET_UDID_OK = "udid of current device is :";
const std::string STRING_GET_UDID_NG = "error: failed to get udid";

const std::string HELP_MSG_NO_REMOVABLE_OPTION =
    "error: you must specify a bundle name with '-n' or '--bundle-name' \n"
    "and a module name with '-m' or '--module-name' \n";

const std::string HELP_MSG_DUMP_FAILED = "error: failed to get information and the parameters may be wrong.";
const std::string HELP_MSG_COMPILE_FAILED = "error: failed to get information and the parameters may be wrong.";
const std::string STRING_REQUIRE_CORRECT_VALUE = "error: option requires a correct value.\n";

const std::string STRING_DUMP_OVERLAY_OK = "overlay info is:";
const std::string STRING_DUMP_OVERLAY_NG = "error: failed to get overlay info";

const std::string STRING_DUMP_TARGET_OVERLAY_OK = "target overlay info is:";
const std::string STRING_DUMP_TARGET_OVERLAY_NG = "error: failed to get target overlay info";
const std::string MSG_ERR_BUNDLEMANAGER_OVERLAY_FEATURE_IS_NOT_SUPPORTED = "feature is not supported.\n";
const std::string COMPILE_SUCCESS_OK = "compile AOT success.\n";
const std::string COMPILE_RESET = "reset AOT success.\n";
const std::string WARNING_USER =
    "Warning: The current user is %. If you want to set the userId as $, please switch to $.\n";
} // namespace

class BundleManagerShellCommand : public ShellCommand {
public:
    BundleManagerShellCommand(int argc, char *argv[]);
    ~BundleManagerShellCommand() override
    {}

private:
    ErrCode CreateCommandMap() override;
    ErrCode CreateMessageMap() override;
    ErrCode Init() override;

    ErrCode RunAsHelpCommand();
    ErrCode RunAsInstallCommand();
    ErrCode RunAsUninstallCommand();
    ErrCode RunAsDumpCommand();
    ErrCode RunAsCleanCommand();
    ErrCode RunAsEnableCommand();
    ErrCode RunAsDisableCommand();
    ErrCode RunAsGetCommand();
    ErrCode RunAsQuickFixCommand();
    ErrCode RunAsDumpOverlay();
    ErrCode RunAsDumpTargetOverlay();
    ErrCode RunAsDumpSharedDependenciesCommand();
    ErrCode RunAsDumpSharedCommand();
    ErrCode RunAsCompileCommand();
    ErrCode RunAsCopyApCommand();
    ErrCode RunAsInstallPluginCommand();
    ErrCode RunAsUninstallPluginCommand();

    std::string CopyAp(const std::string &bundleName, bool isAllBundle) const;

    std::string CompileProcessAot(
        const std::string &bundleName, const std::string &compileMode, bool isAllBundle) const;
    std::string CompileReset(const std::string &bundleName, bool isAllBundle) const;

    std::string DumpBundleList(int32_t userId) const;
    std::string DumpDebugBundleList(int32_t userId) const;
    std::string DumpBundleInfo(const std::string &bundleName, int32_t userId) const;
    std::string DumpShortcutInfos(const std::string &bundleName, int32_t userId) const;
    std::string DumpDistributedBundleInfo(const std::string &deviceId, const std::string &bundleName);
    std::string DumpDependentModuleNames(const std::string &bundleName, const std::string &moduleName) const;
    std::string DumpSharedDependencies(const std::string &bundleName, const std::string &moduleName) const;
    std::string DumpShared(const std::string &bundleName) const;
    std::string DumpSharedAll() const;
    std::string DumpAllLabel(int32_t userId) const;
    std::string DumpBundleLabel(const std::string &bundleName, int32_t userId) const;

    int32_t InstallOperation(const std::vector<std::string> &bundlePaths, InstallParam &installParam,
        int32_t waittingTime, std::string &resultMsg) const;
    int32_t UninstallOperation(const std::string &bundleName, const std::string &moduleName,
                               InstallParam &installParam) const;
    int32_t UninstallSharedOperation(const UninstallParam &uninstallParam) const;
    std::string GetUdid() const;
    bool IsInstallOption(int index) const;
    void GetAbsPaths(const std::vector<std::string> &paths, std::vector<std::string> &absPaths) const;

    ErrCode GetBundlePath(const std::string& param, std::vector<std::string>& bundlePaths) const;

    bool CleanBundleCacheFilesOperation(const std::string &bundleName, int32_t userId, int32_t appIndex = 0) const;
    bool CleanBundleDataFilesOperation(const std::string &bundleName, int32_t userId, int32_t appIndex = 0) const;

    bool SetApplicationEnabledOperation(const AbilityInfo &abilityInfo, bool isEnable, int32_t userId) const;
    std::string DumpOverlayInfo(const std::string &bundleName, const std::string &moduleName,
        const std::string &targetModuleName, int32_t userId);
    std::string DumpTargetOverlayInfo(const std::string &bundleName, const std::string &moduleName, int32_t userId);
    ErrCode ParseSharedDependenciesCommand(int32_t option, std::string &bundleName, std::string &moduleName);
    ErrCode ParseSharedCommand(int32_t option, std::string &bundleName, bool &dumpSharedAll);
    ErrCode ParseCopyApCommand(int32_t option, std::string &bundleName, bool &isAllBundle);
    ErrCode DeployQuickFixDisable(const std::vector<std::string> &quickFixFiles,
        std::shared_ptr<QuickFixResult> &quickFixRes, bool isDebug, const std::string &targetPath) const;
    ErrCode DeleteQuickFix(const std::string &bundleName, std::shared_ptr<QuickFixResult> &quickFixRes) const;
    std::string GetWaringString(int32_t currentUserId, int32_t specifedUserId) const;
    ErrCode ParseInstallPluginCommand(int32_t option, std::string &hostBundleName,
        std::vector<std::string> &pluginPaths);
    ErrCode GetPluginPath(const std::string& param, std::vector<std::string>& pluginPaths) const;
    int32_t TransformErrCode(const int32_t resultCode);
    ErrCode ParseUninstallPluginCommand(int32_t option, std::string &hostBundleName, std::string &pluginBundleName);

    sptr<IBundleMgr> bundleMgrProxy_;
    sptr<IBundleInstaller> bundleInstallerProxy_;

    static std::map<int32_t, int32_t> errCodeMap_;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // FOUNDATION_APPEXECFWK_STANDARD_TOOLS_BM_INCLUDE_BUNDLE_COMMAND_H