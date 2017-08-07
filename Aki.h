
//#include "edao.h"
// #include "json.h"

#pragma warning (disable: 4996)

#define L1(x)               L ## x
#define L(x)                L1(x)
#define TO_STRING1(x)       #x
#define TO_STRING(x)        TO_STRING1(x)
/*
#define EX_DIR              "Arianrhod"
#define STATUS_CONFIG_PATH  EX_DIR"\\status.config.txt"
#define STATUS_RESULT_PATH  EX_DIR"\\status.config.result.txt"
#define STATUS_DUMP_PATH    EX_DIR"\\status.dump.txt"
*/
#define EX_DIR_                     Aki
#define EX_DIR                      TO_STRING(EX_DIR_)
#define STATUS_CONFIG_PATH          TO_STRING(EX_DIR_##\\status.config.txt)
#define STATUS_RESULT_PATH          TO_STRING(EX_DIR_##\\status.config.result.txt)
#define STATUS_FORMULA_PATH         TO_STRING(EX_DIR_##\\status.config.result.formula.txt)
#define STATUS_FORMULA_HTML_PATH    TO_STRING(EX_DIR_##\\status.config.result.formula.html)
#define STATUS_DUMP_PATH            TO_STRING(EX_DIR_##\\status.dump.txt)


#if CONSOLE_DEBUG
LARGE_INTEGER lFrequency, lStopCounter, lStartCounter;
#endif

// mark
ULONG   g_flag = 0;
namespace NFLAG
{
    static const ULONG_PTR IsSubHPEveryAct  = 0x00000001;
}

namespace T_NAME
{
    CHAR_T_STATUS_RatioY RatioY[T_NAME::CHR_COUNT] =
    {
        //  HP  EP  STR DEF ATS ADF DEX AGL MOV SPD DEX AGL RNG
        {   0,  0,  20, 10, 18, 10, 19, 7,  4,  30, 0,  0,  0   },
        {   0,  0,  18, 8,  19, 10, 20, 9,  3,  29, 0,  0,  0   },
        {   0,  0,  25, 9,  20, 10, 19, 6,  3,  28, 0,  0,  0   },
        {   0,  0,  21, 11, 18, 10, 17, 7,  5,  27, 0,  0,  0   },
        {   0,  0,  18, 9,  19, 10, 19, 9,  6,  31, 0,  0,  0   },
        {   0,  0,  20, 8,  18, 10, 21, 10, 5,  30, 0,  0,  0   },
        {   0,  0,  21, 11, 19, 10, 22, 8,  8,  29, 0,  0,  0   },
        {   0,  0,  21, 11, 19, 10, 24, 9,  6,  31, 0,  0,  0   },
        {   0,  0,  19, 10, 18, 10, 18, 8,  4,  28, 0,  0,  0   },
        {   0,  0,  21, 9,  16, 10, 19, 7,  3,  29, 0,  0,  0   },
        {   0,  0,  22, 11, 17, 10, 19, 7,  5,  29, 0,  0,  0   },
        {   0,  0,  0,  204,204,0,  0,  0,  204,204,0,  0,  0   },
    };

    CHAR_T_STATUS_RatioX RatioX[T_NAME::CHR_COUNT] =
    {
        //	HP      EP      STR     DEF     ATS     ADF     DEX     AGL     MOV     SPD     DEX     AGL     RNG
        { 	2.3f,   0.0f,   0.109f, 0.105f, 0.105f, 0.1f,   0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.0f,   0.0f,   0.09f,  0.09f,  0.11f,  0.11f,  0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	1.7f,   0.0f,   0.08f,  0.095f, 0.115f, 0.105f, 0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.5f,   0.0f,   0.115f, 0.105f, 0.1f,   0.09f,  0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.2f,   0.0f,   0.095f, 0.104f, 0.111f, 0.11f,  0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.1f,   0.0f,   0.112f, 0.085f, 0.105f, 0.105f, 0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.7f,   0.0f,   0.113f, 0.105f, 0.113f, 0.105f, 0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.45f,  0.0f,   0.12f,  0.11f,  0.111f, 0.1f,   0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.15f,  0.0f,   0.105f, 0.1f,   0.095f, 0.095f, 0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.4f,   0.0f,   0.11f,  0.1f,   0.1f,   0.09f,  0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
        { 	2.6f,   0.0f,   0.111f, 0.111f, 0.09f,  0.09f,  0.001f, 0.001f, 0.0f,   0.005f, 0.0f,   0.0f,   0.0f,	},
    };

    DECL_SELECTANY
    const char* StatusName[] = {"HP", "EP", "STR", "DEF", "ATS", "ADF", "DEX", "AGL", "MOV", "SPD", "DEXRate", "AGLRate", "RNG"};
}

ULONG_PTR StubCheckStackBalance = 0x006790C6;

BOOL bArianrhodLimitKaijo;
BOOL bEnemySBreak;
BOOL bShowAT;
BOOL bForceKillEnemyAtScene;
BOOL bForceShowBurstMode;
BOOL bForceShowInfOnMiniMap;
BOOL bEnableSkipCraftAnime;
BOOL bEnableSpecialEffect;
INT nDifficulty;
INT nBattleEncount;
INT nSupportCraftInterval;
BOOL bDisableDamageRandom;
BOOL bDisableDamageUpLimit;
INT nATBonus;
INT nSepithUpLimit;
BOOL bAutoAnalyzeMonsInf;

BOOL bSelectTeamMemberFreely;
BOOL bSpecialTeamMemberEquipmentLimitKaijo;
BOOL bEnableSelectTeamMember;
INT nAutoFish;
INT nFastFish;
BOOL bRoyalFlush;
BOOL bAutoHorrorCoaster;

BOOL bConfigStatus;
BOOL bOutputStatusConfigResult;
BOOL bDumpStatus;
INT nYin_no_AGLRate;

BOOL bDisableMouse;

typedef struct _SStatusRate
{
    INT     HP_a;
    INT     STR_a;
    INT     DEF_a;
    INT     ATS_a;
    INT     ADF_a;
    INT     SPD_a;
    INT     DEX_a;
    INT     AGL_a;
    INT     MOV_a;

    INT     HP_b;
    INT     STR_b;
    INT     DEF_b;
    INT     ATS_b;
    INT     ADF_b;
    INT     SPD_b;
    INT     DEX_b;
    INT     AGL_b;
    INT     MOV_b;

    INT     DEXRate;
    INT     AGLRate;
    BOOL    ResistNone;
    BOOL    ResistAbnormalCondition;
    BOOL    ResistAbilityDown;
    BOOL    ResistATDelay;
} SStatusRate;

//SStatusRate statusRateUserDefined;
SStatusRate sRate;
#define     statusRateUserDefined sRate

class CClass
{
    typedef struct // 0x4C
    {
        DUMMY_STRUCT(6);
        USHORT  IsUsed;     // 0x6
        DUMMY_STRUCT(4);
        USHORT  digit;      // 0xC
        DUMMY_STRUCT(0x3E);
    } POKER_ENTRY;

public:
    SHORT THISCALL HorrorCoasterFastExit(int vKey);
    VOID THISCALL PokerRoyalFlush();

    VOID THISCALL ShowHorrorCoasterText(INT x, INT y, float par3, LPCSTR text, ULONG par5, ULONG color);
    DECL_STATIC_METHOD_POINTER(CClass, ShowHorrorCoasterText);

    VOID THISCALL HorrorCoasterEvaluationPositionRestore(ULONG par1);
    DECL_STATIC_METHOD_POINTER(CClass, HorrorCoasterEvaluationPositionRestore);

    VOID THISCALL PositionPC2PSP(PFLOAT par1, Gdiplus::PointF *Target, PFLOAT par3);
};
INIT_STATIC_MEMBER(CClass::StubShowHorrorCoasterText);
INIT_STATIC_MEMBER(CClass::StubHorrorCoasterEvaluationPositionRestore);

/************************************************************************
    C Start
************************************************************************/

// trailing .0
int sprintfDoubleNoZero(char * str, double num)
{
    if (str == nullptr) return 0;
    char* p;
    int nWriten = sprintf(str, "%g", num);
    if (strchr(str, '.') == nullptr)
    {
        p = str + nWriten;
        *p++ = '.';
        *p++ = '0';
        *p = 0;
        nWriten += 2;
    }
    return nWriten;
}

// data : num 0 mum
// char : len 1 len
// wchar: len 2 len
inline size_t nmemcpy( void * dst, const void * src, size_t count ,size_t typesize = 0)
{
    if (src == nullptr || dst == nullptr)
        return 0;

    if (typesize == 0)
    {
        memcpy(dst, src, count);
        return count;
    }
    else
    {
        memcpy(dst, src, count*typesize + typesize);
        return count;
    }
}

size_t nstrcpy(char * dst, const char * src)
{
    return nmemcpy(dst, src, strlen(src), sizeof(*src));
}

size_t nwcscpy(wchar_t* dst, const wchar_t* src)
{
    return nmemcpy(dst, src, wcslen(src), sizeof(*src));
}
#define nstrcpyA nstrcpy
#define nstrcpyW nwcscpy
//#define NCSTRCPYA NCSTRCPY
//#define NCSTRCPYW NCSTRCPY
#define NCSTRCPY(dst, src) nmemcpy(dst, src, CONST_STRLEN(src), sizeof(*(src)))


/************************************************************************
    C End
************************************************************************/

// inline VOID SaturateConvertEx(void* arg1, FLOAT arg2, FLOAT arg3, FLOAT arg4 = 0) {

//     FLOAT v7 = ( arg2 < arg3 ) ? arg2 : arg3;
//     *arg1 = ( ( arg2 < arg3 ) ? arg2 : arg3 > arg4 ) ? ( arg2 < arg3 ) ? arg2 : arg3 : arg4
// }

// #define SaturateConvertEx(arg1, arg2, arg3, arg4) *arg1 = ( ( arg2 < arg3 ) ? arg2 : arg3 > arg4 ) ? ( arg2 < arg3 ) ? arg2 : arg3 : arg4

// #define SaturateConvert(arg1, arg2) ( arg2 <= 0xFFFF ) ? \
//                                     ( arg2 >= arg1 ) ? arg2 : arg1 \
//                                     : -1
#define SaturateConvertEx(...) do {} while(0)
#define SaturateConvert(...) do {} while(0)
// inline INT SaturateConvert(INT arg1, INT arg2) {
//     if ( arg2 <= 0xFFFF )
//     {
//         if ( arg2 >= arg1 )
//             return arg2;
//         else
//             return arg1;
//     }
//     else
//     {
//         return -1;
//     }
// }


VOID ConfigInit()
{
    static WCHAR szConfigExPath[] = L".\\config_ex.ini";
    typedef struct
    {
        PVOID       Var;
        CHAR        Type;
        LPCWSTR     lpAppName;
        LPCWSTR     lpKeyName;
        union
        {
            BOOL    bDefault;
            INT     nDefault;
            LPCWSTR lpDefault;
        };
        //LPCWSTR     lpFileName;
    } CONFIG_ENTRY;

    static CONFIG_ENTRY Config[] =
    {
        { (BOOL*)&bArianrhodLimitKaijo,                     'b',    L"Arianrhod",   L"ArianrhodLimitKaijo",         TRUE,   },
        { (BOOL*)&bEnemySBreak,                             'b',    L"Arianrhod",   L"EnemySBreak",                 TRUE,   },
        { (BOOL*)&bShowAT,                                  'b',    L"Arianrhod",   L"ShowAT",                      TRUE,   },
        { (BOOL*)&bForceKillEnemyAtScene,                   'b',    L"Arianrhod",   L"ForceKillEnemyAtScene",       TRUE,   },
        { (BOOL*)&bForceShowBurstMode,                      'b',    L"Arianrhod",   L"ForceShowBurstMode",          TRUE,   },
        { (BOOL*)&bForceShowInfOnMiniMap,                   'b',    L"Arianrhod",   L"ForceShowInfOnMiniMap",       TRUE,   },
        { (BOOL*)&bEnableSkipCraftAnime,                    'b',    L"Arianrhod",   L"EnableSkipCraftAnime",        TRUE,   },
        { (BOOL*)&bEnableSpecialEffect,                     'b',    L"Arianrhod",   L"EnableSpecialEffect",         FALSE,  },

        { (LONG*)&nDifficulty,                              'i',    L"Battle",      L"Difficulty",                  0,      },
        { (LONG*)&sRate.HP_a,                               'i',    L"Battle",      L"HP_a",                        1000,   },
        { (LONG*)&sRate.STR_a,                              'i',    L"Battle",      L"STR_a",                       1000,   },
        { (LONG*)&sRate.DEF_a,                              'i',    L"Battle",      L"DEF_a",                       1000,   },
        { (LONG*)&sRate.ATS_a,                              'i',    L"Battle",      L"ATS_a",                       1000,   },
        { (LONG*)&sRate.ADF_a,                              'i',    L"Battle",      L"ADF_a",                       1000,   },
        { (LONG*)&sRate.SPD_a,                              'i',    L"Battle",      L"SPD_a",                       1000,   },
        { (LONG*)&sRate.DEX_a,                              'i',    L"Battle",      L"DEX_a",                       1000,   },
        { (LONG*)&sRate.AGL_a,                              'i',    L"Battle",      L"AGL_a",                       1000,   },
        { (LONG*)&sRate.MOV_a,                              'i',    L"Battle",      L"MOV_a",                       1000,   },

        { (LONG*)&sRate.HP_b,                               'i',    L"Battle",      L"HP_b",                        0,      },
        { (LONG*)&sRate.STR_b,                              'i',    L"Battle",      L"STR_b",                       0,      },
        { (LONG*)&sRate.DEF_b,                              'i',    L"Battle",      L"DEF_b",                       0,      },
        { (LONG*)&sRate.ATS_b,                              'i',    L"Battle",      L"ATS_b",                       0,      },
        { (LONG*)&sRate.ADF_b,                              'i',    L"Battle",      L"ADF_b",                       0,      },
        { (LONG*)&sRate.SPD_b,                              'i',    L"Battle",      L"SPD_b",                       0,      },
        { (LONG*)&sRate.DEX_b,                              'i',    L"Battle",      L"DEX_b",                       0,      },
        { (LONG*)&sRate.AGL_b,                              'i',    L"Battle",      L"AGL_b",                       0,      },
        { (LONG*)&sRate.MOV_b,                              'i',    L"Battle",      L"MOV_b",                       0,      },

        { (LONG*)&sRate.DEXRate,                            'i',    L"Battle",      L"DEXRate",                     0,      },
        { (LONG*)&sRate.AGLRate,                            'i',    L"Battle",      L"AGLRate",                     0,      },
        { (BOOL*)&sRate.ResistNone,                         'b',    L"Battle",      L"ResistNone",                  FALSE,  },
        { (BOOL*)&sRate.ResistAbnormalCondition,            'b',    L"Battle",      L"ResistAbnormalCondition",     FALSE,  },
        { (BOOL*)&sRate.ResistAbilityDown,                  'b',    L"Battle",      L"ResistAbilityDown",           FALSE,  },
        { (BOOL*)&sRate.ResistATDelay,                      'b',    L"Battle",      L"ResistATDelay",               FALSE,  },

        { (LONG*)&nBattleEncount,                           'i',    L"Battle",      L"BattleEncount",               3,      },
        { (LONG*)&nSupportCraftInterval,                    'i',    L"Battle",      L"SupportCraftInterval",        0,      },
        { (BOOL*)&bDisableDamageRandom,                     'b',    L"Battle",      L"DisableDamageRandom",         FALSE,  },
        { (BOOL*)&bDisableDamageUpLimit,                    'b',    L"Battle",      L"DisableDamageUpLimit",        FALSE,  },
        { (LONG*)&nATBonus,                                 'i',    L"Battle",      L"ATBonus",                     0,      },
        { (LONG*)&nSepithUpLimit,                           'i',    L"Battle",      L"SepithUpLimit",               0,      },
        { (BOOL*)&bAutoAnalyzeMonsInf,                      'b',    L"Battle",      L"AutoAnalyzeMonsInf",          FALSE,  },

        { (BOOL*)&bSelectTeamMemberFreely,                  'b',    L"DT",          L"SelectTeamMemberFreely",      FALSE,  },
        { (BOOL*)&bSpecialTeamMemberEquipmentLimitKaijo,    'b',    L"DT",          L"SpecialTeamMemberEquipmentLimitKaijo",    FALSE,  },
        { (BOOL*)&bEnableSelectTeamMember,                  'b',    L"DT",          L"EnableSelectTeamMember",      FALSE,  },

        { (LONG*)&nAutoFish,                                'i',    L"DT",          L"AutoFish",                    0,      },
        { (LONG*)&nFastFish,                                'i',    L"DT",          L"FastFish",                    0,      },
        { (BOOL*)&bRoyalFlush,                              'b',    L"DT",          L"RoyalFlush",                  FALSE,  },
        { (BOOL*)&bAutoHorrorCoaster,                       'b',    L"DT",          L"AutoHorrorCoaster",           FALSE,  },

        { (BOOL*)&bConfigStatus,                            'b',    L"Status",      L"ConfigStatus",                FALSE,  },
        { (BOOL*)&bOutputStatusConfigResult,                'b',    L"Status",      L"OutputStatusConfigResult",    TRUE,   },
        { (BOOL*)&bDumpStatus,                              'b',    L"Status",      L"DumpStatus",                  FALSE,  },
        { (LONG*)&nYin_no_AGLRate,                          'i',    L"Status",      L"Yin_no_AGLRate",              0,      },

        { (BOOL*)&bDisableMouse,                            'b',    L"DT",          L"DisableMouse",                FALSE,  },
    };

    CONFIG_ENTRY *Entry;
    FOR_EACH(Entry, Config, countof(Config))
    {
        switch (Entry->Type)
        {
        case 'b':
            *(BOOL*)Entry->Var = !!GetPrivateProfileIntW(Entry->lpAppName, Entry->lpKeyName, Entry->bDefault, szConfigExPath);
            break;
        case 'i':
            *(INT*)Entry->Var = GetPrivateProfileIntW(Entry->lpAppName, Entry->lpKeyName, Entry->nDefault, szConfigExPath);
            break;
        default:
            ;
        }
    }


    if (nSupportCraftInterval < 0 || nSupportCraftInterval == 350)
        nSupportCraftInterval = 0;

    if (nATBonus < 0 || nATBonus > 16)
        nATBonus = 0;

    if (nDifficulty < 0 || nDifficulty > 5)
        nDifficulty = 0;

    // SaturateConvertEx(&nSepithUpLimit, nSepithUpLimit, 9999, 0);
    // nAutoFish = SaturateConvert(USHORT(0), nAutoFish);
    // nFastFish = SaturateConvert(USHORT(0), nFastFish);

    // SaturateConvertEx(&nYin_no_AGLRate, nYin_no_AGLRate, 100, 0);

#if CONSOLE_DEBUG
    FOR_EACH(Entry, Config, countof(Config))
    {
        switch (Entry->Type)
        {
        case 'b':
            PrintConsoleW(L"b:%s:%d\r\n", Entry->lpKeyName, *(BOOL*)Entry->Var);
            break;
        case 'i':
            PrintConsoleW(L"i:%s:%d\r\n", Entry->lpKeyName, *(INT*)Entry->Var);
            break;
        default:
            ;
        }
    }
#endif
}


// //acgn
// VOID THISCALL CBattle::LoadMSFile(PMONSTER_STATUS MSData, ULONG MSFileIndex)
// {
//     ULONG i, v15;
//     char it3Path[MAX_PATH];
//     TYPE_OF(&CBattle::LoadMonsterIt3) StubLoadMonsterIt3;
//     *(PVOID*)&StubLoadMonsterIt3 = (PVOID)0x00673FB3;


//     for (i = 0; i < 20; ++i)
//     {
//         v15 = (i << 8) | 0x88000;
//         if ( (MSFileIndex & 0xFFFFFF00) == ((i << 8) | 0x30088000) )
//         {
//             sprintf(it3Path, (const char*)0xB8FCD8, v15, v15 + 80); //"data/monster/ch%x/ch%x.it3"
//             (this->*StubLoadMonsterIt3)(MSData->CharPosition, 0, it3Path);
//             break;
//             //sub_673FB3(i + 8, 0, &Dest);
//         }
//     }
//     (this->*StubLoadMSFile)(MSData, MSFileIndex);
// }


// NAKED VOID CBattle::NakedAS_8D_5F()
// {
//     INLINE_ASM
//     {
//         mov     ecx, [ebp - 0Ch];
//         push	[ebp + 08h];
//         call    CBattle::AS_8D_5F;
//         push	009D3675h;
//         retn;
//     }
// }

// VOID THISCALL CBattle::AS_8D_5F(PMONSTER_STATUS ChrMSData)
// {
//     int i, start, end;
//     PMONSTER_STATUS MSData;

//     if (ChrMSData->CharPosition >= 8 && ChrMSData->CharPosition < 0x10)
//     {
//         start = 0;
//         end = 4;
//     }
//     else
//     {
//         start = 8;
//         end = 0x10;
//     }

//     for (i=start; i<end; i++)
//     {
//         MSData = this->GetMonsterStatus()+i;
//         if (MSData->State & 0x8000)
//             continue;
//         UnsetCondition(MSData, 0x8000);
//         SetCondition(MSData, 0, 0x8000, 0, 0);
//     }
// }


NAKED VOID FurnitureCompletionRate()
{
    INLINE_ASM
    {
        MOV DWORD PTR SS:[EBP-0x3C],0x9E0;
        MOV DWORD PTR SS:[EBP-0x24],0x1B; //sum
        ret;
    }
}

NAKED VOID MedalReturn()
{
    INLINE_ASM
    {
        MOV ECX,DWORD PTR DS:[EAX+0x45D4]; //backup
        MOV EDX,DWORD PTR DS:[EAX+0x45D0]; //now
        add edx,ecx;
        cmp edx,0xF423F;
        jle MedalReturn01;
        mov edx,0xF423F;
MedalReturn01:
        MOV DWORD PTR DS:[EAX+0x45D0],EDX;
        xor ecx,ecx;
        MOV DWORD PTR DS:[EAX+0x45D4],ecx;
        ret;
    }
}

NAKED VOID TitleVisualCountSaveFix()
{
    INLINE_ASM
    {
        MOV WORD PTR DS:[ECX+0x7EDD6],DX;
        ret;
    }
}

BOOL THISCALL CScript::ScpLeaveParty(PSCENA_ENV_BLOCK Block)
{
    BOOL    Result;
    int     i;
    ULONG   ChrCount = 0;
    PUSHORT pPartyList = GetSaveData()->GetPartyList();

    Result = (this->*StubScpLeaveParty)(Block);

    for (i=0; i<8; i++)
    {
        if (*(pPartyList + i) != 0xFF)
        {
            ChrCount++;
        }
    }

    for (i=7; i>0; i--)
    {
        if (*(pPartyList + i) < 0xC)
        {
            GetSaveData()->SetChrPositionAuto(*(pPartyList + i), pPartyList, ChrCount);
        }
    }

    return Result;
}

ULONG THISCALL CScript::ScpGetFunctionAddress(ULONG_PTR pScena, ULONG function)
{
//    PrintConsoleA("%02d %s\r\n", function, (PCHAR)pScena);
    return (this->*StubScpGetFunctionAddress)(pScena, function);
}

NAKED VOID DecBurstEnergyWhenStandby()
{
    INLINE_ASM
    {
        mov cx,word ptr ds:[eax+0x176];
        pushad;
        push 0x14;
        mov eax,dword ptr ss:[ebp+0x8];
        push eax;
        mov ecx,dword ptr ss:[ebp-0x8];
        mov eax, 0x6775dc;
        call eax;
        popad;
        retn;
    }
}

//
NAKED VOID CBattle::NakedCheckAliveBeforeHeal()
{
    INLINE_ASM
    {
        mov edx,eax;
        mov ecx,DWORD PTR SS:[EBP-0x8];
        jmp CBattle::CheckAliveBeforeHeal;
    }
}

VOID FASTCALL CBattle::CheckAliveBeforeHeal(ULONG CharPosition)
{
    if (CharPosition < 0x12)
    {
        if ((GetMonsterStatus() + CharPosition)->ChrStatus[BattleStatusFinal].InitialHP != 0)
        {
            return;
        }
    }
    *(PVOID *)_AddressOfReturnAddress() = (PVOID)0x997D3A;
}


VOID THISCALL CBattle::SubHPEveryAct2WhenAttack(PMONSTER_STATUS dst, PCHAR_STATUS pChrStatusFinal, INT HP)
{
    UNREFERENCED_PARAMETER(pChrStatusFinal);
    SET_FLAG(g_flag, NFLAG::IsSubHPEveryAct);
    SubHPWhenAttack(dst, HP);
    CLEAR_FLAG(g_flag, NFLAG::IsSubHPEveryAct);
    *(PVOID *)_AddressOfReturnAddress() = (PVOID)0x9E3EEF;
}

NAKED VOID CBattle::NakedHandleConditionBeforeMasterQuartzKipaTakeEffect()
{
    INLINE_ASM
    {
        MOV EDX,DWORD PTR SS:[EBP-0x2C]
        MOV ECX,DWORD PTR SS:[EBP-0x8];
        jmp CBattle::HandleConditionBeforeMasterQuartzKipaTakeEffect;
    }
}

VOID FASTCALL CBattle::HandleConditionBeforeMasterQuartzKipaTakeEffect(PMONSTER_STATUS MSData)
{
    PMS_EFFECT_INFO pEffectInfo;
    LOOP_ONCE
    {
        if (FLAG_ON(MSData->MasterQuartzUsedFlag, 0x20))
            continue;

        pEffectInfo = FindEffectInfoByConditionEx(MSData, CraftConditions::Vanish, 0, FALSE);
        if (pEffectInfo && pEffectInfo->CounterType == pEffectInfo->CounterTypes::Infinite && pEffectInfo->ATLeft == 0)
            continue;

        pEffectInfo = FindEffectInfoByConditionEx(MSData, CraftConditions::Landification);
        if (pEffectInfo)
            pEffectInfo->ATLeft = 9999;
        //SetCondition(MSData, nullptr, CraftConditions::Landification, 0, 0);

        return;
    }
    *(PVOID *)_AddressOfReturnAddress() = (PVOID)0x9838B5;
}

// function extend
PMS_EFFECT_INFO THISCALL CBattle::FindEffectInfoByConditionEx(PMONSTER_STATUS MSData, ULONG_PTR Condition, INT ConditionRateType /* = 0 */, BOOL IsCheckSum /* = TRUE */)
{
    if (IsCheckSum)
    {
        if (!FLAG_ON(MSData->ChrStatus[BattleStatusFinal].ConditionFlags, Condition))
            return nullptr;
    }

    PMS_EFFECT_INFO pEffectInfo;
    FOR_EACH(pEffectInfo, MSData->EffectInfo, 0x21)
    {
        if (!FLAG_ON(pEffectInfo->ConditionFlags, Condition))
            continue;
        if (ConditionRateType == 0 ||
            (ConditionRateType > 0 && pEffectInfo->ConditionRate > 0) ||
            (ConditionRateType < 0 && pEffectInfo->ConditionRate < 0))
        {
            return pEffectInfo;
        }
    }
    return nullptr;
}

PMS_EFFECT_INFO THISCALL CBattle::CheckConditionGreenPepperWhenThinkCraft(PMONSTER_STATUS MSData, ULONG_PTR Condition, INT ConditionRateType)
{
    PMS_EFFECT_INFO pEffectInfo;
    pEffectInfo = FindEffectInfoByConditionEx(MSData, CraftConditions::BodyAbnormal);
    if (pEffectInfo != nullptr && pEffectInfo->ConditionRate == CraftConditions::BodyAbnormal_GreenPepper)
        return pEffectInfo;
    else
        return FindEffectInfoByConditionEx(MSData, Condition, ConditionRateType);
}

BOOL THISCALL CBattle::IsNeedBattleEvaluationSuperKill(ULONG ChrPosition)
{
    if (FLAG_ON(g_flag, NFLAG::IsSubHPEveryAct))
    {
        CLEAR_FLAG(g_flag, NFLAG::IsSubHPEveryAct);
        return FALSE;
    }

    if (GetMonsterStatus()[ChrPosition].IsChrEnemyOnly())
        return TRUE;
    else
        return FALSE;
}

BOOL THISCALL CBattle::GetHitResult(PMONSTER_STATUS src, PMONSTER_STATUS dst)
{
    DECL_METHOD_POINTER_AND_INIT(CBattle, GetHitResult, 0x6799F4);
    if (src == dst && src != nullptr)
    {
        src->IsHitJudged[src->CharPosition] = FALSE;
    }
    return (this->*StubGetHitResult)(src, dst);
}

// 鬼屋按键
SHORT THISCALL CClass::HorrorCoasterFastExit(int vKey)
{
    UNREFERENCED_PARAMETER(vKey);
    //TYPE_OF(&GetAsyncKeyState) StubGetAsyncKeyState;
    //*(PULONG_PTR)&StubGetAsyncKeyState = *(PULONG_PTR)0xDD5A18;

    PULONG  pVar1 = (PULONG)PtrAdd(this, 0x40);
    //PULONG  pVar2 = (PULONG)PtrAdd(this, 0x54);
    if (*pVar1 == 0)
    {
        //*(PVOID *)_AddressOfReturnAddress() = (PVOID)0x00AA42AC;
        return 0;
    }

    if (FLAG_ON(StubGetAsyncKeyState(VK_CONTROL), 0x8000) && FLAG_ON_ALL(StubGetAsyncKeyState('Q'), 0x8001))
        return (SHORT)0x8001;

    return 0;
}

/************************************************************************
    Status Start
************************************************************************/

VOID THISCALL CBattle::SetBattleStatusFinalByDifficulty(PMONSTER_STATUS MSData)
{
    if(!MSData->IsChrEnemyOnly())
    {
        return;
    }

    PCHAR_STATUS pStatusSum = &MSData->ChrStatus[1];

    if (nDifficulty == 5)
    {
        SaturateConvert(&pStatusSum->HPMax, (INT64)pStatusSum->HPMax * sRate.HP_a / 1000 + sRate.HP_b);
        pStatusSum->HP = pStatusSum->HPMax;

        SaturateConvert(&pStatusSum->STR,   (INT64)pStatusSum->STR * sRate.STR_a / 1000 + sRate.STR_b);
        SaturateConvert(&pStatusSum->DEF,   (INT64)pStatusSum->DEF * sRate.DEF_a / 1000 + sRate.DEF_b);
        SaturateConvert(&pStatusSum->ATS,   (INT64)pStatusSum->ATS * sRate.ATS_a / 1000 + sRate.ATS_b);
        SaturateConvert(&pStatusSum->ADF,   (INT64)pStatusSum->ADF * sRate.ADF_a / 1000 + sRate.ADF_b);
        SaturateConvert(&pStatusSum->SPD,   (INT64)pStatusSum->SPD * sRate.SPD_a / 1000 + sRate.SPD_b);
        SaturateConvertEx(&pStatusSum->DEX, (INT64)pStatusSum->DEX * sRate.DEX_a / 1000 + sRate.DEX_b, (SHORT)0xCCC);
        SaturateConvertEx(&pStatusSum->AGL, (INT64)pStatusSum->AGL * sRate.AGL_a / 1000 + sRate.AGL_b, (SHORT)0xCCC);
        SaturateConvert(&pStatusSum->MOV,   (INT64)pStatusSum->MOV * sRate.MOV_a / 1000 + sRate.MOV_b);
    }
    else
    {
        if (pStatusSum->HP != pStatusSum->HPMax)
        {
            pStatusSum->HP = pStatusSum->HPMax;
        }
        (this->*StubSetBattleStatusFinalByDifficulty)(MSData);
    }

    SaturateConvertEx(&MSData->ChrStatus[1].DEXRate, (INT64)MSData->ChrStatus[1].DEXRate + statusRateUserDefined.DEXRate, (SHORT)999, (SHORT)-999);
    SaturateConvertEx(&MSData->ChrStatus[1].AGLRate, (INT64)MSData->ChrStatus[1].AGLRate + statusRateUserDefined.AGLRate, (SHORT)999, (SHORT)-999);

    ULONG   conditionAbnormal = 0x40008FFF;
    ULONG   conditionDown = 0x00FF0000;
    USHORT  conditionATDelay = 0x0800;

    if (statusRateUserDefined.ResistNone)
    {
        MSData->Resistance = 0;
        CLEAR_FLAG(MSData->State2, conditionATDelay);
    }
    if (statusRateUserDefined.ResistAbnormalCondition)
    {
        MSData->Resistance |= conditionAbnormal;
    }
    if (statusRateUserDefined.ResistAbilityDown)
    {
        MSData->Resistance |= conditionDown;
    }
    if (statusRateUserDefined.ResistATDelay)
    {
        SET_FLAG(MSData->State2, conditionATDelay);
    }
}

ULONG THISCALL EDAO::GetDifficulty()
{
    if (nDifficulty > 0 && nDifficulty < 5)
        return nDifficulty - 1;
    else
        return (this->*StubGetDifficulty)();
}

// !bArianrhodLimitKaijo 手册修复
ULONG_PTR StubCalcSpecificStatusByDifficulty;
ULONG CDECL CalcSpecificStatusByDifficulty(ULONG Type, ULONG Correction, ULONG StatusData, PULONG pMSFileIndex)
{
    if (!bArianrhodLimitKaijo && *pMSFileIndex == 0x30004200)
    {
        return StatusData;
    }
    else
        return ((TYPE_OF(&CalcSpecificStatusByDifficulty))StubCalcSpecificStatusByDifficulty)(Type, Correction, StatusData, pMSFileIndex);
}

VOID THISCALL EDAO::SetBattleStatusFinalWhenRecover(ULONG ChrPosition, PCHAR_STATUS pStatusFinal, PCHAR_STATUS pStatusBasic)
{
    // CBattle* Battle = GetBattle();
    // PMONSTER_STATUS MSData = container_of(pStatusBasic, MONSTER_STATUS, ChrStatus);

    // SetBattleStatusFinalByEquipment(ChrPosition, pStatusFinal, pStatusBasic);
    // if (IsCustomChar(MSData->CharID) && Battle->pChrStatusBackup != nullptr)
    // {
    //     PCHAR_STATUS Raw = Battle->pChrStatusBackup + ChrPosition;
    //     PCHAR_STATUS Final = &MSData->ChrStatus[BattleStatusFinal];

    //     Final->MaximumHP    += Raw->MaximumHP   / 2;
    //     Final->InitialHP    += Raw->InitialHP   / 2;
    //     Final->MaximumEP    += Raw->MaximumEP   / 2;
    //     Final->InitialEP    += Raw->InitialEP   / 2;
    //     Final->STR          += Raw->STR         * 2 / 3;
    //     Final->DEF          += Raw->DEF         * 2 / 3;
    //     Final->ATS          += Raw->ATS         * 2 / 3;
    //     Final->ADF          += Raw->ADF         * 2 / 3;
    //     Final->DEX          += Raw->DEX         * 2 / 3;
    //     Final->AGL          += Raw->AGL         * 2 / 3;
    //     Final->MOV          += Raw->MOV         * 2 / 3;
    //     Final->SPD          += Raw->SPD         * 2 / 3;
    //     Final->RNG           = Raw->RNG + 1;
    // }
    // Battle->SetBattleStatusFinalByDifficulty(MSData);
}

PCHAR_T_STATUS EDAO::CalcChrT_StatusNew(PCHAR_T_STATUS pStatus, ULONG ChrNo, ULONG Level)
{
    //PCHAR_T_STATUS pStatus;
    if (!pStatus)
        return pStatus;

    CHAR_T_STATUS_RatioX Status;

    if (ChrNo >= MAXIMUM_CHR_NUMBER_WITH_STATUS)
        return nullptr;

    if (Level > STATUS_LEVEL_MAX)
    {
        Level = STATUS_LEVEL_MAX;
    }
    else if (Level < STATUS_LEVEL_MIN)
    {
        Level = STATUS_LEVEL_MIN;
    }

    using namespace T_NAME;

#if 0 //slow
    for (ULONG j=0; j<countof(StatusName); ++j)
    {
        ((PFLOAT)&Status.HP)[j] = (float)((PINT)&RatioY[ChrNo].HP)[j];
    }
#else
    Status.HP   = (float)RatioY[ChrNo].HP;
    Status.EP   = (float)RatioY[ChrNo].EP;
    Status.STR  = (float)RatioY[ChrNo].STR;
    Status.DEF  = (float)RatioY[ChrNo].DEF;
    Status.ATS  = (float)RatioY[ChrNo].ATS;
    Status.ADF  = (float)RatioY[ChrNo].ADF;
    Status.DEX  = (float)RatioY[ChrNo].DEX;
    Status.AGL  = (float)RatioY[ChrNo].AGL;
    Status.MOV  = (float)RatioY[ChrNo].MOV;
    Status.SPD  = (float)RatioY[ChrNo].SPD;
    Status.DEXRate  = (float)RatioY[ChrNo].DEXRate;
    Status.AGLRate  = (float)RatioY[ChrNo].AGLRate;
    Status.RNG      = (float)RatioY[ChrNo].RNG;
#endif

    for (ULONG i = 1; i < Level + 16; ++i)
    //int i = (Level + 15) * (Level + 16) / 2;
    {
        Status.HP   += (float)i * RatioX[ChrNo].HP;
        // fild fmul fstp fld fadd fstp ...
        Status.STR  += (float)((float)i * RatioX[ChrNo].STR);
    }
    {
        ULONG i = (Level + 15) * (Level + 16) / 2;
        Status.EP   += (float)i * RatioX[ChrNo].EP;
        Status.DEF  += (float)i * RatioX[ChrNo].DEF;
        Status.ATS  += (float)i * RatioX[ChrNo].ATS;
        Status.ADF  += (float)i * RatioX[ChrNo].ADF;
        Status.DEX  += (float)i * RatioX[ChrNo].DEX;
        Status.AGL  += (float)i * RatioX[ChrNo].AGL;
        Status.MOV  += (float)i * RatioX[ChrNo].MOV;
        Status.SPD  += (float)i * RatioX[ChrNo].SPD;
        Status.DEXRate  += (float)i * RatioX[ChrNo].DEXRate;
        Status.AGLRate  += (float)i * RatioX[ChrNo].AGLRate;
        Status.RNG      += (float)i * RatioX[ChrNo].RNG;
    }

    //pStatus = *(PCHAR_T_STATUS*)PtrAdd(this, 0xA2FA4);
    //ZeroMemory(pStatus, sizeof(*pStatus));
    pStatus->Level      = 0;
    //pStatus->EP         = 0;
    SaturateConvertEx(&pStatus->HP, Status.HP, (UINT)99999999);
    SaturateConvertEx(&pStatus->EP, Status.EP, (USHORT)20000);
    SaturateConvertEx(&pStatus->STR, Status.STR, (SHORT)20000);
    SaturateConvertEx(&pStatus->DEF, Status.DEF, (SHORT)20000);
    SaturateConvertEx(&pStatus->ATS, Status.ATS, (SHORT)20000);
    SaturateConvertEx(&pStatus->ADF, Status.ADF, (SHORT)20000);
    SaturateConvertEx(&pStatus->DEX, Status.DEX, (SHORT)0xCCC);
    SaturateConvertEx(&pStatus->AGL, Status.AGL, (SHORT)0xCCC);
    SaturateConvertEx(&pStatus->MOV, Status.MOV, (SHORT)20000);
    SaturateConvertEx(&pStatus->SPD, Status.SPD, (SHORT)20000);
    SaturateConvertEx(&pStatus->DEXRate, Status.DEXRate, (SHORT)20000);
    SaturateConvertEx(&pStatus->AGLRate, Status.AGLRate, (SHORT)20000);
    SaturateConvertEx(&pStatus->RNG, Status.RNG, (USHORT)0x64);
    return pStatus;
}

VOID THISCALL EDAO::CalcChrRawStatusFromLevelNew(ULONG ChrId, ULONG Level, ULONG Unknown /* = 0 */)
{
    (this->*StubCalcChrRawStatusFromLevel)(ChrId, Level, Unknown);
    if (ChrId < MAXIMUM_CHR_NUMBER_WITH_STATUS)
    {
        CSSaveData* SaveData = this->GetSaveData();
        SaveData->ChrStatus[ChrId].DEXRate = EDAO::ChrT_Status.DEXRate;
        SaveData->ChrStatus[ChrId].RNG     = EDAO::ChrT_Status.RNG;
    }
}

PCHAR_STATUS THISCALL EDAO::CalcChrRawStatusByFinalStatus(PCHAR_STATUS RawStatus, ULONG ChrID, PCHAR_STATUS FinalStatus)
{
    PCHAR_STATUS Result = (this->*StubCalcChrRawStatusByFinalStatus)(RawStatus, ChrID, FinalStatus);
    Result->RNG = EDAO::ChrT_Status.RNG;
    return Result;
}

PCHAR_T_STATUS THISCALL EDAO::CalcChrT_Status(ULONG ChrNo, ULONG Level)
{
    //PCHAR_T_STATUS pStatus = *(PCHAR_T_STATUS*)PtrAdd(this, 0xA2FA4);
    PCHAR_T_STATUS pStatus = &EDAO::ChrT_Status;

#if CONSOLE_DEBUG
    static ULONG TestCount = 11;
    if (TestCount)
    {
        QueryPerformanceCounter(&lStartCounter);
        for (int i=0; i<MAXIMUM_CHR_NUMBER_WITH_STATUS; ++i)
        {
            for (int j=STATUS_LEVEL_MIN; j<=STATUS_LEVEL_MAX; j++)
                (this->*StubCalcChrT_Status)(i%(MAXIMUM_CHR_NUMBER_WITH_STATUS-1), j);
        }
        QueryPerformanceCounter(&lStopCounter);
        PrintConsoleW(L"Elapsed time pre: %lf ms\n", (lStopCounter.QuadPart - lStartCounter.QuadPart) * 1000.0 / lFrequency.QuadPart);

        QueryPerformanceCounter(&lStartCounter);
        for (int i=0; i<MAXIMUM_CHR_NUMBER_WITH_STATUS; ++i)
        {
            for (int j=STATUS_LEVEL_MIN; j<=STATUS_LEVEL_MAX; j++)
                CalcChrT_StatusNew(pStatus, i%(MAXIMUM_CHR_NUMBER_WITH_STATUS-1), j);
        }
        QueryPerformanceCounter(&lStopCounter);
        PrintConsoleW(L"Elapsed time new: %lf ms\n", (lStopCounter.QuadPart - lStartCounter.QuadPart) * 1000.0 / lFrequency.QuadPart);
        TestCount--;
    }

#if 1
    //CHAR_T_STATUS       Status;
    CHAR_T_STATUS_ORG   StatusOld;
    PCHAR_T_STATUS      pStatusOld;
    static BOOL IsCompared = FALSE;
    if (!IsCompared)
    {
        IsCompared = TRUE;
        for (int i=0; i<MAXIMUM_CHR_NUMBER_WITH_STATUS; ++i)
        {
            for (int j=STATUS_LEVEL_MIN; j<=STATUS_LEVEL_MAX; j++)
            {
                pStatusOld = (this->*StubCalcChrT_Status)(i, j);
                CopyMemory(&StatusOld, pStatusOld, sizeof(CHAR_T_STATUS_ORG));
                ((CHAR_T_STATUS&)StatusOld).HP = StatusOld.HP;
                pStatus = CalcChrT_StatusNew(pStatus, i, j);

                // arch:IA32 arch:SSE arch:SSE2下完全相同
                if(RtlCompareMemory(&StatusOld, pStatus, sizeof(CHAR_T_STATUS_ORG)) != sizeof(CHAR_T_STATUS_ORG))
                {
                    // AllocConsole();
                    // PrintConsoleW(L"Not Same: Chr:%d Level:%d\n", i, j);
                    pStatusOld = (PCHAR_T_STATUS)&StatusOld;
                    // PrintConsoleW(L"%d %d %d %d %d %d %d %d %d %d %d\n", pStatusOld->HP, pStatusOld->EP, pStatusOld->STR, pStatusOld->DEF, pStatusOld->ATS, pStatusOld->ADF, pStatusOld->DEX, pStatusOld->AGL, pStatusOld->AGLRate, pStatusOld->MOV, pStatusOld->SPD);
                    pStatusOld = pStatus;
                    // PrintConsoleW(L"%d %d %d %d %d %d %d %d %d %d %d\n", pStatusOld->HP, pStatusOld->EP, pStatusOld->STR, pStatusOld->DEF, pStatusOld->ATS, pStatusOld->ADF, pStatusOld->DEX, pStatusOld->AGL, pStatusOld->AGLRate, pStatusOld->MOV, pStatusOld->SPD);
 
                }
            }
        }
    }
#endif
#endif

    return CalcChrT_StatusNew(pStatus, ChrNo, Level);
}

BOOL DumpChrRawStatusUnicode(LPCWSTR FileName)
{
    CHAR_T_STATUS   Status;
    NtFileDisk file;
    WCHAR Buffer[16<<10];
    WCHAR* p;

    if (!FileName)
        return FALSE;

    if (!NT_SUCCESS(file.Create(FileName)))
        return FALSE;

    using namespace T_NAME;

    ULONG BOM = BOM_UTF16_LE;
    file.Write(&BOM, 2);
    for (int i=0; i<CHR_COUNT; i++)
    {
        p = Buffer;
        p += swprintf(p, L"%s\r\n", lpwChrNameChs[i]);
        p += NCSTRCPY(p, L"Level\t");
        p += NCSTRCPY(p, L"HP\tEP\tSTR\tDEF\tATS\tADF\tDEX\tAGL\tMOV\tSPD\tDEXRate\tAGLRate\tRNG\r\n");
        for (int Level=STATUS_LEVEL_MIN; Level<=STATUS_LEVEL_MAX; Level++)
        {
            EDAO::CalcChrT_StatusNew(&Status, i, Level);
            p+= swprintf(p, L"%u\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
                Level, (UINT)Status.HP, (USHORT)Status.EP,
                (SHORT)Status.STR, (SHORT)Status.DEF,
                (SHORT)Status.ATS, (SHORT)Status.ADF,
                (SHORT)Status.DEX, (SHORT)Status.AGL,
                (SHORT)Status.MOV, (SHORT)Status.SPD,
                (SHORT)Status.DEXRate, (SHORT)Status.AGLRate,
                (USHORT)Status.RNG);
        }
        p+= NCSTRCPY(p, L"\r\n");

        file.Seek(0ll, SEEK_END);
        file.Write(Buffer, (char*)p - (char*)Buffer);
    }
    file.Seek(-4, SEEK_CUR);
    file.SetEndOfFile();
    file.Close();

    return TRUE;
}

//...
BOOL DumpChrRawStatusAnsi(LPCWSTR FileName)
{
    CHAR_T_STATUS   Status;
    NtFileDisk file;
    CHAR Buffer[16<<10];
    CHAR* p;

    if (!FileName)
        return FALSE;

    if (!NT_SUCCESS(file.Create(FileName)))
        return FALSE;

    using namespace T_NAME;

    for (int i=0; i<CHR_COUNT; i++)
    {
        p = Buffer;
        p += sprintf(p, "%s\r\n", lpChrNameChs[i]);
        p += NCSTRCPY(p, "Level\t");
        p += NCSTRCPY(p, "HP\tEP\tSTR\tDEF\tATS\tADF\tDEX\tAGL\tMOV\tSPD\tDEXRate\tAGLRate\tRNG\r\n");
        for (int Level=STATUS_LEVEL_MIN; Level<=STATUS_LEVEL_MAX; Level++)
        {
            EDAO::CalcChrT_StatusNew(&Status, i, Level);
            p+= sprintf(p, "%u\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
                Level, (UINT)Status.HP, (USHORT)Status.EP,
                (SHORT)Status.STR, (SHORT)Status.DEF,
                (SHORT)Status.ATS, (SHORT)Status.ADF,
                (SHORT)Status.DEX, (SHORT)Status.AGL,
                (SHORT)Status.MOV, (SHORT)Status.SPD,
                (SHORT)Status.DEXRate, (SHORT)Status.AGLRate,
                (USHORT)Status.RNG);
        }
        p+= NCSTRCPY(p, "\r\n");

        file.Seek(0ll, SEEK_END);
        file.Write(Buffer, (char*)p - (char*)Buffer);
    }
    file.Seek(-2, SEEK_CUR);
    file.SetEndOfFile();
    file.Close();

    return TRUE;
}

BOOL CHAR_T_STATUS_Ratio_To_Formula_Ansi(LPCWSTR FileName)
{
    CHAR_T_STATUS   Status;
    NtFileDisk file;
    CHAR Buffer[16<<10];
    CHAR* p;

    if (!FileName)
        return FALSE;

    if (!NT_SUCCESS(file.Create(FileName)))
        return FALSE;

    using namespace T_NAME;

    p = Buffer;
    p += NCSTRCPY(p, "碧之轨迹 角色能力值 计算公式\r\n\r\nLv = 角色等级\r\na = (Lv+15)(Lv+16)/2\r\n\r\n");
    p += NCSTRCPY(p, "Name\t");
    p += NCSTRCPY(p, "HP\tEP\tSTR\tDEF\tATS\tADF\tDEX\tAGL\tMOV\tSPD\tDEXRate\tAGLRate\tRNG\r\n");

    for (int i=0; i<CHR_COUNT; i++)
    {
        /*
        p += sprintf(p, "%s\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\t%ga+%d\r\n",
            lpChrNameChs[i],
            RatioX[i].HP, RatioY[i].HP,
            RatioX[i].EP, RatioY[i].EP,
            RatioX[i].STR, RatioY[i].STR,
            RatioX[i].DEF, RatioY[i].DEF,
            RatioX[i].ATS, RatioY[i].ATS,
            RatioX[i].ADF, RatioY[i].ADF,
            RatioX[i].DEX, RatioY[i].DEX,
            RatioX[i].AGL, RatioY[i].AGL,
            RatioX[i].MOV, RatioY[i].MOV,
            RatioX[i].SPD, RatioY[i].SPD,
            RatioX[i].DEXRate, RatioY[i].DEXRate,
            RatioX[i].AGLRate, RatioY[i].AGLRate,
            RatioX[i].RNG, RatioY[i].RNG
            );*/
        p += sprintf(p, "%s", lpChrNameChs[i]);
        for (int j=0; j<countof(StatusName); j++)
        {
            p += NCSTRCPY(p, "\t");
            p += sprintfDoubleNoZero(p, (double)((PFLOAT)&RatioX[i].HP)[j]);
            p += sprintf(p, "a+%d", ((PINT)&RatioY[i].HP)[j]);
        }
        p += NCSTRCPY(p, "\r\n");
    }
    file.Write(Buffer, (char*)p - (char*)Buffer);
    file.Close();

    return TRUE;
}

//...ugly
BOOL CHAR_T_STATUS_Ratio_To_Formula_HTML(LPCWSTR FileName)
{
    CHAR_T_STATUS   Status;
    NtFileDisk file;
    CHAR Buffer[16<<10];
    CHAR* p;

    if (!FileName)
        return FALSE;

    if (!NT_SUCCESS(file.Create(FileName)))
        return FALSE;

    using namespace T_NAME;

    static
    CHAR HtmlHead[] = "<html>\r\n<head>\r\n"
        "<meta http-equiv=\"content-type\" content=\"text/html; charset=gbk\">\r\n"
        "<title>碧之轨迹 角色能力值 计算公式</title>\r\n\r\n"
        "<style type=\"text/css\">\r\n"
        ".table1 {background:#585858;}\r\n"
        ".table1 td, .table1 th {background:#FFFFFF;}\r\n"
        "</style>\r\n</head>\r\n\r\n"
        "<body>\r\nLv = 角色等级<br>a = (Lv+15)(Lv+16)/2<br>\r\n";

    file.Write(HtmlHead, CONST_STRLEN(HtmlHead));

    p = Buffer;

    // table start
    p += NCSTRCPY(p, "<table cellpadding=\"2\" cellspacing=\"1\" border=\"0\" class=\"table1\">\r\n<caption>碧之轨迹 角色能力值 计算公式</caption>\r\n");
    p += NCSTRCPY(p, "<tr>");
    p += sprintf(p, "<th>%s</th>", "Name");
    for (int j=0; j<countof(StatusName); j++)
    {
        p += sprintf(p, "<th>%s</th>", StatusName[j]);
    }
    p += NCSTRCPY(p, "</tr>\r\n");

    for (int i=0; i<CHR_COUNT; i++)
    {
        p += NCSTRCPY(p, "<tr>");
        p += sprintf(p, "<td>%s</td>", lpChrNameChs[i]);
        for (int j=0; j<countof(StatusName); j++)
        {
            p += NCSTRCPY(p, "<td>");
            p += sprintfDoubleNoZero(p, (double)((PFLOAT)&RatioX[i].HP)[j]);
            p += sprintf(p, "a+%d", ((PINT)&RatioY[i].HP)[j]);
            p += NCSTRCPY(p, "</td>");
        }
        p += NCSTRCPY(p, "</tr>\r\n");
    }
    p += NCSTRCPY(p, "</table>\r\n\r\n");
    // table end

    p += NCSTRCPY(p, "<br><br>\r\n");
    p += NCSTRCPY(p, "<table cellpadding=\"1\" cellspacing=\"1\" border=\"1\" class=\"table_\">\r\n"
        "<tr><th>Lv</th><td>角色等级</td></tr>\r\n"
        "<tr><th>a</th><td>(Lv+15)(Lv+16)/2</td></tr>\r\n"
        "<tr><th>能力值</th><td>x·a + y</td></tr>\r\n"
        "</table>\r\n\r\n");

    // table start
    p += NCSTRCPY(p, "<table cellpadding=\"4\" cellspacing=\"1\" border=\"1\" class=\"table_\">\r\n<caption>碧之轨迹 角色能力值 计算公式</caption>\r\n");
    p += NCSTRCPY(p, "<tr>");
    p += sprintf(p, "<th rowspan=\"2\">%s</th>", "Name");
    for (int j=0; j<countof(StatusName); j++)
    {
        p += sprintf(p, "<th colspan=\"2\">%s</th>", StatusName[j]);
    }
    p += NCSTRCPY(p, "</tr>\r\n");

    p += NCSTRCPY(p, "<tr>");
    for (int j=0; j<countof(StatusName); j++)
    {
        p += NCSTRCPY(p, "<th>x</th><th>y</th>");
    }
    p += NCSTRCPY(p, "</tr>\r\n");

    for (int i=0; i<CHR_COUNT; i++)
    {
        p += NCSTRCPY(p, "<tr>");
        p += sprintf(p, "<td>%s</td>", lpChrNameChs[i]);
        for (int j=0; j<countof(StatusName); j++)
        {
            p += NCSTRCPY(p, "<td>");
            p += sprintfDoubleNoZero(p, (double)((PFLOAT)&RatioX[i].HP)[j]);
            p += NCSTRCPY(p, "</td>");

            p += NCSTRCPY(p, "<td>");
            p += sprintf(p, "%d", ((PINT)&RatioY[i].HP)[j]);
            p += NCSTRCPY(p, "</td>");
        }
        p += NCSTRCPY(p, "</tr>\r\n");
    }
    p += NCSTRCPY(p, "</table>\r\n\r\n");
    // table end

    p += NCSTRCPY(p, "<br><br>\r\n");
    p += NCSTRCPY(p, "</body>\r\n</html>");

    file.Write(Buffer, (char*)p - (char*)Buffer);
    file.Close();

    return TRUE;
}

// BOOL CHAR_T_STATUS_Ratio_To_Json(const char *filename)
// {
//     using namespace T_NAME;
//     INT Ret;

//     json_object *root_object = json_object_new_object();
//     json_object *chr_object;
//     json_object *status_object;

//     //if(0)
//     {
//         Ret = json_object_to_file_ext((char*)filename, root_object, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOZERO);
//         if (Ret == -1)
//             return FALSE;
//     }
//     //AllocConsole();

//     for (int i=0; i<CHR_COUNT; i++)
//     {
//         json_object_object_add(root_object, lpChrNameChs[i], chr_object = json_object_new_object());

//         for (int j=0; j<countof(StatusName); j++)
//         {
//             json_object_object_add(chr_object, StatusName[j], status_object = json_object_new_array());
//             json_object_array_add(status_object, json_object_new_double( ((PFLOAT)&RatioX[i].HP)[j] ));
//             json_object_array_add(status_object, json_object_new_int( ((PINT)&RatioY[i].HP)[j] ));
//         }
//         //json_object_object_del(chr_object, "EP");
//     }
//     //PrintConsoleA("%s\n", json_object_to_json_string(root_object));
//     Ret = json_object_to_file_ext((char*)filename, root_object, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOZERO);

//     json_object_put(root_object);

//     if (Ret == -1)
//         return FALSE;
//     else
//         return TRUE;
// }

// BOOL CHAR_T_STATUS_Ratio_From_Json(const char *filename)
// {
//     using namespace T_NAME;
//     json_object *root_object = json_object_from_file(filename);
//     json_object *chr_object;
//     json_object *status_object;
//     json_object *value_object[2];

//     if (!root_object)
//         return FALSE;

//     CHAR_T_STATUS_RatioX    RatioX1[CHR_COUNT];
//     CHAR_T_STATUS_RatioY    RatioY1[CHR_COUNT];
//     CopyMemory(RatioX1, RatioX, sizeof(RatioX));
//     CopyMemory(RatioY1, RatioY, sizeof(RatioY));

//     for (int i=0; i<CHR_COUNT; i++)
//     {
//         if (!json_object_object_get_ex(root_object, lpChrNameChs[i], &chr_object))
//             continue;

//         for (int j=0; j<countof(StatusName); j++)
//         {
//             if (json_object_object_get_ex(chr_object, StatusName[j], &status_object))
//             {
//                 value_object[0] = json_object_array_get_idx(status_object, 0);
//                 value_object[1] = json_object_array_get_idx(status_object, 1);
//                 if (value_object[0])
//                     ((PFLOAT)&RatioX1[i].HP)[j]  = (float)json_object_get_double(value_object[0]);
//                 if (value_object[1])
//                     ((PINT)&RatioY1[i].HP)[j]    = json_object_get_int(value_object[1]);
//             }
//         }
//     }
//     json_object_put(root_object);

// #if CONSOLE_DEBUG
// #if 0
//     PrintConsoleA("\nCHAR_T_STATUS_Ratio_From_Json Succeed!\n\n");
//     for (int i=0; i<CHR_COUNT; i++)
//     {
//         if (RtlCompareMemory(&RatioX[i], &RatioX1[i], sizeof(RatioX[i])) != sizeof(RatioX[i]))
//         {
//             PrintConsoleA("RatioX Not Same: %s\n", lpChrNameChs[i]);
//         }
//         if (RtlCompareMemory(&RatioY[i], &RatioY1[i], sizeof(RatioY[i])) != sizeof(RatioY[i]))
//         {
//             PrintConsoleA("RatioY Not Same: %s\n", lpChrNameChs[i]);
//         }
//     }
// #endif
// #endif
//     CopyMemory(RatioX, RatioX1, sizeof(RatioX));
//     CopyMemory(RatioY, RatioY1, sizeof(RatioY));
//     return TRUE;
// }
/************************************************************************
    Status End
************************************************************************/

BOOL THISCALL CGlobal::AddCraft(ULONG ChrId, ULONG Craft)
{
    if (ChrId == T_NAME::Yin && Craft == 0x16E && !GetEDAO()->GetSaveData()->IsYinRixia())
    {
        return FALSE;
    }
    return (this->*StubAddCraft)(ChrId, Craft);
}

BOOL THISCALL CBattle::IsChrCanTeamRush(PMONSTER_STATUS MSData, PCRAFT_INFO pCraft)
{
    BOOL Result = (this->*StubIsChrCanTeamRush)(MSData, pCraft);
    if (Result)
    {
        PAS_FILE pAS_File = GetASFile(MSData);
        if (pAS_File)
        {
            ULONG ActionCount = pAS_File->GetActionCount();
            if (!pAS_File->IsActionValid(BattleActionScript::SysCraft_TeamRushInit, ActionCount) ||
                !pAS_File->IsActionValid(BattleActionScript::SysCraft_TeamRushAction, ActionCount))
            {
                Result = FALSE;
            }
        }
        else
        {
            Result = FALSE;
        }
    }
    return Result;
}

BOOL THISCALL CBattle::CheckQuartz(ULONG ChrPosition, ULONG ItemId, PULONG EquippedIndex /* = nullptr */)
{
    switch (ItemId)
    {
    case 0xBA:  // 情报
        {
            LONG_PTR TargetIndex = GetCurrentTargetIndex();
            if (TargetIndex >=0 && TargetIndex < MAXIMUM_CHR_NUMBER_IN_BATTLE)
            {
                AnalyzeMonsInf(GetMonsterStatus()+TargetIndex);
            }
        }
    case 0xB8:  // 天眼
    case 0xB1:  // 龙眼
        if (EquippedIndex)
            *EquippedIndex = 1;
        return TRUE;
    }

    return (this->*StubCheckQuartz)(ChrPosition, ItemId, EquippedIndex);
}

NAKED VOID SetBattleEncountCondition()
{
    INLINE_ASM
    {
        mov eax, nBattleEncount;
        jmp StubCheckStackBalance;
    }
}

NAKED VOID SetBattleATBonus()
{
    INLINE_ASM
    {
        mov eax, nATBonus;
        jmp StubCheckStackBalance;
    }
}

NAKED VOID SepithUpLimitDisplay()
{
    INLINE_ASM
    {
        MOVZX EAX,WORD PTR DS:[ECX+EAX*2];
        cmp eax, 0x3E7;
        jle SepithUpLimitDisplay01;
        mov eax, 0x3E7;
SepithUpLimitDisplay01:
        cdq;
        ret;
    }
}

NAKED VOID SepithUpLimitDisplay1()
{
    INLINE_ASM
    {
        MOVZX EAX,WORD PTR DS:[ECX+EAX*2];
        CMP EAX,0x3E7;
        jle SHORT L00000001;
        MOV EAX,0x3E7;
L00000001:
        SHL EAX,1;
        RETN;
    }
}

NAKED VOID SepithUpLimitDisplay2()
{
    INLINE_ASM
    {
        MOVZX EDX,WORD PTR DS:[ECX+EAX*2]
        CMP EDX,0x3E7;
        jle SHORT L00000002;
        MOV EDX,0x3E7;
L00000002:
        SHL EDX,1;
        RETN;
    }
}

/************************************************************************
    Restore Start
************************************************************************/

NAKED VOID NakedGetUnderAttackVoiceChrIdRestore()
{
    INLINE_ASM
    {
        CMP DWORD PTR SS:[EBP-0xF8],0x0;
        ret;
    }
}

NAKED VOID NakedConditionalShowOrigAT()
{
    INLINE_ASM
    {
        MOV EAX,DWORD PTR SS:[EBP-0xC];
        cmp byte PTR DS:[EAX+0x6E], 6;  //0x73
        JG L00000001;
        push 0x9F67EB;
        retn;
L00000001:
        push ECX;
        FLDZ;
        FSTP DWORD PTR SS:[ESP];
        push 0x009F66E4;
	    retn;
    }
}
/************************************************************************
    Restore End
************************************************************************/


/************************************************************************
    Fish Start
************************************************************************/

class CFish
{
public:
    BOOL THISCALL IsRodPulled();
    ULONG_PTR THISCALL GetRodEntry(ULONG RodNo);
    VOID ChangeFishingWaitTime();

    //DECL_STATIC_METHOD_POINTER(CFish, IsRodPulled);
    //DECL_STATIC_METHOD_POINTER(CFish, GetRodEntry);
};

//INIT_STATIC_MEMBER(CFish::StubIsRodPulled);
//INIT_STATIC_MEMBER(CFish::StubGetRodEntry);

BOOL THISCALL CFish::IsRodPulled()
{
    DECL_METHOD_POINTER_AND_INIT(CFish, IsRodPulled, 0x6739C8);

    if (nAutoFish == 1)
        return TRUE;
    return (this->*StubIsRodPulled)();
}

ULONG_PTR THISCALL CFish::GetRodEntry(ULONG RodNo)
{
    DECL_METHOD_POINTER_AND_INIT(CFish, GetRodEntry, 0x67A697);

    ULONG_PTR pEntry = (this->*StubGetRodEntry)(RodNo);
    if (nAutoFish > 1 && RodNo < 5)
        *((PUSHORT)pEntry+1) = (USHORT)nAutoFish;
    return pEntry;
}

NAKED VOID CFish::ChangeFishingWaitTime()
{
    INLINE_ASM
    {
        mov edx, nFastFish;
        LEA ECX,DWORD PTR DS:[EAX+EDX];
        ret;
    }
}

/************************************************************************
    Fish End
************************************************************************/

VOID CClass::PokerRoyalFlush()
{
    POKER_ENTRY* Entry = (POKER_ENTRY*)this;

    Entry[0].digit = 6;
    Entry[1].digit = 2;
    Entry[2].digit = 7;
    Entry[3].digit = 3;
    Entry[4].digit = 8;
    Entry[5].digit = 4;

    Entry[6].digit = 9;
    Entry[7].digit = 10;
    Entry[8].digit = 11;
    Entry[9].digit = 12;
    Entry[10].digit = 13;
    Entry[11].digit = 1;

    Entry[12].digit = 5;

    //Swap(Entry[6].digit, Entry[12].digit);  // 7-13
    //Swap(Entry[7].digit, Entry[0].digit);   // 8-1
}

// log
VOID THISCALL CClass::ShowHorrorCoasterText(INT x, INT y, float par3, LPCSTR text, ULONG par5, ULONG color)
{
    // if (text != nullptr && toupper(*text) == 'E')
    //     PrintConsoleA("%d %d %f %s\r\n", x, y, par3, text);
    (this->*StubShowHorrorCoasterText)(x, y, par3, text, par5, color);
    //960*544
    //(this->*StubShowHorrorCoasterText)(236, 55, par3, text, par5, color);
}

// 文本显示位置错误
VOID THISCALL CClass::PositionPC2PSP(PFLOAT par1, Gdiplus::PointF *Target, PFLOAT par3)
{
    DECL_METHOD_POINTER_AND_INIT(CClass, PositionPC2PSP, 0x6724CE);

    EDAO *edao = EDAO::GlobalGetEDAO();

    (this->*StubPositionPC2PSP)(par1, Target, par3);
    Target->X *= PSP_WIDTH_F / edao->GetWindowSize()->cx;
    Target->Y *= PSP_HEIGHT_F / edao->GetWindowSize()->cy;
}

// Extra 鬼屋 结束后不返回Extra，BGM不切换，简体BUG，参照繁体版修复
NAKED VOID Return2ExtraFix()
{
    INLINE_ASM
    {
        mov eax,dword ptr ss:[ebp-0xc];
        cmp dword ptr ds:[eax+0xa70c8],0x0;
        je short LReturn2ExtraFix01;
        //mov eax,dword ptr ss:[ebp-0xc];
        mov ecx,dword ptr ds:[eax+0x82adc];
        //mov edx,0x7;
        //mov word ptr ds:[ecx+0x20],dx;
        mov word ptr ds:[ecx+0x20], 7;

LReturn2ExtraFix01:
        mov eax,0x1;
	    ret;
    }
}


BOOL THISCALL CGlobal::UseItemDouguOld(ULONG ItemId, ULONG ChrId)
{
    BOOL IsCanUse = TRUE;
    CHAR_STATUS *pChrStatus;

    ITEM_ENTRY ItemEntry;

    BYTE Effect;
    COORD EffectPar;

    BOOL Result;

    GetItemEntry(&ItemEntry, ItemId);

    pChrStatus = &GetEDAO()->GetSaveData()->ChrStatus[ChrId];

    LOOP_ONCE
    {
        if (FLAG_ON(pChrStatus->ConditionFlags, CraftConditions::Dead))
        {
            if ( !(ItemEntry.DOUGU.Misc & 0x20) )
            {
                IsCanUse = FALSE;
                break;
                //GetEDAO()->GetSound()->PlaySound(3);
                //return FALSE;
            }
            break;
        }

        if ( (ItemEntry.DOUGU.Effect1 == 27
            || ItemEntry.DOUGU.Effect2 == 27
            || ItemEntry.DOUGU.Effect1 == 66
            || ItemEntry.DOUGU.Effect2 == 66
            || ItemEntry.DOUGU.Effect1 == 51
            || ItemEntry.DOUGU.Effect2 == 51
            || ItemEntry.DOUGU.Effect1 == 50
            || ItemEntry.DOUGU.Effect2 == 50
            || ItemEntry.DOUGU.Effect1 == 73)
            && pChrStatus->InitialHP != pChrStatus->MaximumHP )
        {
            break;
        }

        if ( (ItemEntry.DOUGU.Effect1 == 28 || ItemEntry.DOUGU.Effect2 == 28 || ItemEntry.DOUGU.Effect1 == 67 || ItemEntry.DOUGU.Effect2 == 67)
            && pChrStatus->InitialEP != pChrStatus->MaximumEP )
        {
            break;
        }

        if ( (ItemEntry.DOUGU.Effect1 == 29
            || ItemEntry.DOUGU.Effect2 == 29
            || ItemEntry.DOUGU.Effect1 == 68
            || ItemEntry.DOUGU.Effect2 == 68
            || ItemEntry.DOUGU.Effect1 == 70
            || ItemEntry.DOUGU.Effect2 == 70
            || ItemEntry.DOUGU.Effect1 == 71
            || ItemEntry.DOUGU.Effect2 == 71)
            && pChrStatus->InitialCP != pChrStatus->MaximumCP )
        {
            break;
        }

        IsCanUse = FALSE;
    }

    if (IsCanUse)
    {
        SubItem(ItemId, 1);
        EffectPar.X = ItemEntry.DOUGU.Effect1Parameter;
        EffectPar.Y = ItemEntry.DOUGU.Effect1ST;
        Effect = ItemEntry.DOUGU.Effect1;
        Result = DoEffect(ACTION_ITEM, 255, ChrId, Effect, &EffectPar, 0, TRUE);

        EffectPar.X = ItemEntry.DOUGU.Effect2Parameter;
        EffectPar.Y = ItemEntry.DOUGU.Effect2ST;
        Effect = ItemEntry.DOUGU.Effect2;
        DoEffect(ACTION_ITEM, 255, ChrId, Effect, &EffectPar, 0, Result == FALSE);
        return TRUE;
    }
    else
    {
        GetEDAO()->GetSound()->PlaySound(3);
        return FALSE;
    }
}

BOOL THISCALL CGlobal::UseItemDouguFix(ULONG ItemId, ULONG ChrId)
{
    CHAR_STATUS *pChrStatus;

    ITEM_ENTRY ItemEntry;

    BYTE Effect;
    COORD EffectPar;

    BOOL IsCanUse = TRUE;
    BOOL IsSoundPlayed;
    BOOL Result = TRUE;

    GetItemEntry(&ItemEntry, ItemId);

    pChrStatus = &GetEDAO()->GetSaveData()->ChrStatus[ChrId];

    LOOP_ONCE
    {
        if (FLAG_ON(pChrStatus->ConditionFlags, CraftConditions::Dead))
        {
            if ( !(ItemEntry.DOUGU.Misc & 0x20) )
            {
                IsCanUse = FALSE;
                Result = FALSE;
                //break;
            }
            break;
        }

        if ( ( ItemEntry.DOUGU.Effect1 == 27 || ItemEntry.DOUGU.Effect2 == 27 || ItemEntry.DOUGU.Effect3 == 27
            || ItemEntry.DOUGU.Effect1 == 66 || ItemEntry.DOUGU.Effect2 == 66 || ItemEntry.DOUGU.Effect3 == 66
            || ItemEntry.DOUGU.Effect1 == 51 || ItemEntry.DOUGU.Effect2 == 51 || ItemEntry.DOUGU.Effect3 == 51
            || ItemEntry.DOUGU.Effect1 == 50 || ItemEntry.DOUGU.Effect2 == 50 || ItemEntry.DOUGU.Effect3 == 50
            || ItemEntry.DOUGU.Effect1 == 73 || ItemEntry.DOUGU.Effect2 == 73 || ItemEntry.DOUGU.Effect3 == 73 )
            && pChrStatus->InitialHP != pChrStatus->MaximumHP )
        {
            break;
        }

        if ( ( ItemEntry.DOUGU.Effect1 == 28 || ItemEntry.DOUGU.Effect2 == 28 || ItemEntry.DOUGU.Effect3 == 28
            || ItemEntry.DOUGU.Effect1 == 67 || ItemEntry.DOUGU.Effect2 == 67 || ItemEntry.DOUGU.Effect3 == 67 )
            && pChrStatus->InitialEP != pChrStatus->MaximumEP )
        {
            break;
        }

        if ( ( ItemEntry.DOUGU.Effect1 == 29 || ItemEntry.DOUGU.Effect2 == 29 || ItemEntry.DOUGU.Effect3 == 29
            || ItemEntry.DOUGU.Effect1 == 68 || ItemEntry.DOUGU.Effect2 == 68 || ItemEntry.DOUGU.Effect3 == 68
            || ItemEntry.DOUGU.Effect1 == 70 || ItemEntry.DOUGU.Effect2 == 70 || ItemEntry.DOUGU.Effect3 == 70
            || ItemEntry.DOUGU.Effect1 == 71 || ItemEntry.DOUGU.Effect2 == 71 || ItemEntry.DOUGU.Effect3 == 71 )
            && pChrStatus->InitialCP != pChrStatus->MaximumCP )
        {
            break;
        }

        IsCanUse = FALSE;
    }

    if (IsCanUse)
    {
        SubItem(ItemId, 1);
        EffectPar.X = ItemEntry.DOUGU.Effect1Parameter;
        EffectPar.Y = ItemEntry.DOUGU.Effect1ST;
        Effect = ItemEntry.DOUGU.Effect1;
        IsSoundPlayed = DoEffect(ACTION_ITEM, 255, ChrId, Effect, &EffectPar, 0, TRUE);

        EffectPar.X = ItemEntry.DOUGU.Effect2Parameter;
        EffectPar.Y = ItemEntry.DOUGU.Effect2ST;
        Effect = ItemEntry.DOUGU.Effect2;
        IsSoundPlayed |= DoEffect(ACTION_ITEM, 255, ChrId, Effect, &EffectPar, 0, !IsSoundPlayed);

        EffectPar.X = ItemEntry.DOUGU.Effect3Parameter;
        EffectPar.Y = ItemEntry.DOUGU.Effect3ST;
        Effect = ItemEntry.DOUGU.Effect3;
        DoEffect(ACTION_ITEM, 255, ChrId, Effect, &EffectPar, 0, !IsSoundPlayed);
    }
    else
    {
        GetEDAO()->GetSound()->PlaySound(3);
    }

    return Result;
}

NAKED VOID NakedConfuseSelfBugFix()
{
    INLINE_ASM
    {
        pop     edx;
        xor     ecx, ecx;
	    mov     eax, [ebp + 0x8];
        cmp     eax, [ebp + 0xC];
        je      self;
        inc     ecx;
    self:
        push    ecx;
        push    edx;
        retn;
    }
}

NAKED VOID NakedMovUpMaxFix()
{
    INLINE_ASM
    {
        movzx   eax, word ptr [ebp-0x180];
        cmp     word ptr [ebp+0x18], ax;
        retn;
    }
}

NAKED VOID NakedMovDownMaxFix()
{
    INLINE_ASM
    {
        movzx   eax, word ptr [ebp-0x180];
        neg     eax;
        cmp     word ptr [ebp+0x18], ax;
        retn
    }
}