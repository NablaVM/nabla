#ifndef NABLA_APP_EXEC_HPP
#define NABLA_APP_EXEC_HPP

#include <string>

#include <libnabla/VSysLoadableMachine.hpp>

namespace APP
{
    class NablaExec
    {
    public:
        static constexpr double DEFAULT_GC_CYCLE_SEC = 30.0;

        NablaExec(double gc_cycle_sec=DEFAULT_GC_CYCLE_SEC);

        ~NablaExec();

        int execFile(std::string file);

    private:
        double gc_cycle;
        NABLA::VSYS::LoadableMachine * virtualMachine;

    };
}

#endif