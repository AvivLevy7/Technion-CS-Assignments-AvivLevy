.global _start

.section .text
_start:  
	movq Address(%rip), %rdi                   # rdi = Address
	movl Length(%rip), %eax                    # eax = Length
	lea -1(%rdi, %rax), %rsi                   # rsi = Address + Length-1
	
iterate_HW1:
	cmp %rsi, %rdi                            # Check if left pointer >= right pointer
    jge palindrome_HW1
	
    movzbq (%rdi), %rax
    movzbq (%rsi), %rbx
	
	cmp %rax, %rbx                            # Equality check
    jne not_palindrome_HW1
	
	incq %rdi                                  # rdi++
    decq %rsi                                  # rsi--
    jmp iterate_HW1

palindrome_HW1:
	movb $1, Result                           # Return true 
	jmp end_HW1                               

not_palindrome_HW1:
	movb $0, Result                           # Return false 
	
end_HW1:


	
	
	

	
	
	
	
	
	