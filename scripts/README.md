# Parasite Eve 1 — Decompiled Scripts (`.pes` & `.fxs`)

## 1. Scene VM (`Script_RunQueue`) — Room Scripts (`scripts/rooms/*.pes`)
- **Total valid rooms**: `414` (out of `450` entries in `g_RoomSectorTable`)
- **Total actor scripts**: `2,411`
- **Total binary script size**: `9,411,076` bytes (`100%` byte-matching round-trip)

## 2. FieldFx VM (`FieldFx_RunScript`) — Effect Scripts (`scripts/fieldfx/*.fxs`)
- **Total binary modules**: `64` (`main_exe` + `63` room/scene overlays)
- **Total FieldFx scripts**: `84` (`8` global in `main.exe` + `76` in overlays)
- **Total FieldFx bytecode size**: `1,619` words (`6,476` bytes, `100%` byte-matching round-trip)
- **Central Opcode & Variable Mapping**: [`MAPPING.md`](MAPPING.md)

---

## FieldFx Script Index (`scripts/fieldfx/`)

| Script File | Module | Scripts | Slots | Words (Bytes) | Script Symbols (VRAM) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| [`main_exe.fxs`](fieldfx/main_exe.fxs) | `main_exe` | `8` | `35` | `190` (`760` B) | `g_GlowEffectScript` (`0x800E0928`), `g_GlowEffectBScript` (`0x800E0A50`), `g_GlowEffectCScript` (`0x800E0B38`), `g_FourLayerGlowScript` (`0x800E0C88`), `g_PointSpriteScript` (`0x800E0D6C`), `g_BurstEffectScript` (`0x800E0EB8`), `g_BurstEffectBScript` (`0x800E0F6C`), `g_BurstEffectCScript` (`0x800E0FFC`) |
| [`room_m034_sewer.fxs`](fieldfx/room_m034_sewer.fxs) | `room_m034_sewer` | `1` | `4` | `35` (`140` B) | `g_RoomGlowOrbScript` (`0x8018FF98`) |
| [`room_m049_nypd.fxs`](fieldfx/room_m049_nypd.fxs) | `room_m049_nypd` | `1` | `7` | `2` (`8` B) | `g_RoomScript` (`0x80192BE8`) |
| [`room_m050_nypd.fxs`](fieldfx/room_m050_nypd.fxs) | `room_m050_nypd` | `1` | `7` | `2` (`8` B) | `g_RoomScript` (`0x80192BE8`) |
| [`room_m065_central_park.fxs`](fieldfx/room_m065_central_park.fxs) | `room_m065_central_park` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x80190040`) |
| [`room_m075_central_park.fxs`](fieldfx/room_m075_central_park.fxs) | `room_m075_central_park` | `1` | `5` | `30` (`120` B) | `g_RoomScript` (`0x80193F90`) |
| [`room_m080_central_park.fxs`](fieldfx/room_m080_central_park.fxs) | `room_m080_central_park` | `1` | `5` | `30` (`120` B) | `g_RoomScript` (`0x80193F90`) |
| [`room_m082_central_park.fxs`](fieldfx/room_m082_central_park.fxs) | `room_m082_central_park` | `1` | `5` | `30` (`120` B) | `g_RoomScript` (`0x80193F90`) |
| [`room_m085_central_park.fxs`](fieldfx/room_m085_central_park.fxs) | `room_m085_central_park` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x80190040`) |
| [`room_m087_central_park.fxs`](fieldfx/room_m087_central_park.fxs) | `room_m087_central_park` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x80193534`) |
| [`room_m090_soho.fxs`](fieldfx/room_m090_soho.fxs) | `room_m090_soho` | `1` | `7` | `2` (`8` B) | `g_RoomScript` (`0x80192BE8`) |
| [`room_m107_nypd.fxs`](fieldfx/room_m107_nypd.fxs) | `room_m107_nypd` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x801935F8`) |
| [`room_m111_nypd.fxs`](fieldfx/room_m111_nypd.fxs) | `room_m111_nypd` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80190104`) |
| [`room_m114_nypd.fxs`](fieldfx/room_m114_nypd.fxs) | `room_m114_nypd` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x801935F8`) |
| [`room_m118_nypd.fxs`](fieldfx/room_m118_nypd.fxs) | `room_m118_nypd` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80190104`) |
| [`room_m122_nypd.fxs`](fieldfx/room_m122_nypd.fxs) | `room_m122_nypd` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80190F4C`) |
| [`room_m137_hospital.fxs`](fieldfx/room_m137_hospital.fxs) | `room_m137_hospital` | `1` | `5` | `19` (`76` B) | `g_RoomScript` (`0x80190EA0`) |
| [`room_m140_hospital.fxs`](fieldfx/room_m140_hospital.fxs) | `room_m140_hospital` | `1` | `5` | `19` (`76` B) | `g_RoomScript` (`0x80190EA0`) |
| [`room_m141_hospital.fxs`](fieldfx/room_m141_hospital.fxs) | `room_m141_hospital` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x8019205C`) |
| [`room_m146_hospital.fxs`](fieldfx/room_m146_hospital.fxs) | `room_m146_hospital` | `2` | `11` | `37` (`148` B) | `g_RoomEruptionScript` (`0x801924C8`), `g_RoomScript` (`0x80192580`) |
| [`room_m147_hospital.fxs`](fieldfx/room_m147_hospital.fxs) | `room_m147_hospital` | `1` | `5` | `19` (`76` B) | `g_RoomScript` (`0x80190EA0`) |
| [`room_m153_hospital.fxs`](fieldfx/room_m153_hospital.fxs) | `room_m153_hospital` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x8019553C`) |
| [`room_m154_hospital.fxs`](fieldfx/room_m154_hospital.fxs) | `room_m154_hospital` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x8019553C`) |
| [`room_m167_sewer2.fxs`](fieldfx/room_m167_sewer2.fxs) | `room_m167_sewer2` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m168_sewer2.fxs`](fieldfx/room_m168_sewer2.fxs) | `room_m168_sewer2` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m169_sewer2.fxs`](fieldfx/room_m169_sewer2.fxs) | `room_m169_sewer2` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m173_sewer2.fxs`](fieldfx/room_m173_sewer2.fxs) | `room_m173_sewer2` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m174_sewer2.fxs`](fieldfx/room_m174_sewer2.fxs) | `room_m174_sewer2` | `2` | `17` | `83` (`332` B) | `g_RoomGlowOrbScript` (`0x80196CE0`), `g_RoomDropFieldScript` (`0x80196E4C`) |
| [`room_m178_sewer2.fxs`](fieldfx/room_m178_sewer2.fxs) | `room_m178_sewer2` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m186_subway.fxs`](fieldfx/room_m186_subway.fxs) | `room_m186_subway` | `2` | `6` | `33` (`132` B) | `g_RoomScript` (`0x801941BC`), `g_RoomOrbitSetScript` (`0x80194230`) |
| [`room_m187_subway.fxs`](fieldfx/room_m187_subway.fxs) | `room_m187_subway` | `1` | `4` | `29` (`116` B) | `g_RoomOrbitSetScript` (`0x8019000C`) |
| [`room_m295_chrysler.fxs`](fieldfx/room_m295_chrysler.fxs) | `room_m295_chrysler` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m299_chrysler.fxs`](fieldfx/room_m299_chrysler.fxs) | `room_m299_chrysler` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m309_chrysler.fxs`](fieldfx/room_m309_chrysler.fxs) | `room_m309_chrysler` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m310_chrysler.fxs`](fieldfx/room_m310_chrysler.fxs) | `room_m310_chrysler` | `1` | `2` | `4` (`16` B) | `g_RoomScript` (`0x80193208`) |
| [`room_m324_hospital.fxs`](fieldfx/room_m324_hospital.fxs) | `room_m324_hospital` | `1` | `5` | `19` (`76` B) | `g_RoomScript` (`0x80190EA0`) |
| [`room_m327_hospital.fxs`](fieldfx/room_m327_hospital.fxs) | `room_m327_hospital` | `1` | `5` | `19` (`76` B) | `g_RoomScript` (`0x80190EA0`) |
| [`room_m328_hospital.fxs`](fieldfx/room_m328_hospital.fxs) | `room_m328_hospital` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x8019205C`) |
| [`room_m344_museum.fxs`](fieldfx/room_m344_museum.fxs) | `room_m344_museum` | `1` | `7` | `2` (`8` B) | `g_RoomScript` (`0x80192BE8`) |
| [`room_m348_sewer.fxs`](fieldfx/room_m348_sewer.fxs) | `room_m348_sewer` | `1` | `13` | `48` (`192` B) | `g_RoomDropFieldScript` (`0x80192928`) |
| [`room_m374_central_park.fxs`](fieldfx/room_m374_central_park.fxs) | `room_m374_central_park` | `1` | `7` | `2` (`8` B) | `g_RoomScript` (`0x80192BE8`) |
| [`room_m383_chrysler.fxs`](fieldfx/room_m383_chrysler.fxs) | `room_m383_chrysler` | `2` | `17` | `83` (`332` B) | `g_RoomGlowOrbScript` (`0x80196CE0`), `g_RoomDropFieldScript` (`0x80196E4C`) |
| [`room_m385_chrysler.fxs`](fieldfx/room_m385_chrysler.fxs) | `room_m385_chrysler` | `2` | `6` | `33` (`132` B) | `g_RoomScript` (`0x801941BC`), `g_RoomOrbitSetScript` (`0x80194230`) |
| [`room_m388_chrysler.fxs`](fieldfx/room_m388_chrysler.fxs) | `room_m388_chrysler` | `2` | `6` | `33` (`132` B) | `g_RoomScript` (`0x801941BC`), `g_RoomOrbitSetScript` (`0x80194230`) |
| [`room_m389_chrysler.fxs`](fieldfx/room_m389_chrysler.fxs) | `room_m389_chrysler` | `2` | `6` | `33` (`132` B) | `g_RoomScript` (`0x801941BC`), `g_RoomOrbitSetScript` (`0x80194230`) |
| [`room_m391_chrysler.fxs`](fieldfx/room_m391_chrysler.fxs) | `room_m391_chrysler` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x8019437C`) |
| [`room_m392_chrysler.fxs`](fieldfx/room_m392_chrysler.fxs) | `room_m392_chrysler` | `1` | `5` | `19` (`76` B) | `g_RoomScript` (`0x801944F4`) |
| [`room_m393_chrysler.fxs`](fieldfx/room_m393_chrysler.fxs) | `room_m393_chrysler` | `2` | `7` | `33` (`132` B) | `g_RoomThreeParticleScript` (`0x80194688`), `g_RoomScript` (`0x80194740`) |
| [`room_m394_chrysler.fxs`](fieldfx/room_m394_chrysler.fxs) | `room_m394_chrysler` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x8019437C`) |
| [`room_m395_chrysler.fxs`](fieldfx/room_m395_chrysler.fxs) | `room_m395_chrysler` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x8019437C`) |
| [`room_m396_chrysler.fxs`](fieldfx/room_m396_chrysler.fxs) | `room_m396_chrysler` | `2` | `7` | `33` (`132` B) | `g_RoomThreeParticleScript` (`0x80194688`), `g_RoomScript` (`0x80194740`) |
| [`room_m399_chrysler.fxs`](fieldfx/room_m399_chrysler.fxs) | `room_m399_chrysler` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x80194208`) |
| [`room_m401_chrysler.fxs`](fieldfx/room_m401_chrysler.fxs) | `room_m401_chrysler` | `2` | `7` | `33` (`132` B) | `g_RoomThreeParticleScript` (`0x8019535C`), `g_RoomScript` (`0x80195414`) |
| [`room_m402_chrysler.fxs`](fieldfx/room_m402_chrysler.fxs) | `room_m402_chrysler` | `1` | `2` | `14` (`56` B) | `g_RoomThreeParticleScript` (`0x80194208`) |
| [`scene_e02_chrysler.fxs`](fieldfx/scene_e02_chrysler.fxs) | `scene_e02_chrysler` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x801954EC`) |
| [`scene_e04_chrysler.fxs`](fieldfx/scene_e04_chrysler.fxs) | `scene_e04_chrysler` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x80191FF8`) |
| [`scene_e05_chrysler.fxs`](fieldfx/scene_e05_chrysler.fxs) | `scene_e05_chrysler` | `1` | `6` | `18` (`72` B) | `g_RoomEruptionScript` (`0x80191FF8`) |
| [`scene_e08_chrysler.fxs`](fieldfx/scene_e08_chrysler.fxs) | `scene_e08_chrysler` | `4` | `23` | `101` (`404` B) | `g_RoomScript` (`0x801986C8`), `g_SceneE08OrbScript` (`0x8019877C`), `g_SceneE08SlotsScript` (`0x80198930`), `g_SceneE08LayeredSpriteScript` (`0x80198A64`) |
| [`scene_e09_chrysler.fxs`](fieldfx/scene_e09_chrysler.fxs) | `scene_e09_chrysler` | `1` | `5` | `30` (`120` B) | `g_RoomScript` (`0x80194C78`) |
| [`scene_e10_chrysler.fxs`](fieldfx/scene_e10_chrysler.fxs) | `scene_e10_chrysler` | `1` | `5` | `30` (`120` B) | `g_RoomScript` (`0x80194C78`) |
| [`scene_e11_chrysler.fxs`](fieldfx/scene_e11_chrysler.fxs) | `scene_e11_chrysler` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80191A70`) |
| [`scene_e12_chrysler.fxs`](fieldfx/scene_e12_chrysler.fxs) | `scene_e12_chrysler` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80191A70`) |
| [`scene_e13_chrysler.fxs`](fieldfx/scene_e13_chrysler.fxs) | `scene_e13_chrysler` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80191A70`) |
| [`scene_e14_chrysler.fxs`](fieldfx/scene_e14_chrysler.fxs) | `scene_e14_chrysler` | `1` | `5` | `26` (`104` B) | `g_RoomScript` (`0x80190D88`) |

---

## Room Script Index (`scripts/rooms/`)

| Script File | Room ID | Actors | Spawns | Binary Size | Dialogues | Sample Dialogue |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| [`room_000.pes`](rooms/room_000.pes) | `0` | `6` | `1` | `5,836` B | `204` | Found "ItemItem", |
| [`room_001.pes`](rooms/room_001.pes) | `1` | `6` | `1` | `3,880` B | `204` | Found "ItemItem", |
| [`room_002.pes`](rooms/room_002.pes) | `2` | `6` | `1` | `3,952` B | `204` | Found "ItemItem", |
| [`room_003.pes`](rooms/room_003.pes) | `3` | `5` | `1` | `4,684` B | `204` | Found "ItemItem", |
| [`room_004.pes`](rooms/room_004.pes) | `4` | `7` | `2` | `19,832` B | `204` | Found "ItemItem", |
| [`room_005.pes`](rooms/room_005.pes) | `5` | `4` | `1` | `2,860` B | `204` | Found "ItemItem", |
| [`room_008.pes`](rooms/room_008.pes) | `8` | `5` | `1` | `4,860` B | `204` | Found "ItemItem", |
| [`room_009.pes`](rooms/room_009.pes) | `9` | `4` | `1` | `2,828` B | `204` | Found "ItemItem", |
| [`room_010.pes`](rooms/room_010.pes) | `10` | `4` | `1` | `2,900` B | `204` | Found "ItemItem", |
| [`room_011.pes`](rooms/room_011.pes) | `11` | `7` | `2` | `28,592` B | `204` | Found "ItemItem", |
| [`room_012.pes`](rooms/room_012.pes) | `12` | `4` | `2` | `10,432` B | `204` | Found "ItemItem", |
| [`room_013.pes`](rooms/room_013.pes) | `13` | `9` | `2` | `23,652` B | `204` | Found "ItemItem", |
| [`room_015.pes`](rooms/room_015.pes) | `15` | `8` | `2` | `22,468` B | `204` | Found "ItemItem", |
| [`room_016.pes`](rooms/room_016.pes) | `16` | `7` | `1` | `10,576` B | `204` | Found "ItemItem", |
| [`room_017.pes`](rooms/room_017.pes) | `17` | `6` | `1` | `6,256` B | `204` | Found "ItemItem", |
| [`room_019.pes`](rooms/room_019.pes) | `19` | `8` | `1` | `8,420` B | `204` | Found "ItemItem", |
| [`room_020.pes`](rooms/room_020.pes) | `20` | `7` | `1` | `8,800` B | `204` | Found "ItemItem", |
| [`room_021.pes`](rooms/room_021.pes) | `21` | `6` | `2` | `12,720` B | `204` | Found "ItemItem", |
| [`room_022.pes`](rooms/room_022.pes) | `22` | `4` | `2` | `14,812` B | `204` | Found "ItemItem", |
| [`room_023.pes`](rooms/room_023.pes) | `23` | `6` | `1` | `8,080` B | `204` | Found "ItemItem", |
| [`room_024.pes`](rooms/room_024.pes) | `24` | `5` | `1` | `5,032` B | `204` | Found "ItemItem", |
| [`room_025.pes`](rooms/room_025.pes) | `25` | `3` | `1` | `2,012` B | `25` | Found "ItemItem", |
| [`room_026.pes`](rooms/room_026.pes) | `26` | `5` | `2` | `16,980` B | `25` | Found "ItemItem", |
| [`room_027.pes`](rooms/room_027.pes) | `27` | `10` | `2` | `27,880` B | `25` | Found "ItemItem", |
| [`room_028.pes`](rooms/room_028.pes) | `28` | `4` | `1` | `2,468` B | `25` | Found "ItemItem", |
| [`room_029.pes`](rooms/room_029.pes) | `29` | `7` | `2` | `22,196` B | `25` | Found "ItemItem", |
| [`room_030.pes`](rooms/room_030.pes) | `30` | `9` | `1` | `9,652` B | `25` | Found "ItemItem", |
| [`room_031.pes`](rooms/room_031.pes) | `31` | `8` | `2` | `20,360` B | `25` | Found "ItemItem", |
| [`room_032.pes`](rooms/room_032.pes) | `32` | `5` | `1` | `5,220` B | `25` | Found "ItemItem", |
| [`room_033.pes`](rooms/room_033.pes) | `33` | `6` | `2` | `22,384` B | `25` | Found "ItemItem", |
| [`room_034.pes`](rooms/room_034.pes) | `34` | `5` | `1` | `6,900` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_035.pes`](rooms/room_035.pes) | `35` | `4` | `1` | `6,028` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_036.pes`](rooms/room_036.pes) | `36` | `5` | `1` | `11,732` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_037.pes`](rooms/room_037.pes) | `37` | `5` | `1` | `10,728` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_038.pes`](rooms/room_038.pes) | `38` | `5` | `1` | `12,740` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_039.pes`](rooms/room_039.pes) | `39` | `5` | `1` | `5,688` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_040.pes`](rooms/room_040.pes) | `40` | `9` | `1` | `25,688` B | `224` | / Found "ItemItem", |
| [`room_041.pes`](rooms/room_041.pes) | `41` | `8` | `1` | `13,468` B | `224` | / Found "ItemItem", |
| [`room_042.pes`](rooms/room_042.pes) | `42` | `7` | `1` | `15,272` B | `224` | / Found "ItemItem", |
| [`room_043.pes`](rooms/room_043.pes) | `43` | `8` | `1` | `14,296` B | `205` | / Found "ItemItem", |
| [`room_044.pes`](rooms/room_044.pes) | `44` | `10` | `1` | `6,880` B | `224` | / Found "ItemItem", |
| [`room_045.pes`](rooms/room_045.pes) | `45` | `6` | `1` | `10,140` B | `224` | / Found "ItemItem", |
| [`room_046.pes`](rooms/room_046.pes) | `46` | `10` | `1` | `18,976` B | `224` | / Found "ItemItem", |
| [`room_047.pes`](rooms/room_047.pes) | `47` | `7` | `1` | `23,192` B | `205` | / Found "ItemItem", |
| [`room_048.pes`](rooms/room_048.pes) | `48` | `6` | `1` | `4,556` B | `204` | Found "ItemItem", |
| [`room_049.pes`](rooms/room_049.pes) | `49` | `6` | `1` | `2,996` B | `204` | Found "ItemItem", |
| [`room_050.pes`](rooms/room_050.pes) | `50` | `8` | `1` | `10,120` B | `224` | / Found "ItemItem", |
| [`room_051.pes`](rooms/room_051.pes) | `51` | `8` | `1` | `28,188` B | `224` | / Found "ItemItem", |
| [`room_052.pes`](rooms/room_052.pes) | `52` | `4` | `1` | `2,404` B | `224` | / Found "ItemItem", |
| [`room_053.pes`](rooms/room_053.pes) | `53` | `4` | `1` | `1,844` B | `224` | / Found "ItemItem", |
| [`room_054.pes`](rooms/room_054.pes) | `54` | `9` | `1` | `17,324` B | `224` | / Found "ItemItem", |
| [`room_055.pes`](rooms/room_055.pes) | `55` | `5` | `1` | `3,316` B | `224` | / Found "ItemItem", |
| [`room_056.pes`](rooms/room_056.pes) | `56` | `3` | `1` | `1,540` B | `110` | Found "ItemItem", |
| [`room_057.pes`](rooms/room_057.pes) | `57` | `6` | `1` | `6,832` B | `110` | Found "ItemItem", |
| [`room_058.pes`](rooms/room_058.pes) | `58` | `8` | `2` | `39,828` B | `110` | Found "ItemItem", |
| [`room_059.pes`](rooms/room_059.pes) | `59` | `5` | `1` | `6,312` B | `110` | Found "ItemItem", |
| [`room_060.pes`](rooms/room_060.pes) | `60` | `7` | `2` | `32,976` B | `110` | Found "ItemItem", |
| [`room_061.pes`](rooms/room_061.pes) | `61` | `8` | `2` | `30,292` B | `110` | Found "ItemItem", |
| [`room_062.pes`](rooms/room_062.pes) | `62` | `6` | `2` | `24,712` B | `110` | Found "ItemItem", |
| [`room_063.pes`](rooms/room_063.pes) | `63` | `8` | `1` | `10,364` B | `110` | Found "ItemItem", |
| [`room_064.pes`](rooms/room_064.pes) | `64` | `5` | `2` | `22,460` B | `110` | Found "ItemItem", |
| [`room_065.pes`](rooms/room_065.pes) | `65` | `7` | `2` | `28,884` B | `110` | Found "ItemItem", |
| [`room_066.pes`](rooms/room_066.pes) | `66` | `6` | `2` | `44,688` B | `110` | Found "ItemItem", |
| [`room_067.pes`](rooms/room_067.pes) | `67` | `5` | `1` | `2,684` B | `110` | Found "ItemItem", |
| [`room_068.pes`](rooms/room_068.pes) | `68` | `4` | `1` | `3,400` B | `110` | Found "ItemItem", |
| [`room_069.pes`](rooms/room_069.pes) | `69` | `4` | `1` | `2,676` B | `110` | Found "ItemItem", |
| [`room_070.pes`](rooms/room_070.pes) | `70` | `3` | `1` | `1,308` B | `110` | Found "ItemItem", |
| [`room_071.pes`](rooms/room_071.pes) | `71` | `4` | `1` | `4,736` B | `110` | Found "ItemItem", |
| [`room_072.pes`](rooms/room_072.pes) | `72` | `3` | `1` | `2,412` B | `110` | Found "ItemItem", |
| [`room_073.pes`](rooms/room_073.pes) | `73` | `6` | `1` | `6,808` B | `110` | Found "ItemItem", |
| [`room_074.pes`](rooms/room_074.pes) | `74` | `7` | `2` | `24,516` B | `110` | Found "ItemItem", |
| [`room_075.pes`](rooms/room_075.pes) | `75` | `4` | `1` | `3,372` B | `110` | Found "ItemItem", |
| [`room_076.pes`](rooms/room_076.pes) | `76` | `5` | `1` | `4,200` B | `110` | Found "ItemItem", |
| [`room_077.pes`](rooms/room_077.pes) | `77` | `7` | `2` | `38,396` B | `110` | Found "ItemItem", |
| [`room_078.pes`](rooms/room_078.pes) | `78` | `6` | `1` | `4,896` B | `110` | Found "ItemItem", |
| [`room_079.pes`](rooms/room_079.pes) | `79` | `8` | `2` | `40,720` B | `110` | Found "ItemItem", |
| [`room_080.pes`](rooms/room_080.pes) | `80` | `5` | `2` | `28,504` B | `21` | Eve. You cannot win: submit, |
| [`room_081.pes`](rooms/room_081.pes) | `81` | `7` | `2` | `32,096` B | `110` | Found "ItemItem", |
| [`room_082.pes`](rooms/room_082.pes) | `82` | `6` | `2` | `25,792` B | `110` | Found "ItemItem", |
| [`room_083.pes`](rooms/room_083.pes) | `83` | `5` | `1` | `7,096` B | `110` | Found "ItemItem", |
| [`room_084.pes`](rooms/room_084.pes) | `84` | `5` | `2` | `24,612` B | `110` | Found "ItemItem", |
| [`room_085.pes`](rooms/room_085.pes) | `85` | `5` | `2` | `33,360` B | `110` | Found "ItemItem", |
| [`room_086.pes`](rooms/room_086.pes) | `86` | `6` | `2` | `40,424` B | `110` | Found "ItemItem", |
| [`room_087.pes`](rooms/room_087.pes) | `87` | `4` | `1` | `4,224` B | `110` | Found "ItemItem", |
| [`room_088.pes`](rooms/room_088.pes) | `88` | `5` | `2` | `25,048` B | `110` | Found "ItemItem", |
| [`room_089.pes`](rooms/room_089.pes) | `89` | `6` | `1` | `5,600` B | `134` | Found "ItemItem", |
| [`room_090.pes`](rooms/room_090.pes) | `90` | `5` | `1` | `13,168` B | `134` | Found "ItemItem", |
| [`room_091.pes`](rooms/room_091.pes) | `91` | `5` | `1` | `4,212` B | `134` | Found "ItemItem", |
| [`room_092.pes`](rooms/room_092.pes) | `92` | `7` | `1` | `16,344` B | `134` | Found "ItemItem", |
| [`room_093.pes`](rooms/room_093.pes) | `93` | `6` | `1` | `8,132` B | `134` | Found "ItemItem", |
| [`room_094.pes`](rooms/room_094.pes) | `94` | `9` | `1` | `12,800` B | `134` | Found "ItemItem", |
| [`room_095.pes`](rooms/room_095.pes) | `95` | `5` | `1` | `6,896` B | `134` | Found "ItemItem", |
| [`room_096.pes`](rooms/room_096.pes) | `96` | `10` | `1` | `15,640` B | `134` | Found "ItemItem", |
| [`room_097.pes`](rooms/room_097.pes) | `97` | `6` | `1` | `8,664` B | `205` | / Found "ItemItem", |
| [`room_098.pes`](rooms/room_098.pes) | `98` | `5` | `1` | `7,184` B | `205` | / Found "ItemItem", |
| [`room_099.pes`](rooms/room_099.pes) | `99` | `8` | `2` | `46,460` B | `205` | / Found "ItemItem", |
| [`room_101.pes`](rooms/room_101.pes) | `101` | `8` | `1` | `6,716` B | `205` | / Found "ItemItem", |
| [`room_102.pes`](rooms/room_102.pes) | `102` | `5` | `1` | `16,708` B | `205` | / Found "ItemItem", |
| [`room_103.pes`](rooms/room_103.pes) | `103` | `7` | `2` | `26,896` B | `205` | / Found "ItemItem", |
| [`room_105.pes`](rooms/room_105.pes) | `105` | `6` | `1` | `7,420` B | `205` | / Found "ItemItem", |
| [`room_106.pes`](rooms/room_106.pes) | `106` | `8` | `2` | `42,544` B | `205` | / Found "ItemItem", |
| [`room_107.pes`](rooms/room_107.pes) | `107` | `8` | `1` | `14,556` B | `205` | / Found "ItemItem", |
| [`room_108.pes`](rooms/room_108.pes) | `108` | `4` | `1` | `3,184` B | `205` | / Found "ItemItem", |
| [`room_109.pes`](rooms/room_109.pes) | `109` | `8` | `2` | `39,020` B | `205` | / Found "ItemItem", |
| [`room_110.pes`](rooms/room_110.pes) | `110` | `8` | `2` | `23,912` B | `205` | / Found "ItemItem", |
| [`room_111.pes`](rooms/room_111.pes) | `111` | `6` | `1` | `7,100` B | `205` | / Found "ItemItem", |
| [`room_112.pes`](rooms/room_112.pes) | `112` | `10` | `2` | `43,448` B | `205` | / Found "ItemItem", |
| [`room_113.pes`](rooms/room_113.pes) | `113` | `8` | `2` | `24,024` B | `205` | / Found "ItemItem", |
| [`room_114.pes`](rooms/room_114.pes) | `114` | `8` | `2` | `32,980` B | `205` | / Found "ItemItem", |
| [`room_115.pes`](rooms/room_115.pes) | `115` | `11` | `2` | `48,280` B | `205` | / Found "ItemItem", |
| [`room_116.pes`](rooms/room_116.pes) | `116` | `7` | `1` | `15,488` B | `205` | / Found "ItemItem", |
| [`room_117.pes`](rooms/room_117.pes) | `117` | `8` | `2` | `26,184` B | `205` | / Found "ItemItem", |
| [`room_119.pes`](rooms/room_119.pes) | `119` | `8` | `2` | `45,736` B | `205` | / Found "ItemItem", |
| [`room_120.pes`](rooms/room_120.pes) | `120` | `11` | `2` | `43,296` B | `205` | / Found "ItemItem", |
| [`room_121.pes`](rooms/room_121.pes) | `121` | `7` | `2` | `38,100` B | `205` | / Found "ItemItem", |
| [`room_122.pes`](rooms/room_122.pes) | `122` | `8` | `2` | `28,148` B | `205` | / Found "ItemItem", |
| [`room_123.pes`](rooms/room_123.pes) | `123` | `3` | `1` | `3,148` B | `205` | / Found "ItemItem", |
| [`room_124.pes`](rooms/room_124.pes) | `124` | `4` | `1` | `13,180` B | `137` | Found "ItemItem", |
| [`room_125.pes`](rooms/room_125.pes) | `125` | `9` | `2` | `31,120` B | `137` | Found "ItemItem", |
| [`room_126.pes`](rooms/room_126.pes) | `126` | `6` | `1` | `12,976` B | `137` | Found "ItemItem", |
| [`room_127.pes`](rooms/room_127.pes) | `127` | `3` | `1` | `3,860` B | `137` | Found "ItemItem", |
| [`room_128.pes`](rooms/room_128.pes) | `128` | `5` | `2` | `20,564` B | `137` | Found "ItemItem", |
| [`room_129.pes`](rooms/room_129.pes) | `129` | `3` | `1` | `3,180` B | `137` | Found "ItemItem", |
| [`room_130.pes`](rooms/room_130.pes) | `130` | `3` | `1` | `2,088` B | `137` | Found "ItemItem", |
| [`room_131.pes`](rooms/room_131.pes) | `131` | `3` | `1` | `5,228` B | `137` | Found "ItemItem", |
| [`room_132.pes`](rooms/room_132.pes) | `132` | `3` | `1` | `5,988` B | `137` | Found "ItemItem", |
| [`room_133.pes`](rooms/room_133.pes) | `133` | `3` | `1` | `1,572` B | `137` | Found "ItemItem", |
| [`room_134.pes`](rooms/room_134.pes) | `134` | `3` | `1` | `1,444` B | `137` | Found "ItemItem", |
| [`room_135.pes`](rooms/room_135.pes) | `135` | `3` | `1` | `3,676` B | `137` | Found "ItemItem", |
| [`room_136.pes`](rooms/room_136.pes) | `136` | `7` | `2` | `31,588` B | `137` | Found "ItemItem", |
| [`room_137.pes`](rooms/room_137.pes) | `137` | `5` | `1` | `7,252` B | `137` | Found "ItemItem", |
| [`room_138.pes`](rooms/room_138.pes) | `138` | `8` | `2` | `26,536` B | `137` | Found "ItemItem", |
| [`room_139.pes`](rooms/room_139.pes) | `139` | `8` | `2` | `35,172` B | `137` | Found "ItemItem", |
| [`room_140.pes`](rooms/room_140.pes) | `140` | `9` | `2` | `33,280` B | `137` | Found "ItemItem", |
| [`room_141.pes`](rooms/room_141.pes) | `141` | `4` | `1` | `6,440` B | `137` | Found "ItemItem", |
| [`room_142.pes`](rooms/room_142.pes) | `142` | `4` | `1` | `3,196` B | `137` | Found "ItemItem", |
| [`room_143.pes`](rooms/room_143.pes) | `143` | `4` | `1` | `17,684` B | `137` | Found "ItemItem", |
| [`room_144.pes`](rooms/room_144.pes) | `144` | `9` | `2` | `30,900` B | `137` | Found "ItemItem", |
| [`room_145.pes`](rooms/room_145.pes) | `145` | `9` | `2` | `38,724` B | `137` | Found "ItemItem", |
| [`room_146.pes`](rooms/room_146.pes) | `146` | `8` | `2` | `37,564` B | `137` | Found "ItemItem", |
| [`room_147.pes`](rooms/room_147.pes) | `147` | `4` | `1` | `4,884` B | `137` | Found "ItemItem", |
| [`room_148.pes`](rooms/room_148.pes) | `148` | `6` | `2` | `22,432` B | `137` | Found "ItemItem", |
| [`room_149.pes`](rooms/room_149.pes) | `149` | `4` | `1` | `5,148` B | `137` | Found "ItemItem", |
| [`room_150.pes`](rooms/room_150.pes) | `150` | `5` | `1` | `1,664` B | `137` | Found "ItemItem", |
| [`room_151.pes`](rooms/room_151.pes) | `151` | `8` | `2` | `39,704` B | `137` | Found "ItemItem", |
| [`room_152.pes`](rooms/room_152.pes) | `152` | `8` | `2` | `36,992` B | `137` | Found "ItemItem", |
| [`room_153.pes`](rooms/room_153.pes) | `153` | `11` | `2` | `48,880` B | `137` | Found "ItemItem", |
| [`room_154.pes`](rooms/room_154.pes) | `154` | `3` | `1` | `3,992` B | `137` | Found "ItemItem", |
| [`room_155.pes`](rooms/room_155.pes) | `155` | `8` | `2` | `37,452` B | `137` | Found "ItemItem", |
| [`room_156.pes`](rooms/room_156.pes) | `156` | `2` | `1` | `1,116` B | `137` | Found "ItemItem", |
| [`room_157.pes`](rooms/room_157.pes) | `157` | `5` | `1` | `5,596` B | `137` | Found "ItemItem", |
| [`room_158.pes`](rooms/room_158.pes) | `158` | `6` | `2` | `21,908` B | `137` | Found "ItemItem", |
| [`room_159.pes`](rooms/room_159.pes) | `159` | `5` | `1` | `4,616` B | `137` | Found "ItemItem", |
| [`room_160.pes`](rooms/room_160.pes) | `160` | `4` | `1` | `5,580` B | `29` | Found "ItemItem", |
| [`room_161.pes`](rooms/room_161.pes) | `161` | `8` | `2` | `48,596` B | `29` | Found "ItemItem", |
| [`room_162.pes`](rooms/room_162.pes) | `162` | `10` | `2` | `51,988` B | `29` | Found "ItemItem", |
| [`room_163.pes`](rooms/room_163.pes) | `163` | `4` | `1` | `4,068` B | `29` | Found "ItemItem", |
| [`room_164.pes`](rooms/room_164.pes) | `164` | `5` | `1` | `9,780` B | `29` | Found "ItemItem", |
| [`room_165.pes`](rooms/room_165.pes) | `165` | `4` | `1` | `4,164` B | `26` | Found "ItemItem", |
| [`room_166.pes`](rooms/room_166.pes) | `166` | `10` | `3` | `53,684` B | `26` | Found "ItemItem", |
| [`room_167.pes`](rooms/room_167.pes) | `167` | `11` | `3` | `53,436` B | `26` | Found "ItemItem", |
| [`room_168.pes`](rooms/room_168.pes) | `168` | `10` | `3` | `50,592` B | `26` | Found "ItemItem", |
| [`room_169.pes`](rooms/room_169.pes) | `169` | `4` | `2` | `3,396` B | `26` | Found "ItemItem", |
| [`room_170.pes`](rooms/room_170.pes) | `170` | `4` | `2` | `3,512` B | `26` | Found "ItemItem", |
| [`room_171.pes`](rooms/room_171.pes) | `171` | `7` | `3` | `6,036` B | `26` | Found "ItemItem", |
| [`room_172.pes`](rooms/room_172.pes) | `172` | `9` | `2` | `39,324` B | `26` | Found "ItemItem", |
| [`room_173.pes`](rooms/room_173.pes) | `173` | `8` | `2` | `41,472` B | `26` | Found "ItemItem", |
| [`room_174.pes`](rooms/room_174.pes) | `174` | `4` | `1` | `2,588` B | `26` | Found "ItemItem", |
| [`room_175.pes`](rooms/room_175.pes) | `175` | `6` | `1` | `11,048` B | `26` | Found "ItemItem", |
| [`room_177.pes`](rooms/room_177.pes) | `177` | `5` | `2` | `32,000` B | `26` | Found "ItemItem", |
| [`room_178.pes`](rooms/room_178.pes) | `178` | `3` | `1` | `2,232` B | `26` | Found "ItemItem", |
| [`room_179.pes`](rooms/room_179.pes) | `179` | `4` | `1` | `5,232` B | `26` | Found "ItemItem", |
| [`room_180.pes`](rooms/room_180.pes) | `180` | `4` | `1` | `3,776` B | `26` | Found "ItemItem", |
| [`room_181.pes`](rooms/room_181.pes) | `181` | `5` | `1` | `6,376` B | `26` | Found "ItemItem", |
| [`room_182.pes`](rooms/room_182.pes) | `182` | `4` | `1` | `11,148` B | `26` | Found "ItemItem", |
| [`room_183.pes`](rooms/room_183.pes) | `183` | `6` | `1` | `11,656` B | `26` | Found "ItemItem", |
| [`room_184.pes`](rooms/room_184.pes) | `184` | `8` | `1` | `7,692` B | `26` | Found "ItemItem", |
| [`room_185.pes`](rooms/room_185.pes) | `185` | `7` | `2` | `46,468` B | `26` | Found "ItemItem", |
| [`room_186.pes`](rooms/room_186.pes) | `186` | `6` | `2` | `31,696` B | `26` | Found "ItemItem", |
| [`room_187.pes`](rooms/room_187.pes) | `187` | `10` | `2` | `105,992` B | `26` | Found "ItemItem", |
| [`room_188.pes`](rooms/room_188.pes) | `188` | `4` | `1` | `3,500` B | `26` | Found "ItemItem", |
| [`room_189.pes`](rooms/room_189.pes) | `189` | `5` | `1` | `4,848` B | `26` | Found "ItemItem", |
| [`room_190.pes`](rooms/room_190.pes) | `190` | `10` | `1` | `30,556` B | `230` | Found "ItemItem", |
| [`room_191.pes`](rooms/room_191.pes) | `191` | `8` | `1` | `20,448` B | `200` | Found "ItemItem", |
| [`room_192.pes`](rooms/room_192.pes) | `192` | `5` | `1` | `6,464` B | `200` | Found "ItemItem", |
| [`room_193.pes`](rooms/room_193.pes) | `193` | `4` | `1` | `5,576` B | `200` | Found "ItemItem", |
| [`room_194.pes`](rooms/room_194.pes) | `194` | `9` | `1` | `15,356` B | `230` | Found "ItemItem", |
| [`room_195.pes`](rooms/room_195.pes) | `195` | `6` | `1` | `9,500` B | `200` | Found "ItemItem", |
| [`room_196.pes`](rooms/room_196.pes) | `196` | `9` | `2` | `66,504` B | `200` | Found "ItemItem", |
| [`room_197.pes`](rooms/room_197.pes) | `197` | `8` | `2` | `66,172` B | `200` | Found "ItemItem", |
| [`room_198.pes`](rooms/room_198.pes) | `198` | `10` | `2` | `70,180` B | `200` | Found "ItemItem", |
| [`room_199.pes`](rooms/room_199.pes) | `199` | `4` | `1` | `6,092` B | `200` | Found "ItemItem", |
| [`room_200.pes`](rooms/room_200.pes) | `200` | `5` | `2` | `36,296` B | `200` | Found "ItemItem", |
| [`room_201.pes`](rooms/room_201.pes) | `201` | `4` | `1` | `8,740` B | `200` | Found "ItemItem", |
| [`room_202.pes`](rooms/room_202.pes) | `202` | `8` | `2` | `72,104` B | `200` | Found "ItemItem", |
| [`room_203.pes`](rooms/room_203.pes) | `203` | `4` | `1` | `1,932` B | `200` | Found "ItemItem", |
| [`room_204.pes`](rooms/room_204.pes) | `204` | `6` | `2` | `30,964` B | `200` | Found "ItemItem", |
| [`room_205.pes`](rooms/room_205.pes) | `205` | `7` | `2` | `64,144` B | `200` | Found "ItemItem", |
| [`room_206.pes`](rooms/room_206.pes) | `206` | `8` | `2` | `64,604` B | `200` | Found "ItemItem", |
| [`room_207.pes`](rooms/room_207.pes) | `207` | `3` | `1` | `1,604` B | `230` | Found "ItemItem", |
| [`room_208.pes`](rooms/room_208.pes) | `208` | `3` | `1` | `4,356` B | `200` | Found "ItemItem", |
| [`room_209.pes`](rooms/room_209.pes) | `209` | `7` | `2` | `29,740` B | `200` | Found "ItemItem", |
| [`room_210.pes`](rooms/room_210.pes) | `210` | `4` | `1` | `4,712` B | `200` | Found "ItemItem", |
| [`room_211.pes`](rooms/room_211.pes) | `211` | `3` | `1` | `4,032` B | `200` | Found "ItemItem", |
| [`room_212.pes`](rooms/room_212.pes) | `212` | `4` | `1` | `4,712` B | `200` | Found "ItemItem", |
| [`room_213.pes`](rooms/room_213.pes) | `213` | `8` | `2` | `58,476` B | `200` | Found "ItemItem", |
| [`room_214.pes`](rooms/room_214.pes) | `214` | `3` | `1` | `1,092` B | `200` | Found "ItemItem", |
| [`room_215.pes`](rooms/room_215.pes) | `215` | `3` | `1` | `1,988` B | `200` | Found "ItemItem", |
| [`room_216.pes`](rooms/room_216.pes) | `216` | `8` | `2` | `66,480` B | `200` | Found "ItemItem", |
| [`room_217.pes`](rooms/room_217.pes) | `217` | `6` | `2` | `51,784` B | `200` | Found "ItemItem", |
| [`room_218.pes`](rooms/room_218.pes) | `218` | `6` | `2` | `53,976` B | `200` | Found "ItemItem", |
| [`room_219.pes`](rooms/room_219.pes) | `219` | `7` | `2` | `63,584` B | `200` | Found "ItemItem", |
| [`room_220.pes`](rooms/room_220.pes) | `220` | `7` | `2` | `28,844` B | `230` | Found "ItemItem", |
| [`room_221.pes`](rooms/room_221.pes) | `221` | `5` | `1` | `5,724` B | `200` | Found "ItemItem", |
| [`room_222.pes`](rooms/room_222.pes) | `222` | `7` | `2` | `60,932` B | `200` | Found "ItemItem", |
| [`room_223.pes`](rooms/room_223.pes) | `223` | `7` | `2` | `62,684` B | `200` | Found "ItemItem", |
| [`room_224.pes`](rooms/room_224.pes) | `224` | `3` | `1` | `4,216` B | `200` | Found "ItemItem", |
| [`room_225.pes`](rooms/room_225.pes) | `225` | `7` | `2` | `65,272` B | `200` | Found "ItemItem", |
| [`room_226.pes`](rooms/room_226.pes) | `226` | `8` | `2` | `51,608` B | `200` | Found "ItemItem", |
| [`room_227.pes`](rooms/room_227.pes) | `227` | `4` | `1` | `5,428` B | `200` | Found "ItemItem", |
| [`room_228.pes`](rooms/room_228.pes) | `228` | `7` | `2` | `64,640` B | `200` | Found "ItemItem", |
| [`room_229.pes`](rooms/room_229.pes) | `229` | `6` | `1` | `3,652` B | `230` | Found "ItemItem", |
| [`room_230.pes`](rooms/room_230.pes) | `230` | `7` | `2` | `66,616` B | `200` | Found "ItemItem", |
| [`room_231.pes`](rooms/room_231.pes) | `231` | `7` | `2` | `36,128` B | `200` | Found "ItemItem", |
| [`room_232.pes`](rooms/room_232.pes) | `232` | `6` | `2` | `48,604` B | `230` | Found "ItemItem", |
| [`room_233.pes`](rooms/room_233.pes) | `233` | `8` | `2` | `67,888` B | `200` | Found "ItemItem", |
| [`room_234.pes`](rooms/room_234.pes) | `234` | `3` | `1` | `2,804` B | `200` | Found "ItemItem", |
| [`room_235.pes`](rooms/room_235.pes) | `235` | `3` | `1` | `4,624` B | `200` | Found "ItemItem", |
| [`room_236.pes`](rooms/room_236.pes) | `236` | `6` | `1` | `9,952` B | `200` | Found "ItemItem", |
| [`room_237.pes`](rooms/room_237.pes) | `237` | `4` | `1` | `6,608` B | `200` | Found "ItemItem", |
| [`room_238.pes`](rooms/room_238.pes) | `238` | `6` | `1` | `55,940` B | `230` | Found "ItemItem", |
| [`room_239.pes`](rooms/room_239.pes) | `239` | `3` | `1` | `2,180` B | `200` | Found "ItemItem", |
| [`room_240.pes`](rooms/room_240.pes) | `240` | `4` | `1` | `1,576` B | `200` | Found "ItemItem", |
| [`room_241.pes`](rooms/room_241.pes) | `241` | `8` | `2` | `71,824` B | `200` | Found "ItemItem", |
| [`room_242.pes`](rooms/room_242.pes) | `242` | `5` | `2` | `36,192` B | `200` | Found "ItemItem", |
| [`room_243.pes`](rooms/room_243.pes) | `243` | `4` | `1` | `1,916` B | `200` | Found "ItemItem", |
| [`room_244.pes`](rooms/room_244.pes) | `244` | `10` | `2` | `49,224` B | `200` | Found "ItemItem", |
| [`room_245.pes`](rooms/room_245.pes) | `245` | `9` | `2` | `68,072` B | `200` | Found "ItemItem", |
| [`room_246.pes`](rooms/room_246.pes) | `246` | `4` | `1` | `5,164` B | `200` | Found "ItemItem", |
| [`room_247.pes`](rooms/room_247.pes) | `247` | `3` | `1` | `1,628` B | `200` | Found "ItemItem", |
| [`room_248.pes`](rooms/room_248.pes) | `248` | `4` | `1` | `4,908` B | `200` | Found "ItemItem", |
| [`room_249.pes`](rooms/room_249.pes) | `249` | `7` | `1` | `8,512` B | `200` | Found "ItemItem", |
| [`room_250.pes`](rooms/room_250.pes) | `250` | `6` | `1` | `6,412` B | `200` | Found "ItemItem", |
| [`room_251.pes`](rooms/room_251.pes) | `251` | `3` | `1` | `2,932` B | `200` | Found "ItemItem", |
| [`room_252.pes`](rooms/room_252.pes) | `252` | `4` | `1` | `4,024` B | `200` | Found "ItemItem", |
| [`room_253.pes`](rooms/room_253.pes) | `253` | `6` | `1` | `6,056` B | `200` | Found "ItemItem", |
| [`room_254.pes`](rooms/room_254.pes) | `254` | `6` | `1` | `9,036` B | `200` | Found "ItemItem", |
| [`room_255.pes`](rooms/room_255.pes) | `255` | `4` | `2` | `26,200` B | `230` | Found "ItemItem", |
| [`room_256.pes`](rooms/room_256.pes) | `256` | `4` | `1` | `12,604` B | `200` | Found "ItemItem", |
| [`room_257.pes`](rooms/room_257.pes) | `257` | `4` | `1` | `6,876` B | `200` | Found "ItemItem", |
| [`room_258.pes`](rooms/room_258.pes) | `258` | `5` | `1` | `4,008` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_259.pes`](rooms/room_259.pes) | `259` | `10` | `1` | `15,792` B | `15` | Found "ItemItem", |
| [`room_261.pes`](rooms/room_261.pes) | `261` | `4` | `1` | `3,200` B | `15` | Found "ItemItem", |
| [`room_262.pes`](rooms/room_262.pes) | `262` | `11` | `2` | `62,860` B | `15` | Found "ItemItem", |
| [`room_264.pes`](rooms/room_264.pes) | `264` | `7` | `2` | `56,928` B | `15` | Found "ItemItem", |
| [`room_265.pes`](rooms/room_265.pes) | `265` | `10` | `1` | `12,680` B | `15` | Found "ItemItem", |
| [`room_266.pes`](rooms/room_266.pes) | `266` | `6` | `1` | `8,512` B | `15` | Found "ItemItem", |
| [`room_267.pes`](rooms/room_267.pes) | `267` | `1` | `1` | `1,976` B | `15` | Found "ItemItem", |
| [`room_268.pes`](rooms/room_268.pes) | `268` | `11` | `2` | `70,752` B | `15` | Found "ItemItem", |
| [`room_270.pes`](rooms/room_270.pes) | `270` | `8` | `1` | `15,648` B | `80` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_271.pes`](rooms/room_271.pes) | `271` | `5` | `1` | `7,716` B | `80` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_272.pes`](rooms/room_272.pes) | `272` | `8` | `2` | `26,544` B | `32` | Eve. So it WAS you,,, |
| [`room_274.pes`](rooms/room_274.pes) | `274` | `7` | `1` | `6,032` B | `86` | Found "ItemItem", |
| [`room_275.pes`](rooms/room_275.pes) | `275` | `9` | `1` | `20,000` B | `86` | Found "ItemItem", |
| [`room_277.pes`](rooms/room_277.pes) | `277` | `8` | `1` | `5,620` B | `86` | Found "ItemItem", |
| [`room_278.pes`](rooms/room_278.pes) | `278` | `5` | `1` | `2,600` B | `86` | Found "ItemItem", |
| [`room_279.pes`](rooms/room_279.pes) | `279` | `5` | `1` | `3,452` B | `86` | Found "ItemItem", |
| [`room_280.pes`](rooms/room_280.pes) | `280` | `8` | `1` | `4,544` B | `86` | Found "ItemItem", |
| [`room_281.pes`](rooms/room_281.pes) | `281` | `7` | `1` | `4,680` B | `86` | Found "ItemItem", |
| [`room_282.pes`](rooms/room_282.pes) | `282` | `9` | `1` | `16,076` B | `86` | Found "ItemItem", |
| [`room_283.pes`](rooms/room_283.pes) | `283` | `6` | `1` | `5,268` B | `86` | Found "ItemItem", |
| [`room_284.pes`](rooms/room_284.pes) | `284` | `6` | `1` | `5,596` B | `86` | Found "ItemItem", |
| [`room_285.pes`](rooms/room_285.pes) | `285` | `7` | `1` | `5,292` B | `86` | Found "ItemItem", |
| [`room_286.pes`](rooms/room_286.pes) | `286` | `5` | `1` | `4,936` B | `86` | Found "ItemItem", |
| [`room_287.pes`](rooms/room_287.pes) | `287` | `4` | `1` | `6,884` B | `20` | Daniel. It's finally over,,, |
| [`room_289.pes`](rooms/room_289.pes) | `289` | `4` | `1` | `7,720` B | `139` | Found "ItemItem", |
| [`room_290.pes`](rooms/room_290.pes) | `290` | `8` | `2` | `32,656` B | `139` | Found "ItemItem", |
| [`room_291.pes`](rooms/room_291.pes) | `291` | `3` | `1` | `6,416` B | `139` | Found "ItemItem", |
| [`room_292.pes`](rooms/room_292.pes) | `292` | `3` | `1` | `4,100` B | `139` | Found "ItemItem", |
| [`room_293.pes`](rooms/room_293.pes) | `293` | `3` | `1` | `18,924` B | `139` | Found "ItemItem", |
| [`room_294.pes`](rooms/room_294.pes) | `294` | `7` | `2` | `171,036` B | `139` | Found "ItemItem", |
| [`room_295.pes`](rooms/room_295.pes) | `295` | `3` | `1` | `3,736` B | `139` | Found "ItemItem", |
| [`room_296.pes`](rooms/room_296.pes) | `296` | `3` | `1` | `4,560` B | `139` | Found "ItemItem", |
| [`room_297.pes`](rooms/room_297.pes) | `297` | `6` | `2` | `37,476` B | `139` | Found "ItemItem", |
| [`room_298.pes`](rooms/room_298.pes) | `298` | `6` | `2` | `47,068` B | `139` | Found "ItemItem", |
| [`room_299.pes`](rooms/room_299.pes) | `299` | `3` | `1` | `1,524` B | `139` | Found "ItemItem", |
| [`room_300.pes`](rooms/room_300.pes) | `300` | `3` | `1` | `1,364` B | `139` | Found "ItemItem", |
| [`room_301.pes`](rooms/room_301.pes) | `301` | `3` | `1` | `1,524` B | `139` | Found "ItemItem", |
| [`room_302.pes`](rooms/room_302.pes) | `302` | `3` | `1` | `1,168` B | `139` | Found "ItemItem", |
| [`room_303.pes`](rooms/room_303.pes) | `303` | `3` | `1` | `2,436` B | `139` | Found "ItemItem", |
| [`room_304.pes`](rooms/room_304.pes) | `304` | `3` | `1` | `2,276` B | `139` | Found "ItemItem", |
| [`room_305.pes`](rooms/room_305.pes) | `305` | `3` | `1` | `2,116` B | `139` | Found "ItemItem", |
| [`room_306.pes`](rooms/room_306.pes) | `306` | `3` | `1` | `2,116` B | `139` | Found "ItemItem", |
| [`room_307.pes`](rooms/room_307.pes) | `307` | `6` | `2` | `39,044` B | `139` | Found "ItemItem", |
| [`room_308.pes`](rooms/room_308.pes) | `308` | `6` | `2` | `46,860` B | `139` | Found "ItemItem", |
| [`room_309.pes`](rooms/room_309.pes) | `309` | `6` | `2` | `46,820` B | `139` | Found "ItemItem", |
| [`room_310.pes`](rooms/room_310.pes) | `310` | `6` | `2` | `38,908` B | `139` | Found "ItemItem", |
| [`room_311.pes`](rooms/room_311.pes) | `311` | `3` | `1` | `2,828` B | `139` | Found "ItemItem", |
| [`room_312.pes`](rooms/room_312.pes) | `312` | `3` | `1` | `4,452` B | `139` | Found "ItemItem", |
| [`room_313.pes`](rooms/room_313.pes) | `313` | `3` | `1` | `3,644` B | `139` | Found "ItemItem", |
| [`room_314.pes`](rooms/room_314.pes) | `314` | `3` | `1` | `2,684` B | `139` | Found "ItemItem", |
| [`room_315.pes`](rooms/room_315.pes) | `315` | `3` | `1` | `1,528` B | `139` | Found "ItemItem", |
| [`room_316.pes`](rooms/room_316.pes) | `316` | `3` | `1` | `1,528` B | `139` | Found "ItemItem", |
| [`room_317.pes`](rooms/room_317.pes) | `317` | `6` | `2` | `58,176` B | `139` | Found "ItemItem", |
| [`room_318.pes`](rooms/room_318.pes) | `318` | `8` | `1` | `5,688` B | `204` | Found "ItemItem", |
| [`room_319.pes`](rooms/room_319.pes) | `319` | `4` | `1` | `5,764` B | `137` | Found "ItemItem", |
| [`room_320.pes`](rooms/room_320.pes) | `320` | `4` | `1` | `8,384` B | `137` | Found "ItemItem", |
| [`room_321.pes`](rooms/room_321.pes) | `321` | `4` | `1` | `2,728` B | `137` | Found "ItemItem", |
| [`room_322.pes`](rooms/room_322.pes) | `322` | `5` | `1` | `5,460` B | `137` | Found "ItemItem", |
| [`room_323.pes`](rooms/room_323.pes) | `323` | `7` | `2` | `31,564` B | `137` | Found "ItemItem", |
| [`room_324.pes`](rooms/room_324.pes) | `324` | `6` | `1` | `9,044` B | `137` | Found "ItemItem", |
| [`room_325.pes`](rooms/room_325.pes) | `325` | `8` | `2` | `26,272` B | `137` | Found "ItemItem", |
| [`room_326.pes`](rooms/room_326.pes) | `326` | `8` | `2` | `35,112` B | `137` | Found "ItemItem", |
| [`room_327.pes`](rooms/room_327.pes) | `327` | `9` | `2` | `33,756` B | `137` | Found "ItemItem", |
| [`room_328.pes`](rooms/room_328.pes) | `328` | `5` | `1` | `7,136` B | `137` | Found "ItemItem", |
| [`room_329.pes`](rooms/room_329.pes) | `329` | `4` | `1` | `3,160` B | `137` | Found "ItemItem", |
| [`room_330.pes`](rooms/room_330.pes) | `330` | `4` | `1` | `7,252` B | `137` | Found "ItemItem", |
| [`room_331.pes`](rooms/room_331.pes) | `331` | `9` | `2` | `30,152` B | `137` | Found "ItemItem", |
| [`room_332.pes`](rooms/room_332.pes) | `332` | `8` | `2` | `57,748` B | `15` | Found "ItemItem", |
| [`room_333.pes`](rooms/room_333.pes) | `333` | `9` | `1` | `10,188` B | `25` | Found "ItemItem", |
| [`room_334.pes`](rooms/room_334.pes) | `334` | `3` | `1` | `2,480` B | `139` | Found "ItemItem", |
| [`room_336.pes`](rooms/room_336.pes) | `336` | `3` | `1` | `896` B | `205` | / Found "ItemItem", |
| [`room_338.pes`](rooms/room_338.pes) | `338` | `3` | `1` | `6,004` B | `137` | Found "ItemItem", |
| [`room_339.pes`](rooms/room_339.pes) | `339` | `4` | `1` | `8,584` B | `200` | Found "ItemItem", |
| [`room_340.pes`](rooms/room_340.pes) | `340` | `3` | `1` | `7,336` B | `200` | Found "ItemItem", |
| [`room_341.pes`](rooms/room_341.pes) | `341` | `3` | `1` | `1,632` B | `200` | Found "ItemItem", |
| [`room_342.pes`](rooms/room_342.pes) | `342` | `3` | `1` | `5,796` B | `200` | Found "ItemItem", |
| [`room_343.pes`](rooms/room_343.pes) | `343` | `6` | `1` | `26,236` B | `200` | Found "ItemItem", |
| [`room_344.pes`](rooms/room_344.pes) | `344` | `4` | `1` | `4,928` B | `200` | Found "ItemItem", |
| [`room_345.pes`](rooms/room_345.pes) | `345` | `3` | `1` | `1,140` B | `200` | Found "ItemItem", |
| [`room_346.pes`](rooms/room_346.pes) | `346` | `2` | `2` | `500` B | `40` | ENEMY DAMAGE FRONT |
| [`room_347.pes`](rooms/room_347.pes) | `347` | `4` | `2` | `17,584` B | `25` | Found "ItemItem", |
| [`room_348.pes`](rooms/room_348.pes) | `348` | `4` | `2` | `14,012` B | `230` | Found "ItemItem", |
| [`room_349.pes`](rooms/room_349.pes) | `349` | `5` | `2` | `31,748` B | `86` | Found "ItemItem", |
| [`room_350.pes`](rooms/room_350.pes) | `350` | `2` | `1` | `7,420` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_351.pes`](rooms/room_351.pes) | `351` | `2` | `1` | `10,304` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_352.pes`](rooms/room_352.pes) | `352` | `3` | `1` | `8,524` B | `0` | — |
| [`room_353.pes`](rooms/room_353.pes) | `353` | `3` | `1` | `6,660` B | `0` | — |
| [`room_354.pes`](rooms/room_354.pes) | `354` | `3` | `1` | `7,652` B | `0` | — |
| [`room_357.pes`](rooms/room_357.pes) | `357` | `8` | `2` | `42,688` B | `110` | Found "ItemItem", |
| [`room_358.pes`](rooms/room_358.pes) | `358` | `5` | `1` | `5,056` B | `204` | Found "ItemItem", |
| [`room_359.pes`](rooms/room_359.pes) | `359` | `8` | `1` | `19,576` B | `184` | Will you watch this event? / <FB:09>2 Watch / Skip |
| [`room_360.pes`](rooms/room_360.pes) | `360` | `3` | `1` | `5,044` B | `0` | — |
| [`room_361.pes`](rooms/room_361.pes) | `361` | `3` | `1` | `4,184` B | `0` | — |
| [`room_362.pes`](rooms/room_362.pes) | `362` | `3` | `1` | `6,660` B | `0` | — |
| [`room_364.pes`](rooms/room_364.pes) | `364` | `2` | `1` | `476` B | `0` | — |
| [`room_365.pes`](rooms/room_365.pes) | `365` | `3` | `1` | `3,572` B | `0` | — |
| [`room_366.pes`](rooms/room_366.pes) | `366` | `5` | `1` | `5,060` B | `204` | Found "ItemItem", |
| [`room_370.pes`](rooms/room_370.pes) | `370` | `6` | `1` | `7,120` B | `200` | Found "ItemItem", |
| [`room_371.pes`](rooms/room_371.pes) | `371` | `6` | `1` | `4,660` B | `204` | Found "ItemItem", |
| [`room_372.pes`](rooms/room_372.pes) | `372` | `5` | `1` | `9,340` B | `204` | Found "ItemItem", |
| [`room_373.pes`](rooms/room_373.pes) | `373` | `6` | `1` | `12,824` B | `110` | Found "ItemItem", |
| [`room_376.pes`](rooms/room_376.pes) | `376` | `6` | `1` | `1,960` B | `204` | Found "ItemItem", |
| [`room_377.pes`](rooms/room_377.pes) | `377` | `5` | `1` | `2,124` B | `204` | Found "ItemItem", |
| [`room_378.pes`](rooms/room_378.pes) | `378` | `5` | `1` | `10,768` B | `200` | Found "ItemItem", |
| [`room_379.pes`](rooms/room_379.pes) | `379` | `8` | `2` | `36,472` B | `137` | Found "ItemItem", |
| [`room_380.pes`](rooms/room_380.pes) | `380` | `7` | `1` | `8,000` B | `137` | Found "ItemItem", |
| [`room_381.pes`](rooms/room_381.pes) | `381` | `2` | `1` | `36` B | `40` | ENEMY DAMAGE FRONT |
| [`room_382.pes`](rooms/room_382.pes) | `382` | `5` | `2` | `34,024` B | `139` | Found "ItemItem", |
| [`room_383.pes`](rooms/room_383.pes) | `383` | `7` | `2` | `33,788` B | `139` | Found "ItemItem", |
| [`room_384.pes`](rooms/room_384.pes) | `384` | `6` | `2` | `45,192` B | `139` | Found "ItemItem", |
| [`room_385.pes`](rooms/room_385.pes) | `385` | `5` | `2` | `25,452` B | `139` | Found "ItemItem", |
| [`room_386.pes`](rooms/room_386.pes) | `386` | `7` | `2` | `35,468` B | `139` | Found "ItemItem", |
| [`room_387.pes`](rooms/room_387.pes) | `387` | `6` | `2` | `44,856` B | `139` | Found "ItemItem", |
| [`room_388.pes`](rooms/room_388.pes) | `388` | `6` | `2` | `44,800` B | `139` | Found "ItemItem", |
| [`room_389.pes`](rooms/room_389.pes) | `389` | `8` | `2` | `100,456` B | `139` | Found "ItemItem", |
| [`room_390.pes`](rooms/room_390.pes) | `390` | `8` | `2` | `49,076` B | `139` | Found "ItemItem", |
| [`room_391.pes`](rooms/room_391.pes) | `391` | `8` | `2` | `48,180` B | `139` | Found "ItemItem", |
| [`room_392.pes`](rooms/room_392.pes) | `392` | `7` | `2` | `46,012` B | `139` | Found "ItemItem", |
| [`room_393.pes`](rooms/room_393.pes) | `393` | `8` | `2` | `50,780` B | `139` | Found "ItemItem", |
| [`room_394.pes`](rooms/room_394.pes) | `394` | `8` | `2` | `50,836` B | `139` | Found "ItemItem", |
| [`room_395.pes`](rooms/room_395.pes) | `395` | `7` | `2` | `45,988` B | `139` | Found "ItemItem", |
| [`room_396.pes`](rooms/room_396.pes) | `396` | `7` | `2` | `49,500` B | `139` | Found "ItemItem", |
| [`room_397.pes`](rooms/room_397.pes) | `397` | `6` | `2` | `45,212` B | `139` | Found "ItemItem", |
| [`room_398.pes`](rooms/room_398.pes) | `398` | `7` | `2` | `35,396` B | `139` | Found "ItemItem", |
| [`room_399.pes`](rooms/room_399.pes) | `399` | `6` | `2` | `45,308` B | `139` | Found "ItemItem", |
| [`room_400.pes`](rooms/room_400.pes) | `400` | `8` | `2` | `47,116` B | `139` | Found "ItemItem", |
| [`room_401.pes`](rooms/room_401.pes) | `401` | `7` | `2` | `37,140` B | `139` | Found "ItemItem", |
| [`room_402.pes`](rooms/room_402.pes) | `402` | `6` | `2` | `45,164` B | `139` | Found "ItemItem", |
| [`room_403.pes`](rooms/room_403.pes) | `403` | `6` | `2` | `42,496` B | `139` | Found "ItemItem", |
| [`room_404.pes`](rooms/room_404.pes) | `404` | `7` | `2` | `61,288` B | `139` | Found "ItemItem", |
| [`room_405.pes`](rooms/room_405.pes) | `405` | `6` | `2` | `47,924` B | `139` | Found "ItemItem", |
| [`room_406.pes`](rooms/room_406.pes) | `406` | `6` | `2` | `46,736` B | `139` | Found "ItemItem", |
| [`room_407.pes`](rooms/room_407.pes) | `407` | `7` | `2` | `61,000` B | `139` | Found "ItemItem", |
| [`room_408.pes`](rooms/room_408.pes) | `408` | `7` | `2` | `61,152` B | `139` | Found "ItemItem", |
| [`room_409.pes`](rooms/room_409.pes) | `409` | `6` | `2` | `47,024` B | `139` | Found "ItemItem", |
| [`room_410.pes`](rooms/room_410.pes) | `410` | `11` | `2` | `64,904` B | `139` | Found "ItemItem", |
| [`room_411.pes`](rooms/room_411.pes) | `411` | `7` | `2` | `42,264` B | `139` | Found "ItemItem", |
| [`room_412.pes`](rooms/room_412.pes) | `412` | `6` | `2` | `47,072` B | `139` | Found "ItemItem", |
| [`room_413.pes`](rooms/room_413.pes) | `413` | `6` | `2` | `32,956` B | `139` | Found "ItemItem", |
| [`room_414.pes`](rooms/room_414.pes) | `414` | `6` | `2` | `32,956` B | `139` | Found "ItemItem", |
| [`room_415.pes`](rooms/room_415.pes) | `415` | `6` | `2` | `46,912` B | `139` | Found "ItemItem", |
| [`room_416.pes`](rooms/room_416.pes) | `416` | `6` | `2` | `46,680` B | `139` | Found "ItemItem", |
| [`room_417.pes`](rooms/room_417.pes) | `417` | `7` | `2` | `55,572` B | `139` | Found "ItemItem", |
| [`room_418.pes`](rooms/room_418.pes) | `418` | `8` | `2` | `41,948` B | `139` | Found "ItemItem", |
| [`room_419.pes`](rooms/room_419.pes) | `419` | `8` | `2` | `41,996` B | `139` | Found "ItemItem", |
| [`room_420.pes`](rooms/room_420.pes) | `420` | `7` | `2` | `39,716` B | `139` | Found "ItemItem", |
| [`room_421.pes`](rooms/room_421.pes) | `421` | `7` | `2` | `39,772` B | `139` | Found "ItemItem", |
| [`room_422.pes`](rooms/room_422.pes) | `422` | `7` | `2` | `39,660` B | `139` | Found "ItemItem", |
| [`room_423.pes`](rooms/room_423.pes) | `423` | `6` | `2` | `31,952` B | `139` | Found "ItemItem", |
| [`room_424.pes`](rooms/room_424.pes) | `424` | `5` | `1` | `3,300` B | `86` | Found "ItemItem", |
| [`room_425.pes`](rooms/room_425.pes) | `425` | `8` | `1` | `8,512` B | `86` | Found "ItemItem", |
| [`room_426.pes`](rooms/room_426.pes) | `426` | `5` | `1` | `9,564` B | `86` | Found "ItemItem", |
| [`room_427.pes`](rooms/room_427.pes) | `427` | `4` | `2` | `12,776` B | `86` | Found "ItemItem", |
| [`room_428.pes`](rooms/room_428.pes) | `428` | `5` | `2` | `38,072` B | `86` | Found "ItemItem", |
| [`room_429.pes`](rooms/room_429.pes) | `429` | `5` | `2` | `25,940` B | `86` | Found "ItemItem", |
| [`room_430.pes`](rooms/room_430.pes) | `430` | `2` | `1` | `11,672` B | `204` | Found "ItemItem", |
| [`room_431.pes`](rooms/room_431.pes) | `431` | `4` | `2` | `20,568` B | `32` | Eve. So it WAS you,,, |
| [`room_432.pes`](rooms/room_432.pes) | `432` | `3` | `1` | `2,884` B | `26` | Found "ItemItem", |
| [`room_433.pes`](rooms/room_433.pes) | `433` | `7` | `2` | `170,900` B | `139` | Found "ItemItem", |
| [`room_434.pes`](rooms/room_434.pes) | `434` | `7` | `2` | `171,000` B | `139` | Found "ItemItem", |
| [`room_435.pes`](rooms/room_435.pes) | `435` | `7` | `2` | `172,372` B | `139` | Found "ItemItem", |
| [`room_436.pes`](rooms/room_436.pes) | `436` | `7` | `2` | `170,272` B | `139` | Found "ItemItem", |
| [`room_437.pes`](rooms/room_437.pes) | `437` | `2` | `1` | `372` B | `0` | — |
