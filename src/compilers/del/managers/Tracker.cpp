#include "Tracker.hpp"

namespace DEL
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Tracker::Tracker() : lines_tracked(0)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Tracker::~Tracker()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Tracker::inc_lines_tracked()
    {
        lines_tracked++;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    uint64_t Tracker::get_lines_tracked() const
    {
        return lines_tracked;
    }
}