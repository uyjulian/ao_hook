#include "mylibrary.h"



#pragma warning(default:4750 4995)
//#include "Hook.cpp"
//#include "HookHelper.cpp"
/*
  ___________________________________________________
  Opcode Length Disassembler.
  Coded By Ms-Rem ( Ms-Rem@yandex.ru ) ICQ 286370715
  ---------------------------------------------------
  12.08.2005 - fixed many bugs...
  09.08.2005 - fixed bug with 0F BA opcode.
  07.08.2005 - added SSE, SSE2, SSE3 and 3Dnow instruction support.
  06.08.2005 - fixed bug with F6 and F7 opcodes.
  29.07.2005 - fixed bug with OP_X64_WORD opcodes.
*/


#define OP_X64_NONE           0x00
#define OP_X64_MODRM          0x01
#define OP_X64_DATA_I8        0x02
#define OP_X64_DATA_I16       0x04
#define OP_X64_DATA_I32       0x08
#define OP_X64_DATA_PRE66_67  0x10
#define OP_X64_WORD           0x20
#define OP_X64_REL32          0x40

ForceInline PUCHAR GetOpCodeFlags()
{
    static UCHAR OpcodeFlags[256] =
    {
        OP_X64_MODRM,                      // 00
        OP_X64_MODRM,                      // 01
        OP_X64_MODRM,                      // 02
        OP_X64_MODRM,                      // 03
        OP_X64_DATA_I8,                    // 04
        OP_X64_DATA_PRE66_67,              // 05
        OP_X64_NONE,                       // 06
        OP_X64_NONE,                       // 07
        OP_X64_MODRM,                      // 08
        OP_X64_MODRM,                      // 09
        OP_X64_MODRM,                      // 0A
        OP_X64_MODRM,                      // 0B
        OP_X64_DATA_I8,                    // 0C
        OP_X64_DATA_PRE66_67,              // 0D
        OP_X64_NONE,                       // 0E
        OP_X64_NONE,                       // 0F
        OP_X64_MODRM,                      // 10
        OP_X64_MODRM,                      // 11
        OP_X64_MODRM,                      // 12
        OP_X64_MODRM,                      // 13
        OP_X64_DATA_I8,                    // 14
        OP_X64_DATA_PRE66_67,              // 15
        OP_X64_NONE,                       // 16
        OP_X64_NONE,                       // 17
        OP_X64_MODRM,                      // 18
        OP_X64_MODRM,                      // 19
        OP_X64_MODRM,                      // 1A
        OP_X64_MODRM,                      // 1B
        OP_X64_DATA_I8,                    // 1C
        OP_X64_DATA_PRE66_67,              // 1D
        OP_X64_NONE,                       // 1E
        OP_X64_NONE,                       // 1F
        OP_X64_MODRM,                      // 20
        OP_X64_MODRM,                      // 21
        OP_X64_MODRM,                      // 22
        OP_X64_MODRM,                      // 23
        OP_X64_DATA_I8,                    // 24
        OP_X64_DATA_PRE66_67,              // 25
        OP_X64_NONE,                       // 26
        OP_X64_NONE,                       // 27
        OP_X64_MODRM,                      // 28
        OP_X64_MODRM,                      // 29
        OP_X64_MODRM,                      // 2A
        OP_X64_MODRM,                      // 2B
        OP_X64_DATA_I8,                    // 2C
        OP_X64_DATA_PRE66_67,              // 2D
        OP_X64_NONE,                       // 2E
        OP_X64_NONE,                       // 2F
        OP_X64_MODRM,                      // 30
        OP_X64_MODRM,                      // 31
        OP_X64_MODRM,                      // 32
        OP_X64_MODRM,                      // 33
        OP_X64_DATA_I8,                    // 34
        OP_X64_DATA_PRE66_67,              // 35
        OP_X64_NONE,                       // 36
        OP_X64_NONE,                       // 37
        OP_X64_MODRM,                      // 38
        OP_X64_MODRM,                      // 39
        OP_X64_MODRM,                      // 3A
        OP_X64_MODRM,                      // 3B
        OP_X64_DATA_I8,                    // 3C
        OP_X64_DATA_PRE66_67,              // 3D
        OP_X64_NONE,                       // 3E
        OP_X64_NONE,                       // 3F
        OP_X64_NONE,                       // 40
        OP_X64_NONE,                       // 41
        OP_X64_NONE,                       // 42
        OP_X64_NONE,                       // 43
        OP_X64_NONE,                       // 44
        OP_X64_NONE,                       // 45
        OP_X64_NONE,                       // 46
        OP_X64_NONE,                       // 47
        OP_X64_NONE,                       // 48
        OP_X64_NONE,                       // 49
        OP_X64_NONE,                       // 4A
        OP_X64_NONE,                       // 4B
        OP_X64_NONE,                       // 4C
        OP_X64_NONE,                       // 4D
        OP_X64_NONE,                       // 4E
        OP_X64_NONE,                       // 4F
        OP_X64_NONE,                       // 50
        OP_X64_NONE,                       // 51
        OP_X64_NONE,                       // 52
        OP_X64_NONE,                       // 53
        OP_X64_NONE,                       // 54
        OP_X64_NONE,                       // 55
        OP_X64_NONE,                       // 56
        OP_X64_NONE,                       // 57
        OP_X64_NONE,                       // 58
        OP_X64_NONE,                       // 59
        OP_X64_NONE,                       // 5A
        OP_X64_NONE,                       // 5B
        OP_X64_NONE,                       // 5C
        OP_X64_NONE,                       // 5D
        OP_X64_NONE,                       // 5E
        OP_X64_NONE,                       // 5F
        OP_X64_NONE,                       // 60
        OP_X64_NONE,                       // 61
        OP_X64_MODRM,                      // 62
        OP_X64_MODRM,                      // 63
        OP_X64_NONE,                       // 64
        OP_X64_NONE,                       // 65
        OP_X64_NONE,                       // 66
        OP_X64_NONE,                       // 67
        OP_X64_DATA_PRE66_67,              // 68
        OP_X64_MODRM | OP_X64_DATA_PRE66_67,   // 69
        OP_X64_DATA_I8,                    // 6A
        OP_X64_MODRM | OP_X64_DATA_I8,         // 6B
        OP_X64_NONE,                       // 6C
        OP_X64_NONE,                       // 6D
        OP_X64_NONE,                       // 6E
        OP_X64_NONE,                       // 6F
        OP_X64_DATA_I8,                    // 70
        OP_X64_DATA_I8,                    // 71
        OP_X64_DATA_I8,                    // 72
        OP_X64_DATA_I8,                    // 73
        OP_X64_DATA_I8,                    // 74
        OP_X64_DATA_I8,                    // 75
        OP_X64_DATA_I8,                    // 76
        OP_X64_DATA_I8,                    // 77
        OP_X64_DATA_I8,                    // 78
        OP_X64_DATA_I8,                    // 79
        OP_X64_DATA_I8,                    // 7A
        OP_X64_DATA_I8,                    // 7B
        OP_X64_DATA_I8,                    // 7C
        OP_X64_DATA_I8,                    // 7D
        OP_X64_DATA_I8,                    // 7E
        OP_X64_DATA_I8,                    // 7F
        OP_X64_MODRM | OP_X64_DATA_I8,         // 80
        OP_X64_MODRM | OP_X64_DATA_PRE66_67,   // 81
        OP_X64_MODRM | OP_X64_DATA_I8,         // 82
        OP_X64_MODRM | OP_X64_DATA_I8,         // 83
        OP_X64_MODRM,                      // 84
        OP_X64_MODRM,                      // 85
        OP_X64_MODRM,                      // 86
        OP_X64_MODRM,                      // 87
        OP_X64_MODRM,                      // 88
        OP_X64_MODRM,                      // 89
        OP_X64_MODRM,                      // 8A
        OP_X64_MODRM,                      // 8B
        OP_X64_MODRM,                      // 8C
        OP_X64_MODRM,                      // 8D
        OP_X64_MODRM,                      // 8E
        OP_X64_MODRM,                      // 8F
        OP_X64_NONE,                       // 90
        OP_X64_NONE,                       // 91
        OP_X64_NONE,                       // 92
        OP_X64_NONE,                       // 93
        OP_X64_NONE,                       // 94
        OP_X64_NONE,                       // 95
        OP_X64_NONE,                       // 96
        OP_X64_NONE,                       // 97
        OP_X64_NONE,                       // 98
        OP_X64_NONE,                       // 99
        OP_X64_DATA_I16 | OP_X64_DATA_PRE66_67,// 9A
        OP_X64_NONE,                       // 9B
        OP_X64_NONE,                       // 9C
        OP_X64_NONE,                       // 9D
        OP_X64_NONE,                       // 9E
        OP_X64_NONE,                       // 9F
        OP_X64_DATA_PRE66_67,              // A0
        OP_X64_DATA_PRE66_67,              // A1
        OP_X64_DATA_PRE66_67,              // A2
        OP_X64_DATA_PRE66_67,              // A3
        OP_X64_NONE,                       // A4
        OP_X64_NONE,                       // A5
        OP_X64_NONE,                       // A6
        OP_X64_NONE,                       // A7
        OP_X64_DATA_I8,                    // A8
        OP_X64_DATA_PRE66_67,              // A9
        OP_X64_NONE,                       // AA
        OP_X64_NONE,                       // AB
        OP_X64_NONE,                       // AC
        OP_X64_NONE,                       // AD
        OP_X64_NONE,                       // AE
        OP_X64_NONE,                       // AF
        OP_X64_DATA_I8,                    // B0
        OP_X64_DATA_I8,                    // B1
        OP_X64_DATA_I8,                    // B2
        OP_X64_DATA_I8,                    // B3
        OP_X64_DATA_I8,                    // B4
        OP_X64_DATA_I8,                    // B5
        OP_X64_DATA_I8,                    // B6
        OP_X64_DATA_I8,                    // B7
        OP_X64_DATA_PRE66_67,              // B8
        OP_X64_DATA_PRE66_67,              // B9
        OP_X64_DATA_PRE66_67,              // BA
        OP_X64_DATA_PRE66_67,              // BB
        OP_X64_DATA_PRE66_67,              // BC
        OP_X64_DATA_PRE66_67,              // BD
        OP_X64_DATA_PRE66_67,              // BE
        OP_X64_DATA_PRE66_67,              // BF
        OP_X64_MODRM | OP_X64_DATA_I8,         // C0
        OP_X64_MODRM | OP_X64_DATA_I8,         // C1
        OP_X64_DATA_I16,                   // C2
        OP_X64_NONE,                       // C3
        OP_X64_MODRM,                      // C4
        OP_X64_MODRM,                      // C5
        OP_X64_MODRM   | OP_X64_DATA_I8,       // C6
        OP_X64_MODRM   | OP_X64_DATA_PRE66_67, // C7
        OP_X64_DATA_I8 | OP_X64_DATA_I16,      // C8
        OP_X64_NONE,                       // C9
        OP_X64_DATA_I16,                   // CA
        OP_X64_NONE,                       // CB
        OP_X64_NONE,                       // CC
        OP_X64_DATA_I8,                    // CD
        OP_X64_NONE,                       // CE
        OP_X64_NONE,                       // CF
        OP_X64_MODRM,                      // D0
        OP_X64_MODRM,                      // D1
        OP_X64_MODRM,                      // D2
        OP_X64_MODRM,                      // D3
        OP_X64_DATA_I8,                    // D4
        OP_X64_DATA_I8,                    // D5
        OP_X64_NONE,                       // D6
        OP_X64_NONE,                       // D7
        OP_X64_WORD,                       // D8
        OP_X64_WORD,                       // D9
        OP_X64_WORD,                       // DA
        OP_X64_WORD,                       // DB
        OP_X64_WORD,                       // DC
        OP_X64_WORD,                       // DD
        OP_X64_WORD,                       // DE
        OP_X64_WORD,                       // DF
        OP_X64_DATA_I8,                    // E0
        OP_X64_DATA_I8,                    // E1
        OP_X64_DATA_I8,                    // E2
        OP_X64_DATA_I8,                    // E3
        OP_X64_DATA_I8,                    // E4
        OP_X64_DATA_I8,                    // E5
        OP_X64_DATA_I8,                    // E6
        OP_X64_DATA_I8,                    // E7
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // E8
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // E9
        OP_X64_DATA_I16 | OP_X64_DATA_PRE66_67,// EA
        OP_X64_DATA_I8,                    // EB
        OP_X64_NONE,                       // EC
        OP_X64_NONE,                       // ED
        OP_X64_NONE,                       // EE
        OP_X64_NONE,                       // EF
        OP_X64_NONE,                       // F0
        OP_X64_NONE,                       // F1
        OP_X64_NONE,                       // F2
        OP_X64_NONE,                       // F3
        OP_X64_NONE,                       // F4
        OP_X64_NONE,                       // F5
        OP_X64_MODRM,                      // F6
        OP_X64_MODRM,                      // F7
        OP_X64_NONE,                       // F8
        OP_X64_NONE,                       // F9
        OP_X64_NONE,                       // FA
        OP_X64_NONE,                       // FB
        OP_X64_NONE,                       // FC
        OP_X64_NONE,                       // FD
        OP_X64_MODRM,                      // FE
        OP_X64_MODRM | OP_X64_REL32            // FF
    };

    return OpcodeFlags;
}

ForceInline PUCHAR GetOpCodeFlagsExt()
{
    static UCHAR OpcodeFlagsExt[256] =
    {
        OP_X64_MODRM,                      // 00
        OP_X64_MODRM,                      // 01
        OP_X64_MODRM,                      // 02
        OP_X64_MODRM,                      // 03
        OP_X64_NONE,                       // 04
        OP_X64_NONE,                       // 05
        OP_X64_NONE,                       // 06
        OP_X64_NONE,                       // 07
        OP_X64_NONE,                       // 08
        OP_X64_NONE,                       // 09
        OP_X64_NONE,                       // 0A
        OP_X64_NONE,                       // 0B
        OP_X64_NONE,                       // 0C
        OP_X64_MODRM,                      // 0D
        OP_X64_NONE,                       // 0E
        OP_X64_MODRM | OP_X64_DATA_I8,         // 0F
        OP_X64_MODRM,                      // 10
        OP_X64_MODRM,                      // 11
        OP_X64_MODRM,                      // 12
        OP_X64_MODRM,                      // 13
        OP_X64_MODRM,                      // 14
        OP_X64_MODRM,                      // 15
        OP_X64_MODRM,                      // 16
        OP_X64_MODRM,                      // 17
        OP_X64_MODRM,                      // 18
        OP_X64_NONE,                       // 19
        OP_X64_NONE,                       // 1A
        OP_X64_NONE,                       // 1B
        OP_X64_NONE,                       // 1C
        OP_X64_NONE,                       // 1D
        OP_X64_NONE,                       // 1E
        OP_X64_NONE,                       // 1F
        OP_X64_MODRM,                      // 20
        OP_X64_MODRM,                      // 21
        OP_X64_MODRM,                      // 22
        OP_X64_MODRM,                      // 23
        OP_X64_MODRM,                      // 24
        OP_X64_NONE,                       // 25
        OP_X64_MODRM,                      // 26
        OP_X64_NONE,                       // 27
        OP_X64_MODRM,                      // 28
        OP_X64_MODRM,                      // 29
        OP_X64_MODRM,                      // 2A
        OP_X64_MODRM,                      // 2B
        OP_X64_MODRM,                      // 2C
        OP_X64_MODRM,                      // 2D
        OP_X64_MODRM,                      // 2E
        OP_X64_MODRM,                      // 2F
        OP_X64_NONE,                       // 30
        OP_X64_NONE,                       // 31
        OP_X64_NONE,                       // 32
        OP_X64_NONE,                       // 33
        OP_X64_NONE,                       // 34
        OP_X64_NONE,                       // 35
        OP_X64_NONE,                       // 36
        OP_X64_NONE,                       // 37
        OP_X64_NONE,                       // 38
        OP_X64_NONE,                       // 39
        OP_X64_NONE,                       // 3A
        OP_X64_NONE,                       // 3B
        OP_X64_NONE,                       // 3C
        OP_X64_NONE,                       // 3D
        OP_X64_NONE,                       // 3E
        OP_X64_NONE,                       // 3F
        OP_X64_MODRM,                      // 40
        OP_X64_MODRM,                      // 41
        OP_X64_MODRM,                      // 42
        OP_X64_MODRM,                      // 43
        OP_X64_MODRM,                      // 44
        OP_X64_MODRM,                      // 45
        OP_X64_MODRM,                      // 46
        OP_X64_MODRM,                      // 47
        OP_X64_MODRM,                      // 48
        OP_X64_MODRM,                      // 49
        OP_X64_MODRM,                      // 4A
        OP_X64_MODRM,                      // 4B
        OP_X64_MODRM,                      // 4C
        OP_X64_MODRM,                      // 4D
        OP_X64_MODRM,                      // 4E
        OP_X64_MODRM,                      // 4F
        OP_X64_MODRM,                      // 50
        OP_X64_MODRM,                      // 51
        OP_X64_MODRM,                      // 52
        OP_X64_MODRM,                      // 53
        OP_X64_MODRM,                      // 54
        OP_X64_MODRM,                      // 55
        OP_X64_MODRM,                      // 56
        OP_X64_MODRM,                      // 57
        OP_X64_MODRM,                      // 58
        OP_X64_MODRM,                      // 59
        OP_X64_MODRM,                      // 5A
        OP_X64_MODRM,                      // 5B
        OP_X64_MODRM,                      // 5C
        OP_X64_MODRM,                      // 5D
        OP_X64_MODRM,                      // 5E
        OP_X64_MODRM,                      // 5F
        OP_X64_MODRM,                      // 60
        OP_X64_MODRM,                      // 61
        OP_X64_MODRM,                      // 62
        OP_X64_MODRM,                      // 63
        OP_X64_MODRM,                      // 64
        OP_X64_MODRM,                      // 65
        OP_X64_MODRM,                      // 66
        OP_X64_MODRM,                      // 67
        OP_X64_MODRM,                      // 68
        OP_X64_MODRM,                      // 69
        OP_X64_MODRM,                      // 6A
        OP_X64_MODRM,                      // 6B
        OP_X64_MODRM,                      // 6C
        OP_X64_MODRM,                      // 6D
        OP_X64_MODRM,                      // 6E
        OP_X64_MODRM,                      // 6F
        OP_X64_MODRM | OP_X64_DATA_I8,         // 70
        OP_X64_MODRM | OP_X64_DATA_I8,         // 71
        OP_X64_MODRM | OP_X64_DATA_I8,         // 72
        OP_X64_MODRM | OP_X64_DATA_I8,         // 73
        OP_X64_MODRM,                      // 74
        OP_X64_MODRM,                      // 75
        OP_X64_MODRM,                      // 76
        OP_X64_NONE,                       // 77
        OP_X64_NONE,                       // 78
        OP_X64_NONE,                       // 79
        OP_X64_NONE,                       // 7A
        OP_X64_NONE,                       // 7B
        OP_X64_MODRM,                      // 7C
        OP_X64_MODRM,                      // 7D
        OP_X64_MODRM,                      // 7E
        OP_X64_MODRM,                      // 7F
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 80
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 81
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 82
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 83
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 84
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 85
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 86
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 87
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 88
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 89
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 8A
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 8B
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 8C
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 8D
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 8E
        OP_X64_DATA_PRE66_67 | OP_X64_REL32,   // 8F
        OP_X64_MODRM,                      // 90
        OP_X64_MODRM,                      // 91
        OP_X64_MODRM,                      // 92
        OP_X64_MODRM,                      // 93
        OP_X64_MODRM,                      // 94
        OP_X64_MODRM,                      // 95
        OP_X64_MODRM,                      // 96
        OP_X64_MODRM,                      // 97
        OP_X64_MODRM,                      // 98
        OP_X64_MODRM,                      // 99
        OP_X64_MODRM,                      // 9A
        OP_X64_MODRM,                      // 9B
        OP_X64_MODRM,                      // 9C
        OP_X64_MODRM,                      // 9D
        OP_X64_MODRM,                      // 9E
        OP_X64_MODRM,                      // 9F
        OP_X64_NONE,                       // A0
        OP_X64_NONE,                       // A1
        OP_X64_NONE,                       // A2
        OP_X64_MODRM,                      // A3
        OP_X64_MODRM | OP_X64_DATA_I8,         // A4
        OP_X64_MODRM,                      // A5
        OP_X64_NONE,                       // A6
        OP_X64_NONE,                       // A7
        OP_X64_NONE,                       // A8
        OP_X64_NONE,                       // A9
        OP_X64_NONE,                       // AA
        OP_X64_MODRM,                      // AB
        OP_X64_MODRM | OP_X64_DATA_I8,         // AC
        OP_X64_MODRM,                      // AD
        OP_X64_MODRM,                      // AE
        OP_X64_MODRM,                      // AF
        OP_X64_MODRM,                      // B0
        OP_X64_MODRM,                      // B1
        OP_X64_MODRM,                      // B2
        OP_X64_MODRM,                      // B3
        OP_X64_MODRM,                      // B4
        OP_X64_MODRM,                      // B5
        OP_X64_MODRM,                      // B6
        OP_X64_MODRM,                      // B7
        OP_X64_NONE,                       // B8
        OP_X64_NONE,                       // B9
        OP_X64_MODRM | OP_X64_DATA_I8,         // BA
        OP_X64_MODRM,                      // BB
        OP_X64_MODRM,                      // BC
        OP_X64_MODRM,                      // BD
        OP_X64_MODRM,                      // BE
        OP_X64_MODRM,                      // BF
        OP_X64_MODRM,                      // C0
        OP_X64_MODRM,                      // C1
        OP_X64_MODRM | OP_X64_DATA_I8,         // C2
        OP_X64_MODRM,                      // C3
        OP_X64_MODRM | OP_X64_DATA_I8,         // C4
        OP_X64_MODRM | OP_X64_DATA_I8,         // C5
        OP_X64_MODRM | OP_X64_DATA_I8,         // C6
        OP_X64_MODRM,                      // C7
        OP_X64_NONE,                       // C8
        OP_X64_NONE,                       // C9
        OP_X64_NONE,                       // CA
        OP_X64_NONE,                       // CB
        OP_X64_NONE,                       // CC
        OP_X64_NONE,                       // CD
        OP_X64_NONE,                       // CE
        OP_X64_NONE,                       // CF
        OP_X64_MODRM,                      // D0
        OP_X64_MODRM,                      // D1
        OP_X64_MODRM,                      // D2
        OP_X64_MODRM,                      // D3
        OP_X64_MODRM,                      // D4
        OP_X64_MODRM,                      // D5
        OP_X64_MODRM,                      // D6
        OP_X64_MODRM,                      // D7
        OP_X64_MODRM,                      // D8
        OP_X64_MODRM,                      // D9
        OP_X64_MODRM,                      // DA
        OP_X64_MODRM,                      // DB
        OP_X64_MODRM,                      // DC
        OP_X64_MODRM,                      // DD
        OP_X64_MODRM,                      // DE
        OP_X64_MODRM,                      // DF
        OP_X64_MODRM,                      // E0
        OP_X64_MODRM,                      // E1
        OP_X64_MODRM,                      // E2
        OP_X64_MODRM,                      // E3
        OP_X64_MODRM,                      // E4
        OP_X64_MODRM,                      // E5
        OP_X64_MODRM,                      // E6
        OP_X64_MODRM,                      // E7
        OP_X64_MODRM,                      // E8
        OP_X64_MODRM,                      // E9
        OP_X64_MODRM,                      // EA
        OP_X64_MODRM,                      // EB
        OP_X64_MODRM,                      // EC
        OP_X64_MODRM,                      // ED
        OP_X64_MODRM,                      // EE
        OP_X64_MODRM,                      // EF
        OP_X64_MODRM,                      // F0
        OP_X64_MODRM,                      // F1
        OP_X64_MODRM,                      // F2
        OP_X64_MODRM,                      // F3
        OP_X64_MODRM,                      // F4
        OP_X64_MODRM,                      // F5
        OP_X64_MODRM,                      // F6
        OP_X64_MODRM,                      // F7
        OP_X64_MODRM,                      // F8
        OP_X64_MODRM,                      // F9
        OP_X64_MODRM,                      // FA
        OP_X64_MODRM,                      // FB
        OP_X64_MODRM,                      // FC
        OP_X64_MODRM,                      // FD
        OP_X64_MODRM,                      // FE
        OP_X64_NONE                        // FF
    };

    return OpcodeFlagsExt;
}

ULONG_PTR FASTCALL LdeGetOpCodeSize64(PVOID Code, PVOID *OpCodePtr)
{
    // OpCode High 5 bits as index, (1 << low 3bits) as flag

    static UCHAR PrefixTable[0x20] =
    {
        0x00, 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00
    };

    PUCHAR      Ptr;
    BOOL        PFX66, PFX67;
    BOOL        SibPresent;
    BOOL        Rex;
    ULONG_PTR   Flags;
    ULONG_PTR   Mod, RM, Reg;
    ULONG_PTR   OffsetSize, Imm64;
    ULONG_PTR   Opcode;

    Imm64       = 0;
    OffsetSize  = 0;
    PFX66       = FALSE;
    PFX67       = FALSE;
    Rex         = FALSE;
    Ptr         = (PUCHAR)Code;

//    while ( (*cPtr == 0x2E) || (*cPtr == 0x3E) || (*cPtr == 0x36) ||
//            (*cPtr == 0x26) || (*cPtr == 0x64) || (*cPtr == 0x65) ||
//            (*cPtr == 0xF0) || (*cPtr == 0xF2) || (*cPtr == 0xF3) ||
//            (*cPtr == 0x66) || (*cPtr == 0x67) )
    while (PrefixTable[*Ptr >> 3] & (1 << (*Ptr & 7)))
    {
        PFX66 = *Ptr == 0x66;
        PFX67 = *Ptr == 0x67;
        Ptr++;
        if (Ptr > (PUCHAR)Code + 16)
            return 0;
    }

    // 0x40 ~ 0x4F
    if (((*Ptr) >> 4) == 0x4)
    {
        Rex = *Ptr & 0x0F;
        if (Rex)
            ++Ptr;
    }

    Opcode = *Ptr;
    if (OpCodePtr)
        *OpCodePtr = Ptr;

    if (*Ptr == 0x0F)
    {
        Ptr++;
        Flags = GetOpCodeFlagsExt()[*Ptr];
    }
    else
    {
        ULONG_PTR tmp;

        Flags = GetOpCodeFlags()[Opcode];

        // if (Opcode >= 0xA0 && Opcode <= 0xA3)
        tmp = Opcode >> 2;
        if (tmp == 0x28)
        {
            PFX66 = PFX67;
        }
        else if (Rex && (tmp >> 1) == 0x17)     // 0xB8 ~ 0xBF  mov r64, imm64
        {
            Imm64 = 4;
        }
    }

    Ptr++;
    Ptr += FLAG_ON(Flags, OP_X64_WORD);

    if (Flags & OP_X64_MODRM)
    {
        Mod = *Ptr >> 6;
        Reg = (*Ptr & 0x38) >> 3;
        RM  = *Ptr &  7;
        Ptr++;

        if ((Opcode == 0xF6) && !Reg)
            SET_FLAG(Flags, OP_X64_DATA_I8);

        if ((Opcode == 0xF7) && !Reg)
            SET_FLAG(Flags, OP_X64_DATA_PRE66_67);

        SibPresent = !PFX67 & (RM == 4);
        switch (Mod)
        {
            case 0:
                if (PFX67 && (RM == 6))
                    OffsetSize = 2;
                if (!PFX67 && (RM == 5))
                    OffsetSize = 4;
             break;

            case 1:
                OffsetSize = 1;
                break;

            case 2:
                OffsetSize = PFX67 ? 2 : 4;
                break;

            case 3:
                SibPresent = FALSE;
                break;
        }

        if (SibPresent)
        {
            if (((*Ptr & 7) == 5) && ((!Mod) || (Mod == 2)))
                OffsetSize = 4;

            Ptr++;
        }

        Ptr += OffsetSize;
    }

    Ptr += FLAG_ON(Flags, OP_X64_DATA_I8);
    Ptr += FLAG_ON(Flags, OP_X64_DATA_I16) ? 2 : 0;
    Ptr += FLAG_ON(Flags, OP_X64_DATA_I32) ? 4 : 0;
    Ptr += FLAG_ON(Flags, OP_X64_DATA_PRE66_67) ? (PFX66 ? 2 : 4) : 0;
    Ptr += (Rex & 9) ? Imm64 : 0;   // 0x48 || 0x49

    return PtrOffset(Ptr, Code);
}

ULONG_PTR FASTCALL SizeOfProc(PVOID Proc)
{
    ULONG_PTR   Length;
    PBYTE       pOpcode;
    ULONG_PTR   Result = 0;

    do
    {
        Length = LdeGetOpCodeSize64(Proc, (PVOID *)&pOpcode);
        Result += Length;
        if ((Length == 1) && (*pOpcode == 0xC3))
            break;

        Proc = (PVOID)((ULONG_PTR)Proc + Length);
    } while (Length);

    return Result;
}

BOOL FASTCALL IsRelativeCmd(PBYTE OpCode)
{
    UCHAR Flags;

    if (*OpCode == 0x0F)
    {
        Flags = GetOpCodeFlagsExt()[OpCode[1]];
    }
    else
    {
        Flags = GetOpCodeFlags()[*OpCode];
    }

    return FLAG_ON(Flags, OP_X64_REL32);
}

#define OP_X86_NONE           0x000
#define OP_X86_DATA_I8        0x001
#define OP_X86_DATA_I16       0x002
#define OP_X86_DATA_I32       0x004
#define OP_X86_MODRM          0x008
#define OP_X86_DATA_PRE66_67  0x010
#define OP_X86_PREFIX         0x020
#define OP_X86_REL32          0x040
#define OP_X86_REL8           0x080

/* extended opcode flags (by analyzer) */
#define OP_X86_EXTENDED       0x100

ForceInline PBYTE GetPackedTable()
{
    static BYTE PackedTable[] =
    {
        0x80, 0x84, 0x80, 0x84, 0x80, 0x84, 0x80, 0x84,
        0x80, 0x88, 0x80, 0x88, 0x80, 0x88, 0x80, 0x88,
        0x8c, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b, 0x8b,
        0x90, 0x94, 0x98, 0x8b, 0x9c, 0x9c, 0x9c, 0x9c,
        0xa0, 0x80, 0x80, 0x80, 0x8b, 0x8b, 0xa4, 0x8b,
        0xa8, 0x8b, 0x84, 0x8b, 0xac, 0xac, 0xa8, 0xa8,
        0xb0, 0xb4, 0xb8, 0xbc, 0x80, 0xc0, 0x80, 0x80,
        0x9c, 0xac, 0xc4, 0x8b, 0xc8, 0x90, 0x8b, 0x90,
        0x80, 0x8b, 0x8b, 0xcc, 0x80, 0x80, 0xd0, 0x8b,
        0x80, 0xd4, 0x80, 0x80, 0x8b, 0x8b, 0x8b, 0x8b,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0xd8, 0xdc, 0x8b, 0x80,
        0xe0, 0xe0, 0xe0, 0xe0, 0x80, 0x80, 0x80, 0x80,
        0x8f, 0xcf, 0x8f, 0xdb, 0x80, 0x80, 0xe4, 0x80,
        0xe8, 0xd9, 0x8b, 0x8b, 0x80, 0x80, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xdc,
        0x08, 0x08, 0x08, 0x08, 0x01, 0x10, 0x00, 0x00,
        0x01, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x08, 0x08, 0x20, 0x20, 0x20, 0x20,
        0x10, 0x18, 0x01, 0x09, 0x81, 0x81, 0x81, 0x81,
        0x09, 0x18, 0x09, 0x09, 0x00, 0x00, 0x12, 0x00,
        0x10, 0x10, 0x10, 0x10, 0x01, 0x01, 0x01, 0x01,
        0x09, 0x09, 0x02, 0x00, 0x08, 0x08, 0x09, 0x18,
        0x03, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00, 0x50, 0x50, 0x12, 0x81,
        0x20, 0x00, 0x20, 0x20, 0x00, 0x08, 0x00, 0x09,
        0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00,
        0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x00,
        0x50, 0x50, 0x50, 0x50, 0x00, 0x00, 0x09, 0x08,
        0x08, 0x08, 0x09, 0x08
    };

    return PackedTable;
}

ForceInline ULONG_PTR GetOpCodeFlags32(ULONG_PTR OpCode)
{
    return GetPackedTable()[GetPackedTable()[OpCode / 4] + (OpCode % 4)];
}

ULONG_PTR GetOpCodeExtendedFlags32(unsigned char *addr)
{
    ULONG_PTR opcode;

    opcode = addr[0];
    if (opcode == 0x0F)
    {
        opcode = addr[1] | OP_X86_EXTENDED;
    }

    return GetOpCodeFlags32(opcode) | (opcode & OP_X86_EXTENDED);
}

ULONG_PTR FASTCALL LdeGetOpCodeSize32(PVOID Code, PVOID *OpCodePtr)
{
    unsigned char i_mod, i_rm, i_reg;
    ULONG_PTR op1, op2, flags;
    ULONG_PTR pfx66, pfx67;
    ULONG_PTR osize, oflen;
    PBYTE code;

    pfx66 = 0;
    pfx67 = 0;
    osize = 0;
    oflen = 0;

    code = (PBYTE)Code;
    op1 = *code;

    /* skip preffixes */
    while (GetOpCodeFlags32(op1) & OP_X86_PREFIX)
    {
        switch (op1)
        {
            case 0x66:
                pfx66 = 1;
                break;

            case 0x67:
                pfx67 = 1;
                break;
        }

        osize++;
        op1 = *++code;
    }

    /* get opcode size and flags */
    if (OpCodePtr != NULL)
        *OpCodePtr = code;

    ++code;
    osize++;

    if (op1 == 0x0F)
    {
        op2 = (*code | OP_X86_EXTENDED);
        code++;
        osize++;
    }
    else
    {
        op2 = op1;

        /* pfx66 = pfx67 for opcodes A0 - A3 */
        if (op2 >= 0xA0 && op2 <= 0xA3)
        {
                pfx66 = pfx67;
        }
    }

    flags = GetOpCodeFlags32(op2);

    /* process MODRM byte */
    if (flags & OP_X86_MODRM)
    {
        i_mod = (*code >> 6);
        i_reg = (*code & 0x38) >> 3;
        i_rm  = (*code & 7);
        code++;
        osize++;

        /* in F6 and F7 opcodes, immediate value present if i_reg == 0 */
        if (op1 == 0xF6 && i_reg == 0)
        {
            flags |= OP_X86_DATA_I8;
        }
        if (op1 == 0xF7 && i_reg == 0)
        {
            flags |= OP_X86_DATA_PRE66_67;
        }

        switch (i_mod)
        {
            case 0:
                if (pfx67)
                {
                    if (i_rm == 6)
                        oflen = 2;
                }
                else
                {
                    if (i_rm == 5)
                        oflen = 4;
                }
                break;

            case 1:
                oflen = 1;
                break;

            case 2:
                if (pfx67)
                    oflen = 2;
                else
                    oflen = 4;
                break;
        }

        /* process SIB byte */
        if (pfx67 == 0 && i_rm == 4 && i_mod != 3)
        {
            if ( (*code & 7) == 5 && (i_mod != 1) )
            {
                oflen = 4;
            }

            oflen++;
        }

        osize += oflen;
    }

    /* process offset */
    if (flags & OP_X86_DATA_PRE66_67)
    {
        osize += 4 - (pfx66 << 1);
    }
    /* process immediate value */
    osize += (flags & 7);

    return osize;
}
