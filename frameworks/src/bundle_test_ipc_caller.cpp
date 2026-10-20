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

#include "bundle_test_ipc_caller.h"

#include <fstream>
#include <getopt.h>
#include <map>
#include <sstream>
#include <vector>

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_command_common.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ipc_object_stub.h"
#include "message_parcel.h"
#include "message_option.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::u16string INTERFACE_DESCRIPTOR = u"ohos.appexecfwk.BundleMgr";
const std::string TAG_SUCCESS = "success";
const std::string TYPE_STRING = "string";
const std::string TYPE_INT32 = "int32";
const std::string TYPE_BOOL = "bool";
const std::string TYPE_INT32VECTOR = "int32vector";
const std::string TYPE_STRINGVECTOR = "stringvector";
const std::string TYPE_WANT = "want";
const std::string TYPE_REMOTEOBJECT = "remoteobject";
const std::string SOURCE_OPTION = "option";
const std::string FIELD_IPC_CODE = "ipcCode";
const std::string FIELD_OPTIONS = "options";
const std::string FIELD_PARAMS = "params";
const std::string FIELD_REPLY = "reply";
const std::string FIELD_COMMANDS = "commands";
const std::string FIELD_SHORT = "short";
const std::string FIELD_LONG = "long";
const std::string FIELD_TYPE = "type";
const std::string FIELD_PARAM = "param";
const std::string FIELD_REQUIRED = "required";
const std::string FIELD_NAME = "name";
const std::string FIELD_SOURCE = "source";
const std::string FIELD_CONDITION = "condition";
const char HELP_SHORT_OPT = 'h';
const char HELP_LONG_OPT_VAL = 'h';

struct GetoptStateGuard {
    int savedOptind;
    int savedOpterr;
    GetoptStateGuard() : savedOptind(optind), savedOpterr(opterr) {}
    ~GetoptStateGuard()
    {
        optind = savedOptind;
        opterr = savedOpterr;
    }
};
}  // namespace

struct GenericIpcCaller::Impl {
    struct OptionDef {
        char shortOpt = 0;
        std::string longOpt;
        std::string type;
        std::string paramName;
        bool required = false;
    };

    struct ParamDef {
        std::string name;
        std::string type;
        std::string source;
    };

    struct ReplyDef {
        std::string type;
        std::string name;
        std::string condition;
    };

    struct CommandDef {
        uint32_t ipcCode = 0;
        std::vector<OptionDef> options;
        std::vector<ParamDef> params;
        std::vector<ReplyDef> reply;
    };

    std::map<std::string, CommandDef> commands;

    bool ParseFromJson(const nlohmann::json &root)
    {
        auto commandsIter = root.find(FIELD_COMMANDS);
        if (commandsIter == root.end() || !commandsIter->is_object()) {
            APP_LOGE("commands field missing or not object.");
            return false;
        }
        for (const auto &cmdPair : commandsIter->items()) {
            const std::string &cmdName = cmdPair.key();
            const auto &cmdJson = cmdPair.value();
            if (!cmdJson.is_object()) {
                APP_LOGE("command %{public}s is not object.", cmdName.c_str());
                continue;
            }
            CommandDef def;
            if (!ParseCommand(cmdJson, def)) {
                APP_LOGE("parse command %{public}s failed.", cmdName.c_str());
                continue;
            }
            commands[cmdName] = std::move(def);
        }
        return !commands.empty();
    }

    bool ParseCommand(const nlohmann::json &cmdJson, CommandDef &def)
    {
        auto ipcIter = cmdJson.find(FIELD_IPC_CODE);
        if (ipcIter == cmdJson.end() || !ipcIter->is_number_unsigned()) {
            APP_LOGE("ipcCode missing or invalid.");
            return false;
        }
        def.ipcCode = ipcIter->get<uint32_t>();

        auto optsIter = cmdJson.find(FIELD_OPTIONS);
        if (optsIter != cmdJson.end() && optsIter->is_array()) {
            for (const auto &opt : *optsIter) {
                OptionDef od;
                if (!ParseOption(opt, od)) {
                    continue;
                }
                def.options.push_back(std::move(od));
            }
        }

        auto paramsIter = cmdJson.find(FIELD_PARAMS);
        if (paramsIter != cmdJson.end() && paramsIter->is_array()) {
            for (const auto &param : *paramsIter) {
                ParamDef pd;
                if (!ParseParam(param, pd)) {
                    continue;
                }
                def.params.push_back(std::move(pd));
            }
        }

        auto replyIter = cmdJson.find(FIELD_REPLY);
        if (replyIter != cmdJson.end() && replyIter->is_array()) {
            for (const auto &rep : *replyIter) {
                ReplyDef rd;
                if (!ParseReply(rep, rd)) {
                    continue;
                }
                def.reply.push_back(std::move(rd));
            }
        }
        return true;
    }

    bool ParseOption(const nlohmann::json &opt, OptionDef &od)
    {
        auto shortIter = opt.find(FIELD_SHORT);
        if (shortIter == opt.end() || !shortIter->is_string()) {
            APP_LOGE("option short missing.");
            return false;
        }
        std::string shortStr = shortIter->get<std::string>();
        if (shortStr.empty()) {
            APP_LOGE("option short empty.");
            return false;
        }
        od.shortOpt = shortStr[0];

        auto longIter = opt.find(FIELD_LONG);
        if (longIter != opt.end() && longIter->is_string()) {
            od.longOpt = longIter->get<std::string>();
        }

        auto typeIter = opt.find(FIELD_TYPE);
        if (typeIter != opt.end() && typeIter->is_string()) {
            od.type = typeIter->get<std::string>();
        }

        auto paramIter = opt.find(FIELD_PARAM);
        if (paramIter != opt.end() && paramIter->is_string()) {
            od.paramName = paramIter->get<std::string>();
        }

        auto reqIter = opt.find(FIELD_REQUIRED);
        if (reqIter != opt.end() && reqIter->is_boolean()) {
            od.required = reqIter->get<bool>();
        }
        return true;
    }

    bool ParseParam(const nlohmann::json &param, ParamDef &pd)
    {
        auto nameIter = param.find(FIELD_NAME);
        if (nameIter == param.end() || !nameIter->is_string()) {
            APP_LOGE("param name missing.");
            return false;
        }
        pd.name = nameIter->get<std::string>();

        auto typeIter = param.find(FIELD_TYPE);
        if (typeIter != param.end() && typeIter->is_string()) {
            pd.type = typeIter->get<std::string>();
        }

        auto srcIter = param.find(FIELD_SOURCE);
        if (srcIter != param.end() && srcIter->is_string()) {
            pd.source = srcIter->get<std::string>();
        }
        return true;
    }

    bool ParseReply(const nlohmann::json &rep, ReplyDef &rd)
    {
        auto typeIter = rep.find(FIELD_TYPE);
        if (typeIter == rep.end() || !typeIter->is_string()) {
            APP_LOGE("reply type missing.");
            return false;
        }
        rd.type = typeIter->get<std::string>();

        auto nameIter = rep.find(FIELD_NAME);
        if (nameIter != rep.end() && nameIter->is_string()) {
            rd.name = nameIter->get<std::string>();
        }

        auto condIter = rep.find(FIELD_CONDITION);
        if (condIter != rep.end() && condIter->is_string()) {
            rd.condition = condIter->get<std::string>();
        }
        return true;
    }

    std::string BuildShortOptions(const std::vector<OptionDef> &options)
    {
        std::string shortOpts = "h";
        for (const auto &opt : options) {
            shortOpts += opt.shortOpt;
            if (opt.type != TYPE_BOOL) {
                shortOpts += ":";
            }
        }
        return shortOpts;
    }

    // option.name points to OptionDef::longOpt's c_str(); caller must ensure
    // def.options outlives the returned vector (guaranteed by ExecuteCommand scope).
    std::vector<option> BuildLongOptions(const std::vector<OptionDef> &options)
    {
        std::vector<option> longOpts;
        option helpOpt = {};
        helpOpt.name = "help";
        helpOpt.has_arg = no_argument;
        helpOpt.flag = nullptr;
        helpOpt.val = HELP_LONG_OPT_VAL;
        longOpts.push_back(helpOpt);
        for (const auto &opt : options) {
            option o = {};
            o.name = opt.longOpt.c_str();
            o.has_arg = (opt.type == TYPE_BOOL) ? no_argument : required_argument;
            o.flag = nullptr;
            o.val = opt.shortOpt;
            longOpts.push_back(o);
        }
        option endOpt = {};
        endOpt.name = nullptr;
        endOpt.has_arg = 0;
        endOpt.flag = nullptr;
        endOpt.val = 0;
        longOpts.push_back(endOpt);
        return longOpts;
    }

    ErrCode ParseCommandLine(const CommandDef &def, int argc, char **argv,
        std::map<std::string, std::string> &parsedValues, std::map<std::string, bool> &parsedFlags,
        bool &showHelp, std::string &output)
    {
        std::string shortOpts = BuildShortOptions(def.options);
        std::vector<option> longOpts = BuildLongOptions(def.options);
        GetoptStateGuard getoptGuard;
        optind = 0;
        opterr = 0;
        int32_t opt = getopt_long(argc, argv, shortOpts.c_str(), longOpts.data(), nullptr);
        while (opt != -1) {
            if (optind < 0 || optind > argc) {
                output = "error: invalid option index.\n";
                return OHOS::ERR_INVALID_VALUE;
            }
            if (opt == '?') {
                output = "error: unknown option.\n";
                return OHOS::ERR_INVALID_VALUE;
            }
            if (opt == HELP_SHORT_OPT) {
                showHelp = true;
                break;
            }
            HandleOptionMatch(def, opt, parsedValues, parsedFlags);
            opt = getopt_long(argc, argv, shortOpts.c_str(), longOpts.data(), nullptr);
        }
        return OHOS::ERR_OK;
    }

    bool HandleOptionMatch(const CommandDef &def, int32_t opt,
        std::map<std::string, std::string> &parsedValues, std::map<std::string, bool> &parsedFlags)
    {
        for (const auto &optDef : def.options) {
            if (opt != optDef.shortOpt) {
                continue;
            }
            if (optDef.type == TYPE_BOOL) {
                parsedFlags[optDef.paramName] = true;
            } else if (optarg != nullptr) {
                parsedValues[optDef.paramName] = optarg;
            }
            return true;
        }
        return false;
    }

    void BuildHelpOutput(const CommandDef &def, const std::string &name, std::string &output)
    {
        output = "usage: " + name + " <options>\n";
        for (const auto &optDef : def.options) {
            output.append("  -");
            output.append(1, optDef.shortOpt);
            output.append(", --");
            output.append(optDef.longOpt);
            if (optDef.type != TYPE_BOOL) {
                output.append(" <");
                output.append(optDef.type);
                output.append(">");
            }
            if (optDef.required) {
                output.append(" (required)");
            }
            output.append("\n");
        }
    }

    bool ValidateRequiredOptions(const CommandDef &def,
        const std::map<std::string, std::string> &parsedValues, std::string &output)
    {
        for (const auto &optDef : def.options) {
            if (!optDef.required || optDef.type == TYPE_BOOL) {
                continue;
            }
            auto it = parsedValues.find(optDef.paramName);
            if (it == parsedValues.end() || it->second.empty()) {
                output = "error: required option -" + std::string(1, optDef.shortOpt) +
                         " (--" + optDef.longOpt + ") is missing or empty.\n";
                return false;
            }
        }
        return true;
    }

    ErrCode WriteParamsToParcel(GenericIpcCaller *caller, const CommandDef &def,
        const std::map<std::string, std::string> &parsedValues,
        const std::map<std::string, bool> &parsedFlags, MessageParcel &data, std::string &output)
    {
        for (const auto &paramDef : def.params) {
            if (paramDef.source != SOURCE_OPTION) {
                continue;
            }
            auto valIter = parsedValues.find(paramDef.name);
            if (valIter == parsedValues.end()) {
                ErrCode err = WriteDefaultParam(caller, paramDef, parsedValues, parsedFlags, data, output);
                if (err != OHOS::ERR_OK) {
                    return err;
                }
                continue;
            }
            ErrCode err = WriteProvidedParam(caller, paramDef, valIter->second, parsedValues, data, output);
            if (err != OHOS::ERR_OK) {
                return err;
            }
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteDefaultParam(GenericIpcCaller *caller, const ParamDef &paramDef,
        const std::map<std::string, std::string> &parsedValues,
        const std::map<std::string, bool> &parsedFlags, MessageParcel &data, std::string &output)
    {
        auto flagIter = parsedFlags.find(paramDef.name);
        if (flagIter != parsedFlags.end()) {
            return WriteBoolParam(data, flagIter->second, paramDef.name, output);
        }
        if (paramDef.type == TYPE_REMOTEOBJECT) {
            return WriteRemoteObjectParam(data, paramDef.name, output);
        }
        if (paramDef.type == TYPE_WANT) {
            return WriteWantParamWrapper(caller, parsedValues, data, paramDef.name, output);
        }
        if (paramDef.type == TYPE_INT32) {
            return WriteDefaultInt32(data, paramDef.name, output);
        }
        if (paramDef.type == TYPE_STRING) {
            return WriteDefaultString(data, paramDef.name, output);
        }
        APP_LOGE("param %{public}s not found in parsed options.", paramDef.name.c_str());
        output = "error: param '" + paramDef.name + "' not provided.\n";
        return OHOS::ERR_INVALID_VALUE;
    }

    ErrCode WriteProvidedParam(GenericIpcCaller *caller, const ParamDef &paramDef, const std::string &val,
        const std::map<std::string, std::string> &parsedValues, MessageParcel &data, std::string &output)
    {
        if (paramDef.type == TYPE_STRING) {
            return WriteStringParam(data, val, paramDef.name, output);
        }
        if (paramDef.type == TYPE_INT32) {
            return WriteProvidedInt32(data, val, paramDef.name, output);
        }
        if (paramDef.type == TYPE_BOOL) {
            return WriteProvidedBool(data, val, paramDef.name, output);
        }
        if (paramDef.type == TYPE_WANT) {
            return WriteWantParamWrapper(caller, parsedValues, data, paramDef.name, output);
        }
        if (paramDef.type == TYPE_REMOTEOBJECT) {
            return WriteRemoteObjectParam(data, paramDef.name, output);
        }
        APP_LOGE("unsupported param type %{public}s.", paramDef.type.c_str());
        output = "error: unsupported param type.\n";
        return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
    }

    ErrCode ProcessReply(const CommandDef &def, MessageParcel &reply, std::string &output)
    {
        std::map<std::string, std::pair<std::string, int32_t>> conditionValues;
        for (const auto &replyDef : def.reply) {
            if (!ShouldProcessReply(replyDef, conditionValues, output)) {
                continue;
            }
            ErrCode err = ReadReplyField(replyDef, reply, conditionValues, output);
            if (err != OHOS::ERR_OK) {
                return err;
            }
        }
        return OHOS::ERR_OK;
    }

    bool ShouldProcessReply(const ReplyDef &replyDef,
        const std::map<std::string, std::pair<std::string, int32_t>> &conditionValues, std::string &output)
    {
        if (replyDef.condition.empty()) {
            return true;
        }
        auto condIter = conditionValues.find(replyDef.condition);
        if (condIter == conditionValues.end()) {
            APP_LOGE("condition field %{public}s not found in previous reply fields.",
                replyDef.condition.c_str());
            output.append("error: config condition field '" + replyDef.condition + "' not found.\n");
            return false;
        }
        const auto &cond = condIter->second;
        bool shouldSkip = false;
        if (cond.first == TYPE_BOOL) {
            shouldSkip = (cond.second == 0);
        } else if (cond.first == TYPE_INT32) {
            shouldSkip = (cond.second != 0);
        }
        if (shouldSkip) {
            APP_LOGD("skip reply field %{public}s, condition %{public}s failed.",
                replyDef.name.c_str(), replyDef.condition.c_str());
            return false;
        }
        return true;
    }

    ErrCode ReadReplyField(const ReplyDef &replyDef, MessageParcel &reply,
        std::map<std::string, std::pair<std::string, int32_t>> &conditionValues, std::string &output)
    {
        if (replyDef.type == TYPE_BOOL) {
            return ReadBoolReply(replyDef, reply, conditionValues, output);
        }
        if (replyDef.type == TYPE_INT32VECTOR) {
            return ReadInt32VectorReply(replyDef, reply, output);
        }
        if (replyDef.type == TYPE_STRINGVECTOR) {
            return ReadStringVectorReply(replyDef, reply, output);
        }
        if (replyDef.type == TYPE_INT32) {
            return ReadInt32Reply(replyDef, reply, conditionValues, output);
        }
        if (replyDef.type == TYPE_STRING) {
            return ReadStringReply(replyDef, reply, output);
        }
        APP_LOGE("unsupported reply type %{public}s.", replyDef.type.c_str());
        return OHOS::ERR_OK;
    }

    ErrCode ReadBoolReply(const ReplyDef &replyDef, MessageParcel &reply,
        std::map<std::string, std::pair<std::string, int32_t>> &conditionValues, std::string &output)
    {
        bool val = reply.ReadBool();
        conditionValues[replyDef.name] = {TYPE_BOOL, val ? 1 : 0};
        if (!replyDef.name.empty()) {
            output.append(replyDef.name);
            output.append(": ");
            output.append(val ? "true" : "false");
            output.append("\n");
        }
        if (!val && replyDef.name == TAG_SUCCESS) {
            APP_LOGE("IPC reply indicates failure.");
            output = "error: operation failed.\n";
            return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode ReadInt32VectorReply(const ReplyDef &replyDef, MessageParcel &reply, std::string &output)
    {
        std::vector<int32_t> vec;
        if (!reply.ReadInt32Vector(&vec)) {
            APP_LOGE("failed to read int32vector from reply.");
            output.append("error: failed to read reply.\n");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        output.append(replyDef.name);
        output.append(": [");
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                output.append(", ");
            }
            output.append(std::to_string(vec[i]));
        }
        output.append("]\n");
        return OHOS::ERR_OK;
    }

    ErrCode ReadStringVectorReply(const ReplyDef &replyDef, MessageParcel &reply, std::string &output)
    {
        std::vector<std::string> vec;
        if (!reply.ReadStringVector(&vec)) {
            APP_LOGE("failed to read stringvector from reply.");
            output.append("error: failed to read reply.\n");
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        output.append(replyDef.name);
        output.append(": [");
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) {
                output.append(", ");
            }
            output.append(vec[i]);
        }
        output.append("]\n");
        return OHOS::ERR_OK;
    }

    ErrCode ReadInt32Reply(const ReplyDef &replyDef, MessageParcel &reply,
        std::map<std::string, std::pair<std::string, int32_t>> &conditionValues, std::string &output)
    {
        int32_t val = reply.ReadInt32();
        conditionValues[replyDef.name] = {TYPE_INT32, val};
        output.append(replyDef.name);
        output.append(": ");
        output.append(std::to_string(val));
        output.append("\n");
        return OHOS::ERR_OK;
    }

    ErrCode ReadStringReply(const ReplyDef &replyDef, MessageParcel &reply, std::string &output)
    {
        std::string val = reply.ReadString();
        output.append(replyDef.name);
        output.append(": ");
        output.append(val);
        output.append("\n");
        return OHOS::ERR_OK;
    }

    ErrCode WriteBoolParam(MessageParcel &data, bool val, const std::string &name, std::string &output)
    {
        if (!data.WriteBool(val)) {
            APP_LOGE("failed to write bool param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteStringParam(MessageParcel &data, const std::string &val,
        const std::string &name, std::string &output)
    {
        if (!data.WriteString(val)) {
            APP_LOGE("failed to write string param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteProvidedInt32(MessageParcel &data, const std::string &val,
        const std::string &name, std::string &output)
    {
        int32_t intVal = 0;
        if (!OHOS::StrToInt(val, intVal)) {
            APP_LOGE("failed to convert %{public}s to int32.", val.c_str());
            output = "error: invalid int32 value for '" + name + "'.\n";
            return OHOS::ERR_INVALID_VALUE;
        }
        if (!data.WriteInt32(intVal)) {
            APP_LOGE("failed to write int32 param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteProvidedBool(MessageParcel &data, const std::string &val,
        const std::string &name, std::string &output)
    {
        bool boolVal = (val == "true" || val == "1");
        if (!data.WriteBool(boolVal)) {
            APP_LOGE("failed to write bool param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteDefaultInt32(MessageParcel &data, const std::string &name, std::string &output)
    {
        APP_LOGD("param %{public}s not provided, write default 0.", name.c_str());
        if (!data.WriteInt32(0)) {
            APP_LOGE("failed to write default int32 param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteDefaultString(MessageParcel &data, const std::string &name, std::string &output)
    {
        APP_LOGD("param %{public}s not provided, write empty string.", name.c_str());
        if (!data.WriteString("")) {
            APP_LOGE("failed to write default string param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteRemoteObjectParam(MessageParcel &data, const std::string &name, std::string &output)
    {
        sptr<IRemoteObject> placeholder = new (std::nothrow) IPCObjectStub(u"");
        if (placeholder == nullptr) {
            APP_LOGE("failed to allocate IPCObjectStub for %{public}s.", name.c_str());
            output = "error: memory allocation failed.\n";
            return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
        }
        if (!data.WriteRemoteObject(placeholder)) {
            APP_LOGE("failed to write remoteobject param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode WriteWantParamWrapper(GenericIpcCaller *caller,
        const std::map<std::string, std::string> &parsedValues, MessageParcel &data,
        const std::string &name, std::string &output)
    {
        AAFwk::Want want;
        if (!caller->WriteWantParam(want, parsedValues, data)) {
            APP_LOGE("failed to write want param %{public}s.", name.c_str());
            output = "error: parcel write failed.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        return OHOS::ERR_OK;
    }

    ErrCode PrepareParcelAndSend(const CommandDef &def, GenericIpcCaller *caller,
        const std::map<std::string, std::string> &parsedValues,
        const std::map<std::string, bool> &parsedFlags, std::string &output)
    {
        sptr<IRemoteObject> remote = GetBundleMgrRemote(output);
        if (remote == nullptr) {
            return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
        }
        MessageParcel data;
        if (!data.WriteInterfaceToken(INTERFACE_DESCRIPTOR)) {
            APP_LOGE("failed to write interface token.");
            output = "error: failed to write interface token.\n";
            return ERR_APPEXECFWK_PARCEL_ERROR;
        }
        ErrCode err = WriteParamsToParcel(caller, def, parsedValues, parsedFlags, data, output);
        if (err != OHOS::ERR_OK) {
            return err;
        }
        MessageParcel reply;
        return SendRequest(def, remote, data, reply, output);
    }

    sptr<IRemoteObject> GetBundleMgrRemote(std::string &output)
    {
        sptr<IBundleMgr> bundleMgrProxy = BundleCommandCommon::GetBundleMgrProxy();
        if (bundleMgrProxy == nullptr) {
            APP_LOGE("failed to get bundle manager proxy.");
            output = "error: failed to get bundle manager proxy.\n";
            return nullptr;
        }
        sptr<IRemoteObject> remote = bundleMgrProxy->AsObject();
        if (remote == nullptr) {
            APP_LOGE("remote object is null.");
            output = "error: remote object is null.\n";
            return nullptr;
        }
        return remote;
    }

    ErrCode SendRequest(const CommandDef &def, const sptr<IRemoteObject> &remote,
        MessageParcel &data, MessageParcel &reply, std::string &output)
    {
        MessageOption option(MessageOption::TF_SYNC);
        APP_LOGI("ExecuteCommand begin, ipcCode: %{public}u", def.ipcCode);
        int32_t result = remote->SendRequest(def.ipcCode, data, reply, option);
        if (result != NO_ERROR) {
            APP_LOGE("SendRequest failed, ipcCode: %{public}u, result: %{public}d", def.ipcCode, result);
            output = "error: IPC request failed, result: " + std::to_string(result) + ".\n";
            return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
        }
        return ProcessReply(def, reply, output);
    }
};

std::string GenericIpcCaller::cmdsPath_ = "/data/test/bundle_test_tool_cmds.json";

GenericIpcCaller::GenericIpcCaller() : impl_(std::make_unique<Impl>()) {}

GenericIpcCaller::~GenericIpcCaller() = default;

bool GenericIpcCaller::LoadConfig()
{
    if (configLoaded_) {
        return true;
    }
    std::ifstream ifs(cmdsPath_);
    if (!ifs.is_open()) {
        APP_LOGE("failed to open config file: %{public}s", cmdsPath_.c_str());
        return false;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string content = ss.str();
    nlohmann::json root = nlohmann::json::parse(content, nullptr, false);
    if (root.is_discarded() || !root.is_object()) {
        APP_LOGE("config file parse error or not object.");
        return false;
    }
    if (!impl_->ParseFromJson(root)) {
        APP_LOGE("failed to parse commands from config.");
        return false;
    }
    configLoaded_ = true;
    APP_LOGD("config loaded successfully, commands count: %{public}zu", impl_->commands.size());
    return true;
}

std::string GenericIpcCaller::ListCommands()
{
    std::string result = "Available commands:\n";
    if (!LoadConfig()) {
        result = "error: failed to load config.\n";
        return result;
    }
    for (const auto &cmd : impl_->commands) {
        result.append("  ");
        result.append(cmd.first);
        result.append("\n");
    }
    return result;
}

ErrCode GenericIpcCaller::ExecuteCommand(const std::string &name, int argc, char **argv, std::string &output)
{
    if (!LoadConfig()) {
        output = "error: failed to load config.\n";
        return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
    }
    auto cmdIter = impl_->commands.find(name);
    if (cmdIter == impl_->commands.end()) {
        APP_LOGE("command %{public}s not found in config.", name.c_str());
        output = "error: command '" + name + "' not found.\n";
        return ERR_APPEXECFWK_SERVICE_INTERNAL_ERROR;
    }
    const auto &def = cmdIter->second;
    std::map<std::string, std::string> parsedValues;
    std::map<std::string, bool> parsedFlags;
    bool showHelp = false;
    ErrCode err = impl_->ParseCommandLine(def, argc, argv, parsedValues, parsedFlags, showHelp, output);
    if (err != OHOS::ERR_OK) {
        return err;
    }
    if (showHelp) {
        impl_->BuildHelpOutput(def, name, output);
        return OHOS::ERR_OK;
    }
    if (!impl_->ValidateRequiredOptions(def, parsedValues, output)) {
        return OHOS::ERR_INVALID_VALUE;
    }
    APP_LOGI("ExecuteCommand begin, command: %{public}s", name.c_str());
    return impl_->PrepareParcelAndSend(def, this, parsedValues, parsedFlags, output);
}

bool GenericIpcCaller::WriteWantParam(AAFwk::Want &want,
    const std::map<std::string, std::string> &parsedValues, MessageParcel &data)
{
    auto bundleNameIter = parsedValues.find("bundleName");
    if (bundleNameIter != parsedValues.end()) {
        std::string abilityName;
        auto abilityIter = parsedValues.find("abilityName");
        if (abilityIter != parsedValues.end()) {
            abilityName = abilityIter->second;
        }
        std::string moduleName;
        auto moduleIter = parsedValues.find("moduleName");
        if (moduleIter != parsedValues.end()) {
            moduleName = moduleIter->second;
        }
        std::string deviceId;
        auto deviceIter = parsedValues.find("deviceId");
        if (deviceIter != parsedValues.end()) {
            deviceId = deviceIter->second;
        }
        want.SetElementName(deviceId, bundleNameIter->second, abilityName, moduleName);
    }
    auto actionIter = parsedValues.find("action");
    if (actionIter != parsedValues.end()) {
        want.SetAction(actionIter->second);
    }
    auto uriIter = parsedValues.find("uri");
    if (uriIter != parsedValues.end()) {
        want.SetUri(uriIter->second);
    }
    auto typeIter = parsedValues.find("type");
    if (typeIter != parsedValues.end()) {
        want.SetType(typeIter->second);
    }
    auto entityIter = parsedValues.find("entity");
    if (entityIter != parsedValues.end()) {
        want.AddEntity(entityIter->second);
    }
    return data.WriteParcelable(&want);
}

}  // namespace AppExecFwk
}  // namespace OHOS
