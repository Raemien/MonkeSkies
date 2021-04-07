#pragma once
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

Configuration& getConfig();
extern Logger& getLogger();
extern Il2CppObject* backgroundTexture;
extern Il2CppObject* backgroundObject;
extern std::string bgDirectoryPath;
extern void LoadBackground(std::string filename);