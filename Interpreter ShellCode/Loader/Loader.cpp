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
    char selfAsm[] = "0_4_q_pq70+i20_q_q38_5_4_q_pq70+i18_q_q30_a_4_d_pq70+i10_d_d18_e_4_q_pq70+i8_q_q10_13_0_q_q8_q__14_0_q_q28_q__15_11_q_q70_q_i228_1c_4_d_d0_d_i8_21_13_q_q0_q_i0_25_4_d_d10_d_i14a_2a_4_q_q18_q_pq70+i260_32_2_q_pq18+q0_q__35_4_q_q10_q_pq70+i250_3d_4_q_pq10_q_q0_40_4_q_q0_q_pq70+i250_48_4_q_q0_q_pq0_4b_4_b_pq0_b_i30_4e_4_q_q0_q_pq70+i258_56_4_d_pq0_d_i1_5c_4_d_d0_d_i8_61_13_q_q0_q_ib_65_8_q_q18_q_lq70+ie0_6d_4_q_q10_q_pq70+i240_75_4_q_q30_q_pq70+i260_7d_2_q_pq30+q0_q__81_4_q_pq70+i98_q_q0_89_12_q_pq70+i98_q_i-1_92_22_q_i427_q__98_4_d_d0_d_i8_9d_13_q_q0_q_ie_a1_16_d_d38_d_d38_a4_4_q_q10_q_pq70+i250_ac_4_q_q30_q_pq10_af_4_d_d18_d_i104_b4_4_q_q10_q_pq70+i240_bc_4_q_q40_q_pq70+i260_c4_2_q_pq40+q0_q__c8_17_d_d0_d_d0_ca_23_q_ied_q__cc_4_d_d0_d_i8_d1_13_q_q0_q_id_d5_4_q_q10_q_pq70+i98_dd_4_q_q18_q_pq70+i260_e5_2_q_pq18+q0_q__e8_21_q_i450_q__ed_4_d_d0_d_i1_f2_13_q_q0_q_i0_f6_5_d_d0_b_pq70+q0+i10c_fe_12_d_d0_d_i2e_101_22_q_i3fb_q__107_4_d_d0_d_i8_10c_13_q_q0_q_if_110_8_q_q18_q_lq70+ib8_118_8_q_q10_q_lq70+if4_120_4_q_q30_q_pq70+i260_128_2_q_pq30+q0_q__12c_17_d_d0_d_d0_12e_23_q_i151_q__130_4_d_d0_d_i8_135_13_q_q0_q_id_139_4_q_q10_q_pq70+i98_141_4_q_q18_q_pq70+i260_149_2_q_pq18+q0_q__14c_21_q_i427_q__151_4_b_pq70+i60_b_ia_156_4_b_pq70+i61_b_i25_15b_4_b_pq70+i62_b_i64_160_4_b_pq70+i63_b_i2c_165_4_b_pq70+i64_b_i25_16a_4_b_pq70+i65_b_i73_16f_4_b_pq70+i66_b_i2c_174_4_b_pq70+i67_b_i25_179_4_b_pq70+i68_b_i6c_17e_4_b_pq70+i69_b_i6c_183_4_b_pq70+i6a_b_i75_188_4_b_pq70+i6b_b_i2c_18d_4_b_pq70+i6c_b_i25_192_4_b_pq70+i6d_b_i30_197_4_b_pq70+i6e_b_i34_19c_4_b_pq70+i6f_b_i64_1a1_4_b_pq70+i70_b_i2e_1a6_4_b_pq70+i71_b_i25_1ab_4_b_pq70+i72_b_i30_1b0_4_b_pq70+i73_b_i32_1b5_4_b_pq70+i74_b_i64_1ba_4_b_pq70+i75_b_i2e_1bf_4_b_pq70+i76_b_i25_1c4_4_b_pq70+i77_b_i30_1c9_4_b_pq70+i78_b_i32_1ce_4_b_pq70+i79_b_i64_1d3_4_b_pq70+i7a_b_i20_1d8_4_b_pq70+i7b_b_i25_1dd_4_b_pq70+i7c_b_i30_1e2_4_b_pq70+i7d_b_i32_1e7_4_b_pq70+i7e_b_i64_1ec_4_b_pq70+i7f_b_i3a_1f1_4_b_pq70+i80_b_i25_1f9_4_b_pq70+i81_b_i30_201_4_b_pq70+i82_b_i32_209_4_b_pq70+i83_b_i64_211_4_b_pq70+i84_b_i3a_219_4_b_pq70+i85_b_i25_221_4_b_pq70+i86_b_i30_229_4_b_pq70+i87_b_i32_231_4_b_pq70+i88_b_i64_239_4_b_pq70+i89_b_i0_241_4_d_d0_d_pq70+ie0_248_14_d_d0_d_i10_24b_17_d_d0_d_d0_24d_22_q_i25c_q__24f_4_d_pq70+i90_d_i1_25a_21_q_i267_q__25c_4_d_pq70+i90_d_i0_267_6_d_d0_w_pq70+ic4_26f_4_d_pq70+ia0_d_d0_276_6_d_d10_w_pq70+ic2_27e_4_d_pq70+ia4_d_d10_285_6_d_d18_w_pq70+ic0_28d_4_d_pq70+ia8_d_d18_294_6_d_d30_w_pq70+ibe_29d_4_d_pq70+iac_d_d30_2a5_6_d_d38_w_pq70+iba_2ae_4_d_pq70+ib0_d_d38_2b6_6_d_d40_w_pq70+ib8_2bf_4_d_pq70+ib4_d_d40_2c7_4_d_d48_d_pq70+ifc_2cf_18_q_q48_q_i20_2d3_4_d_d8_d_pq70+i100_2da_15_q_q48_q_q8_2dd_4_q_q8_q_q48_2e0_4_d_d48_d_i8_2e6_13_q_q48_q_i3_2ea_4_q_q28_q_pq70+i250_2f2_4_q_q10_q_pq28_2f5_4_q_q28_q_pq70+i260_2fd_2_q_pq28+q48_q__301_4_q_q10_q_pq70+i250_309_9_q_q0_q_pq10_30c_4_d_d10_d_i8_311_13_q_q10_q_i6_315_4_q_pq70+ic8_q_q10_31d_4_d_d18_d_pq70+ia0_324_4_d_pq70+i58_d_d18_328_4_d_d18_d_pq70+ia4_32f_4_d_pq70+i50_d_d18_333_4_d_d18_d_pq70+ia8_33a_4_d_pq70+i48_d_d18_33e_4_d_d18_d_pq70+iac_345_4_d_pq70+i40_d_d18_349_4_d_d18_d_pq70+ib0_350_4_d_pq70+i38_d_d18_354_4_d_d18_d_pq70+ib4_35b_4_d_pq70+i30_d_d18_35f_4_q_pq70+i28_q_q8_364_8_q_q18_q_lq70+i10c_36c_4_q_pq70+i20_q_q18_371_4_d_d38_d_pq70+i90_379_8_q_q30_q_lq70+i60_37e_4_d_d18_d_i14a_383_4_q_q10_q_q0_386_4_q_q0_q_pq70+i260_38e_4_q_q8_q_pq70+ic8_396_2_q_pq0+q8_q__399_4_d_d0_d_i8_39e_13_q_q0_q_i3_3a2_4_q_q10_q_pq70+i250_3aa_4_q_q10_q_pq10_3ad_4_q_q18_q_pq70+i260_3b5_2_q_pq18+q0_q__3b8_9_q_q0_q_i14a_3be_4_d_d10_d_i8_3c3_13_q_q10_q_i1_3c7_4_q_pq70+id0_q_q10_3cf_4_q_q18_q_q0_3d2_4_q_q0_q_pq70+i250_3da_4_q_q10_q_pq0_3dd_4_q_q0_q_pq70+i260_3e5_4_q_q8_q_pq70+id0_3ed_2_q_pq0+q8_q__3f0_4_q_q10_q_pq70+i250_3f8_4_q_pq10_q_q0_3fb_4_d_d0_d_i8_400_13_q_q0_q_ic_404_8_q_q18_q_lq70+ie0_40c_4_q_q10_q_pq70+i98_414_4_q_q8_q_pq70+i260_41c_2_q_pq8+q0_q__41f_17_d_d0_d_d0_421_23_q_ied_q__427_4_d_d0_d_i8_42c_13_q_q0_q_i3_430_4_q_q10_q_pq70+i250_438_4_q_q10_q_pq10_43b_4_q_q18_q_pq70+i260_443_2_q_pq18+q0_q__446_4_q_q10_q_pq70+i258_44e_4_d_pq10_d_d0_450_9_q_q70_q_i228_457_1_q_q28_q__458_1_q_q8_q__459_3_q__q__!";

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
