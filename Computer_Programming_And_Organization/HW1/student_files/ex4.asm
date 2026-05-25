.global _start

.section .text
_start:
	movq $Lower, %rsi       
    movq $Upper, %rdi       
	
copy_HW1:
    movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1            # al = null
	

# Check if al isnt 0-9
	cmpb $'0', %al          
    jb check_HW1
    cmpb $'9', %al          
    ja check_HW1            	
	
	movb %al, (%rdi)         # Updating Upper
    incq %rdi                # Upper++
	jmp ignore_HW1
	
check_HW1:
# Check if al isnt A-Z
	cmpb $'A', %al          
    jb edit_char_HW1
    cmpb $'Z', %al          
    ja edit_char_HW1            
	
	movb %al, (%rdi)         # Updating Upper
    incq %rdi                # Upper++
	jmp ignore_HW1
	
edit_char_HW1:
# Check if al isnt a-z
	cmpb $'a', %al          
    jb ignore_HW1
    cmpb $'z', %al          
    ja ignore_HW1
	
	subb $32, %al            # Convert to upper case
	movb %al, (%rdi)         # Updating Upper
    incq %rdi                # Upper++
	
ignore_HW1:
    incq %rsi                # Lower++
    jmp copy_HW1
	
done_HW1: