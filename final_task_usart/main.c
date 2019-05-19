#include "main.h"

void EXTI0_IRQHandler (void)
{
  //если нажата кнопка
  if (EXTI_GetITStatus (EXTI_Line0) != RESET)
  {
    EXTI_ClearITPendingBit (EXTI_Line0);
    //тестовый пакет
    packet_t* p = (packet_t*) malloc(LEN_DATA_PACKET);
    p->cmd = CMD_LedBrightInc;
    memset(p->params, 0, LEN_DATA_PARAM);
    QPush(&output_q, p);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
  }
}

//interval timer handler
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
  }
}

//duration timer handler
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    tim_stop();
  }
}

void USART1_IRQHandler()
{
  if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_TC);
    uint8_t data = 0xFF;
    USART_SendData(USART1, data);
    GPIO_SetBits(GPIOD, GPIO_Pin_15);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);
    /*
    static uint8_t SEND_PROGRESS = 0;
    static uint8_t send_data_cnt = 0;
    static uint8_t * p_out = NULL;

    if ((output_q.head == NULL) && (SEND_PROGRESS != 0)) USART_ITConfig(USART1, USART_IT_TC, DISABLE);//???hz;
    else
    {
        //начало отправки нового пакета
        if ((SEND_PROGRESS == 0) && (output_q.head != NULL))
        {
            send_data_cnt = 0;
            SEND_PROGRESS = 1;
            p_out = (uint8_t *) QPop(&output_q);
            USART_SendData(USART1, *(p_out + send_data_cnt));
            send_data_cnt++;
        }
        //доотправка
        else if (SEND_PROGRESS != 0)
        {
            USART_SendData(USART1, *(p_out + send_data_cnt));
            send_data_cnt++;
            if (send_data_cnt == LEN_DATA_PACKET)
            {
              free(p_out);
              SEND_PROGRESS = 0;
            }
        }
    }
    */
  }

  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
      USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      /*
      static uint8_t REC_PROGRESS = 0;
      static uint8_t rec_data_cnt = 0;
      static uint8_t * p_in = NULL;

      //начинаем принимать новый пакет
      if (REC_PROGRESS == 0) //ToDo - работает если LEN_DATA_PACKET>1
      {
        rec_data_cnt = 0;
        REC_PROGRESS = 1;

        p_in = (uint8_t *) malloc(LEN_DATA_PACKET);

        *(p_in + rec_data_cnt) = USART_ReceiveData(USART1);
        rec_data_cnt++;
      }
      else 
      {
        //дополучаем
        *(p_in + rec_data_cnt) = USART_ReceiveData(USART1);
        rec_data_cnt++;
        if (rec_data_cnt == LEN_DATA_PACKET)
        {
          QPush(&input_q, (packet_t*) p_in);
          REC_PROGRESS = 0;
        }
      } 
      */
      uint8_t data = USART_ReceiveData(USART1);
      GPIO_SetBits(GPIOD, GPIO_Pin_15);
      USART_ITConfig (USART1, USART_IT_RXNE, DISABLE);
  }
}

uint8_t do_cmd (packet_t * packet)
{ 
  uint8_t res = OK_CONST; 
  switch (packet->cmd)
  {
    case CMD_LedBrightSet:
      res = set_bright (packet->params[0], packet->params[1]);
    break;

    case CMD_LedBrightInc:
      GPIO_SetBits(GPIOD, GPIO_Pin_15);//ToDo
    break;

    case CMD_LedBrightDec:
      ;//ToDo
    break;

    case CMD_TimStart:
      tim_start();
    break;

    case CMD_TimStop:
      tim_stop();
    break;

    case CMD_TimIntervalSet:
      tim_set_interval(*((uint16_t *) packet->params));
    break;

    case CMD_TimIntervalInc:
      ;//ToDo
    break;

    case CMD_TimIntervalDec:
      ;//ToDo
    break;

    case CMD_TimDurationSet:
      tim_set_duration (packet->params[0]);
    break;

    case CMD_TimDurationInc:
      ;//ToDo
    break;

    case CMD_TimDurationDec:
      ;//ToDo
    break;

    default:
      res = ERROR_CONST;
    break;
  }
  free(packet);

  return res;
}

int main(void)
{
  init_system();

  input_q.head = NULL;
  input_q.tail = NULL;

  output_q.head = NULL;
  output_q.tail = NULL;

  USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);
  while (1)
  {
    //if (input_q.head != NULL) do_cmd (QPop(&input_q));
  }

}
