#include "pe1/text.h"
#include "pe1/menu_widget.h"

/* The port label under a save slot list; the list's parent selects the
 * port (selected_base 0x25 or 0x26). */
void Menu_DrawSaveSlotPortLabel(MenuWidgetNode *node) {
    Draw_OffsetCursor(0, 2);
    Draw_FlushPrimList(node->parent->selected_base - 0x11);
}
