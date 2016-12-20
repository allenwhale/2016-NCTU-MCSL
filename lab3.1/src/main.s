.syntax unified
.cpu cortex-m4
.thumb

.data
	user_stack: .zero 128         // Declare own stack
	expr_result: .word 0

.text
	.global main
	postfix_expr: .asciz "-100 10 20 + - 10 ++"
	.align 4                      // Prevent unalignment from postfix_expr declaration

// Postfix
// Input: r0 - start of postfix_expr, r8 - start of stack pointer (Used for Reference)
// Output: r1 - ans(-1 if invalid), write to expr_result in memory

postfix:
postfix_loop:
	bl atoi                       // Call ATOI to process next item
	cmp r1, #1                    // If ATOI return 1, it parsed an invalid string
	beq postfix_invalid           //     branch to postfix_invalid
	cmp r1, #-3                   // If ATOI return -3, it parsed an end of string
	beq postfix_finish            //     branch to postfix_finish
	cmp r1, #-1                   // If ATOI return -1, it parsed an operator
	beq postfix_op                //     branch to postfix_op
postfix_number:                   // Else, it parsed an number
	PUSH {r2}                     //     PUSH the number into the stack
	b postfix_loop                //     continue the loop
postfix_op:                       // If it parsed an operator
	cbnz r2, postfix_op_minus     //     Check r2 to see if it is plus(0) or minus (1)
postfix_op_add:                   // If it is a plus
	mov r3, sp                    // Check stack to see if there are two items to POP
	sub r3, r8, r3                // Use r8 as a reference point
	cmp r3, 8                     // If there aren't two items to POP
	blt postfix_invalid           //     branch to postfix_invalid
	POP {r2, r3}                  // Else, POP the two numbers into r2, r3
	add r2, r3, r2                //     Add them
	PUSH {r2}                     //     Save them back into stack
	b postfix_loop                //     continue the loop
postfix_op_minus:                 // If it is a minus
	mov r3, sp                    // Check stack to see if there are two items to POP
	sub r3, r8, r3                // Use r8 as a reference point
	cmp r3, 8                     // If there aren't two items to POP
	blt postfix_invalid           //     branch to postfix_invalid
	POP {r2, r3}                  // Else, POP the two numbers into r2, r3
	sub r2, r3, r2                //     Subtract them
	PUSH {r2}                     //     Save them back into stack
	b postfix_loop                //     continue the loop
postfix_finish:                   // Parsed end of String
	mov r3, sp                    // Check stack to see if there are one items left
	sub r3, r8, r3                // Use r8 as a reference point
	cmp r3, 4                     // If there aren't one items left
	bne postfix_invalid           //     branch to postfix_invalid
	ldr r0, =expr_result
	POP {r1}                      // POP the result
	str r1, [r0]                  // Write it to expr_result
	b program_end                 // Branch to program_end
postfix_invalid:                  // Something is invalid
	ldr r0, =expr_result
	mov r1, #-1
	str r1, [r0]                  // Write -1 to expr_result
	b program_end                 // Branch to program_end

// Strlen
// Input: r0 - start of string
// Output: r0 - ans

strlen:
	mov r1, #0                    // r1 - counter
strlen_loop:
	ldrb r2, [r0], #1             // load one byte at a time
	cbz r2, strlen_end            // Check if it is end of string
	add r1, r1, #1                // Increment Counter
	b strlen_loop                 // Continue loop
strlen_end:
	mov r0, r1                    // move counter value to r0
	bx lr                         // return

// ATOI
// Input: r0 - start of postfix_expr
// Output: r0 - next start of postfix_expr
//         r1 = valid(0) or not(1) or plus/minus(-1) or finish(-3)
//         r2 = value or sign(+-)

atoi:
	mov r1, #0                    // r1 = value
	mov r2, #0                    // r2 = sign
	mov r5, #0                    // r5 = have only encountered +/-
	ldrb r3, [r0]                 // Load next byte
	cbz r3, atoi_stop             // If end of string, branch to atoi_stop
	cmp r3, #45                   // If it is "-"
	beq atoi_neg                  //     branch to atoi_neg
	cmp r3, #43                   // If it is "+"
	beq atoi_pos                  //     branch to atoi_pos
	cmp r3, #32                   // If it is not space
	bne atoi_digit                //     branch to atoi_digit
	add r0, r0, #1                // Else, increment r0's position
	b atoi                        //     branch to atoi
atoi_digit:
	ldrb r3, [r0], #1             // Load the next byte and increment r0
	cbz r3, atoi_done             // If encounter end of string or encounter space
	cmp r3, #32
	beq atoi_done                 //     branch to atoi_done
	cmp r3, #48                   // If not between 0~9
	blt atoi_invalid              //     branch to atoi_invalid
	cmp r3, #57
	bgt atoi_invalid
	mov r5, #0                    // Else, set r5 back to 0 (have encounter not only op)
	sub r3, r3, #48               // Switch ascii code to number (subtract 48)
	lsl r4, r1, #3                // r1 = r1*10+r3
	lsl r1, r1, #1                // r1 = r1*8+r1*2+r3
	add r1, r1, r4
	add r1, r1, r3
	b atoi_digit                  // Continue loop
atoi_neg:
	mov r5, #1                    // Set r5 to 1, encounter op
	mov r2, #1                    // Set sign t0 neg(1)
	add r0, #1                    // Increment r0
	b atoi_digit                  // Branch to atoi_digit
atoi_pos:
	mov r5, #1                    // Set r5 to 1, encounter op
	add r0, #1                    // Increment r0
	b atoi_digit                  // Branch to atoi_digit
atoi_done:
	cbnz r5, atoi_plus_minus      // Check if an op is what we only get
	cbz r2, atoi_ret_pos          // If sign is not neg, branch to atoi_ret_pos
atoi_ret_neg:
	neg r2, r1                    // Sign is neg, neg the value in r1 to r2
	mov r1, #0                    // Set return code to 0(valid)
	sub r0, r0, #1                // Move r0 back one to prevent skipping end of string
	bx lr                         // Return
atoi_ret_pos:
	mov r2, r1                    // Sign is pos, mov the value in r1 to r2
	mov r1, #0                    // Set return code to 0(valid)
	sub r0, r0, #1                // Move r0 back one to prevent skipping end of string
	bx lr                         // Return
atoi_invalid:
	mov r1, #1                    // Set return code to 1(invalid)
	sub r0, r0, #1                // Move r0 back one to prevent skipping end of string
	bx lr                         // Return
atoi_stop:
	mov r1, #-3                   // Set return code to -3(end of string)
	sub r0, r0, #1                // Move r0 back one to prevent skipping end of string
	bx lr                         // Return
atoi_plus_minus:
	mov r1, #-1                   // Set return code to -1(encounter op)
	sub r0, r0, #1                // Move r0 back one to prevent skipping end of string
	bx lr                         // Return

main:
	ldr r0, =user_stack           // setup self defined stack
	add r0, #128                  // make sp point to correct location (top of stack)
	mov sp, r0
	mov r8, r0                    // r8 = stack base for refernce
	ldr r0, =postfix_expr         // load r0 with the location of postfix_expr
	bl postfix                    // Start postfix operation

program_end: b program_end
