#include "UI/BackgroundListViewController.hpp"
#include "BGConfig.hpp"

#include "monkecomputer/shared/ViewLib/CustomComputer.hpp"
#include "monkecomputer/shared/InputHandlers/UISelectionHandler.hpp"
#include "monkecomputer/shared/EKeyboardKey.hpp"

#include <dirent.h>
#include <list>

DEFINE_CLASS(MonkeSkies::BackgroundListViewController);

using namespace GorillaUI;

bool viewingList = false;
std::vector<std::string> bgVector = {};

namespace MonkeSkies
{
    void BackgroundListViewController::Awake()
    {
        listHandler = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, true);
        RefreshList();
    }

    void SelectImage(int index)
    {
        std::string filename = bgVector[index];
        getConfig().config["selectedFile"].SetString(filename, getConfig().config.GetAllocator());
        getConfig().Write();
        LoadBackground(bgDirectoryPath + filename);
    }

    void BackgroundListViewController::RefreshList()
    {
        bgVector.clear();
        bgVector = {};
        DIR* imgdir = opendir(bgDirectoryPath.c_str());
        dirent* fileent;
        while ((fileent = readdir(imgdir)) != NULL)
        {
            std::string filename = fileent->d_name;
            for (char &ch : filename) ch = tolower(ch);
            if (filename.ends_with(".png") || filename.ends_with(".jpg") || filename.ends_with(".jpeg"))
            {
                bgVector.push_back(fileent->d_name);
            }
        }

        GorillaUI::UISelectionHandler* selection = (GorillaUI::UISelectionHandler*)listHandler;
        selection->min = 0;
        selection->max = bgVector.size();
        selection->selectionCallback = SelectImage;
        for (size_t i = 0; i < bgVector.size(); i++)
        {
            std::string selectedFile = getConfig().config["selectedFile"].GetString();
            if (bgVector[i] == selectedFile) {
                selection->currentSelectionIndex = i;
            }
        }
    }

    void BackgroundListViewController::DrawSelection()
    {
        GorillaUI::UISelectionHandler* selection = (GorillaUI::UISelectionHandler*)listHandler;
        std::string configtxt = getConfig().config["selectedFile"].GetString();
        std::string selectedtxt = bgVector[selection->currentSelectionIndex];
        text += (configtxt == selectedtxt) ? "\nSelected Image: <color=#8197ef>" : "\nSelected Image: <color=#81ceef>";
        text += selectedtxt + "</color>";
    }

    void BackgroundListViewController::DrawHints(bool hasBgs)
    {
        if (!hasBgs)
        {
            text += string_format("<size=48><color=white>No images found!\n\nTo use this mod, place valid 360 images in the following folder:\n</color>%s </size>", bgDirectoryPath.c_str());
            CustomComputer::Redraw();
            return;
        }
        text += "\n\n\n\n\n <color=white>Up/Down: </color>Navigate List";
        text += "\n <color=white>Enter:   </color>Confirm Selection";
        text += "\n <color=white>R:       </color>Refresh List";
    }

    void BackgroundListViewController::Redraw()
    {
        bool hasBackgrounds = (bgVector.size() > 0);
        text = "<color=white>---------------</color><color=#81ceef>Sky Textures</color><color=white>---------------</color>\n";
        if (hasBackgrounds) DrawSelection();
        DrawHints(hasBackgrounds);
        CustomComputer::Redraw();
    }

    void BackgroundListViewController::OnKeyPressed(int keycode)
    {
        GorillaUI::UISelectionHandler* selection = (GorillaUI::UISelectionHandler*)listHandler;
        EKeyboardKey key = (EKeyboardKey)keycode;
        selection->HandleKey(key);

        switch (key)
        {
        case EKeyboardKey::R:
            RefreshList();
            break;
        default:
            break;
        }

        Redraw();
    }

    void BackgroundListViewController::DidActivate(bool firstActivation)
    {
        RefreshList();
        Redraw();
    }

}