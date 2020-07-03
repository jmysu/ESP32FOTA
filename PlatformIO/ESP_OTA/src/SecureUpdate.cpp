/*
   Example from ESP32 Arduino HttpUpdate 

   httpUpdateSecure.ino
    Created on: 16.10.2018 as an adaptation of the ESP8266 version of httpUpdate.ino

   Use this link to get correct root SSL certification:
      https://projects.petrucci.ch/esp32/?page=ssl

*/
#include <Update.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <time.h>

void printLocalTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("[PrintLocalTime]: Failed to obtain time");
        return;
        }
    Serial.println(&timeinfo, "\n[PrintLocalTime]: %A, %B %d %Y %H:%M:%S");
}

// Set time via NTP, as required for x.509 validation
void setClock() {
    configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");  // UTC

    Serial.print(F("Waiting for NTP time sync: "));
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) {
        yield();
        delay(500);
        Serial.print(F("."));
        now = time(nullptr);
        }

    Serial.println(F(""));
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print(F("Current time: "));
    Serial.println(asctime(&timeinfo));
}
// certificate for https://raw.githubusercontent.com
// DigiCert High Assurance EV Root CA, valid until Sun Oct 22 2028, size: 1688 bytes
const char* rootCACertificateRAWGitHub =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEsTCCA5mgAwIBAgIQBOHnpNxc8vNtwCtCuF0VnzANBgkqhkiG9w0BAQsFADBs\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\n"
    "ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowcDEL\n"
    "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\n"
    "LmRpZ2ljZXJ0LmNvbTEvMC0GA1UEAxMmRGlnaUNlcnQgU0hBMiBIaWdoIEFzc3Vy\n"
    "YW5jZSBTZXJ2ZXIgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2\n"
    "4C/CJAbIbQRf1+8KZAayfSImZRauQkCbztyfn3YHPsMwVYcZuU+UDlqUH1VWtMIC\n"
    "Kq/QmO4LQNfE0DtyyBSe75CxEamu0si4QzrZCwvV1ZX1QK/IHe1NnF9Xt4ZQaJn1\n"
    "itrSxwUfqJfJ3KSxgoQtxq2lnMcZgqaFD15EWCo3j/018QsIJzJa9buLnqS9UdAn\n"
    "4t07QjOjBSjEuyjMmqwrIw14xnvmXnG3Sj4I+4G3FhahnSMSTeXXkgisdaScus0X\n"
    "sh5ENWV/UyU50RwKmmMbGZJ0aAo3wsJSSMs5WqK24V3B3aAguCGikyZvFEohQcft\n"
    "bZvySC/zA/WiaJJTL17jAgMBAAGjggFJMIIBRTASBgNVHRMBAf8ECDAGAQH/AgEA\n"
    "MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIw\n"
    "NAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2Vy\n"
    "dC5jb20wSwYDVR0fBEQwQjBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQuY29t\n"
    "L0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENBLmNybDA9BgNVHSAENjA0MDIG\n"
    "BFUdIAAwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQ\n"
    "UzAdBgNVHQ4EFgQUUWj/kK8CB3U8zNllZGKiErhZcjswHwYDVR0jBBgwFoAUsT7D\n"
    "aQP4v0cB1JgmGggC72NkK8MwDQYJKoZIhvcNAQELBQADggEBABiKlYkD5m3fXPwd\n"
    "aOpKj4PWUS+Na0QWnqxj9dJubISZi6qBcYRb7TROsLd5kinMLYBq8I4g4Xmk/gNH\n"
    "E+r1hspZcX30BJZr01lYPf7TMSVcGDiEo+afgv2MW5gxTs14nhr9hctJqvIni5ly\n"
    "/D6q1UEL2tU2ob8cbkdJf17ZSHwD2f2LSaCYJkJA69aSEaRkCldUxPUd1gJea6zu\n"
    "xICaEnL6VpPX/78whQYwvwt/Tv9XBZ0k7YXDK/umdaisLRbvfXknsuvCnQsH6qqF\n"
    "0wGjIChBWUMo0oHjqvbsezt3tkBigAVBRQHvFwY+3sAzm2fTYS5yh+Rp/BIAV0Ae\n"
    "cPUeybQ=\n"
    "-----END CERTIFICATE-----\n"
    "";

int progressPercent = 0;
int last = 0;
long lastMillis;
void OnOtaProgress(int progress, int totalt) {
	progressPercent = (100 * progress) / totalt;
	if (last != progressPercent && progressPercent % 10 == 0) {
		//print every 10%
        Serial.print("\r[OTA:");
		Serial.print(progressPercent);
		Serial.print("%]");
        Serial.printf("@%ldsecs ", (millis()-lastMillis)/1000);
	}
	last = progressPercent;
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
}

void loop() {
    // wait for WiFi connection
    if ((WiFi.status() == WL_CONNECTED)) {
        setClock();

        WiFiClientSecure client;
        client.setCACert(rootCACertificateRAWGitHub);

        // Reading data over SSL may be slow, use an adequate timeout
        client.setTimeout(60000 / 1000);  // timeout argument is defined in seconds for setTimeout, Should be under 30" for 1024K binary

        // Add Progress%
        Update.onProgress(OnOtaProgress);
        // The line below is optional. It can be used to blink the LED on the board during flashing.  The LED will be on during download of one buffer of data from the network. The LED will
        // be off during writing that buffer to flash on a good connection the LED should flash regularly. On a bad connection the LED will be  on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
        // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
        // httpUpdate.setLedPin(LED_BUILTIN, HIGH);
        httpUpdate.setLedPin(2, HIGH);
        httpUpdate.rebootOnUpdate(false);  //Don't reboot after update completed!
        printLocalTime();
        lastMillis = millis();
        t_httpUpdate_return ret = httpUpdate.update(client, "https://raw.githubusercontent.com/jmysu/ESP32FOTA/master/firmware.bin");
        switch (ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                Serial.println("\n!!!HTTP_UPDATE_OK!!!");
                break;
            }
        printLocalTime();
        while (1) {
            delay(1);
            }
        }
}