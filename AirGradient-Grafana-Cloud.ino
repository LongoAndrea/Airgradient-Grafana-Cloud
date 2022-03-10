#include "config.h"
#include <PromLokiTransport.h>
#include <PrometheusArduino.h>
#include <AirGradient.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, SDA, SCL);
AirGradient ag = AirGradient();


PromLokiTransport transport;
PromClient client(transport);

// Create a write request for 1 series.
WriteRequest req(1, 512);

TimeSeries ts1(2, "pm2", "{job=\"esp32-test\",host=\"esp32\"}");

int loopCounter = 0;

void setup() {
    Serial.begin(115200);
    ag.PMS_Init();
    display.init();
    display.flipScreenVertically();
    // Wait 5s for serial connection 
    uint8_t serialTimeout;
    while (!Serial && serialTimeout < 50) {
        delay(100);
        serialTimeout++;
    }

    Serial.println("Starting");
    Serial.print("Free Mem Before Setup: ");
    Serial.println(freeMemory());

    // Configure and start the transport layer
    transport.setUseTls(false);    
    transport.setWifiSsid(WIFI_SSID);
    transport.setWifiPass(WIFI_PASSWORD);
    transport.setDebug(Serial);  // Remove this line to disable debug logging of the client.
    if (!transport.begin()) {
        Serial.println(transport.errmsg);
        while (true) {};
    }

    // Configure the client
    client.setUrl(GC_URL);
    client.setPath((char*)GC_PATH);
    client.setPort(GC_PORT);
    client.setUser(GC_USER);
    client.setPass(GC_PASS);
    client.setDebug(Serial);  // Remove this line to disable debug logging of the client.
    if (!client.begin()) {
        Serial.println(client.errmsg);
        while (true) {};
    }

    // Add our TimeSeries to the WriteRequest
    req.addTimeSeries(ts1);
    //req.addTimeSeries(ts2);
    req.setDebug(Serial);  // Remove this line to disable debug logging of the write request serialization and compression.

    Serial.print("Free Mem After Setup: ");
    Serial.println(freeMemory());

};



void loop() {
    int64_t time;
    time = transport.getTimeMillis();
    Serial.println(time);

    // Efficiency in requests can be gained by batching writes so we accumulate a few samples before sending.
    // This is not necessary however, especially if your writes are infrequent. It's still recommended to build batches when you can.

    //show on the oled screen PM2 value
    showTextRectangle("PM2", String(ag.getPM2_Raw()), false);
    if (!ts1.addSample(time, ag.getPM2_Raw())) {
        Serial.println(ts1.errmsg);
    }    
    loopCounter++;
    if (loopCounter >= 4) {
        // Send data
        loopCounter = 0;
        PromClient::SendResult res = client.send(req);
        if (!res == PromClient::SendResult::SUCCESS) {
            Serial.println(client.errmsg);
            // Note: additional retries or error handling could be implemented here.
            // the result could also be:
            // PromClient::SendResult::FAILED_DONT_RETRY
            // PromClient::SendResult::FAILED_RETRYABLE
        }
        // Batches are not automatically reset so that additional retry logic could be implemented by the library user.
        // Reset batches after a succesful send.
        ts1.resetSamples();
        
    }

    // Grafana Cloud defaults to ingesting and showing one sample every 15 seconds.
    // If you select a higher frequency here and work with Grafana Cloud, you will need
    // to change the datasource's scrape interval as well. 500ms intervals are known to work and you can
    // try go higher if you need to.
    // Collection and Sending could be parallelized or timed to ensure we're on a 15 seconds cadence,
    // not simply add 15 second to however long collection & sending took.
    delay(15000);


};

// DISPLAY
void showTextRectangle(String ln1, String ln2, boolean small) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  if (small) {
    display.setFont(ArialMT_Plain_16);
  } else {
    display.setFont(ArialMT_Plain_24);
  }
  display.drawString(32, 16, ln1);
  display.drawString(32, 36, ln2);
  display.display();
}
