/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/psyq_gpu.h"
#include "pe1/text.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_widget.h"

void Menu_DrawBlendColorArrows(MenuWidgetNode *panel);
s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags);
void Menu_DrawBlendColorChannelList(int node);
void Menu_DrawBlendColorOptionList(int node);
void Menu_DrawScreenAdjustPanel(void);
int Menu_ScreenAdjustInputHandler(void *node, int input);

void Menu_OpenBlendColorScreen(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *panel;
    MenuWidgetNode *options;

    parent = MenuWidget_CreateSimpleNode(0x2E, arg0, 0, 0);
    panel = MenuWidget_CreateNode(0x2E, parent, parent);
    parent->draw = (void (*)())Menu_DrawBlendColorArrows;
    parent->update = (void (*)())Menu_StepColorSelect;
    parent->appearance.gradientPoints = g_MenuBlendColorPanelPoints;
    parent->disabled = 1;
    panel->draw = (void (*)())Menu_DrawBlendColorChannelList;
    panel->flags = 1;
    options = MenuWidget_CreateNode(0x31, parent, parent);
    options->draw = (void (*)())Menu_DrawBlendColorOptionList;
    panel->linkedNext = options;
    options->linkedPrevious = panel;
    MenuWidget_SetCurrentNode(panel->cursor_x < 0 ? options : panel);
    g_MenuBlendColorBackup = Draw_GetBlendColor();
}

/* Up/down arrows over the three colour channels; the pair over the selected
 * channel lights up while that direction is held. */
void Menu_DrawBlendColorArrows(MenuWidgetNode *panel) {
    int dimmed0;
    int dimmed1;
    int dimmed2;
    int dimmed3;
    int dimmed4;
    int dimmed5;
    u16 directionHeld;
    s32 selectedChannel;

    selectedChannel = MenuWidget_GridCellIndex(MenuWidget_GetChild(panel, 0));
    directionHeld = MenuInput_GetStatusFlags() & 0x1000;
    Draw_OffsetCursor(0x12, 5);
    dimmed0 = 0;
    if ((selectedChannel != 0) || (directionHeld == 0)) {
        dimmed0 = 1;
    }
    Draw_SetTextDimmed(dimmed0);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    dimmed1 = 0;
    if ((selectedChannel != 1) || (directionHeld == 0)) {
        dimmed1 = 1;
    }
    Draw_SetTextDimmed(dimmed1);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    dimmed2 = 0;
    if ((selectedChannel != 2) || (directionHeld == 0)) {
        dimmed2 = 1;
    }
    Draw_SetTextDimmed(dimmed2);
    Draw_AllocSprite(0x4A);
    directionHeld = MenuInput_GetStatusFlags() & 0x4000;
    Draw_OffsetCursor(-0x40, 0x19);
    dimmed3 = 0;
    if ((selectedChannel != 0) || (directionHeld == 0)) {
        dimmed3 = 1;
    }
    Draw_SetTextDimmed(dimmed3);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    dimmed4 = 0;
    if ((selectedChannel != 1) || (directionHeld == 0)) {
        dimmed4 = 1;
    }
    Draw_SetTextDimmed(dimmed4);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    dimmed5 = 0;
    if ((selectedChannel != 2) || (directionHeld == 0)) {
        dimmed5 = 1;
    }
    Draw_SetTextDimmed(dimmed5);
    Draw_AllocSprite(0x4B);
}

s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags) {
    s32 shift;
    s32 choice;
    s32 color;
    s32 level;
    s32 value;
    s32 others;
    MenuWidgetNode *options;

    shift = MenuWidget_GridCellIndex(MenuWidget_GetChild(panel, 0)) << 3;
    color = Draw_GetBlendColor();
    if (flags & 0x1000) {
        others = color & ~(0xFF << shift);
        level = ((color >> shift) & 0xFF) + 2;
        if (level < 0xE9) {
            value = level << shift;
        } else {
            value = 0xE8 << shift;
        }
        Draw_BlendColor(others | value);
        Menu_PlayMoveSound();
    } else if (flags & 0x4000) {
        others = color & ~(0xFF << shift);
        level = ((color >> shift) & 0xFF) - 2;
        if (level >= 0x20) {
            value = level << shift;
        } else {
            value = 0x20 << shift;
        }
        Draw_BlendColor(others | value);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        options = MenuWidget_GetChild(panel, 1);
        choice = MenuWidget_GridCellIndex(options);
        if (choice == 0) {
            g_SavedDrawBlendColor[0] = Draw_GetBlendColor();
            MenuWidget_DestroyNode(panel);
        } else if (choice == 1) {
            Draw_BlendColor(0x404040);
            g_SavedDrawBlendColor[0] = Draw_GetBlendColor();
            MenuWidget_DestroyNode(panel);
        } else {
            options->cursor_x = 0;
            options->cursor_y = 0;
            MenuWidget_SetCurrentNode(options);
            MenuWidget_GetChild(panel, 0)->cursor_x = -1;
        }
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(panel);
        Draw_BlendColor(g_MenuBlendColorBackup);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBlendColorChannelList(int node) {
    MenuWidget_DrawList((MenuWidgetNode *)node, Menu_DrawBlendColorChannelRow);
}

int g_MenuScreenBaseYBackup;

void Menu_DrawBlendColorOptionList(int node) {
    MenuWidget_DrawList((MenuWidgetNode *)node, Menu_DrawBlendColorOptionRow);
}

void Menu_OpenScreenAdjustView(MenuWidgetNode *parent) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x38, parent, 0, 1);
    node->draw = (void (*)())Menu_DrawScreenAdjustPanel;
    node->update = (void (*)())Menu_ScreenAdjustInputHandler;
    MenuWidget_SetCurrentNode(node);
    g_MenuScreenBaseYBackup = Draw_GetBaseY();
}

void Menu_DrawScreenAdjustPanel(void) {
    Draw_OffsetCursor(0x10, 0xA);
    Draw_PrintSignedNumberWidth3(8 - Draw_GetBaseY());
    Draw_SetTextDimmed(VSync(-1) & 0x10);
    Draw_OffsetCursor(-0x10, -0x69);
    Draw_AllocSprite(0x7B);
    Draw_OffsetCursor(0, 0xBE);
    Draw_EmitGlyph(0x7B, 2);
}

int Menu_ScreenAdjustInputHandler(void *node, int input) {
    int zero_arg;

    do {
        if ((input & 0x1000) != 0) {
            zero_arg = 0;
            input = -1;
        } else if ((input & 0x4000) != 0) {
            zero_arg = 0;
            input = 1;
        } else {
            break;
        }
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    } while (0);

    if ((input & 0x10000) != 0) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x40) != 0) {
        Draw_SetBaseOffsetPosition(0, g_MenuScreenBaseYBackup - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}
