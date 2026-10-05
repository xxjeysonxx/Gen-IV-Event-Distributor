// SPDX-License-Identifier: GPL-3.0-or-later
#include <nds.h>
#include <stdio.h>
#include <calico/nds/pxi.h>
#include <calico/nds/wlmgr.h>
#define CMD_STOP 0u
#define CMD_START 1u
typedef struct { const char* name; u8 lang; u8 game; } EventInfo;
static const EventInfo events[]={
    {"ARCEUS",0,0},
    {"CROBAT KONA",0,0},
    {"DRAGONITE",0,0},
    {"JIRACHI",0,0},
    {"LUCARIO",0,0},
    {"PICHU SHINY",0,0},
    {"PIKACHU ASH",0,0},
    {"PIKACHU BETA",0,0},
    {"REGIGAS",0,0},
    {"SHAYMIN!",0,0},
    {"WEAVILE",0,0},
    {"BR ELECTIVIRE",0,1},
    {"BR MAGMORTAR",0,1},
    {"BR pikachu",0,1},
    {"Celebi",0,1},
    {"Jirachi",0,1},
    {"Manaphy Egg",0,1},
    {"Mew",0,1},
    {"Shiny eevee",0,1},
    {"Shiny Entei",0,1},
    {"Shiny Raikou",0,1},
    {"Shiny Suicune",0,1},
    {"Winner's Path pokewalker",0,1},
    {"Yellow Forest Pokewalker",0,1},
    {"ARCEUS!",1,0},
    {"CELEBI!",1,0},
    {"ELECTRIVE BR",1,0},
    {"JIRACHI!",1,0},
    {"MAGMOTAR BR",1,0},
    {"MANAPHY EGG",1,0},
    {"PICHU SHINY",1,0},
    {"PIKACHU BR",1,0},
    {"REGIGIGAS!",1,0},
    {"SHAYMIN!",1,0},
    {"Camino Amarillo pokewalker",1,1},
    {"CELEBI",1,1},
    {"EEVEE SHINY",1,1},
    {"ELECTRIVE BR",1,1},
    {"Entei Shiny",1,1},
    {"HUEVO MANAPHY BETA",1,1},
    {"JIRACHI!",1,1},
    {"MAGMORTAR BR",1,1},
    {"MEW!",1,1},
    {"PIKACHU BR",1,1},
    {"Raikou Shiny",1,1},
    {"Suicune Shiny",1,1},
    {"Via Campeon Pokewalker",1,1},
};
#define EVENT_COUNT (sizeof(events)/sizeof(events[0]))
#define VISIBLE_ROWS 6
static const char* langNames[]={"English","Espanol"};
static const char* gameNames[]={"Diamond / Pearl","HeartGold / SoulSilver"};
static PrintConsole topConsole,bottomConsole;
static volatile bool diagSeen,diagOn,diagRadio;
static volatile unsigned diagDone,diagError,diagDropped,diagFrag;
static void diagHandler(void* user,u32 v){(void)user;diagSeen=true;diagOn=(v>>25)&1;diagRadio=(v>>24)&1;diagDone=(v>>18)&0x3f;diagError=(v>>12)&0x3f;diagDropped=(v>>6)&0x3f;diagFrag=v&0x3f;}
static unsigned filtered[EVENT_COUNT],filteredCount;
static void rebuild(unsigned lang,unsigned game){filteredCount=0;for(unsigned i=0;i<EVENT_COUNT;i++)if(events[i].lang==lang&&events[i].game==game)filtered[filteredCount++]=i;}
static void header(const char* page){consoleSelect(&topConsole);consoleClear();iprintf("GEN IV EVENT DISTRIBUTOR PRE-ALPHA\nBy SooraMaru\n-------------------------------\n%s\n\n",page);}
static void drawLang(unsigned s){header("Select language:");for(unsigned i=0;i<2;i++)iprintf("%c %s\n\n",i==s?'>':' ',langNames[i]);iprintf("\nA=OPEN   START=EXIT\n");}
static void drawGame(unsigned s,unsigned lang){header(langNames[lang]);iprintf("Select game:\n\n");for(unsigned i=0;i<2;i++)iprintf("%c %s\n\n",i==s?'>':' ',gameNames[i]);iprintf("\nA=OPEN   B=BACK\n");}
static void drawEvents(unsigned sel,unsigned lang,unsigned game,bool on){header(gameNames[game]);unsigned first=(sel/VISIBLE_ROWS)*VISIBLE_ROWS;for(unsigned r=0;r<VISIBLE_ROWS&&first+r<filteredCount;r++){unsigned pos=first+r,id=filtered[pos];iprintf("%c %-27.27s\n",pos==sel?'>':' ',events[id].name);iprintf("\n");}iprintf("\n%u/%u  %s [LOCKED]\n",sel+1,filteredCount,langNames[lang]);iprintf("%s\n",on?"DISTRIBUTING - B=STOP":"A=SEND  B=BACK");}
static bool debugMode=false;
#define BROADCAST_CHANNEL 7
#define GIFT_COOLDOWN_FRAMES 180

static void drawLog(unsigned sel,unsigned lang,unsigned game,bool on,unsigned cooldown){
    consoleSelect(&bottomConsole);
    consoleClear();

    if(!debugMode){
        iprintf("GEN IV EVENT DISTRIBUTOR\n");
        iprintf("------------------------\n\n");
        if(on){
            iprintf("Enviando regalo....\n\n");
            iprintf("Canal activo: %u\n",BROADCAST_CHANNEL);
        }else if(cooldown){
            unsigned sec=(cooldown+59)/60;
            iprintf("Preparando siguiente regalo...\n\n");
            iprintf("Espera: %u s\n",sec);
            iprintf("Canal activo: %u\n",BROADCAST_CHANNEL);
        }else{
            iprintf("Listo para enviar.\n\n");
            iprintf("Canal activo: %u\n",BROADCAST_CHANNEL);
        }
        return;
    }

    iprintf("DEBUG / STATUS\n");
    iprintf("---------------------\n");
    iprintf("Language: %s [LOCKED]\n",langNames[lang]);
    iprintf("Game: %.20s\n",gameNames[game]);
    if(filteredCount){
        unsigned id=filtered[sel];
        iprintf("Selected: %.20s\n",events[id].name);
    }
    iprintf("Broadcast: %s\n",on?"ON":"OFF");
    iprintf("Channel: %u\n",BROADCAST_CHANNEL);
    iprintf("Cooldown: %u frames\n",cooldown);
    if(diagSeen){
        iprintf("ARM7: %s  MWL: %s\n",diagOn?"ON":"OFF",diagRadio?"ON":"OFF");
        iprintf("TX done:%2u err:%2u drop:%2u\n",diagDone,diagError,diagDropped);
        iprintf("Fragment: %u/9\n",diagFrag);
    }else{
        iprintf("Telemetry: waiting...\n");
    }
}
int main(void){
 videoSetMode(MODE_0_2D);videoSetModeSub(MODE_0_2D);vramSetBankA(VRAM_A_MAIN_BG);vramSetBankC(VRAM_C_SUB_BG);
 consoleInit(&topConsole,3,BgType_Text4bpp,BgSize_T_256x256,31,0,true,true);consoleInit(&bottomConsole,3,BgType_Text4bpp,BgSize_T_256x256,31,0,false,true);
 consoleSelect(&bottomConsole);iprintf("Initializing ARM7 / Wi-Fi...\n");pxiSetHandler(PxiChannel_User2,diagHandler,NULL);pxiWaitRemote(PxiChannel_User0);
 if(!wlmgrInitDefault()){iprintf("Wi-Fi initialization failed.\n");while(pmMainLoop())swiWaitForVBlank();return 1;}wlmgrStart(WlMgrMode_LocalComms);unsigned timeout=0;while(wlmgrGetState()!=WlMgrState_Idle&&timeout<300){swiWaitForVBlank();timeout++;}if(wlmgrGetState()!=WlMgrState_Idle){iprintf("NTR Wi-Fi failed to start.\n");while(pmMainLoop())swiWaitForVBlank();return 2;}
 unsigned page=0,lang=0,game=0,sel=0,redraw=0,cooldown=0;bool on=false;drawLang(lang);drawLog(sel,lang,game,on,cooldown);
 while(pmMainLoop()){swiWaitForVBlank();scanKeys();int k=keysDown();
  if(cooldown) cooldown--;
  if((keysHeld()&(KEY_L|KEY_R))==(KEY_L|KEY_R) && (k&(KEY_L|KEY_R))){
      debugMode=!debugMode;
      drawLog(sel,lang,game,on,cooldown);
  }
  if(page==0){if(k&KEY_UP){lang^=1;drawLang(lang);}if(k&KEY_DOWN){lang^=1;drawLang(lang);}if(k&KEY_A){page=1;game=0;drawGame(game,lang);}if(k&KEY_START)break;continue;}
  if(page==1){if(k&KEY_UP){game^=1;drawGame(game,lang);}if(k&KEY_DOWN){game^=1;drawGame(game,lang);}if(k&KEY_B){page=0;drawLang(lang);}if(k&KEY_A){rebuild(lang,game);sel=0;page=2;drawEvents(sel,lang,game,on);drawLog(sel,lang,game,on,cooldown);}continue;}
  if(!on){if(k&KEY_UP){sel=sel?sel-1:filteredCount-1;drawEvents(sel,lang,game,on);drawLog(sel,lang,game,on,cooldown);}if(k&KEY_DOWN){sel=(sel+1)%filteredCount;drawEvents(sel,lang,game,on);drawLog(sel,lang,game,on,cooldown);}if(k&KEY_A&&filteredCount&&cooldown==0){unsigned id=filtered[sel];diagSeen=false;pxiSend(PxiChannel_User0,CMD_START|(id<<8)|(lang<<16));on=true;drawEvents(sel,lang,game,on);drawLog(sel,lang,game,on,cooldown);}if(k&KEY_B){page=1;drawGame(game,lang);consoleSelect(&bottomConsole);consoleClear();iprintf("Language locked: %s\n",langNames[lang]);}}else if(k&KEY_B){
      pxiSend(PxiChannel_User0,CMD_STOP);
      on=false;
      cooldown=GIFT_COOLDOWN_FRAMES;
      drawEvents(sel,lang,game,on);
      drawLog(sel,lang,game,on,cooldown);
  }
  if((on||cooldown)&&++redraw>=30){
      redraw=0;
      if(on) pxiSend(PxiChannel_User2,1);
      drawLog(sel,lang,game,on,cooldown);
  }
 }pxiSend(PxiChannel_User0,CMD_STOP);wlmgrStop();return 0;
}
