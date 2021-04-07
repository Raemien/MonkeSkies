#pragma once

#include "custom-types/shared/macros.hpp"
#include "monkecomputer/shared/ViewLib/View.hpp"
#include "monkecomputer/shared/InputHandlers/UISelectionHandler.hpp"

DECLARE_CLASS_CODEGEN(MonkeSkies, BackgroundListViewController, GorillaUI::Components::View,
    DECLARE_METHOD(void, DidActivate, bool firstActivation);
    DECLARE_METHOD(void, OnKeyPressed, int key);
    DECLARE_METHOD(void, Awake);
    DECLARE_METHOD(void, Redraw);
    DECLARE_METHOD(void, DrawSelection);
    DECLARE_METHOD(void, DrawHints, bool hasBgs);
    DECLARE_METHOD(void, RefreshList);

    DECLARE_INSTANCE_FIELD(void*, listHandler);

    REGISTER_FUNCTION(BackgroundListViewController,
        REGISTER_METHOD(DidActivate);
        REGISTER_METHOD(OnKeyPressed);
        REGISTER_METHOD(Awake);
        REGISTER_METHOD(Redraw);
        REGISTER_METHOD(DrawSelection);
        REGISTER_METHOD(DrawHints);
        REGISTER_METHOD(RefreshList);
        REGISTER_FIELD(listHandler);
    )
)