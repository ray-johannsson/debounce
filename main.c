#include <stdio.h>
#include <signal.h>
#include <pigpio.h>

#define BUTTON_PIN 17
#define DEBOUNCE_DELAY_MS 50

volatile int running = 1;

void handle_sigint(int sig)
{
    running = 0;
}

void handle_sigterm(int sig)
{
    gpioTerminate();
    exit(0);
}

int main()
{
    if (gpioInitialise() < 0) {
        printf("Failed to initialize pigpio\n");
        return 1;
    }

    gpioSetMode(BUTTON_PIN, PI_INPUT);

    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigterm);

    int button_state = 0;
    int last_button_state = 0;

    while (running) {
        int new_button_state = gpioRead(BUTTON_PIN);

        if (new_button_state != last_button_state) {
            gpioDelay(DEBOUNCE_DELAY_MS * 1000);

            new_button_state = gpioRead(BUTTON_PIN);

            if (new_button_state != last_button_state) {
                button_state = new_button_state;
                printf("Button state: %d\n", button_state);
            }
        }

        last_button_state = new_button_state;
    }

    gpioTerminate();

    return 0;
}
