#pragma once

class CANManager;
class DriveController;

struct ECUContext
{
    CANManager *can = nullptr;
    DriveController *drive = nullptr;
};
