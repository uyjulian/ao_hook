#include "edao.h"
#include "ml/mylibrary.h"

// Aki


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


NAKED VOID CBattle::NakedHandleConditionBeforeMasterQuartzKipaTakeEffect()
{
    INLINE_ASM
    {
        MOV EDX,DWORD PTR SS:[EBP-0x2C]
        MOV ECX,DWORD PTR SS:[EBP-0x8];
        jmp CBattle::HandleConditionBeforeMasterQuartzKipaTakeEffect;
    }
}


