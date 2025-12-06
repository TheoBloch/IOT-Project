#include "neo_blinky.h"
#include "global.h"
#include "temp_humi_monitor.h"

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);  // Khởi tạo NeoPixel strip

void neo_blinky(void *pvParameters) {
    strip.begin();  //generate NeoPixel
    strip.clear();  // set all pixel off
    strip.show();   // Update the pixels

    while (1) {
        if (xSemaphoreTake(xDisplaySemaphore, portMAX_DELAY)) {  // Wait for semaphore to receive new data
            float humidity;
            SensorData data;
            if (xQueuePeek(xSensorQueue, &data, 5000)) {  // Get humidity value from queue
                humidity = data.humidity;
                Serial.printf("Humidity in neo :%f\n",humidity);
                // Adjust NeoPixel color based on humidity
                if (humidity < 40.0) {  // Low humidity (Red)
                    strip.setPixelColor(0, strip.Color(255, 0, 0));  // Set red color
                } 
                else if (humidity >= 40.0 && humidity < 70.0) {  // Medium humidity (Yellow)
                    strip.setPixelColor(0, strip.Color(255, 255, 0));  // Set yellow color
                } 
                else {  // High humidity (Green)
                    strip.setPixelColor(0, strip.Color(0, 255, 0));  // Set green color
                }

                strip.show();  // Update NeoPixel LED color
            }
        }
    }
}