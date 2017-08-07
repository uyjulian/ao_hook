#include "mylibrary.h"


#pragma push_macro("POINTER_SIZE")

#undef POINTER_SIZE
#define POINTER_SIZE sizeof(PVOID)

NTSTATUS
CopyOneOpCode(
    PVOID       Target,
    PVOID       Source,
    PULONG_PTR  TargetOpLength,
    PULONG_PTR  SourceOpLength,
    ULONG_PTR   ForwardSize,
    ULONG_PTR   BackwardSize,
    PVOID       TargetIp
)
{
    ULONG_PTR   Length, OpCode, OpCodeLength, OpOffsetLength, Selector;
    LONG_PTR    OpOffset, *pOpOffset;
    PBYTE       Func, p;
    ULONG_PTR   NextOpAddress;

    enum { OpCodeOffsetShort = 1, OpCodeOffsetLong = POINTER_SIZE };

    TargetIp = TargetIp == IMAGE_INVALID_VA ? Target : TargetIp;

    Func = (PBYTE)Source;
    p    = (PBYTE)Target;

    Length = GetOpCodeSize(Func);

    OpCode          = *Func;
    pOpOffset       = (PLONG_PTR)(Func + 1);
    OpOffsetLength  = OpCodeOffsetShort;
    OpCodeLength    = 2;
    Selector        = -1u;

    if (((OpCode & 0xF0) ^ 0x70) == 0)
//    if (OpCode >= 0x70 && OpCode <= 0x7F)
    {
        OpCode = 0x800F | ((OpCode - 0x70) << 8);
    }
    else if (OpCode == 0xEB)
    {
        OpCode = 0xE9;
        OpCodeLength = 1;
    }
    else if (OpCode == 0xE8 || OpCode == 0xE9)
    {
        OpOffsetLength  = OpCodeOffsetLong;
        OpCodeLength    = 1;
    }
    else
    {
        ULONG _Op;

        OpCode = *(PUSHORT)Func;
        _Op = SWAP2(OpCode);

        if ((_Op & ~0xF) == 0x0F80)
        // if (_Op >= 0x0F80 && _Op <= 0x0F8F)
        {
            OpOffsetLength = OpCodeOffsetLong;
            pOpOffset = (PLONG_PTR)(Func + 2);
        }
        else
        {
DEFAULT_OP_CODE:
            CopyMemory(p, Func, Length);
            p += Length;

            goto EXIT_PROC;
        }
    }

    OpOffset = 0;
    switch (OpOffsetLength)
    {
        case OpCodeOffsetShort:
            OpOffset = *(PCHAR)pOpOffset;
            break;

        case OpCodeOffsetLong:
        default:
            OpOffset = *(PLONG_PTR)pOpOffset;
            break;
    }

//    ULONG_PTR NewOffset = (ULONG_PTR)Func + Length + OpOffset;
//    if (NewOffset >= (ULONG_PTR)Destination && NewOffset < (ULONG_PTR)Destination + BytesToNop + HookOpSize)
//        goto DEFAULT_OP_CODE;

    LONG_PTR NewOffset = Length + OpOffset;

    if (NewOffset < 0)
    {
        if ((ULONG_PTR)-NewOffset < BackwardSize)
            goto DEFAULT_OP_CODE;
    }
    else if ((ULONG_PTR)NewOffset < ForwardSize)
    {
        goto DEFAULT_OP_CODE;
    }

    NextOpAddress = (ULONG_PTR)Func + Length;
    OpOffset = (NextOpAddress + OpOffset) - ((ULONG_PTR)TargetIp + OpCodeLength + OpCodeOffsetLong + (Selector == -1 ? 0 : 2));
    switch (OpCodeLength)
    {
        case 1:
            *p++ = (BYTE)OpCode;
            break;

        case 2:
        default:
            *(PUSHORT)p = (USHORT)OpCode;
            p += 2;
            break;
    }
/*
    if (Selector != -1)
    {
        *(PUSHORT)p = Selector;
        p += 2;
    }
*/
    *(PLONG_PTR)p = OpOffset;
    p += POINTER_SIZE;

//    Length = PtrOffset(p, Destination);

EXIT_PROC:
    if (SourceOpLength != NULL)
        *SourceOpLength = Length;

    if (TargetOpLength != NULL)
        *TargetOpLength = PtrOffset(p, Target);

    return STATUS_SUCCESS;
}


ML_NAMESPACE_BEGIN(Mp);

#if ML_X86

#define SIZE_OF_JUMP_CODE   5

#elif ML_AMD64


#endif // arch

VOID PatchNop(PVOID Address, ULONG_PTR BytesToPatch)
{
    PBYTE Buffer = (PBYTE)Address;

    switch (BytesToPatch)
    {
        case 0:
            return;

        case 1:
            //
            // nop
            //
            Buffer[0] = 0x90;
            break;

        case 2:
            //
            // mov eax, eax
            //
            *(PUSHORT)Buffer = 0xC08B;
            break;

        case 3:
            //
            // lea eax, [eax+0]
            //
            *(PUSHORT)Buffer = 0x408D;
            Buffer[2] = 0x00;
            break;

        case 4:
            //
            // lea esi, [esi]
            //
            *(PULONG)Buffer = 0x0026748D;
            break;

        case 5:
            // 2 + 3
            *Buffer = 0x8B;
            *(PULONG)(Buffer + 1) = 0x408DC0;
            break;

        case 6:
            // lea eax, [eax+0]
            *(PULONG)Buffer = 0x808D;
            *(PUSHORT)(Buffer + 4) = 0;
            break;

        case 7:
            // lea esi, [esi]
            *(PULONG)Buffer = 0x0026B48D;
            *(PULONG)&Buffer[3] = 0;
            break;
    }
}

class MemoryPatchManager
{
protected:
    GrowableArray<PTRAMPOLINE_DATA> TrampolineList;
    HANDLE ExecutableHeap;

public:
    MemoryPatchManager()
    {
        this->ExecutableHeap = RtlCreateHeap(HEAP_CREATE_ENABLE_EXECUTE | HEAP_GROWABLE | HEAP_CREATE_ALIGN_16, nullptr, 0, 0, nullptr, nullptr);
    }

protected:
    NoInline NTSTATUS HandleMemoryPatch(PPATCH_MEMORY_DATA PatchData, PVOID BaseAddress)
    {
        auto&       Patch = PatchData->Memory;
        PVOID       Address;
        ULONG       Protect;
        NTSTATUS    Status;

        if (Patch.Size == 0)
            return STATUS_SUCCESS;

        if (Patch.Address == IMAGE_INVALID_RVA)
            return STATUS_SUCCESS;

        Address = PtrAdd(Patch.Options.VirtualAddress ? nullptr : BaseAddress, Patch.Address);
        Status = ProtectMemory(Address, Patch.Size, PAGE_EXECUTE_READWRITE, &Protect);
        FAIL_RETURN(Status);

        if (Patch.Options.DataIsBuffer)
        {
            CopyMemory(Address, (PVOID)Patch.Data, Patch.Size);
        }
        else
        {
            if (Patch.Options.BackupData)
            {
                PTRAMPOLINE_DATA TrampolineData;

                TrampolineData = AllocateTrampolineData();
                if (TrampolineData == nullptr)
                    return STATUS_NO_MEMORY;

                TrampolineData->PatchData = *PatchData;
                TrampolineData->PatchData.Memory.Backup = 0;
                TrampolineData->PatchData.Memory.Options.VirtualAddress = TRUE;
                TrampolineData->PatchData.Memory.Address = (ULONG_PTR)Address;
                CopyMemory(&TrampolineData->PatchData.Memory.Backup, Address, Patch.Size);

                *PatchData = TrampolineData->PatchData;

                this->TrampolineList.Add(TrampolineData);
            }

            CopyMemory(&Patch.Backup, Address, Patch.Size);
            CopyMemory(Address, &Patch.Data, Patch.Size);
        }

        Status = ProtectMemory(Address, Patch.Size, Protect, nullptr);

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS HandleFunctionPatch(PPATCH_MEMORY_DATA PatchData, PVOID BaseAddress)
    {
        auto&               Function = PatchData->Function;
        BYTE                LocalHookBuffer[TRAMPOLINE_SIZE];
        PVOID               Address;
        PBYTE               Trampoline;
        ULONG               Protect;
        ULONG_PTR           HookOpSize, CopyOpSize;
        NTSTATUS            Status;
        PTRAMPOLINE_DATA    TrampolineData;

        if (Function.Target == nullptr)
            return STATUS_SUCCESS;

        if (Function.Source == IMAGE_INVALID_RVA)
            return STATUS_SUCCESS;

        Address = PtrAdd(Function.Options.VirtualAddress ? nullptr : BaseAddress, Function.Source);

        HookOpSize = GetSizeOfHookOpSize(Function.HookOp);
        if (HookOpSize == ULONG_PTR_MAX)
            return STATUS_BUFFER_TOO_SMALL;

        Status = GetHookAddressAndSize(Address, HookOpSize, &Address, &CopyOpSize);
        FAIL_RETURN(Status);

        if (CopyOpSize > TRAMPOLINE_SIZE)
            RaiseDebugBreak();

        if (Function.Options.NakedTrampoline == FALSE)
        {
            Status = GenerateHookCode(LocalHookBuffer, Address, Function.Target, Function.HookOp, HookOpSize);
            FAIL_RETURN(Status);

            PatchNop(&LocalHookBuffer[HookOpSize], CopyOpSize - HookOpSize);
        }

        TrampolineData = nullptr;
        if (Function.Options.NakedTrampoline != FALSE || (Function.Trampoline != nullptr && Function.Options.DoNotDisassemble == FALSE))
        {
            TrampolineData = AllocateTrampolineData();
            if (TrampolineData == nullptr)
                return STATUS_NO_MEMORY;

            TrampolineData->PatchData = *PatchData;
            TrampolineData->PatchData.Function.NopBytes = CopyOpSize - HookOpSize;
            TrampolineData->PatchData.Function.Source = (ULONG_PTR)Address;
            TrampolineData->PatchData.Function.Options.VirtualAddress = TRUE;

            TrampolineData->OriginSize = CopyOpSize;
            CopyMemory(TrampolineData->OriginalCode, Address, CopyOpSize);

            TrampolineData->JumpBackAddress = PtrAdd(Address, CopyOpSize);
            Trampoline = TrampolineData->Trampoline;

            if (Function.Options.NakedTrampoline == FALSE)
            {
                CopyTrampolineStub(Trampoline, Address, CopyOpSize);
                if (CopyOpSize == HookOpSize && Function.HookOp == OpCall && Function.Options.KeepRawTrampoline == FALSE)
                {
                    TrampolineData->Trampoline[0] = 0xE9;
                }
                else
                {
                    GenerateJumpBack(Trampoline, &TrampolineData->JumpBackAddress);
                }
            }
            else
            {
                GenerateNakedTrampoline(Trampoline, Address, CopyOpSize, TrampolineData);

                Status = GenerateHookCode(LocalHookBuffer, Address, TrampolineData->Trampoline, Function.HookOp, HookOpSize);
                if (NT_FAILED(Status))
                {
                    FreeTrampolineData(TrampolineData);
                    return Status;
                }

                PatchNop(&LocalHookBuffer[HookOpSize], CopyOpSize - HookOpSize);
            }

            if (Trampoline - TrampolineData->Trampoline > TRAMPOLINE_SIZE)
                RaiseDebugBreak();

            FlushInstructionCache(TrampolineData->Trampoline, TRAMPOLINE_SIZE);

            if (Function.Trampoline != nullptr)
                *Function.Trampoline = TrampolineData->Trampoline;

            this->TrampolineList.Add(TrampolineData);
        }

        Status = ProtectMemory(Address, CopyOpSize, PAGE_EXECUTE_READWRITE, &Protect);
        if (NT_FAILED(Status))
        {
            FreeTrampolineData(TrampolineData);
            return Status;
        }

        CopyMemory(Address, LocalHookBuffer, CopyOpSize);
        FlushInstructionCache(Address, CopyOpSize);

        Status = ProtectMemory(Address, CopyOpSize, Protect, &Protect);

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS GenerateHookCode(PBYTE Buffer, PVOID SourceIp, PVOID Target, ULONG_PTR HookOp, ULONG_PTR HookOpSize)
    {
        ULONG_PTR RegisterIndex;

#if ML_X86

        switch (HookOp)
        {
            case OpCall:
            case OpJump:
                //
                // jmp imm
                //
                *Buffer++ = HookOp == OpCall ? 0xE8 : 0xE9;
                *(PULONG)Buffer = PtrOffset(Target, PtrAdd(SourceIp, HookOpSize));
                break;

            case OpPush:
                //
                // push imm
                // ret
                //
                *Buffer++ = 0x68;
                *(PULONG)Buffer = (ULONG)Target;
                Buffer += POINTER_SIZE;
                *Buffer = 0xC3;
                break;

            default:
                if (HookOp < OpJRax || HookOp > OpJRdi)
                    return STATUS_INVALID_PARAMETER;

                //
                // mov r32, imm
                // jmp r32
                //
                RegisterIndex = HookOp - OpJRax;
                *Buffer++ = 0xB8 + (BYTE)RegisterIndex;
                *(PVOID *)Buffer = Target;
                Buffer += POINTER_SIZE;
                *(PUSHORT)Buffer = (USHORT)(0xE0FF + RegisterIndex);
                break;
        }

#elif ML_AMD64

        switch (HookOp)
        {
            case OpPush:
                //
                // push imm.low
                // mov dword ptr [rsp + 4], imm.high
                // ret
                //
                *Buffer++ = 0x68;
                *(PULONG)Buffer = (ULONG)((ULONG_PTR)Target >> 32);
                Buffer += POINTER_SIZE;

                *Buffer++ = 0xC7;
                *Buffer++ = 0x04;
                *Buffer++ = 0x24;
                *(PULONG)Buffer = (ULONG)(ULONG_PTR)Target;

                *Buffer = 0xC3;
                break;

            default:
                if (HookOp < OpJRax || HookOp > OpJRdi)
                    return STATUS_INVALID_PARAMETER;

                //
                // mov r32, imm
                // jmp r32
                //
                RegisterIndex = HookOp - OpJRax;
                *Buffer++ = 0x48;
                *Buffer++ = 0xB8 + (BYTE)RegisterIndex;
                *(PVOID *)Buffer = Target;
                Buffer += POINTER_SIZE;
                *(PUSHORT)Buffer = (USHORT)(0xE0FF + RegisterIndex);
                break;
        }

#else

        return STATUS_NOT_IMPLEMENTED;

#endif

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS GenerateNakedTrampoline(PBYTE& Trampoline, PVOID Address, ULONG_PTR CopyOpSize, PTRAMPOLINE_DATA TrampolineData)
    {
        NTSTATUS    Status;
        PVOID*      AddressOfReturnAddress;
        auto&       Function = TrampolineData->PatchData.Function;

        if (Function.Options.ExecuteTrampoline != FALSE)
        {
            Status = CopyTrampolineStub(Trampoline, Address, CopyOpSize);
            FAIL_RETURN(Status);
        }

#if ML_X86

        //
        // push     eax
        // push     esp
        // push     eax
        // push     ecx
        // push     edx
        // push     ebx
        // push     ebp
        // push     esi
        // push     edi
        // pushfd
        //
        // mov      eax, ReturnAddress
        // mov      [esp + 24h], eax
        // mov      eax, CallBack
        // lea      ecx, [esp];      ecx = PTRAMPOLINE_NAKED_CONTEXT
        // call     eax
        //
        // popfd
        // pop      edi
        // pop      esi
        // pop      ebp
        // pop      ebx
        // pop      edx
        // pop      ecx
        // pop      eax
        // pop      esp
        //
        // ret
        //

        if (Function.HookOp != OpCall)
        {
            // push eax
            *Trampoline++ = 0x50;
        }

        *Trampoline++ = 0x54;       // push    esp
        *Trampoline++ = 0x50;       // push    eax
        *Trampoline++ = 0x51;       // push    ecx
        *Trampoline++ = 0x52;       // push    edx
        *Trampoline++ = 0x53;       // push    ebx
        *Trampoline++ = 0x55;       // push    ebp
        *Trampoline++ = 0x56;       // push    esi
        *Trampoline++ = 0x57;       // push    edi
        *Trampoline++ = 0x9C;       // pushfd

        // mov eax, imm
        *Trampoline++ = 0xB8;
        *(PVOID *)Trampoline = PtrAdd(Address, CopyOpSize);
        AddressOfReturnAddress = (PVOID *)Trampoline;
        Trampoline += POINTER_SIZE;

        // mov [esp + 24h], eax
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x44;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x24;

        // mov eax, CallBack
        *Trampoline++ = 0xB8;
        *(PVOID *)Trampoline = Function.Target;
        Trampoline += POINTER_SIZE;

        // lea ecx, [esp]
        *Trampoline++ = 0x8D;
        *Trampoline++ = 0x0C;
        *Trampoline++ = 0x24;

        // call eax
        *Trampoline++ = 0xFF;
        *Trampoline++ = 0xD0;

        *Trampoline++ = 0x9D;       // popfd
        *Trampoline++ = 0x5F;       // pop     edi
        *Trampoline++ = 0x5E;       // pop     esi
        *Trampoline++ = 0x5D;       // pop     ebp
        *Trampoline++ = 0x5B;       // pop     ebx
        *Trampoline++ = 0x5A;       // pop     edx
        *Trampoline++ = 0x59;       // pop     ecx
        *Trampoline++ = 0x58;       // pop     eax
        *Trampoline++ = 0x5C;       // pop     esp

        // ret
        *Trampoline++ = 0xC3;

#else

        //
        // SIZE_OF_CONTEXT equ 78h
        // push    rax
        // pushfq
        // lea     rsp, [rsp - SIZE_OF_CONTEXT]
        // mov     [rsp + 00h], rax
        // mov     [rsp + 08h], rcx
        // mov     [rsp + 10h], rdx
        // mov     [rsp + 18h], rbx
        // mov     [rsp + 20h], rbp
        // mov     [rsp + 28h], rsi
        // mov     [rsp + 30h], rdi
        // mov     [rsp + 38h], r8
        // mov     [rsp + 40h], r9
        // mov     [rsp + 48h], r10
        // mov     [rsp + 50h], r11
        // mov     [rsp + 58h], r12
        // mov     [rsp + 60h], r13
        // mov     [rsp + 68h], r14
        // mov     [rsp + 70h], r15
        // mov     rax, ReturnAddress
        // mov     [rsp + SIZE_OF_CONTEXT + 10h], rax
        // mov     rax, CallBack
        // lea     rcx, [rsp];      rcx = PTRAMPOLINE_NAKED_CONTEXT
        // lea     rsp, [rsp - 20h]
        // call    rax
        // lea     rsp, [rsp + 20h]
        // mov     rax, [rsp + 00h]
        // mov     rcx, [rsp + 08h]
        // mov     rdx, [rsp + 10h]
        // mov     rbx, [rsp + 18h]
        // mov     rbp, [rsp + 20h]
        // mov     rsi, [rsp + 28h]
        // mov     rdi, [rsp + 30h]
        // mov     r8 , [rsp + 38h]
        // mov     r9 , [rsp + 40h]
        // mov     r10, [rsp + 48h]
        // mov     r11, [rsp + 50h]
        // mov     r12, [rsp + 58h]
        // mov     r13, [rsp + 60h]
        // mov     r14, [rsp + 68h]
        // mov     r15, [rsp + 70h]
        // lea     rsp, [rsp + SIZE_OF_CONTEXT]
        // popfq
        // ret
        //

        if (Function.HookOp != OpCall)
        {
            // push rax
            *Trampoline++ = 0x50;
        }

        // pushfq
        *Trampoline++ = 0x9C;

        // lea     rsp, [rsp-78h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8D;
        *Trampoline++ = 0x64;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x88;

        // mov     [rsp + 00h], rax
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x04;
        *Trampoline++ = 0x24;

        // mov     [rsp + 08h], rcx
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x08;

        // mov     [rsp + 10h], rdx
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x54;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x10;

        // mov     [rsp + 18h], rbx
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x5C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x18;

        // mov     [rsp + 20h], rbp
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x6C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x20;

        // mov     [rsp + 28h], rsi
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x74;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x28;

        // mov     [rsp + 30h], rdi
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x7C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x30;

        // mov     [rsp + 38h], r8
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x44;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x38;

        // mov     [rsp + 40h], r9
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x40;

        // mov     [rsp + 48h], r10
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x54;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x48;

        // mov     [rsp + 50h], r11
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x5C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x50;

        // mov     [rsp + 58h], r12
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x64;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x58;

        // mov     [rsp + 60h], r13
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x6C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x60;

        // mov     [rsp + 68h], r14
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x74;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x68;

        // mov     [rsp + 70h], r15
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x7C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x70;

        // mov     rax, ReturnAddress
        *Trampoline++ = 0x48;
        *Trampoline++ = 0xB8;
        *(PVOID *)Trampoline = PtrAdd(Address, CopyOpSize);
        AddressOfReturnAddress = (PVOID *)Trampoline;
        Trampoline += POINTER_SIZE;

        // mov     [rsp + SIZE_OF_CONTEXT + 10], rax
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x89;
        *Trampoline++ = 0x84;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x80;
        *Trampoline++ = 0x00;
        *Trampoline++ = 0x00;
        *Trampoline++ = 0x00;

        // mov     rax, CallBack
        *Trampoline++ = 0x48;
        *Trampoline++ = 0xB8;
        *(PVOID *)Trampoline = Function.Target;
        Trampoline += POINTER_SIZE;

        // lea     rcx, [rsp];      rcx = PTRAMPOLINE_NAKED_CONTEXT
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8D;
        *Trampoline++ = 0x0C;
        *Trampoline++ = 0x24;

        // lea     rsp, [rsp - 20h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8D;
        *Trampoline++ = 0x64;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0xE0;

        // call    rax
        *Trampoline++ = 0xFF;
        *Trampoline++ = 0xD0;

        // lea     rsp, [rsp + 20h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8D;
        *Trampoline++ = 0x64;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x20;

        // mov     rax, [rsp + 00h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x04;
        *Trampoline++ = 0x24;

        // mov     rcx, [rsp + 08h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x08;

        // mov     rdx, [rsp + 10h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x54;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x10;

        // mov     rbx, [rsp + 18h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x5C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x18;

        // mov     rbp, [rsp + 20h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x6C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x20;

        // mov     rsi, [rsp + 28h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x74;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x28;

        // mov     rdi, [rsp + 30h]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x7C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x30;

        // mov     r8 , [rsp + 38h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x44;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x38;

        // mov     r9 , [rsp + 40h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x40;

        // mov     r10, [rsp + 48h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x54;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x48;

        // mov     r11, [rsp + 50h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x5C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x50;

        // mov     r12, [rsp + 58h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x64;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x58;

        // mov     r13, [rsp + 60h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x6C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x60;

        // mov     r14, [rsp + 68h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x74;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x68;

        // mov     r15, [rsp + 70h]
        *Trampoline++ = 0x4C;
        *Trampoline++ = 0x8B;
        *Trampoline++ = 0x7C;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x70;

        // lea     rsp, [rsp + SIZE_OF_CONTEXT]
        *Trampoline++ = 0x48;
        *Trampoline++ = 0x8D;
        *Trampoline++ = 0x64;
        *Trampoline++ = 0x24;
        *Trampoline++ = 0x78;

        // popfq
        *Trampoline++ = 0x9D;

        // ret
        *Trampoline++ = 0xC3;

//#error NOT COMPLETE

#endif // arch

        if (Function.Options.ExecuteTrampoline == FALSE)
        {
            *AddressOfReturnAddress = Trampoline;

            Status = CopyTrampolineStub(Trampoline, Address, CopyOpSize);
            FAIL_RETURN(Status);

            Status = GenerateJumpBack(Trampoline, &TrampolineData->JumpBackAddress);
            FAIL_RETURN(Status);
        }

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS GenerateJumpBack(PBYTE& Trampoline, PVOID AddressOfJumpAddress)
    {
        // jmp [imm32]
        *Trampoline++ = 0xFF;
        *Trampoline++ = 0x25;

        *(PVOID *)Trampoline = AddressOfJumpAddress;
        Trampoline += 2 + 4;

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS CopyTrampolineStub(PBYTE& Trampoline, PVOID Address, ULONG_PTR CopyOpSize)
    {
        ULONG_PTR SourceOpLength, ForwardSize, BackwardSize;
        PBYTE Source = (PBYTE)Address;

        ForwardSize  = CopyOpSize;
        BackwardSize = 0;

        for (LONG_PTR Bytes = CopyOpSize; Bytes > 0; Bytes -= SourceOpLength)
        {
            ULONG_PTR TargetOpLength;

            if (Source[0] == 0xC2 || Source[0] == 0xC3)
                return STATUS_BUFFER_TOO_SMALL;

            CopyOneOpCode(Trampoline, Source, &TargetOpLength, &SourceOpLength, ForwardSize, BackwardSize);

            BackwardSize+= SourceOpLength;
            ForwardSize -= SourceOpLength;
            Trampoline  += TargetOpLength;
            Source      += SourceOpLength;
        }

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS GetHookAddressAndSize(PVOID Address, ULONG_PTR HookOpSize, PVOID *FinalAddress, PULONG_PTR Size)
    {
        ULONG_PTR   Length, OpSize;
        PBYTE       Buffer;

        OpSize = 0;
        Buffer = (PBYTE)Address;
        *FinalAddress = Address;

#if ML_X86 || ML_AMD64

        while (OpSize < HookOpSize)
        {
            if (Buffer[0] == 0xC2 || Buffer[0] == 0xC3)
                return STATUS_BUFFER_TOO_SMALL;

            Length  = GetOpCodeSize(Buffer);

            // jmp short const
            if (Buffer[0] == 0xEB)
            {
                if (OpSize == 0)
                {
                    if (Buffer[1] != 0)
                    {
                        Buffer += *(PCHAR)&Buffer[1] + Length;
                        *FinalAddress = Buffer;
                        continue;
                    }
                }
                else if (OpSize < HookOpSize - Length)
                {
                    return STATUS_BUFFER_TOO_SMALL;
                }
            }
            else if (Buffer[0] == 0xFF && Buffer[1] == 0x25)
            {
                // jmp [rimm]

#if ML_X86
                if (OpSize != 0 && OpSize + Length < HookOpSize)
                    return STATUS_BUFFER_TOO_SMALL;

                if (OpSize == 0 && HookOpSize > Length)
                {
                    Buffer = **(PBYTE **)&Buffer[2];
                    *FinalAddress = Buffer;
                    continue;
                }
#elif ML_AMD64
                if (OpSize != 0)
                    return STATUS_BUFFER_TOO_SMALL;

                Buffer = *(PBYTE *)(Buffer + Length + *(PLONG)&Buffer[2]);
                *FinalAddress = Buffer;
                continue;
#endif // arch

            }

            Buffer += Length;
            OpSize += Length;
        }

#else

#error "not support"

#endif // arch

        *Size = OpSize;

        return STATUS_SUCCESS;
    }

    NoInline ULONG_PTR GetSizeOfHookOpSize(ULONG_PTR HookOp)
    {

#if ML_AMD64

        switch (HookOp)
        {
            case OpPush:
                // 68 00 00 00 80       push    80000000h
                // C7 04 24 00 00 00 80 mov     dword ptr [rsp], 80000000h
                // C3                   ret
                return 0xD;

            //case OpJumpIndirect:
            //    //
            //    // FF 25 00 00 00 00 jmp [rimm]
            //    // 00 00 00 00 00 00 00 00
            //    //
            //    break;
            //    //return 0xE;

            default:
                if (HookOp >= OpJRax && HookOp <= OpJRdi)
                {
                    //
                    // 48 B8 00 00 00 80 00 00 00 80    mov     rax, 8000000080000000h
                    // FF E0                            jmp     rax
                    //
                    return 0xC;
                }
        }

#elif ML_X86

        switch (HookOp)
        {
            case OpPush:
                //
                // 68 00000000  push const
                // C3           ret
                //
                return 1 + POINTER_SIZE + 1;

            case OpCall:
            case OpJump:
                //
                // E8 00000000  call    const
                // E9 00000000  jmp     const
                //
                return 1 + POINTER_SIZE;

            default:
                if (HookOp >= OpJRax && HookOp <= OpJRdi)
                {
                    //
                    // b8 88888888      mov     r32, const
                    // ffe0             jmp     r32
                    //
                    return 1 + POINTER_SIZE + 2;
                }
        }

#endif

        return ULONG_PTR_MAX;
    }

    ForceInline
    NTSTATUS
    ProtectMemory(
        PVOID       BaseAddress,
        ULONG_PTR   Size,
        ULONG       NewProtect,
        PULONG      OldProtect
    )
    {
        return Mm::ProtectMemory(CurrentProcess, BaseAddress, Size, NewProtect, OldProtect);
    }

    ForceInline
    NTSTATUS
    FlushInstructionCache(
        PVOID       BaseAddress,
        ULONG_PTR   NumberOfBytesToFlush
    )
    {
        return NtFlushInstructionCache(CurrentProcess, BaseAddress, NumberOfBytesToFlush);
    }

    PTRAMPOLINE_DATA AllocateTrampolineData()
    {
        return (PTRAMPOLINE_DATA)RtlAllocateHeap(this->ExecutableHeap, HEAP_ZERO_MEMORY, sizeof(TRAMPOLINE_DATA));
    }

    VOID FreeTrampolineData(PTRAMPOLINE_DATA TrampolineData)
    {
        if (TrampolineData != nullptr)
            RtlFreeHeap(this->ExecutableHeap, 0, TrampolineData);
    }

    NTSTATUS RemoveTrampolineData(PTRAMPOLINE_DATA TrampolineData)
    {
        NTSTATUS Status;
        ULONG_PTR Index;

        Index = this->TrampolineList.IndexOf(TrampolineData);
        if (Index == this->TrampolineList.kInvalidIndex)
            return STATUS_NOT_FOUND;

        this->TrampolineList.Remove(Index);
        FreeTrampolineData(TrampolineData);

        return STATUS_SUCCESS;
    }

    DECL_NORETURN VOID RaiseDebugBreak()
    {
        __debugbreak();
    }

public:
    NoInline NTSTATUS PatchMemory(PPATCH_MEMORY_DATA PatchData, ULONG_PTR PatchCount, PVOID BaseAddress)
    {
        NTSTATUS Status;

        FOR_EACH(PatchData, PatchData, PatchCount)
        {
            switch (PatchData->PatchType)
            {
                case PatchMemoryTypes::MemoryPatch:
                    Status = HandleMemoryPatch(PatchData, BaseAddress);
                    break;

                case PatchMemoryTypes::FunctionPatch:
                    Status = HandleFunctionPatch(PatchData, BaseAddress);
                    break;

                default:
                    Status = STATUS_NOT_IMPLEMENTED;
                    break;
            }

            FAIL_RETURN(Status);
        }

        return STATUS_SUCCESS;
    }

    NTSTATUS RestoreMemory(PTRAMPOLINE_DATA TrampolineData)
    {
        NTSTATUS Status;
        PATCH_MEMORY_DATA PatchData = MemoryPatchVa(&TrampolineData->OriginalCode, TrampolineData->OriginSize, TrampolineData->PatchData.Function.Source);
        Status = HandleMemoryPatch(&PatchData, nullptr);
        NT_SUCCESS(Status) && RemoveTrampolineData(TrampolineData);

        return Status;
    }

public:
    static NoInline NTSTATUS CreateInstance(MemoryPatchManager **Manager)
    {
        ml::MlInitialize();

        *Manager = (MemoryPatchManager *)AllocateMemory(sizeof(**Manager));
        if (*Manager == nullptr)
            return STATUS_NO_MEMORY;

        ZeroMemory(*Manager, sizeof(**Manager));
        new (*Manager) MemoryPatchManager;

        return STATUS_SUCCESS;
    }

    static NoInline NTSTATUS DestroyInstance(MemoryPatchManager *Manager)
    {
        if (Manager != nullptr)
        {
            Manager->~MemoryPatchManager();
            FreeMemory(Manager);
        }

        return STATUS_SUCCESS;
    }
};

ForceInline MemoryPatchManager*& MemoryPatchManagerInstance()
{
    static MemoryPatchManager *Manager;
    return Manager;
}

NTSTATUS MP_CALL PatchMemory(PPATCH_MEMORY_DATA PatchData, ULONG_PTR PatchCount, PVOID BaseAddress)
{
    NTSTATUS Status;

    if (MemoryPatchManagerInstance() == nullptr)
    {
        Status = MemoryPatchManager::CreateInstance(&MemoryPatchManagerInstance());
        FAIL_RETURN(Status);
    }

    return MemoryPatchManagerInstance()->PatchMemory(PatchData, PatchCount, BaseAddress);
}

NTSTATUS MP_CALL RestoreMemory(PTRAMPOLINE_DATA TrampolineData)
{
    if (MemoryPatchManagerInstance() == nullptr)
        return STATUS_FLT_NOT_INITIALIZED;

    return MemoryPatchManagerInstance()->RestoreMemory(TrampolineData);
}

ML_NAMESPACE_END_(Mp);

#pragma pop_macro("POINTER_SIZE")


