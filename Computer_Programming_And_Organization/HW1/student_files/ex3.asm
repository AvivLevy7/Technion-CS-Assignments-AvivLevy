.global _start

.section .text
_start:
	
	movq First, %rdi             # Load the head of the list
	xorl %ecx, %ecx               # ecx = 0

traverse_list_arith_1_HW1:
    cmpq $0, %rdi                # If pointer is NULL, end of list
    je arithmetic_rank_1_HW1
    
    movq (%rdi), %rsi            # Get the next node
    movl 8(%rdi), %r8d           # Get the current node's value

    testl %ecx, %ecx
    jz next_node_arith_1_HW1    # Skip once

    subl %ebx, %r8d              # r8d = difference

    cmpl $1, %ecx
    jz save_diff_arith_1_HW1     # Save first diff
    
    subl %edx, %r8d              # r8d = difference of differences
    cmpl $0, %r8d
    jne prepare_HW1
	addl %edx, %r8d

save_diff_arith_1_HW1:
    movl %r8d, %edx              # edx = current difference

next_node_arith_1_HW1:
    movl 8(%rdi), %ebx           # ebx = current value
    movq %rsi, %rdi              # Next
    incl %ecx                    # ecx++
    jmp traverse_list_arith_1_HW1
	
	
	
prepare_HW1:
	movl %r8d, %r9d
	movq First, %rdi             # Load the head of the list
	xorl %ecx, %ecx               # ecx = 0

	
	
traverse_list_arith_2_HW1:
    cmpq $0, %rdi                # If pointer is NULL, end of list
    je arithmetic_rank_2_HW1
    
    movq (%rdi), %rsi            # Get the next node
    movl 8(%rdi), %r8d           # Get the current node's value

    testl %ecx, %ecx
    jz next_node_arith_2_HW1    # Skip once

    subl %ebx, %r8d              # r8d = difference

    cmpl $1, %ecx
    jz save_diff_arith_2_HW1     # Save first diff
    
    subl %edx, %r8d              # r8d = difference of differences
    cmpl %r9d, %r8d
    jne nothing_HW1
	addl %edx, %r8d

save_diff_arith_2_HW1:
    movl %r8d, %edx              # edx = current difference

next_node_arith_2_HW1:
    movl 8(%rdi), %ebx           # ebx = current value
    movq %rsi, %rdi              # Next
    incl %ecx                    # ecx++
    jmp traverse_list_arith_2_HW1
	
	

	
arithmetic_rank_1_HW1:
	movl $1, %eax
	jmp end_HW1
	
arithmetic_rank_2_HW1:
	movl $2, %eax
	jmp end_HW1
	
nothing_HW1:
	xorl %eax, %eax
	
end_HW1:
	movl %eax, Result
	