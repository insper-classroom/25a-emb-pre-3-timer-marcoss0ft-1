#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_press = 0;
volatile int flag_release = 0;
volatile int long_press_detected = 0;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    if (gpio_get(BTN_PIN_R) == 0) { // Se o botão ainda estiver pressionado
        long_press_detected = 1; // Marca que o botão foi pressionado por tempo suficiente
    }
    return 0;
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) { // Botão pressionado
        flag_press = 1;
    } else if (events == GPIO_IRQ_EDGE_RISE) { // Botão solto
        flag_release = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); // LED começa apagado

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(
        BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    while (true) {
        if (flag_press) {
            flag_press = 0;
            long_press_detected = 0;
            add_alarm_in_ms(500, alarm_callback, NULL, false); // Inicia alarme para 500ms
        }

        if (flag_release) {
            flag_release = 0;
            if (long_press_detected) {
                gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); // Alterna o LED
            }
        }
    }
}
