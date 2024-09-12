# No_X_BOF

### 请给我 Star 🌟，非常感谢！这对我很重要！

### Please give me Star 🌟, thank you very much! It is very important to me!

### 1. 介绍

https://github.com/HackerCalico/No_X_BOF-ShellCode

无需解密，无需 X 内存，通过解释器直接加载运行 R 内存中的 BOF。

项目原本为 No_X_Memory_ShellCode_Loader，已将其移入分支。

规避了以下特征：

(1) 申请 RWX 内存。

(2) 来回修改 W 和 X 的内存属性。

(3) 内存中出现 BOF 特征码。

![1.png](https://raw.githubusercontent.com/HackerCalico/No_X_BOF-ShellCode/main/run.png)

### 2. 使用方法

(1) 使用 GCC 生成 BOF 文件。

本项目采用 https://github.com/mstorsjo/llvm-mingw/releases/download/20240903/llvm-mingw-20240903-ucrt-x86_64.zip 中的 GCC。

将 llvm-mingw-20240903-ucrt-x86_64\bin 添加至环境变量后，运行 BOF\Compile 编译.bat 即可生成 bof.o。

(2) BOF --- 转换器 ---> 自定义汇编指令。

```baseh
pip install capstone
```

先将 BOF 反汇编，生成信息文件至 Disassembly 文件夹。

```bash
> python Converter.py
1.反汇编
2.生成 Payload
3.生成未混淆 Payload (实验)
选择: 1
BOF 使用的汇编指令: {'ret', 'push', 'pop', 'jne', 'shl', 'je', 'shr', 'mov', 'sub', 'nop', 'movsxd', 'movzx', 'lea', 'and', 'test', 'add', 'cmp', 'xor', 'or', 'call', 'movabs', 'jmp'}
已将汇编中的 imul a, a, b 全部替换为 imul a, b
反汇编结果生成至文件夹: Disassembly
```

再通过信息文件生成 Payload.bin，包含 BOF 加载信息、自定义汇编。

```bash
> python Converter.py
1.反汇编
2.生成 Payload
3.生成未混淆 Payload (实验)
选择: 2
请手动在 Instruction.cpp 的 InvokeInstruction 函数开头定义:
int jmpIndex = 24;
PVOID mnemonicMap[] = { Push, Pop, Call, Ret, Mov, Movzx, Movabs, Movsxd, Lea, Nop, Add, Inc, Sub, Cmp, Imul, And, Or, Xor, Test, Shl, Shr, Cdqe, RepStosb, RepMovsb, Jmp, Je, Jne, Jbe, Jl, Jge, Jle };
Payload (BOF 函数偏移 + .text 重定位名称 + 混淆映射 + 自定义汇编 + .rdata) 生成至: Payload.bin
```

(3) 解释器 加载运行 Payload.bin。

运行 Loader\Loader.cpp 即可。

需要配置 LLVM (clang-cl) 以支持 x64 内联汇编:

Visual Studio Installer ---> 单个组件 ---> LLVM (clang-cl) 和 Clang ---> 安装

### 3. 代码实现

最近比较忙，所以不能及时写。

首先你要懂 BOF 加载原理，之后代码实现类似 No_X_Memory_ShellCode_Loader，可以先结合 https://github.com/HackerCalico/No_X_BOF-ShellCode/tree/No_X_Memory_ShellCode_Loader 研究一下。
