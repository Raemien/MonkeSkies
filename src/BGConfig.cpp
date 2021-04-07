#include "main.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

struct Config_t
{
    bool enabled = true;
    int imageWidth = 3840;
    int rotationOffset = 0;
    std::string selectedFile = "SteamVR.png";
}
Settings;

void SetupConfig()
{
    getConfig().config.RemoveAllMembers();
    getConfig().config.SetObject();
    auto& allocator = getConfig().config.GetAllocator();
    getConfig().config.AddMember("enabled", Settings.enabled, allocator);
    getConfig().config.AddMember("imageWidth", Settings.imageWidth, allocator);
    getConfig().config.AddMember("rotationOffset", Settings.rotationOffset, allocator);
    getConfig().config.AddMember("selectedFile", Settings.selectedFile, allocator);
    getConfig().Write();
}

bool LoadConfig()
{
    getConfig().Load();
    if(!getConfig().config.HasMember("enabled") || !getConfig().config["enabled"].IsBool()) return false;
    if(!getConfig().config.HasMember("imageWidth") || !getConfig().config["imageWidth"].IsInt()) return false;
    if(!getConfig().config.HasMember("rotationOffset") || !getConfig().config["rotationOffset"].IsInt()) return false;
    if(!getConfig().config.HasMember("selectedFile") || !getConfig().config["selectedFile"].IsString()) return false;
    return true;
}