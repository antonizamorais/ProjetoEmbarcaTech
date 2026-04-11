#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

// --- DEFINIÇÕES DE HARDWARE ---
#define LED_GREEN_PIN 11
#define BUTTON_A_PIN  5
#define I2C_PORT i2c1
#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define DISPLAY_ADDR 0x3C

// --- VARIÁVEIS GLOBAIS ---
static volatile uint32_t last_time = 0; 

// --- MICRO-FONTE 8x6 (Mapeamento de pixels) ---
const uint8_t micro_font[] = {
    0x7c, 0x12, 0x11, 0x12, 0x7c, 0x00, // A (0)
    0x3e, 0x41, 0x49, 0x49, 0x7a, 0x00, // G (1)
    0x7f, 0x09, 0x19, 0x29, 0x46, 0x00, // R (2)
    0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, // O (3)
    0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00, // M (4)
    0x00, 0x41, 0x7f, 0x41, 0x00, 0x00, // I (5)
    0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00  // U (6)
};

// --- FUNÇÕES DO DISPLAY ---
void oled_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, buf, 2, false);
}

void oled_init() {
    uint8_t init_cmds[] = {0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF};
    for (int i = 0; i < sizeof(init_cmds); i++) oled_command(init_cmds[i]);
}

void oled_set_cursor(uint8_t page, uint8_t column) {
    oled_command(0xB0 + page);
    oled_command(0x00 + (column & 0x0F));
    oled_command(0x10 + (column >> 4));
}

void oled_clear() {
    for (int p = 0; p < 8; p++) {
        oled_set_cursor(p, 0);
        uint8_t clean[129] = {0x40}; 
        i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, clean, 129, false);
    }
}

void oled_write_char(uint8_t char_index) {
    uint8_t buf[7] = {0x40};
    for(int i=0; i<6; i++) buf[i+1] = micro_font[(char_index * 6) + i];
    i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, buf, 7, false);
}

// --- INTERRUPÇÃO DO BOTÃO (LED TOGGLE) ---
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_time > 200) {
        last_time = current_time;
        // Inverte o estado do LED verde
        gpio_put(LED_GREEN_PIN, !gpio_get(LED_GREEN_PIN));
        printf("Botao A pressionado. LED alterado!\n");
    }
}

int main() {
    stdio_init_all();

    // I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // LED
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_put(LED_GREEN_PIN, 0);

    // Botão
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Display
    oled_init();
    oled_clear(); // Garante nitidez
    
    // Escreve AGROMIMU
    uint8_t nome[] = {0, 1, 2, 3, 4, 5, 4, 6}; 
    oled_set_cursor(3, 35);
    for(int i=0; i<8; i++) oled_write_char(nome[i]);

    printf("AGROMIMU: V1.0 Estavel Iniciada.\n");

    while (1) {
        tight_loop_contents();
    }
    return 0;
}