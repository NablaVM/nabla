#ifndef DEL_TRACKER_HPP
#define DEL_TRACKER_HPP

#include <stdint.h>

namespace DEL
{
    //! \brief Object to track where we are in the process of compiling 
    class Tracker
    {
    public:
        //! \brief Construct Tracker
        Tracker();

        //! \brief Deconstruct tracker
        ~Tracker();

        //! \brief Incriment the number of lines tracked
        void inc_lines_tracked();

        //! \brief Retrieve the number of lines tracked
        //! \retval uint64_t Representing the number of lines tracked (0 indexed)
        uint64_t get_lines_tracked() const;

    private:
        uint64_t lines_tracked; // Not necissarily where we are at in analyzing
    };
}

#endif