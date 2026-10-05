#pragma once
#include <stdint.h>
#include <stdbool.h>
#define PCD_SIZE  0x358
#define XPCD_SIZE 0x3A8
#define FRAG_SIZE 0x68
#define FRAG_COUNT 10
#define GGID_EN 0x00400318u
#define GGID_ES 0x008000D0u

typedef struct {
    uint16_t checksum;
    uint8_t xpcd[XPCD_SIZE];
    uint8_t encrypted[XPCD_SIZE];
    uint8_t fragments[FRAG_COUNT][FRAG_SIZE];
    int16_t fragment_index[FRAG_COUNT];
} Gen4Distribution;

uint16_t gen4_checksum(const uint8_t *data, unsigned size);
void gen4_make_key(uint8_t key[8], const uint8_t mac[6], uint16_t checksum);
void gen4_rc4(uint8_t *data, unsigned size, const uint8_t *key, unsigned key_size);
bool gen4_prepare(Gen4Distribution *out, const uint8_t pcd[PCD_SIZE], const uint8_t mac[6]);
