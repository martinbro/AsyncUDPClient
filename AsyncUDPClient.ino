#include "WiFi.h"
#include "AsyncUDP.h"

const char * ssid = "MyESP32AP";// Skal aftales
const char * password = "testpassword";// Skal aftales

// Tildeler fast Ip-adresse til at identificere 'modulet' 
IPAddress local_IP(192, 168, 4, 3);// Skal aftales
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

WiFiUDP UDP;//KUN HVIS Unicast
AsyncUDP udp;

void setup()
{
    //0. starter seriel kommunikation
    Serial.begin(115200);
    while (!Serial){ delay(10);}

    // 1. Sætter lokal access point op. Formålet er at kommunikere med de andre ESPér
    WiFi.mode(WIFI_STA);

    
    // 2. sætter den faste IPadresse op defineret ovenfor 
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
        Serial.println();
        udp.onPacket([](AsyncUDPPacket packet) {
            // Serial.print("UDP Packet Type: ");
            Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
            Serial.print(", From: ");
            Serial.print(packet.remoteIP());
            //Flere eksempler på 'metakommunikation'
            // Serial.print(":");
            // Serial.print(packet.remotePort());
            // Serial.print(", To: ");
            // Serial.print(packet.localIP());
            // Serial.print(":");
            // Serial.print(packet.localPort());
            Serial.print(", Data: ");
            
            // Serial.write(packet.data(), packet.length());

            if (packet.length() > 0)
            {
                char buffer[packet.length()];
                //cast´er fra byte-array til char-array
                sprintf(buffer, "%s", packet.data());
                buffer[packet.length()] ='\0';// Da unicast ikke er '\0' termineret   
                // String testString = String( (char*) packet.data());
                // Serial.printf("%s\n",buffer);
                Serial.println(buffer);
            }
            
            Serial.println();
            // evt auto svar til server
            // packet.printf(" %u bytes of data", packet.length());
        });
    }
}

void loop()
{
    delay(1000);//ALDRIG delay i produktion
    //Send broadcast  til port 1234
    udp.broadcastTo("Hej server - eller andre moduler med fast portnummer på", 1234);
    delay(1000);//ALDRIG delay i produktion

    //unicast til IP "192.168.4.1" og port 1234
    UDP.beginPacket("192.168.4.1",1234);
    UDP.print("Unicast Besked fra Esp32 batterimodul til server og KUN server");//low level kom. skal '/0' termineres
    UDP.endPacket();
    UDP.beginPacket("192.168.4.1",1234);
    UDP.print(12345678);
    UDP.endPacket();
}