#include "StepperMotorFsmAction.hpp"
#include "StepperMotorFsmEntryExit.hpp"

#include <hsm/hsm.h>

int main() {
    StepperMotor sm;
    cout << "---------------------------------------------------------------------" << endl;
    {
        using namespace action;

        hsm::sm<StepperMotorSm, StepperMotor> fsm{sm};

        std::cout << "----- process event  --> enable " << std::endl;
        fsm.process_event(enable{});

        std::cout << "----- process event  --> rotate_step " << std::endl;
        fsm.process_event(rotate_step{
                .steps = 3,
        });

        for (int i = 0; i < 5; ++i) {
            std::cout << "----- process event  --> step_done " << std::endl;
            fsm.process_event(step_done{});
        }

        std::cout << "----- process event  --> disable " << std::endl;
        fsm.process_event(disable{});
    }
    cout << "---------------------------------------------------------------------" << endl;
    {
        using namespace entryexit;
        hsm::sm<StepperMotorSmEntryExit, StepperMotor> fsm{sm};

        std::cout << "----- process event  --> enable " << std::endl;
        fsm.process_event(enable{});

        std::cout << "----- process event  --> rotate_step " << std::endl;
        fsm.process_event(rotate_step{
                .steps = 3,
        });

        for (int i = 0; i < 5; ++i) {
            std::cout << "----- process event  --> step_done " << std::endl;
            fsm.process_event(step_done{});
        }

        std::cout << "----- process event  --> disable " << std::endl;
        fsm.process_event(disable{});
    }
}
