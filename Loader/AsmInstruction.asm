; 源码: GenerateAsmInstruction\Instruction

.CODE
    AsmCall PROC
    push rbp
    push r15
    push r14
    push r13
    push r12
    push rsi
    push rdi
    push rbx
    sub rsp, 0b0h
    lea rbp, [rsp + 080h]
    mov qword ptr [rbp + 028h], rdx
    mov qword ptr [rbp + 020h], rcx
    mov qword ptr [rbp + 018h], rsp
    mov qword ptr [rbp + 010h], rbp
    mov rax, qword ptr [rbp + 020h]
    mov rax, qword ptr [rax]
    mov qword ptr [rbp + 8], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax]
    mov qword ptr [rbp], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 8]
    mov qword ptr [rbp - 8], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 010h]
    mov qword ptr [rbp - 010h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 018h]
    mov qword ptr [rbp - 018h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 020h]
    mov qword ptr [rbp - 020h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 028h]
    mov qword ptr [rbp - 028h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 030h]
    mov qword ptr [rbp - 030h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 038h]
    mov qword ptr [rbp - 038h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 040h]
    mov qword ptr [rbp - 040h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 048h]
    mov qword ptr [rbp - 048h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 050h]
    mov qword ptr [rbp - 050h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 058h]
    mov qword ptr [rbp - 058h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 060h]
    mov qword ptr [rbp - 060h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 068h]
    mov qword ptr [rbp - 068h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 070h]
    mov qword ptr [rbp - 070h], rax
    mov rax, qword ptr [rbp + 028h]
    mov rax, qword ptr [rax + 078h]
    mov qword ptr [rbp - 078h], rax
    mov rax, qword ptr [rbp]
    mov rbx, qword ptr [rbp - 8]
    mov rcx, qword ptr [rbp - 010h]
    mov rdx, qword ptr [rbp - 018h]
    mov rsi, qword ptr [rbp - 020h]
    mov rdi, qword ptr [rbp - 028h]
    mov r8, qword ptr [rbp - 030h]
    mov r9, qword ptr [rbp - 038h]
    mov r10, qword ptr [rbp - 040h]
    mov r11, qword ptr [rbp - 048h]
    mov r12, qword ptr [rbp - 050h]
    mov r13, qword ptr [rbp - 058h]
    mov r14, qword ptr [rbp - 060h]
    mov r15, qword ptr [rbp - 068h]
    mov rsp, qword ptr [rbp - 070h]
    call qword ptr [rbp + 8]
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rsp
    push rbp
    mov qword ptr [rbp - 080h], rsp
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 078h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 070h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 8], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 068h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 010h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 060h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 018h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 058h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 020h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 050h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 028h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 048h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 030h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 040h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 038h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 038h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 040h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 030h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 048h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 028h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 050h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 020h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 058h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 018h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 060h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 010h]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 068h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax + 8]
    add rcx, 070h
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 070h], rcx
    mov rax, qword ptr [rbp - 080h]
    mov rcx, qword ptr [rax]
    mov rax, qword ptr [rbp + 028h]
    mov qword ptr [rax + 078h], rcx
    mov rsp, qword ptr [rbp + 018h]
    mov rbp, qword ptr [rbp + 010h]
    add rsp, 0b0h
    pop rbx
    pop rdi
    pop rsi
    pop r12
    pop r13
    pop r14
    pop r15
    pop rbp
    ret
    AsmCall ENDP

    AsmMovsx PROC
    sub rsp, 020h
    mov qword ptr [rsp + 018h], r9
    mov byte ptr [rsp + 017h], r8b
    mov qword ptr [rsp + 8], rdx
    mov byte ptr [rsp + 7], cl
    mov r8, qword ptr [rsp + 8]
    mov r9, qword ptr [rsp + 018h]
    mov r10, qword ptr [r8]
    mov r11, qword ptr [r9]
    movsx eax, byte ptr [rsp + 7]
    cmp eax, 071h
    je jmpMovsx1
    movsx eax, byte ptr [rsp + 7]
    cmp eax, 064h
    jne jmpMovsx2
    jmpMovsx1:
    movsx eax, byte ptr [rsp + 017h]
    cmp eax, 077h
    jne jmpMovsx3
    movsx r10, r11w
    jmp jmpMovsx4
    jmpMovsx3:
    movsx eax, byte ptr [rsp + 017h]
    cmp eax, 062h
    jne jmpMovsx5
    movsx r10, r11b
    jmpMovsx5:
    jmp jmpMovsx4
    jmpMovsx4:
    jmp jmpMovsx6
    jmpMovsx2:
    movsx eax, byte ptr [rsp + 7]
    cmp eax, 077h
    jne jmpMovsx7
    movsx r10w, r11b
    jmpMovsx7:
    jmp jmpMovsx6
    jmpMovsx6:
    mov qword ptr [r8], r10
    add rsp, 020h
    ret
    AsmMovsx ENDP

    AsmMovzx PROC
    sub rsp, 020h
    mov qword ptr [rsp + 018h], r9
    mov byte ptr [rsp + 017h], r8b
    mov qword ptr [rsp + 8], rdx
    mov byte ptr [rsp + 7], cl
    mov r8, qword ptr [rsp + 8]
    mov r9, qword ptr [rsp + 018h]
    mov r10, qword ptr [r8]
    mov r11, qword ptr [r9]
    movsx eax, byte ptr [rsp + 7]
    cmp eax, 071h
    je jmpMovzx1
    movsx eax, byte ptr [rsp + 7]
    cmp eax, 064h
    jne jmpMovzx2
    jmpMovzx1:
    movsx eax, byte ptr [rsp + 017h]
    cmp eax, 077h
    jne jmpMovzx3
    movzx r10, r11w
    jmp jmpMovzx4
    jmpMovzx3:
    movsx eax, byte ptr [rsp + 017h]
    cmp eax, 062h
    jne jmpMovzx5
    movzx r10, r11b
    jmpMovzx5:
    jmp jmpMovzx4
    jmpMovzx4:
    jmp jmpMovzx6
    jmpMovzx2:
    movsx eax, byte ptr [rsp + 7]
    cmp eax, 077h
    jne jmpMovzx7
    movzx r10w, r11b
    jmpMovzx7:
    jmp jmpMovzx6
    jmpMovzx6:
    mov qword ptr [r8], r10
    add rsp, 020h
    ret
    AsmMovzx ENDP

    AsmCmp PROC
    push rbp
    sub rsp, 028h
    lea rbp, [rsp + 020h]
    mov qword ptr [rbp], r9
    mov qword ptr [rbp - 8], r8
    mov qword ptr [rbp - 010h], rdx
    mov byte ptr [rbp - 011h], cl
    mov r8, qword ptr [rbp - 010h]
    mov r9, qword ptr [rbp - 8]
    mov r10, qword ptr [r8]
    mov r11, qword ptr [r9]
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 071h
    jne jmpCmp1
    cmp r10, r11
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpCmp1:
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 064h
    jne jmpCmp2
    cmp r10d, r11d
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpCmp2:
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 077h
    jne jmpCmp3
    cmp r10w, r11w
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpCmp3:
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 062h
    jne jmpCmp4
    cmp r10b, r11b
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpCmp4:
    mov rcx, qword ptr [rbp - 020h]
    mov rax, qword ptr [rbp]
    mov qword ptr [rax + 088h], rcx
    add rsp, 028h
    pop rbp
    ret
    AsmCmp ENDP

    AsmTest PROC
    push rbp
    sub rsp, 028h
    lea rbp, [rsp + 020h]
    mov qword ptr [rbp], r9
    mov qword ptr [rbp - 8], r8
    mov qword ptr [rbp - 010h], rdx
    mov byte ptr [rbp - 011h], cl
    mov r8, qword ptr [rbp - 010h]
    mov r9, qword ptr [rbp - 8]
    mov r10, qword ptr [r8]
    mov r11, qword ptr [r9]
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 071h
    jne jmpTest1
    test r10, r11
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpTest1:
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 064h
    jne jmpTest2
    test r10d, r11d
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpTest2:
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 077h
    jne jmpTest3
    test r10w, r11w
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpTest3:
    movsx eax, byte ptr [rbp - 011h]
    cmp eax, 062h
    jne jmpTest4
    test r10b, r11b
    pushfq
    pop rax
    mov qword ptr [rbp - 020h], rax
    jmpTest4:
    mov rcx, qword ptr [rbp - 020h]
    mov rax, qword ptr [rbp]
    mov qword ptr [rax + 088h], rcx
    add rsp, 028h
    pop rbp
    ret
    AsmTest ENDP

    AsmJe PROC
    push rbp
    sub rsp, 010h
    lea rbp, [rsp + 010h]
    mov qword ptr [rbp - 8], rcx
    mov dword ptr [rbp - 0ch], 1
    mov rax, qword ptr [rbp - 8]
    push rax
    popfq
    je jmpJe
    mov dword ptr [rbp - 0ch], 0
    jmpJe:
    mov eax, dword ptr [rbp - 0ch]
    add rsp, 010h
    pop rbp
    ret
    AsmJe ENDP

    AsmJne PROC
    push rbp
    sub rsp, 010h
    lea rbp, [rsp + 010h]
    mov qword ptr [rbp - 8], rcx
    mov dword ptr [rbp - 0ch], 1
    mov rax, qword ptr [rbp - 8]
    push rax
    popfq
    jne jmpJne
    mov dword ptr [rbp - 0ch], 0
    jmpJne:
    mov eax, dword ptr [rbp - 0ch]
    add rsp, 010h
    pop rbp
    ret
    AsmJne ENDP

    AsmJbe PROC
    push rbp
    sub rsp, 010h
    lea rbp, [rsp + 010h]
    mov qword ptr [rbp - 8], rcx
    mov dword ptr [rbp - 0ch], 1
    mov rax, qword ptr [rbp - 8]
    push rax
    popfq
    jbe jmpJbe
    mov dword ptr [rbp - 0ch], 0
    jmpJbe:
    mov eax, dword ptr [rbp - 0ch]
    add rsp, 010h
    pop rbp
    ret
    AsmJbe ENDP

    AsmJl PROC
    push rbp
    sub rsp, 010h
    lea rbp, [rsp + 010h]
    mov qword ptr [rbp - 8], rcx
    mov dword ptr [rbp - 0ch], 1
    mov rax, qword ptr [rbp - 8]
    push rax
    popfq
    jl jmpJl
    mov dword ptr [rbp - 0ch], 0
    jmpJl:
    mov eax, dword ptr [rbp - 0ch]
    add rsp, 010h
    pop rbp
    ret
    AsmJl ENDP

    AsmJge PROC
    push rbp
    sub rsp, 010h
    lea rbp, [rsp + 010h]
    mov qword ptr [rbp - 8], rcx
    mov dword ptr [rbp - 0ch], 1
    mov rax, qword ptr [rbp - 8]
    push rax
    popfq
    jge jmpJge
    mov dword ptr [rbp - 0ch], 0
    jmpJge:
    mov eax, dword ptr [rbp - 0ch]
    add rsp, 010h
    pop rbp
    ret
    AsmJge ENDP

    AsmJle PROC
    push rbp
    sub rsp, 010h
    lea rbp, [rsp + 010h]
    mov qword ptr [rbp - 8], rcx
    mov dword ptr [rbp - 0ch], 1
    mov rax, qword ptr [rbp - 8]
    push rax
    popfq
    jle jmpJle
    mov dword ptr [rbp - 0ch], 0
    jmpJle:
    mov eax, dword ptr [rbp - 0ch]
    add rsp, 010h
    pop rbp
    ret
    AsmJle ENDP
END
