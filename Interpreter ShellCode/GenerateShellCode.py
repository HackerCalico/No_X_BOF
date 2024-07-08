import os
import subprocess

if __name__ == '__main__':
    with open('ToolsPath.txt', 'r') as file:
        toolsPath = file.read().split('\n')

    interpreterPath = os.getcwd() + '\\' + toolsPath[0]
    generateShellCodePath = os.getcwd() + '\\' + toolsPath[1]

    subprocess.call([generateShellCodePath, interpreterPath])