.global _start

.section .text
_start:
	# Input: eax = Num1
	
	movl Num1, %eax  
	xor %r8d, %r8d        # r8d = 0
	xor %ecx, %ecx        # ecx = 0
	
	
count_ones_HW1:
    # Counts the number of 1's
	
	testl $1, %eax         # Check if the least significant bit = 0
    jz skip_HW1           
	incl %ecx              # ecx++
	
skip_HW1:
	# Skip 1 bit
	shrq $1, %rax          # Shift right by 1
	testl %eax, %eax       # Check id eax = 0
	jnz count_ones_HW1        # Repeat if eax is not zero
	
	
	testl %r8d, %r8d       # Check id r8d = 0
	jz again_HW1
	jmp finish_HW1
	
	
again_HW1:
	movl %ecx, %r9d       # r9d = result1
	movl Num2, %eax       # Input: eax = Num2
	incl %r8d              # r8d++
	xorl %ecx, %ecx        # ecx = 0
	jmp count_ones_HW1
	
finish_HW1:
	movl %ecx, %r10d       # r10d = result2
	testl $1, %r9d          # Check if r9d is even/odd
	jz even_HW1
	jmp odd_HW1
	
even_HW1:
	testl $1, %r10d          # Check if r10d is even/odd
	jz return_1_HW1
	jmp return_0_HW1
	
odd_HW1:
	testl $1, %r10d          # Check if r10d is even/odd
	jz return_0_HW1
	jmp return_1_HW1
	
return_0_HW1:
	movb $0, BitCheck            #  BitCheck = 0
	jmp end_HW1
	
	
return_1_HW1:
	movb $1, BitCheck            #  BitCheck = 1
	
end_HW1: