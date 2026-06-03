#ifndef HTML_H
#define HTML_H



#include <WebServer.h>

// These are defined in your .ino file:
extern WebServer server;
extern String resultLog;

// Entire page stored in flash so we don't blow RAM.
// We copy it to a String at request time to perform %RESULTS% replacement.
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<meta charset='utf-8'>
<title>ESP32 Stopwatch</title>
<link rel="stylesheet" href="/app.css">

<script src="/app.js" defer></script>


</head>
<body>
<div class="wrap">
  <div class="card">
    <div class="header">
      <h2 class="title">ESP32 Stopwatch</h2>
      <div class="stat">
        <div id="statusDot"></div>
        <div class="statusText" id="statusText">Checking...</div>
      </div>
      <div id="timeDisplay">0:00:00</div>
    </div>

    <div class="grid">
      <div class="field">
        <label for="name">Swimmer Name</label>
        <input type="text" id="name" placeholder="Enter name" autocomplete="off">
      </div>

      <div class="field">
        <label for="pool">Pool</label>
        <input type="text" id="pool" placeholder="Enter pool (e.g. Aquatic Center)" autocomplete="off">
      </div>

      <div class="field">
        <label>Stroke</label>
        <div class="chips" id="strokeChips">
          <div class="chip" data-val="Freestyle">Freestyle</div>
          <div class="chip" data-val="Back">Back</div>
          <div class="chip" data-val="Fly">Fly</div>
          <div class="chip" data-val="Frog">Frog</div>
          <div class="chip" data-val="Kick Board">Kick Board</div>
        </div>
      </div>

      <div class="field">
        <label>Distance</label>
        <div class="chips" id="distanceChips">
          <div class="chip" data-val="25m">25m</div>
          <div class="chip" data-val="50m">50m</div>
          <div class="chip" data-val="100m">100m</div>
          <div class="chip" data-val="200m">200m</div>
        </div>
      </div>
    </div>

    <div class="controls">
      <button class="start" id="btnStart" onclick="safeAction('/start', this)">Start</button>
      <button class="stop" id="btnStop" onclick="safeAction('/stop', this)">Stop</button>
      <button class="ghost" onclick="safeAction('/reset', this)">Reset</button>
      <button class="ghost" onclick="submitTime()">Finished</button>
    </div>

    <div class="toolbar">
      <a class="btn" href="/download" id="dlBtn">Download CSV</a>
      <button class="btn" onclick="copyResults()">Copy</button>
      <button class="btn" onclick="clearAll()">Clear All</button>
      <div class="btn" id="themeToggle">Theme</div>
    </div>

    <div class="results">
      <div id="log">
        <h3>Recorded Times:</h3>
        <div id="results">%RESULTS%</div>
      </div>
    </div>
  </div>
</div>


      for (var j2=0;j2<stops2.length;j2++) stops2[j2].style.background = '#e11d48';
      var ghosts2 = document.getElementsByClassName('ghost');
      for (var k2=0;k2<ghosts2.length;k2++) ghosts2[k2].style.background = '#2a2f3d';

      var t2 = document.getElementById('timeDisplay');
      if (t2) { t2.style.background = '#0f1220'; t2.style.color = '#ffffff'; }

      // Clear inline overrides so base dark CSS applies
      var els2 
</body>
</html>
)rawliteral";

// Inline so you can include from multiple translation units without linker clashes
inline void handleRoot() {
  String html = FPSTR(INDEX_HTML);        // copy from flash to RAM
  html.replace("%RESULTS%", resultLog);   // inject current results
  server.send(200, "text/html", html);    // serve it
}

#endif // HTML_H
