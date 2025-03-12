#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile uint64_t press_time = 0;
volatile int flag_release = 0;

void btn_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_PIN_R) {
        if (events == GPIO_IRQ_EDGE_FALL) { // Botão pressionado
            press_time = to_ms_since_boot(get_absolute_time());
        } else if (events == GPIO_IRQ_EDGE_RISE) { // Botão solto
            flag_release = 1;
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); 

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        if (flag_release) {
            flag_release = 0;
            uint64_t elapsed_time = to_ms_since_boot(get_absolute_time()) - press_time;
            if (elapsed_time >= 500) {
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
            }
        }
    }
}
