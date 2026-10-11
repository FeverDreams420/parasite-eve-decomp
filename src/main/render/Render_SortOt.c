/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/render_prim.h"

/* Each entry packs a grid position: x in bits 22..31, y in bits 12..21. */
#define ENTRY_X(value) ((value) >> 22)
#define ENTRY_Y(value) (((value) >> 12) & 0x3FF)

/* Moves the grid cursor over `entries`. A restart picks the last entry of
 * the bottom row (descending) or the first entry of the top row. Otherwise
 * the cursor steps one column (16 units) left or right; if that column is
 * empty it moves to the nearest entry of the same row, or to the nearest end
 * of the next row, and restarts when there is none. */
int Render_MoveGridCursor(PrimObj *obj, u32 *entries, u8 ascending, u8 restart) {
    int bestX = 0x7FFF;
    int best = 0;
    int first = 0;
    int bestY;
    int step;
    int i;
    int y;
    u8 found;
    u8 later;
    u32 value;
    int previous;

    if (restart) {
        if (!ascending) {
            bestY = -1;
            bestX = -1;
            for (i = 0; i < obj->count; i++) {
                y = ENTRY_Y(entries[i]);
                if (bestY < y)
                    bestY = y;
            }
            for (i = 0; i < obj->count; i++) {
                value = entries[i];
                if (ENTRY_Y(value) == bestY && bestX < (int)ENTRY_X(value)) {
                    bestX = ENTRY_X(value);
                    first = i;
                }
            }
            previous = g_GridCursorIndex;
            g_GridCursorIndex = first;
            g_GridCursorPrevious = previous;
            g_GridCursorX = ENTRY_X(entries[first]);
            g_GridCursorY = ENTRY_Y(entries[first]);
            return first;
        }
        bestY = 0x7FFF;
        for (i = 0; i < obj->count; i++) {
            y = ENTRY_Y(entries[i]);
            if (y < bestY)
                bestY = y;
        }
        for (i = 0; i < obj->count; i++) {
            value = entries[i];
            if (ENTRY_Y(value) == bestY && (int)ENTRY_X(value) < bestX) {
                bestX = ENTRY_X(value);
                /* Matching debt: the loop notes of this one-pass block raise
                 * best's reference weight so the allocator ranks it ahead of
                 * found and later (see docs/CODE_ORGANIZATION.md). */
                do {
                    best = i;
                } while (0);
            }
        }
    } else {
        step = 16;
        if (!ascending)
            step = -16;
        found = 0;
        later = 0;
        for (i = 0; i < obj->count; i++) {
            value = entries[i];
            if (ENTRY_X(value) == g_GridCursorX + step) {
                if (ENTRY_Y(value) == g_GridCursorY) {
                    previous = g_GridCursorIndex;
                    g_GridCursorIndex = i;
                    g_GridCursorPrevious = previous;
                    g_GridCursorX = ENTRY_X(entries[i]);
                    g_GridCursorY = ENTRY_Y(entries[i]);
                    return i;
                }
            } else if (ENTRY_Y(value) == g_GridCursorY && g_GridCursorX < (int)ENTRY_X(value)) {
                later = 1;
            }
        }
        if (!found && later == 1) {
            if (!ascending) {
                bestX = 0;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY && (int)ENTRY_X(value) < g_GridCursorX &&
                        bestX < (int)ENTRY_X(value)) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            } else {
                bestX = 0x7FFF;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY && g_GridCursorX < (int)ENTRY_X(value) &&
                        (int)ENTRY_X(value) < bestX) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            }
        }
        if (found != 1) {
            found = 0;
            if (!ascending) {
                bestX = -1;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY + step && bestX < (int)ENTRY_X(value)) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            } else {
                bestX = 0x7FFF;
                for (i = 0; i < obj->count; i++) {
                    value = entries[i];
                    if (ENTRY_Y(value) == g_GridCursorY + step && (int)ENTRY_X(value) < bestX) {
                        bestX = ENTRY_X(value);
                        best = i;
                        found = 1;
                    }
                }
            }
            if (!found)
                return Render_MoveGridCursor(obj, entries, ascending, 1);
        }
    }
    previous = g_GridCursorIndex;
    g_GridCursorIndex = best;
    g_GridCursorPrevious = previous;
    g_GridCursorX = ENTRY_X(entries[best]);
    g_GridCursorY = ENTRY_Y(entries[best]);
    return best;
}
