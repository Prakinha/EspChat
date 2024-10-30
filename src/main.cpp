#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>


const char* ssid = "S23+";       
const char* password = "amopavao";  

// Credenciais do Access Point
const char* ap_ssid = "S23+";       
const char* ap_password = "amopavao";        

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool modoSimultaneo = false;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t * data, size_t len) {
    if (type == WS_EVT_DATA) {
        // Converte os dados recebidos em uma String
        String msg = "";
        for (size_t i = 0; i < len; i++) {
            msg += (char) data[i];
        }

        // Envia a mensagem para todos os clientes conectados
        ws.textAll(msg);
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    if (modoSimultaneo) {
        WiFi.mode(WIFI_AP_STA);

        WiFi.softAP(ap_ssid, ap_password);
        Serial.println("ESP32 em modo Access Point + Station (Simultâneo)");
        Serial.print("Endereço IP do Access Point: ");
        Serial.println(WiFi.softAPIP());

        WiFi.begin(ssid, password);
        Serial.println("Tentando conectar ao Wi-Fi...");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        Serial.println("\nConectado ao Wi-Fi!");
        Serial.print("Endereço IP da Station: ");
        Serial.println(WiFi.localIP());
    } else {
    bool wifiConnected = false;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("Tentando conectar ao Wi-Fi...");

    unsigned long startAttemptTime = millis();
    const unsigned long wifiTimeout = 10000; 

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        
        Serial.println("\nConectado ao Wi-Fi!");
        Serial.print("Endereço IP: ");
        Serial.println(WiFi.localIP());
        digitalWrite(BUILTIN_LED, HIGH);
        delay(500);
        digitalWrite(BUILTIN_LED, LOW);
        delay(500);
        digitalWrite(BUILTIN_LED, HIGH);
        delay(500);
        digitalWrite(BUILTIN_LED, LOW);
        delay(500);

    } else {
        wifiConnected = false;
        Serial.println("\nNão foi possível conectar ao Wi-Fi.");
        Serial.println("Iniciando modo Access Point...");

        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid, ap_password);
        Serial.print("Endereço IP do Access Point: ");
        Serial.println(WiFi.softAPIP());

        digitalWrite(BUILTIN_LED, HIGH);
        delay(500);
        digitalWrite(BUILTIN_LED, LOW);
        delay(500);
    }
    }
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", 
        "<!DOCTYPE html>"
        "<html lang='pt-br'>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>Discordo</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; }"
        "#chat { height: 300px; overflow-y: scroll; border: 1px solid #ccc; padding: 10px; }"
        "#input { width: calc(100% - 22px); }"
        "</style>"
        "</head>"
        "<body>"
        "<h1>Discordo</h1>"
        "<div id='chat'></div>"
        "<input type='text' id='input' placeholder='Digite uma mensagem...' onkeydown='if(event.key === \"Enter\"){sendMessage();}'>"
        "<button onclick='sendMessage()'>Enviar</button>"
        "<button onclick='changeUsername()'>Mudar Nome de Usuário</button>"
        "<script>"
        "var ws;"
        "var username = '';"
        "window.addEventListener('load', function() {"
        "    if (localStorage.getItem('username')) {"
        "        username = localStorage.getItem('username');"
        "    } else {"
        "        username = prompt('Por favor, insira seu nome de usuário:');"
        "        if (!username) { username = 'Usuário Anônimo'; }"
        "        localStorage.setItem('username', username);"
        "    }"
        "    document.title = 'Discordo - ' + username;"
        "    ws = new WebSocket('ws://' + window.location.hostname + '/ws');"
        "    ws.onmessage = function(event) {"
        "        var chat = document.getElementById('chat');"
        "        chat.innerHTML += event.data + '<br>';"
        "        chat.scrollTop = chat.scrollHeight;"
        "    };"
        "});"
        "function sendMessage() {"
        "    var input = document.getElementById('input');"
        "    if (input.value !== '') {"
        "        var date = new Date();"
        "        var time = ('0' + date.getHours()).slice(-2) + ':' + ('0' + date.getMinutes()).slice(-2);"
        "        var message = '<strong>[' + time + '] ' + username + ':</strong> ' + input.value;"
        "       if(input.value.toLowerCase() === 'pedro angelus'){"
        "var message = '<strong>PEDRO ANGELUS!!!!!</strong>';}"
        "       if(input.value.toLowerCase() === '>amostradinho'){"
        "var message = '┌( ͝° ͜ʖ͡°)=ε/̵͇̿̿/’̿’̿ ̿ ';}"
        "       if(input.value.toLowerCase() === '>calabreso' || input.value === '>calma calabreso'){"
        "var message = 'ƪ(ړײ)ƪ​​';}"
        "        ws.send(message);"
        "        input.value = '';"
        "    }"
        "}"
        "function changeUsername() {"
        "    username = prompt('Por favor, insira seu novo nome de usuário:');"
        "    if (!username) { username = 'Usuário Anônimo'; }"
        "    localStorage.setItem('username', username);"
        "    document.title = 'PedroChats - ' + username;"
        "}"
        "</script>"
        "</body>"
        "</html>");
    });

    server.begin();
}

void loop() {
    ws.cleanupClients();
}