#ifndef ACE_CONFIG_H_
#define ACE_CONFIG_H_

#include <string>
#include <vector>

std::string GetConfigPath();
void EnsureConfigExists();
std::string GetTerminalCommand();
std::vector<std::string> GetAutostartCommands();

#endif  // ACE_CONFIG_H_
