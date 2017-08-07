#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Rtl);

NTSTATUS
DosPathNameToNtPathName(
    PCWSTR                  DosName,
    PUNICODE_STRING         NtName,
    PCWSTR*                 DosFilePath,
    PRTL_RELATIVE_NAME_U    FileName
)
{
#if ML_USER_MODE

    BOOL Success;

    Success = RtlDosPathNameToNtPathName_U(DosName, NtName, DosFilePath, FileName);

    return Success ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;

#elif ML_KERNEL_MODE

    return STATUS_NOT_IMPLEMENTED;

#endif // r
}

NTSTATUS GetSystemDirectory(PUNICODE_STRING Buffer, BOOL Wow64NoRedirect)
{
    BOOL                IsWow64;
    WCHAR               LocalBuffer[0x100];
    NTSTATUS            Status;
    HANDLE              KnownDllDirectoryHandle, KnownDllPathSymbolicLink;
    OBJECT_ATTRIBUTES   ObjectAttributes;
    UNICODE_STRING      LocalString;
    PUNICODE_STRING     KnownDlls;

    static UNICODE_STRING KnownDllsName     = RTL_CONSTANT_STRING(L"\\KnownDlls");
    static UNICODE_STRING KnownDlls32Name   = RTL_CONSTANT_STRING(L"\\KnownDlls32");
    static UNICODE_STRING KnownDllPathName  = RTL_CONSTANT_STRING(L"KnownDllPath");

    KnownDlls = &KnownDllsName;

    if (Wow64NoRedirect == FALSE && Ps::IsWow64Process() != FALSE)
        KnownDlls = &KnownDlls32Name;

#if ML_USER_MODE

    InitializeObjectAttributes(&ObjectAttributes, KnownDlls, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
    Status = NtOpenDirectoryObject(&KnownDllDirectoryHandle, DIRECTORY_QUERY | DIRECTORY_TRAVERSE, &ObjectAttributes);
    FAIL_RETURN(Status);

    InitializeObjectAttributes(&ObjectAttributes, &KnownDllPathName, OBJ_CASE_INSENSITIVE, KnownDllDirectoryHandle, nullptr);
    Status = NtOpenSymbolicLinkObject(&KnownDllPathSymbolicLink, SYMBOLIC_LINK_QUERY, &ObjectAttributes);
    NtClose(KnownDllDirectoryHandle);
    FAIL_RETURN(Status);

    RtlInitEmptyString(&LocalString, LocalBuffer, countof(LocalBuffer));

    Status = NtQuerySymbolicLinkObject(KnownDllPathSymbolicLink, &LocalString, nullptr);
    NtClose(KnownDllPathSymbolicLink);
    FAIL_RETURN(Status);

    RtlAppendUnicodeToString(&LocalString, L"\\");

    return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &LocalString, Buffer);

#else // r0

    InitializeObjectAttributes(&ObjectAttributes, KnownDlls, OBJ_CASE_INSENSITIVE, nullptr, nullptr);
    Status = ZwOpenDirectoryObject(&KnownDllDirectoryHandle, DIRECTORY_QUERY | DIRECTORY_TRAVERSE, &ObjectAttributes);
    FAIL_RETURN(Status);

    InitializeObjectAttributes(&ObjectAttributes, &KnownDllPathName, OBJ_CASE_INSENSITIVE, KnownDllDirectoryHandle, nullptr);
    Status = ZwOpenSymbolicLinkObject(&KnownDllPathSymbolicLink, SYMBOLIC_LINK_QUERY, &ObjectAttributes);
    NtClose(KnownDllDirectoryHandle);
    FAIL_RETURN(Status);

    RtlInitEmptyString(&LocalString, LocalBuffer, countof(LocalBuffer));

    Status = ZwQuerySymbolicLinkObject(KnownDllPathSymbolicLink, &LocalString, nullptr);
    ZwClose(KnownDllPathSymbolicLink);
    FAIL_RETURN(Status);

    RtlAppendUnicodeToString(&LocalString, L"\\");

    return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &LocalString, Buffer);

#endif
}

#if ML_USER_MODE

NTSTATUS GetModuleDirectory(ml::String &Path, PVOID ModuleBase)
{
    PWSTR           FileName;
    PLDR_MODULE     Self;
    UNICODE_STRING  SelfPath;

    Self = FindLdrModuleByHandle(ModuleBase);
    SelfPath = Self->FullDllName;
    FileName = findnamew(SelfPath.Buffer, SelfPath.Length / sizeof(SelfPath.Buffer[0]));
    SelfPath.Length = (USHORT)PtrOffset(FileName, SelfPath.Buffer);

    Path = SelfPath;

    return STATUS_SUCCESS;
}

NTSTATUS GetWorkingDirectory(PUNICODE_STRING Buffer)
{
    return RtlDuplicateUnicodeString(
                RTL_DUPSTR_ADD_NULL,
                &Ps::CurrentPeb()->ProcessParameters->CurrentDirectory.DosPath,
                Buffer
            );
}

NTSTATUS SetWorkingDirectory(PCWSTR PathName)
{
    UNICODE_STRING Path;

    RtlInitUnicodeString(&Path, PathName);
    return SetWorkingDirectory(&Path);
}

NTSTATUS SetWorkingDirectory(PUNICODE_STRING PathName)
{
    return RtlSetCurrentDirectory_U(PathName);
}

NTSTATUS SetExeDirectoryAsCurrent()
{
    UNICODE_STRING  ExePath;
    PLDR_MODULE     LdrModule;
    PWSTR           Buffer; // for stupid XP

    LdrModule       = Ldr::FindLdrModuleByHandle(NULL);
    ExePath         = LdrModule->FullDllName;
    ExePath.Length -= LdrModule->BaseDllName.Length;

    Buffer = (PWSTR)AllocStack(ExePath.Length + sizeof(ExePath.Buffer[0]));
    CopyMemory(Buffer, ExePath.Buffer, ExePath.Length);
    *PtrAdd(Buffer, ExePath.Length) = 0;
    ExePath.Buffer = Buffer;

    return RtlSetCurrentDirectory_U(&ExePath);
}

NTSTATUS NtPathNameToDosPathName(PUNICODE_STRING DosPath, PUNICODE_STRING NtPath)
{
    NTSTATUS                    Status;
    ULONG_PTR                   BufferSize;
    PWSTR                       Buffer;
    RTL_UNICODE_STRING_BUFFER   UnicodeBuffer;

    BufferSize = NtPath->MaximumLength + MAX_NTPATH * sizeof(WCHAR);

    Buffer = (PWSTR)AllocStack(BufferSize);

    ZeroMemory(&UnicodeBuffer, sizeof(UnicodeBuffer));

    UnicodeBuffer.String                = *NtPath;
    UnicodeBuffer.String.Buffer         = Buffer;
    UnicodeBuffer.String.MaximumLength  = (USHORT)BufferSize;
    UnicodeBuffer.ByteBuffer.Buffer     = (PUCHAR)Buffer;
    UnicodeBuffer.ByteBuffer.Size       = BufferSize;

    CopyMemory(Buffer, NtPath->Buffer, NtPath->Length);

    Status = RtlNtPathNameToDosPathName(0, &UnicodeBuffer, NULL, NULL);
    FAIL_RETURN(Status);

    return RtlDuplicateUnicodeString(RTL_DUPSTR_ADD_NULL, &UnicodeBuffer.String, DosPath);
}

#endif // r3


BOOL
DoesNameContainWildCards(
    IN PUNICODE_STRING Name
)

/*++

Routine Description:

    This routine simply scans the input Name string looking for any Nt
    wild card characters.

Arguments:

    Name - The string to check.

Return Value:

    BOOLEAN - TRUE if one or more wild card characters was found.

--*/
{
    PWSTR p;

    //
    //  Check each character in the name to see if it's a wildcard
    //  character.
    //

#if ML_USER_MODE

    static const UCHAR LEGAL_ANSI_CHARACTER_ARRAY[128] =
    {
        0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
        0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
        0x17, 0x07, 0x18, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x18, 0x16, 0x16, 0x17, 0x07, 0x00,
        0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x04, 0x16, 0x18, 0x16, 0x18, 0x18,
        0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
        0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x16, 0x00, 0x16, 0x17, 0x17,
        0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
        0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x10, 0x17, 0x17, 0x17
    };

#endif // r3

    if( Name->Length ) {
        for( p = Name->Buffer + (Name->Length / sizeof(WCHAR)) - 1;
             p >= Name->Buffer && *p != L'\\' ;
             p-- ) {

            //
            //  check for a wild card character
            //

            if (FsRtlIsUnicodeCharacterWild( *p )) {

                //
                //  Tell caller that this name contains wild cards
                //

                return TRUE;
            }
        }
    }

    //
    //  No wildcard characters were found, so return to our caller
    //

    return FALSE;
}

#define MATCHES_ARRAY_SIZE 16

//
//  Local support routine prototypes
//

BOOL
IsNameInExpressionPrivate (
    IN PUNICODE_STRING Expression,
    IN PUNICODE_STRING Name,
    IN BOOL IgnoreCase,
    IN PWCH UpcaseTable
)

/*++

Routine Description:

    This routine compares a Dbcs name and an expression and tells the caller
    if the name is in the language defined by the expression.  The input name
    cannot contain wildcards, while the expression may contain wildcards.

    Expression wild cards are evaluated as shown in the nondeterministic
    finite automatons below.  Note that ~* and ~? are DOS_STAR and DOS_QM.


             ~* is DOS_STAR, ~? is DOS_QM, and ~. is DOS_DOT


                                       S
                                    <-----<
                                 X  |     |  e       Y
             X * Y ==       (0)----->-(1)->-----(2)-----(3)


                                      S-.
                                    <-----<
                                 X  |     |  e       Y
             X ~* Y ==      (0)----->-(1)->-----(2)-----(3)



                                X     S     S     Y
             X ?? Y ==      (0)---(1)---(2)---(3)---(4)



                                X     .        .      Y
             X ~.~. Y ==    (0)---(1)----(2)------(3)---(4)
                                   |      |________|
                                   |           ^   |
                                   |_______________|
                                      ^EOF or .^


                                X     S-.     S-.     Y
             X ~?~? Y ==    (0)---(1)-----(2)-----(3)---(4)
                                   |      |________|
                                   |           ^   |
                                   |_______________|
                                      ^EOF or .^



         where S is any single character

               S-. is any single character except the final .

               e is a null character transition

               EOF is the end of the name string

    In words:

        * matches 0 or more characters.

        ? matches exactly 1 character.

        DOS_STAR matches 0 or more characters until encountering and matching
            the final . in the name.

        DOS_QM matches any single character, or upon encountering a period or
            end of name string, advances the expression to the end of the
            set of contiguous DOS_QMs.

        DOS_DOT matches either a . or zero characters beyond name string.

Arguments:

    Expression - Supplies the input expression to check against
        (Caller must already upcase if passing CaseInsensitive TRUE.)

    Name - Supplies the input name to check for.

    CaseInsensitive - TRUE if Name should be Upcased before comparing.

Return Value:

    BOOLEAN - TRUE if Name is an element in the set of strings denoted
        by the input Expression and FALSE otherwise.

--*/

{
    USHORT NameOffset;
    USHORT ExprOffset;

    ULONG SrcCount;
    ULONG DestCount;
    ULONG PreviousDestCount;
    ULONG MatchesCount;

    WCHAR NameChar = 0, ExprChar;

    USHORT LocalBuffer[MATCHES_ARRAY_SIZE * 2];

    USHORT *AuxBuffer = NULL;
    USHORT *PreviousMatches;
    USHORT *CurrentMatches;

    USHORT MaxState;
    USHORT CurrentState;

    BOOL NameFinished = FALSE;

    //
    //  The idea behind the algorithm is pretty simple.  We keep track of
    //  all possible locations in the regular expression that are matching
    //  the name.  If when the name has been exhausted one of the locations
    //  in the expression is also just exhausted, the name is in the language
    //  defined by the regular expression.
    //

    //
    //  If one string is empty return FALSE.  If both are empty return TRUE.
    //

    if ( (Name->Length == 0) || (Expression->Length == 0) ) {

        return (BOOL)(!(Name->Length + Expression->Length));
    }

    //
    //  Special case by far the most common wild card search of *
    //

    if ((Expression->Length == 2) && (Expression->Buffer[0] == L'*')) {

        return TRUE;
    }

    //
    //  Also special case expressions of the form *X.  With this and the prior
    //  case we have covered virtually all normal queries.
    //

    if (Expression->Buffer[0] == L'*') {

        UNICODE_STRING LocalExpression;

        LocalExpression = *Expression;

        LocalExpression.Buffer += 1;
        LocalExpression.Length -= 2;

        //
        //  Only special case an expression with a single *
        //

        if ( !DoesNameContainWildCards( &LocalExpression ) ) {

            ULONG StartingNameOffset;

            if (Name->Length < (USHORT)(Expression->Length - sizeof(WCHAR))) {

                return FALSE;
            }

            StartingNameOffset = ( Name->Length -
                                   LocalExpression.Length ) / sizeof(WCHAR);

            //
            //  Do a simple memory compare if case sensitive, otherwise
            //  we have got to check this one character at a time.
            //

            if ( !IgnoreCase ) {

                return (BOOL) RtlEqualMemory( LocalExpression.Buffer,
                                                 Name->Buffer + StartingNameOffset,
                                                 LocalExpression.Length );

            } else {

                for ( ExprOffset = 0;
                      ExprOffset < (USHORT)(LocalExpression.Length / sizeof(WCHAR));
                      ExprOffset += 1 ) {

                    NameChar = Name->Buffer[StartingNameOffset + ExprOffset];
                    NameChar = UpcaseTable[NameChar];

                    ExprChar = LocalExpression.Buffer[ExprOffset];

                    if ( NameChar != ExprChar ) {

                        return FALSE;
                    }
                }

                return TRUE;
            }
        }
    }

    //
    //  Walk through the name string, picking off characters.  We go one
    //  character beyond the end because some wild cards are able to match
    //  zero characters beyond the end of the string.
    //
    //  With each new name character we determine a new set of states that
    //  match the name so far.  We use two arrays that we swap back and forth
    //  for this purpose.  One array lists the possible expression states for
    //  all name characters up to but not including the current one, and other
    //  array is used to build up the list of states considering the current
    //  name character as well.  The arrays are then switched and the process
    //  repeated.
    //
    //  There is not a one-to-one correspondence between state number and
    //  offset into the expression.  This is evident from the NFAs in the
    //  initial comment to this function.  State numbering is not continuous.
    //  This allows a simple conversion between state number and expression
    //  offset.  Each character in the expression can represent one or two
    //  states.  * and DOS_STAR generate two states: ExprOffset*2 and
    //  ExprOffset*2 + 1.  All other expression characters can produce only
    //  a single state.  Thus ExprOffset = State/2.
    //
    //
    //  Here is a short description of the variables involved:
    //
    //  NameOffset  - The offset of the current name char being processed.
    //
    //  ExprOffset  - The offset of the current expression char being processed.
    //
    //  SrcCount    - Prior match being investigated with current name char
    //
    //  DestCount   - Next location to put a matching assuming current name char
    //
    //  NameFinished - Allows one more iteration through the Matches array
    //                 after the name is exhausted (to come *s for example)
    //
    //  PreviousDestCount - This is used to prevent entry duplication, see coment
    //
    //  PreviousMatches   - Holds the previous set of matches (the Src array)
    //
    //  CurrentMatches    - Holds the current set of matches (the Dest array)
    //
    //  AuxBuffer, LocalBuffer - the storage for the Matches arrays
    //

    //
    //  Set up the initial variables
    //

    PreviousMatches = &LocalBuffer[0];
    CurrentMatches = &LocalBuffer[MATCHES_ARRAY_SIZE];

    PreviousMatches[0] = 0;
    MatchesCount = 1;

    NameOffset = 0;

    MaxState = (USHORT)(Expression->Length * 2);

    while ( !NameFinished ) {

        if ( NameOffset < Name->Length ) {

            NameChar = Name->Buffer[NameOffset / sizeof(WCHAR)];

            NameOffset += sizeof(WCHAR);;

        } else {

            NameFinished = TRUE;

            //
            //  if we have already exhausted the expression, cool.  Don't
            //  continue.
            //

            if ( PreviousMatches[MatchesCount-1] == MaxState ) {

                break;
            }
        }


        //
        //  Now, for each of the previous stored expression matches, see what
        //  we can do with this name character.
        //

        SrcCount = 0;
        DestCount = 0;
        PreviousDestCount = 0;

        while ( SrcCount < MatchesCount ) {

            USHORT Length;

            //
            //  We have to carry on our expression analysis as far as possible
            //  for each character of name, so we loop here until the
            //  expression stops matching.  A clue here is that expression
            //  cases that can match zero or more characters end with a
            //  continue, while those that can accept only a single character
            //  end with a break.
            //

            ExprOffset = (USHORT)((PreviousMatches[SrcCount++] + 1) / 2);


            Length = 0;

            LOOP_FOREVER
            {

                if ( ExprOffset == Expression->Length ) {

                    break;
                }

                //
                //  The first time through the loop we don't want
                //  to increment ExprOffset.
                //

                ExprOffset = (USHORT)(ExprOffset + Length);
                Length = sizeof(WCHAR);

                CurrentState = (USHORT)(ExprOffset * 2);

                if ( ExprOffset == Expression->Length ) {

                    CurrentMatches[DestCount++] = MaxState;
                    break;
                }

                ExprChar = Expression->Buffer[ExprOffset / sizeof(WCHAR)];

                //
                //  Before we get started, we have to check for something
                //  really gross.  We may be about to exhaust the local
                //  space for ExpressionMatches[][], so we have to allocate
                //  some pool if this is the case.  Yuk!
                //

                if ( (DestCount >= MATCHES_ARRAY_SIZE - 2) &&
                     (AuxBuffer == NULL) ) {

                    ULONG ExpressionChars;

                    ExpressionChars = Expression->Length / sizeof(WCHAR);

                    AuxBuffer = (PUSHORT)AllocateMemoryP((ExpressionChars + 1) * sizeof(USHORT) * 2 * 2);

                    RtlCopyMemory( AuxBuffer,
                                   CurrentMatches,
                                   MATCHES_ARRAY_SIZE * sizeof(USHORT) );

                    CurrentMatches = AuxBuffer;

                    RtlCopyMemory( AuxBuffer + (ExpressionChars+1)*2,
                                   PreviousMatches,
                                   MATCHES_ARRAY_SIZE * sizeof(USHORT) );

                    PreviousMatches = AuxBuffer + (ExpressionChars+1)*2;
                }

                //
                //  * matches any character zero or more times.
                //

                if (ExprChar == L'*') {

                    CurrentMatches[DestCount++] = CurrentState;
                    CurrentMatches[DestCount++] = CurrentState + 3;
                    continue;
                }

                //
                //  DOS_STAR matches any character except . zero or more times.
                //

                if (ExprChar == DOS_STAR) {

                    BOOL ICanEatADot = FALSE;

                    //
                    //  If we are at a period, determine if we are allowed to
                    //  consume it, ie. make sure it is not the last one.
                    //

                    if ( !NameFinished && (NameChar == '.') ) {

                        USHORT Offset;

                        for ( Offset = NameOffset;
                              Offset < Name->Length;
                              Offset = (USHORT)(Offset + Length) ) {

                            if (Name->Buffer[Offset / sizeof(WCHAR)] == L'.') {

                                ICanEatADot = TRUE;
                                break;
                            }
                        }
                    }

                    if (NameFinished || (NameChar != L'.') || ICanEatADot) {

                        CurrentMatches[DestCount++] = CurrentState;
                        CurrentMatches[DestCount++] = CurrentState + 3;
                        continue;

                    } else {

                        //
                        //  We are at a period.  We can only match zero
                        //  characters (ie. the epsilon transition).
                        //

                        CurrentMatches[DestCount++] = CurrentState + 3;
                        continue;
                    }
                }

                //
                //  The following expression characters all match by consuming
                //  a character, thus force the expression, and thus state
                //  forward.
                //

                CurrentState += (USHORT)(sizeof(WCHAR) * 2);

                //
                //  DOS_QM is the most complicated.  If the name is finished,
                //  we can match zero characters.  If this name is a '.', we
                //  don't match, but look at the next expression.  Otherwise
                //  we match a single character.
                //

                if ( ExprChar == DOS_QM ) {

                    if ( NameFinished || (NameChar == L'.') ) {

                        continue;
                    }

                    CurrentMatches[DestCount++] = CurrentState;
                    break;
                }

                //
                //  A DOS_DOT can match either a period, or zero characters
                //  beyond the end of name.
                //

                if (ExprChar == DOS_DOT) {

                    if ( NameFinished ) {

                        continue;
                    }

                    if (NameChar == L'.') {

                        CurrentMatches[DestCount++] = CurrentState;
                        break;
                    }
                }

                //
                //  From this point on a name character is required to even
                //  continue, let alone make a match.
                //

                if ( NameFinished ) {

                    break;
                }

                //
                //  If this expression was a '?' we can match it once.
                //

                if (ExprChar == L'?') {

                    CurrentMatches[DestCount++] = CurrentState;
                    break;
                }

                //
                //  Finally, check if the expression char matches the name char
                //

                if (ExprChar == (WCHAR)(IgnoreCase ?
                                        UpcaseTable[NameChar] : NameChar)) {

                    CurrentMatches[DestCount++] = CurrentState;
                    break;
                }

                //
                //  The expression didn't match so go look at the next
                //  previous match.
                //

                break;
            }


            //
            //  Prevent duplication in the destination array.
            //
            //  Each of the arrays is monotonically increasing and non-
            //  duplicating, thus we skip over any source element in the src
            //  array if we just added the same element to the destination
            //  array.  This guarantees non-duplication in the dest. array.
            //

            while ((SrcCount < MatchesCount) &&
                   (PreviousDestCount < DestCount)) {

                while ((SrcCount < MatchesCount) &&
                       (PreviousMatches[SrcCount] <
                        CurrentMatches[PreviousDestCount])) {

                    SrcCount += 1;
                }

                PreviousDestCount += 1;
            }
        }

        //
        //  If we found no matches in the just finished iteration, it's time
        //  to bail.
        //

        if ( DestCount == 0 )
        {
            FreeMemoryP(AuxBuffer);
            return FALSE;
        }

        //
        //  Swap the meaning the two arrays
        //

        {
            USHORT *Tmp;

            Tmp = PreviousMatches;

            PreviousMatches = CurrentMatches;

            CurrentMatches = Tmp;
        }

        MatchesCount = DestCount;
    }


    CurrentState = PreviousMatches[MatchesCount-1];

    FreeMemoryP(AuxBuffer);


    return (BOOL)(CurrentState == MaxState);
}

BOOL
IsNameInExpression(
    IN PUNICODE_STRING  Expression,
    IN PUNICODE_STRING  Name,
    IN BOOL             IgnoreCase,
    IN PWSTR            UpcaseTable OPTIONAL
)
{
    BOOL Result = FALSE;
    UNICODE_STRING LocalExpression, LocalName;

    //
    //  If we weren't given an upcase table, we have to upcase the names
    //  ourselves.
    //

    if (IgnoreCase && UpcaseTable == NULL)
    {

        NTSTATUS Status;

        Status = RtlUpcaseUnicodeString(&LocalName, Name, TRUE);
        if (NT_FAILED(Status))
            return FALSE;

        Status = RtlUpcaseUnicodeString(&LocalExpression, Expression, TRUE);
        if (NT_FAILED(Status))
        {
            RtlFreeUnicodeString(&LocalName);
            return FALSE;
        }

        Name = &LocalName;
        Expression = &LocalExpression;

        IgnoreCase = FALSE;

    }
    else
    {
        LocalName.Buffer = NULL;
        LocalExpression.Buffer = NULL;
    }

    //
    //  Now call the main routine, remembering to free the upcased string
    //  if we allocated one.
    //

    SEH_TRY
    {
        Result = IsNameInExpressionPrivate(Expression, Name, IgnoreCase, UpcaseTable);
    }
    SEH_FINALLY
    {
        if (LocalName.Buffer != NULL)
            RtlFreeUnicodeString(&LocalName);

        if (LocalExpression.Buffer != NULL)
            RtlFreeUnicodeString(&LocalExpression);
    }

    return Result;
}

NTSTATUS EnvironmentAppend(PUNICODE_STRING Key, PUNICODE_STRING Value)
{
    PWSTR           EnvBuffer, QuoteBuffer, Sub;
    ULONG           Length;
    UNICODE_STRING  Env, QuoteKey;

    Length = Key->Length + sizeof(*QuoteBuffer) * 2;
    QuoteBuffer = (PWSTR)AllocStack(Length);
    RtlInitEmptyString(&QuoteKey, QuoteBuffer, Length);

    QuoteBuffer[0] = '%';
    QuoteBuffer[Length / sizeof(*QuoteBuffer) - 1] = '%';
    CopyMemory(&QuoteBuffer[1], Key->Buffer, Key->Length);
    QuoteKey.Length = QuoteKey.MaximumLength;

    RtlInitEmptyString(&Env);
    RtlExpandEnvironmentStrings_U(nullptr, &QuoteKey, &Env, &Length);

    Length += Value->MaximumLength + sizeof(*EnvBuffer) * 3;
    EnvBuffer = (PWSTR)AllocStack(Length);
    RtlInitEmptyString(&Env, EnvBuffer, Length);

    RtlExpandEnvironmentStrings_U(nullptr, &QuoteKey, &Env, nullptr);

    Env.Buffer[Env.Length / sizeof(Env.Buffer[0])] = 0;
    Sub = wcsstr(Env.Buffer, Value->Buffer);
    if (Sub != nullptr)
    {
        switch (Sub[Value->Length / sizeof(Value->Buffer[0])])
        {
            case 0:
            case ';':
                return STATUS_SUCCESS;
        }
    }

    RtlAppendUnicodeToString(&Env, L";");
    RtlAppendUnicodeStringToString(&Env, Value);
    RtlAppendUnicodeToString(&Env, L";");

    return RtlSetEnvironmentVariable(nullptr, Key, &Env);
}

ML_NAMESPACE_END_(Rtl);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
