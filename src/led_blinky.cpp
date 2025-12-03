#include "led_blinky.h"

void led_blinky(void *pvParameters) {
    pinMode(LED_GPIO, OUTPUT);  // Đặt GPIO LED làm OUTPUT

    while (1) {
        if (xSemaphoreTake(xDisplaySemaphore, portMAX_DELAY)) {  // Chờ dữ liệu từ cảm biến
            float temperature;
            if (xQueueReceive(xSensorQueue, &temperature, 0)) {  // Nhận giá trị nhiệt độ từ queue
                // Điều chỉnh hành vi của LED dựa trên nhiệt độ
                if (temperature < 20.0) {
                    digitalWrite(LED_GPIO, HIGH);  
                    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Chớp LED chậm
                    digitalWrite(LED_GPIO, LOW);  
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                } 
                else if (temperature >= 20.0 && temperature < 30.0) {
                    digitalWrite(LED_GPIO, HIGH);  
                    vTaskDelay(500 / portTICK_PERIOD_MS);  // Chớp LED nhanh
                    digitalWrite(LED_GPIO, LOW);  
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                } 
                else {
                    digitalWrite(LED_GPIO, HIGH);  // LED sáng liên tục
                }
            }
        }
    }
}