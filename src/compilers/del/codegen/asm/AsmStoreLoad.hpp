#ifndef DEL_ASM_STORE_LOAD_SUPPORT_HPP
#define DEL_ASM_STORE_LOAD_SUPPORT_HPP




#include <string>

namespace DEL
{
namespace BUILT_IN
{

static const std::string ASM_ALLOC = R"asm(
;
;	ALLOC
;   Input : r0 - Number of bytes to allocate
;   Output: r0 - Address of new allocation in DS Device
;
<__del__ds__alloc:

	lsh r0  r0  $16 	; Move allocation request into place
	lsh r1  $13 $56 	; Move DS Device ID into place
	or  r10 r0  r1  	; Or together to create command
	mov r0 r12          ; Store resulting address in r0
    ret
>

)asm";

static const std::string ASM_LOAD = R"asm(
;
;	LOAD
;   Input : r0 - Address of item in DS
;   Input : r1 - Number of words to load 
;   Output: r0 - 0 if no errors returned, 1 if error loading
;   Output: Bytes loaded from DS will be in the global stack from the end of when this method called
;           spanning to the end of the byte length requested for load
;
<__del__ds__load:

	; Get size of GS and store for command
	size r5 gs
	pushw ls r5

	mov r9 $0
	mov r8 $0

expand_stack_for_load:
	pushw gs r9

    ; Inc r8 counter
    add r8 r8 $1

    ; Check that our counter is less than the number of 
    ; words the caller asked us to load (r1)
	blt r8 r1 expand_stack_for_load

	; Get the new size of GS
	size r5 gs
	pushw ls r5

	mov r11 r0      ; Load address caller gave us 
	popw r3 ls      ; End idx
	popw r4 ls      ; Start idx
	lsh r4 r4 $32   ; Shift start index prepping for OR
	or r12 r3 r4
	lsh r0 $13 $56  ; Move DS id into position
	lsh r1 $20 $48  ; Mark sub-id for 'load' command
	or r10 r0 r1  	; Or command into trigger register

	; Move result to r0 so the user can handle issues
	mov r0 r11
    ret
>

)asm";

static const std::string ASM_STORE = R"asm(
;
;	STORE
;   Input : r0 - Address of item in DS
;   Input : r1 - Start index of GS to load
;   Input : r2 - End index of GS to load
;   Output: r0 - 0 if no errors returned, 1 if error loading
;   Note  : This method will removes data for transit from the GS
;           after the call is made 
;
<__del__ds__store:

	sub r3 r2 r1    ; Determine how many bytes for cleanup

	mov r11 r0    	; Move DS address stored in r0 into target register r11
	lsh r0 r1 $32 	; Left shift start address 
	or r12 r0 r2  	; Or start and end GS locations for command into target register r12
	lsh r0 $13 $56	; Move DS ID into position
	lsh r1 $10 $48	; Mark sub-id for 'store' command
	or r10 r1 r0  	; Or command into trigger register

	; Move result to r0 so the user can handle issues
	mov r0 r11
	mov r1 $0

	; Clean up the GS
gs_cleanup:

	popw r5 gs

	; Inc r1 counter by WORD length becasue we are popping words
	add r1 r1 $8
	blt r1 r3 gs_cleanup
	ret
>

)asm";

}
}

#endif 