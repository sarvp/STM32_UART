/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdio.h"
#include "String.h"
#include"stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MSG_LENGTH 1500
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char rxgsm[100] = {0} ;
char rx2[100] = {0} ;
char buf[1000] = {0};
char mobile[] = "8668780789";
char message[MSG_LENGTH] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void SerialPrint(const char *serialstring)
{
	char sendCMD[100] = {0};
	sprintf(sendCMD,serialstring);
	HAL_UART_Transmit(&huart2, (uint8_t *) sendCMD, strlen(sendCMD), HAL_MAX_DELAY);
}

/*
void SerialPrintln(const char serialstringln)
{
	//sprintf(sendCMD,gsmCMD);
	//const char nl = '\n';
	//const char serialstring = (const char)serialstringln + nl;
	//HAL_UART_Transmit(&huart2, (uint8_t *) serialstring, sizeof(serialstring), HAL_MAX_DELAY);
}
*/

void gsm_Send(const char *gsmCMD)
{
	char sendCMD[300] = {0};
	sprintf(sendCMD,gsmCMD);
	HAL_UART_Transmit(&huart1, (uint8_t *) sendCMD, strlen(sendCMD), HAL_MAX_DELAY);
	memset(buf,0,sizeof(buf));
	HAL_UART_Receive(&huart1, (uint8_t *)buf, sizeof(buf),500);
}

bool gsm_Check_with_cmd(const char *sendCMD, const char *cmpCMD)
{
	//memset(buf,0,sizeof(buf));
	gsm_Send(sendCMD);
	HAL_Delay(100);
	if( strstr((char *)buf,cmpCMD) )
	{
		return true;
	}
	else return false ;
}

bool gsm_init(void)
{
	if(!gsm_Check_with_cmd("AT\r\n", "OK"))
	{
		memset(buf,0,sizeof(buf));
		return false ;
	}
	memset(buf,0,sizeof(buf));
	if(!gsm_Check_with_cmd("AT+CFUN=1\r\n", "OK"))
	{
		memset(buf,0,sizeof(buf));
		return false ;
	}
	memset(buf,0,sizeof(buf));
	if(!gsm_Check_with_cmd("AT+CMGF=1\r\n", "OK"))
	{
		memset(buf,0,sizeof(buf));
		return false ;
	}
	memset(buf,0,sizeof(buf));
	if(!gsm_Check_with_cmd("AT+CNMI=1,1,0,0,0\r\n", "OK"))
	{
		memset(buf,0,sizeof(buf));
		return false ;
	}
	memset(buf,0,sizeof(buf));
	if(!gsm_Check_with_cmd("AT+CPIN?\r\n", "+CPIN: READY"))
	{
		memset(buf,0,sizeof(buf));
		return false ;
	}
	//memset(buf,0,sizeof(buf));
	return true ;
}

bool is_gsm_registred(void)
{
	if(!gsm_Check_with_cmd("AT+CREG?\r\n", "+CREG: 1,1"))
	{
		memset(buf,0,sizeof(buf));
		gsm_Send("AT+CREG=1\r\r\n");
		HAL_Delay(2000);
		memset(buf,0,sizeof(buf));
		if(!gsm_Check_with_cmd("AT+CREG?\r\n", "+CREG: 1,1"))
		{
			return false ;
		}
	}
	return true ;
}

bool is_sms_unread(void)
{
	//gsm_check_with_cmd("AT+CMGL=\"REC UNREAD\",1");
	char sendCMD[100], smsrxcheck[30] = {0};
	//sprintf(sendCMD,"AT+CMGL=\"REC UNREAD\",1\n");
	//HAL_UART_Transmit(&huart1, (uint8_t *) sendCMD, strlen(sendCMD), 100);
	gsm_Send("AT+CMGL=\"REC UNREAD\",1\r\n");
	memset(smsrxcheck,0,sizeof(smsrxcheck));
	sprintf(smsrxcheck,buf);
	//SerialPrint(smsrxcheck);
	//HAL_UART_Receive(&huart1, (uint8_t *)message, MSG_LENGTH, 5000);
	if( !strstr(smsrxcheck,"OK") )
	{
		char smsdata[200] = {0};
		memset(sendCMD,0,sizeof(sendCMD));
		//sprintf(sendCMD,"AT+CMGL=\"REC UNREAD\",1\n");
		//HAL_UART_Transmit(&huart1, (uint8_t *) sendCMD, strlen(sendCMD), 100);
		gsm_Send("AT+CMGL=\"REC UNREAD\",1\r\n");
		//memset(smsrxcheck,0,sizeof(smsrxcheck));
		memset(smsdata,0,sizeof(smsdata));
		sprintf(smsdata,buf);
		//HAL_UART_Receive(&huart1, (uint8_t *)smsdata, (80+MSG_LENGTH), 5000);
		//SerialPrint(smsdata);
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart1, (uint8_t *)buf, 100);
}
void flushstr(char *flushstr)
{
	char emptystr[sizeof(*flushstr)];
	for(int sizecount = 0; sizecount < sizeof(*flushstr); sizecount++)
	{
		emptystr[sizecount] = '\0';
	}
	sprintf(flushstr,emptystr);
}
*/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	uint8_t i = 0 ;

    char msg[100] = {0};
    //char cmd[100] = {0};

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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /*
  	sprintf(msg,"SMS with GSM Module\r\n");
  	HAL_UART_Transmit(&huart2, msg,sizeof(msg),1000);
  	sprintf(msg,"Checking Module...\r\n");
  	HAL_UART_Transmit(&huart2, msg,sizeof(msg),1000);
  	uint8_t flag=1;

  	while(flag){
  		sprintf(cmd,"AT\r\n");
  		HAL_UART_Transmit(&huart1,cmd,sizeof(cmd),1000);
  		HAL_UART_Receive_IT (&huart1, buf, 30);
  		HAL_Delay(1000);

  		sprintf(msg,buf);
  		HAL_UART_Transmit(&huart2,msg,sizeof(msg),1000);

  		if(strstr((char *)buf,"OK")){
  			sprintf(msg,"Module Connected\r\n");
  			HAL_UART_Transmit(&huart2,msg,sizeof(msg),1000);
  			flag=0;
  		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  		}
  		HAL_Delay(1000);
  		HAL_UART_Transmit(&huart2,buf,sizeof(buf),1000);
  		memset(buf,0,sizeof(buf));
  	}

  	sprintf(msg,"Sending Message\r\n");
  	HAL_UART_Transmit(&huart2,msg,sizeof(msg),1000);
  	sprintf(cmd,"AT+CMGF=1\r\n");
  	HAL_UART_Transmit(&huart1,cmd,sizeof(cmd),1000);
  	HAL_UART_Receive_IT (&huart1, buf, 30);
  	HAL_Delay(1000);
  	HAL_UART_Transmit(&huart2,buf,sizeof(buf),1000);
  	memset(buf,0,sizeof(buf));

  	sprintf(cmd,"AT+CMGS=\"%s\"\r\n",mobile);
  	HAL_UART_Transmit(&huart1, cmd,sizeof(cmd),1000);
  	HAL_Delay(100);
  	sprintf(cmd,"Hello from GSM%c",0x1a);
  	HAL_UART_Transmit(&huart1, cmd,sizeof(cmd),1000);
  	//HAL_UART_Transmit(&huart1, ((char)(0x1a)),1,1000);
  	HAL_UART_Receive_IT (&huart1, buf, 30);
  	HAL_Delay(5000);

  	HAL_UART_Transmit(&huart2, buf,sizeof(buf),1000);
  	sprintf(msg,"Message Sent\r\n");
  	HAL_UART_Transmit(&huart2, msg,sizeof(msg),1000);*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    	//sprintf(msg,"In While Loop :\n");
    	//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)sizeof(msg),HAL_MAX_DELAY);
    	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);
    	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_9, RESET);
    	//uint8_t status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
    	if( ! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5))
    	{
    		while(! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5));
    		//sprintf(msg,"Switch Pressed...\n");
    		//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t )sizeof(msg),HAL_MAX_DELAY);
    		sprintf(msg,"Switch 1 Pressed\n");
    		SerialPrint(msg);
    		memset(msg,0,sizeof(msg));
    		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11|GPIO_PIN_9);
    		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, RESET);
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);
    		//memset(msg,0,sizeof(msg));
    		i ++ ;
    		sprintf(msg,"%d\n",i);
    		SerialPrint(msg);
    		memset(msg,0,sizeof(msg));
    		//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)sizeof(msg),HAL_MAX_DELAY);
      		//sprintf(cmd,"ATD 8668780789;\r\n");
    		//sprintf(cmd,"AT+CREG?\r\n");
      		//HAL_UART_Transmit(&huart1,(uint8_t *)cmd,(uint16_t)sizeof(cmd),HAL_MAX_DELAY);
    		//sprintf(msg,"AT+CREG?\r\n");
    		//bool id = gsm_Check_with_cmd(msg, "+CREG: 1,1");
    		//sprintf(msg,"AT+CREG?\r\n");
    		//gsm_Send(msg);
    		//memset(msg,0,sizeof(msg));
    		//bool id = gsm_Check_with_cmd("AT\r\n","OK");
    		//HAL_UART_Receive(&huart1, (uint8_t *)buf, 100, 1000);
      		//HAL_Delay(100);
    		//bool id = gsm_init();
      		//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)strlen(msg),HAL_MAX_DELAY);
      		//if(strstr((char *)buf,"+CREG: 1,1")){
      		//if(is_gsm_registred()){
      		if(gsm_init())	{
    			sprintf(msg,(const char *)buf);
      			SerialPrint(msg);
      			//sprintf(msg,"Calling . . .\r\n");
      			//sprintf(msg,"Registered on Netwrok . . .\r\n");
      			SerialPrint("Registered on Netwrok . . .\r\n");
      			memset(msg,0,sizeof(msg));
      			//HAL_UART_Transmit(&huart2,(uint8_t *)buf,(uint16_t)sizeof(buf),HAL_MAX_DELAY);
      			//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
      			//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9|GPIO_PIN_11);
      			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, SET);
      			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
      			HAL_Delay(300);
      			//memset(buf,0,sizeof(buf));
      		}
      		else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
    	}
    	memset(msg,0,sizeof(msg));
    	memset(buf,0,sizeof(buf));

    	if( ! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4))
    	{
    		while(! HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4));
    		//sprintf(msg,"Switch Pressed...\n");
    		//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t )sizeof(msg),HAL_MAX_DELAY);
    		sprintf(msg,"Switch 2 Pressed\n");
    		SerialPrint(msg);
    		memset(msg,0,sizeof(msg));
    		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11|GPIO_PIN_9);
    		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, RESET);
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, RESET);
    		//memset(msg,0,sizeof(msg));
    		i ++ ;
    		sprintf(msg,"%d\n",i);
    		SerialPrint(msg);
    		memset(msg,0,sizeof(msg));
    		//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)sizeof(msg),HAL_MAX_DELAY);
    		//sprintf(cmd,"ATD 8668780789;\r\n");
    		//sprintf(cmd,"AT+CREG?\r\n");
    		//HAL_UART_Transmit(&huart1,(uint8_t *)cmd,(uint16_t)sizeof(cmd),HAL_MAX_DELAY);
    		//sprintf(msg,"AT\r\n");
    		//gsm_Send(msg);
    		//memset(msg,0,sizeof(msg));
    		//bool id = gsm_Check_with_cmd("AT\r\n","OK");
    		//HAL_UART_Receive(&huart1, (uint8_t *)buf, 100,1000);
    		//HAL_Delay(100);

    		//sprintf(msg,(const char *)buf);
    		//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)strlen(msg),HAL_MAX_DELAY);
    		//if(strstr((char *)buf,"OK\r\n")){
    		if(is_gsm_registred()){
    			sprintf(msg,(const char *)buf);
    			SerialPrint(msg);
    			sprintf(msg,"Initialized . . .\r\n");
    			//sprintf(msg,"New Message Received . . .\r\n");
    			SerialPrint(msg);
    			memset(msg,0,sizeof(msg));
    			//HAL_UART_Transmit(&huart2,(uint8_t *)buf,(uint16_t)sizeof(buf),HAL_MAX_DELAY);
    			//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    			//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9|GPIO_PIN_11);
    			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, SET);
    			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
    			HAL_Delay(300);
    			//memset(buf,0,sizeof(buf));
    		}
    		else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
    	}
    	memset(msg,0,sizeof(msg));
    	memset(buf,0,sizeof(buf));
    	//HAL_UART_Receive(&huart1, (uint8_t *)buf, 100, 1000);
    	//HAL_Delay(100);
    	//sprintf(msg,(const char *)buf);
    	//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)sizeof(msg),HAL_MAX_DELAY);
    	//memset(msg,0,sizeof(msg));
    	//memset(buf,0,sizeof(buf));
    	//sprintf(msg,"In While Loop, Switch not pressed...\n");
    	//HAL_UART_Transmit(&huart2,(uint8_t *)msg,(uint16_t)sizeof(msg),HAL_MAX_DELAY);
    	//else ;
      //HAL_UART_Receive_IT (&huart1, buf, 100);
      //HAL_UART_Receive(&huart1, buf, sizeof(buf), HAL_MAX_DELAY);
      //HAL_Delay(300);
      //sprintf(msg,buf);
      //HAL_UART_Transmit(&huart2,msg,sizeof(msg),HAL_MAX_DELAY);
      //memset(buf,0,sizeof(buf));
      //memset(cmd,0,sizeof(cmd));
      //memset(msg,0,sizeof(msg));
      //HAL_UART_Receive_IT (&huart2, buf, 100);
      //HAL_UART_Receive(&huart2, buf, sizeof(buf), HAL_MAX_DELAY);
      //sprintf(cmd,buf);
      //HAL_UART_Transmit(&huart1,cmd,sizeof(cmd),HAL_MAX_DELAY);
      //HAL_Delay(1000);
      //HAL_UART_Transmit(&huart2,buf,sizeof(buf),1000);
      //memset(buf,0,sizeof(buf));
      //memset(cmd,0,sizeof(cmd));
      //memset(msg,0,sizeof(msg));
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 4800;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
