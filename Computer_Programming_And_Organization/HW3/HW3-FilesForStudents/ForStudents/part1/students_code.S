.global	compute_char_repeats, parse_lines

.section .text
compute_char_repeats:
	pushq %rbp
	movq %rsp, %rbp
	
	xorl %eax, %eax         # result = 0
	xorl %ecx, %ecx         # counter = 0
	
iterate:
	cmpl %esi, %ecx         # check if done
	je done
    movb (%rdi), %r8b       # r8b = char
    incq %rdi               # rdi++
	incl %ecx               # counter++
	
	cmpb %dl, %r8b
	je increase             # found special char
	jmp iterate
	
increase:
	incl %eax               # result++
	jmp iterate

done:
	leave
	ret
	
#=====================================================================================================================================

parse_lines:
	pushq %rbp
	movq %rsp, %rbp
	subq    $193, %rsp       # 128 bytes for buf + 25 bytes for local + 40 bytes for registers restore
	
    # local variables
    movq %rdi, -8(%rbp)      # path
    movq %rsi, -16(%rbp)     # line_max_len 
    movq %rdx, -24(%rbp)     # line_max_repeat
	
	movb $0, -25(%rbp)       # bytes_read = 0
	movq -16(%rbp), %rax
	movl $0, (%rax)          # line_max_len = 0
	movq -24(%rbp), %rax
	movl $0, (%rax)          # line_max_repeat = 0
	
	# -153 -> -25 is the buffer
	# saving old registers
	movq %r15, -193(%rbp)
	movq %rbx, -185(%rbp)
	movq %r12, -177(%rbp)
	movq %r13, -169(%rbp)
	movq %r14, -161(%rbp)
	
	movl $1, %r13d           # lines_counter = 1   
	
	# getting the special char
	call get_the_special_char
	movb %al, %bl            # special char
	
	# syscall open
	movq $2, %rax            # syscall number for open
    movq -8(%rbp), %rdi      # path
	movq $0, %rsi            # O_RDONLY == 0
    syscall
    movq %rax, %r12          # saving the file descriptor
	leaq -154(%rbp), %rsi    # set buffer
    
main_iterate:
	# syscall read
	movq $0, %rax            # syscall number for read
    movq %r12, %rdi          # fd
    incq %rsi                # points to next char in the buffer
    movq $1, %rdx            # bytes to read
    syscall
	movb %al, %bh
    testb %al, %al
    je newline               # al==0 → EOF
	cmpb $'\n', (%rsi)
	je newline
	
	movb -25(%rbp), %al     
	incb %al                 # bytes_read++
	movb %al, -25(%rbp)
	jmp main_iterate
	
newline:
	# calling compute_char_repeats
	leaq -153(%rbp), %rdi    # param 1
	movzbl -25(%rbp), %esi   # param 2
	movb %bl, %dl            # param 3
	call compute_char_repeats
	
	leaq -154(%rbp), %rsi    # reset buffer
	
	movq -16(%rbp), %r14
	movb -25(%rbp), %r15b
	movb $0, -25(%rbp)
	cmpb %r15b, (%r14)       # (current_line_length > max_line_length) -> max = current
	jae continue1
	
	movb %r15b, (%r14)
	
continue1:	
	movq -24(%rbp), %r14
	cmpl %eax, (%r14)  		 # (current_special_char > max_special_char) -> max = current
	jae continue2
	
	movl %eax, (%r14)
	
continue2:	
	testb %bh, %bh
	je finish
	
	incl %r13d               # lines_counter++
	jmp main_iterate
	
finish:
    movq $3, %rax         # sys_close
    movq %r12, %rdi       # file descriptor in rdi
    syscall               # call the kernel to close the file
	mov %r13d, %eax
	
	# restore old registers
	movq -193(%rbp), %r15
	movq -185(%rbp), %rbx
	movq -177(%rbp), %r14
	movq -169(%rbp), %r13
	movq -161(%rbp), %r12
	
	leave
	ret
