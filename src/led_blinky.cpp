#include "led_blinky.h"

void led_blinky(void *pvParameters) {
    pinMode(LED_GPIO, OUTPUT);  // Đặt GPIO LED làm OUTPUT

    while (1) {
        if (xSemaphoreTake(xDisplaySemaphore, portMAX_DELAY)) {  // Chờ dữ liệu từ cảm biến
            SensorData data;
            float temperature;
            if (xQueuePeek(xSensorQueue, &data, 5000)) {  // Nhận giá trị nhiệt độ từ queue
                // Điều chỉnh hành vi của LED dựa trên nhiệt độ
                temperature=data.temperature;
                Serial.printf("Temperature in neo :%f\n",temperature);

                if (temperature < 20.0f) {
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                }
                else if (temperature < 30.0f) {           // 20–29.9°C
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(500 / portTICK_PERIOD_MS);
                }
                else {                                    // ≥ 30°C → nháy nhanh
                    digitalWrite(LED_GPIO, HIGH);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                    digitalWrite(LED_GPIO, LOW);
                    vTaskDelay(200 / portTICK_PERIOD_MS);
                }}
        }
    }
}