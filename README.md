# ProjetoEmbarcaTech
# AGROMIMU - Sistema de Monitoramento Inercial para Cargas Vivas
O AGROMIMU é uma solução de telemetria IoT voltada para o bem-estar animal no agronegócio. Utilizando o microcontrolador RP2040 (Pico W) na placa BitDogLab, o sistema monitora em tempo real as forças inerciais (força G) durante o transporte de cargas vivas, detectando manobras bruscas que possam causar estresse ou lesões nos animais.

# Objetivo do Projeto
Fornecer uma ferramenta de auditoria e auxílio à condução que:
Detecte freadas, acelerações e curvas que excedam os limites de segurança biológica.
Notifique o condutor localmente via alertas visuais (LED/OLED).
Transmita dados de telemetria via Wi-Fi para monitoramento remoto.
Garanta a integridade da carga e a conformidade com normas de bem-estar animal.

# Destaques Técnicos do Projeto:
Separação de Privilégios de Visualização: A interface de operação (status em tempo real) é distinta da interface de auditoria (Rating de condução), acessível apenas via hardware (Botão B).

Tratamento de Interrupções (IRQ): Uso de handlers para reconhecimento de alertas (Botão A), otimizando o ciclo de processamento do RP2040.

Lógica de Auditoria: Implementação de um contador de eventos críticos para gerar uma nota de desempenho (A, B ou C) baseada no bem-estar animal.

# Tecnologias e Periféricos (Atualizado)
* Linguagem: C (Pico SDK).
* Microcontrolador: Raspberry Pi Pico W (RP2040).
* Sensores e Entrada:
    * Joystick Analógico (ADC): Utilizado para a simulação de forças inerciais (eixos X e Y) e detecção de incidentes de condução.
    * Botão A (GPIO): Interrupção de hardware para reconhecimento (Acknowledge) de alertas críticos.
    * Botão B (GPIO): Interface de consulta para exibição do Rating de Auditoria.
* Display: OLED 128x64 (via I2C) com atualização dinâmica de status e telemetria de auditoria.
* Feedback Visual e Sonoro:
    * LED RGB: Indicação visual de status (Verde: OK | Amarelo: Aviso | Vermelho: Alerta Crítico).
    * Buzzer Piezoelétrico: Alerta sonoro de alta intensidade para eventos de impacto excessivo.
* Conectividade e Auditoria:
    * Edge Computing: Processamento local de dados para classificação de desempenho em tempo real.
    * Rating de Condução: Algoritmo interno para registro de incidentes e atribuição de nota (A, B ou C).
      
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
