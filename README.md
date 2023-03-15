# Airgradient-Grafana-Cloud
Airgradient sensor (esp8266 Wemos D1 Mini) sends data to GrafanaCloud.

Airgradient has a [DIY air sensor](https://www.airgradient.com/diy/) guide.I've followed the guide but I built my air sensor with only the PM2 sensor (Plantower PMS5003 PM Sensor).
In this project, I want to send the sensor data to GrafanaCloud to log and graph the data.

The code is a mix of the official [AirGradient](https://github.com/airgradienthq/arduino) Arduino sketch ```C02_PM_SHT_OLED_WIFI``` and Grafana Arduino libraries 
   - https://github.com/grafana/prometheus-arduino
  -  https://github.com/grafana/loki-arduino
   - https://github.com/grafana/arduino-prom-loki-transport
  -  https://github.com/grafana/arduino-snappy-proto

In addition, the Airgradient library has been modified to display pm1 and pm10 values in addition to the pm2 it displays by default. This was added from this repo: [d3vilh/airgradient-improved](https://github.com/d3vilh/airgradient-improved)

## HOW TO USE

First, follow the guide of [DIY air sensor](https://www.airgradient.com/diy/), then install the Grafana libraries and create an account ([see this guide](https://github.com/grafana/diy-iot))
Then, you have to modify the PromLokiTransport library: go to the folder where Arduino saves the libraries, open the folder PromLokiTransport, open src and finally open the file PromLokiTransport.h .   
Find the line -> ```#elif defined(ESP8266)``` and substitute ``` #error "ESP8266 is currently unsupported, it's proving too difficult to work around the 4k stack size limitation at the moment."``` with this  
``` #include "clients/ESP8266Client.h"```  
```typedef ESP8266Client PromLokiTransport; ```

Remember to fill all the gaps in the config.h with your credentials. For help, you can follow this simple tutorial: https://github.com/grafana/diy-iot

Now you should be able to compile the sketch and upload software to your board using Arduino IDE.

In GrafanaCould create a dashboard and pick the data source from prometheus

The results should be:

![image](https://user-images.githubusercontent.com/46966357/157704757-cd4f6c93-b8bb-4097-8a50-6decb1266950.png)

