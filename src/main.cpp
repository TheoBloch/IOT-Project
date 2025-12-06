#include "global.h"

#include "led_blinky.h"
#include "neo_blinky.h"
#include "temp_humi_monitor.h"
// #include "mainserver.h"
// #include "tinyml.h"
//#include "coreiot.h"

// include task
#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_wifi.h"
#include "task_webserver.h"
#include "task_core_iot.h"

void setup()
{
  Serial.begin(115200);

  // Tạo semaphore và queue cho các task
    xSensorQueue      = xQueueCreate(1, sizeof(SensorData));   // Queue 1 phần tử chứa dữ liệu cảm biến
    xDisplaySemaphore = xSemaphoreCreateBinary();               // Semaphore to signal new data

    if (xSensorQueue == nullptr || xDisplaySemaphore == nullptr) {
        Serial.println("Failed to create RTOS objects!");
        while (1) { delay(1000); }  // if failed, halt here
    }

  xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
  xTaskCreate(neo_blinky, "Task NEO Blink", 2048, NULL, 2, NULL);
  xTaskCreate(temp_humi_monitor, "Task TEMP HUMI Monitor", 5129, NULL, 2, NULL);
  //xTaskCreate(task_webserver, "Task  Webserver" ,10000  ,NULL  ,2 , NULL);
  // xTaskCreate( tiny_ml_task, "Tiny ML Task" ,2048  ,NULL  ,2 , NULL);
  //xTaskCreate(coreiot_task, "CoreIOT Task" ,4096  ,NULL  ,2 , NULL);
   xTaskCreate(Task_Toogle_BOOT, "Task_Toogle_BOOT", 4096, NULL, 2, NULL);
}

void loop()
{
  // if (check_info_File(1))
  // {
  //   if (!Wifi_reconnect())
  //   {
  //    //Webserver_stop();
  //   }
  //   else
  //   {
  //     //CORE_IOT_reconnect();
  //   }
  // }
  // //Webserver_reconnect();
}