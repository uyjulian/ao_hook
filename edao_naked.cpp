
#include "edao.h"

NAKED VOID CBattle::NakedAS_8D_5F()
{
    INLINE_ASM
    {
        mov     ecx, [ebp - 0Ch];
        push	[ebp + 08h];
        call    CBattle::AS_8D_5F;
        push	009D3675h;
        retn;
    }
}
