.global _start

.section .text
_start:
	movq $String, %rsi
	decq %rsi               # Lower--
	xorb %bl, %bl           # bl = case 1->false
	xorb %cl, %cl           # cl = case 2->false
	
iterate_HW1:
	incq %rsi               # Lower++
    movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1       # al = space
	
	cmpb $1, %cl
	je check_lowercase_HW1
	
# Check if zero
	cmpb $'0', %al
	je case_1_HW1
	
	
# Check if al isnt 0-9
	cmpb $'0', %al          
    jb check_lowercase_HW1
    cmpb $'9', %al          
    ja check_lowercase_HW1  	
	
	jmp case_2_HW1

check_lowercase_HW1:
	cmpb $1, %bl
	je slide_HW1
	
# Check if al isnt a-z
	cmpb $'a', %al          
    jb slide_HW1
    cmpb $'z', %al          
    ja slide_HW1

	jmp case_3_HW1
	
	
	

	

slide_HW1:
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp slide_HW1
	
	
	
	
	
	
case_1_HW1:
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'x', %al
	je case1_continue_HW1
	cmpb $'b', %al
	je case1_continue_HW1
	jmp case_2_HW1
	
case1_continue_HW1:
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
# Check if al isnt 0-9
	cmpb $'0', %al          
    jb slide_HW1
    cmpb $'9', %al          
    ja slide_HW1  	





	
case_2_HW1:
	movb $1, %cl            # cl = case 2->true
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	jne iterate_HW1       # al = space
	
# Check if al isnt 0-9
	cmpb $'0', %al          
    jb fail2_HW1
    cmpb $'9', %al          
    ja fail2_HW1
	
	jmp case_2_HW1
	
fail2_HW1:
	xorb %cl, %cl           # cl = case 2->false
	jmp slide_HW1
		
		
		
		
		
		
		
		
		
		
case_3_HW1:
# add_HW1
	movb $1, %bl            # bl = case 1->true
	cmpb $'a', %al 
	jne sub_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'d', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'d', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp fail1_HW1
	
sub_HW1:
	cmpb $'s', %al 
	jne mul_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'u', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'b', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp fail1_HW1
	
mul_HW1:
	cmpb $'m', %al 
	jne div_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'u', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'l', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp fail1_HW1
	
div_HW1:
	cmpb $'d', %al 
	jne imul_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'i', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'v', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp fail1_HW1
	
imul_HW1:
	cmpb $'i', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'m', %al 
	jne idiv_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'u', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'l', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp fail1_HW1

idiv_HW1:
	cmpb $'d', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'i', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
	cmpb $'v', %al 
	jne fail1_HW1
    incq %rsi               # Lower++
	movb (%rsi), %al        # al = char
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space


fail1_HW1:
	xorb %bl, %bl           # bl = case 1->false
    testb %al, %al
    je done_HW1             # al = null
	cmpb $' ', %al
	je iterate_HW1          # al = space
	jmp slide_HW1






done_HW1:
	cmpb $1, %bl
	je return1_HW1
	
	cmpb $1, %cl
	je return2_HW1
	
	movb $0, Result
	jmp end_HW1
	
return1_HW1:
	
	cmpb $1, %cl
	je return3_HW1
	
	movb $1, Result
	jmp end_HW1
	
return2_HW1:
	movb $2, Result
	jmp end_HW1
	
return3_HW1:
	movb $3, Result
	jmp end_HW1
	
end_HW1:

	