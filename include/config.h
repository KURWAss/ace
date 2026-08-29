#ifndef ACE_CONFIG_H_
#define ACE_CONFIG_H_

#include <string>

std::string GetConfigPath();
void EnsureConfigExists();
std::string GetTerminalCommand();

#endif  // ACE_CONFIG_H_
