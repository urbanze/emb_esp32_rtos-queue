#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_system.h>
#include <esp_log.h>

QueueHandle_t buffer;//Objeto da queue

void t1(void*z)
{
    uint32_t snd = 0;
    while(1)
    {
        if (snd < 15)//se menor que 15
        {
            xQueueSend(buffer, &snd, pdMS_TO_TICKS(0));//Envia a variavel para queue
            snd++;//incrementa a variavel
        }
        else//se nao, espera 5seg para testar o timeout da outra tarefa
        {
            vTaskDelay(pdMS_TO_TICKS(5000));
            snd = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void t2(void*z)
{
    uint32_t rcv = 0;
    while(1)
    {
        if (xQueueReceive(buffer, &rcv, pdMS_TO_TICKS(1000)) == true)//Se recebeu o valor dentro de 1seg (timeout), mostrara na tela
        {
            ESP_LOGI("Queue", "Item recebido: %u", rcv);//Mostra o valor recebido na tela
        }
        else
        {
            ESP_LOGE("Queue", "Item nao recebido, timeout expirou!");//Se o timeout expirou, mostra erro
        }
    }
}

void app_main()
{
    buffer = xQueueCreate(10, sizeof(uint32_t));//Cria a queue *buffer* com 10 slots de 4 Bytes

    xTaskCreatePinnedToCore(t1, "t1", 4096, NULL, 1, NULL, 0);//Cria a tarefa que escreve valores na queue
    xTaskCreatePinnedToCore(t2, "t2", 4096, NULL, 1, NULL, 0);//Cria a tarefa que le valores da queue
}
