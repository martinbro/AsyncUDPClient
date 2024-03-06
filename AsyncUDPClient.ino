#include "WiFi.h"
#include "AsyncUDP.h"

#define LED 2

// const char* ssid = "02495500"; //Enter SSID
// const char* password = "00809748"; //Enter Password
const char* ssid = "MyESP32AP"; //Enter SSID
const char* password = "testpassword"; //Enter Password

// Tildeler fast Ip-adresse til at identificere 'modulet' 
IPAddress local_IP(192, 168, 4, 3); 
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

int t[7] = {0,250, 243, 320, 331, 270, 400};
int v[4] = {135, 32, 41,35};
int I[1] = {120};
int i = 0;

WiFiUDP UDP;//KUN HVIS Unicast
AsyncUDP udp;
unsigned long time_last_mesurement = 0;

void setup()
{
    pinMode(LED,OUTPUT);

    //0. starter seriel kommunikation
    Serial.begin(115200);
    while (!Serial){ delay(10);}

    // // 1. Sætter lokal access point op. Formålet er at kommunikere med de andre ESPér
    WiFi.mode(WIFI_STA);

    
    // // 2. sætter den faste IPadresse op defineret ovenfor 
    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("STA Failed to configure");
        //ESP.restart(); //Respons 'Erlang-style'
    }

    // 3. Starter accespoint
    WiFi.begin(ssid, password);
    Serial.print("Venter på forbindelse");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        //ESP.restart(); Som alternativ - 'Erlang-style'
    }

    // 4. lytter efter indkommen beskeder på port:22345 ... Vi har 65.535 portnumbers at vælge imellem.
    if(udp.listen(WiFi.localIP(),22345)) {
        Serial.print("UDP Listening on IP: ");
        Serial.println(WiFi.localIP());
        udp.onPacket([](AsyncUDPPacket packet) {
            // Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            //Flere eksempler på 'metakommunikation'
            Serial.print(":");
            Serial.print(packet.remotePort());
            // Serial.print(", To: ");
            // Serial.print(packet.localIP());
            // Serial.print(":");
            // Serial.print(packet.localPort());
            Serial.print(", Data: ");
            
            Serial.write(packet.data(), packet.length());
                    Serial.println();

            if (packet.length() > 0)
            {
                char buffer[packet.length()+1];
                // //cast´er fra byte-array til char-array
                if(!packet.isBroadcast()){
                    sprintf(buffer, "%s", packet.data());
                    buffer[packet.length()] ='\0';// Da unicast ikke er '\0' termineret   
                    Serial.println("HER:");
                    Serial.println(buffer);
                    Serial.println(atoi(buffer));
                    Serial.println();
                }else{
                // Hvis vi KUN bruger broadcast
                String testString = String( (char*) packet.data());
                Serial.println(testString);

                }


            }
            
            Serial.println();
            // evt auto svar til server
            // packet.printf(" %u bytes of data", packet.length());
        });
    }
    
}

void loop()
{
    
    if (millis() - time_last_mesurement > 1000)
    {
        char buffer[128];
        t[1]+=random(3)-1;
        t[2]+=random(3)-1;
        t[3]+=random(3)-1;
        t[4]+=random(3)-1;
        t[5]+=random(3)-1;
        t[6]+=random(3)-1;
        v[0]+=random(3)-1;
        v[1]+=random(3)-1;
        v[2]+=random(3)-1;
        v[3]+=random(3)-1;
        I[0]+=random(3)-1;

        sprintf(buffer,"t:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",i++,t[1],t[2],t[3],t[4],t[5],t[6],v[0],v[1],v[2],v[3],I[0]);
        // Serial.print("Sender ny besked: ");
        // Serial.println(buffer);
        time_last_mesurement = millis();
        // client.send(buffer);
        udp.broadcastTo(buffer, 1234);
    }

}