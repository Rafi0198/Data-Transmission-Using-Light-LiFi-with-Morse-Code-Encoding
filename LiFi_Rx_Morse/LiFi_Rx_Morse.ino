#define LED_PIN 2
#define LDR_PIN 4
#define THRESHOLD 4000
#define PERIOD 15
#define BUZZER_PIN 23
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 20,4); 

bool previous_state;
bool current_state;
String receivedText = "";  // Use String to dynamically allocate space
//int index = 0;
byte lcd_col=0; byte lcd_row=1; 

// Morse Code Dictionary
const char* morseCode[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
    ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
    "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "-----", 
    ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----."
};

// Function prototypes
bool get_ldr();
char get_byte();
void printText();
void playMorseCode(const String &text);
void playTone(const char* code);
void playDot(); // Function to play a dot in Morse code
void printMorseCode(const char* code); // Function to print Morse code
void playWordGap();  // Corrected word gap with -....-

void setup() {
    Serial.begin(9600);
    lcd.init(); 
    lcd.backlight(); 
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LDR_PIN, INPUT);

    lcd.setCursor(0, 0);
    lcd.print("WELCOME TO RAFI'S");
    lcd.setCursor(0, 1);
    lcd.print("LiFi BASED Tx-Rx");
    lcd.setCursor(0, 2);
    lcd.print("UNDERGRAD STUDENT");
    lcd.setCursor(0, 3);
    lcd.print("DEPT. OF EEE, BUET");
    delay(2000); 
    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print("LiFi DECODED RX MSG:");
    
   
}

void loop() {

  
    current_state = get_ldr();
//    lcd.setCursor(0, 0);
//    lcd.print("LiFi DECODED RX MSG:");
    
    if (!current_state && previous_state) {
        char receivedChar = get_byte();
        
        if (receivedChar == '\n') {  // End of transmission (message delimiter)
            Serial.print("\nReceived Text: ");
            Serial.println(receivedText);  // Print the full received text
            playMorseCode(receivedText);  // Convert and play Morse code
            receivedText = "";  // Reset for next message
            digitalWrite(LED_PIN,LOW); 
        } 
        else {
            receivedText += receivedChar;  // Append the received character
            Serial.print(receivedChar);  // Print as received
            
            printToLCD(receivedChar);
            digitalWrite(LED_PIN,HIGH);
            //Serial.print(analogRead(4));
        }
    }
    
    previous_state = current_state;
}

void printToLCD(char my_byte) {
    //Serial.print(my_byte); // Print to serial monitor
    
    lcd.setCursor(lcd_col, lcd_row);
    lcd.print(my_byte);
    lcd_col++;

    if (lcd_col >= 20) {  // Move to next row if column limit is reached
        lcd_col = 0;
        lcd_row++;
    }
    if (lcd_row >= 4) { // Clear screen if row limit is reached
        lcd.clear();
        lcd_row = 0;
    }
}

// Read LDR state
bool get_ldr() {
    int voltage = analogRead(LDR_PIN);
    //Serial.println(voltage);
    return voltage > THRESHOLD ? true : false;
}

// Receive a character byte
char get_byte() {
    char ret = 0;
    delay(PERIOD * 1.5);
    for (int i = 0; i < 8; i++) {
        ret = ret | get_ldr() << i;
        delay(PERIOD);
    }
    return ret;
}

// Convert received text to Morse and play
void playMorseCode(const String &text) {
    Serial.print("\nMorse Code: ");

    for (int i = 0; i < text.length(); i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            playTone(morseCode[text[i] - 'A']);
            printMorseCode(morseCode[text[i] - 'A']);
        } 
        else if (text[i] >= 'a' && text[i] <= 'z') {
            playTone(morseCode[text[i] - 'a']);
            printMorseCode(morseCode[text[i] - 'a']);
        } 
        else if (text[i] >= '0' && text[i] <= '9') {
            playTone(morseCode[text[i] - '0' + 26]);
            printMorseCode(morseCode[text[i] - '0' + 26]);
        } 
        else if (text[i] == ' ') {
            printMorseCode("-....-");  // Correct word gap as -....-
            playWordGap();  // Play correct word gap as -....-
        }
    }

    Serial.println("");
}

// Play Morse Code using the buzzer
void playTone(const char* code) {
    for (int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '.') {
            playDot();  // Play a dot
        } 
        else if (code[i] == '-') {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(200);  // Dash duration (now faster)
        }
        digitalWrite(BUZZER_PIN, LOW);
        delay(50);  // Short intra-character gap (dot-dash)
    }
    delay(100);  // Reduced inter-character gap between characters
}

// Play a single dot (.)
void playDot() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);  // Dot duration (now faster)
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);  // Short intra-character gap
}

// Play 7 dots as the gap between words (now corrected to -....- with buzzer)
void playWordGap() {
    for (int i = 0; i < 7; i++) {
        playDot();  // Play 7 dots to indicate the word gap
    }
}

// Function to print the Morse code
void printMorseCode(const char* code) {
    Serial.print(code);  // Print the Morse code for the character
    Serial.print(" ");
}
