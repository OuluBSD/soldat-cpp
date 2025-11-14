#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <set>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "Constants.h"

// Forward declarations for dependencies
struct TCvarBase;
struct TCommand;

// Command flags
enum class TCommandFlag {
    CMD_INIT = 0,
    CMD_ALIAS,
    CMD_SCRIPT,
    CMD_DEFERRED,
    CMD_ADMINONLY,
    CMD_PLAYERONLY
};

using TCommandFlags = std::set<TCommandFlag>;

const int MAX_COMMANDS = 1024;

// Using declarations
using TCommandFunction = std::function<void(const std::vector<std::string>&, uint8_t)>;

struct TCommand {
    std::string Name;
    TCommandFunction FunctionPtr;
    std::string Description;
    TCommandFlags Flags;
};

using TCommandTargets = std::vector<uint8_t>;

// Function declarations
void CommandInit();
bool ParseInput(const std::string& Input); // overload
bool ParseInput(const std::string& Input, uint8_t Sender); // overload
bool LoadConfig(const std::string& ConfigName);

TCommand* CommandAdd(const std::string& CommandNameVar, TCommandFunction CommandPtr, 
                     const std::string& Description, const TCommandFlags& Flags);
TCommand* CommandFind(const std::string& Name);
void CommandExecuteAlias(const std::vector<std::string>& Args, uint8_t Sender);
void ParseCommandLine();
void RunDeferredCommands();
TCommandTargets CommandTarget(const std::string& Target, uint8_t Sender);

// Global variables (extern declarations to be defined in a source file)
extern std::unordered_map<std::string, std::unique_ptr<TCommand>> Commands;
extern std::vector<std::string> DeferredCommands;
extern bool DeferredInitialized;

namespace CommandImpl {
    // Command implementations
    inline void CommandExec(const std::vector<std::string>& Args, uint8_t Sender = 255) {
        if (Args.size() == 1) {
            // MainConsole.Console('Usage: exec "filename.cfg"', GAME_MESSAGE_COLOR);
            return;
        }
        LoadConfig(Args[1]);
    }

    inline void CommandToggle(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() < 3) {
            // MainConsole.Console('Usage: toggle "cvarname" "value" "value2"', GAME_MESSAGE_COLOR);
            return;
        }
        
        TCvarBase* ACvar = TCvarBase::Find(Args[1]);
        if (!ACvar) {
            // MainConsole.Console('Toggle: Cvar ' + Args[1] + ' not found', DEBUG_MESSAGE_COLOR);
            return;
        }
        
        if (ACvar->ValueAsString() == Args[2]) {
            ACvar->ParseAndSetValue(Args[3]);
        } else {
            ACvar->ParseAndSetValue(Args[2]);
        }
    }

    inline void CommandAlias(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() < 3) {
            // MainConsole.Console('Usage: alias "name" "command"', GAME_MESSAGE_COLOR);
            return;
        }
        
        std::string AliasName = Args[1];
        // Check if cvar or command with this name already exists
        // if ((TCvarBase::Find(AliasName) != nullptr) && (CommandFind(AliasName) != nullptr)) {
        //     MainConsole.Console('Cannot use this alias name because it''s already used', DEBUG_MESSAGE_COLOR);
        //     return;
        // }
        
        CommandAdd(AliasName, CommandExecuteAlias, Args[2], {TCommandFlag::CMD_ALIAS});
        // MainConsole.Console('New alias: ' + Args[1] + ' with command: ' + Args[2], GAME_MESSAGE_COLOR);
    }

    inline void CommandExecuteAlias(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.empty()) return;
        
        TCommand* CommandPtr = CommandFind(Args[0]);
        if (!CommandPtr) {
            // MainConsole.Console('Cannot find alias command', DEBUG_MESSAGE_COLOR);
            return;
        }
        
        std::string commandText = CommandPtr->Description;
        // Split commandText by semicolon
        std::vector<std::string> commands;
        std::string delimiter = ";";
        size_t pos = 0;
        std::string token;
        
        while ((pos = commandText.find(delimiter)) != std::string::npos) {
            token = commandText.substr(0, pos);
            commands.push_back(token);
            commandText.erase(0, pos + delimiter.length());
        }
        commands.push_back(commandText);
        
        for (const auto& cmd : commands) {
            ParseInput(cmd);
        }
    }

    inline void CommandEcho(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() == 1) {
            // MainConsole.Console('Usage: echo "text"', GAME_MESSAGE_COLOR);
            return;
        }
        
        // MainConsole.Console(Args[1], GAME_MESSAGE_COLOR);
    }

    inline void CommandReset(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() == 1) {
            // MainConsole.Console('Usage: reset "cvarname"', GAME_MESSAGE_COLOR);
            return;
        }
        
        std::string CvarName = Args[1];
        TCvarBase* ACvar = TCvarBase::Find(CvarName);
        if (!ACvar) {
            // MainConsole.Console('Reset: Cvar ' + CvarName + ' not found', DEBUG_MESSAGE_COLOR);
            return;
        }
        
        ACvar->Reset();
        // MainConsole.Console('Reset: ' + CvarName + ' set to: ' + ACvar->ValueAsString(), DEBUG_MESSAGE_COLOR);
    }

    inline void CommandCmdlist(const std::vector<std::string>& Args, uint8_t Sender) {
        for (const auto& pair : Commands) {
            // MainConsole.Console(pair.second->Name + " - " + pair.second->Description, GAME_MESSAGE_COLOR);
        }
    }

    inline void CommandCvarlist(const std::vector<std::string>& Args, uint8_t Sender) {
        // This would iterate through Cvars and print them
        // Implementation depends on Cvar structure
    }

    inline void CommandInc(const std::vector<std::string>& Args, uint8_t Sender) {
        if (Args.size() < 5) {
            // MainConsole.Console('Usage: inc "cvarname" "min" "max" "delta"', GAME_MESSAGE_COLOR);
            return;
        }
        
        std::string CvarName = Args[1];
        TCvarBase* ACvar = TCvarBase::Find(CvarName);
        if (!ACvar) {
            // MainConsole.Console('Inc: Cvar ' + CvarName + ' not found', DEBUG_MESSAGE_COLOR);
            return;
        }
        
        // This function would handle incrementing either integer or float cvars
        // Implementation would require type checking and conversion
    }

    inline TCommand* CommandFind(const std::string& Name) {
        std::string lowerName = Name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        
        auto it = Commands.find(lowerName);
        if (it != Commands.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    inline TCommand* CommandAdd(const std::string& CommandNameVar, TCommandFunction CommandPtr, 
                                const std::string& Description, const TCommandFlags& Flags) {
        std::string CommandName = CommandNameVar;
        std::transform(CommandName.begin(), CommandName.end(), CommandName.begin(), ::tolower);
        
        if (CommandFind(CommandName) != nullptr) {
            // Debug message about command already existing
            return nullptr;
        }
        
        auto newCommand = std::make_unique<TCommand>();
        newCommand->Name = CommandName;
        newCommand->FunctionPtr = CommandPtr;
        newCommand->Description = Description;
        newCommand->Flags = Flags;
        
        TCommand* result = newCommand.get();
        Commands[CommandName] = std::move(newCommand);
        
        return result;
    }

    inline bool ParseInput(const std::string& Input) {
        return ParseInput(Input, 0);
    }

    inline bool ParseInput(const std::string& Input, uint8_t Sender) {
        if (Input.empty()) {
            return false;
        }
        
        // Split input by spaces to create argument array
        std::vector<std::string> InputArray;
        std::stringstream ss(Input);
        std::string token;
        
        while (ss >> token) {
            InputArray.push_back(token);
        }
        
        if (InputArray.empty()) {
            return false;
        }
        
        TCommand* CommandPtr = CommandFind(InputArray[0]);
        
        if (CommandPtr != nullptr) {
            // Check flags and execute command if appropriate
            if (CommandPtr->Flags.count(TCommandFlag::CMD_DEFERRED) && !DeferredInitialized) {
                DeferredCommands.push_back(Input);
            } else {
                CommandPtr->FunctionPtr(InputArray, Sender);
            }
            return true;
        }
        
        // Check if it's a cvar instead of command
        TCvarBase* ACvar = TCvarBase::Find(InputArray[0]);
        if (ACvar != nullptr) {
            if (InputArray.size() == 1) {
                // Print current value
                // MainConsole.Console(Format('%s is "%s" (%s)', [ACvar->Name, ACvar->ValueAsString, ACvar->Description]), DEBUG_MESSAGE_COLOR);
            } else if (InputArray.size() == 2) {
                if (!ACvar->ParseAndSetValue(InputArray[1])) {
                    // Print error message
                    // MainConsole.Console(Format('Unable to set %s: %s', [ACvar->Name, ACvar->GetErrorMessage()]), DEBUG_MESSAGE_COLOR);
                } else {
                    // Print success message
                    // MainConsole.Console(Format('%s is now set to: "%s"', [ACvar->Name, ACvar->ValueAsString]), DEBUG_MESSAGE_COLOR);
                }
            }
            return true;
        }
        
        return false;
    }

    inline bool LoadConfig(const std::string& ConfigName) {
        // This function would read a config file and parse each line
        // Implementation would depend on file I/O operations
        // For now, returning false as placeholder
        return false;
    }

    inline void ParseCommandLine() {
        // This would parse command line arguments
        // Implementation would depend on argc/argv access
    }

    inline TCommandTargets CommandTarget(const std::string& Target, uint8_t Sender) {
        TCommandTargets targets;
        // This function would identify which players match the target string
        // Implementation depends on player data structures
        return targets;
    }

    inline void RunDeferredCommands() {
        DeferredInitialized = true;
        if (DeferredCommands.empty()) {
            return;
        }
        
        for (const auto& cmd : DeferredCommands) {
            ParseInput(cmd);
        }
        
        DeferredCommands.clear();
    }

    inline void CommandInit() {
        Commands.clear();
        DeferredCommands.clear();
        
        CommandAdd("echo", CommandEcho, "echo text", {});
        CommandAdd("exec", CommandExec, "executes fileconfig", {});
        CommandAdd("cmdlist", CommandCmdlist, "list of commands", {});
        CommandAdd("cvarlist", CommandCvarlist, "list of cvars", {});
        CommandAdd("reset", CommandReset, "resets cvar to default value", {});
        CommandAdd("alias", CommandAlias, "creates alias", {});
        CommandAdd("toggle", CommandToggle, "toggles cvar between two values", {});
        CommandAdd("inc", CommandInc, "increments cvar value", {});
    }
}

// Using declarations to bring into global namespace
using CommandImpl::TCommand;
using CommandImpl::TCommandFunction;
using CommandImpl::TCommandFlags;
using CommandImpl::TCommandTargets;
using CommandImpl::CommandInit;
using CommandImpl::ParseInput;
using CommandImpl::LoadConfig;
using CommandImpl::CommandAdd;
using CommandImpl::CommandFind;
using CommandImpl::CommandExecuteAlias;
using CommandImpl::ParseCommandLine;
using CommandImpl::RunDeferredCommands;
using CommandImpl::CommandTarget;

// Global variables
extern std::unordered_map<std::string, std::unique_ptr<TCommand>> Commands;
extern std::vector<std::string> DeferredCommands;
extern bool DeferredInitialized = false;

#endif // COMMAND_H