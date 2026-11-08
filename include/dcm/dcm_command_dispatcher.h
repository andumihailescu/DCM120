#pragma once

struct ECUContext;
struct can_frame;

namespace DCMCommandDispatcher
{
    // Returns true only when a structurally valid DCM120 command was
    // recognized and its handler was invoked.
    bool dispatch(
        const can_frame &frame,
        ECUContext *context);
}
