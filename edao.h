#ifndef _EDAO_H_5c8a3013_4334_4138_9413_3d0209da878e_
#define _EDAO_H_5c8a3013_4334_4138_9413_3d0209da878e_

#define DIRECTINPUT_VERSION 0x800

#include "ml/mylibrary.h"
// #include "Aki.h"
#include <GdiPlus.h>
#include <dinput.h>
//TODO: figure out what this actually is
#define FLAG_ON_ALL FLAG_ON

#if D3D9_VER
    #define NtGetTickCount (ULONG64)GetTickCount
#endif

#if 0
    #undef DebugPrint
    #define DebugPrint(...) { AllocConsole(); PrintConsoleW(__VA_ARGS__); PrintConsoleW(L"\n"); }
#endif

#pragma warning (disable: 4201)
#pragma warning (disable: 4996)

ML_OVERLOAD_NEW

class EDAO;
class CGlobal;
class CScript;
class CSSaveData;
class CBattle;

class CMap;
class CInput;
class CSound;
class CCamera;
class CMiniGame;
class CDebug;

#define CActor CSSaveData

#define INIT_STATIC_MEMBER(x) DECL_SELECTANY TYPE_OF(x) x = nullptr
#define DECL_STATIC_METHOD_POINTER(cls, method) static TYPE_OF(&cls::method) Stub##method
#define DETOUR_METHOD(cls, method, addr, ...) TYPE_OF(&cls::method) (method); *(PULONG_PTR)&(method) = addr; return (this->*method)(__VA_ARGS__)
#define DECL_METHOD_POINTER_AND_INIT(cls, method, addr) TYPE_OF(&cls::method) Stub##method; *(PULONG_PTR)&(Stub##method) = addr
#define READONLY_PROPERTY(type, var) __declspec(property(get = Get##var)) type var; type Get##var()

#define MINIMUM_CUSTOM_CHAR_ID          0xB0
#define MINIMUM_CUSTOM_CRAFT_INDEX      0x3E8
#define MAXIMUM_CHR_NUMBER_IN_BATTLE    0x16
#define MAXIMUM_CHR_NUMBER_WITH_STATUS  0xC
#define SCENA_FLAG_SIZE                 0x220
#define AVATAR_CHR_POSITION             0x14

#define STATUS_LEVEL_MIN       45
#define STATUS_LEVEL_MAX       150

#define PSP_WIDTH_F                       480.f
#define PSP_HEIGHT_F                      272.f

BOOL Initialize_ex(PVOID BaseAddress);

DECL_SELECTANY TYPE_OF(&GetAsyncKeyState) StubGetAsyncKeyState;

#pragma pack(push, 1)

typedef struct
{
    ULONG Magic;
    ULONG CompressedSize;

} UCL_COMPRESS_HEADER;


ML_NAMESPACE_BEGIN(CraftConditions)

    static const ULONG_PTR Poison              = 0x00000001;
    static const ULONG_PTR Frozen              = 0x00000002;
    static const ULONG_PTR Landification       = 0x00000004;
    static const ULONG_PTR Sleeping            = 0x00000008;
    static const ULONG_PTR BanArts             = 0x00000010;
    static const ULONG_PTR Darkness            = 0x00000020;
    static const ULONG_PTR BanCraft            = 0x00000040;
    static const ULONG_PTR Confusion           = 0x00000080;
    static const ULONG_PTR Stun                = 0x00000100;
    static const ULONG_PTR OnehitKill          = 0x00000200;
    static const ULONG_PTR Burning             = 0x00000400;
    static const ULONG_PTR Rage                = 0x00000800;
    static const ULONG_PTR ArtsGuard           = 0x00001000;
    static const ULONG_PTR CraftGuard          = 0x00002000;

    static const ULONG_PTR MaxGuard            = 0x00004000;
    static const ULONG_PTR Vanish              = 0x00008000;
    static const ULONG_PTR StrUp               = 0x00010000;
    static const ULONG_PTR DefUp               = 0x00020000;
    static const ULONG_PTR AtsUp               = 0x00040000;
    static const ULONG_PTR AdfUp               = 0x00080000;
    static const ULONG_PTR DexUp               = 0x00100000;
    static const ULONG_PTR AglUp               = 0x00200000;
    static const ULONG_PTR MovUp               = 0x00400000;
    static const ULONG_PTR SpdUp               = 0x00800000;
    static const ULONG_PTR HPRecovery          = 0x01000000;
    static const ULONG_PTR CPRecovery          = 0x02000000;

    static const ULONG_PTR Stealth             = 0x04000000;
    static const ULONG_PTR ArtsReflect         = 0x08000000;
    static const ULONG_PTR Reserve_1           = 0x10000000;
    static const ULONG_PTR Boost               = 0x10000000;
    static const ULONG_PTR BurningHeart        = 0x10000000;
    static const ULONG_PTR Reserve_2           = 0x20000000;
    static const ULONG_PTR CraftReflect        = 0x20000000;
    static const ULONG_PTR Reserve_3           = 0x40000000;
    static const ULONG_PTR GreenPepper         = 0x40000000;
    static const ULONG_PTR BodyAbnormal        = 0x40000000;

    static const ULONG_PTR Dead                = 0x80000000;

    static const SHORT BodyAbnormal_Little = 0x2;
    static const SHORT BodyAbnormal_GreenPepper = 0x3;

ML_NAMESPACE_END

ML_NAMESPACE_BEGIN(BattleActionScript)
enum
{
    SysCraft_Init           = 0x00,
    SysCraft_Stand          = 0x01,
    SysCraft_Move           = 0x02,
    SysCraft_UnderAttack    = 0x03,
    SysCraft_Dead           = 0x04,
    SysCraft_NormalAttack   = 0x05,
    SysCraft_ArtsAria       = 0x06,
    SysCraft_ArtsCast       = 0x07,
    SysCraft_Win            = 0x08,
    SysCraft_EnterBattle    = 0x09,
    SysCraft_UseItem        = 0x0A,
    SysCraft_Stun           = 0x0B,
    SysCraft_Unknown2       = 0x0C,
    SysCraft_Reserve1       = 0x0D,
    SysCraft_Reserve2       = 0x0E,
    SysCraft_Counter        = 0x0F,
    SysCraft_TeamRushInit   = 0x1E,
    SysCraft_TeamRushAction = 0x1F,

    INVALID_ACTION_OFFSET   = 0xFFFF,
    EMPTY_ACTION            = INVALID_ACTION_OFFSET,
};
ML_NAMESPACE_END

ML_NAMESPACE_BEGIN(T_NAME)
enum
{
    Lloyd       = 0x0,
    Elie        = 0x1,
    Tio         = 0x2,
    Randy       = 0x3,

    Lazy        = 0x4,
    Waji        = 0x4,

    Yin         = 0x5,
    Rixia       = 0x5,
    INNE        = 0x5,

    Zeit        = 0x6,
    Arios       = 0x7,
    Noel        = 0x8,
    Dudley      = 0x9,
    Garcia      = 0xA,
    ReserveB    = 0xB,
    CHR_COUNT   = 0xC,
};

DECL_SELECTANY
char* lpChrName[] = {"Lloyd", "Elie", "Tio", "Randy", "Lazy", "Yin/Rixia", "Zeit", "Arios", "Noel", "Dudley", "Garcia", "ReserveB"};
DECL_SELECTANY
char* lpChrNameChs[] = {"罗伊德", "艾莉", "缇欧", "兰迪", "瓦吉", "银/莉夏", "神狼蔡特", "亚里欧斯", "诺艾尔上士", "达德利搜查官", "加尔西亚", "保留B"};
DECL_SELECTANY
WCHAR* lpwChrNameChs[] = {L"罗伊德", L"艾莉", L"缇欧", L"兰迪", L"瓦吉", L"银/莉夏", L"神狼蔡特", L"亚里欧斯", L"诺艾尔上士", L"达德利搜查官", L"加尔西亚", L"保留B"};

ML_NAMESPACE_END

typedef struct  // 0x18
{
    BYTE    Condition;
    BYTE    Probability;
    BYTE    Target;
    BYTE    TargetCondition;
    BYTE    AriaActionIndex;
    BYTE    ActionIndex;
    USHORT  CraftIndex;
    ULONG   Parameter[4];

} CRAFT_AI_INFO, *PCRAFT_AI_INFO;

ML_NAMESPACE_BEGIN(CraftInfoTargets)

enum
{
    OtherSide   = 0x10,
    SelfSide    = 0x80,
};

ML_NAMESPACE_END

typedef struct
{
    USHORT  ActionIndex;
    BYTE    Target;
    BYTE    Unknown_3;
    BYTE    Attribute;
    BYTE    RangeType;
    BYTE    State1;
    BYTE    State2;
    BYTE    RNG;
    BYTE    RangeSize;
    BYTE    AriaTime;
    BYTE    SkillTime;
    USHORT  EP_CP;
    USHORT  RangeSize2;
    USHORT  State1Parameter;
    USHORT  State1Time;
    USHORT  State2Parameter;
    USHORT  State2Time;

    DUMMY_STRUCT(4);

} CRAFT_INFO, *PCRAFT_INFO;

typedef struct
{
    CHAR Description[0x100];
    CHAR Name[0x20];

} CRAFT_DESCRIPTION;

enum
{
    ACTION_ATTACK       = 0,
    ACTION_MOVE         = 1,
    ACTION_ARTS         = 2,
    ACTION_CRAFT        = 3,
    ACTION_SCRAFT       = 4,
    ACTION_ITEM         = 5,
    ACTION_ARIA_ARTS    = 6,
    ACTION_CAST_ARTS    = 7,
    ACTION_ARIA_CRAFT   = 8,
    ACTION_CAST_CRAFT   = 9,
};

enum
{
    BattleStatusRaw,
    BattleStatusFinal,
};


typedef union
{
    DUMMY_STRUCT(0x34);

    struct
    {
        ULONG                   MaximumHP;                  // 0x0     0x234    0x268
        ULONG                   InitialHP;                  // 0x4     0x238    0x26C
        USHORT                  Level;                      // 0x8     0x23C    0x270
        USHORT                  MaximumEP;                  // 0xA     0x23E    0x272
        USHORT                  InitialEP;                  // 0xC     0x240    0x274
        USHORT                  InitialCP;                  // 0xE     0x242    0x276
        ULONG                   EXP;                        // 0x10    0x244    0x278
        //DUMMY_STRUCT(2);

        SHORT                   STR;                        // 0x14    0x248    0x27C
        SHORT                   DEF;                        // 0x16    0x24A    0x27E
        SHORT                   ATS;                        // 0x18    0x24C    0x280
        SHORT                   ADF;                        // 0x1A    0x24E    0x282
        SHORT                   DEX;                        // 0x1C    0x250    0x284
        SHORT                   AGL;                        // 0x1E    0x252    0x286
        SHORT                   MOV;                        // 0x20    0x254    0x288
        SHORT                   SPD;                        // 0x22    0x256    0x28A
        SHORT                   DEXRate;                    // 0x24    0x258    0x28C
        SHORT                   AGLRate;                    // 0x26    0x25A    0x28E
        USHORT                  MaximumCP;                  // 0x28    0x25C    0x290

        DUMMY_STRUCT(2);                                    // 0x25E

        USHORT                  RNG;                        // 0x2C    0x260    0x294

        DUMMY_STRUCT(2);

        ULONG                   ConditionFlags;             // 0x30    0x264    0x298
    };

    struct  
    {
        ULONG                   HPMax;                      // 0x0     0x234    0x268
        ULONG                   HP;                         // 0x4     0x238    0x26C
    };

} CHAR_STATUS, *PCHAR_STATUS;

typedef struct _CHAR_T_STATUS_ORG   // 0x18
{
    USHORT      Level;
    USHORT      HP;
    USHORT      EP;
    SHORT       STR;
    SHORT       DEF;
    SHORT       ATS;
    SHORT       ADF;
    SHORT       DEX;
    SHORT       AGL;
    SHORT       AGLRate;
    SHORT       MOV;
    SHORT       SPD;
} CHAR_T_STATUS_ORG, *PCHAR_T_STATUS_ORG;

typedef struct _CHAR_T_STATUS
{
    //USHORT      Level;
    UINT        HP;
    //USHORT      HP;
    USHORT      EP;
    SHORT       STR;
    SHORT       DEF;
    SHORT       ATS;
    SHORT       ADF;
    SHORT       DEX;
    SHORT       AGL;
    SHORT       AGLRate;
    SHORT       MOV;
    SHORT       SPD;
    SHORT       DEXRate;
    USHORT      RNG;
    USHORT      Level;
} CHAR_T_STATUS, *PCHAR_T_STATUS;

typedef struct _CHAR_T_STATUS_RatioX
{
    float      HP;
    float      EP;
    float      STR;
    float      DEF;
    float      ATS;
    float      ADF;
    float      DEX;
    float      AGL;
    float      MOV;
    float      SPD;
    float      DEXRate;
    float      AGLRate;
    float      RNG;
} CHAR_T_STATUS_RatioX;

typedef struct _CHAR_T_STATUS_RatioY
{
    INT      HP;
    INT      EP;
    INT      STR;
    INT      DEF;
    INT      ATS;
    INT      ADF;
    INT      DEX;
    INT      AGL;
    INT      MOV;
    INT      SPD;
    INT      DEXRate;
    INT      AGLRate;
    INT      RNG;
} CHAR_T_STATUS_RatioY;

typedef struct
{
    ULONG               ConditionFlags;
    PVOID               Effect;
    //BYTE                Type; // 1 回合; 2 次数; 3 AT条动多少次; 4 永久
    BYTE                CounterType;
    BYTE                Flags;
    SHORT               ConditionRate;
    LONG                ATLeft;
    LONG                Unknown4;

    enum CounterTypes
    {
        ByAT        = 0,
        ByRounds    = 1,
        ByTimes     = 2,
        ByActions   = 3,
        Infinite    = 4,
    };

} MS_EFFECT_INFO, *PMS_EFFECT_INFO;

enum
{
    CHR_FLAG_ENEMY  = 0x1000,
    CHR_FLAG_NPC    = 0x2000,
    CHR_FLAG_PARTY  = 0x4000,
    CHR_FLAG_EMPTY  = 0x8000,
};

enum
{
    CHR_FLAG2_ResistBeatBack    = 0x0200,
    CHR_FLAG2_ResistATDelay     = 0x0800,
    //CHR_FLAG2_AbsoluteMiss      = 0x2000,
};

typedef union BattleCtrlData
{
    DUMMY_STRUCT(0x31C);
    struct
    {
        DUMMY_STRUCT(0x80);

        union
        {
            DUMMY_STRUCT(0x60);
            struct
            {
                FLOAT   X;                  // 0x080
                FLOAT   what;
                FLOAT   Y;                  // 0x088
            };

        } PositionData;                     // 0x080

        CHAR    Name[0x20];                 // 0x0E0

        DUMMY_STRUCT(0x68);                 // 0x100

        USHORT  Flags;                      // 0x168
    };

} BattleCtrlData, *PBattleCtrlData;

//typedef union _MONSTER_STATUS
typedef struct _MONSTER_STATUS
{
    //DUMMY_STRUCT(0x2424);

    BOOL IsChrEnemy(BOOL CheckAIType = TRUE)
    {
        if (CheckAIType && AiType == 0xFF)
            return FALSE;

        return FLAG_OFF(State, CHR_FLAG_NPC | CHR_FLAG_PARTY | CHR_FLAG_EMPTY);
    }

    BOOL IsChrEnemyOnly()
    {
        return (State & (CHR_FLAG_ENEMY | CHR_FLAG_NPC | CHR_FLAG_PARTY | CHR_FLAG_EMPTY)) == CHR_FLAG_ENEMY;
    }

    BOOL IsChrEmpty()
    {
        return FLAG_ON(State, CHR_FLAG_EMPTY);
    }

    BOOL IsChrCanThinkSCraft(BOOL CheckAiType = FALSE)
    {
        if (!IsChrEnemy())
            return FALSE;

        if (AiType == 0xFF)
            return FALSE;

        if (!CheckAiType)
            return TRUE;

        switch (AiType)
        {
            case 0x00:
            case 0x02:
            case 0x1F:
                return TRUE;
        }

        return FALSE;
    }

    //struct
    //{

        USHORT                  CharPosition;               // 0x00
        USHORT                  State;                      // 0x02
        USHORT                  State2;                     // 0x04
        DUMMY_STRUCT(4);                                    // 0x06
        USHORT                  CharID;                     // 0x0A
        DUMMY_STRUCT(4);                                    // 0x0C
        ULONG                   SymbolIndex;                // 0x10
        ULONG                   MSFileIndex;                // 0x14
        ULONG                   ASFileIndex;                // 0x18
        DUMMY_STRUCT(1);                                    // 0x1C
        BYTE                    TeamRushAddition;           // 0x1D
        USHORT                  MasterQuartzUsedFlag;       // 0x1E

        DUMMY_STRUCT(0x16C - 0x20);

        USHORT                  CurrentActionType;          // 0x16C

        DUMMY_STRUCT(2);

        USHORT                  PreviousActionType;         // 0x170
        USHORT                  SelectedActionType;         // 0x172
        USHORT                  Unknown_174;
        USHORT                  Unknown_176;
        USHORT                  Unknown_178;
        USHORT                  Unknown_17A;
        USHORT                  WhoAttackMe;                // 0x17C
        USHORT                  CurrentCraftIndex;          // 0x17E
        USHORT                  LastActionIndex;            // 0x180
        USHORT                  CurrentAiIndex;             // 0x182

        DUMMY_STRUCT(0x1AA - 0x184);

        USHORT                  Target[0x10];               // 0x1AA
        BYTE                    TargetCount;                // 0x1CA
        BYTE                    SelectedTargetIndex;        // 0x1CB
        COORD                   SelectedTargetPos;          // 0x1CC

        //DUMMY_STRUCT(0x234 - 0x1D0);
        DUMMY_STRUCT(0x20C - 0x1D0);
        BYTE                    IsHitMiss[0x10];            // 0x20C
        BYTE                    IsHitJudged[0x10];          // 0x21C
        //DUMMY_STRUCT(0x234 - 0x22C);

        PBattleCtrlData         CtrlData;                   // 0x22C
        PVOID                   Unknown_230;                // 0x230
        CHAR_STATUS ChrStatus[2];                           // 0x234

        USHORT MoveSPD;                                     // 0x29C

        DUMMY_STRUCT(2);

        MS_EFFECT_INFO          EffectInfo[32];             // 0x2A0

        DUMMY_STRUCT(0x538 - (0x2A0 + sizeof(MS_EFFECT_INFO) * 32));

        ULONG                   AT;                         // 0x538
        ULONG                   AT2;                        // 0x53C
        USHORT                  AiType;                     // 0x540
        USHORT                  EXPGet;                     // 0x542
        USHORT                  DropItem[2];                // 0x544
        BYTE                    DropRate[2];                // 0x548

        DUMMY_STRUCT(2);

        USHORT                  Equipment[5];               // 0x54C
        USHORT                  Orbment[7];                 // 0x556
        CRAFT_AI_INFO           Attack;                     // 0x564
        CRAFT_AI_INFO           ArtsAiInfo[80];             // 0x57C
        CRAFT_AI_INFO           CraftAiInfo[15];            // 0xCFC
        CRAFT_AI_INFO           SCraftAiInfo[5];            // 0xE64
        CRAFT_AI_INFO           SupportAiInfo[3];           // 0xEDC

        struct
        {
            USHORT                  CraftIndex;             // 0xF24
            BYTE                    AriaActionIndex;        // 0xF26
            BYTE                    ActionIndex;            // 0xF27

        } SelectedCraft;

        BYTE                    Runaway[4];                 // 0xF28

        CRAFT_INFO                  CraftInfo[16];          // 0xF2C
        CRAFT_DESCRIPTION           CraftDescription[16];   // 0x10EC

        BYTE                    Sepith[7];                  // 0x22EC
        DUMMY_STRUCT(3);
        USHORT  				AttributeRate[7];           // 0x22F6
        ULONG       			Resistance;					// 0x2304
        DUMMY_STRUCT(0x78);
        char				    CharacterIntro[0x80];		// 0x2380

        //DUMMY_STRUCT(0x2408 - 0x10EC - sizeof(CRAFT_DESCRIPTION) * 10);
        DUMMY_STRUCT(0x2408 - 0x2380 - 0x80);
        ULONG                       SummonCount;            // 0x2408
        DUMMY_STRUCT(0x2424 - 0x2408 - 0x4);

    //};

} MONSTER_STATUS, *PMONSTER_STATUS;

typedef union _AS_FILE
{
    ULONG GetActionCount()
    {
        ULONG Count = 0;
        PUSHORT ActionList = GetActionList();
        while (ActionList < (PUSHORT)PtrAdd(this, sizeof(*this)) && *ActionList != 0)
        {
            Count++;
            ActionList++;
        }
        return Count;
    }

    BOOL IsActionValid(ULONG ActionNo, ULONG ActionCount)
    {
        if (ActionNo >= ActionCount)
            return FALSE;
        if (GetActionList()[ActionNo] == BattleActionScript::EMPTY_ACTION)
            return FALSE;
        return TRUE;
    }

    PUSHORT GetActionList()
    {
        return (PUSHORT)PtrAdd(this, (ULONG)ActionListOffset);
    }

    DUMMY_STRUCT(0x5F00);
    struct
    {
        USHORT      ActionListOffset;
        USHORT      ChrPosFactorOffset;
    };
} AS_FILE, *PAS_FILE;

typedef union _ITEM_ENTRY
{
    BYTE    Bytes[0x1C];

    struct
    {
        USHORT  ItemId;
        BYTE    Misc;
        BYTE    Limit;
        BYTE    MaxAmount;
        BYTE    Type;           // 0x5
        DUMMY_STRUCT(0x1C - 0x6 - 0x4);
        ULONG   Price;
    };

    struct _DOUGU
    {
        USHORT  ItemId;                 // 0x0
        BYTE    Misc;                   // 0x2
        BYTE    Effect3;                // 0x3
        BYTE    MaxAmount;              // 0x4
        BYTE    Type;                   // 0x5
        BYTE    Effect1;                // 0x6
        BYTE    Effect2;                // 0x7
        BYTE    dummy1;                 // 0x8
        BYTE    ShapeScope;             // 0x9
        CHAR    RNG;                    // 0xA
        BYTE    ScopeRadius;            // 0xB
        SHORT   Effect1Parameter;       // 0xC
        USHORT  Effect1ST;              // 0xE
        SHORT   Effect2Parameter;       // 0x10
        USHORT  Effect2ST;              // 0x12
        CHAR    Effect3Parameter;       // 0x14
        BYTE    Effect3ST;              // 0x15
        BYTE    dummy2;                 // 0x16
        BYTE    DisplayIndex;           // 0x17
        ULONG   Price;                  // 0x18
    } DOUGU;
} ITEM_ENTRY, *PITEM_ENTRY;

EDAO* GlobalGetEDAO();

class CSSaveData
{
#pragma pack(push, 1)
    typedef union
    {
        DUMMY_STRUCT(0x4C4);
        struct
        {
            DUMMY_STRUCT(0x4);
            INT     Adapter;        // 0x4
            INT     Device;
            INT     Mode;
            INT     WindowWidth;    // 0x10
            INT     WindowHeight;   // 0x14
            DUMMY_STRUCT(0x475-0x18);
            BYTE    WindowMode;     // 0x475
            DUMMY_STRUCT(0x484-0x476);
            BYTE    BgmVolumeIni;   // 0x484    ini
            BYTE    SeVolumeIni;    // 0x485
            BYTE    BgmOff;         // 0x486    ini invalid
            BYTE    SeOff;          // 0x487    ini invalid
            DUMMY_STRUCT(0x34);
            BYTE    BgmVolume;      // 0x4BC    81C68
            BYTE    SeVolume;       // 0x4BD    81C69
            DUMMY_STRUCT(0x2);
            ULONG   Option;         // 0x4C0

        } ;

    } SystemConfigData;

    typedef struct
    {
        DUMMY_STRUCT(0x7EDD6);
        USHORT  TitleVisualCount1;  // 0x7EDD6
        DUMMY_STRUCT(0x817AC - 0x7EDD6 - 2);
        //DUMMY_STRUCT(0x817AC);
        SystemConfigData Config;    // 0x817AC
        DUMMY_STRUCT(0xC);
        UINT64  Record;             // 0x81C7C
        ULONG   Tokuten;            // 0x81C84
        ULONG   Medal;              // 0x81C88
        DUMMY_STRUCT(0x8);
        ULONG   GameAccount;        // 0x81C94 invalid bug? PomttoAccount
        DUMMY_STRUCT(0x25434 - 0xC);
        ULONG   TitleVisualCount;   // 0xA70C0
        ULONG   ExtraMode;          // 0xA70C4
        DUMMY_STRUCT(0x4);
        ULONG   Unknown_4D4;        // 0xA70CC

    } MemorySystemData;


    typedef union  // 0x504
    {
        DUMMY_STRUCT(0x504);
        struct
        {
            SystemConfigData Config;
            UINT64  Record;             // 0x4C4
            ULONG   ExtraMode;          // 0x4CC
            ULONG   Tokuten;            // 0x4D0
            ULONG   Unknown_4D4;        // 0x4D4
            ULONG   Medal;              // 0x4D8
            ULONG   TitleVisualCount;   // 0x4DC
            ULONG   GameAccount;        // 0x4E0
        };

    } LocalSystemData;
#pragma pack(pop)

public:
    VOID SaveData2SystemData();
    VOID SystemData2SaveData();

    EDAO* GetEDAO()
    {
        return (EDAO *)PtrSub(this, 0x78CB8);
    }

    MemorySystemData* GetMemorySystemData()
    {
        return (MemorySystemData*)GetEDAO();
    }

    // SaveData2SystemData()
    VOID THISCALL SaveSystemData(LocalSystemData* pLocal)
    {
        if (pLocal == nullptr)
            return;

        MemorySystemData* pMemory = GetMemorySystemData();
        ZeroMemory(pLocal, sizeof(*pLocal));

        pLocal->Config = pMemory->Config;
        pLocal->Record = pMemory->Record;
        pLocal->ExtraMode = pMemory->ExtraMode;
        pLocal->Tokuten = pMemory->Tokuten;
        pLocal->Unknown_4D4 = pMemory->Unknown_4D4;
        pLocal->Medal = pMemory->Medal;
        pLocal->TitleVisualCount = MY_MAX(pMemory->TitleVisualCount, pMemory->TitleVisualCount1);
        pLocal->GameAccount = pMemory->GameAccount;

    }

    // SystemData2SaveData()
    VOID THISCALL LoadSystemData(LocalSystemData* pLocal)
    {
        if (pLocal == nullptr)
            return;

        MemorySystemData* pMemory = GetMemorySystemData();

        //pMemory->Config = pLocal->Config;
        if (pLocal->Config.WindowMode == 0 && pLocal->Config.WindowWidth == 0){}
        else
            pMemory->Config.Option = pLocal->Config.Option;
        pMemory->Record = pLocal->Record;
        pMemory->ExtraMode = pLocal->ExtraMode;
        pMemory->Tokuten = pLocal->Tokuten;
        pMemory->Unknown_4D4 = pLocal->Unknown_4D4;
        pMemory->Medal = pLocal->Medal;
        pMemory->TitleVisualCount = pLocal->TitleVisualCount;
        pMemory->GameAccount = pLocal->GameAccount;
    }

public:

    PUSHORT GetPartyChipMap()
    {
        return (PUSHORT)PtrAdd(this, 0x6140);
    }

    PUSHORT GetPartyList()
    {
        return (PUSHORT)PtrAdd(this, 0x2CC);
    }

    PUSHORT GetPartyListSaved()
    {
        return (PUSHORT)PtrAdd(this, 0x2DC);
    }

    BOOL IsCustomChar(ULONG_PTR ChrId)
    {
        //mark
        return ChrId >= MAXIMUM_CHR_NUMBER_WITH_STATUS ? FALSE : GetPartyChipMap()[ChrId] >= MINIMUM_CUSTOM_CHAR_ID;
    }

    PUSHORT GetChrMagicList()
    {
        return (PUSHORT)PtrAdd(this, 0x5D4);
    }

    PBYTE GetScenaFlags()
    {
        return (PBYTE)PtrAdd(this, 0x9C);
    }

    BOOL IsYinRixia()
    {
        return FLAG_ON(GetScenaFlags()[0x165], 1 << 5);
    }

    BOOL IsLazyKnight()
    {
        return FLAG_ON(GetScenaFlags()[0x1A0], 1);
    }

    ULONG FASTCALL GetTeamAttackMemberId(ULONG ChrId);


    // mark
    VOID THISCALL SetChrPositionAuto(ULONG ChrId, PUSHORT pPartyList, ULONG ChrCount)
    {
        DETOUR_METHOD(CActor, SetChrPositionAuto, 0x676628, ChrId, pPartyList, ChrCount);
    }

public:
    DUMMY_STRUCT(0x94);
    ULONG       SystemFlag[2];              // 0x94
    BYTE        Flag[SCENA_FLAG_SIZE];      // 0x9C
    PVOID       pScenaCharacterInf[2];      // 0x2BC    0x2C0
    ULONG       ScenaCharacterCount[2];     // 0x2C4    0x2C8
    USHORT      PartyList[8];               // 0x2CC
    USHORT      PartyListSaved[8];          // 0x2DC
    CHAR_STATUS ChrStatus[MAXIMUM_CHR_NUMBER_WITH_STATUS]; // 0x2EC

};
#pragma pack(pop)

typedef union
{
    ULONG Flags;
    struct
    {
        UCHAR   HP              : 1;        // 0x00000001
        UCHAR   Level           : 1;        // 0x00000002
        UCHAR   EXP             : 1;        // 0x00000004
        UCHAR   Information     : 1;
        UCHAR   Resist          : 1;        // 0x00000020
        UCHAR   AttributeRate   : 1;        // 0x00000040

                                            // 0x10000000   orb
    };

    BOOL AllValid()
    {
        ULONG AllFlags = 0x01 | 0x02 | 0x04 | 0x20 | 0x40;
        return (Flags & AllFlags) == AllFlags;
    }

    BOOL IsShowByOrbment()
    {
        return FLAG_ON(Flags, 0x10000000);
    }

} MONSTER_INFO_FLAGS, *PMONSTER_INFO_FLAGS;



enum
{
    COLOR_WHITE     = 0,
    COLOR_ORANGE    = 1,
    COLOR_RED       = 2,
    COLOR_BLUE      = 3,
    COLOR_YELLOW    = 4,
    COLOR_GREEN     = 5,
    COLOR_GRAY      = 6,
    COLOR_PINK      = 7,
    COLOR_GOLD      = 8,
    COLOR_BLACK     = 9,
    COLOR_YELLOWB   = 10,


    COLOR_MAXIMUM   = 21,
};

class CBattleInfoBox
{
public:
    EDAO* GetEDAO()
    {
        return *(EDAO **)PtrAdd(this, 0xC);
    }

    CBattle* GetBattle()
    {
        return (CBattle *)PtrSub(this, 0xF0D24);
    }

    PCOORD GetUpperLeftCoord()
    {
        return (PCOORD)PtrAdd(*(PULONG_PTR)PtrAdd(this, 0x20), 0xF2);
    }

    ULONG_PTR GetBackgroundColor()
    {
        return *(PULONG)PtrAdd(*(PULONG_PTR)PtrAdd(this, 0x20), 0x100);
    }

    VOID SetTargetIsEnemy(BOOL Is)
    {
        *(PULONG)PtrAdd(*(PULONG_PTR)PtrAdd(this, 0x20), 0xEC) = Is ? 0 : 1;
    }

    ULONG_PTR IsTargetEnemy()
    {
        return *(PULONG)PtrAdd(*(PULONG_PTR)PtrAdd(this, 0x20), 0xEC) != 1;
    }

    VOID SetMonsterInfoFlags(ULONG_PTR Flags)
    {
        *(PULONG_PTR)PtrAdd(this, 0x1028) = Flags;
    }

    MONSTER_INFO_FLAGS GetMonsterInfoFlags()
    {
        return *(PMONSTER_INFO_FLAGS)PtrAdd(this, 0x1028);
    }

    NoInline VOID DrawSimpleText(LONG X, LONG Y, PCSTR Text, ULONG ColorIndex, LONG Weight = FW_NORMAL, ULONG ZeroU1 = 1, FLOAT ZeroF1 = 1)
    {
        TYPE_OF(&CBattleInfoBox::DrawSimpleText) StubDrawSimpleText;

        *(PVOID *)&StubDrawSimpleText = (PVOID)0x67A101;

        return (this->*StubDrawSimpleText)(X, Y, Text, ColorIndex, Weight, ZeroU1, ZeroF1);
    }

public:
    VOID THISCALL SetMonsterInfoBoxSize(LONG X, LONG Y, LONG Width, LONG Height);
    VOID THISCALL DrawMonsterStatus();

    DECL_STATIC_METHOD_POINTER(CBattleInfoBox, DrawMonsterStatus);
};

INIT_STATIC_MEMBER(CBattleInfoBox::StubDrawMonsterStatus);

#pragma pack(push, 1)

typedef union
{
    DUMMY_STRUCT(0x78);
    struct
    {
        DUMMY_STRUCT(0x60);

        PMONSTER_STATUS MSData;     // 0x60

        CBattle* Battle;

        ULONG   IconAT;             // 0x68
        USHORT  Flags;              // 0x6C 不含20 空; 含10 AT条移动; 含04 行动、delay后的([20A]0销毁); 含40 当前行动的(1销毁)
        BYTE    Sequence;           // 0x6E

        DUMMY_STRUCT(2);
        BYTE    RNo;		        // 0x71

        DUMMY_STRUCT(1);

        //BYTE    Pri;	            // 0x73
        BYTE    sequence;                // 0x73
        BOOLEAN IsSBreaking;

        DUMMY_STRUCT(3);
    };

} AT_BAR_ENTRY, *PAT_BAR_ENTRY;

class CBattleATBar
{
public:
    AT_BAR_ENTRY   Entry[0x3C];
    PAT_BAR_ENTRY  EntryPointer[0x3C];      // 0x1C20

    BOOL IsCurrentChrSBreak()
    {
        return EntryPointer[0]->IsSBreaking;
    }

    // -1 for null
    BYTE THISCALL GetChrAT0()
    {
        TYPE_OF(&CBattleATBar::GetChrAT0) f;
        *(PVOID *)&f = (PVOID)0x00677230;
		return (this->*f)();
    }

    VOID AdvanceChrInATBar(PMONSTER_STATUS MSData, BOOL InsertToFirstPos)
    {
        TYPE_OF(&CBattleATBar::AdvanceChrInATBar) AdvanceChrInATBar;

        *(PVOID *)&AdvanceChrInATBar = (PVOID)0x676D3F;

        return (this->*AdvanceChrInATBar)(MSData, InsertToFirstPos);
    }

    NoInline PAT_BAR_ENTRY FindATBarEntry(PMONSTER_STATUS MSData)
    {
        PAT_BAR_ENTRY *Entry;

        FOR_EACH(Entry, EntryPointer, countof(EntryPointer))
        {
            if (Entry[0]->MSData == MSData)
                return Entry[0];
        }

        return nullptr;
    }
/*
    NoInline PAT_BAR_ENTRY FindATBarEntry0NoSecond()
    {
        PAT_BAR_ENTRY *Entry;
        static PAT_BAR_ENTRY PreEntry;

        if (PreEntry == EntryPointer[0])
            return PreEntry;

        FOR_EACH(Entry, &EntryPointer[1], countof(EntryPointer))
        {
            //if (Entry[0]->MSData == nullptr) //有不连续的情况
                //break;
            if (Entry[0]->MSData == EntryPointer[0]->MSData)
            {
                PreEntry = Entry[0];
                return Entry[0];
            }
        }

        return nullptr;
    }
*/
    PAT_BAR_ENTRY THISCALL LookupReplaceAtBarEntry(PMONSTER_STATUS MSData, BOOL IsFirst);
};

#pragma pack(pop)

ML_NAMESPACE_BEGIN(ArtsPage)

enum ArtsPageType
{
    Attack      = 0,
    Recovery    = 1,
    Debuff      = 2,
    Auxiliary   = 3,
    Master      = 4,
};

ML_NAMESPACE_END

class CArtsNameWindow
{
public:

    READONLY_PROPERTY(ULONG_PTR, SelectedArtsIndex)
    {
        return *(PULONG)PtrAdd(this, 0xBC);
    }

    READONLY_PROPERTY(ArtsPage::ArtsPageType, ArtsType)
    {
        return (ArtsPage::ArtsPageType)*(PULONG)PtrAdd(this, 0xDC);
    }
};

class CBattle
{
public:
    VOID THISCALL SetSelectedAttack(PMONSTER_STATUS MSData);
    VOID THISCALL SetSelectedMagic(PMONSTER_STATUS MSData, USHORT CraftIndex, USHORT AiIndex);
    VOID THISCALL SetSelectedCraft(PMONSTER_STATUS MSData, USHORT CraftIndex, USHORT AiIndex);
    VOID THISCALL SetSelectedSCraft(PMONSTER_STATUS MSData, USHORT CraftIndex, USHORT AiIndex);

    CSSaveData* GetSaveData()
    {
        return *(CSSaveData **)PtrAdd(this, 0x38D28);
    }

    READONLY_PROPERTY(CArtsNameWindow*, ArtsNameWindow)
    {
        return *(CArtsNameWindow **)PtrAdd(this, 0xFF59C);
    }

    READONLY_PROPERTY(BattleCtrlData*, CtrlData)
    {
        return (BattleCtrlData *)PtrAdd(this, 0x660);
    }

    CBattleInfoBox* GetBattleInfoBox()
    {
        return (CBattleInfoBox *)PtrAdd(this, 0xF0D24);
    }

    EDAO* GetEDAO()
    {
        return *(EDAO **)PtrAdd(this, 0x38D24);
    }

    CGlobal* GetGlobal()
    {
        return *(CGlobal **)PtrAdd(this, 0x38D30);
    }

    CBattleATBar* GetBattleATBar()
    {
        return (CBattleATBar *)PtrAdd(this, 0x103148);
    }

    BOOL IsCustomChar(ULONG_PTR ChrId)
    {
        return GetSaveData()->IsCustomChar(ChrId);
    }

    BOOL IsForceInsertToFirst()
    {
        return *(PBOOL)PtrAdd(this, 0x3A7B0);
    }

    PVOID GetMSFileBuffer()
    {
        return PtrAdd(this, 0x114ED0);
    }

    PMONSTER_STATUS GetMonsterStatus()
    {
        return (PMONSTER_STATUS)PtrAdd(this, 0x4DE4);
    }

    LONG_PTR GetCurrentChrIndex()
    {
        return *(PLONG)PtrAdd(this, 0x113080);
    }

    LONG_PTR GetCurrentTargetIndex()
    {
        return *(PLONG)PtrAdd(this, 0x113090);
    }

    VOID ShowConditionText(PMONSTER_STATUS MSData, PCSTR Text, ULONG Color = RGBA(255, 136, 0, 255), ULONG Unknown = 0)
    {
        TYPE_OF(&CBattle::ShowConditionText) ShowConditionText;
        *(PULONG_PTR)&ShowConditionText = 0xA17420;
        return (this->*ShowConditionText)(MSData, Text, Color, Unknown);
    }

    // mark
    PMS_EFFECT_INFO THISCALL FindEffectInfoByCondition(PMONSTER_STATUS MSData, ULONG_PTR Condition, INT TimeLeft = 0)
    {
        //DETOUR_METHOD(CBattle, FindEffectInfoByCondition, 0x673CC5, MSData, Condition, ConditionRateType);
        TYPE_OF(&CBattle::FindEffectInfoByCondition) FindEffectInfoByCondition;

        *(PULONG_PTR)&FindEffectInfoByCondition = 0x9E34A0;

        return (this->*FindEffectInfoByCondition)(MSData, Condition, TimeLeft);
    }

    PMS_EFFECT_INFO THISCALL FindEffectInfoByConditionEx(PMONSTER_STATUS MSData, ULONG_PTR Condition, INT ConditionRateType = 0, BOOL IsCheckSum = TRUE);

    VOID THISCALL ShowSkipAnimeButton()
    {
        DETOUR_METHOD(CBattle, ShowSkipAnimeButton, 0x673513);
    }

    VOID THISCALL CancelAria(PMONSTER_STATUS MSData, BOOL Reset)
    {
        DETOUR_METHOD(CBattle, CancelAria, 0x99DDC0, MSData, Reset);
    }

    VOID THISCALL UpdateHP(PMONSTER_STATUS MSData, LONG Increment, LONG Initial, BOOL What = TRUE)
    {
        TYPE_OF(&CBattle::UpdateHP) UpdateHP;
        *(PULONG_PTR)&UpdateHP = 0x9ED1F0;
        return (this->*UpdateHP)(MSData, Increment, Initial, What);
    }

    VOID THISCALL UpdateEffectLeftTime(PMONSTER_STATUS MSData, LONG LeftTime, BYTE CounterType, ULONG ConditionFlag)
    {
        DETOUR_METHOD(CBattle, UpdateEffectLeftTime, 0x9E3570, MSData, LeftTime, CounterType, ConditionFlag);
    }

    /************************************************************************
      bug fix
    ************************************************************************/
    VOID
    THISCALL
    ExecuteActionScript(
        PMONSTER_STATUS MSData,
        PBYTE           ActionScript,
        BYTE            ChrThreadId,
        USHORT          ScriptOffset,
        ULONG           Unknown1,
        ULONG           Unknown2,
        ULONG           Unknown3
    );

    /************************************************************************
      tweak
    ************************************************************************/

    VOID NakedNoResistConditionUp();

    static LONG CDECL FormatBattleChrAT(PSTR Buffer, PCSTR Format, LONG Index, LONG No, LONG IcoAT, LONG ObjAT, LONG Pri);
    static LONG CDECL ShowSkipCraftAnimeButton(...);

    /************************************************************************
      hack for use enemy
    ************************************************************************/

    PMONSTER_STATUS FASTCALL OverWriteBattleStatusWithChrStatus(PMONSTER_STATUS MSData, PCHAR_STATUS ChrStatus);
    VOID NakedOverWriteBattleStatusWithChrStatus();

    VOID NakedIsChrStatusNeedRefresh();
    BOOL FASTCALL IsChrStatusNeedRefresh(ULONG_PTR ChrPosition, PCHAR_STATUS CurrentStatus, LONG_PTR PrevLevel);

    ULONG NakedGetChrIdForSCraft();

    VOID NakedGetTurnVoiceChrId();
    VOID NakedGetReplySupportVoiceChrId();
    VOID NakedGetRunawayVoiceChrId();
    VOID NakedGetTeamRushVoiceChrId();
    VOID NakedGetUnderAttackVoiceChrId();
    VOID NakedGetUnderAttackVoiceChrId2();
    VOID NakedGetSBreakVoiceChrId();

    ULONG FASTCALL GetVoiceChrIdWorker(PMONSTER_STATUS MSData);

    VOID NakedCopyMagicAndCraftData();
    VOID FASTCALL CopyMagicAndCraftData(PMONSTER_STATUS MSData);

    VOID NakedFindReplaceChr();
    VOID NakedCheckCraftTargetBits();

    VOID THISCALL GetConditionIconPosByIndex(Gdiplus::PointF *Position, PMS_EFFECT_INFO EffectInfo, ULONG_PTR ConditionIndex);
    BOOL THISCALL IsTargetCraftReflect(PMONSTER_STATUS Self, PMONSTER_STATUS Target, ULONG_PTR ActionType = 0xFFFF);
    VOID THISCALL OnSetChrConditionFlag(PMONSTER_STATUS MSData, ULONG Param2, ULONG ConditionFlag, USHORT Param4, USHORT Param5);

    BOOL FASTCALL UpdateCraftReflectLeftTime(BOOL CanNotReflect, PMONSTER_STATUS MSData);
    VOID NakedUpdateCraftReflectLeftTime();

#pragma pack(push, 1)
    typedef struct
    {
        BYTE    OpCode;
        BYTE    Function;

        union
        {
            ULONG Param[4];

        };

    } AS_8D_PARAM, *PAS_8D_PARAM;
#pragma pack(pop)

    enum
    {
        AS_8D_FUNCTION_MINIMUM  = 0x70,

        //AS_8D_FUNCTION_REI_JI_MAI_GO    =   AS_8D_FUNCTION_MINIMUM,
        AS_8D_FUNCTION_REI_JI_MAI_GO    = 0x70,
        AS_8D_FUNCTION_AVATAR           = 0x71,
    };

    VOID NakedAS8DDispatcher();
    VOID FASTCALL AS8DDispatcher(PMONSTER_STATUS MSData, PAS_8D_PARAM ASBuffer);
    VOID HandleAvatar(PMONSTER_STATUS MSData, PAS_8D_PARAM Parameter);
    ULONG FASTCALL FindEmptyPosition(BOOL FindEnemyOnly = FALSE);

    BOOL THISCALL CloneMSData(PMONSTER_STATUS MSData, USHORT CraftIndex, PCRAFT_AI_INFO AIInfo)
    {
        DETOUR_METHOD(CBattle, CloneMSData, 0x9B6DF0, MSData, CraftIndex, AIInfo);
    }

    VOID THISCALL ResetCtrlData(ULONG CharPosition)
    {
        DETOUR_METHOD(CBattle, ResetCtrlData, 0x9A8100, CharPosition);
    }

    VOID THISCALL ResetMSData(ULONG CharPosition)
    {
        DETOUR_METHOD(CBattle, ResetMSData, 0x9A84D0, CharPosition);
    }

    BOOL THISCALL LoadMSData(ULONG MSFileIndex, ULONG CharPosition)
    {
        DETOUR_METHOD(CBattle, LoadMSData, 0x9A4A50, MSFileIndex, CharPosition);
    }

    BOOL THISCALL IsAvatarLoaded(ULONG AvatarIndex);
 
    PROPERTY(ULONG, SummonX);
    PROPERTY(ULONG, SummonY);

    GET(SummonX)
    {
        return *(PULONG)PtrAdd(this, 0xF3F48);
    }

    SET(SummonX)
    {
        *(PULONG)PtrAdd(this, 0xF3F48) = value;
    }

    GET(SummonY)
    {
        return *(PULONG)PtrAdd(this, 0xF3F4C);
    }

    SET(SummonY)
    {
        *(PULONG)PtrAdd(this, 0xF3F4C) = value;
    }
 

    /************************************************************************
      enemy sbreak
    ************************************************************************/

#define THINK_SBREAK_FILTER TAG4('THSB')
#define FIND_EMPTY_POSITION_FILTER  TAG4('EPOS')

    VOID NakedGetBattleState();
    VOID FASTCALL HandleBattleState(ULONG_PTR CurrentState);
    VOID THISCALL SetCurrentActionChrInfo(USHORT Type, PMONSTER_STATUS MSData);

    LONG NakedEnemyThinkAction();
    BOOL FASTCALL EnemyThinkAction(PMONSTER_STATUS MSData);

    BOOL THISCALL ThinkRunaway(PMONSTER_STATUS MSData);
    BOOL THISCALL ThinkSCraft(PMONSTER_STATUS MSData);

    BOOL ThinkSBreak(PMONSTER_STATUS MSData, PAT_BAR_ENTRY Entry);


    /************************************************************************
      acgn beg
    ************************************************************************/

    VOID THISCALL LoadMSFile(PMONSTER_STATUS MSData, ULONG MSFileIndex);
    VOID THISCALL LoadMonsterIt3(ULONG CharPosition, ULONG par2, PSTR it3Path);
    VOID NakedAS_8D_5F();
    VOID THISCALL AS_8D_5F(PMONSTER_STATUS);

    // mark
    // 不处理部分人物模型特效，取消vanish也不会回来
    VOID THISCALL UnsetCondition(PMONSTER_STATUS MSData, ULONG condition)
    {
        //DETOUR_METHOD(CBattle, UnsetCondition, 0x672AE1, MSData, condition);
        TYPE_OF(&CBattle::UnsetCondition) f;
        *(PVOID *)&f = (PVOID)0x672AE1;
        return (this->*f)(MSData, condition);
    }

    VOID THISCALL RemoveCondition(PMONSTER_STATUS MSData, ULONG condition, INT type) // >0 Rate>0; <0 Rate <0; 0 all
    {
        DETOUR_METHOD(CBattle, RemoveCondition, 0x67B0BA, MSData, condition, type);
    }

    // 不经过抗性检测
    VOID THISCALL SetCondition(PMONSTER_STATUS dst, PMONSTER_STATUS src, ULONG condition, ULONG at, ULONG conditionRate)
    {
        //DETOUR_METHOD(CBattle, SetCondition, 0x676EA2, dst, src, condition, at, conditionRate);
        TYPE_OF(&CBattle::SetCondition) f;
        *(PVOID *)&f = (PVOID)0x676EA2;
        return (this->*f)(dst, src, condition, at, conditionRate);
	}

    VOID THISCALL AddCondition(PMONSTER_STATUS src, PMONSTER_STATUS dst, ULONG condition, ULONG at, ULONG conditionRate)
    {
        DETOUR_METHOD(CBattle, AddCondition, 0x677D11, src, dst, condition, at, conditionRate);
    }

    BOOL CheckCondition(PMONSTER_STATUS MSData, ULONG condition, ULONG par3=0)
    {
        return FindEffectInfoByCondition(MSData, condition, par3) != nullptr;
    }

    VOID THISCALL SubHPWhenAttack(PMONSTER_STATUS dst, INT HP)
    {
        DETOUR_METHOD(CBattle, SubHPWhenAttack, 0x679869, dst, HP);
    }

    VOID THISCALL SubHPEveryAct(PMONSTER_STATUS dst, INT HP)
    {
        DETOUR_METHOD(CBattle, SubHPEveryAct, 0x674157, dst, HP);
    }

    VOID THISCALL AnalyzeMonsInf(PMONSTER_STATUS MSData, BOOL IsSkipBattleEvaluation = FALSE)
    {
        DETOUR_METHOD(CBattle, AnalyzeMonsInf, 0x6747D3, MSData, IsSkipBattleEvaluation);
    }

    DECL_STATIC_METHOD_POINTER(CBattle, LoadMSFile);

    /************************************************************************
      acgn end
    ************************************************************************/

    // mark
    PAS_FILE GetASFile()
    {
        return (PAS_FILE)PtrAdd(this, 0x3A800);
    }

    PAS_FILE GetASFile(PMONSTER_STATUS MSData)
    {
        if (MSData->CharPosition >= MAXIMUM_CHR_NUMBER_IN_BATTLE || MSData->IsChrEmpty())
            return nullptr;
        else
            return &(GetASFile()[MSData->CharPosition]);
    }


    VOID NakedCheckAliveBeforeHeal();
    VOID FASTCALL CheckAliveBeforeHeal(ULONG CharPosition);
    VOID THISCALL SubHPEveryAct2WhenAttack(PMONSTER_STATUS dst, PCHAR_STATUS pChrStatusFinal, INT HP);
    VOID NakedHandleConditionBeforeMasterQuartzKipaTakeEffect();
    VOID FASTCALL HandleConditionBeforeMasterQuartzKipaTakeEffect(PMONSTER_STATUS MSData);
    BOOL THISCALL IsNeedBattleEvaluationSuperKill(ULONG ChrPosition);

    VOID THISCALL SetBattleStatusFinalByDifficulty(PMONSTER_STATUS MSData);
    DECL_STATIC_METHOD_POINTER(CBattle, SetBattleStatusFinalByDifficulty);

    BOOL THISCALL CheckQuartz(ULONG ChrPosition, ULONG ItemId, PULONG EquippedIndex = nullptr);
    DECL_STATIC_METHOD_POINTER(CBattle, CheckQuartz);

    PMS_EFFECT_INFO THISCALL CheckConditionGreenPepperWhenThinkCraft(PMONSTER_STATUS MSData, ULONG_PTR Condition, INT ConditionRateType);

    BOOL IsChrCanTeamRush(PMONSTER_STATUS MSData, PCRAFT_INFO pCraft);
    DECL_STATIC_METHOD_POINTER(CBattle, IsChrCanTeamRush);

    BOOL GetHitResult(PMONSTER_STATUS src, PMONSTER_STATUS dst);

    DECL_STATIC_METHOD_POINTER(CBattle, SetCurrentActionChrInfo);
    DECL_STATIC_METHOD_POINTER(CBattle, ThinkRunaway);
    DECL_STATIC_METHOD_POINTER(CBattle, ThinkSCraft);
    DECL_STATIC_METHOD_POINTER(CBattle, ExecuteActionScript);
    DECL_STATIC_METHOD_POINTER(CBattle, IsTargetCraftReflect);
    DECL_STATIC_METHOD_POINTER(CBattle, OnSetChrConditionFlag);

    static PCHAR_STATUS pChrStatusBackup;
};

INIT_STATIC_MEMBER(CBattle::StubSetCurrentActionChrInfo);
INIT_STATIC_MEMBER(CBattle::StubThinkRunaway);
INIT_STATIC_MEMBER(CBattle::StubThinkSCraft);
INIT_STATIC_MEMBER(CBattle::StubLoadMSFile);
INIT_STATIC_MEMBER(CBattle::StubExecuteActionScript);
INIT_STATIC_MEMBER(CBattle::StubIsTargetCraftReflect);
INIT_STATIC_MEMBER(CBattle::StubOnSetChrConditionFlag);

INIT_STATIC_MEMBER(CBattle::StubSetBattleStatusFinalByDifficulty);
INIT_STATIC_MEMBER(CBattle::StubCheckQuartz);
INIT_STATIC_MEMBER(CBattle::StubIsChrCanTeamRush);

INIT_STATIC_MEMBER(CBattle::pChrStatusBackup);

class CSound
{
public:
    PCSTR GetSoundDataPath()
    {
        return (PCSTR)PtrAdd(this, 0x10);
    }

    VOID THISCALL PlaySound(ULONG SeIndex, ULONG v1 = 1, ULONG v2 = 0, ULONG v3 = 100, ULONG v4 = 0, ULONG v5 = 35, ULONG v6 = 0)
    {
        TYPE_OF(&CSound::PlaySound) PlaySound;

        *(PVOID *)&PlaySound = (PVOID)0x677271;

        return (this->*PlaySound)(SeIndex, v1, v2, v3, v4, v5, v6);
    }

    PVOID THISCALL GetSeEntryBySoundIndex(ULONG SeIndex)
    {
        DETOUR_METHOD(CSound, GetSeEntryBySoundIndex, 0x90E690, SeIndex);
    }

    PVOID THISCALL GetSoundPathByIndex(PSTR SoundPath, ULONG SeIndex);

    DECL_STATIC_METHOD_POINTER(CSound, GetSoundPathByIndex);
};

INIT_STATIC_MEMBER(CSound::StubGetSoundPathByIndex);

typedef struct
{
    DUMMY_STRUCT(0x30);

    FLOAT Vertical;         // 0x30
    FLOAT Obliquity;        // 0x34
    FLOAT Horizon;          // 0x38

    DUMMY_STRUCT(0x54 - 0x38 - sizeof(FLOAT));

    FLOAT Distance;         // 0x54

} CAMERA_INFO, *PCAMERA_INFO;

class CCamera
{
public:
    PCAMERA_INFO GetCameraInfo()
    {
        return (PCAMERA_INFO)*(PVOID *)PtrAdd(this, 0xB88);
    }
};

typedef struct
{
    USHORT  State;
    BYTE    ScenaIndex;

    DUMMY_STRUCT(1);

    ULONG   FunctionOffset;
    ULONG   CurrentOffset;

} *PSCENA_ENV_BLOCK;

class CScript
{
public:
    EDAO* GetEDAO()
    {
        return (EDAO *)PtrSub(this, 0x384EC);
    }

    CCamera* GetCamera()
    {
        return *(CCamera **)PtrAdd(this, 0x524);
    }

    CSSaveData* GetSaveData()
    {
        return *(CSSaveData **)this;
    }

    PBYTE* GetScenaTable()
    {
        return (PBYTE *)PtrAdd(this, 0x7D4);
    }


    BOOL THISCALL ScpSaveRestoreParty(PSCENA_ENV_BLOCK Block);
    // mark
    BOOL THISCALL ScpLeaveParty(PSCENA_ENV_BLOCK Block);
    ULONG THISCALL ScpGetFunctionAddress(ULONG_PTR pScena, ULONG function);

    VOID FASTCALL InheritSaveData(PBYTE ScenarioFlags);
    VOID NakedInheritSaveData();

    DECL_STATIC_METHOD_POINTER(CScript, InheritSaveData);
    DECL_STATIC_METHOD_POINTER(CScript, ScpLeaveParty);
    DECL_STATIC_METHOD_POINTER(CScript, ScpGetFunctionAddress);
    DECL_STATIC_METHOD_POINTER(CScript, ScpSaveRestoreParty);
};

INIT_STATIC_MEMBER(CScript::StubInheritSaveData);
INIT_STATIC_MEMBER(CScript::StubScpSaveRestoreParty);
INIT_STATIC_MEMBER(CScript::StubScpLeaveParty);
INIT_STATIC_MEMBER(CScript::StubScpGetFunctionAddress);

class CMap
{
public:
    PULONG GetFrameNumber()
    {
        return (PULONG)PtrAdd(this, 0x1C7C);
    }
};

class CInput
{
public:
    BOOL UseJoyStick()
    {
        return *(PBOOL)this;
    }

    LPDIRECTINPUTDEVICE8A GetDInputDevice()
    {
        return *(LPDIRECTINPUTDEVICE8A *)PtrAdd(this, 0x218);
    }

    VOID THISCALL HandleMainInterfaceInputState(PVOID Parameter1, CInput *Input, PVOID Parameter3);

    DECL_STATIC_METHOD_POINTER(CInput, HandleMainInterfaceInputState);
};

INIT_STATIC_MEMBER(CInput::StubHandleMainInterfaceInputState);


class EDAO
{
    // battle

public:

    static EDAO* GlobalGetEDAO()
    {
        //return ::GlobalGetEDAO();
        return *(EDAO **)0xC29988;
    }

    static VOID JumpToMap()
    {
        return ((TYPE_OF(&EDAO::JumpToMap))0x6A0DF0)();
    }

    CMap* GetMap()
    {
        return (CMap *)PtrAdd(this, 0x141C);
    }

    CGlobal* GetGlobal()
    {
        return (CGlobal *)PtrAdd(this, 0x4D3E8);
    }

    CBattle* GetBattle()
    {
        return *(CBattle **)PtrAdd(this, 0x82BA4);
    }

    CSound* GetSound()
    {
        return (CSound *)PtrAdd(this, 0x3A628);
    }

    CScript* GetScript()
    {
        return (CScript *)PtrAdd(this, 0x384EC);
    }

    CSSaveData* GetSaveData()
    {
        //return GetScript()->GetSaveData();
        return (CSSaveData*)PtrAdd(this, 0x78CB8);
    }

    CDebug* GetDebug()
    {
        return (CDebug *)PtrAdd(this, 0xB80D8);
    }

    BOOL IsCustomChar(ULONG_PTR ChrId)
    {
        return GetSaveData()->IsCustomChar(ChrId);
    }

    PUSHORT GetSBreakList()
    {
        return (PUSHORT)PtrAdd(this, 0x7EE10);
    }

    PFLOAT GetChrCoord()
    {
        return (PFLOAT)(*(PULONG_PTR)PtrAdd(EDAO::GlobalGetEDAO(), 0x78CB8 + 0x2BC));
    }

    VOID UpdateChrCoord(PVOID Parameter)
    {
        DETOUR_METHOD(EDAO, UpdateChrCoord, 0x74F490, Parameter);
    }

    ULONG_PTR GetLayer()
    {
        return *(PUSHORT)PtrAdd(this, 0xA6FA8);
    }

    PSIZE GetWindowSize()
    {
        return (PSIZE)PtrAdd(this, 0x3084);
    }

    VOID LoadFieldAttackData(ULONG_PTR Dummy = 0)
    {
        TYPE_OF(&EDAO::LoadFieldAttackData) LoadFieldAttackData;
        *(PULONG_PTR)&LoadFieldAttackData = 0x6F8D30;
        return (this->*LoadFieldAttackData)(Dummy);
    }

    LONG THISCALL AoMessageBox(PCSTR Text, BOOL CanUseCancelButton = TRUE)
    {
        typedef struct
        {
            PVOID Callbacks[4];

        } MSGBOX_CALLBACK;

        LONG (FASTCALL *AoMessageBoxWorker)(EDAO*, PVOID, BOOL CanUseCancelButton, PCSTR Text, MSGBOX_CALLBACK, ULONG, ULONG, ULONG);

        MSGBOX_CALLBACK cb = { (PVOID)0x676056, nullptr, nullptr, nullptr };

        *(PULONG_PTR)&AoMessageBoxWorker = 0x67A4D5;
        return AoMessageBoxWorker(this, 0, CanUseCancelButton, Text, cb, 0, 0, -1);
    }

    VOID StubDrawNumber(LONG X, LONG Y, PCSTR Text, ULONG OneU1, ULONG Color, ULONG ZeroU1);

    NoInline VOID DrawNumber(LONG X, LONG Y, PCSTR Text, ULONG_PTR Color, ULONG OneU1 = 1, ULONG ZeroU1 = 0)
    {
        TYPE_OF(&EDAO::StubDrawNumber) StubDrawNumber;

        *(PVOID *)&StubDrawNumber = (PVOID)0x734A00;

        return (this->*StubDrawNumber)(X, Y, Text, OneU1, Color, ZeroU1);
    }

    VOID DrawText(PCSTR Text, LONG X, LONG Y, LONG FontSize, BYTE Red, BYTE Green, BYTE Blue)
    {
        DETOUR_METHOD(EDAO, DrawText, 0x87D710, Text, X, Y, FontSize, Red, Green, Blue);
    }

    VOID THISCALL StubDrawRectangle(USHORT Layer, LONG Left, LONG Top, LONG Right, LONG Bottom, FLOAT ZeroF1, FLOAT ZeroF2, FLOAT ZeroF3, FLOAT ZeroF4, ULONG UpperLeftColor, ULONG UpperRightColor, ULONG ZeroU1,ULONG ZeroU2,ULONG ZeroU3,ULONG ZeroU4,ULONG ZeroU5,ULONG ZeroU6,FLOAT ZeroF5);

    NoInline
    VOID
    THISCALL
    DrawRectangle(
        /* USHORT Layer */
        LONG    Left,
        LONG    Top,
        LONG    Right,
        LONG    Bottom,

        ULONG   UpperLeftColor,
        ULONG   UpperRightColor,

        FLOAT   ZeroF1 = 0,
        FLOAT   ZeroF2 = 0,
        FLOAT   ZeroF3 = 0,
        FLOAT   ZeroF4 = 0,

        ULONG   ZeroU1 = 0,
        ULONG   ZeroU2 = 0,
        ULONG   ZeroU3 = 0,
        ULONG   ZeroU4 = 0,
        ULONG   ZeroU5 = 0,
        ULONG   ZeroU6 = 0,

        FLOAT   ZeroF5 = 0
    )
    {
        TYPE_OF(&EDAO::StubDrawRectangle) StubDrawRectangle;
        *(PVOID *)&StubDrawRectangle = (PVOID)0x6726EA;

        return (PtrAdd(this, 0x254)->*StubDrawRectangle)(
                    GetLayer(),
                    Left,
                    Top,
                    Right,
                    Bottom,

                    ZeroF1,
                    ZeroF2,
                    ZeroF3,
                    ZeroF4,

                    UpperLeftColor,
                    UpperRightColor,

                    ZeroU1,
                    ZeroU2,
                    ZeroU3,
                    ZeroU4,
                    ZeroU5,
                    ZeroU6,

                    ZeroF5
                );
    }

    VOID CalcChrRawStatusFromLevel(ULONG ChrId, ULONG Level, ULONG Unknown = 0)
    {
        //DETOUR_METHOD(EDAO, CalcChrRawStatusFromLevel, 0x675FF7, ChrId, Level, Unknown);
        TYPE_OF(&EDAO::CalcChrRawStatusFromLevel) f;

        *(PVOID *)&f = (PVOID)0x675FF7;

        return (this->*f)(ChrId, Level, Unknown);
    }

    ArtsPage::ArtsPageType THISCALL GetCraftType(PCRAFT_INFO StateCraftInfo, PCRAFT_INFO EPCPCraftInfo = nullptr)
    {
        DETOUR_METHOD(EDAO, GetCraftType, 0x97F100, StateCraftInfo, EPCPCraftInfo);
    }


    /************************************************************************
      hack for boss
    ************************************************************************/

    VOID NakedGetChrSBreak();
    VOID FASTCALL GetChrSBreak(PMONSTER_STATUS MSData);
    LONG FASTCALL GetStatusIcon(ULONG ChrId);
    LONG FASTCALL GetCFace(ULONG ChrId);
    LONG FASTCALL GetLeaderChangeVoice(ULONG ChrId);

    static LONG CDECL GetCampImage(PSTR Buffer, PCSTR Format, LONG ChrId);
    static LONG CDECL GetBattleFace(PSTR Buffer, PCSTR Format, PCSTR DataPath, LONG ChrId);
    static LONG CDECL GetFieldAttackChr(PSTR Buffer, PCSTR Format, LONG ChrId);

    /************************************************************************
      tweak
    ************************************************************************/

    static VOID NakedLoadSaveDataThumb();
    static VOID NakedSetSaveDataScrollStep();

    // hook
public:
    VOID THISCALL Fade(ULONG Param1, ULONG Param2, ULONG Param3, ULONG Param4, ULONG Param5, ULONG Param6);
    BOOL THISCALL CheckItemEquipped(ULONG ItemId, PULONG EquippedIndex);

    //mark
    ULONG THISCALL GetDifficulty();

    VOID THISCALL SetBattleStatusFinalByEquipment(ULONG ChrPosition, PCHAR_STATUS pStatusFinal, PCHAR_STATUS pStatusBasic)
    {
        DETOUR_METHOD(EDAO, SetBattleStatusFinalByEquipment, 0x00676ECA, ChrPosition, pStatusFinal, pStatusBasic);
    }

    // 木偶 复活
    VOID THISCALL SetBattleStatusFinalWhenRecover(ULONG ChrPosition, PCHAR_STATUS pStatusFinal, PCHAR_STATUS pStatusBasic);

    DECL_STATIC_METHOD_POINTER(EDAO, CheckItemEquipped);
    DECL_STATIC_METHOD_POINTER(EDAO, GetDifficulty);

    static CHAR_T_STATUS ChrT_Status;
    static PCHAR_T_STATUS CalcChrT_StatusNew(PCHAR_T_STATUS pStatus, ULONG ChrNo, ULONG Level);
    PCHAR_T_STATUS THISCALL CalcChrT_Status(ULONG ChrNo, ULONG Level);
    DECL_STATIC_METHOD_POINTER(EDAO, CalcChrT_Status);

    VOID THISCALL CalcChrRawStatusFromLevelNew(ULONG ChrId, ULONG Level, ULONG Unknown = 0);
    DECL_STATIC_METHOD_POINTER(EDAO, CalcChrRawStatusFromLevel);

    PCHAR_STATUS CalcChrRawStatusByFinalStatus(PCHAR_STATUS RawStatus, ULONG ChrID, PCHAR_STATUS FinalStatus);
    DECL_STATIC_METHOD_POINTER(EDAO, CalcChrRawStatusByFinalStatus);
/*
    static ULONG GetWindowWidth()
    {
        return *(PULONG)PtrAdd(GlobalGetEDAO(), 0x3084);
    }

    static ULONG GetWindowHeight()
    {
        return *(PULONG)PtrAdd(GlobalGetEDAO(), 0x3088);
    }

    VOID THISCALL ShowDebugText(LPCSTR lpText, INT x, INT y, INT color, INT par5, INT par6, INT par7)
    {
        DETOUR_METHOD(EDAO, ShowDebugText, 0x006729FB, lpText, x, y, color, par5, par6, par7);
    }

    VOID THISCALL ShowDebugTextPositionRestore1(LPCSTR lpText, INT x, INT y, INT color, INT par5, INT par6, INT par7)
    {
        x = (x-1)  * 480 / EDAO::GetWindowWidth() + 1;
        y = (y-1) * 272 / EDAO::GetWindowHeight() + 1;
        ShowDebugText(lpText, x, y, color, par5, par6, par7);
    }

    VOID THISCALL ShowDebugTextPositionRestore2(LPCSTR lpText, INT x, INT y, INT color, INT par5, INT par6, INT par7)
    {
        x = x * 480 / EDAO::GetWindowWidth();
        y = y * 272 / EDAO::GetWindowHeight();
        ShowDebugText(lpText, x, y, color, par5, par6, par7);
    }
*/
};

INIT_STATIC_MEMBER(EDAO::StubCheckItemEquipped);
INIT_STATIC_MEMBER(EDAO::StubGetDifficulty);
INIT_STATIC_MEMBER(EDAO::StubCalcChrT_Status);
INIT_STATIC_MEMBER(EDAO::StubCalcChrRawStatusFromLevel);
INIT_STATIC_MEMBER(EDAO::StubCalcChrRawStatusByFinalStatus);
DECL_SELECTANY TYPE_OF(EDAO::ChrT_Status) EDAO::ChrT_Status;

class CCoordConverter
{
public:
    VOID MapPSPCoordToPC(Gdiplus::PointF Source[2], Gdiplus::PointF Target[2], PFLOAT Transform = nullptr)
    {
        DETOUR_METHOD(CCoordConverter, MapPSPCoordToPC, 0x6A7030, Source, Target, Transform);
    }
};

class CMiniGame
{
public:

    static VOID FASTCALL HorrorHouse_GetMonsterPosition(CCoordConverter *Converter, PVOID, Gdiplus::PointF *PSPCoord, Gdiplus::PointF *PCCoord, PFLOAT Transform)
    {
        EDAO *edao = EDAO::GlobalGetEDAO();

        Converter->MapPSPCoordToPC(PSPCoord, PCCoord, Transform);

        PCCoord->X *= PSP_WIDTH_F / edao->GetWindowSize()->cx;
        PCCoord->Y *= PSP_HEIGHT_F / edao->GetWindowSize()->cy;
    }
};

class CGlobal
{
public:
    PCRAFT_INFO THISCALL GetMagicData(USHORT MagicId);
    PCSTR       THISCALL GetMagicDescription(USHORT MagicId);
    PBYTE       THISCALL GetMagicQueryTable(USHORT MagicId);

    EDAO* GetEDAO()
    {
        return (EDAO *)PtrSub(this, 0x4D3E8);
    }

    BOOL IsCustomChar(ULONG_PTR ChrId)
    {
        return GetEDAO()->GetSaveData()->IsCustomChar(ChrId);
    }

    PCHAR_STATUS GetChrStatus(ULONG_PTR ChrId)
    {
        return &((PCHAR_STATUS)PtrAdd(this, 0x2BBBC))[ChrId];
    }

    VOID CalcChrFinalStatus(ULONG ChrId, PCHAR_STATUS FinalStatus, PCHAR_STATUS RawStatus)
    {
        //DETOUR_METHOD(CGlobal, CalcChrFinalStatus, 0x677B36, ChrId, FinalStatus, RawStatus);
        TYPE_OF(&CGlobal::CalcChrFinalStatus) f;

        *(PVOID *)&f = (PVOID)0x677B36;

        return (this->*f)(ChrId, FinalStatus, RawStatus);
    }

    PITEM_ENTRY THISCALL GetItemEntry(PITEM_ENTRY pItemEntry, ULONG ItemId)
    {
        DETOUR_METHOD(CGlobal, GetItemEntry, 0x00673991, pItemEntry, ItemId);
    }

    BOOL THISCALL SubItem(ULONG ItemId, ULONG Count)
    {
        DETOUR_METHOD(CGlobal, SubItem, 0x00675EEE, ItemId, Count);
    }

    BOOL THISCALL DoEffect(ULONG ActionType, ULONG SrcChrId, ULONG DstChrId, ULONG Effect, PCOORD pEffectPar, ULONG par6, BOOL IsPlaySound)
    {
        DETOUR_METHOD(CGlobal, DoEffect, 0x006772A8, ActionType, SrcChrId, DstChrId, Effect, pEffectPar, par6, IsPlaySound);
    }

    PBYTE THISCALL FixWeaponShapeAndRange(USHORT ItemId);

    DECL_STATIC_METHOD_POINTER(CGlobal, FixWeaponShapeAndRange);

    //mark
    BOOL THISCALL AddCraft(ULONG ChrId, ULONG Craft);
    DECL_STATIC_METHOD_POINTER(CGlobal, AddCraft);

    BOOL THISCALL UseItemDouguOld(ULONG ItemId, ULONG ChrId);
    BOOL THISCALL UseItemDouguFix(ULONG ItemId, ULONG ChrId);

    static TYPE_OF(&CGlobal::GetMagicData)          StubGetMagicData;
    static TYPE_OF(&CGlobal::GetMagicDescription)   StubGetMagicDescription;
    static TYPE_OF(&CGlobal::GetMagicQueryTable)    StubGetMagicQueryTable;
};

DECL_SELECTANY TYPE_OF(CGlobal::StubGetMagicData)           CGlobal::StubGetMagicData = nullptr;
DECL_SELECTANY TYPE_OF(CGlobal::StubGetMagicDescription)    CGlobal::StubGetMagicDescription = nullptr;
DECL_SELECTANY TYPE_OF(CGlobal::StubGetMagicQueryTable)     CGlobal::StubGetMagicQueryTable = nullptr;


INIT_STATIC_MEMBER(CGlobal::StubFixWeaponShapeAndRange);

INIT_STATIC_MEMBER(CGlobal::StubAddCraft);

BOOL AoIsFileExist(PCSTR FileName);

//mark
class CDebug
{
public:
    VOID THISCALL SelectPartyChr()
    {
        DETOUR_METHOD(CDebug, SelectPartyChr, 0x672F82);
    }

    VOID THISCALL MainControl()
    {
        //TYPE_OF(&GetAsyncKeyState) StubGetAsyncKeyState;
        //*(PULONG_PTR)&StubGetAsyncKeyState = *(PULONG_PTR)0xDD5A18;

        if (FLAG_ON(StubGetAsyncKeyState(VK_CONTROL), 0x8000))
        {
            static BOOL IsSelectPartyChr;
            if (FLAG_ON_ALL(StubGetAsyncKeyState('P'), 0x8001) && !IsSelectPartyChr)
            {
                IsSelectPartyChr = TRUE;
                SelectPartyChr();
                IsSelectPartyChr = FALSE;
                return;
            }
        }

        return (this->*StubMainControl)();
    }

    BOOL THISCALL IsFileExist(PCSTR FileName);
    DECL_STATIC_METHOD_POINTER(CDebug, MainControl);
};

INIT_STATIC_MEMBER(CDebug::StubMainControl);

#endif // _EDAO_H_5c8a3013_4334_4138_9413_3d0209da878e_
