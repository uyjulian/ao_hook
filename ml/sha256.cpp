#include "mylibrary.h"


#if MY_OS_WIN32

#if !MY_X64

#define SHA224_DIGEST_SIZE ( 224 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA384_DIGEST_SIZE ( 384 / 8)
#define SHA512_DIGEST_SIZE ( 512 / 8)

#define SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA512_BLOCK_SIZE  (1024 / 8)
#define SHA384_BLOCK_SIZE  SHA512_BLOCK_SIZE
#define SHA224_BLOCK_SIZE  SHA256_BLOCK_SIZE

#pragma warning(push, 0)

ASM VOID STDCALL sha256_update(sha256_ctx *ctx, PVOID message, ULONG len)
{
    static ULONG sha256_k[] =
    {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
        0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
        0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
        0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
        0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
        0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
        0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
    };

    INLINE_ASM
    {
        sub     esp, 0x128
        mov     dword ptr [esp+0xC], ebp
        mov     dword ptr [esp+0x8], ebx
        mov     dword ptr [esp+0x4], edi
        mov     dword ptr [esp], esi
        mov     esi, dword ptr [esp+0x12C]
        mov     eax, dword ptr [esi]
        mov     ebx, dword ptr [esi+0x4]
        mov     ecx, dword ptr [esi+0x8]
        mov     edx, dword ptr [esi+0xC]
        mov     ebp, dword ptr [esi+0x10]
        mov     edi, dword ptr [esi+0x14]
        bswap   eax
        bswap   ebx
        bswap   ecx
        bswap   edx
        bswap   ebp
        bswap   edi
        mov     dword ptr [esp+0x28], eax
        mov     dword ptr [esp+0x2C], ebx
        mov     dword ptr [esp+0x30], ecx
        mov     dword ptr [esp+0x34], edx
        mov     dword ptr [esp+0x38], ebp
        mov     dword ptr [esp+0x3C], edi
        mov     eax, dword ptr [esi+0x18]
        mov     ebx, dword ptr [esi+0x1C]
        mov     ecx, dword ptr [esi+0x20]
        mov     edx, dword ptr [esi+0x24]
        mov     ebp, dword ptr [esi+0x28]
        mov     edi, dword ptr [esi+0x2C]
        bswap   eax
        bswap   ebx
        bswap   ecx
        bswap   edx
        bswap   ebp
        bswap   edi
        mov     dword ptr [esp+0x40], eax
        mov     dword ptr [esp+0x44], ebx
        mov     dword ptr [esp+0x48], ecx
        mov     dword ptr [esp+0x4C], edx
        mov     dword ptr [esp+0x50], ebp
        mov     dword ptr [esp+0x54], edi
        mov     eax, dword ptr [esi+0x30]
        mov     ebx, dword ptr [esi+0x34]
        mov     ecx, dword ptr [esi+0x38]
        mov     edx, dword ptr [esi+0x3C]
        bswap   eax
        bswap   ebx
        bswap   ecx
        bswap   edx
        mov     dword ptr [esp+0x58], eax
        mov     dword ptr [esp+0x5C], ebx
        mov     dword ptr [esp+0x60], ecx
        mov     dword ptr [esp+0x64], edx
        mov     ecx, -0xC0
        mov     esi, dword ptr [esp+0x130]
        jmp L057;
        ASM_DUMMY(6);
        ASM_DUMMY(6);
        ASM_DUMMY(6);
        ASM_DUMMY(5);
L057:
            mov     eax, dword ptr [esp+ecx+0xEC]
            mov     ebx, dword ptr [esp+ecx+0x120]
            mov     edi, dword ptr [esp+ecx+0x10C]
            mov     ebp, eax
            mov     edx, ebx
            ror     eax, 0x7
            shr     ebp, 0x3
            ror     ebx, 0x11
            shr     edx, 0xA
            xor     ebp, eax
            ror     eax, 0xB
            xor     edx, ebx
            ror     ebx, 0x2
            xor     eax, ebp
            add     edi, dword ptr [esp+ecx+0xE8]
            xor     ebx, edx
            add     edi, eax
            add     edi, ebx
            mov     dword ptr [esp+ecx+0x128], edi
            add     ecx, 0x4
        jnz L057
        mov     eax, dword ptr [esi]
        mov     ebx, dword ptr [esi+0x4]
        mov     ecx, dword ptr [esi+0x8]
        mov     edx, dword ptr [esi+0xC]
        mov     ebp, dword ptr [esi+0x10]
        mov     edi, dword ptr [esi+0x14]
        mov     dword ptr [esp+0x10], ebx
        mov     dword ptr [esp+0x14], ecx
        mov     dword ptr [esp+0x18], edx
        mov     dword ptr [esp+0x1C], edi
        mov     ebx, dword ptr [esi+0x18]
        mov     edx, dword ptr [esi+0x1C]
        mov     dword ptr [esp+0x20], ebx
        mov     ecx, -0x100
        ASM_DUMMY(6);
L093:
            mov     ebx, ebp
            add     edx, dword ptr [esp+ecx+0x128]
            mov     edi, ebp
            ror     ebx, 0x6
            mov     esi, ebp
            ror     edi, 0xB
            add     edx, dword ptr [ecx+sha256_k+100h]
            ror     esi, 0x19
            xor     ebx, edi
            mov     edi, dword ptr [esp+0x20]
            xor     ebx, esi
            mov     esi, dword ptr [esp+0x1C]
            mov     dword ptr [esp+0x1C], ebp
            mov     dword ptr [esp+0x20], esi
            xor     esi, edi
            mov     dword ptr [esp+0x24], edi
            add     edx, ebx
            and     esi, ebp
            mov     ebx, dword ptr [esp+0x10]
            xor     esi, edi
            mov     edi, dword ptr [esp+0x14]
            add     edx, esi
            mov     ebp, ebx
            mov     dword ptr [esp+0x14], ebx
            xor     ebx, eax
            mov     dword ptr [esp+0x10], eax
            and     ebp, eax
            and     ebx, edi
            mov     esi, eax
            add     ebx, ebp
            ror     esi, 0x2
            mov     ebp, eax
            ror     eax, 0xD
            xor     eax, esi
            ror     ebp, 0x16
            xor     eax, ebp
            mov     ebp, dword ptr [esp+0x18]
            mov     dword ptr [esp+0x18], edi
            add     ebp, edx
            add     eax, edx
            mov     edx, dword ptr [esp+0x24]
            add     eax, ebx
            add     ecx, 0x4
        jnz L093
        mov     esi, dword ptr [esp+0x130]
        mov     ebx, dword ptr [esp+0x10]
        mov     ecx, dword ptr [esp+0x14]
        add     dword ptr [esi], eax
        add     dword ptr [esi+0x4], ebx
        mov     ebx, dword ptr [esp+0x1C]
        mov     eax, dword ptr [esp+0x20]
        add     dword ptr [esi+0x8], ecx
        add     dword ptr [esi+0xC], edi
        add     dword ptr [esi+0x10], ebp
        add     dword ptr [esi+0x14], ebx
        add     dword ptr [esi+0x18], eax
        add     dword ptr [esi+0x1C], edx
        mov     esi, dword ptr [esp]
        mov     edi, dword ptr [esp+0x4]
        mov     ebx, dword ptr [esp+0x8]
        mov     ebp, dword ptr [esp+0xC]
        add     esp, 0x128
        ret     0x8

    }
}

ASM VOID STDCALL sha256(PVOID message, ULONG len, PVOID digest)
{
    INLINE_ASM
    {
          push    ebp
          mov     ebp, esp
          add     esp, -0x80
          push    edi
          push    esi
          mov     edx, dword ptr [ebp+0x10]
          mov     edi, dword ptr [ebp+0xC]
          mov     esi, dword ptr [ebp+0x8]
          mov     dword ptr [edx], 0x6A09E667
          mov     dword ptr [edx+0x4], 0xBB67AE85
          mov     dword ptr [edx+0x8], 0x3C6EF372
          mov     dword ptr [edx+0xC], 0xA54FF53A
          mov     dword ptr [edx+0x10], 0x510E527F
          mov     dword ptr [edx+0x14], 0x9B05688C
          mov     dword ptr [edx+0x18], 0x1F83D9AB
          mov     dword ptr [edx+0x1C], 0x5BE0CD19
          shr     edi, 0x6
          je L024
L018:
          push    dword ptr [ebp+0x10]
          push    esi
          call    sha256_update
          add     esi, 0x40
          dec     edi
          jnz L018
L024:
          mov     ecx, dword ptr [ebp+0xC]
          lea     edi, dword ptr [ebp-0x80]
          and     ecx, 0x3F
          push    dword ptr [ebp+0x10]
          push    edi
          mov     eax, ecx
          cld
          rep     movs byte ptr es:[edi], byte ptr [esi]
          mov     byte ptr [edi], 0x80
          inc     edi
          mov     ecx, eax
          xor     eax, eax
          bswap   esi
          sub     ecx, 0x37
          ja L042
          neg     ecx
          rep     stos byte ptr es:[edi]
          jmp L049
L042:
          sub     ecx, 0x40
          neg     ecx
          rep     stos byte ptr es:[edi]
          call    sha256_update
          lea     edx, dword ptr [ebp-0x40]
          push    dword ptr [ebp+0x10]
          push    edx
L049:
          mov     esi, dword ptr [ebp+0xC]
          mov     eax, esi
          rol     esi, 0x3
          shl     eax, 0x3
          and     esi, 0x7
          bswap   eax
          bswap   esi
          mov     dword ptr [edi+0x4], eax
          mov     dword ptr [edi], esi
          call    sha256_update
          pop     esi
          pop     edi
          leave
          ret     0xC
    }
}

#pragma warning(pop)

#endif // MY_X64
#endif // MY_OS_WIN32

