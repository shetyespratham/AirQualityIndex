#define TS_ENABLE_SSL
#define SECRET_SHA1_FINGERPRINT "27 18 92 DD A4 26 C3 07 09 B9 7A E6 C5 21 B9 5B 48 F7 16 E1"
#include <Arduino.h>
#include <SensirionI2CSen5x.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>

// The used commands use up to 48 bytes. On some Arduino's the default buffer
// space is not large enough
#define MAXBUF_REQUIREMENT 48

#if (defined(I2C_BUFFER_LENGTH) &&                 \
     (I2C_BUFFER_LENGTH >= MAXBUF_REQUIREMENT)) || \
    (defined(BUFFER_LENGTH) && BUFFER_LENGTH >= MAXBUF_REQUIREMENT)
#define USE_PRODUCT_INFO
#endif

char ssid[] = "Sampada_4G";
char pass[] = "Kailas250@668";
const char* myWriteAPIKey = "PZMYA92OQE5SEZH3";
unsigned long myChannelNumber = 2387861;
const char * fingerprint = SECRET_SHA1_FINGERPRINT;

SensirionI2CSen5x sen5x;
WiFiClientSecure client;

const char* rootCA = R"(-----BEGIN CERTIFICATE-----
MIIE9DCCA9ygAwIBAgIQCF+UwC2Fe+jMFP9T7aI+KjANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH
MjAeFw0yMDA5MjQwMDAwMDBaFw0zMDA5MjMyMzU5NTlaMFkxCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh
bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD
ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV
cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy
FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc
3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8
osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT
zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAa4wggGqMB0GA1Ud
DgQWBBR0hYDAZsffN97PvSk3qgMdvu3NFzAfBgNVHSMEGDAWgBROIlQgGJXm427m
D/r6uRLtBhePOTAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUHAwEG
CCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwdgYIKwYBBQUHAQEEajBoMCQG
CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG
NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH
Mi5jcnQwewYDVR0fBHQwcjA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t
L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA3oDWgM4YxaHR0cDovL2NybDQuZGln
aWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDAwBgNVHSAEKTAnMAcG
BWeBDAEBMAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEB
CwUAA4IBAQB1i8A8W+//cFxrivUh76wx5kM9gK/XVakew44YbHnT96xC34+IxZ20
dfPJCP2K/lHz8p0gGgQ1zvi2QXmv/8yWXpTTmh1wLqIxi/ulzH9W3xc3l7/BjUOG
q4xmfrnti/EPjLXUVa9ciZ7gpyptsqNjMhg7y961n4OzEQGsIA2QlxK3KZw1tdeR
Du9Ab21cO72h2fviyy52QNI6uyy/FgvqvQNbTpg6Ku0FUAcVkzxzOZGUWkgOxtNK
Aa9mObm9QjQc2wgD80D8EuiuPKuK1ftyeWSm4w5VsTuVP61gM2eKrLanXPDtWlIb
1GHhJRLmB7WqlLLwKPZhJl5VHPgB63dx
-----END CERTIFICATE-----
)";

void printModuleVersions() {
    uint16_t error;
    char errorMessage[256];

    unsigned char productName[32];
    uint8_t productNameSize = 32;

    error = sen5x.getProductName(productName, productNameSize);

    if (error) {
        Serial.print("Error trying to execute getProductName(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("ProductName:");
        Serial.println((char*)productName);
    }

    uint8_t firmwareMajor;
    uint8_t firmwareMinor;
    bool firmwareDebug;
    uint8_t hardwareMajor;
    uint8_t hardwareMinor;
    uint8_t protocolMajor;
    uint8_t protocolMinor;

    error = sen5x.getVersion(firmwareMajor, firmwareMinor, firmwareDebug,
                             hardwareMajor, hardwareMinor, protocolMajor,
                             protocolMinor);
    if (error) {
        Serial.print("Error trying to execute getVersion(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Firmware: ");
        Serial.print(firmwareMajor);
        Serial.print(".");
        Serial.print(firmwareMinor);
        Serial.print(", ");

        Serial.print("Hardware: ");
        Serial.print(hardwareMajor);
        Serial.print(".");
        Serial.println(hardwareMinor);
    }
}

void printSerialNumber() {
    uint16_t error;
    char errorMessage[256];
    unsigned char serialNumber[32];
    uint8_t serialNumberSize = 32;

    error = sen5x.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("SerialNumber:");
        Serial.println((char*)serialNumber);
    }
}

void setup() {

    Serial.begin(115200);
    while (!Serial) {
      ;
    }
    Wire.begin();
    sen5x.begin(Wire);
    WiFi.mode(WIFI_STA);
//    client.setFingerprint(fingerprint);
//    client.getFingerprintSHA256(uint8_t *sha256_result);
    client.setCACert(rootCA);
 //   delay(3000);
    ThingSpeak.begin(client);
    if(WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      while(WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        Serial.print(".");
        delay(15000);
        Serial.println(WiFi.status());
      }
      Serial.println("\nConnected.");
    }
    uint16_t error;
    char errorMessage[256];
    error = sen5x.deviceReset();
    if (error) {
        Serial.print("Error trying to execute deviceReset(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

// Print SEN55 module information if i2c buffers are large enough
#ifdef USE_PRODUCT_INFO
    printSerialNumber();
    printModuleVersions();
#endif

    float tempOffset = 0.0;
    error = sen5x.setTemperatureOffsetSimple(tempOffset);
    if (error) {
        Serial.print("Error trying to execute setTemperatureOffsetSimple(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Temperature Offset set to ");
        Serial.print(tempOffset);
        Serial.println(" deg. Celsius (SEN54/SEN55 only");
    }

    // Start Measurement
    error = sen5x.startMeasurement();
    if (error) {
        Serial.print("Error trying to execute startMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
}

void loop() {
    uint16_t error;
    char errorMessage[256];

    delay(60000);

    // Read Measurement
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = sen5x.readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
        Serial.print("Error trying to execute readMeasuredValues(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        if(WiFi.status() == WL_CONNECTED) {
          ThingSpeak.setField(1, ambientTemperature); //temperature
          ThingSpeak.setField(2, ambientHumidity); //Humidity
          ThingSpeak.setField(3, massConcentrationPm2p5); //2.5
          ThingSpeak.setField(4, massConcentrationPm10p0); //10
          ThingSpeak.setField(5, massConcentrationPm1p0); //1.0
          ThingSpeak.setField(6, massConcentrationPm4p0); //4.0
          ThingSpeak.setField(7, vocIndex); //voc
          ThingSpeak.setField(8, noxIndex); //nox
          int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
          // ThingSpeak.writeFields(unsigned long channelNumber, mywriteAPIKey);
          if(x == 200) {
            Serial.println("Channel update successful");
          } else {
            Serial.println("Problem updating channel");
            Serial.println(x);
          }
        }
        Serial.print("MassConcentrationPm1p0:");
        Serial.print(massConcentrationPm1p0);
        Serial.print("\t");
        Serial.print("MassConcentrationPm2p5:");
        Serial.print(massConcentrationPm2p5);
        Serial.print("\t");
        Serial.print("MassConcentrationPm4p0:");
        Serial.print(massConcentrationPm4p0);
        Serial.print("\t");
        Serial.print("MassConcentrationPm10p0:");
        Serial.print(massConcentrationPm10p0);
        Serial.print("\t");
        Serial.print("AmbientHumidity:");
        if (isnan(ambientHumidity)) {
            Serial.print("n/a");
        } else {
            Serial.print(ambientHumidity);
        }
        Serial.print("\t");
        Serial.print("AmbientTemperature:");
        if (isnan(ambientTemperature)) {
            Serial.print("n/a");
        } else {
            Serial.print(ambientTemperature);
        }
        Serial.print("\t");
        Serial.print("VocIndex:");
        if (isnan(vocIndex)) {
            Serial.print("n/a");
        } else {
            Serial.print(vocIndex);
        }
        Serial.print("\t");
        Serial.print("NoxIndex:");
        if (isnan(noxIndex)) {
            Serial.println("n/a");
        } else {
            Serial.println(noxIndex);
        }
    }
}