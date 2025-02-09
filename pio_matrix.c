/*
 * Código Desenvolvido por: Gilmaiane Porto Silva
 * Funcionalidades do Projeto:
 * 1. Modificação da Biblioteca font.h:
 *    - Adição de caracteres minúsculos à biblioteca font.h.
 *
 * 2. Entrada de caracteres via PC:
 *    - Utilize o Serial Monitor do VS Code para digitar caracteres.
 *    - Cada caractere digitado no Serial Monitor será exibido no display SSD1306.
 *    - Apenas um caractere será enviado de cada vez, sem necessidade de suportar strings completas.
 *    - Quando um número entre 0 e 9 for digitado, um símbolo correspondente ao número será exibido
 *      na matriz 5x5 WS2812.
 *
 * 3. Interação com o Botão A:
 *    - Pressionar o Botão A alterna o estado do LED RGB Verde (ligado/desligado).
 *    - A operação é registrada de duas formas:
 *      - Uma mensagem sobre o estado do LED é exibida no display SSD1306.
 *      - Um texto descritivo sobre a operação é enviado ao Serial Monitor.
 *
 * 4. Interação com o Botão B:
 *    - Pressionar o Botão B alterna o estado do LED RGB Azul (ligado/desligado).
 *    - A operação é registrada de duas formas:
 *      - Uma mensagem sobre o estado do LED é exibida no display SSD1306.
 *      - Um texto descritivo sobre a operação é enviado ao Serial Monitor.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"

//Arquivo display
#include "ssd1306.h"
#include "font.h"
#include "hardware/i2c.h"

// Arquivo .pio
#include "pio_matrix.pio.h"
#include "led_matrix.h"
#include "reboot.h"
#include "buzzer.h"

const uint PIN_BOTAO_A = 5;  // Botão A
const uint PIN_BOTAO_B = 6;  // Botão B


// Número de LEDs
#define NUM_PIXELS 25

// Pino de saída
#define OUT_PIN 7

// Tempo do frame
#define FPS 200

//Pino de saída e frequência do buzzer
#define BUZZER 10 
#define FREQUENCY 500

//i2c
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

// Definição dos LEDs
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12
#define tempo 500

uint sm;  // Variável para armazenar o número da máquina de estados


// Variáveis globais para controle do tempo (debounce)
uint32_t last_time_A = 0;
uint32_t last_time_B = 0;

// Estado dos LEDs
bool led_green_on = false;
bool led_blue_on = false;

ssd1306_t ssd; // Inicializa a estrutura do display
float r = 0.1, g = 0.0, b = 0.001; // Inicialização das variáveis RGB

uint32_t matrix_rgb_conversor(double b, double r, double g) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}
// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);


void inicializacao() {
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);

    // Configura os botões como entrada com pull-up interno
    gpio_init(PIN_BOTAO_A);
    gpio_set_dir(PIN_BOTAO_A, GPIO_IN);
    gpio_pull_up(PIN_BOTAO_A);

    gpio_init(PIN_BOTAO_B);
    gpio_set_dir(PIN_BOTAO_B, GPIO_IN);
    gpio_pull_up(PIN_BOTAO_B);

    // Configura a interrupção com callback para os botões A e B
    gpio_set_irq_enabled_with_callback(PIN_BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PIN_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
   
}

void exibir_no_display(char c) {
    ssd1306_fill(&ssd, 0); // Limpa o display (cor de fundo preta)
    ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
    
    char texto[2] = {c, '\0'}; // Converte o caractere para string (array de 2 posições: caractere + null terminator)
    
    ssd1306_draw_string(&ssd, texto, 53, 30); // Exibe o caractere na posição (10,20)
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(2000); // Mantém a exibição por 2 segundos
}

int main() {

    stdio_init_all();
    inicializacao();
    setup_led_matrix();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
 
    
    // Atualiza o conteúdo do display com animação padrão
    bool cor = true;
    cor = !cor;
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "Bem Vindo", 30, 10); // Desenha uma string
    ssd1306_draw_string(&ssd, "Embarcatech", 26, 30); // Desenha uma string
    ssd1306_draw_string(&ssd, "Gilmaiane", 30, 48); // Desenha uma string 
    ssd1306_send_data(&ssd); // Atualiza o display
    sleep_ms(1000);

    while (true) {

        if (stdio_usb_connected()) { // Certifica-se de que o USB está conectado
            sleep_ms(10);
            char c;
            if (scanf("%c", &c) == 1) { // Lê um caractere da entrada padrão
                printf("Recebido: '%c'\n", c);

        // Exibe qualquer caractere no display OLED
                 exibir_no_display(c);
        
        // Seleciona a animação correta para a matriz de LEDs
        switch (c) {
                case '0': desenho_pio1(desenho0, pio, sm, r, g, b); break;
                case '1': desenho_pio1(desenho1, pio, sm, r, g, b); break;
                case '2': desenho_pio1(desenho2, pio, sm, r, g, b); break;
                case '3': desenho_pio1(desenho3, pio, sm, r, g, b); break;
                case '4': desenho_pio1(desenho4, pio, sm, r, g, b); break;
                case '5': desenho_pio1(desenho5, pio, sm, r, g, b); break;
                case '6': desenho_pio1(desenho6, pio, sm, r, g, b); break;
                case '7': desenho_pio1(desenho7, pio, sm, r, g, b); break;
                case '8': desenho_pio1(desenho8, pio, sm, r, g, b); break;
                case '9': desenho_pio1(desenho9, pio, sm, r, g, b); break;
                case 'a': desenho_pio1(apagaleds, pio, sm, r, g, b); printf("Apaga LEDs\n"); break;
                case 'c': desenho_pio1(coracao, pio, sm, r, g, b); printf("coração\n"); break;
                case '#': start_buzzer(BUZZER, FREQUENCY); animacao(1, 10, xadrez, sm); stop_buzzer(BUZZER); printf("Animação\n");break; 
                case '*': printf("Reiniciando...\n"); reboot_device();break;//Função reboot
                default: printf("Caractere inválido: '%c'\n", c); break; // Outros caracteres não afetam a matriz de LEDs

                }
            }
        }
    }
    return 0;
    }

   // Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());  // Tempo atual

    // Se o evento for do botão A
    if (gpio == PIN_BOTAO_A && (current_time - last_time_A > 500)) {  
        last_time_A = current_time;  // Atualiza o tempo do último evento do botão A
        
        // Alterna o estado do LED verde
        led_green_on = !led_green_on;
        gpio_put(LED_GREEN, led_green_on ? 1 : 0);  

        //Configuração do Display
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
        char mensagem[30];
        snprintf(mensagem, sizeof(mensagem), "LED VERDE %s", led_green_on ? "\nON" : "\nOFF");
        ssd1306_draw_string(&ssd, mensagem, 10, 30);
        ssd1306_send_data(&ssd);
        // Exibe no Serial Monitor
        printf("Botão A pressionado! Estado do LED Verde: %s\n", led_green_on ? "Ligado" : "Desligado");
    }

    // Se o evento for do botão B
    if (gpio == PIN_BOTAO_B && (current_time - last_time_B > 500)) {  
        last_time_B = current_time;

        // Alterna o estado do LED azul
        led_blue_on = !led_blue_on;
        gpio_put(LED_BLUE, led_blue_on ? 1 : 0);
        //Configuração do Display
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, 3, 3, 122, 58, 1, 0); // Desenha um retângulo
        char mensagem[30];
        snprintf(mensagem, sizeof(mensagem), "LED AZUL %s", led_blue_on ? "\nON" : "\nOFF");
        ssd1306_draw_string(&ssd, mensagem, 10, 30);
        ssd1306_send_data(&ssd);
        printf("Botão B pressionado! Estado do LED Azul: %s\n", led_blue_on ? "Ligado" : "Desligado");
    }
}
