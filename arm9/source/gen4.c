#include "gen4.h"
#include <string.h>

uint16_t gen4_checksum(const uint8_t *data, unsigned size) {
    uint16_t c = 0;
    for (unsigned i=0; i+1<size; i+=2) {
        uint16_t w = (uint16_t)data[i] | ((uint16_t)data[i+1] << 8);
        c = (uint16_t)(c + w);
        c = (uint16_t)((c << 1) | (c >> 15));
    }
    return c;
}

void gen4_make_key(uint8_t key[8], const uint8_t mac[6], uint16_t c) {
    key[0]=mac[0]; key[1]=mac[1]; key[2]=(uint8_t)c; key[3]=(uint8_t)(c>>8);
    key[4]=mac[4]; key[5]=mac[5]; key[6]=mac[2]; key[7]=mac[3];
    uint16_t hw=0x3FA2;
    for (int i=0;i<4;i++) {
        uint16_t v=(uint16_t)key[i*2] | ((uint16_t)key[i*2+1]<<8);
        v ^= hw; hw=v;
        key[i*2]=(uint8_t)v; key[i*2+1]=(uint8_t)(v>>8);
    }
}

void gen4_rc4(uint8_t *data, unsigned size, const uint8_t *key, unsigned key_size) {
    uint8_t s[256];
    for (unsigned i=0;i<256;i++) s[i]=(uint8_t)i;
    unsigned j=0;
    for (unsigned i=0;i<256;i++) { j=(j+s[i]+key[i%key_size])&255; uint8_t t=s[i];s[i]=s[j];s[j]=t; }
    unsigned i=0; j=0;
    for (unsigned n=0;n<size;n++) { i=(i+1)&255; j=(j+s[i])&255; uint8_t t=s[i];s[i]=s[j];s[j]=t; data[n]^=s[(s[i]+s[j])&255]; }
}

bool gen4_prepare(Gen4Distribution *o, const uint8_t pcd[PCD_SIZE], const uint8_t mac[6]) {
    if (!o || !pcd || !mac) return false;
    memset(o,0,sizeof(*o));
    /* xPCD = PCD header 0x104..0x153 prepended to the complete PCD. */
    memcpy(o->xpcd, pcd+0x104, 0x50);
    memcpy(o->xpcd+0x50, pcd, PCD_SIZE);
    o->checksum=gen4_checksum(o->xpcd,XPCD_SIZE);
    memcpy(o->encrypted,o->xpcd,XPCD_SIZE);
    uint8_t key[8]; gen4_make_key(key,mac,o->checksum);
    gen4_rc4(o->encrypted,XPCD_SIZE,key,sizeof(key));
    /* Slot 0 is protocol index -1: unencrypted 0x50-byte header + zero padding. */
    memcpy(o->fragments[0], pcd+0x104, 0x50);
    o->fragment_index[0]=-1;
    for (int f=0;f<9;f++) {
        memcpy(o->fragments[f+1],o->encrypted+f*FRAG_SIZE,FRAG_SIZE);
        o->fragment_index[f+1]=(int16_t)f;
    }
    return true;
}
