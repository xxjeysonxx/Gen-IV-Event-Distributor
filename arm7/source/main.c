// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Gen4 Event Distributor ARM7
 *
 * Base initialization follows devkitPro/default-arm7 (ZPL-2.1, fincs/devkitPro).
 * Modified 2026-10-04: added User0 PXI control and a Gen IV beacon TX thread.
 */
#include <string.h>
#include <calico.h>
#include <calico/nds/arm7/ntrwifi.h>
#include <calico/dev/netbuf.h>
#include <calico/dev/mwl.h>
#include "beacon_data.h"

#define CMD_STOP  0u
#define CMD_START 1u
#define CHANNEL   7u
#define FRAME_LEN 188u

/* Gen IV Mystery Gift discovery IDs.
 * Keep a complete 10-fragment cycle on one GGID, then switch language.
 * This preserves the proven beacon/fragment timing while allowing both
 * English and Spanish retail games to discover the same selected gift.
 */
#define GGID_EN 0x00400318u
#define GGID_ES 0x008000D0u

static volatile bool s_broadcasting = false;
static u16 s_sequence;
static unsigned s_fragment;
static volatile unsigned s_event;
static volatile unsigned s_language;
static volatile u32 s_txQueued;
static volatile u32 s_txDone;
static volatile u32 s_txError;
static volatile u32 s_txDropped;
static volatile bool s_radioStarted;

static Thread s_beaconThread;
alignas(8) static u8 s_beaconStack[2048];

static void put16(u8* p,u16 v){p[0]=(u8)v;p[1]=(u8)(v>>8);}
static void put32(u8* p,u32 v){p[0]=(u8)v;p[1]=(u8)(v>>8);p[2]=(u8)(v>>16);p[3]=(u8)(v>>24);}

static void userHandler(void* user,u32 data)
{
    (void)user;
    if((data&0xffu)==CMD_START){
        unsigned requested=(data>>8)&0xffu;
        if(requested>=GEN4_EVENT_COUNT) requested=0;
        s_event=requested;
        s_language=(data>>16)&1u;
        s_fragment=0;
                s_sequence=0;
        s_txQueued=0;
        s_txDone=0;
        s_txError=0;
        s_txDropped=0;
        /*
         * Keep MWL alive across gifts.  Do NOT clear s_radioStarted here:
         * STOP/START can arrive between beacon-thread iterations, and the old
         * code could then mwlDevStart() an already-running device.
         */
        s_broadcasting=true;
    } else if(data==CMD_STOP) {
        s_broadcasting=false;
    }
}

static void txCallback(void* arg,MwlTxEvent evt,MwlDataTxHdr* hdr)
{
    (void)arg; (void)hdr;
    switch(evt){
        case MwlTxEvent_Queued:  s_txQueued++;  break;
        case MwlTxEvent_Done:    s_txDone++;    break;
        case MwlTxEvent_Error:   s_txError++;   break;
        case MwlTxEvent_Dropped: s_txDropped++; break;
    }
}

static bool sendBeacon(unsigned slot)
{
    NetBuf* nb=netbufAlloc(0,FRAME_LEN,NetBufPool_Tx);
    if(!nb) {
        s_txDropped++;
        return false;
    }

    u8* p=(u8*)netbufGet(nb);
    memset(p,0,FRAME_LEN);

    /* Raw IEEE 802.11 management beacon. */
    p[0]=0x80;
    memset(p+4,0xff,6);
    memcpy(p+10,g_gen4_mac,6);
    memcpy(p+16,g_gen4_mac,6);
    put16(p+22,(u16)((s_sequence++ & 0x0fff)<<4));

    put16(p+32,0x000a);
    put16(p+34,0x0021);

    unsigned o=36;
    p[o++]=0x01; p[o++]=0x02; p[o++]=0x82; p[o++]=0x84;
    p[o++]=0x03; p[o++]=0x01; p[o++]=CHANNEL;
    p[o++]=0x05; p[o++]=0x05;
    p[o++]=0x01; p[o++]=0x02; p[o++]=0; p[o++]=0; p[o++]=0;

    p[o++]=0xdd; p[o++]=0x88;
    p[o++]=0x00; p[o++]=0x09; p[o++]=0xbf; p[o++]=0x00;

    put32(p+o,10); o+=4;
    put16(p+o,1); o+=2;
    put16(p+o,1); o+=2;
    u32 activeGgid = s_language ? GGID_ES : GGID_EN;
    put32(p+o,activeGgid); o+=4;
    put16(p+o,0); o+=2;
    put16(p+o,0x70); o+=2;
    put16(p+o,0x28); o+=2;
    put16(p+o,0x0c); o+=2;
    put16(p+o,g_gen4_events[s_event].checksum); o+=2;
    put16(p+o,slot==9 ? 0xffff : (u16)slot); o+=2;
    put32(p+o,0x3a8); o+=4;
    memcpy(p+o,g_gen4_events[s_event].fragments[slot],GEN4_FRAG_SIZE);
    o+=GEN4_FRAG_SIZE;

    if(o!=FRAME_LEN){
        netbufFree(nb);
        s_txDropped++;
        return false;
    }

    /*
     * IMPORTANT: ntrwifiTx() is intentionally NOT used here.
     * It accepts DIX/Ethernet frames and first calls mwlDevDixToWlan().
     * Our packet is already a complete raw 802.11 management frame.
     */
    netbufFlush(nb);
    mwlDevTx(0,nb,txCallback,NULL);
    return true;
}

static void sendTelemetry(void)
{
    /*
     * PXI immediate payload is kept within 26 bits.
     * 25: broadcaster ON
     * 24: radio started
     * 23..18 queued (6-bit rolling)
     * 17..12 done
     * 11..6  errors
     * 5..0   dropped
     */
    u32 v=0;
    if(s_broadcasting) v|=1u<<25;
    if(s_radioStarted) v|=1u<<24;
    v|=(s_txQueued  &0x3f)<<18;
    v|=(s_txDone    &0x3f)<<12;
    v|=(s_txError   &0x3f)<<6;
    v|=(s_txDropped &0x3f);
    pxiSend(PxiChannel_User1,v);
}


static void statusHandler(void* user,u32 data)
{
    (void)user; (void)data;
    /* 25:on 24:radio 23..18:done 17..12:error 11..6:dropped 5..0:fragment */
    u32 v=0;
    if(s_broadcasting) v|=1u<<25;
    if(s_radioStarted) v|=1u<<24;
    v|=(s_txDone&0x3f)<<18;
    v|=(s_txError&0x3f)<<12;
    v|=(s_txDropped&0x3f)<<6;
    v|=s_fragment&0x3f;
    pxiSend(PxiChannel_User2,v);
}
static int beaconThreadMain(void* arg)
{
    (void)arg;
    unsigned reportDiv=0;

    for(;;){
        if(s_broadcasting){
            if(!s_radioStarted){
                /*
                 * ntrwifiInit() has already powered/reset/calibrated Mitsumi.
                 * WlMgr leaves it initialized but not started while Idle.
                 * Start MWL explicitly for raw management-frame transmission.
                 *
                 * Infra mode is deliberate: Calico enables normal TX queues
                 * in mwlDevStart() for LocalGuest/Infra. LocalHost currently
                 * has unfinished beacon/TX setup in Calico.
                 */
                mwlDevSetMode(MwlMode_Infra);
                mwlDevSetBssid(g_gen4_mac);
                mwlDevSetChannel(CHANNEL);
                mwlDevStart();
                s_radioStarted=true;
            }

            sendBeacon(s_fragment);
            s_fragment++;
            if(s_fragment>=GEN4_FRAG_COUNT){
                s_fragment=0;
                /* Keep one discovery GGID stable for many complete cards.
                 * Switching after every card (~100 ms) was too fast for
                 * retail-game scanning and made one language disappear.
                 */
                
            }

            if(++reportDiv>=10){
                reportDiv=0;
                sendTelemetry();
            }
            threadSleep(10240);
        } else {
            /*
             * Idle between gifts: keep the already-started MWL session alive.
             * We simply stop queuing beacons.  The next CMD_START changes the
             * event/language, resets fragment/sequence, and resumes on the
             * same radio session.  This avoids a STOP->START MWL race that
             * could leave a retail receiver in a bad state.
             */
            threadSleep(1000);
        }
    }
    return 0;
}

int main(int argc,char* argv[])
{
    (void)argc; (void)argv;

    /* Keep the normal devkitPro/default-arm7 initialization sequence. */
    envReadNvramSettings();
    keypadStartExtServer();

    lcdSetIrqMask(DISPSTAT_IE_ALL,DISPSTAT_IE_VBLANK);
    irqEnable(IRQ_VBLANK);

    rtcInit();
    rtcSyncTime();

    pmInit();

    touchInit();
    touchStartServer(80,MAIN_THREAD_PRIO);

    /* Standard Calico wireless server. */
    wlmgrStartServer(MAIN_THREAD_PRIO-8);

    /* Private control channel used only for START/STOP. */
    pxiSetHandler(PxiChannel_User0,userHandler,NULL);
    pxiSetHandler(PxiChannel_User2,statusHandler,NULL);

    /* Beacon work must not replace the normal ARM7 PM/event loop. */
    threadPrepare(&s_beaconThread,beaconThreadMain,NULL,
        &s_beaconStack[sizeof(s_beaconStack)],MAIN_THREAD_PRIO-7);
    threadAttachLocalStorage(&s_beaconThread,NULL);
    threadStart(&s_beaconThread);

    while(pmMainLoop()){
        threadWaitForVBlank();
    }
    return 0;
}
