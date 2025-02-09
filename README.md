# Projeto: Controle de Display OLED e Matriz de LEDs WS2812 com RP2040

## Desenvolvido por: Gilmaiane Porto Silva

## Descrição
Este projeto permite a interação entre um display OLED SSD1306 e uma matriz 5x5 de LEDs WS2812, controlados por um microcontrolador RP2040. O sistema recebe caracteres via monitor serial e exibe-os no display, além de exibir animações na matriz de LEDs para números e caracteres especiais.

## Funcionalidades

### 1. Modificação da Biblioteca font.h
- Adição de caracteres minúsculos à biblioteca `font.h` para melhor compatibilidade com o display OLED.

### 2. Entrada de Caracteres e Strings via PC
- O monitor serial do VS Code permite a entrada de caracteres individuais ou strings completas.
- A string digitada é exibida no display OLED apenas quando o usuário pressiona **Enter** (`\n`).
- Quando um número (0-9) é digitado, um símbolo correspondente é mostrado na matriz de LEDs WS2812.
- Caracteres especiais (`*` e `#`) possuem animações específicas na matriz.

### 3. Interação com os Botões
#### Botão A:
- Alterna o estado do LED RGB Verde (ligado/desligado).
- Exibe o estado atualizado no display OLED.
- Envia uma mensagem descritiva ao monitor serial.

#### Botão B:
- Alterna o estado do LED RGB Azul (ligado/desligado).
- Exibe o estado atualizado no display OLED.
- Envia uma mensagem descritiva ao monitor serial.

### 4. Controle da Matriz de LEDs
- Exibição de números de 0 a 9 com efeitos visuais.
- Animações para caracteres especiais (`*` e `#`).

### 5. Controle do Buzzer
- Emissão de som para feedback ao pressionar botões ou inserir caracteres especiais.

## Hardware Utilizado
- Microcontrolador RP2040 (Placa BitDogLab)
- Display OLED SSD1306 (I2C)
- Matriz de LEDs WS2812 (5x5)
- LEDs RGB individuais (Vermelho, Verde, Azul)
- Botões de entrada (A e B)
- Buzzer

## Bibliotecas Utilizadas
- `pico/stdlib.h`
- `hardware/i2c.h`
- `ssd1306.h`
- `font.h`
- `led_matrix.h`
- `pio_matrix.pio.h`
- `led_control.h`
- `teclado.h`
- `buzzer.h`

## Configuração de Pinos
- **Botão A:** GPIO 5
- **Botão B:** GPIO 6
- **Saída da Matriz WS2812:** GPIO 7
- **Buzzer:** GPIO 10
- **I2C SDA:** GPIO 14
- **I2C SCL:** GPIO 15
- **LED RGB Vermelho:** GPIO 13
- **LED RGB Verde:** GPIO 11
- **LED RGB Azul:** GPIO 12

## Como Utilizar
1. **Conecte o RP2040** ao seu computador via USB.
2. **Abra o monitor serial** no VS Code.
3. **Digite um caractere ou string completa** no monitor serial e pressione **Enter** para vê-la exibida no display OLED.
4. **Pressione os botões A ou B** para alternar os LEDs RGB e visualizar o status atualizado no display.
5. **Digite um número (0-9)** para acionar uma animação correspondente na matriz de LEDs WS2812.
6. **Digite `*` ou `#`** para exibir animações específicas na matriz.

## Exemplo de Uso
- Entrada no Monitor Serial: `1`
- Display OLED: `1` exibido na tela
- Matriz de LEDs: Exibe animação correspondente ao primeiro caractere da string

## Futuras Melhorias
- Ajuste dinâmico de brilho da matriz de LEDs.
- Integração com sensores adicionais.

## Licença
Este projeto é de código aberto e pode ser utilizado livremente para fins educacionais e experimentais.

