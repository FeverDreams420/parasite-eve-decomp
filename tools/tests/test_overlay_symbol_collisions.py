import unittest

from tools.scripts import overlay_symbol_collisions as osc


def result(**conflicts):
    return {"conflicting": {n: {"locations": loc} for n, loc in conflicts.items()}}


class NewConflictsTests(unittest.TestCase):
    BASE = {
        "conflicting": ["D_80190084", "g_RoomScript"],
        "conflicting_locations": {
            "D_80190084": ["room_a:0x80190084", "room_b:0x80190084", "room_c:0x80190084"],
            "g_RoomScript": ["room_a:0x80191000", "room_b:0x80192000"],
        },
    }

    def test_baseline_names_pass(self):
        res = result(D_80190084=["room_a:0x80190084", "room_b:0x80190084"])
        self.assertEqual(osc.new_conflicts(res, self.BASE), [])

    def test_renamed_auto_name_is_not_new(self):
        res = result(g_RoomDropFrames=["room_a:0x80190084", "room_b:0x80190084",
                                       "room_c:0x80190084"])
        self.assertEqual(osc.new_conflicts(res, self.BASE), [])

    def test_partly_renamed_conflict_is_not_new(self):
        res = result(g_RoomDropFrames=["room_a:0x80190084", "room_c:0x80190084"])
        self.assertEqual(osc.new_conflicts(res, self.BASE), [])

    def test_new_location_is_new(self):
        res = result(g_RoomDropFrames=["room_a:0x80190084", "room_d:0x80190084"])
        self.assertEqual(osc.new_conflicts(res, self.BASE), ["g_RoomDropFrames"])

    def test_union_of_two_conflicts_is_new(self):
        res = result(g_Merged=["room_a:0x80190084", "room_a:0x80191000"])
        self.assertEqual(osc.new_conflicts(res, self.BASE), ["g_Merged"])

    def test_old_baseline_without_locations(self):
        base = {"conflicting": ["D_80190084"]}
        res = result(g_New=["room_a:0x80190084"])
        self.assertEqual(osc.new_conflicts(res, base), ["g_New"])


if __name__ == "__main__":
    unittest.main()
