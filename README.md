# No_X_BOF

### 请给我 Star 🌟，非常感谢！这对我很重要！

### Please give me Star 🌟, thank you very much! It is very important to me!

### 1. 介绍

https://github.com/HackerCalico/No_X_BOF

Magical BOF 加载器，加载 BOF 无需可执行权限内存。

项目原本为 No X ShellCode，已将其移入分支: https://github.com/HackerCalico/No_X_BOF/tree/No_X_Memory_ShellCode_Loader

原理是通过 "汇编解释器" 运行 BOF 汇编指令，规避了以下特征：

(1) 申请 RWX 内存。

(2) 来回修改 W 和 X 的内存属性。

(3) 内存中出现 BOF 特征码。

由于项目的机器码本身可能成为特征，于是我开发了另一个项目 RAT Obfuscator 来进行保护，它支持混淆 ShellCode 以及 EXE 中的函数机器码。

### 2. 使用方法

<mark>请先尝试项目提供的 BOF 样例：BOF\bof.o</mark>

(1) 生成 BOF

bof.c 中定义了两个可以调用的 BOF 函数: ExecuteCmd$$、GetFileInfoList$$，以及一个内部函数: TestCall$$。GetFileInfoList$$ 会调用 TestCall$$，过去的版本并不支持自定义函数之间存在调用链。

项目使用 https://github.com/mstorsjo/llvm-mingw/releases/download/20240903/llvm-mingw-20240903-ucrt-x86_64.zip 中的 clang 进行编译。

将 llvm-mingw-20240903-ucrt-x86_64\bin 添加至环境变量后，运行 BOF\Compile 编译.bat 即可生成 bof.o。

(2) 将 BOF 转为 "解释器" 可运行的 Payload

```bash
pip install capstone
```

将 BOF 反汇编，生成 BOF 加载的必要信息至 Disassembly 文件夹。必要信息包含 .text 的汇编指令、.rdata 原数据、重定位名称、BOF 函数偏移。

```bash
> python Converter.py
1.反汇编
2.生成 Payload
Choice: 1
[!] 请手动对 Disassembly\asm.txt 中调用 TestCall$$ 的 call [地址] 进行重定位
[!] 请手动对 Disassembly\asm.txt 中调用 TestCall$$ 的 call [地址] 进行重定位
b'ExecuteCmd$$' Hash: -504283653
b'TestCall$$' Hash: -953508912
b'GetFileInfoList$$' Hash: 1280936002
BOF 使用的汇编指令: {'jmp', ..., 'je'}
[+] 反汇编结果生成至文件夹 Disassembly
```

可以看到提示了要手动对 TestCall$$ 的 call 进行重定位，打开 Disassembly\asm.txt。

```bash
0x264_ret_
0x265_nop_word ptr cs:[rax + rax]
0x270_push_rsi
....
0x29b_call_0x2a0
....
0x306_call_0x30b
```

结合 bof.c 可以很容易判断出 TestCall$$ 的虚拟地址是 0x270，而 call_0x2a0 和 call_0x30b 都是调用 TestCall$$，所以手动把它们都改成 call_0x270。

下一步就是通过这些必要信息生成 Payload。

```bash
> python Converter.py
1.反汇编
2.生成 Payload
Choice: 2
请手动在 Instruction.cpp 的 InvokeInstruction 函数开头定义:
const int pushIndex = 0, ..., jleIndex = 27;
if (mnemonicIndex < 0 || mnemonicIndex > 27) {
return 2;
}
[+] Payload 生成至 Payload.bin
```

该过程会把 "原汇编指令" 转为 "自定义指令"， 再把这些必要信息拼接为 Payload。

"自定义指令" 在格式上比 "原汇编指令" 更容易解析，可以减轻 "解释器" 的压力。

(3) 通过 "解释器" 运行 Payload，调用 BOF 函数

运行 Loader\Loader.cpp 即可。

需要配置 LLVM (clang-cl) 以支持 x64 内联汇编:

Visual Studio Installer ---> 单个组件 ---> LLVM (clang-cl) 和 Clang ---> 安装

### 3. 功能实现

<mark>具体实现请看代码，下面对重点简要概括：</mark>

(1) "自定义指令" 格式

```bash
原指令: 0x2c lea r9, [rip + 0x03]
自定义指令: 0x2c 0x0D qq38 qlr0000000000000000++i3
```

0x2c 为虚拟地址，在处理跳转时起作用。

0x0D 为 lea 在指令列表中的下标。

q 表示 "操作数1" 是 QWORD 类型，q38 表示 R9 寄存器。在 "解释器" 中通过 vtRegs 数组存储虚拟寄存器的值，而 R9 相对 vtRegs 的偏移是 0x38。

q 表示 "操作数2" 是 QWORD 类型，l 表示该操作是 lea 的第二个操作数，r0000000000000000 表示 RIP，++ 表示 + (便于运算，详见源码)，i3 表示立即数 0x03。

BOF 中的 RIP 存在特殊情况，lea r9, [rip + 0x3] 中 "rip" 的实际含义并不是寄存器 RIP，BOF 加载过程中对 .text 的重定位其实就是修改这些 "rip" 的值。

所以 "rip" 这里不用相对 vtRegs 的偏移，而是预留 16 位。重定位后会被替换为 "i地址" (导入的 DLL 函数的指针或 .rdata 地址)。

(2) "解释器" 处理流程

1. 解析 Payload，获取 Payload 中的 "自定义指令"、.rdata 原数据、重定位名称、BOF 函数偏移信息。

2. 查找 BOF 函数虚拟地址，通过 BOF 函数 Hash 对照 "BOF 函数偏移信息"，找到 BOF 函数在 "自定义指令" 中的虚拟地址。

3. 创建虚拟栈、创建虚拟寄存器、初始化虚拟寄存器 (将 BOF 函数的参数存入虚拟寄存器)。

4. 从 BOF 函数虚拟地址对应的 "自定义指令" 开始逐条运行。
