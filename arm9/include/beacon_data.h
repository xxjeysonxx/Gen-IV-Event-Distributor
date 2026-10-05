#pragma once
#include <nds/ndstypes.h>
#define GEN4_FRAG_COUNT 10
#define GEN4_FRAG_SIZE 0x68
#define GEN4_EVENT_COUNT 54
typedef struct { u16 checksum; u32 ggid; const u8 (*fragments)[GEN4_FRAG_SIZE]; } Gen4BeaconEvent;
extern const u8 g_gen4_mac[6];
extern const Gen4BeaconEvent g_gen4_events[GEN4_EVENT_COUNT];
