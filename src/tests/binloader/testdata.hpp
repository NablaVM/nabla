#include <string>


namespace NABLA
{
    namespace TEST
    {
        namespace BINLOADER
        {
            const std::string ASM_CONSTANTS =
                ".file \"constants\"\n"
                ".init main\n"
                ".int8  integer     96\n"
                ".int16 integer1    42\n"
                ".int32 integer2    55\n"
                ".int64 integer3    568888\n"
                ".double lhsd       10.0\n"
                ".double rhsd       1.5\n"
                ".string str \"This is a string\"\n"
                "<main:\n"
                "    size r0 gs\n"
                "    ret\n"
                ">\n";

            const std::string ASM_FUNCTIONS =
                ".file \"functions\"\n"
                ".init main\n"
                "<awesome:\n"
                ">\n"
                "<moreso:\n"
                ">\n"
                "<main:\n"
                ">\n";

            const std::string ASM_INSTRUCTS = 
                ".init math\n"
                "<math:\n"
                "    mov r0 $2\n"
                "    mul r0 r0 $2 \n"
                "    add r0 r0 r0\n"
                "    sub r0 r0 $2\n"
                "    div r0 $10 r0 \n"
                "    call notmath\n"
                ">\n"
                "<notmath:\n"
                "alabel:\n"
                ">\n";
        }
    }
}