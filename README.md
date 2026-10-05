# Gen4 Event Distributor v0.9 — experimental PGT support

Adds the supplied `Manaphy Egg [PPorg].pgt` (0x104 / 260 bytes).

A Generation IV PGT is the inner gift data, not a complete Wonder Card. PKHeX
defines PGT as 0x104 bytes and PCD as the complete card whose first 0x104 bytes
are the PGT. Project Pokémon also documents that missing Gen IV PCDs are
sometimes reconstructed from PGTs using placeholder card metadata.

For this experimental build the supplied Manaphy PGT is wrapped into a 0x358
PCD using known-working metadata layout, with:
- gift payload: the supplied PGT, unchanged;
- visible title: `Manaphy Egg [PGT]`;
- synthetic local Card ID: `0xF001`;
- checksum for broadcast wrapper: `0x09CF`.

This does NOT claim to reconstruct the original historical Wonder Card metadata.
It is a functional placeholder card intended to test whether the retail game
accepts and delivers the PGT over the already proven wireless path.

The top-screen event list and bottom/touch personal log from v0.8.1 are retained.
`By SooraMaru` is retained.

Build: `make clean && make`

Credits/licenses from prior builds remain intact. No Nintendo distribution-ROM
executable code is included.

## Nintendo DS menu identity

The ROM now uses `icon.bmp` as its Nintendo DS menu/banner icon.

Banner text:
- Gen4 Event Distributor
- Pokemon Gen IV Events
- SooraMaru

The source image supplied by SooraMaru is converted to the DS banner requirement:
32x32 pixels, indexed 16-color BMP.

## v0.9.2 icon fix
The DS banner icon is now emitted as a true 32x32 **4-bpp indexed BMP with exactly
16 palette entries**, rather than Pillow's default 8-bpp indexed BMP. This matches
the Nintendo DS banner icon format expected by the devkitPro/ndstool conversion
path and avoids the striped/corrupted area seen in melonDS ROM Info.

## v0.9.3 — English + Spanish discovery

The ARM7 broadcaster now alternates the Gen IV Mystery Gift GGID after each
complete 10-fragment Wonder Card cycle:

- English: `0x00400318`
- Spanish: `0x008000D0`

A whole card cycle remains on the same GGID before switching, so a receiving
game never has to assemble one card from mixed-language discovery cycles.

The Wonder Card/PGT payload itself is unchanged. The proven raw `mwlDevTx()`
path, channel 7, checksums, encryption and fragment generation are unchanged.

This build intentionally targets EN + ES first so it can be verified on the
user's English and Spanish retail copies before adding the remaining language
GGIDs.

## v0.9.4 — EN/ES dwell-time fix

Hardware testing of v0.9.3 showed the Spanish retail game could discover the
distribution while the English game no longer did. v0.9.3 changed language
after every single 10-fragment card cycle, which is too aggressive for a game
that is scanning and locking onto a distribution beacon.

v0.9.4 keeps each GGID stable for 20 complete card cycles before changing:

    EN: 20 complete cards
    ES: 20 complete cards
    repeat

Only the discovery dwell timing changed. The raw MWL transmit path, channel,
Wonder Card data, checksum, RC4 and fragment contents are untouched.

## v0.9.5 — Manual language selector

Hardware testing showed that rotating EN/ES discovery GGIDs during one
distribution session is unreliable. This version therefore keeps one GGID
fixed for the entire session.

Idle-menu controls:
- UP/DOWN: choose event
- X: toggle ENGLISH / ESPANOL
- A: start distribution using the selected language
- B: stop distribution
- START: exit

English uses `0x00400318`; Spanish uses `0x008000D0`.
The selected language is sent to ARM7 in bit 16 of the START command.
The raw `mwlDevTx()` path and Wonder Card payload preparation are unchanged.


## v0.9.6 — Hardware-tested compatibility labels

Catalog rebuilt from `data(1).zip`. Removed Shiny Eevee (RAM overflow/crash), Korean #0063, and #0183. Added #0058, #0152, and #0004 Shaymin. Compatibility labels reflect the user's real-hardware tests. Mew is marked HG/SS and carries an explicit warning that D/P/Pt crashed during testing. Manaphy PGT wrapper is marked all Gen IV games / all languages. Jirachi is marked D/P / English only.

The application icon is the user-provided `icon.bmp`. DS banner subtitle remains `Gen IV Events BETA`.

## v0.9.6-CAT isolation build

This build intentionally uses the real-hardware-stable v0.9.6 ARM7 transmitter source unchanged.
Only the catalog data and ARM9 menu were expanded to the user-tested `eventos.zip` organization:
English/Spanish -> Diamond/Pearl or HeartGold/SoulSilver -> event.

Purpose: isolate whether the freeze is caused by the larger static ARM7 event table or by later transmitter changes.
The raw `mwlDevTx()` beacon loop, 10.24 ms sleep, radio start/stop path, GGIDs, and TX callback behavior are exactly from the supplied stable v0.9.6 baseline.

## Fragment-order fix

The catalog generator had placed the special unencrypted Wonder Card header
fragment at index 0. The known-good v0.9.6 tables place it at index 9, after
the nine encrypted xPCD fragments.

Bad catalog order:
`SPECIAL, ENC0, ENC1, ... ENC8`

Known-good / corrected order:
`ENC0, ENC1, ... ENC8, SPECIAL`

All 54 generated event tables were corrected. ARM7 radio/TX code was not
changed.

## Consecutive-gift session fix

Fixed a race when stopping one distribution and starting another.

Previously `CMD_START` unconditionally set `s_radioStarted=false`. If START
arrived before the beacon thread had processed STOP, the physical MWL radio
could still be running while the software flag said it was stopped. The
beacon thread could then call `mwlDevStart()` again on an already-started
device.

This build keeps the MWL radio session alive between gifts. B stops beacon
transmission only; selecting another gift and pressing A resets the
fragment/sequence counters and resumes using the same radio session. No
beacons are sent while idle.

The corrected fragment order from the previous FIX build is retained.

## PRE-ALPHA community build

- Mandatory 3-second quiet period after stopping one gift before another can
  start broadcasting. A is ignored until the cooldown reaches zero.
- Checksums are no longer shown in the public UI. The checksum required by the
  Gen IV wireless protocol is intentionally still present internally.
- Bottom screen is simplified for community testers: `Enviando regalo....`
  and active channel during broadcast.
- Hold/press L+R together to toggle the detailed debug/status screen.
- The corrected fragment order and same-session MWL behavior are retained.

## v0.10.1 PRE-ALPHA — eventos actualizados

Catálogo reemplazado por el contenido de `eventos(1).zip`.
Solo se integran archivos `.pcd` de 856 bytes y la carpeta determina idioma/juego.

- ENG / DP: 11
- ENG / HG-SS: 13
- ESP / DP: 10
- ESP / HG-SS: 13
- Total: 47

Se conserva el orden de fragmentos probado `ENC0..ENC8 -> SPECIAL`, la pausa
de 3 segundos entre regalos, el debug L+R y la interfaz simplificada.
