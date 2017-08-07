// #pragma comment(linker, "/SECTION:.text,ERW /MERGE:.rdata=.text /MERGE:.data=.text")
// #pragma comment(linker, "/SECTION:.Asuna,ERW /MERGE:.text=.Asuna")
// #pragma comment(linker, "/ENTRY:DllMain")


#include "edao.h"
//#include "SoundArc.h"
#include "ml/mylibrary.h"
//#include "edao_vm.h"


#define CONSOLE_DEBUG   0

// BOOL UnInitialize(PVOID BaseAddress)
// {
//     return FALSE;
// }

#include "Aki.h"
//#include "freetype_hook.h"

VOID Test()
{
    //CHAR_T_STATUS_Ratio_To_Json("config_status.txt");
    //CHAR_T_STATUS_Ratio_From_Json("ed_ao_ex.status.config.txt");
    //CHAR_T_STATUS_Ratio_To_Json("config_status_mem.txt");
    //DumpChrRawStatus(L"ed_ao_ex.status.dump.txt");
}

VOID LoadStatusConfig()
{
    if (!Io::IsPathExists(L(EX_DIR)))
    {
        NtFileDisk file;
        file.CreateDirectory(L(EX_DIR));
        file.Close();
    }

    if (bConfigStatus)
    {
        // if (!Io::IsPathExists(STATUS_CONFIG_PATH))
        // {
        //     CHAR_T_STATUS_Ratio_To_Json(STATUS_CONFIG_PATH);
        //     if (bOutputStatusConfigResult)
        //     {
        //         NFILE::WriteFileW(L(STATUS_RESULT_PATH), "配置文件不存在，已重新生成", sizeof("配置文件不存在，已重新生成")-sizeof(CHAR));
        //     }
        // }
        // else
        // {
        //     if (CHAR_T_STATUS_Ratio_From_Json(STATUS_CONFIG_PATH))
        //     {
        //         if (bOutputStatusConfigResult)
        //         {
        //             CHAR_T_STATUS_Ratio_To_Json(STATUS_RESULT_PATH);
        //             CHAR_T_STATUS_Ratio_To_Formula_Ansi(L(STATUS_FORMULA_PATH));
        //             CHAR_T_STATUS_Ratio_To_Formula_HTML(L(STATUS_FORMULA_HTML_PATH));
        //         }
        //     }
        //     else if (bOutputStatusConfigResult)
        //     {
        //         NFILE::WriteFileW(L(STATUS_RESULT_PATH), "配置文件解析失败", sizeof("配置文件解析失败")-sizeof(CHAR));
        //     }
        // }
    }
    else if (nYin_no_AGLRate)
    {
        T_NAME::RatioY[T_NAME::Yin].AGLRate = nYin_no_AGLRate;
    }

    if (bDumpStatus)
    {
        DumpChrRawStatusAnsi(L(STATUS_DUMP_PATH));
        //DumpChrRawStatusUnicode(L(STATUS_DUMP_PATH)L".UNICODE");
    }
}

BOOL Initialize_ex(PVOID BaseAddress)
{
    //ml::MlInitialize();

    // LdrDisableThreadCalloutsForDll(BaseAddress);

    // SetExeDirectoryAsCurrent();

    //mark
    *(PULONG_PTR)&StubGetAsyncKeyState = 0xAED230;
    //StubGetAsyncKeyState = GetAsyncKeyState;

//    HMODULE hModule = GetExeModuleHandle();

#if CONSOLE_DEBUG
    QueryPerformanceFrequency(&lFrequency);

    //*(PBOOL)((PBYTE)Nt_GetModuleHandle(L"DINPUT8.dll") + 0x70E8) = FALSE;
#endif

/*
    {
        HMODULE hModuleUser32 = Nt_GetModuleHandle(L"user32.dll");
        ULONG_PTR procSendMessageA = (ULONG_PTR)Nt_GetProcAddress(hModuleUser32, "SendMessageA");
        static unsigned char pSendMessageAData[5] = {0x8B, 0xFF, 0x55, 0x8B, 0xEC};
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(pSendMessageAData, 5, procSendMessageA-(ULONG_PTR)hModuleUser32),
        };
        Nt_PatchMemory(p, countof(p), nullptr, 0, hModuleUser32);
    }

    static unsigned char p0086B6A0[9] = {0x55, 0x8B, 0xEC, 0x81, 0xEC, 0x54, 0x01, 0x00ull, 0x00ull};
*/
    using namespace Mp;

    PATCH_MEMORY_DATA p[] =
    {
        // bug fix
        MemoryPatchRva(0xEB,  1,  0x60CC8F),      // burst condition
        MemoryPatchRva(0xEB,  1,  0x623062),      // inc CP when can't sbreak

        // restore
        //MemoryPatchRva(p0086B6A0, 9, 0x0086B6A0-0x400000),

        // CHAR_T_STATUS.HP SHORT->INT
        // MemoryPatchRva(0x9000488B,    4,  0x006C1682-0x400000),   // HP SHORT-INT
        //MemoryPatchRva(0x0090D233,    3,  0x006C1692-0x400000),   // EP 0
        // MemoryPatchRva(0x9000488B,    4,  0x0072E778-0x400000),   // HP SHORT-INT
        // MemoryPatchRva(0x9000488B,    4,  0x0072E782-0x400000),   // HP SHORT-INT
        //MemoryPatchRva(0x0090C933,    3,  0x0072E78D-0x400000),   // EP 0
        //MemoryPatchRva(0x0090C933,    3,  0x0072E798-0x400000),   // EP 0
        // MemoryPatchRva(0x9000518B,    4,  0x0086F297-0x400000),   // HP SHORT-INT
        // MemoryPatchRva(0x9000518B,    4,  0x0088C0C3-0x400000),   // HP SHORT-INT
        // MemoryPatchRva(0x1,           1,  0x006C1BA3-0x400000),   // EP add

        // MemoryPatchRva(0x9BF4,        2,  0x00A27874-0x400000),   // EXP %4d-%5d
        // MemoryPatchRva(0x70,          1,  0x00A278E3-0x400000),   // EXP pos 76-70

        MemoryPatchRva(9000,          4,  0x0090EC1E-0x400000),   // se max 0x1068 -> 0x2328
        MemoryPatchRva(9000,          4,  0x0090E6B6-0x400000),   // se max 0x1068 -> 0x2328
        MemoryPatchRva(9000,          4,  0x0090EE83-0x400000),   // se max 0x1068 -> 0x2328
        MemoryPatchRva(9000,          4,  0x0090F5F6-0x400000),   // se max 0x1068 -> 0x2328

#if 1
        FunctionJumpRva(0x27969D, (&CBattle::SetSelectedAttack)),
        FunctionJumpRva(0x275DF4, (&CBattle::SetSelectedCraft)),
        FunctionJumpRva(0x272AB9, (&CBattle::SetSelectedSCraft)),
        //FunctionJumpRva(0x279553, (&CBattle::SetSelectedMagic)),
#endif
/*
        FunctionCallRva(0x5DE1D9, (&CBattle::NakedNoResistConditionUp)),
        // bug fix

        FunctionCallRva(0x5B1BE6, (&CBattleATBar::LookupReplaceAtBarEntry)),
        FunctionJumpRva     (0x275DAE, (&CBattle::ExecuteActionScript), CBattle::StubExecuteActionScript),

        FunctionJumpRva     (0x550C90, (&CScript::ScpSaveRestoreParty), CScript::StubScpSaveRestoreParty),

        // hack for boss

        FunctionCallRva(0x5DFA21, NakedGetUnderAttackVoiceChrIdRestore),
        FunctionCallRva(0x5DFA1B, (&CBattle::NakedGetUnderAttackVoiceChrId)),  // boss挨打语音修复
*/
        FunctionCallRva(0x5A3814, (&CBattle::NakedOverWriteBattleStatusWithChrStatus)),

        FunctionJumpRva     (0x275061, (&CBattle::IsChrCanTeamRush), &CBattle::StubIsChrCanTeamRush),

        // monster info box

        //FunctionCallRva(0x626AEA, (&CBattleInfoBox::SetMonsterInfoBoxSize)),
        //FunctionJumpRva(0x27AC8C, (PVOID)0xA26D50),
        //FunctionJumpRva     (0x27AC8C, (&CBattleInfoBox::DrawMonsterStatus), CBattleInfoBox::StubDrawMonsterStatus), // USHORT->SHORT

        // bug fix
        FunctionCallRva(0x2A2E2C, FurnitureCompletionRate),
        FunctionJumpRva(0x498F50, (PVOID)0x898DD1),   // 装卸主回路无法出成就 七耀之贤士
        FunctionCallRva(0x3616A6, MedalReturn),
        FunctionCallRva(0x520F63, TitleVisualCountSaveFix),


        FunctionJumpRva     (0x277280, (&CScript::ScpLeaveParty), &CScript::StubScpLeaveParty),
        FunctionCallRva(0x599489, DecBurstEnergyWhenStandby),

        FunctionCallRva(0x597C61, (&CBattle::NakedCheckAliveBeforeHeal)),

        FunctionCallRva(0x5E3ED6, (&CBattle::SubHPEveryAct2WhenAttack)),
        FunctionCallRva(0x5837EE, (&CBattle::NakedHandleConditionBeforeMasterQuartzKipaTakeEffect)),
        FunctionCallRva(0x5838EA, (&CBattle::IsNeedBattleEvaluationSuperKill)),

        FunctionCallRva(0x5B3FD2, (&CBattle::GetHitResult)),    // 灵猫多次魔反仅吸收第一次fix

        FunctionCallRva(0x6A4244, (&CClass::HorrorCoasterFastExit)),
        //FunctionJumpRva     (0x273888, (&CClass::HorrorCoasterEvaluationPositionRestore), CClass::StubHorrorCoasterEvaluationPositionRestore),
        FunctionCallRva(0x6A58FF, (&CClass::PositionPC2PSP)),   // 鬼屋评价显示位置修正
        FunctionCallRva(0x36419B, Return2ExtraFix),

        FunctionJumpRva(0x279986, (&CSSaveData::SaveSystemData)),
        FunctionJumpRva(0x279FA8, (&CSSaveData::LoadSystemData)),

        //TODO: integrate FreeType
        //FunctionJumpRva(0x276402, NFreeType::FT_New_Face_Ex),     // load font once
        FunctionJumpRva     (0x2798F0, (&EDAO::CalcChrT_Status), &EDAO::StubCalcChrT_Status),
        FunctionJumpRva     (0x275FF7, (&EDAO::CalcChrRawStatusFromLevelNew), &EDAO::StubCalcChrRawStatusFromLevel),
        FunctionJumpRva     (0x273473, (&EDAO::CalcChrRawStatusByFinalStatus), &EDAO::StubCalcChrRawStatusByFinalStatus),

        //
        FunctionJumpRva     (0x2756E7, (&CBattle::SetBattleStatusFinalByDifficulty), &CBattle::StubSetBattleStatusFinalByDifficulty),
        FunctionJumpRva     (0x274E40, (&EDAO::GetDifficulty), &EDAO::StubGetDifficulty),
        //TODO: Fix container_of
        //FunctionCallRva(0x5AA772, (&EDAO::SetBattleStatusFinalWhenRecover)),    // 木偶恢复能力值
        FunctionJumpRva     (0x278F68, CalcSpecificStatusByDifficulty, &StubCalcSpecificStatusByDifficulty),

        FunctionJumpRva     (0x275471, (&CGlobal::AddCraft), &CGlobal::StubAddCraft), // 银 不能习得 月舞
        FunctionJumpRva(0x277A55, (&CGlobal::UseItemDouguFix)),                 // 非战斗使用物品无效果3

        // fish
        FunctionCallRva(0x675BC2, (&CFish::IsRodPulled)),
        FunctionCallRva(0x67973B, (&CFish::GetRodEntry)),

        // debug fix
        //FunctionCallRva(0x480915, (&EDAO::ShowDebugTextPositionRestore1)),
        //FunctionCallRva(0x48094D, (&EDAO::ShowDebugTextPositionRestore2)),
        FunctionCallRva(0x4806D3, (&CClass::PositionPC2PSP)),   // F6  #%03d : %s(lv%d)
        FunctionCallRva(0x480A1E, (&CClass::PositionPC2PSP)),   // F6 ChrName
        FunctionCallRva(0x47E2CA, (&CClass::PositionPC2PSP)),   // F6  ( %02d ) : %s [%d->%d],%d
        FunctionCallRva(0x5DB5E1, (&CClass::PositionPC2PSP)),   // F6 In Battle
        //FunctionCallRva(0x47E46D, (&EDAO::ShowDebugTextPositionRestore1)),
        //FunctionCallRva(0x47E4AB, (&EDAO::ShowDebugTextPositionRestore2)),

        // 变青椒后只能普通攻击
        FunctionCallRva(0x58E76B, (&CBattle::CheckConditionGreenPepperWhenThinkCraft)),     // ThinkSCraft
        FunctionCallRva(0x58E4DB, (&CBattle::CheckConditionGreenPepperWhenThinkCraft)),     // ThinkCraft
        FunctionCallRva(0x58E24B, (&CBattle::CheckConditionGreenPepperWhenThinkCraft)),     // ThinkMagic

        FunctionCallRva(0x5E2A8A, NakedConfuseSelfBugFix),    // 混乱自己卡住
        FunctionCallRva(0x5DE3AA, NakedMovUpMaxFix),          // MOV UP/DOWN MAX 5
        FunctionCallRva(0x5DE3C0, NakedMovDownMaxFix),

#if CONSOLE_DEBUG
        // log
        //FunctionJumpRva     (0x51EB50, (&CScript::ScpGetFunctionAddress), CScript::StubScpGetFunctionAddress),
        //FunctionJumpVa     (0x006729D8, (&CClass::ShowHorrorCoasterText), CClass::StubShowHorrorCoasterText),
#endif
    };

#if CONSOLE_DEBUG
    AllocConsole();
    PrintConsoleW(L"%x\r\n", sizeof(MONSTER_STATUS));
    PrintConsoleW(L"%x\r\n", sizeof(ITEM_ENTRY));
    PrintConsoleW(L"%x\r\n", sizeof(CArtsNameWindow));
    PrintConsoleW(L"%x\r\n", FIELD_OFFSET(MONSTER_STATUS, AT));
    PrintConsoleW(L"%x\r\n", FIELD_OFFSET(MONSTER_STATUS, Resistance));
    //PrintConsoleW(L"%x\r\n", FIELD_OFFSET(MONSTER_STATUS, SummonCount));
    PrintConsoleW(L"%x\r\n", FIELD_OFFSET(AT_BAR_ENTRY, IconAT));
    PrintConsoleW(L"%x\r\n", FIELD_OFFSET(AT_BAR_ENTRY, Pri));
    PrintConsoleW(L"%x\r\n", FIELD_OFFSET(CSSaveData, ChrStatus));
    Test();
#endif

    PatchMemory(p, countof(p), GetExeModuleHandle());

    ConfigInit();
    LoadStatusConfig();

    if (bArianrhodLimitKaijo)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0xEB, 1, 0x5A3E40),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    if (!bEnemySBreak)
    {
        static unsigned char p0096526F[6] = {0x89, 0x8D, 0x6C, 0xFD, 0xFF, 0xFF};
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(p0096526F, 6, 0x56526F),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    if (!bShowAT)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x4F74,    2, 0x5F668D),    // disable orig at
            MemoryPatchRva(0x85,      1, 0x5F66D9),    // disable orig at
            MemoryPatchRva(0x84,      1, 0x5F68A4),    // force show debug at
            MemoryPatchRva(0x1A,      1, 0x5F693D),    // debug at pos.X advance

            FunctionCallRva(0x5F690B, (PVOID)0x6748DC),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }
    else
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x4F74,    2, 0x5F668D),   // disable orig at
            MemoryPatchRva(0x85,      1, 0x5F66D9),   // disable orig at
            MemoryPatchRva(0x6E,      1, 0x5F68D7),   // Pri->Sequence

            FunctionCallRva(0x5F690B, CBattle::FormatBattleChrAT),
            FunctionJumpRva(0x5F66DE, NakedConditionalShowOrigAT),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    if (!bForceKillEnemyAtScene)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x9D,      1, 0x2F9EE3),    // one hit
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    if (!bForceShowBurstMode)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x0D,  1,  0x55F6E1),        // 爆灵
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    if (!bForceShowInfOnMiniMap)
    {
        PATCH_MEMORY_DATA p[] =
        {
            FunctionJumpRva(0x279AA3, (PVOID)0x740F50),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    if (!bEnableSkipCraftAnime)
    {
        PATCH_MEMORY_DATA p[] =
        {
            FunctionCallRva(0x5B05C6, (PVOID)0x678AF4),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 特效关闭
    if (!bEnableSpecialEffect)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x7E,  1,  0x2CAA98),      // enable shimmer when width > 1024
            MemoryPatchRva(0x7E,  1,  0x2C33BE),      // enable blur when width > 1024
            MemoryPatchRva(0x7E,  1,  0x2EFBB8),      // capture ?
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 遇敌方式，开场状况
    if (nBattleEncount != 3)
    {
        if (FLAG_ON(nBattleEncount, 0x10))
        {
            PATCH_MEMORY_DATA p[] =
            {
                MemoryPatchRva(0x00ull,		1,	0x006FBCAD-0x400000),	// 阳炎
                MemoryPatchRva(0xEB,		1,	0x00750B34-0x400000),	// 宝箱靠近怪 无法开启
            };
            PatchMemory(p, countof(p), GetExeModuleHandle());
        }

        if (FLAG_ON(nBattleEncount, 0x20))
        {
            PATCH_MEMORY_DATA p[] =
            {
                MemoryPatchRva(0x00ull,		1,	0x006F5781-0x400000),	// 叶隐
            };
            PatchMemory(p, countof(p), GetExeModuleHandle());
        }

        CLEAR_FLAG(nBattleEncount, 0xFFFFFFF8);
        if (nBattleEncount <= 4 && nBattleEncount != 3)
        {
            PATCH_MEMORY_DATA p[] =
            {
                FunctionCallRva(0x2F6700, SetBattleEncountCondition),
            };
            PatchMemory(p, countof(p), GetExeModuleHandle());
        }
    }

    // 支援战技AT
    if (nSupportCraftInterval != 0)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(nSupportCraftInterval,		    4,	0x00725AF6-0x400000),
            MemoryPatchRva(nSupportCraftInterval,		    4,	0x0099EFE7-0x400000),
            MemoryPatchRva(nSupportCraftInterval/7*3,		4,	0x0099FB75-0x400000),
            MemoryPatchRva(nSupportCraftInterval/7*4 + 1, 4,	0x0099FB7B-0x400000),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 伤害计算去随机补正
    if (bDisableDamageRandom)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x81,  1,	0x00976162-0x400000),
            MemoryPatchRva(0xEB,  1,	0x009B6A54-0x400000),   // MasterArt
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 伤害上限解除
    if (bDisableDamageUpLimit)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0xEB,  1,	0x009763AE-0x400000),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // ATBonus设置
    if (nATBonus != 0)
    {
        if (nATBonus == 16)
            nATBonus = 0;

        PATCH_MEMORY_DATA p[] =
        {
            FunctionCallRva(0x5F9562, SetBattleATBonus),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 晶片上限
    if (nSepithUpLimit != 0)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(nSepithUpLimit,		    4,	0x00981262 -0x400000),
            MemoryPatchRva(nSepithUpLimit,		    4,	0x0098127E -0x400000),

            //FunctionCallRva(0x61F452, SepithUpLimitDisplay),
            //FunctionCallRva(0x61F474, SepithUpLimitDisplay),
            //FunctionCallRva(0x620981, SepithUpLimitDisplay),
            //FunctionCallRva(0x62099A, SepithUpLimitDisplay),
            FunctionCallRva(0x620E27, SepithUpLimitDisplay1),
            FunctionCallRva(0x620E8C, SepithUpLimitDisplay2),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 自动解析魔兽信息
    if (bAutoAnalyzeMonsInf)
    {
        PATCH_MEMORY_DATA p[] =
        {
            FunctionJumpRva    (0x274238, (&CBattle::CheckQuartz), &CBattle::StubCheckQuartz),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 可以让主角四人组离队
    if (bSelectTeamMemberFreely)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0xEB,  1,  0x50079D),
            MemoryPatchRva(0x00ull,  1,  0x5007D8),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // Fish
    {
        if (nFastFish)
        {
            PATCH_MEMORY_DATA p[] =
            {
                FunctionCallRva(0x675883, (&CFish::ChangeFishingWaitTime)),
                FunctionCallRva(0x6758A5, (&CFish::ChangeFishingWaitTime)),
            };
            PatchMemory(p, countof(p), GetExeModuleHandle());
        }
    }

    // 亚里欧斯等特殊人员装备回路解锁
    if (bSpecialTeamMemberEquipmentLimitKaijo)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x63EB,  2,  0x4E1E55),    // quartz
            MemoryPatchRva(0x35EB,  2,  0x4C7AC2),    // mQuartz display
            MemoryPatchRva(0x6AEB,  2,  0x49A278),    // equip
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 组队
    if (bEnableSelectTeamMember)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x00ull,  1,  0x48740C),       // count

            FunctionJumpRva(0x273BDF, (&CDebug::MainControl), &CDebug::StubMainControl),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // Poker
    if (bRoyalFlush)
    {
        PATCH_MEMORY_DATA p[] =
        {
            FunctionJumpRva(0x27785C, (&CClass::PokerRoyalFlush)),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // 自动 恐怖过山车
    if (bAutoHorrorCoaster)
    {
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x11EB,    2,  0x6AAB1A),
            MemoryPatchRva(0x00ull,      1,  0x6AABC6),
        };
        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    // disable mouse
    if (bDisableMouse)
    {
        //ShowCursor(FALSE);
        PATCH_MEMORY_DATA p[] =
        {
            MemoryPatchRva(0x00ull,      1,  0x410C60),  // disable key
            MemoryPatchRva(0x48C033,  3,  0x406511),  // disable cursor move
            MemoryPatchRva(0x48C033,  3,  0x40651C),  // disable cursor move
        };

        PatchMemory(p, countof(p), GetExeModuleHandle());
    }

    return TRUE;
}

// BOOL WINAPI DllMain(PVOID BaseAddress, ULONG Reason, PVOID Reserved)
// {
//     //PrintConsoleW(L"%d", FIELD_OFFSET(MONSTER_STATUS, SummonCount));

//     switch (Reason)
//     {
//         case DLL_PROCESS_ATTACH:
//             return Initialize(BaseAddress) || UnInitialize(BaseAddress);

//         case DLL_PROCESS_DETACH:
//             UnInitialize(BaseAddress);
//             break;
//     }

//     return TRUE;
// }

