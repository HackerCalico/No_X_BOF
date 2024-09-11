import re

def SimplifyImul(asm):
    simpleAsm = ''
    for line in asm.split('\n'):
        if 'imul' in line:
            imul = re.findall(r'imul_(.+), (.+),.+', line)[0]
            if imul[0] == imul[1]:
                line = line.replace(imul[0] + ', ' + imul[1], imul[0])
        simpleAsm += line + '\n'
    print('已将汇编中的 imul a, a, b 全部替换为 imul a, b')
    return simpleAsm