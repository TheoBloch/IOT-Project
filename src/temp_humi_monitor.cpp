#include "temp_humi_monitor.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT20.h>
DHT20 dht20;
LiquidCrystal_I2C lcd(33,16,2);


void temp_humi_monitor(void *pvParameters) {
    Wire.begin(11, 12);
    Serial.begin(115200);
    dht20.begin();

    // Khởi tạo LCD
    lcd.begin();
    lcd.backlight();
    lcd.print("Starting...");

    Serial.println("Temp & Humidity Task Started");

    for (;;) {
        dht20.read();
        float t = dht20.getTemperature();
        float h = dht20.getHumidity();

        // Cấu trúc lưu dữ liệu cảm biến
        SensorData data;
        data.valid       = !(isnan(t) || isnan(h));
        data.temperature = data.valid ? t : -1.0f;
        data.humidity    = data.valid ? h : -1.0f;

        // Gửi dữ liệu mới nhất vào queue
        xQueueOverwrite(xSensorQueue, &data);
        xSemaphoreGive(xDisplaySemaphore);   // Báo LCD: "Có dữ liệu mới!"

        // In ra serial monitor
        Serial.printf("Temp: %.1f°C | Hum: %.1f%% | %s\n",
                      data.temperature, data.humidity, data.valid ? "OK" : "ERR");

        // Task ngủ 5s rồi tiếp tục đọc
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void lcd_task(void *pvParameters) {
    for (;;) {
        // Đợi có dữ liệu mới từ sensor
        if (xSemaphoreTake(xDisplaySemaphore, portMAX_DELAY) == pdTRUE) {
            SensorData data;
            if (xQueueReceive(xSensorQueue, &data, 0) == pdTRUE) {
                lcd.clear();  // Xóa màn hình

                // Dòng 1: hiển thị giá trị cảm biến
                lcd.setCursor(0, 0);
                if (data.valid)
                    lcd.printf("%.1fC  %.1f%%", data.temperature, data.humidity);
                else
                    lcd.print("SENSOR ERROR");

                // Dòng 2: tính trạng thái hiển thị
                lcd.setCursor(0, 1);
                if (!data.valid) {
                    lcd.print("!! ERROR !!");
                }
                else if (data.temperature > 35 || data.humidity > 80) {
                    lcd.print("CRITICAL");
                }
                else if (data.temperature >= 30 || data.humidity >= 70) {
                    lcd.print("WARNING ");
                }
                else {
                    lcd.print("NORMAL  ");
                }
            }
        }
    }
}