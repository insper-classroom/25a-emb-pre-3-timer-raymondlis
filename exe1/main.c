#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int g_timer_flag = 0;
volatile bool timer_running = false;
repeating_timer_t timer;

bool timer_callback(repeating_timer_t *rt) {
    g_timer_flag = 1;
    return true;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (timer_running) {
            cancel_repeating_timer(&timer);
            gpio_put(LED_PIN_R, 0);
        } else {
            add_repeating_timer_ms(500, timer_callback, NULL, &timer);
        }
        timer_running = !timer_running;
    }
}

int main() {
    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, 
        &btn_callback);
    

    while (true) {
        if (g_timer_flag) {
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
            g_timer_flag = 0;
        }
    }
}
