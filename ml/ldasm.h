#ifndef _LDASM_H_6c4dff54_0f1d_4946_b50c_f9b96c5ade1e_
#define _LDASM_H_6c4dff54_0f1d_4946_b50c_f9b96c5ade1e_


ULONG_PTR FASTCALL LdeGetOpCodeSize32(PVOID Code, PVOID DEF_VAL(*OpCodePtr, NULL));
ULONG_PTR FASTCALL LdeGetOpCodeSize64(PVOID Code, PVOID DEF_VAL(*OpCodePtr, NULL));
ULONG_PTR FASTCALL SizeOfProc(PVOID Proc);
BOOL FASTCALL IsRelativeCmd(PBYTE OpCode);

#endif // _LDASM_H_6c4dff54_0f1d_4946_b50c_f9b96c5ade1e_