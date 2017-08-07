#ifndef _ACAUTOMATION_H_c7409518_683e_4837_902d_973c3a4ca8ed_
#define _ACAUTOMATION_H_c7409518_683e_4837_902d_973c3a4ca8ed_


#if CPP_DEFINED



#if !defined(COMPACT_TRIE_MAGIC)
    #define COMPACT_TRIE_MAGIC  TAG4('TRIE')
#endif // COMPACT_TRIE_MAGIC

#define TRIE_DEBUG 1


_ML_CPP_HEAD_


using ml::GrowableArray;


typedef ULONG NODE_CONTEXT, *PNODE_CONTEXT;

typedef ULONG  UNSIGNED_OFFSET, *PUNSIGNED_OFFSET;
//typedef LONG   SIGNED_OFFSET, *PSIGNED_OFFSET;

ML_NAMESPACE_BEGIN(TrieBitsIndex)

enum
{
    Flags          = 32,
    NodeCount      = 33,

    Max,
};

ML_NAMESPACE_END_(TrieBitsIndex);

ML_NAMESPACE_BEGIN(TrieBitsFlags)

enum
{
    HasContext      = 0x01,
    HasNodes        = 0x02,
};

ML_NAMESPACE_END_(TrieBitsFlags);

static const ULONG_PTR MAXIMUM_NEXT_NODE = 256;

typedef struct TRIE_NODE
{
    typedef TRIE_NODE *PTRIE_NODE;

    static const ULONG_PTR kInvalidIndex = ULONG_PTR_MAX;

    union
    {
        PVOID           Pointer;
        NODE_CONTEXT    Value;

    } Context;

    PTRIE_NODE Failure;

    BYTE SlotsFlags[TrieBitsIndex::Flags + 1];
    BYTE SlotIndex[MAXIMUM_NEXT_NODE];

    GrowableArray<PTRIE_NODE> Next2;

    TRIE_NODE()
    {
        ZeroMemory(this, sizeof(*this));
        Context.Pointer = (PVOID)-1;
    }

    VOID SetFlags(ULONG_PTR Flags)
    {
        SET_FLAG(SlotsFlags[TrieBitsIndex::Flags], Flags);
    }

    BOOL TestFlags(ULONG_PTR Flags)
    {
        return FLAG_ON(SlotsFlags[TrieBitsIndex::Flags], Flags);
    }

    PTRIE_NODE GetNext(ULONG_PTR Index)
    {
        if (FLAG_OFF(SlotsFlags[Index / 8], 1 << (Index % 8)))
            return nullptr;

        return Next2[SlotIndex[Index]];
    }

    PTRIE_NODE* AllocateSlot(ULONG_PTR Index)
    {
        NTSTATUS Status;

        if (FLAG_OFF(SlotsFlags[Index / 8], 1 << (Index % 8)))
        {
            PTRIE_NODE empty = nullptr;
            Status = Next2.Add(empty);
            if (NT_FAILED(Status))
                return nullptr;

            SET_FLAG(SlotsFlags[Index / 8], 1 << (Index % 8));
            SlotIndex[Index] = (BYTE)(Next2.GetSize() - 1);
        }

        return &Next2[SlotIndex[Index]];
    }

} TRIE_NODE, *PTRIE_NODE;

typedef struct
{
    PVOID           Data;
    ULONG_PTR       SizeInBytes;
    NODE_CONTEXT    Context;

} TRIE_BYTES_ENTRY, *PTRIE_BYTES_ENTRY;

#define ADD_TRIE_STRING(_str, ...) { _str, CONST_STRLEN(_str) * sizeof(_str[0]), __VA_ARGS__ }

typedef struct
{
    ULONG Magic;
    ULONG IndexBitsTableOffset;
    ULONG IndexTableOffset;
    ULONG OffsetTableOffset;
    ULONG NodesDataOffset;
    ULONG Reserve[3];

} COMPACT_TRIE_HEADER, *PCOMPACT_TRIE_HEADER;

typedef struct TRIE_NODE_OFFSET_TABLE_ENTRY
{
    UNSIGNED_OFFSET   IndexBitsOffset;
    UNSIGNED_OFFSET   IndexOffset;

    TRIE_NODE_OFFSET_TABLE_ENTRY(ULONG_PTR IndexBitsOffset = 0, ULONG_PTR IndexOffset = 0)
    {
        this->IndexBitsOffset = (UNSIGNED_OFFSET)IndexBitsOffset;
        this->IndexOffset = (UNSIGNED_OFFSET)IndexOffset;
    }

    BOOL operator==(const TRIE_NODE_OFFSET_TABLE_ENTRY &entry)
    {
        return CompareMemory(this, &entry, sizeof(entry)) == 0;
    }

} TRIE_NODE_OFFSET_TABLE_ENTRY, *PTRIE_NODE_OFFSET_TABLE_ENTRY;


typedef struct COMPACT_TRIE_NODE
{
    USHORT  OffsetTableIndex;

    //ULONG   Context;
    // ULONG RVOffsetOfNodes[];

} COMPACT_TRIE_NODE, *PCOMPACT_TRIE_NODE;


class CompactTrie
{
protected:
    PCOMPACT_TRIE_HEADER            Header;
    PBYTE                           IndexBitsTable;
    PBYTE                           IndexTable;
    PTRIE_NODE_OFFSET_TABLE_ENTRY   OffsetTable;
    PCOMPACT_TRIE_NODE              Root;

public:

    CompactTrie()
    {
        ZeroMemory(this, sizeof(*this));
    }

    ~CompactTrie()
    {
        FreeMemoryP(Header);
    }

    NTSTATUS Initialize(PVOID CompactTree, ULONG_PTR SizeOfTree)
    {
        if (((PCOMPACT_TRIE_HEADER)CompactTree)->Magic * 2 != COMPACT_TRIE_MAGIC * 2)
            return STATUS_CONTEXT_MISMATCH;

        Header = (PCOMPACT_TRIE_HEADER)AllocateMemoryP(SizeOfTree);
        if (Header == nullptr)
            return STATUS_NO_MEMORY;

        CopyMemory(Header, CompactTree, SizeOfTree);

        IndexBitsTable  = (PBYTE)PtrAdd(Header, Header->IndexBitsTableOffset);
        IndexTable      = (PBYTE)PtrAdd(Header, Header->IndexTableOffset);
        OffsetTable     = (PTRIE_NODE_OFFSET_TABLE_ENTRY)PtrAdd(Header, Header->OffsetTableOffset);
        Root            = (PCOMPACT_TRIE_NODE)PtrAdd(Header, Header->NodesDataOffset);

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS Lookup(PTRIE_BYTES_ENTRY Entry, PNODE_CONTEXT Context = nullptr)
    {
        ULONG_PTR           Index;
        PBYTE               Buffer, IndexBitsTable, IndexTable;
        PUNSIGNED_OFFSET    NextNodeOffset;
        PCOMPACT_TRIE_NODE  Node;

        if (this->Root == nullptr)
            return STATUS_NO_MORE_ENTRIES;

        if (Entry->SizeInBytes == 0)
            return STATUS_NOT_FOUND;

        Node = this->Root;

        IndexBitsTable  = PtrAdd(this->IndexBitsTable,  (LONG_PTR)this->OffsetTable[Node->OffsetTableIndex].IndexBitsOffset);
        IndexTable      = PtrAdd(this->IndexTable,      (LONG_PTR)this->OffsetTable[Node->OffsetTableIndex].IndexOffset);

        FOR_EACH(Buffer, (PBYTE)Entry->Data, Entry->SizeInBytes)
        {
            Index = Buffer[0];

            if (FLAG_OFF(IndexBitsTable[Index / 8], 1 << (Index % 8)))
                return STATUS_NOT_FOUND;

            NextNodeOffset = (PUNSIGNED_OFFSET)(Node + 1);
            Node = PtrAdd(Node, NextNodeOffset[IndexTable[Index]]);

            IndexBitsTable  = PtrAdd(this->IndexBitsTable,  (LONG_PTR)this->OffsetTable[Node->OffsetTableIndex].IndexBitsOffset);
            IndexTable      = PtrAdd(this->IndexTable,      (LONG_PTR)this->OffsetTable[Node->OffsetTableIndex].IndexOffset);
        }

        if (FLAG_OFF(IndexBitsTable[TrieBitsIndex::Flags], TrieBitsFlags::HasContext))
            return STATUS_CONTEXT_MISMATCH;

        if (Context != nullptr)
        {
            ULONG_PTR NodeCount;
            PNODE_CONTEXT NodeContext;

            NodeContext = (PNODE_CONTEXT)Node;

            NodeCount = (IndexBitsTable[TrieBitsIndex::NodeCount] + 1) & -FLAG_ON(IndexBitsTable[TrieBitsIndex::Flags], TrieBitsFlags::HasNodes);
            NodeContext = PtrAdd(NodeContext, NodeCount * sizeof(*NextNodeOffset) + sizeof(*Node));

            *Context = *NodeContext;
        }

        return STATUS_SUCCESS;
    }
};


template<typename NodeType = TRIE_NODE>
class StaticTrieT
{
public:
    typedef NodeType *PNodeType;
    typedef GrowableArray<NodeType> NodeArray;
    typedef GrowableArray<PNodeType> NodePArray;

protected:
    typedef struct NODE_OFFSET_MAP
    {
        PNodeType Node;
        ULONG     Offset;

        NODE_OFFSET_MAP(PNodeType Node = nullptr, ULONG Offset = 0)
        {
            this->Node = Node;
            this->Offset = Offset;
        }

    } NODE_OFFSET_MAP, *PNODE_OFFSET_MAP;

    typedef struct NODE_FIX_ENTRY
    {
        PCOMPACT_TRIE_NODE  Compact;
        PNodeType           Node;
        PUNSIGNED_OFFSET    Offset;

        NODE_FIX_ENTRY()
        {
        }

        NODE_FIX_ENTRY(PCOMPACT_TRIE_NODE Compact, PNodeType Node, PVOID Offset)
        {
            this->Compact   = Compact;
            this->Node      = Node;
            this->Offset    = (PUNSIGNED_OFFSET)Offset;
        }

    } NODE_FIX_ENTRY, *PNODE_FIX_ENTRY;

protected:

    PNodeType Root;

public:
    StaticTrieT()
    {
        this->Root = nullptr;
    }

    NoInline PVOID FindReuseData(PVOID Begin, PVOID End, PVOID Data, ULONG_PTR Size)
    {
        PVOID     Found;
        ULONG_PTR SearchLength;

        if (Begin >= End)
            return Begin;

        SearchLength = PtrOffset(End, Begin);

        Found = Sunday(Begin, SearchLength, Data, Size);
        if (Found == nullptr && SearchLength != 0)
        {
            for (ULONG_PTR Count = Size - 1; Count != 0; --Count)
            {
                if (Count > SearchLength)
                    continue;

                if (RtlCompareMemory(PtrSub(End, Count), Data, Count) != Count)
                    continue;

                Found = PtrSub(End, Count);
                break;
            }
        }

        return Found == nullptr ? End : Found;
    }

    NoInline PNODE_OFFSET_MAP LookupNodeOffsetEntry(GrowableArray<NODE_OFFSET_MAP> &NodeOffsetTable, PNodeType Node)
    {
        PNODE_OFFSET_MAP Entry;

        FOR_EACH_VEC(Entry, NodeOffsetTable)
        {
            if (Entry->Node == Node)
                return Entry;
        }

        return nullptr;
    }

    NTSTATUS BuildCompactTree(PVOID *CompactTree, PULONG_PTR CompactSize)
    {
        NTSTATUS                Status;
        PBYTE                   IndexBitsBase, IndexBits;
        PBYTE                   IndexBufferBase, IndexBuffer;
        PBYTE                   NodesBufferBase, NodesBuffer;
        ULONG_PTR               FinalSize, NodeCount, IndexBufferSize, IndexBitsSize, NodesBufferSize, OffsetTableSize;
        PCOMPACT_TRIE_HEADER    CompactHeader;
        PNODE_FIX_ENTRY         FixEntry;

        NodeCount = 0;
        Status = EnumNodes(
                    [&](PNodeType Parent, PNodeType Node, ULONG_PTR IndexOfNext)
                    {
                        ++NodeCount;
                        return STATUS_SUCCESS;
                    }
                );
        FAIL_RETURN(Status);

        GrowableArray<NODE_OFFSET_MAP>              NodeOffsetTable;
        GrowableArray<NODE_FIX_ENTRY>               NodeFixEntries;
        GrowableArray<TRIE_NODE_OFFSET_TABLE_ENTRY> OffsetEntries;

        IndexBitsBase   = nullptr;
        IndexBufferBase = nullptr;
        NodesBufferBase = nullptr;

        SCOPE_EXIT
        {
            FreeMemoryP(IndexBitsBase);
            FreeMemoryP(IndexBufferBase);
            FreeMemoryP(NodesBufferBase);
        }
        SCOPE_EXIT_END;

        IndexBitsSize   = NodeCount * TrieBitsIndex::Max;
        IndexBufferSize = NodeCount * MAXIMUM_NEXT_NODE;
        NodesBufferSize = NodeCount * sizeof(*this->Root);

        IndexBufferBase = (PBYTE)AllocateMemoryP(IndexBufferSize);
        if (IndexBufferBase == nullptr)
            return STATUS_NO_MEMORY;

        IndexBitsBase = (PBYTE)AllocateMemoryP(IndexBitsSize);
        if (IndexBitsBase == nullptr)
            return STATUS_NO_MEMORY;

        NodesBufferBase = (PBYTE)AllocateMemoryP(NodesBufferSize);
        if (NodesBufferBase == nullptr)
            return STATUS_NO_MEMORY;

        IndexBits   = IndexBitsBase;
        IndexBuffer = IndexBufferBase;
        NodesBuffer = NodesBufferBase;

        ZeroMemory(IndexBits, IndexBitsSize);
        ZeroMemory(IndexBuffer, IndexBufferSize);
        ZeroMemory(NodesBuffer, NodesBufferSize);

        Status = EnumNodes(
                    [&](PNodeType Parent, PNodeType Node, ULONG_PTR IndexOfNext)
                    {
                        BYTE                            LocalIndex[MAXIMUM_NEXT_NODE];
                        BYTE                            LocalBits[TrieBitsIndex::Max];
                        ULONG_PTR                       OffsetTableIndex;
                        ULONG_PTR                       NodeCount, Length, FirstNodeIndex, LastNodeIndex;
                        PNodeType                      *NextBase, Next;
                        PBYTE                           ReferenceBuffer, SearchBase, SearchEnd;
                        TRIE_NODE_OFFSET_TABLE_ENTRY    TableEntry;
                        PCOMPACT_TRIE_NODE              CompactNode;

                        ZeroMemory(LocalIndex, sizeof(LocalIndex));
                        ZeroMemory(LocalBits, sizeof(LocalBits));

                        CompactNode = (PCOMPACT_TRIE_NODE)NodesBuffer;
                        NodesBuffer += sizeof(*CompactNode);

                        NodeCount = 0;

                        FirstNodeIndex = ULONG_PTR_MAX;
                        LastNodeIndex = 0;

                        for (ULONG_PTR Index = 0; Index != MAXIMUM_NEXT_NODE; ++Index)
                        {
                            Next = Node->GetNext(Index);
                            if (Next == nullptr)
                                continue;

                            LocalBits[Index / 8] |= 1 << (Index % 8);
                            LocalIndex[Index] = (BYTE)NodeCount++;

                            NodeFixEntries.Add(NODE_FIX_ENTRY(CompactNode, Next, NodesBuffer));

                            NodesBuffer += sizeof(*NodeFixEntries[0].Offset);

                            FirstNodeIndex = ML_MIN(FirstNodeIndex, Index);
                            LastNodeIndex = Index + 1;
                        }

                        if (Node->TestFlags(TrieBitsFlags::HasContext))
                        {
                            *(PNODE_CONTEXT)NodesBuffer = Node->Context.Value;
                            NodesBuffer += sizeof(Node->Context.Value);
                            SET_FLAG(LocalBits[TrieBitsIndex::Flags], TrieBitsFlags::HasContext);
                        }

                        if (NodeCount != 0)
                        {
                            SET_FLAG(LocalBits[TrieBitsIndex::Flags], TrieBitsFlags::HasNodes);
                            LocalBits[TrieBitsIndex::NodeCount] = (BYTE)(NodeCount - 1);
                        }

                        NodeOffsetTable.Add(NODE_OFFSET_MAP(Node, PtrOffset(CompactNode, NodesBufferBase)));

                        SearchBase      = IndexBitsBase;
                        SearchEnd       = IndexBits;
                        Length          = sizeof(LocalBits);
                        ReferenceBuffer = (PBYTE)FindReuseData(SearchBase, SearchEnd, LocalBits, Length);

                        TableEntry.IndexBitsOffset = (UNSIGNED_OFFSET)PtrOffset(ReferenceBuffer, IndexBitsBase);

                        if (ReferenceBuffer + Length > SearchEnd)
                        {
                            CopyMemory(ReferenceBuffer, LocalBits, Length);
                            IndexBits = PtrAdd(ReferenceBuffer, Length);
                        }

                        TableEntry.IndexOffset = (UNSIGNED_OFFSET)-1;

                        if (NodeCount != 0)
                        {
                            ULONG_PTR ForwardLength;

                            ForwardLength = FirstNodeIndex * sizeof(LocalIndex[0]);

                            SearchBase      = IndexBufferBase + sizeof(LocalIndex);
                            SearchEnd       = IndexBuffer;
                            Length          = (LastNodeIndex - FirstNodeIndex) * sizeof(LocalIndex[0]);
                            ReferenceBuffer = (PBYTE)FindReuseData(SearchBase, SearchEnd, &LocalIndex[FirstNodeIndex], Length);

                            TableEntry.IndexOffset = (UNSIGNED_OFFSET)(PtrOffset(ReferenceBuffer, IndexBufferBase) - ForwardLength);

                            if (ReferenceBuffer + Length > SearchEnd)
                            {
                                CopyMemory(ReferenceBuffer, &LocalIndex[FirstNodeIndex], Length);
                                IndexBuffer = PtrAdd(ReferenceBuffer, Length);
                            }
                        }

                        OffsetTableIndex = OffsetEntries.IndexOf(TableEntry);
                        if (OffsetTableIndex == OffsetEntries.kInvalidIndex)
                        {
                            OffsetEntries.Add(TableEntry);
                            OffsetTableIndex = OffsetEntries.GetSize() - 1;
                        }

                        if (OffsetTableIndex > 0xFFFF)
                            DbgBreakPoint();

                        CompactNode->OffsetTableIndex = (USHORT)OffsetTableIndex;

                        return STATUS_SUCCESS;
                    }
                );

        FOR_EACH_VEC(FixEntry, NodeFixEntries)
        {
            ULONG_PTR   CompactSize, NodeCount, Offset, ContextSize;
            PBYTE       IndexBits, IndexTable;
            PVOID       CompactEnd;
            PTRIE_NODE_OFFSET_TABLE_ENTRY TableEntry = &OffsetEntries[FixEntry->Compact->OffsetTableIndex];

            IndexBits = PtrAdd(IndexBitsBase, TableEntry->IndexBitsOffset);
            IndexTable = PtrAdd(IndexBufferBase, TableEntry->IndexOffset);

            NodeCount = (IndexBits[TrieBitsIndex::NodeCount] + 1) & -FLAG_ON(IndexBits[TrieBitsIndex::Flags], TrieBitsFlags::HasNodes);
            ContextSize = FLAG_ON(IndexBits[TrieBitsIndex::Flags], TrieBitsFlags::HasContext) ? sizeof(NODE_CONTEXT) : 0;

            CompactSize = sizeof(*FixEntry->Compact);
            CompactSize += NodeCount * sizeof(*FixEntry->Offset);
            CompactSize += ContextSize;

            //CompactEnd = PtrAdd(FixEntry->Compact, CompactSize);

            Offset = LookupNodeOffsetEntry(NodeOffsetTable, FixEntry->Node)->Offset;
            Offset = PtrOffset(Offset, PtrOffset(FixEntry->Compact, NodesBufferBase));

            //if (Offset >= 0xFFFF) DbgBreakPoint();

            *FixEntry->Offset =(UNSIGNED_OFFSET)Offset;
        }

        IndexBitsSize   = PtrOffset(IndexBits, IndexBitsBase);
        IndexBufferSize = PtrOffset(IndexBuffer, IndexBufferBase);
        NodesBufferSize = PtrOffset(NodesBuffer, NodesBufferBase);
        OffsetTableSize = OffsetEntries.GetSize() * sizeof(OffsetEntries[0]);

        FinalSize = sizeof(*CompactHeader) + IndexBitsSize + IndexBufferSize + OffsetTableSize + NodesBufferSize + 0x200;

        *CompactTree = AllocateMemoryP(FinalSize);

        if (*CompactTree != nullptr)
        {
            CompactHeader = (PCOMPACT_TRIE_HEADER)*CompactTree;

            ZeroMemory(CompactHeader, FinalSize);

            CompactHeader->Magic                = COMPACT_TRIE_MAGIC;
            CompactHeader->IndexBitsTableOffset = sizeof(*CompactHeader);
            CompactHeader->IndexTableOffset     = CompactHeader->IndexBitsTableOffset   + ROUND_UP(IndexBitsSize, 16);
            CompactHeader->OffsetTableOffset    = CompactHeader->IndexTableOffset       + ROUND_UP(IndexBufferSize, 16);
            CompactHeader->NodesDataOffset      = CompactHeader->OffsetTableOffset      + ROUND_UP(OffsetTableSize, 16);

            ++CompactHeader;

            CopyMemory(CompactHeader, IndexBitsBase, IndexBitsSize);
            CompactHeader = PtrAdd(CompactHeader, ROUND_UP(IndexBitsSize, 16));

            CopyMemory(CompactHeader, IndexBufferBase, IndexBufferSize);
            CompactHeader = PtrAdd(CompactHeader, ROUND_UP(IndexBufferSize, 16));

            CopyMemory(CompactHeader, OffsetEntries.GetData(), OffsetTableSize);
            CompactHeader = PtrAdd(CompactHeader, ROUND_UP(OffsetTableSize, 16));

            CopyMemory(CompactHeader, NodesBufferBase, NodesBufferSize);
            CompactHeader = PtrAdd(CompactHeader, ROUND_UP(NodesBufferSize, 16));

            *CompactSize = PtrOffset(CompactHeader, *CompactTree);
        }
        else
        {
            Status = STATUS_NO_MEMORY;
        }

        return Status;
    }

    /*++

    NTSTATUS CallBack(PNodeType Parent, PNodeType Node, ULONG_PTR IndexOfNext);

    --*/

    template<typename CALL_BACK> NTSTATUS EnumNodes(CALL_BACK CallBack)
    {
        NTSTATUS    Status;
        ULONG_PTR   QueueIndex;
        PNodeType   Root, Node, Next;

        static const ULONG_PTR NumberOfNext = MAXIMUM_NEXT_NODE;

        Root        = this->Root;
        QueueIndex  = 0;

        if (Root == nullptr)
            return STATUS_NO_MORE_ENTRIES;


        NodePArray Queue;

        Status = Queue.Add(Root);
        FAIL_RETURN(Status);

        Status = CallBack(nullptr, Root, Queue.kInvalidIndex);
        FAIL_RETURN(Status);

        do
        {
            Node = Queue[QueueIndex++];

            for (ULONG_PTR Index = 0; Index != NumberOfNext; ++Index)
            {
                Next = Node->GetNext(Index);
                if (Next == nullptr)
                    continue;

                Status = CallBack(Node, Next, Index);
                FAIL_RETURN(Status);

                Status = Queue.Add(Next);
                FAIL_RETURN(Status);
            }

        } while (QueueIndex != Queue.GetSize());

        return STATUS_SUCCESS;
    }

    NTSTATUS LookupWithoutFailure(PTRIE_BYTES_ENTRY DataToLookup, PNODE_CONTEXT Context = nullptr)
    {
        return LookupWorker(
                    [](PNodeType)
                    {
                        return nullptr;
                    },
                    DataToLookup,
                    Context
                );
    }

    NTSTATUS Lookup(PTRIE_BYTES_ENTRY DataToLookup, PNODE_CONTEXT Context = nullptr)
    {
        return LookupWorker(
                    [](PNodeType Node)
                    {
                        return Node->Failure;
                    },
                    DataToLookup,
                    Context
                );
    }


protected:

    template<typename T>
    NoInline
    NTSTATUS
    LookupWorker(
        T                   GetFailureRoutine,
        PTRIE_BYTES_ENTRY   DataToLookup,
        PNODE_CONTEXT       Context = nullptr
    )
    {
        PBYTE       Buffer;
        ULONG_PTR   Index;
        PNodeType   Node, Next;

        Node = this->Root;
        if (Node == nullptr)
            return STATUS_FLT_NOT_INITIALIZED;

        FOR_EACH(Buffer, (PBYTE)DataToLookup->Data, DataToLookup->SizeInBytes)
        {
            Index = Buffer[0];
            Next = Node->GetNext(Index);
            while (Next == nullptr)
            {
                Node = GetFailureRoutine(Node);
                if (Node == nullptr)
                    return STATUS_NOT_FOUND;

                Next = Node->GetNext(Index);
            }

            Node = Next;
        }

        if (!Node->TestFlags(TrieBitsFlags::HasContext))
            return STATUS_CONTEXT_MISMATCH;

        if (Context != nullptr)
            *Context = Node->Context.Value;

        return STATUS_SUCCESS;
    }
};

template<typename NodeType = TRIE_NODE>
class TrieT : public StaticTrieT<NodeType>
{
public:

#if TRIE_DEBUG

    LONG_PTR NodeCount;

#endif

    TrieT()
    {
#if TRIE_DEBUG
        NodeCount = 0;
#endif
    }

    ~TrieT()
    {
        PNodeType *Node;
        NodePArray FreeList;

        this->EnumNodes(
            [&](PNodeType Parent, PNodeType Node, ULONG_PTR IndexOfNext) -> NTSTATUS
            {
                if (Parent != nullptr)
                    FreeList.Add(Node);

                return STATUS_SUCCESS;
            }
        );

        FOR_EACH_VEC(Node, FreeList)
        {
            DestroyNode(*Node);
        }

        DestroyNode(this->Root);
    }

public:

    NTSTATUS InsertBytesEntry(PTRIE_BYTES_ENTRY Bytes)
    {
        PBYTE       Buffer;
        ULONG_PTR   Index;
        PNodeType   Node, Next, *Slot;

        if (this->Root == nullptr)
            return STATUS_FLT_NOT_INITIALIZED;

        if (Bytes->SizeInBytes == 0)
            return STATUS_BUFFER_TOO_SMALL;

        Node = this->Root;

        FOR_EACH(Buffer, (PBYTE)Bytes->Data, Bytes->SizeInBytes)
        {
            Index = Buffer[0];

            Slot = Node->AllocateSlot(Index);
            if (Slot == nullptr)
                return STATUS_NO_MEMORY;

            Next = *Slot;
            if (Next == nullptr)
            {
                Next = CreateNode();
                if (Next == nullptr)
                    return STATUS_NO_MEMORY;

                *Slot = Next;
            }

            Node = Next;
        }

        Node->SetFlags(TrieBitsFlags::HasContext);
        Node->Context.Value = Bytes->Context;

        return STATUS_SUCCESS;
    }

    NTSTATUS UpdateFailurePointers()
    {
        return EnumNodes(
                    [=](PNodeType Parent, PNodeType Node, ULONG_PTR IndexOfNext) -> NTSTATUS
                    {
                        PNodeType Failure, Next;

                        if (Parent == nullptr)
                            return STATUS_SUCCESS;

                        if (Parent == this->Root)
                        {
                            Node->Failure = nullptr;
                        }
                        else
                        {
                            Failure = Parent->Failure;
                            while (Failure != nullptr)
                            {
                                Next = Failure->GetNext(IndexOfNext);
                                if (Next != nullptr)
                                {
                                    Failure = Next;
                                    break;
                                }

                                Failure = Failure->Failure;
                            }

                            Node->Failure = Failure;
                        }

                        return STATUS_SUCCESS;
                    }
                );
    }

    NoInline NTSTATUS InitializeRootNode()
    {
        this->Root = CreateNode();
        if (this->Root == nullptr)
            return STATUS_NO_MEMORY;

        return STATUS_SUCCESS;
    }

    NoInline NTSTATUS BuildFromBytesList(PTRIE_BYTES_ENTRY BytesList, ULONG_PTR NumberOfBytes)
    {
        NTSTATUS Status = STATUS_NO_MORE_ENTRIES;

        FOR_EACH(BytesList, BytesList, NumberOfBytes)
        {
            Status = InsertBytesEntry(BytesList);
            FAIL_BREAK(Status);
        }

        if (NT_SUCCESS(Status))
            Status = UpdateFailurePointers();

        return Status;
    }

    NTSTATUS Release()
    {
        if (this == nullptr)
            return STATUS_SUCCESS;

        delete this;

        return STATUS_SUCCESS;
    }

    static NTSTATUS CreateTrieFromBytesList(TrieT **Tree, PTRIE_BYTES_ENTRY BytesList, ULONG_PTR NumberOfBytes)
    {
        TrieT*      root;
        NTSTATUS    Status;

        root = new TrieT();
        if (root == nullptr)
            return STATUS_NO_MEMORY;

        Status = root->InitializeRootNode();
        if (NT_FAILED(Status))
        {
            root->Release();
            return Status;
        }

        Status = root->BuildFromBytesList(BytesList, NumberOfBytes);
        if (NT_FAILED(Status))
        {
            root->Release();
            return Status;
        }

        *Tree = root;
        return Status;
    }

protected:

    PNodeType CreateNode()
    {
        PNodeType Node = new NodeType();

#if TRIE_DEBUG
        NodeCount += Node != nullptr;
#endif

        return Node;
    }

    VOID DestroyNode(PNodeType Node)
    {

#if TRIE_DEBUG
        NodeCount -= Node != nullptr;
        if (NodeCount < 0)
            DbgBreakPoint();
#endif
        delete Node;
    }
};

typedef TrieT<> Trie;


_ML_CPP_TAIL_

#endif // CPP_DEFINED

#endif // _ACAUTOMATION_H_c7409518_683e_4837_902d_973c3a4ca8ed_