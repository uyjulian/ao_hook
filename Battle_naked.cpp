#include "edao.h"

NAKED VOID CBattle::NakedCopyMagicAndCraftData()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 08h];
        mov     edx, [ebp + 08h];
        and     dword ptr [ebp - 20h], 0;
        jmp     CBattle::CopyMagicAndCraftData
    }
}


NAKED VOID CBattle::NakedOverWriteBattleStatusWithChrStatus()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 08h];
        mov     edx, edi;
        push    eax;
        call    CBattle::OverWriteBattleStatusWithChrStatus
        ret;
    }
}


NAKED VOID CBattle::NakedIsChrStatusNeedRefresh()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 0Ch];
        mov     edx, dword ptr [ebp - 8Ch];
        push    dword ptr [ebp - 25E8h];
        push    eax;
        call    CBattle::IsChrStatusNeedRefresh
        neg     eax;
        ret;
    }
}


NAKED ULONG CBattle::NakedGetChrIdForSCraft()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 08h];
        call    CBattle::GetSaveData
        mov     ecx, eax;
        call    CSSaveData::GetPartyChipMap
        mov     ecx, dword ptr [ebp - 14h];
        movzx   ecx, [ecx]MONSTER_STATUS.CharID;
        movzx   eax, [eax + ecx * 2];
        cmp     eax, MINIMUM_CUSTOM_CHAR_ID
        cmovae  ecx, eax;
        ret;
    }
}


/*++

  mov     word ptr [ebp-const], r16
  mov     r32, const
  mov     word ptr [ebp-const], r16
  mov     r32, const
  mov     word ptr [ebp-const], r16
  mov     r32, [ebp+8]
  movzx   r32, [r32+0A]

--*/

NAKED VOID CBattle::NakedGetTurnVoiceChrId()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 0Ch];
        mov     edx, [ebp + 08h];
        call    CBattle::GetVoiceChrIdWorker;
        mov     ecx, eax;
        ret;
    }
}

NAKED VOID CBattle::NakedGetReplySupportVoiceChrId()
{
    INLINE_ASM
    {
        jmp CBattle::NakedGetTurnVoiceChrId;
    }
}

NAKED VOID CBattle::NakedGetRunawayVoiceChrId()
{
    INLINE_ASM
    {
        jmp CBattle::NakedGetTurnVoiceChrId;
    }
}

NAKED VOID CBattle::NakedGetTeamRushVoiceChrId()
{
    INLINE_ASM
    {
        jmp CBattle::NakedGetTurnVoiceChrId;
    }
}

NAKED VOID CBattle::NakedGetUnderAttackVoiceChrId()
{
    INLINE_ASM
    {
        call    CBattle::NakedGetTurnVoiceChrId
        mov     dword ptr [ebp-0xF8], eax;
        ret;
    }
}

NAKED VOID CBattle::NakedGetUnderAttackVoiceChrId2()
{
    INLINE_ASM
    {
        jmp CBattle::NakedGetTurnVoiceChrId;
    }
}

NAKED VOID CBattle::NakedGetSBreakVoiceChrId()
{
    INLINE_ASM
    {
        jmp CBattle::NakedGetTurnVoiceChrId;
    }
}


NAKED VOID CBattle::NakedGetBattleState()
{
    static ULONG PreviousState;

    INLINE_ASM
    {
        mov     dword ptr [ebp - 294h], ecx;
        mov     edx, dword ptr [eax + 113078h];
        mov     ecx, eax;
        jmp     CBattle::HandleBattleState
    }
}

NAKED LONG CBattle::NakedEnemyThinkAction()
{
    INLINE_ASM
    {
        mov     dword ptr [ebp - 114h], eax;
        mov     edx, [ebp + 08h];
        mov     ecx, [ebp - 0Ch];
        call    CBattle::EnemyThinkAction
        or      eax, eax;
        mov     eax, 991CBDh;
        cmove   eax, [esp];
        mov     [esp], eax;
        ret;
    }
}


NAKED VOID CBattle::NakedAS8DDispatcher()
{
    INLINE_ASM
    {
        push    ecx;
        mov     ecx, [ebp - 0Ch];
        mov     edx, [ebp + 08h];
        mov     eax, [ebp + 0Ch];
        mov     eax, [eax + 20h];
        lea     eax, [eax - 12h];
        push    eax;
        call    CBattle::AS8DDispatcher
        pop     ecx;
        cmp     ecx, 0x62;
        ret;
    }
}


NAKED VOID CBattle::NakedNoResistConditionUp()
{
    enum
    {
        Conditions =    CraftConditions::StrUp |
                        CraftConditions::DefUp |
                        CraftConditions::AtsUp |
                        CraftConditions::AdfUp |
                        CraftConditions::DexUp |
                        CraftConditions::AglUp |
                        CraftConditions::MovUp |
                        CraftConditions::SpdUp,
    };

    INLINE_ASM
    {
        mov     edx, [ebp-0x15C];
        test    edx, Conditions;
        je      _RET2;

        and     ecx, edx;
        je      _RET;

        mov     eax, [ebp + 0x18];
        and     eax, 1 << 15;

_RET:
        ret;

_RET2:
        and     ecx, edx;
        ret;

    }
}


NAKED VOID CBattle::NakedFindReplaceChr()
{
    INLINE_ASM
    {
        mov     dword ptr [ebp - 24h], 0;
        mov     ecx, [ebp + 8h];
        lea     edx, [ebp - 70h];
        call    FindReplaceChr
        ret;
    }
}


NAKED VOID CBattle::NakedCheckCraftTargetBits()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 044h];
        mov     edx, [ebp - 12Ch];
        call    CheckPartyCraftTargetBits
        mov     [ebp - 0CCh], eax;
        ret;
    }
}


NAKED VOID CBattle::NakedUpdateCraftReflectLeftTime()
{
    INLINE_ASM
    {
        push    [ebp - 020h];
        mov     ecx, [ebp - 08h];
        movzx   edx, al;
        call    UpdateCraftReflectLeftTime
        test    eax, eax;
        ret;
    }
}


NAKED VOID EDAO::NakedGetChrSBreak()
{
    INLINE_ASM
    {
        mov     ecx, eax;
        jmp     EDAO::GetChrSBreak;
    }
}

