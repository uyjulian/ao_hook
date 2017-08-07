#ifndef _HASHTABLE_H_f92cdc12_70f0_4679_aa3d_d9e1a22117ed_
#define _HASHTABLE_H_f92cdc12_70f0_4679_aa3d_d9e1a22117ed_

template<typename TYPE, ULONG_PTR INITIAL_TABLE_SIZE = 521>
class HashTableT
{
    typedef TypeTraits<TYPE> VALUE_TYPE;

    typedef typename VALUE_TYPE::VALUE_TYPE      VALUE;
    typedef typename VALUE_TYPE::REF_TYPE        VALUE_REF;
    typedef typename VALUE_TYPE::CONST_REF_TYPE  CONST_VALUE_REF;
    typedef VALUE   *PVALUE;


    typedef struct HASH_VALUE
    {
        ULONG           Index;
        ULARGE_INTEGER  Key;

        HASH_VALUE()
        {
            Reset();
        }

        ForceInline BOOL Valid()
        {
            return this->Key.QuadPart != 0;
        }

        ForceInline VOID Reset()
        {
            ZeroMemory(this, sizeof(*this));
        }

        BOOL operator== (const HASH_VALUE& that)
        {
            return this->Key.QuadPart == that.Key.QuadPart;
        }

    } HASH_VALUE;

    typedef struct
    {
        HASH_VALUE  Hash;
        VALUE       Element;

    } HASH_TABLE_ITEM, *PHASH_TABLE_ITEM;


    typedef HASH_TABLE_ITEM HashTableEntry;

    template<class KEY_TYPE>
    class Hasher
    {
    public:
        static HASH_VALUE Hash(HashTableT* thiz, typename TypeTraits<KEY_TYPE>::CONST_REF_TYPE Key);
    };

#define HASHER_END };
#define HASHER(type, argtype) \
    template<> class Hasher<type> \
    { \
    public: \
        static HASH_VALUE Hash(HashTableT* thiz, argtype Key)

    HASHER(PWSTR, PWSTR)
    {
        return thiz->HashString(Key);
    }
    HASHER_END

    HASHER(PCWSTR, PCWSTR)
    {
        return thiz->HashString(Key);
    }
    HASHER_END

    HASHER(INT32, INT32)
    {
        return thiz->HashData(&Key, sizeof(Key));
    }
    HASHER_END

    HASHER(INT64, INT64)
    {
        return thiz->HashData(&Key, sizeof(Key));
    }
    HASHER_END

    HASHER(UINT32, UINT32)
    {
        return thiz->HashData(&Key, sizeof(Key));
    }
    HASHER_END

    HASHER(UINT64, UINT64)
    {
        return thiz->HashData(&Key, sizeof(Key));
    }
    HASHER_END

    HASHER(LONG, LONG)
    {
        return thiz->HashData(&Key, sizeof(Key));
    }
    HASHER_END

    HASHER(ULONG, ULONG)
    {
        return thiz->HashData(&Key, sizeof(Key));
    }
    HASHER_END

public:
    NoInline HashTableT()
    {
        this->Entries = nullptr;
        this->CalcTable = nullptr;
        this->TableUsedSize = 0;
        this->TableSize = 0;

        this->GrowthFactor = 1.3;
    }

    NoInline ~HashTableT()
    {
        SafeDeleteT(this->CalcTable);
        SafeDeleteArrayT(this->Entries);
    }

    NoInline NTSTATUS Initialize()
    {
        HashTableEntry *Entry;

        ml::MlInitialize();
        this->CalcTable = new ULONG[this->CalcTableSize];
        if (this->CalcTable == nullptr)
            return STATUS_NO_MEMORY;

        ULONG Seed = 0x00100001;

        for (int index1 = 0; index1 != 0x100; index1++)
        {
            for(int index2 = index1, i = 5; i != 0; index2 += 0x100, --i)
            {
                ULONG temp1, temp2;

                Seed = (Seed * 125 + 3) % 0x2AAAAB;
                temp1 = (Seed & 0xFFFF) << 16;
                Seed = (Seed * 125 + 3) % 0x2AAAAB;
                temp2 = (Seed & 0xFFFF);
                this->CalcTable[index2] = temp1 | temp2;
            }
        }

        return STATUS_SUCCESS;
    }

    ULONG_PTR Count()
    {
        return this->TableUsedSize;
    }

    ULONG_PTR Size()
    {
        return this->TableSize;
    }

    NoInline HASH_VALUE HashString(PCSTR Ansi, ULONG_PTR Length = -1)
    {
        PSTR Local;

        if (Length == -1)
            Length = StrLengthA(Ansi);

        //Local = (PSTR)AllocStack(Length);
        //CopyMemory(Local, Ansi, Length);
        //StringUpperA(Local, Length);
        Local = (PSTR)Ansi;

        return HashData(Local, Length);
    }

    NoInline HASH_VALUE HashString(PCWSTR Unicode, ULONG_PTR Length = -1)
    {
        PWSTR Local;

        if (Length == -1)
            Length = StrLengthW(Unicode);

        Length *= sizeof(Unicode[0]);

        //Local = (PWSTR)AllocStack(Length);
        //CopyMemory(Local, Unicode, Length);
        //StringUpperW(Local, Length / sizeof(Unicode[0]));
        Local = (PWSTR)Unicode;

        return HashData(Local, Length);
    }

    ForceInline VOID Update(ULONG b, ULONG& v1, ULONG& v2, ULONG t)
    {
        v1 = this->CalcTable[(t << 8) + b] ^ (v1 + v2);
        v2 = b + v1 + v2 + (v2 << 5) + 3;
    }

    NoInline HASH_VALUE HashData(PVOID Bytes, ULONG_PTR Length)
    {
        ULONG       Seed1, Seed2, Seed3, b;
        PBYTE       Data;
        HASH_VALUE  Hash;

        Data = (PBYTE)Bytes;

        Hash.Index          = 0x7FED7FED;
        Hash.Key.LowPart    = 0x7FED7FED;
        Hash.Key.HighPart   = 0x7FED7FED;

        Seed1 = 0xEEEEEEEE;
        Seed2 = 0xEEEEEEEE;
        Seed3 = 0xEEEEEEEE;
        for (; Length; Length--)
        {
            b = *Data++;

            Update(b, Hash.Index, Seed1, 0);
            Update(b, Hash.Key.LowPart, Seed2, 1);
            Update(b, Hash.Key.HighPart, Seed3, 2);
        }

        return Hash;
    }

    template<typename KEY_TYPE>
    NoInline VALUE_REF Add(const KEY_TYPE& Key, CONST_VALUE_REF Value)
    {
        return AddElement(Hasher<KEY_TYPE>::Hash(this, Key), Value).Element;
    }

    NoInline VALUE_REF Add(PVOID Key, ULONG_PTR Length, CONST_VALUE_REF Value)
    {
        return AddElement(HashData(Key, Length), Value).Element;
    }

    template<typename KEY_TYPE>
    NoInline PVALUE Get(const KEY_TYPE& Key)
    {
        return LookupElement(Hasher<KEY_TYPE>::Hash(this, Key));
    }

    NoInline PVALUE Get(PVOID Key, ULONG_PTR Length)
    {
        return LookupElement(HashString(Key, Length));
    }

    template<class STRING_TYPE>
    NoInline BOOL Contains(STRING_TYPE StringKey)
    {
        return Get(StringKey) != nullptr;
    }

    NoInline BOOL Contains(PVOID Bytes, ULONG_PTR Length)
    {
        return Get(Bytes, Length) != nullptr;
    }

    template<class STRING_TYPE>
    NoInline VOID Remove(STRING_TYPE StringKey)
    {
        RemoveElement(HashString(StringKey));
    }

    NoInline VOID Remove(PVOID Bytes, ULONG_PTR Length)
    {
        RemoveElement(HashData(Bytes, Length));
    }

protected:
    NoInline NTSTATUS IncreaseCapacity()
    {
        if (this->TableSize * 3 / 4 > this->TableUsedSize)
            return STATUS_SUCCESS;

        ULONG_PTR NewSize, OldSize;
        HashTableEntry *NewEntries, *OldEntries, *Entry;

        NewSize = GetTableSize(this->TableSize == 0 ? INITIAL_TABLE_SIZE : (ULONG_PTR)(this->TableSize * this->GrowthFactor));
        NewEntries = new HashTableEntry[NewSize];
        if (NewEntries == nullptr)
            return STATUS_NO_MEMORY;

        OldEntries = this->Entries;
        OldSize = this->TableSize;

        this->Entries = NewEntries;
        this->TableSize = NewSize;
        this->TableUsedSize = 0;

        if (OldEntries == nullptr)
            return STATUS_SUCCESS;

        FOR_EACH(Entry, OldEntries, OldSize)
        {
            if (Entry->Hash.Valid())
                AddElement(Entry->Hash, Entry->Element);
        }

        delete[] OldEntries;

        return STATUS_SUCCESS;
    }

    NoInline HASH_TABLE_ITEM& AddElement(const HASH_VALUE& Hash, CONST_VALUE_REF Element)
    {
        IncreaseCapacity();

        auto Entry = LookupEntry(Hash, TRUE);

        Entry->Hash = Hash;
        Entry->Element = Element;
        ++this->TableUsedSize;

        return *Entry;
    }

    NoInline VOID RemoveElement(const HASH_VALUE& Hash)
    {
        auto Entry = LookupEntry(Hash, FALSE);

        if (Entry == nullptr)
            return;

        Entry->Hash.Reset();
        Entry->Element.~TYPE();
        --this->TableUsedSize;
    }

    NoInline PVALUE LookupElement(const HASH_VALUE& Hash)
    {
        auto Entry = this->LookupEntry(Hash, FALSE);
        return Entry == nullptr ? nullptr : &Entry->Element;
    }

    NoInline HashTableEntry* LookupEntry(const HASH_VALUE& Hash, BOOL Empty)
    {
        ULONG_PTR       Index, InitialIndex;
        HashTableEntry* Entry;

        if (this->TableSize == 0)
            return nullptr;

        InitialIndex = Hash.Index % this->TableSize;
        Index = InitialIndex;
        Entry = &this->Entries[InitialIndex];

        if (Empty == FALSE)
        {
            do
            {
                if (Entry->Hash == Hash)
                    return Entry;

                if (Entry->Hash.Valid() == FALSE)
                    return nullptr;

                ++Index;
                ++Entry;
                Index = Index == this->TableSize ? 0 : Index;
                Entry = Index == 0 ? this->Entries : Entry;

            } while (Index != InitialIndex);
        }
        else
        {
            do
            {
                if (Entry->Hash == Hash)
                    return Entry;

                if (Entry->Hash.Valid() == FALSE)
                    return Entry;

                ++Index;
                ++Entry;
                Index = Index == this->TableSize ? 0 : Index;
                Entry = Index == 0 ? this->Entries : Entry;

            } while (Index != InitialIndex);
        }

        return nullptr;
    }

    BOOL MillerRabin(ULONG64 n, ULONG_PTR k)
    {
        if(n == k)
            return TRUE;

        if (n == 2 || n == 3 )
            return TRUE;

        if (n <= 1 || !(n & 1))
            return FALSE;

        ULONG64 s, d, b, e, x;

        // Factor n-1 as d 2^s
        for(s = 0, d = n - 1; !(d & 1); s++)
            d >>= 1;

        // x = k^d mod n using exponentiation by squaring
        // The squaring overflows for n >= 2^32
        for(x = 1, b = k % n, e = d; e; e >>= 1)
        {
            if(e & 1)
                x = (x * b) % n;

            b = (b * b) % n;
        }

        // Verify k^(d 2^[0…s-1]) mod n != 1
        if(x == 1 || x == n-1)
            return TRUE;

        while(s-- > 1)
        {
            x = (x * x) % n;
            if(x == 1)
                return FALSE;

            if(x == n-1)
                return TRUE;
        }

        return FALSE;
    }

    BOOL IsPrime(ULONG_PTR n)
    {
        return (n > 73 &&
                !(n % 2 && n % 3 && n % 5 && n % 7 &&
                  n % 11 && n % 13 && n % 17 && n % 19 && n % 23 && n % 29 &&
                  n % 31 && n % 37 && n % 41 && n % 43 && n % 47 && n % 53 &&
                  n % 59 && n % 61 && n % 67 && n % 71 && n % 73)
                ) ? FALSE:
                    MillerRabin(n, 2) && MillerRabin(n, 7) && MillerRabin(n, 61);
    }

    ULONG_PTR GetTableSize(ULONG_PTR TableSize)
    {
        if ((TableSize & 1) == 0)
            ++TableSize;

        while (IsPrime(TableSize) == FALSE)
            TableSize += 2;

        return TableSize;
    }

protected:
    HashTableEntry* Entries;
    PULONG          CalcTable;
    ULONG_PTR       TableSize;
    ULONG_PTR       TableUsedSize;
    DOUBLE          GrowthFactor;

    static const ULONG_PTR CalcTableSize = 0x500;
};

#endif // _HASHTABLE_H_f92cdc12_70f0_4679_aa3d_d9e1a22117ed_