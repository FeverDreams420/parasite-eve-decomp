/* CC1_FLAGS: -fno-force-mem */
/* MASPSX_FLAGS: --expand-div */

#include "pe1/geom_state.h"
#include "pe1/game_state.h"

/* Geometry animation control: reset, update and script setup commands. */




int Obj_ResetAllEntries(void) {
    GeomState *state;
    GeomStateAddress base, controls, slots;
    GeomEntry *entries;
    unsigned int i;
    unsigned int count;

    state = D_800B1624;
    base.state = D_800B1624;
    controls.word = base.word + state->ctrl_offset;
    base.word += state->entry_offset;
    entries = base.entry;
    count = state->entry_count;
    for (i = 0; i < count; i++) {
        GeomCtrlEntry *control = &controls.ctrl[i];
        u8 group = control->position.parts.groupFraction;

        slots.ctrl = control;
        slots.word += control->slot_offset;
        control->step = 0x100;
        control->elapsed = 0;
        control->head.b.flags = 1;
        control->position.packed = group;
        entries[*slots.bytes].head.b.flags |= 2;
    }
    return 0;
}



/* Advances the geometry animations of the selected group. */
int Geom_StepGroupAnimations(void)
{
    GeomStateAddress state, controls, entries;
    unsigned int i, count;

    if (!(g_GameStateFlags & 0x104) &&
        (g_GameState.flags & 0xC00000) != 0x800000) {
        state.state = D_800B1624;
        controls.word = state.word + state.state->ctrl_offset;
        entries.word = state.word + state.state->entry_offset;
        count = state.state->entry_count;
        for (i = 0; i < count; i++) {
            GeomCtrlEntry *control = &controls.ctrl[i];
            int position = control->position.bits.value;

            if ((control->head.b.flags & 2) &&
                (control->head.b.flags & 0x14) &&
                control->position.bits.group == g_GeomGroupSel) {
                unsigned int j;
                GeomStateAddress slotTable;
                GeomAnimationSlot *slots;
                unsigned int frames;
                GeomAnimationSlot *slot;

                slotTable.ctrl = control;
                slotTable.word += control->slot_offset;
                slots = slotTable.slots;
                frames = control->head.packed >> 8;
                for (j = 0; j < frames; j++)
                    entries.entry[slots[j].entry].head.b.flags &= ~2;
                /* Subtraction retains retail's address-add operand order. */
                slot = slots - (-(position >> 8));
                entries.entry[slot->entry].head.b.flags |= 2;
                if (slot->duration < 0) {
                    slot->duration = 0;
                    control->elapsed = 0;
                    control->head.b.flags &= ~4;
                    return 0;
                }
                {
                    unsigned short elapsed = control->elapsed + 1;
                    control->elapsed = elapsed;
                    if (elapsed >= slot->duration) {
                        int next;
                        unsigned int length;

                        control->elapsed = 0;
                        control->position.bits.value = position + control->step;
                        next = control->position.bits.value;
                        length = control->head.packed >> 8;
                        if ((next >> 8) >= (int)length) {
                            if (control->head.b.flags & 0x20)
                                control->position.bits.value = next % (int)(length << 8);
                            else
                                control->position.bits.value = 0;
                            control->head.b.flags &= ~4;
                        } else if ((next >> 8) < 0) {
                            if (control->head.b.flags & 0x20)
                                control->position.bits.value = (int)(length << 8) -
                                    (-next % (int)(length << 8));
                            else
                                control->position.bits.value = (length - 1) << 8;
                            control->head.b.flags &= ~4;
                        }
                    }
                }
            }
        }
    }
    return 0;
}


/* Contiguous commands update the shared geometry animation control table. */

#define GEOM_CTRL_ENTRY(address, base, index) \
    (GEOM_STATE_OFFSET(address, base, ctrl_offset, (index) << 4), (address).ctrl)


int Obj_SetEntryLimit(int arg0, int arg1)
{
  GeomStateAddress address, base;
  GeomCtrlEntry *entry;
  u32 positionWord;
  register u32 flags;
  u32 hi;
  int ret;
  entry = GEOM_CTRL_ENTRY(address, base, arg0);
  positionWord = entry->position.packed;
  positionWord &= 0xFF;
  ret = entry->head.b.flags;
  flags = ret;
  hi = arg1 << 16;
  entry->elapsed = 0;
  positionWord |= hi;
  flags |= 2;
  entry->head.b.flags = flags;
  ret = 0;
  entry->position.packed = positionWord;
  return ret;
}


int Sys_SetFlagSlot(int arg0, int arg1) {
    GeomStateAddress table, base;
    GeomCtrlEntry *slot;

    GEOM_STATE_OFFSET(table, base, ctrl_offset, 0);
    slot = table.ctrl + arg0;
    if (arg1 != 0) {
        slot->head.b.flags |= 6;
    } else {
        slot->head.b.flags &= 0xF9;
    }

    return 0;
}


s16 Obj_GetEntryField6(int index) {
    GeomStateAddress address, base;

    return GEOM_CTRL_ENTRY(address, base, index)->position.parts.frame;
}

int Obj_SetEntryField8(int index, unsigned int value) {
    GeomStateAddress address, base;

    GEOM_CTRL_ENTRY(address, base, index)->step = value >> 8;
    return 0;
}

int Obj_FillEntrySlotValues(int index, u8 value)
{
    GeomStateAddress address, base, slots;
    GeomCtrlEntry *entry;
    int count;
    int i;

    entry = GEOM_CTRL_ENTRY(address, base, index);
    slots.ctrl = entry;
    slots.word += entry->slot_offset;
    count = entry->head.packed >> 8;
    for (i = 0; i < count; i++) {
        slots.slots[i].duration = value;
    }
    return 0;
}

int Obj_SetEntrySlotValue(int index, int slot, u8 value) {
    GeomStateAddress address, base;
    GeomCtrlEntry *entry;
    int ret;

    entry = GEOM_CTRL_ENTRY(address, base, index);
    address.word += entry->slot_offset;
    ret = 0;
    address.slots[slot].duration = value;
    return ret;
}

int Obj_SetEntryFlags(int index, int bits) {
    GeomStateAddress address, base;

    GEOM_CTRL_ENTRY(address, base, index)->head.b.flags |= bits & 0x30;
    return 0;
}



/* The do { } while (0) blocks are load-bearing: their loop notes fence the
 * cc1 scheduler so the flag load/stores keep the retail order (permuter zero). */
int Menu_SetSlotEntry(int index, unsigned int limit, unsigned int slot)
{
    GeomStateAddress table, base;
    GeomCtrlEntry *entry;
    u32 positionWord;
    register u32 flags;
    u32 hi;
    GeomAnimationSlot *slots;
    s32 offset;
    int neg;
    s16 f8;

    GEOM_STATE_OFFSET(table, base, ctrl_offset, index << 4);
    entry = table.ctrl;
    positionWord = entry->position.packed;
    positionWord &= 0xFF;
    offset = entry->slot_offset;
    hi = limit << 16;
    do {
        entry->elapsed = 0;
        positionWord |= hi;
        do {
            flags = entry->head.b.flags;
            table.word += offset;
            slots = table.slots;
            entry->position.packed = positionWord;
        } while (0);
        neg = -1;
        flags |= 6;
        entry->head.b.flags = flags;
        slots[slot].duration = neg;
        f8 = entry->step;
        if ((f8 > 0 && slot < limit) || (f8 < 0 && limit < slot)) {
            entry->step = -(u16)entry->step;
        }
        return 0;
    } while (0);
}
