#include <WiFi.h>

#define LED_PIN 23
#define PERIOD 15

const char *ssid = "ESP32_TX";   // WiFi Name (ESP32 as a hotspot)
const char *password = "12345678";   // WiFi Password

WiFiServer server(80);  // Start a TCP server on port 80

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);

    WiFi.softAP(ssid, password);  // Start WiFi in Access Point mode
    Serial.println("ESP32 WiFi Server Started!");
    Serial.print("Connect to WiFi: ");
    Serial.println(ssid);
    digitalWrite(LED_PIN,HIGH);

    server.begin();  // Start the TCP server
}

void loop() {
    WiFiClient client = server.available(); // Wait for a client connection

    if (client) {
        Serial.println("Client Connected!");
        String inputString = ""; // To store the received string

        while (client.connected()) {
            while (client.available()) {  // Read incoming data
                char receivedChar = client.read();
                if (receivedChar == '\n') {  // End of message (newline)
                    Serial.print("Received: ");
                    Serial.println(inputString);  // Print received text
                    client.print("ESP32 Received: " + inputString + "\n"); // Acknowledge back

                    // Send the received string as LED signals
                    for (int i = 0; i < inputString.length(); i++) {
                        send_byte(inputString[i]);
                    }
                    send_byte('\n');  // Send newline as end of message
                    
                    inputString = ""; // Reset for next message
                } 
                else {
                    inputString += receivedChar; // Append character to string
                }
            }
        }
        client.stop();  // Close the connection
        Serial.println("Client Disconnected.");
    }
}

// Send a character byte via LED
void send_byte(char my_byte) {
    digitalWrite(LED_PIN, LOW);
    delay(PERIOD);

    for (int i = 0; i < 8; i++) {
        digitalWrite(LED_PIN, (my_byte & (0x01 << i)) != 0);
        delay(PERIOD);
    }

    digitalWrite(LED_PIN, HIGH);
    delay(PERIOD);
}
