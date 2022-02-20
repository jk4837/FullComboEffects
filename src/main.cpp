#include "main.hpp"

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "HMUI/ImageView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

using namespace GlobalNamespace;
using namespace TMPro;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

static void setFullComboUI(UnityEngine::GameObject *clearedBannerGo)
{
    try
    {
        clearedBannerGo->GetComponentsInChildren<TextMeshProUGUI *>()->get(0)->set_text(il2cpp_utils::newcsstr("FULL COMBO"));
    }
    catch (const std::exception &e)
    {
        getLogger().log(Logging::ERROR, "Failed to set text to cleared info.");
        getLogger().log(Logging::ERROR, std::string(e.what()));
    }

    try
    {
        clearedBannerGo->GetComponentsInChildren<HMUI::ImageView *>()->get(0)->set_color(UnityEngine::Color::get_yellow());
    }
    catch (const std::exception &e)
    {
        getLogger().log(Logging::ERROR, "Failed to set color to background of cleared info.");
        getLogger().log(Logging::ERROR, std::string(e.what()));
    }
}

static void setNotFullComboUI(UnityEngine::GameObject *clearedBannerGo)
{
    // no need to reset text to LEVEL CLEARED

    // reset background to red color
    try
    {
        const auto originColor = UnityEngine::Color(1, 0.374, 0, 1);
        clearedBannerGo->GetComponentsInChildren<HMUI::ImageView *>()->get(0)->set_color(originColor);
    }
    catch (const std::exception &e)
    {
        getLogger().log(Logging::ERROR, "Failed to reset color to background of cleared info.");
        getLogger().log(Logging::ERROR, std::string(e.what()));
    }
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::SetDataToUI, void, ResultsViewController *self)
{
    Results(self);

    if (self->levelCompletionResults->levelEndStateType == LevelCompletionResults::LevelEndStateType::Cleared)
    {
        if (self->levelCompletionResults->fullCombo)
        {
            self->newHighScore = true; // for trigger sounds and fireworks
            setFullComboUI(self->clearedBannerGo);
        } else {
            setNotFullComboUI(self->clearedBannerGo);
        }
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;

    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    INSTALL_HOOK(getLogger(), Results);
}