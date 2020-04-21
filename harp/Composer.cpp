#include "Composer.hpp"

#include "InsManifest.hpp"

namespace HARP
{

    Composer::Composer()
    {

    }

    bool Composer::populate(std::vector<uint8_t> bytes)
    {
        enum class LoadStatus
        {
            IDLE, EXPECT_CONST, EXPECT_FUNC, STOP
        };

        LoadStatus ls;

        uint64_t entryAddress = 0;
        uint64_t numberOfConstants = 0;

        for(uint64_t i = 0; i < bytes.size(); i++)
        {

            if(ls == LoadStatus::IDLE)
            {
                if(bytes[i] == MANIFEST::INS_SEG_CONST)
                {
                    ls = LoadStatus::EXPECT_CONST;

                    
                    for(int n = 7; n >= 0; n--)   
                    {
                        if(i + 1 < bytes.size())
                        {
                            i++;
                            numberOfConstants |= (uint64_t)bytes[i] << (n * 8);
                        }
                        else
                        {
                            return false;
                        }
                    }

                    if(numberOfConstants == 0)
                    {
                        ls = LoadStatus::IDLE;
                    }
                }
                else if(bytes[i] == MANIFEST::INS_SEG_FUNC)
                {
                    ls = LoadStatus::EXPECT_FUNC;

                    //  Read in the entry address expected 
                    //
                    for(int n = 7; n >= 0; n--)   
                    {
                        uint8_t tmp = 0;
                        if(i + i < bytes.size())
                        {
                            i++;
                            expectedEntryAddress |= (uint64_t)bytes[i] << (n * 8);
                        }
                        else
                        {
                            return VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION;
                        }
                    }
                }
            }
        }

    }


}