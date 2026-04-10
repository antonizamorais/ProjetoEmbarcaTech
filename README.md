# ProjetoEmbarcaTech
# AGROMIMU - Sistema de Monitoramento Inercial para Cargas Vivas
O AGROMIMU é uma solução de telemetria IoT voltada para o bem-estar animal no agronegócio. Utilizando o microcontrolador RP2040 (Pico W) na placa BitDogLab, o sistema monitora em tempo real as forças inerciais (força G) durante o transporte de cargas vivas, detectando manobras bruscas que possam causar estresse ou lesões nos animais.

# Objetivo do Projeto
Fornecer uma ferramenta de auditoria e auxílio à condução que:
Detecte freadas, acelerações e curvas que excedam os limites de segurança biológica.
Notifique o condutor localmente via alertas visuais (LED/OLED).
Transmita dados de telemetria via Wi-Fi para monitoramento remoto.
Garanta a integridade da carga e a conformidade com normas de bem-estar animal.

#Tecnologias e Periféricos
* Linguagem: C (Pico SDK)
* Microcontrolador: Raspberry Pi Pico W (RP2040)
* Sensor: MPU6050 (Acelerômetro/Giroscópio via I2C)
* Display: OLED 128x64 (via I2C com suporte a DMA)
* Conectividade: Wi-Fi (Protocolo HTTP/MQTT)
* Feedback: LED RGB e Interface UART

# Instruções de Instalação e Execução
1. Pré-requisitos e Dependências
Para compilar e enviar o código para a BitDogLab, você precisará de:
* Raspberry Pi Pico SDK instalado e configurado em seu ambiente.
* CMake (versão 3.13 ou superior).
* GNU Arm Embedded Toolchain (compilador arm-none-eabi-gcc).
* VSCode (recomendado com a extensão Raspberry Pi Pico).

3. Clonando o Repositório
git clone https://github.com/antonizamorais/ProjetoEmbarcaTech.git
cd ProjetoEmbarcaTech

5. Configuração do Wi-Fi
Crie um arquivo chamado wifi_secrets.h na pasta include/ para manter suas credenciais seguras:
#define WIFI_SSID "NOME_DA_REDE"
#define WIFI_PASSWORD "SENHA_DA_REDE"

4. Compilação
mkdir build
cd build
cmake ..
make

6. Execução
Conecte a BitDogLab ao seu computador via USB mantendo o botão BOOTSEL pressionado (ou use o modo de depuração via VS Code).
Arraste o arquivo ProjetoEmbarcaTech.uf2 gerado na pasta build para o drive da placa.

O sistema iniciará a calibração automática. Mantenha a placa em uma superfície plana.
