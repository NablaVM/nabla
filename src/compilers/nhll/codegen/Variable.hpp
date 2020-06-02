#ifndef NHLL_CODEGEN_VARIABLE
#define NHLL_CODEGEN_VARIABLE

namespace NHLL
{
    enum class MemoryClassification
    {
        FIXED, DYNAMIC
    };

    enum class UnderlyingPrimitives
    {
        INT, REAL, STR, MIXED
    };

    enum class DatumSize
    {
        EIGHT, SIXTEEN, THIRTYTWO, SIXTYFOUR
    };

    class BaseVariable
    {
    public: 
        BaseVariable(uint64_t addr, DatumSize size, std::string name, std::string def, UnderlyingPrimitives underlying_prims) : 
            address(addr), datumSize(size), name(name), definition(def), underlying_prims(underlying_prims){}
        uint64_t address;
        DatumSize datumSize;
        std::string name;
        std::string definition;
        UnderlyingPrimitives underlying_prims;
    };

    class Nhll_Integer: public BaseVariable
    {
    public:
        Nhll_Integer(uint64_t address, std::string name, 
                     std::string definition, DatumSize datumSize=DatumSize::SIXTYFOUR) : 
                        BaseVariable(address, datumSize, name, definition, UnderlyingPrimitives::INT)
        {
        }
    };
    
    class Nhll_Real : public BaseVariable
    {
    public:
        Nhll_Real(uint64_t address, std::string name, 
                     std::string definition, DatumSize datumSize=DatumSize::SIXTYFOUR) : 
                        BaseVariable(address, datumSize, name, definition, UnderlyingPrimitives::REAL)
        {
        }
    };

    class Nhll_String : public BaseVariable
    {
    public:
        Nhll_String(uint64_t address, std::string name, 
                     std::string definition, DatumSize datumSize=DatumSize::SIXTYFOUR) : 
                        BaseVariable(address, datumSize, name, definition, UnderlyingPrimitives::STR)
        {
        }
    };
}

#endif