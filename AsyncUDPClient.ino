#include "WiFi.h"
#include "AsyncUDP.h"
//#include "esp_wifi.h"

static const char* TAG = "MyModule";

const char * ssid = "MyESP32AP";
const char * password = "testpassword";
// Set your Static IP address
IPAddress local_IP(192, 168, 4, 2);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

AsyncUDP udp;

void setup()
{
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);
    // esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR );
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
    }

    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        ESP_LOGW("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    WiFi.setAutoReconnect(true);

    if(udp.listen(12345)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        

        udp.onPacket([](AsyncUDPPacket packet) {
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            Serial.print(":");
            Serial.print(packet.remotePort());
            Serial.print(", Data: ");
            Serial.write(packet.data(), packet.length());
            Serial.println();
            //reply to the client
            packet.printf("Got %u bytes of data", packet.length());
        });
    }
   
}

void loop()
{
    delay(2000);
    //Send broadcast til port 1234
    udp.broadcastTo("Er her nogen? æøå? Er her nogen? æøå? Er her nogen? æøå?", 1234);
   
}
