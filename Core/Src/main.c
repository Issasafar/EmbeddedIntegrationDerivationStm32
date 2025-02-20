/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "string.h"
#include "stdio.h"
#include <stdbool.h>
#include "lcd_txt.h"
#include "float.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
enum FunctionType {
    sine = 0,
    cosine = 1
};
enum OperationType {
    integration = 0,
    derivation = 1
};
enum Alignment {
    topCenter,
    topStart,
    topEnd,
    bottomCenter,
    bottomStart,
    bottomEnd,

};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */

double derivative(enum FunctionType fun, double x, double n) {
    double h = sqrt(n);
    if (fun == sine) {
        return (sin(x + h) - sin(x - h)) / (2.0 * h);
    } else if (fun == cosine) {
        return (cos(x + h) - cos(x - h)) / (2.0 * h);
    } else {
        return 0;
    }
}

double trapezoidalIntegral(enum FunctionType fun, double a, double b, double n) {
    double h = (b - a) / n;
    double integral = 0.0;
    for (int i = 0; i <= n; ++i) {
        double x = a + i * h;
        double weight = (i == 0 || i == n) ? 0.5 : 1.0;
        if (fun == sine) {
            integral += weight * sin(x);
        } else if (fun == cosine) {
            integral += weight * cos(x);
        }
    }
    integral *= h;
    return integral;
}

void printToScreen(const char *message, enum Alignment alignment) {
    int columnCount = 16;
    int messageSize = (int) strlen(message);
    switch (alignment) {
        case (topStart):
            LCD_Puts(0, 0, (int8_t *) message);
            break;
        case (topCenter):
            LCD_Puts(0, ((columnCount - messageSize) / 2), (int8_t *) message);
            break;
        case (topEnd):
            LCD_Puts(0, ((columnCount - messageSize)), (int8_t *) message);
            break;
        case (bottomStart):
            LCD_Puts(1, 0, (int8_t *) message);
            break;
        case (bottomCenter):
            LCD_Puts(1, ((columnCount - messageSize) / 2), (int8_t *) message);
            break;
        case (bottomEnd):
            LCD_Puts(1, ((columnCount - messageSize)), (int8_t *) message);
            break;
        default:
            break;
    }

}

enum FunctionType selectFunction(bool *exit) {
    char *selectionStr[2] = {"Sin(x)", "Cos(x)"};
    printToScreen("Select", topCenter);
    printToScreen(selectionStr[0], bottomStart);
    printToScreen(selectionStr[1], bottomEnd);
    enum FunctionType choices[] = {sine, cosine};
    char displayStr[6];
    int i = 1;
    enum FunctionType selected = choices[i];
    bool configured = false;
    while (!configured) {
        // settings
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
            *exit = true;
            break;
        }
        // inc - dec
        if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0) || (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0)) {
            ++i;
            i = (i) % 2;
            selected = choices[i];
            sprintf(displayStr, "%s", selectionStr[i]);
            if (selected == sine) {
                LCD_Clear_RC(1, 10, 6);
                printToScreen(displayStr, bottomStart);
            } else {
                LCD_Clear_RC(1, 0, 6);
                printToScreen(displayStr, bottomEnd);
            }
        }
        // ok
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 0) {
            LCD_Clear();
            configured = true;
        }
        HAL_Delay(150);
    }
    return selected;
}

enum OperationType selectOperation(bool *exit) {
    char *selectionStr[2] = {"Diff", "Integ"};
    printToScreen("Select", topCenter);
    printToScreen(selectionStr[0], bottomStart);
    printToScreen(selectionStr[1], bottomEnd);
    enum OperationType choices[] = {derivation, integration};
    char displayStr[5];
    int i = 1;
    enum OperationType selected = choices[i];
    bool configured = false;
    while (!configured) {
        // settings
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
            *exit = true;
            break;
        }
        // inc - dec
        if ((HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0) || (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0)) {
            ++i;
            i = i % 2;
            selected = choices[i];
            sprintf(displayStr, "%s", selectionStr[i]);
            if (selected == derivation) {
                LCD_Clear_RC(1, 10, 6);
                printToScreen(displayStr, bottomStart);
            } else {
                LCD_Clear_RC(1, 0, 6);
                printToScreen(displayStr, bottomEnd);
            }
        }
        // ok
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 0) {
            LCD_Clear();
            configured = true;
        }
        HAL_Delay(150);
    }
    LCD_Clear();
    return selected;
}

void selectPoints(enum OperationType operationType, bool *exit, double *points) {
    char *pointsStr[2] = {"", ""};
    char *x1Str = "X1=";
    char *x2Str = "X2=";
    char *xStr = "X=";
    bool configured[2] = {false, false};
    int currentPointIndex = 0;
    LCD_Clear();
    while (!configured[0] || !configured[1]) {

        printToScreen("Select points:", topCenter);
        if (!configured[0]) {
            // first point
            if (operationType == derivation) {
                configured[1] = true;
            }
            sprintf(pointsStr[0], "%.2f", points[0]);
            if (operationType == derivation) {
                printToScreen(xStr, bottomStart);
            } else {
                printToScreen(x1Str, bottomStart);
            }

            printToScreen(pointsStr[0], bottomEnd);

        } else if (!configured[1]) {
            // second point
            sprintf(pointsStr[1], "%.2f", points[1]);
            printToScreen(x2Str, bottomStart);
            printToScreen(pointsStr[1], bottomEnd);
        }
        // settings
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 0) {
            *exit = true;
            break;
        }
        // ok
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 0) {
            if (!configured[0]) {
                ++currentPointIndex;
                configured[0] = true;
                LCD_Clear();
            } else {
                configured[1] = true;
            }
        }
        // inc
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == 0) {
            points[currentPointIndex] += .5f;
        }
        // dec
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0) {
            points[currentPointIndex] -= .5f;
        }
        HAL_Delay(125);
    }
    LCD_Clear();
}

void printResult(enum OperationType operationType, double result) {
    char str[20] = {};
    sprintf(str, "%.4f", result);
    if (operationType == derivation) {
        printToScreen("Diff-Result:", topCenter);
    } else {
        printToScreen("Integ-Result:", topCenter);
    }
    printToScreen(str, bottomStart);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    /* USER CODE BEGIN 2 */

    LCD_Init(); // initialize the lcd screen
    LCD_Puts(0, 2, (int8_t *) "Welcome :)");
    LCD_Puts(1, 2, (int8_t *) "PLease Wait");

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {


        HAL_Delay(2000);
        LCD_Clear();
        bool exit = false;

        while (!exit) {

            HAL_Delay(200);
            // select function
            enum FunctionType fn = selectFunction(&exit);
            if (exit) {
                break;
            }
            HAL_Delay(200);
            // select operation
            enum OperationType op = selectOperation(&exit);
            if (exit) {
                break;
            }
            HAL_Delay(200);
            // select point/s
            double points[2] = {.0f, .0f};
            selectPoints(op, &exit, points);
            if (exit) {
                break;
            }
            HAL_Delay(200);
            // show result
            double result = .0f;
            double n = DBL_EPSILON;
            if (op == derivation) {
                result = derivative(fn, points[0], n);
            } else if (op == integration) {
                result = trapezoidalIntegral(fn, points[0], points[1], n);
            }
            printResult(op, result);

            // wait for ok to restart
            while (1) {
                if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 0) {
                    LCD_Clear();
                    exit = true;
                    break;
                }
            }

        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

    /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pins : PA8 PA9 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PA11 */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PB3 PB5 PB9 */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
