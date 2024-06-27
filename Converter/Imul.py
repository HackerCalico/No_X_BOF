import re

def ProcessImul(asm):
    newAsm = ''
    for line in asm.split('\n'):
        if 'imul' in line:
            imul = re.findall(r'imul_(.+), (.+),.+', line)[0]
            if imul[0] == imul[1]:
                line = line.replace(imul[0] + ', ' + imul[1], imul[0])
        newAsm += line + '\n'
    return newAsm