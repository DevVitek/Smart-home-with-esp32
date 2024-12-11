#include <WiFi.h>

const char *ssid = "dulik";  // Your WiFi SSID
const char *password = "asdjkl1234567890";  // Your WiFi Password
const int LED1 = 21;  // Pin connected to the single LED
const int RED_PIN = 18;  // Pin connected to the red channel of the RGB LED
const int GREEN_PIN = 5;  // Pin connected to the green channel of the RGB LED
const int BLUE_PIN = 4;  // Pin connected to the blue channel of the RGB LED

WiFiServer server(80);  // Create a server that listens on port 80

// Webpage content
String webpage = R"rawliteral(
<!DOCTYPE html>
<html> 
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">

    <title>Web interface</title>
    <style> 
        body {
            margin: 0;
            background-color: rgb(36, 36, 36);
            padding: 0;
            font-family: Arial, sans-serif;
            text-align: center;
        }
        h1 {
            text-align: center;
            font-family: Arial, sans-serif;
            background-color: rgb(0, 57, 110);
            color: white;
            padding: 10px 20px; 
            margin: 0;
        }
        p {
            font-family: Arial, sans-serif;
            color: white;
            margin: 20px;
            text-align: center;
            font-size: 20px;
            margin-top: 60px;
        }
       
        

        button:hover {
            background-color: rgb(0, 66, 128);
        }

        button {
            text-align: center;
            font-family: Arial, sans-serif;
            background-color: rgb(0, 94, 182);
            color: white;
            padding: 35px 70px;
            border: none;
            border-radius: 5px;
            font-size: 16px;
            margin: 10px;
            cursor: pointer;
        }
        .slider{
            width: 300px; 
            margin-top: 30px;

        }

        h2 {
            margin: 0;
        }
      
    </style>
</head>
<body>
    <h1>Web interface for lights and sensors</h1>
    <p>Use the buttons below to control LED1</p>
    <button onclick="sendCommand('A')">Turn LED1 OFF</button>
    <button onclick="sendCommand('B')">Turn LED1 ON</button>
    
    <p>Use sliders to set RGB values for LED2</p>
    <div>
        R <input type="range" min="0" max="255" id="redSlider" class="slider" oninput="updateValue('redValue', this.value)">
        <span id="redValue">0</span>
    </div>
    <div>
        G <input type="range" min="0" max="255" id="greenSlider" class="slider" oninput="updateValue('greenValue', this.value)">
        <span id="greenValue">0</span>
    </div>
    <div>
        B <input type="range" min="0" max="255" id="blueSlider" class="slider" oninput="updateValue('blueValue', this.value)">
        <span id="blueValue">0</span>
    </div>
    <button onclick="sendRGB()">Set RGB</button>

    <script>
        function updateValue(id, value) {
            document.getElementById(id).textContent = value;
        }

        function sendCommand(command) {
            fetch(`/${command}`).then(response => {
                if (response.ok) {
                    console.log(`Command ${command} sent successfully.`);
                } else {
                    console.error(`Error sending command ${command}.`);
                }
            });
        }

        function sendRGB() {
            const red = document.getElementById("redSlider").value;
            const green = document.getElementById("greenSlider").value;
            const blue = document.getElementById("blueSlider").value;

            fetch(`/rgb?red=${red}&green=${green}&blue=${blue}`).then(response => {
                if (response.ok) {
                    console.log(`RGB values sent: R=${red}, G=${green}, B=${blue}`);
                } else {
                    console.error("Error sending RGB values.");
                }
            });
        }
    </script>
</body>
</html>

)rawliteral";

void setup() {
    Serial.begin(115200);

    // Initialize pins
    pinMode(LED1, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    // Connect to Wi-Fi
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();  // Start the server
}

void loop() {
    WiFiClient client = server.available();  // Check if a client is connected

    if (client) {
        Serial.println("New Client.");
        String request = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                request += c;

                // Stop reading when the request ends
                if (request.endsWith("\r\n\r\n")) break;
            }
        }

        // Process the request
        if (request.indexOf("GET /A") >= 0) {
            digitalWrite(LED1, LOW);  // Turn LED1 OFF
            Serial.println("LED1 OFF");
        } else if (request.indexOf("GET /B") >= 0) {
            digitalWrite(LED1, HIGH);  // Turn LED1 ON
            Serial.println("LED1 ON");
        } else if (request.indexOf("GET /rgb") >= 0) {
            int red = getValue(request, "red");
            int green = getValue(request, "green");
            int blue = getValue(request, "blue");

            analogWrite(RED_PIN, red);
            analogWrite(GREEN_PIN, green);
            analogWrite(BLUE_PIN, blue);

            Serial.printf("Set RGB to R:%d G:%d B:%d\n", red, green, blue);
        }

        // Serve the webpage
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.println(webpage);
        client.println();

        client.stop();  // Close the connection
        Serial.println("Client disconnected.");
    }
}

// Function to parse a parameter value from the request
int getValue(String request, String key) {
    int startIndex = request.indexOf(key + "=") + key.length() + 1;
    int endIndex = request.indexOf("&", startIndex);
    if (endIndex == -1) endIndex = request.indexOf(" ", startIndex);
    return request.substring(startIndex, endIndex).toInt();
}
 
