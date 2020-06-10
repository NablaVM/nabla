#ifndef DEL_ASM_MATH_SUPPORT_HPP
#define DEL_ASM_MATH_SUPPORT_HPP

/*

    Note : This information should not be directly used by anyone except for the AsmSupport class, as it handles 
    multiple import issues and ensures that the correct thing is loaded

*/


#include <string>

namespace DEL
{
namespace BUILT_IN
{
    static const std::string ASM_MOD_FUNCTION_NAME   = "__del__math__MODULUS";
    static const std::string ASM_POW_FUNCTION_NAME   = "__del__math__POWER";
    static const std::string ASM_MOD_D_FUNCTION_NAME = "__del__math__MODULUS_D";
    static const std::string ASM_POW_D_FUNCTION_NAME = "__del__math__POWER_D";

/*


        MATH EXTENSIONS - STANDARD ARITH


*/

static const std::string ASM_MOD = R"asm(
;
;	MODULUS
;	Performs modulus operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 % r2
<__del__math__MODULUS:

    blt r1 r2 modspecial
    pushw ls r1
    pushw ls r2 
    pushw ls r3
    pushw ls r4

	mov r3 $0   		; Init temp, and result
	div r3 r1 r2		; Divide Num/Denom
    mov r4 $0
	beq r3 r4 moduiz	; Check if mod is 0
	jmp moduinz	    	; If not, go to moduinz
moduiz:
		mov r0 $0
		jmp moddone
moduinz:
		mul r3 r2 r3 	; Mult denom by result
		sub r0 r1 r3	; Sub result from num
		jmp moddone
moddone:
    popw r4 ls 
    popw r3 ls 
    popw r2 ls 
    popw r1 ls 
    ret
modspecial:
    mov r0 r1
>
)asm";

static const std::string ASM_POW = R"asm(
;
;	POWER
;	Performs power operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 pow( r2 )

<__del__math__POWER:
    pushw ls r3
    mov r0 $1
    mov r3 $1
top:
    bgt r3 r2 bottom
    add r3 r3 $1
    mul r0 r0 r1
    jmp top

bottom:
    popw r3 ls 
    ret
>

)asm";



/*


        MATH EXTENSIONS - DOUBLE ARITH


*/

static const std::string ASM_MOD_D = R"asm(
;
;	MODULUS_D
;	Performs modulus operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 % r2
<__del__math__MODULUS_D:

    blt.d r1 r2 modspecial
    pushw ls r1
    pushw ls r2 
    pushw ls r3
    pushw ls r4

	mov r3 $0   		; Init temp, and result
	div.d r3 r1 r2		; Divide Num/Denom
    mov r4 $0
	beq.d r3 r4 moduiz	; Check if mod is 0
	jmp moduinz	    	; If not, go to moduinz
moduiz:
		mov r0 $0
		jmp moddone
moduinz:
		mul.d r3 r2 r3 	; Mult denom by result
		sub.d r0 r1 r3	; Sub result from num
		jmp moddone
moddone:
    popw r4 ls 
    popw r3 ls 
    popw r2 ls 
    popw r1 ls 
    ret
modspecial:
    mov r0 r1
>
)asm";

static const std::string ASM_POW_D = R"asm(
;
;	POWER_D
;	Performs power operation on r1, and r2
;	Results in r0
;	Ex:  r0 = r1 pow( r2 )

<__del__math__POWER_D:
    pushw ls r3
    pushw ls r4
    mov r4 $1
    mov r0 $1
    mov r3 $1
top:
    bgt.d r3 r2 bottom
    add.d r3 r3 r4
    mul.d r0 r0 r1
    jmp top

bottom:
    popw r4 ls 
    popw r3 ls 
    ret
>

)asm";

}
}

#endif 