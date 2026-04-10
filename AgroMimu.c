#include <stdio.h>
#include "pico/stdlib.h"

// Definições de pinos diretamente no código para teste inicial
#define LED_GREEN_PIN 11

int main() {
    stdio_init_all();

    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    while (1) {
        gpio_put(LED_GREEN_PIN, 1);
        printf("Sistema AGROMIMU: Operação Simples\n");
        sleep_ms(500);

        gpio_put(LED_GREEN_PIN, 0);
        sleep_ms(500);
    }
    return 0;
}