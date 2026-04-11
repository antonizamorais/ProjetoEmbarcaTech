#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define LED_RED_PIN   13
#define LED_GREEN_PIN 11
#define BUTTON_A_PIN  5
#define BUTTON_B_PIN  6 
#define BUZZER_A_PIN  21
#define JOY_X_PIN     26
#define JOY_Y_PIN     27
#define I2C_PORT i2c1
#define DISPLAY_ADDR 0x3C

static volatile bool alerta_ativo = false;
static volatile int contador_incidentes = 0;
static volatile uint32_t last_time = 0;

// --- FONTE 8x6 ---
const uint8_t micro_font[] = {
    0x7c, 0x12, 0x11, 0x12, 0x7c, 0x00, // A (0)
    0x3e, 0x41, 0x49, 0x49, 0x7a, 0x00, // G (1)
    0x7f, 0x09, 0x19, 0x29, 0x46, 0x00, // R (2)
    0x3e, 0x41, 0x41, 0x41, 0x3e, 0x00, // O (3)
    0x7f, 0x02, 0x0c, 0x02, 0x7f, 0x00, // M (4)
    0x00, 0x41, 0x7f, 0x41, 0x00, 0x00, // I (5)
    0x3f, 0x40, 0x40, 0x40, 0x3f, 0x00, // U (6)
    0x7f, 0x40, 0x40, 0x40, 0x40, 0x00, // L (7)
    0x01, 0x01, 0x7f, 0x01, 0x01, 0x00, // T (8)
    0x7f, 0x49, 0x49, 0x49, 0x41, 0x00, // E (9)
    0x1f, 0x20, 0x40, 0x20, 0x1f, 0x00, // V (10)
    0x7f, 0x08, 0x14, 0x22, 0x41, 0x00, // K (11)
    0x7f, 0x49, 0x49, 0x49, 0x36, 0x00, // B (12)
    0x3e, 0x41, 0x41, 0x41, 0x22, 0x00, // C (13)
    0x7f, 0x04, 0x08, 0x10, 0x7f, 0x00, // N (14)
    0x46, 0x49, 0x49, 0x49, 0x31, 0x00  // S (15)
};

void oled_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, buf, 2, false);
}

void oled_set_cursor(uint8_t page, uint8_t col) {
    oled_command(0xB0 + page);
    oled_command(0x00 + (col & 0x0F));
    oled_command(0x10 + (col >> 4));
}

void oled_write_char(uint8_t idx) {
    uint8_t buf[7] = {0x40};
    for(int i=0; i<6; i++) buf[i+1] = micro_font[(idx * 6) + i];
    i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, buf, 7, false);
}

void oled_clear_all() {
    for (int p = 0; p < 8; p++) {
        oled_set_cursor(p, 0);
        uint8_t clean[129] = {0x40}; 
        i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, clean, 129, false);
    }
}

void oled_clear_line(uint8_t page) {
    oled_set_cursor(page, 0);
    uint8_t clean[129] = {0x40}; 
    i2c_write_blocking(I2C_PORT, DISPLAY_ADDR, clean, 129, false);
}

void som_alerta() {
    for(int i=0; i<30; i++) {
        gpio_put(BUZZER_A_PIN, 1); sleep_us(400);
        gpio_put(BUZZER_A_PIN, 0); sleep_us(400);
    }
}

void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_time > 200) {
        last_time = current_time;
        alerta_ativo = false; 
    }
}

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14); gpio_pull_up(15);
    adc_init(); adc_gpio_init(JOY_X_PIN); adc_gpio_init(JOY_Y_PIN);
    
    gpio_init(LED_RED_PIN); gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN); gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(BUZZER_A_PIN); gpio_set_dir(BUZZER_A_PIN, GPIO_OUT);
    gpio_init(BUTTON_A_PIN); gpio_set_dir(BUTTON_A_PIN, GPIO_IN); gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN); gpio_set_dir(BUTTON_B_PIN, GPIO_IN); gpio_pull_up(BUTTON_B_PIN);
    
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    uint8_t init_cmds[] = {0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF};
    for (int i = 0; i < sizeof(init_cmds); i++) oled_command(init_cmds[i]);
    oled_clear_all();

    while (1) {
        adc_select_input(0); int x = adc_read();
        adc_select_input(1); int y = adc_read();
        int desvio = abs(y - 2048) > abs(x - 2048) ? abs(y - 2048) : abs(x - 2048);

        if (desvio > 1600 && !alerta_ativo) {
            alerta_ativo = true;
            contador_incidentes++;
        }

        if (!gpio_get(BUTTON_B_PIN)) {
            // --- TELA DE AUDITORIA (BOTÃO B PRESSIONADO) ---
            oled_clear_all();
            oled_set_cursor(3, 30);
            uint8_t nota_label[] = {14, 3, 8, 0}; // N O T A
            for(int i=0; i<4; i++) oled_write_char(nota_label[i]);
            
            oled_write_char(9); // Espaço (Placeholder)
            
            // Exibe a Letra da Nota
            if (contador_incidentes == 0) oled_write_char(0);      // A
            else if (contador_incidentes <= 3) oled_write_char(12); // B
            else oled_write_char(13);                             // C
            
            sleep_ms(200); // Debounce visual
        } 
        else {
            // --- TELA DE OPERAÇÃO NORMAL ---
            // Linha 1: Nome do Projeto
            oled_set_cursor(1, 40);
            uint8_t n[] = {0, 1, 2, 3, 4, 5, 4, 6}; // AGROMIMU
            for(int i=0; i<8; i++) oled_write_char(n[i]);

            // Linha 5: Status
            oled_clear_line(5);
            oled_set_cursor(5, 45);

            if (alerta_ativo) {
                uint8_t al[] = {0, 7, 9, 2, 8, 0}; // A L E R T A
                for(int i=0; i<6; i++) oled_write_char(al[i]);
                gpio_put(LED_RED_PIN, 1); gpio_put(LED_GREEN_PIN, 0);
                oled_command(0xA7); som_alerta(); sleep_ms(50); oled_command(0xA6);
            } else if (desvio > 600) {
                uint8_t av[] = {0, 10, 5, 15, 3}; // A V I S O
                for(int i=0; i<5; i++) oled_write_char(av[i]);
                gpio_put(LED_RED_PIN, 1); gpio_put(LED_GREEN_PIN, 1);
            } else {
                oled_write_char(3); oled_write_char(11); // O K
                gpio_put(LED_RED_PIN, 0); gpio_put(LED_GREEN_PIN, 1);
            }
        }
        sleep_ms(100);
    }
}