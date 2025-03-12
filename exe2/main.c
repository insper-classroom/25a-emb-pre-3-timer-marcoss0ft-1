#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

volatile int timer0_active = 0;
volatile int timer1_active = 0;

repeating_timer_t timer_0;
repeating_timer_t timer_1;

bool timer_0_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
    return true; // Mantém o timer repetindo
}

bool timer_1_callback(repeating_timer_t *rt) {
    gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
    return true; // Mantém o timer repetindo
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) { // Detecta a borda de descida
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0); // LED começa apagado

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0); // LED começa apagado

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    // Configura interrupções para os dois botões
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        if (flag_r) {
            flag_r = 0;
            if (!timer0_active) {
                if (add_repeating_timer_ms(500, timer_0_callback, NULL, &timer_0)) {
                    timer0_active = 1;
                }
            } else {
                cancel_repeating_timer(&timer_0);
                timer0_active = 0;
                gpio_put(LED_PIN_R, 0); // Garante que o LED termine apagado
            }
        }

        if (flag_g) {
            flag_g = 0;
            if (!timer1_active) {
                if (add_repeating_timer_ms(250, timer_1_callback, NULL, &timer_1)) {
                    timer1_active = 1;
                }
            } else {
                cancel_repeating_timer(&timer_1); // Corrigido para cancelar o timer certo
                timer1_active = 0;
                gpio_put(LED_PIN_G, 0); // Garante que o LED termine apagado
            }
        }
    }
}
