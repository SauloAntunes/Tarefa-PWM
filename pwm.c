#include "pico/stdlib.h" // Bubconjunto central de bibliotecas do SDK Pico
#include "hardware/pwm.h" // Biblioteca para controlar o hardware de PWM

// Definições dos pinos do servomotor e LED e valores para os ângulos de movimento
#define PIN_SERVO 22
#define PIN_LED_B 12
#define ANGULO_180 7864.2
#define ANGULO_90 4816.8225
#define ANGULO_0 1638.375

const uint16_t WRAP_PERIOD = 65535; // Valor máximo do contador - WRAP
const float PWM_DIVISER = 38.2; // Divisor do clock para o PWM
const uint16_t SERVO_STEP = 500; // Passo de incremento/decremento para o duty cycle
uint32_t servo_level = 7000; // Nível inicial do PWM (duty cycle)
bool up_down = true; // Variável para controlar se o nível do servomotor aumenta ou diminui


// Prototipação das rotinas
void pwm_setup();
void movimento_angulo(uint16_t dutyCycle);
void movimento_periodico();


// Rotina principal
int main()
{
    // Inicializar o pino GPIO12 e define a direção como saída
    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);
    
    // Configura o PWM
    pwm_setup();

    // Variável para controlar a lógica dos movimentos
    uint8_t movimento = 0;

    // Loop principal
    while (1)
    {
        // Lógica para o movimento do servomotor
        if (movimento == 0)
        {
            servo_level += SERVO_STEP; // Incrementa o nível do servomotor
            if (servo_level >= ANGULO_180)
            {
                movimento_angulo(ANGULO_180); // Chama a rotina para movimentar o servomotor em 180 graus
                movimento = 1;
            }
        }
        else if (movimento == 1)
        {
            servo_level -= SERVO_STEP; // Decrementa o nível do servomotor
            if (servo_level <= ANGULO_90)
            {
                movimento_angulo(ANGULO_90); // Chama a rotina para movimentar o servomotor em 90 graus
                movimento = 2;
            }
        }
        else if (movimento == 2)
        {
            servo_level -= SERVO_STEP; // Decrementa o nível do servomotor
            if (servo_level <= ANGULO_0)
            {
                movimento_angulo(ANGULO_0); // Chama a rotina para movimentar o servomotor em 0 graus
                movimento = 3;
            }
        }
        else if (movimento == 3)
        {
            movimento_periodico(); // Chama a rotina para movimentar o servomotor entre os ângulos de 0 e 180 graus
        }
    }
}


// Rotina para configurar o módulo PWM do servomotor e do LED
void pwm_setup()
{
    gpio_set_function(PIN_SERVO, GPIO_FUNC_PWM); // Habilitar o pino GPIO como PWM do servomotor
    uint sliceNum = pwm_gpio_to_slice_num(PIN_SERVO); // Obter o canal PWM da GPIO do servomotor
    pwm_config config = pwm_get_default_config(); // Obtem a configuração padrão para o PWM
    pwm_config_set_clkdiv(&config, 38.2); // Define o divisor de clock do PWM
    pwm_init(sliceNum, &config, true); // Inicializa o PWM com as configurações do objeto

    gpio_set_function(PIN_LED_B, GPIO_FUNC_PWM); // Habilitar o pino GPIO como PWM do LED
    sliceNum = pwm_gpio_to_slice_num(PIN_LED_B); // Obter o canal PWM da GPIO do LED
    pwm_init(sliceNum, &config, true); // Inicializa o PWM com as configurações do objeto
}

// Rotina para realizar o movimento baseado no ângulo definido
void movimento_angulo(uint16_t dutyCycle)
{
    pwm_set_gpio_level(PIN_SERVO, dutyCycle); // Define o nível atual do PWM (duty cycle) para o servomotor
    pwm_set_gpio_level(PIN_LED_B, dutyCycle); // Define o nível atual do PWM (duty cycle) para o LED
    sleep_ms(5000); 
}

// Rotina para realizar o movimento periódico entre os ângulos de 0 e 180 graus
void movimento_periodico()
{
    if (up_down) 
    {
        servo_level += 16; // Incrementa o nível do servomotor
        if (servo_level >= ANGULO_180)
            up_down = 0; // Muda direção para diminuir quando atingir o período máximo
    }
    else
    {
        servo_level -= 16; // Decrementa o nível do servomotor
        if (servo_level <= ANGULO_0)
            up_down = 1; // Muda direção para aumentar quando atingir o mínimo
    }

    pwm_set_gpio_level(PIN_SERVO, servo_level); // Define o nível atual do PWM (duty cycle) para o servomotor
    pwm_set_gpio_level(PIN_LED_B, servo_level); // Define o nível atual do PWM (duty cycle) para o LED

    sleep_ms(10);
}