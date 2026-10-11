#include "menu_memcard_node.h"

#include "func_8012B710_externs.h"

void func_8012B710(void) {
    MenuMemcardListNode *node;
    int kind;

    node = ((MenuMemcardListNode *)D_801D1370);
    kind = 2;
    while (node != 0) {
        if (node->kind == kind) {
            break;
        }
        node = node->next;
    }

    node->payload = &D_80193200;
}
