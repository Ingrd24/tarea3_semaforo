/* Includes ------------------------------------------------------------------*/
#include "main.h"

typedef enum {
  VERDE,
  VERDE_PARPANDEANTE,
  ROJO,
  ROJO_PARPANDEANTE
} EstadoSemaforo;

EstadoSemaforo estadoActual = VERDE;
int BOTON_PRESIONADO = 0;  // BANDERA PARA EL BOTÓN

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void cambiarEstado(EstadoSemaforo nuevoEstado);
void blinkLED(GPIO_TypeDef *port, uint16_t pin, int delay, int times);
void ejecutarSecuencia(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // Inicia en estado VERDE
  cambiarEstado(VERDE);

  while (1)
  {
    // Verifica si el botón está presionado
    if (HAL_GPIO_ReadPin(GPIOB, BOTON) == GPIO_PIN_SET && BOTON_PRESIONADO == 0) {
      BOTON_PRESIONADO = 1;  // Marca la pulsación
      ejecutarSecuencia();  // Ejecuta la secuencia completa
    }

    // Restablece la bandera cuando se suelta el botón
    if (HAL_GPIO_ReadPin(GPIOB, BOTON) == GPIO_PIN_RESET) {
      BOTON_PRESIONADO = 0;
    }
  }
}

/* Función que ejecuta la secuencia completa de estados */
void ejecutarSecuencia(void) {
  cambiarEstado(VERDE_PARPANDEANTE);   // Cambia a VERDE_PARPANDEANTE

  cambiarEstado(ROJO);                 // Cambia a ROJO
  HAL_Delay(3000);                     // Espera 3 segundos

  cambiarEstado(ROJO_PARPANDEANTE);    // Cambia a ROJO_PARPANDEANTE


  cambiarEstado(VERDE);                // Vuelve a VERDE
}

/* Cambia el estado de los LEDs según el estado del semáforo */
void cambiarEstado(EstadoSemaforo nuevoEstado) {
  estadoActual = nuevoEstado;

  switch (estadoActual) {
    case VERDE:
      HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_RESET);  // Enciende verde
      HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_SET);     // Apaga rojo
      break;

    case VERDE_PARPANDEANTE:
      blinkLED(GPIOB, LED_VERDE, 200, 5);                   // Parpadea verde
      HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_SET);    // Apaga verde
      break;

    case ROJO:
      HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_RESET);   // Enciende rojo
      HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_SET);    // Apaga verde
      break;

    case ROJO_PARPANDEANTE:
      blinkLED(GPIOC, LED_ROJO, 200, 5);                    // Parpadea rojo
      HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_SET);     // Apaga rojo
      break;
  }
}

/* Parpadea el LED especificado */
void blinkLED(GPIO_TypeDef *port, uint16_t pin, int delay, int times) {
  for (int i = 0; i < times; i++) {
    HAL_GPIO_TogglePin(port, pin);
    HAL_Delay(delay);
  }
}

/* Configuración del Clock */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/* Inicialización de GPIO */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, LED_VERDE, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, LED_ROJO, GPIO_PIN_SET);

  /* Configuración del botón */
  GPIO_InitStruct.Pin = BOTON;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configuración de los LEDs */
  GPIO_InitStruct.Pin = LED_VERDE;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LED_ROJO;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* Manejo de error */
void Error_Handler(void) {
  while (1) {}
}
