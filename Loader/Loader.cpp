#include "Interpreter.h"

/*
* ⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️⚠️
* 1.常规: 平台工具集(LLVM (clang-cl))
* 2.C/C++
* 常规: 附加包含目录($(ProjectDir)json-3.11.3)
*/

int main() {
    // 读取 Payload
    HANDLE hFile = CreateFileA("..\\Converter\\Payload.bin", GENERIC_READ, NULL, NULL, OPEN_EXISTING, 0, NULL);
    DWORD payloadLength = GetFileSize(hFile, NULL);
    PVOID pPayload = malloc(payloadLength);
    DWORD readFileSize;
    ReadFile(hFile, pPayload, payloadLength, &readFileSize, NULL);    

    // 解析 Payload
    BofPayload bofPayload;
    try {
        ParsePayload((char*)pPayload, payloadLength, bofPayload);
    }
    catch (exception& e) {
        cout << e.what() << endl;
        return 0;
    }
    catch (...) {
        return 0;
    }
    free(pPayload);

    while (true) {
        // BOF 命令执行
        char* commandPara = "cmd /c tasklist";
        int commandParaLength = strlen(commandPara) + 1;
        char* outputData;
        int outputDataLength = 0;
        PVOID specialParaList[] = { NULL };
        try {
            MagicInvoke("ExecuteCmd$$", commandPara, commandParaLength, outputData, outputDataLength, specialParaList, bofPayload);
        }
        catch (exception& e) {
            cout << e.what() << endl;
            return 0;
        }
        catch (...) {
            return 0;
        }
        if (outputDataLength > 0) {
            *(outputData + outputDataLength) = '\0';
            cout << outputData << endl;
        }

        // BOF 获取文件信息列表
        commandPara = "C:\\Windows\\System32\\*";
        commandParaLength = strlen(commandPara) + 1;
        try {
            MagicInvoke("GetFileInfoList$$", commandPara, commandParaLength, outputData, outputDataLength, specialParaList, bofPayload);
        }
        catch (exception& e) {
            cout << e.what() << endl;
            return 0;
        }
        catch (...) {
            return 0;
        }
        if (outputDataLength > 0) {
            *(outputData + outputDataLength) = '\0';
            cout << outputData << endl;
        }
        getchar();
    }
}
