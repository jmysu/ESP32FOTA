#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <esp_https_ota.h>
#include <StreamString.h>

//SSL Root CA exported from Firefox
const char* root_ca_github =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrTCCApWgAwIBAgICSC8wDQYJKoZIhvcNAQELBQAwYjELMAkGA1UEBhMCQ1ox\n"
    "DTALBgNVBAgMBEJybm8xDDAKBgNVBAoMA0FWRzEdMBsGA1UECwwUU29mdHdhcmUg\n"
    "RGV2ZWxvcG1lbnQxFzAVBgNVBAMMDkFWRyB0cnVzdGVkIENBMB4XDTIwMDUwNTAw\n"
    "MDAwMFoXDTIyMDUxMDEyMDAwMFowZjELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNh\n"
    "bGlmb3JuaWExFjAUBgNVBAcTDVNhbiBGcmFuY2lzY28xFTATBgNVBAoTDEdpdEh1\n"
    "YiwgSW5jLjETMBEGA1UEAxMKZ2l0aHViLmNvbTCCASIwDQYJKoZIhvcNAQEBBQAD\n"
    "ggEPADCCAQoCggEBAMFKf7oaAJJyF/iOaKvsX+JgRNjW9ZdlhTXtbajNWpoLaPIn\n"
    "1zjqhqbBOCv9ogWsTQ07S/9lJWPpmP3qInI1718dduX42jBoHFRmWL4nRqSnJS/O\n"
    "l42hJfTotng0wKrjazOKpfIpA6b1A02fgB2uo5Q2uaBCy2+Ph7AgE1c9JLhmxFif\n"
    "YW2bfLVIX7bGjXxGI0E5rMEcuHecFednWDlhvluCYR1P+zxaF7ZdqpMb/kJlKP7X\n"
    "nlEMtM8zRAxU0g+ADGcSF+GdauaFG5TWlMrxmPDducaI8ksonfWJUmgoDotqquEM\n"
    "MqHvIUr1XU8/3MMZeEflfcQD1bbIgl3NyswtgL0CAwEAAaNpMGcwHQYDVR0OBBYE\n"
    "FLj32/gYTOtPcYKzeXt4HiqXgxtCMB8GA1UdIwQYMBaAFLFH/8fv4+CppOKmYh8l\n"
    "PUE1MhwhMCUGA1UdEQQeMByCCmdpdGh1Yi5jb22CDnd3dy5naXRodWIuY29tMA0G\n"
    "CSqGSIb3DQEBCwUAA4IBAQBLIgU0EpAst2JpZDBny5DkuF2BBh0TV9KyH1mAdY5x\n"
    "IRsWVqGGcWDAXfTSf2eWkEqE4dLJlKuSe27qfiBasx7M6I653GyFISKc6xx3M7wU\n"
    "17FePzs/835vrs3yOL+axPsabkATxQnKRhNUMG5HhNTOSbAxwifYpdGSiwf3J0Pa\n"
    "/BdGMMIMC8wDVDEtPWTeZVVEXSD2MQES9KDNFLsxPLMeVsgae6qbHuvTGk2M9Zml\n"
    "kPfAqUvPgyNaK6P/B3VnNwtnAas8hxZWY46HmeZjC2g3Wq9BKWAl0nMyfScLoACF\n"
    "E/K1E2EU29rmoKYMYCYyFy3rG4d/4sJZR/TWpCg9V96j\n"
    "-----END CERTIFICATE-----\n";
const char* root_ca_githubSSL =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIECTCCAvGgAwIBAgICSBMwDQYJKoZIhvcNAQELBQAwYjELMAkGA1UEBhMCQ1ox\n"
    "DTALBgNVBAgMBEJybm8xDDAKBgNVBAoMA0FWRzEdMBsGA1UECwwUU29mdHdhcmUg\n"
    "RGV2ZWxvcG1lbnQxFzAVBgNVBAMMDkFWRyB0cnVzdGVkIENBMB4XDTIwMDUwNjAw\n"
    "MDAwMFoXDTIyMDQxNDEyMDAwMFowajELMAkGA1UEBhMCVVMxEzARBgNVBAgTCkNh\n"
    "bGlmb3JuaWExFjAUBgNVBAcTDVNhbiBGcmFuY2lzY28xFTATBgNVBAoTDEdpdEh1\n"
    "YiwgSW5jLjEXMBUGA1UEAxMOd3d3LmdpdGh1Yi5jb20wggEiMA0GCSqGSIb3DQEB\n"
    "AQUAA4IBDwAwggEKAoIBAQDBSn+6GgCSchf4jmir7F/iYETY1vWXZYU17W2ozVqa\n"
    "C2jyJ9c46oamwTgr/aIFrE0NO0v/ZSVj6Zj96iJyNe9fHXbl+NowaBxUZli+J0ak\n"
    "pyUvzpeNoSX06LZ4NMCq42sziqXyKQOm9QNNn4AdrqOUNrmgQstvj4ewIBNXPSS4\n"
    "ZsRYn2Ftm3y1SF+2xo18RiNBOazBHLh3nBXnZ1g5Yb5bgmEdT/s8Whe2XaqTG/5C\n"
    "ZSj+155RDLTPM0QMVNIPgAxnEhfhnWrmhRuU1pTK8Zjw3bnGiPJLKJ31iVJoKA6L\n"
    "aqrhDDKh7yFK9V1PP9zDGXhH5X3EA9W2yIJdzcrMLYC9AgMBAAGjgcAwgb0wHQYD\n"
    "VR0OBBYEFLj32/gYTOtPcYKzeXt4HiqXgxtCMB8GA1UdIwQYMBaAFLFH/8fv4+Cp\n"
    "pOKmYh8lPUE1MhwhMHsGA1UdEQR0MHKCDnd3dy5naXRodWIuY29tggwqLmdpdGh1\n"
    "Yi5jb22CCmdpdGh1Yi5jb22CCyouZ2l0aHViLmlvgglnaXRodWIuaW+CFyouZ2l0\n"
    "aHVidXNlcmNvbnRlbnQuY29tghVnaXRodWJ1c2VyY29udGVudC5jb20wDQYJKoZI\n"
    "hvcNAQELBQADggEBAKwKtCa2aBcSW5ZDPWsnDq1f3ukZHFh16GL8RwO6vwxsMaoB\n"
    "CIT7JFVJSLc/R0G1VS8FMra8VAb1muRND5f8Gl+M1A+pPA+meKvFHxiV8CPW8zSz\n"
    "oYpGtnwdKWLP1KjxqI95I5OVoVBU7nGveAkbBzHzFv1TdhvPul8op+usqInd4oPX\n"
    "lTufONPR1V79UzdVkTjAcNFNKGwAl9fmnDMo2BI3KSTTQDQB9KBqTQ/TjjQ1RmNq\n"
    "lPiMracJjsmMdDaM+tcLwIDPVGlx+iU7PMiJTttRFt2Ir2obwdq/2Gi2dRq6uBM9\n"
    "CyrN1yO6Dnm5wN218QjWYvklxN5wVj5d03jt6yU=\n"
    "-----END CERTIFICATE-----\n";

// Method to compare two versions.
// Returns 1 if v1 is bigger;
//        -1 if v1 is smaller;
//         0 if equal
int versionCompare(String v1, String v2) {
    // vnum stores each numeric part of version
    int vnum1 = 0, vnum2 = 0;

    // loop untill both string are processed
    for (int i = 0, j = 0; (i < v1.length() || j < v2.length());) {
        // storing numeric part of version 1 in vnum1
        while (i < v1.length() && v1[i] != '.') {
            vnum1 = vnum1 * 10 + (v1[i] - '0');
            i++;
        }
        // storing numeric part of version 2 in vnum2
        while (j < v2.length() && v2[j] != '.') {
            vnum2 = vnum2 * 10 + (v2[j] - '0');
            j++;
        }

        if (vnum1 > vnum2)
            return 1;
        if (vnum2 > vnum1)
            return -1;

        // if equal, reset variables and go for next numeric part
        vnum1 = vnum2 = 0;
        i++;
        j++;
    }
    return 0;
}

//Check target description json file
//
//Return cbBinPath when sMyName found and sMyVer is less than description version
//
int httpsCheckJson(String sMyName, String sMyVer, String sJsonPath, char* cpHost, char* cpBinPath) {
    HTTPClient http;
    int iRet = 0;
    http.begin(sJsonPath, root_ca_github);
    int httpCode = http.GET();  //Make the request
    Serial.printf("Http Returned Code:%d\n", httpCode);
    /* HTTP 狀態碼表明一個 HTTP 要求是否已經被完成。回應分為五種：
          資訊回應 (Informational responses, 100–199),
          成功回應 (Successful responses, 200–299),
          重定向 (Redirects, 300–399),
          用戶端錯誤 (Client errors, 400–499),
          伺服器端錯誤 (Server errors, 500–599).
        */
    if (httpCode > 0) {  //Check for the returning code
        String jPayload = http.getString();
        //Serial.println(httpCode);
        Serial.println(jPayload);

        //Convert payload into json
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, jPayload);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            iRet = -9;
            ;
        }
        //Retrieve array from json doc
        JsonArray array = doc["files"];
        serializeJson(array, Serial);

        for (int i = 0; i < array.size(); i++) {
            JsonObject jObj = array[i].as<JsonObject>();
            Serial.printf("\nFiles[%d]", i);
            //if (array[i].containsKey("name") && array[i]["name"]==sMyName) Serial.print("***");

            // using C++11 syntax to itrate objects
            for (JsonPair p : jObj) {
                const char* key = p.key().c_str();
                JsonVariant value = p.value();
                const char* data = value.as<char*>();
                Serial.printf("\n\t[%s] \t= %s", key, data);
                if ((p.key() == "name") && (p.value() == sMyName)) Serial.print(" ***");
                if ((array[i]["name"] == sMyName) && (p.key() == "ver")) {  //Found target file name, and this variant is ver
                    iRet = versionCompare(sMyVer, array[i]["ver"]);
                    if (iRet < 0) {
                        Serial.print(" > ");  // Remote version > current version

                        String sHost = array[i]["host"].as<String>();
                        String sPath = array[i]["bin"].as<String>();

                        memcpy(cpHost, sHost.c_str(), sHost.length());  //send sHost to cpHost
                        //Serial.printf("\nGot Host:%s", cpHost);
                        memcpy(cpBinPath, sPath.c_str(), sPath.length());  //send sPath to cpBinPath
                        //Serial.printf("\nGot Bin:%s", cpBinPath);
                    } else if (iRet > 0)
                        Serial.print(" < ");
                    else
                        Serial.print(" = ");

                    Serial.printf("[%s]", sMyVer.c_str());
                }
            }
        }
        Serial.println();
    }
    http.end();  //Free the resources

    return iRet;
}

/*

WiFiClientSecure client;
client.setInsecure();

//You must use https://raw.githubusercontent.com, this is for download raw data from GitHub's public repos, just the file. Your full link to the file must be:

#define repo "https://raw.githubusercontent.com/<user>/<repo>/master/<path to the .bin>"
t_httpUpdate_return ret = ESPhttpUpdate.update(client, repo);


void httpsUpdate(String sHost, String sPath) {
    WiFiClientSecure client;
    HTTPClient http;
    http.begin(sHost, root_ca_githubSSL);

    //client.setCACert(root_ca_githubSSL);
    // Reading data over SSL may be slow, use an adequate timeout
    client.setTimeout(600000 / 1000);  // timeout argument is defined in seconds for setTimeout

    //httpUpdate.setLedPin(LED_BUILTIN, HIGH);
    httpUpdate.setLedPin(2, HIGH);

    t_httpUpdate_return ret = httpUpdate.update(client, sHost + sPath);
    switch (ret) {
        case HTTP_UPDATE_FAILED:
            Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            Serial.println("HTTP_UPDATE_NO_UPDATES");
            break;

        case HTTP_UPDATE_OK:
            Serial.println("HTTP_UPDATE_OK");
            break;
    }
}*/

bool PerformOTA(String& binURL) {
    log_d("Fetching OTA from: %s", binURL.c_str());

    if (binURL.length() == 0) {
        return false;
    }

    bool isSSL = binURL.startsWith("https");
    if (!isSSL) {
        log_e("Trying to access a non-ssl URL on a secure update checker");
        return false;
    }

    HTTPClient httpClient;
    if (!httpClient.begin(binURL)) {
        log_e("Error initializing client");
        return false;
    }

    const auto httpCode = httpClient.GET();
    if (httpCode != HTTP_CODE_OK) {
        log_e("[HTTP] [ERROR] [%d] %s",
              httpCode,
              httpClient.errorToString(httpCode).c_str());
        log_d("Response:\n%s",
              httpClient.getString().c_str());
        return false;
    }

    const auto payloadSize = httpClient.getSize();
log_v("\nPayLoad Size:%d", payloadSize);   
 
    auto& payloadStream = httpClient.getStream();
    const bool canBegin = Update.begin(payloadSize, 0, 2, HIGH); //LED2 HIGH

    if (payloadSize <= 0) {
        log_e("Fetched binary has 0 size");
        return false;
    }

    if (!canBegin) {
        log_e("Not enough space to begin OTA");
        return false;
    }
log_v("\nWritStream...");
    const auto written = Update.writeStream(payloadStream);
    if (written != payloadSize) {
        log_e("Error. Written %lu out of %lu", written, payloadSize);
        return false;
    }

    if (!Update.end()) {
        StreamString errorMsg;
        Update.printError(errorMsg);
        log_e("Error Occurred: %s", errorMsg.c_str());
        return false;
    }

    if (!Update.isFinished()) {
        StreamString errorMsg;
        Update.printError(errorMsg);
        log_e("Undefined OTA update error: %s", errorMsg.c_str());
        return false;
    }

    log_i("Update completed. Rebooting");
    ESP.restart();
    return true;
}