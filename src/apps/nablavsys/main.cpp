#include <iostream>

#include "VSysMachine.hpp"          // raw virtual machine
#include "VSysLoadableMachine.hpp"  // Binary-loadable machine


int main(int argc, char **argv)
{

    std::cout << "Nabla with new vsys architecture" << std::endl;

    NABLA::VSYS::LoadableMachine virtualMachine;

    return 0;
}