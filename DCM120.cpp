#include "app/app.h"
#include "config/dcm120_config.h"

#include "hardware/watchdog.h"
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();

    App app;

    if (!app.init())
    {
        // App::init() leaves throttle neutral whenever initialization fails.
        while (true)
        {
            tight_loop_contents();
        }
    }

    // Backstop the cooperative 100 ms signal timeout. A stalled main loop
    // resets the PWM peripheral instead of holding the last commands forever.
    // watchdog_enable(DCM120Config::Safety::WatchdogTimeoutMs, true);

    while (true)
    {
        app.update();
        // watchdog_update();
        //  Keep polling responsive without making the GPIO ISR perform SPI.
        //  A later high-load design can wire MCP2515 INT for stronger RX bounds.
        sleep_us(100);
    }
}
