#pragma once

#include <cstdint>

#include "can/can_manager.h"
#include "drive/drive_controller.h"
#include "ecu_context/ecu_context.h"
#include "input/pwm_input.h"

class App
{
public:
    App();

    bool init();
    void update();
    void setDriveMode(DriveMode mode);
    DriveMode driveMode() const;
    void setMotorDirection(MotorDirection direction);
    MotorDirection motorDirection() const;

private:
    void serviceCan();

    PwmInput speedInput;
    DriveController drive;
    CANManager can;
    ECUContext context;

    std::uint32_t lastCanInitAttemptUs = 0;
    bool canReady = false;
};
