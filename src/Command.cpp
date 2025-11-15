#include "Command.h"

// Define global variables
std::unordered_map<std::string, std::unique_ptr<TCommand>> Commands;
std::vector<std::string> DeferredCommands;
bool DeferredInitialized = false;