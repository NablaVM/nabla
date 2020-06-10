#ifndef DEL_SYSTEM_SETTINGS_HPP
#define DEL_SYSTEM_SETTINGS_HPP

namespace DEL
{
namespace SETTINGS
{
    //  Settings for reserved spaces in the global stack
    //
    //
    static constexpr int GS_FRAME_OFFSET_RESERVE  = 1;
    static constexpr int GS_FUNC_PARAM_RESERVE    = 10;
    static constexpr int GS_GENERAL_RESERVE       = 5;

    //  To avoid magic numbers, we say why use 8
    //
    static constexpr int SYSTEM_WORD_SIZE_BYTES   = 8;

    //  The index into GS where program data will start 
    //
    static constexpr int GS_INDEX_PROGRAM_SPACE   = SYSTEM_WORD_SIZE_BYTES * (GS_FRAME_OFFSET_RESERVE + GS_FUNC_PARAM_RESERVE + GS_GENERAL_RESERVE);
}
}

#endif