.globl my_ili_handler

.text
.align 4, 0x90
my_ili_handler:
  
  #PUSH_AND_CLEAR_REGS
  pushq %r8
  pushq %r9
  pushq %r10
  pushq %r11
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15
  pushq %rax
  pushq %rbx
  pushq %rcx
  pushq %rdx
  pushq %rsi
  pushq %rbp
  pushq %rsp


	xorq %rax, %rax # return value
	xorq %rdi, %rdi # parameter
	xorq %rdx, %rdx # temp
	
	
	# get the invalid opcode
	movq 120(%rsp), %rdx
	movq (%rdx), %rdx
	
	# case of one byte opcode
	cmpb $0x0f, %dl
	jne one_byte_handler
	
	movb %dh, %al 
	movq %rax, %rdi # prepare parameter
	call what_to_do
	
	# if %rax zero then old handler
	testq %rax, %rax
	jz old_handler
	jmp two_byte_handler
	
one_byte_handler:
	
	movb %dl, %al 
	movq %rax, %rdi
	call what_to_do
	
	# if %rax zero then old handler
	testq %rax, %rax
	jz old_handler
	

movq %rax, %rdi

	popq %rsp
	popq %rbp
	popq %rsi
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8

addq $1, (%rsp) # go over the opcode
jmp done

two_byte_handler:
movq %rax, %rdi

	popq %rsp
	popq %rbp
	popq %rsi
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8

addq $2, (%rsp) # go over the opcode
jmp done

old_handler:

	popq %rsp
	popq %rbp
	popq %rsi
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
  
	jmp * old_ili_handler
  
done:
  iretq
