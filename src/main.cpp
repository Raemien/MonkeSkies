#include "UI/BackgroundListViewController.hpp"
#include "BGConfig.hpp"
#include "typedefs.h"

#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "monkecomputer/shared/GorillaUI.hpp"
#include "monkecomputer/shared/Register.hpp"
#include "custom-types/shared/register.hpp"

#include <sys/stat.h>
#include <fstream>

Il2CppObject* backgroundObject;
Il2CppObject* backgroundMat;
Il2CppObject* backgroundTexture;

static ModInfo modInfo;

std::string bgDirectoryPath;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
    return *logger;
}

Configuration& getConfig() {
    static Configuration config(modInfo);
    return config;
}

void HideSky(bool hide)
{
    Il2CppObject* skyObj = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>("UnityEngine", "GameObject", "Find", il2cpp_utils::createcsstr("Level/sky")));
    bool hideSky = (getConfig().config["enabled"].GetBool() && backgroundTexture);
    if (skyObj && hideSky) CRASH_UNLESS(il2cpp_utils::RunMethod(skyObj, "SetActive", !hide));
}

void CreateBGObject()
{
    if (!backgroundObject && backgroundTexture)
    {
        // Create object
        backgroundObject = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>("UnityEngine", "GameObject", "CreatePrimitive", (int)0));
        CRASH_UNLESS(il2cpp_utils::RunMethod(backgroundObject, "set_name", il2cpp_utils::createcsstr("CustomSkyObject")));
        CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Object", "DontDestroyOnLoad", backgroundObject));

        // Material + shader management
        Il2CppObject* bgrenderer = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>(backgroundObject, "GetComponent", il2cpp_utils::GetSystemType("UnityEngine", "Renderer")));
        Il2CppObject* bgcollider = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>(backgroundObject, "GetComponent", il2cpp_utils::GetSystemType("UnityEngine", "Collider")));
        Il2CppObject* bgshader = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>("UnityEngine", "Shader", "Find", il2cpp_utils::createcsstr("UI/Default")));
        backgroundMat = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>(bgrenderer, "get_material"));
        CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Component", "Destroy", bgcollider));
        CRASH_UNLESS(il2cpp_utils::RunMethod(bgrenderer, "set_sortingOrder", -8192));
        static auto set_shader = reinterpret_cast<function_ptr_t<void, Il2CppObject*, Il2CppObject*>>(il2cpp_functions::resolve_icall("UnityEngine.Material::set_shader"));
        set_shader(backgroundMat, bgshader);
        CRASH_UNLESS(il2cpp_utils::RunMethod(backgroundMat, "SetTexture", il2cpp_utils::createcsstr("_MainTex"), backgroundTexture));

        // Set transforms
        Il2CppObject* bgtrans = CRASH_UNLESS(il2cpp_utils::RunMethodUnsafe<Il2CppObject*>(backgroundObject, "get_transform"));
        CRASH_UNLESS(il2cpp_utils::RunMethod(bgtrans, "set_localScale", Vector3{-800, -800, -800}));
        CRASH_UNLESS(il2cpp_utils::RunMethod(bgtrans, "set_localPosition", Vector3{0, 0, 0}));
        CRASH_UNLESS(il2cpp_utils::RunMethod(bgtrans, "set_localEulerAngles", Vector3{0, (float)(getConfig().config["rotationOffset"].GetInt() - 90), 180}));
    }
}

void LoadBackground(std::string path)
{
    std::string filename = path.substr(path.find_last_of("/"));
    if (fileexists(path))
    {
        std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
        Array<uint8_t>* bytes = il2cpp_utils::vectorToArray(data);

        int width = getConfig().config["imageWidth"].GetInt();
        int height = std::floor((float)width * (2.0f / 3.0f));
        if (backgroundTexture) CRASH_UNLESS(il2cpp_utils::RunMethod("UnityEngine", "Component", "Destroy", backgroundTexture));
        backgroundTexture = CRASH_UNLESS(il2cpp_utils::New<Il2CppObject*>("UnityEngine", "Texture2D", width, height, (int)4, false));
        static auto LoadImage = reinterpret_cast<function_ptr_t<bool, Il2CppObject*, Array<uint8_t>*, bool>>(il2cpp_functions::resolve_icall("UnityEngine.ImageConversion::LoadImage"));
        bool success = LoadImage(backgroundTexture, bytes, false);

        std::string resulttxt = success ? "[MonkeSkies] successfully loaded '" + filename + "'." : "[MonkeSkies] failed to load '" + filename + "'.";
        getLogger().info(resulttxt);

        if (backgroundMat && success) {
            CRASH_UNLESS(il2cpp_utils::RunMethod(backgroundMat, "SetTexture", il2cpp_utils::createcsstr("_MainTex"), backgroundTexture));
        }
        else if (success) CreateBGObject();
    }
}

void InitBackgrounds()
{
    auto& modcfg = getConfig().config;
    int success = mkdir(bgDirectoryPath.c_str(), 0777);
    std::string backgroundPath = bgDirectoryPath;
    backgroundPath += modcfg["selectedFile"].GetString();

    if (success != 0 && fileexists(backgroundPath))
    {
        LoadBackground(backgroundPath);
    }
}

MAKE_HOOK_OFFSETLESS(PlayerController_Awake, void)
{
    PlayerController_Awake();
    auto& modcfg = getConfig().config;
    if (!backgroundObject && modcfg["enabled"].GetBool())
    {
        InitBackgrounds();
        HideSky(true);
        CreateBGObject();
    }
}

extern "C" void setup(ModInfo& info) {

    info.id = "MonkeSkies";
    info.version = "1.0.3";
    modInfo = info;
    bgDirectoryPath = getDataDir(info);
    getConfig().Load();
}

extern "C" void load() {
    if (!LoadConfig()) SetupConfig();
    il2cpp_functions::Init();
    GorillaUI::Init();
    INSTALL_HOOK_OFFSETLESS(getLogger(), PlayerController_Awake, il2cpp_utils::FindMethodUnsafe("GorillaLocomotion", "Player", "Awake", 0));
}
