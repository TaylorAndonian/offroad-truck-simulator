/**
 * MKS-ESP32-FOC V1.0 — Dual Motor Open-Loop Velocity + WiFi Web Server
 *
 * Library needed (Arduino Library Manager):
 *   - SimpleFOC  (by SimpleFOC community)
 *
 * WiFi.h and WebServer.h are built into the ESP32 core — no extra install.
 *
 * Board: ESP32 Dev Module
 *        Tools > Partition Scheme > Huge APP (3MB No OTA)
 *
 * Pins verified from official MKS example code:
 *   Motor 1:  PWM = 32, 33, 25    EN = 22
 *   Motor 2:  PWM = 26, 27, 14    EN = 12
 *
 * Web UI:  http://<board-ip>/
 * Speed slider: rad/s  (negative = reverse)
 * Voltage limit slider: caps how hard the driver pushes the motor
 */

#include <SimpleFOC.h>
#include <WiFi.h>
#include <WebServer.h>

// ─── WiFi ────────────────────────────────────────────────────────────────────
// Copy secrets.example.h to secrets.h and fill in your network.
#include "secrets.h"   // defines WIFI_SSID and WIFI_PASSWORD (git-ignored)

// ─── Motors & drivers ────────────────────────────────────────────────────────
// Change the number in BLDCMotor() to match your motor's pole pairs
BLDCMotor      motor1  = BLDCMotor(7);
BLDCDriver3PWM driver1 = BLDCDriver3PWM(32, 33, 25, 22);

BLDCMotor      motor2  = BLDCMotor(7);
BLDCDriver3PWM driver2 = BLDCDriver3PWM(26, 27, 14, 12);

// ─── State ───────────────────────────────────────────────────────────────────
struct MotorState {
  bool  enabled      = false;
  float speed        = 0.0f;   // rad/s
  float voltageLimit = 3.0f;   // V — keep low until you know your motor is happy
};
MotorState m1, m2;

// ─── Web server ──────────────────────────────────────────────────────────────
WebServer server(80);

const char INDEX_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 FOC Motor Control</title>
<style>
  :root{--bg:#111;--card:#1e1e1e;--accent:#00d4aa;--text:#e0e0e0}
  *{box-sizing:border-box;margin:0;padding:0}
  body{background:var(--bg);color:var(--text);font-family:'Segoe UI',sans-serif;
       display:flex;flex-direction:column;align-items:center;padding:24px 16px}
  h1{color:var(--accent);margin-bottom:24px;font-size:1.5rem;letter-spacing:1px}
  .card{background:var(--card);border-radius:12px;padding:24px;width:100%;
        max-width:420px;margin-bottom:20px;box-shadow:0 4px 20px #0006}
  .card h2{color:var(--accent);margin-bottom:16px;font-size:1.1rem}
  label{display:flex;justify-content:space-between;margin-bottom:6px;font-size:.85rem}
  input[type=range]{width:100%;accent-color:var(--accent);margin-bottom:14px}
  .row{display:flex;gap:12px;margin-top:8px}
  button{flex:1;padding:10px;border:none;border-radius:8px;cursor:pointer;
         font-size:.9rem;font-weight:600;transition:.2s}
  .btn-on{background:var(--accent);color:#000}
  .btn-off{background:#444;color:#fff}
  .status{text-align:center;margin-top:10px;font-size:.8rem;opacity:.6}
  .pill{display:inline-block;padding:2px 10px;border-radius:20px;font-size:.75rem;
        font-weight:700;margin-left:8px}
  .on{background:#00d4aa22;color:var(--accent);border:1px solid var(--accent)}
  .off{background:#ffffff11;color:#888;border:1px solid #555}
</style>
</head>
<body>
<h1>&#9889; ESP32 FOC Dual Motor</h1>

<div class="card">
  <h2>Motor 1 <span class="pill off" id="m1pill">OFF</span></h2>
  <label>Speed <span id="m1sv">0.0 rad/s</span></label>
  <input type="range" id="m1s" min="-40" max="40" step="0.5" value="0"
         oninput="document.getElementById('m1sv').textContent=parseFloat(this.value).toFixed(1)+' rad/s'">
  <label>Voltage Limit <span id="m1lv">3.0 V</span></label>
  <input type="range" id="m1l" min="0.5" max="24" step="0.5" value="3"
         oninput="document.getElementById('m1lv').textContent=parseFloat(this.value).toFixed(1)+' V'">
  <div class="row">
    <button class="btn-on"  onclick="cmd(1,'on')">&#9654; ON</button>
    <button class="btn-off" onclick="cmd(1,'off')">&#9632; OFF</button>
  </div>
  <div class="status" id="m1st">Idle</div>
</div>

<div class="card">
  <h2>Motor 2 <span class="pill off" id="m2pill">OFF</span></h2>
  <label>Speed <span id="m2sv">0.0 rad/s</span></label>
  <input type="range" id="m2s" min="-40" max="40" step="0.5" value="0"
         oninput="document.getElementById('m2sv').textContent=parseFloat(this.value).toFixed(1)+' rad/s'">
  <label>Voltage Limit <span id="m2lv">3.0 V</span></label>
  <input type="range" id="m2l" min="0.5" max="24" step="0.5" value="3"
         oninput="document.getElementById('m2lv').textContent=parseFloat(this.value).toFixed(1)+' V'">
  <div class="row">
    <button class="btn-on"  onclick="cmd(2,'on')">&#9654; ON</button>
    <button class="btn-off" onclick="cmd(2,'off')">&#9632; OFF</button>
  </div>
  <div class="status" id="m2st">Idle</div>
</div>

<script>
function cmd(m,action){
  const s=parseFloat(document.getElementById('m'+m+'s').value).toFixed(2);
  const l=parseFloat(document.getElementById('m'+m+'l').value).toFixed(2);
  document.getElementById('m'+m+'st').textContent='Sending...';
  fetch('/motor?m='+m+'&action='+action+'&s='+s+'&l='+l)
    .then(r=>r.text()).then(t=>{
      document.getElementById('m'+m+'st').textContent=t;
      const on=action==='on';
      const pill=document.getElementById('m'+m+'pill');
      pill.textContent=on?'ON':'OFF';
      pill.className='pill '+(on?'on':'off');
    }).catch(()=>{document.getElementById('m'+m+'st').textContent='Error - check connection';});
}
setInterval(()=>{
  fetch('/status').then(r=>r.json()).then(d=>{
    for(const m of[1,2]){
      const on=d['m'+m+'_on'];
      document.getElementById('m'+m+'pill').textContent=on?'ON':'OFF';
      document.getElementById('m'+m+'pill').className='pill '+(on?'on':'off');
      document.getElementById('m'+m+'st').textContent=
        on?'Running @ '+d['m'+m+'_s']+' rad/s  |  limit '+d['m'+m+'_l']+' V':'Idle';
    }
  }).catch(()=>{});
},2000);
</script>
</body>
</html>
)rawhtml";

// ─── Handlers ────────────────────────────────────────────────────────────────
void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleMotor() {
  if (!server.hasArg("m") || !server.hasArg("action")) {
    server.send(400, "text/plain", "Bad request"); return;
  }
  int    mNum   = server.arg("m").toInt();
  String action = server.arg("action");
  float  spd    = server.hasArg("s") ? server.arg("s").toFloat() : 0.0f;
  float  lim    = server.hasArg("l") ? server.arg("l").toFloat() : 3.0f;

  MotorState* ms = (mNum == 1) ? &m1 : &m2;
  ms->voltageLimit = constrain(lim, 0.5f, 24.0f);
  ms->speed        = spd;

  if (action == "on") {
    ms->enabled = true;
    server.send(200, "text/plain",
      "Motor " + String(mNum) + " ON  " +
      String(ms->speed, 1) + " rad/s  |  limit " + String(ms->voltageLimit, 1) + " V");
  } else {
    ms->enabled = false;
    server.send(200, "text/plain", "Motor " + String(mNum) + " OFF");
  }
}

void handleStatus() {
  String j = "{";
  j += "\"m1_on\":"  + String(m1.enabled ? "true" : "false") + ",";
  j += "\"m1_s\":"   + String(m1.speed, 1) + ",";
  j += "\"m1_l\":"   + String(m1.voltageLimit, 1) + ",";
  j += "\"m2_on\":"  + String(m2.enabled ? "true" : "false") + ",";
  j += "\"m2_s\":"   + String(m2.speed, 1) + ",";
  j += "\"m2_l\":"   + String(m2.voltageLimit, 1);
  j += "}";
  server.send(200, "application/json", j);
}

// ─── Setup ───────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== MKS ESP32 FOC Dual Motor ===");

  // Motor 1 — set limits BEFORE init(), matching official example order
  driver1.voltage_power_supply = 24;  // change to your PSU voltage
  driver1.init();
  motor1.linkDriver(&driver1);
  motor1.voltage_limit  = m1.voltageLimit;
  motor1.velocity_limit = 40;         // rad/s hard cap
  motor1.controller     = MotionControlType::velocity_openloop;
  motor1.init();
  motor1.disable();                   // start with motor off

  // Motor 2
  driver2.voltage_power_supply = 24;
  driver2.init();
  motor2.linkDriver(&driver2);
  motor2.voltage_limit  = m2.voltageLimit;
  motor2.velocity_limit = 40;
  motor2.controller     = MotionControlType::velocity_openloop;
  motor2.init();
  motor2.disable();

  // WiFi
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.printf("\nReady! Open: http://%s/\n", WiFi.localIP().toString().c_str());

  server.on("/",       handleRoot);
  server.on("/motor",  handleMotor);
  server.on("/status", handleStatus);
  server.begin();
}

// ─── Loop ────────────────────────────────────────────────────────────────────
void loop() {
  server.handleClient();

  // Motor 1
  if (m1.enabled) {
    motor1.voltage_limit = m1.voltageLimit;
    motor1.enable();
    motor1.move(m1.speed);
  } else {
    motor1.disable();
  }

  // Motor 2
  if (m2.enabled) {
    motor2.voltage_limit = m2.voltageLimit;
    motor2.enable();
    motor2.move(m2.speed);
  } else {
    motor2.disable();
  }
}
