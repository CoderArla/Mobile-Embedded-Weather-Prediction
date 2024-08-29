#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Constants
#define DHTPIN 2
#define DHTTYPE DHT22
#define RAIN_AO_PIN A2
#define RAIN_DO_PIN 4
#define LDRPIN A0

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// DHT Sensor
DHT dht(DHTPIN, DHTTYPE);

// Variables
unsigned long lastTime = 0;
unsigned long sunshineDuration = 0;

// Function prototypes
void updateSunshineDuration();


// Function prototype
int predictDecisionTree(float temperature_2m, float rain, float wind_speed_100m, float latitude, float surface_pressure, float wind_speed_10m, float relative_humidity_2m, float sunshine_duration, float coastline_distance);

void setup() {
    // Initialize the LCD
    lcd.init();
    lcd.backlight();
    
    // Initialize DHT sensor
    dht.begin();

    pinMode(A1, INPUT);
    pinMode(RAIN_AO_PIN, INPUT);
    pinMode(RAIN_DO_PIN, INPUT);
    
    // Initialize serial communication
    Serial.begin(9600);
}

void loop() {
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= 1000) {
        updateSunshineDuration();
        lastTime = currentTime;
    }
    // Read sensor data
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    float rain = analogRead(RAIN_AO_PIN); // Replace with actual rain sensor reading
    float wind_speed_100m = 30; // Replace with actual wind speed sensor reading
    float latitude = 22.57; // Replace with actual latitude value
    float surface_pressure = analogRead(A1); // Replace with actual pressure sensor reading
    float wind_speed_10m = 20; // Replace with actual wind speed at 10m sensor reading
    
    float coastline_distance = 94.58; // Replace with actual coastline distance value
    int lightLevel = analogRead(LDRPIN); // Read LDR value
    
    // Check if any reads failed
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Predict weather condition
    int weatherCondition = predictDecisionTree(temperature, rain, wind_speed_100m, latitude, surface_pressure, wind_speed_10m, humidity, sunshineDuration, coastline_distance);

    String state ;
    if(weatherCondition == 0){
      state = "Normal        ";
    }
    else if(weatherCondition == 1)
    {
      state="Heavy Rain";
    }
    else  if(weatherCondition ==2){
      state = "Storm";
    }
    else if(weatherCondition == 3){
      state = "High temperature";
    }

    // Display the result on LCD
    lcd.setCursor(0, 0);
    lcd.print("Rainfall: " + String(rain) + "mm");
    lcd.setCursor(0, 1);
    lcd.print("Cond: " + String(state));

    // Wait a few seconds before next reading
    delay(5000);
}

void updateSunshineDuration() {
    int ldrValue = analogRead(LDRPIN);
    if (ldrValue > 39) {
        sunshineDuration++;
    }
}

int predictDecisionTree(float temperature_2m, float rain, float wind_speed_100m, float latitude, float surface_pressure, float wind_speed_10m, float relative_humidity_2m, float sunshine_duration, float coastline_distance) {
    if (temperature_2m < 32.95) {
        if (rain < 39.6) {
            return 0; // Class 0
        } else {
            if (wind_speed_100m < 39.85) {
                return 1; // Class 1
            } else {
                if (temperature_2m < 26.05) {
                    if (rain < 45.5) {
                        if (temperature_2m < 25.5) {
                            return 2; // Class 2
                        } else {
                            if (latitude < 19.9297) {
                                return 1.5; // Class 1.5
                            } else {
                                return 1.5; // Class 1.5
                            }
                        }
                    } else {
                        if (temperature_2m < 24.1) {
                            return 1; // Class 1
                        } else {
                            if (wind_speed_10m < 25.3) {
                                return 1; // Class 1
                            } else {
                                return 1.44117647; // Class 1.44117647
                            }
                        }
                    }
                } else {
                    if (surface_pressure < 986.4) {
                        return 2; // Class 2
                    } else {
                        if (rain < 75.1) {
                            if (temperature_2m < 27.2) {
                                return 1.52941176; // Class 1.52941176
                            } else {
                                return 1; // Class 1
                            }
                        } else {
                            if (relative_humidity_2m < 91.5) {
                                return 1.66666667; // Class 1.66666667
                            } else {
                                return 2; // Class 2
                            }
                        }
                    }
                }
            }
        }
    } else {
        if (relative_humidity_2m < 64.5) {
            if (rain < 27.3) {
                return 0; // Class 0
            } else {
                return 1; // Class 1
            }
        } else {
            if (rain < 35.8) {
                return 3; // Class 3
            } else {
                if (sunshine_duration < 3379.33) {
                    return 1; // Class 1
                } else {
                    if (coastline_distance < 106.59499740600586) {
                        if (rain < 42.7) {
                            return 2; // Class 2
                        } else {
                            return 2; // Class 2
                        }
                    } else {
                        return 3; // Class 3
                    }
                }
            }
        }
    }
}
