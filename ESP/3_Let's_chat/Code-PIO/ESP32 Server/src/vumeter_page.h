
const char vumeter_page_html[] PROGMEM = R"rawSrting(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 VU Meter</title>
  <style>
    html {font-family: Arial; display: inline-block}
    h2 {font-size: 2.3rem; text-align: center}
    p {font-size: 1.9rem;}
    table {width: 100%%}
    button {width: 49%%; height: 50px; font-size: 0.9rem;}
    body {max-width: 500px; margin:0px auto; padding: 0px 7px;}
    .slider { -webkit-appearance: none; margin: 14px 0; width: 100%%; height: 15px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 25px; height: 25px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 25px; height: 25px; background: #003249; cursor: pointer; }
    .labelCol {width: 80px}
    .valCol {width: 40px; padding-left: 14px}
    .back-button {
      width: 100%%;
      height: 50px;
      font-size: 0.9rem;
      background: #e53e3e;
      color: white;
      border: none;
      cursor: pointer;
      margin-top: 20px;
      border-radius: 4px;
    }
    .back-button:hover {
      background: #c53030;
    }
  </style>
</head>
<body>
  <h2>ESP32 VU Meter</h2>
  <button type="button" id="nextBtn" onclick="sendData('n',1)">Next pattern</button>
  <button type="button" id="autoBtn" onclick="sendData('a',1)">Auto Change Pattern</button>
  </br></br>
  <label id="labelAutoChangeTime" for="displayTime">Seconds to show each pattern on auto </label>
  <input id="displayTime" type="number" min="1" max="65535" onchange="sendData('t',this.value)" value="%DISPLAYTIME%">
  </br></br>
  <table border="0">
  <tr>
    <td class="labelCol"><label id="labelBrightness" for="brightnessSlider">Brightness</label></td>
    <td><input type="range" id="brightnessSlider" onchange="sendData('b',this.value)" min="0" max="255" value="%BRIGHTNESSVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="brightnessValue">%BRIGHTNESSVALUE%</span></td>
  </tr><tr>
    <td class="sliderCol"><label id="labelGain" for="gainSlider">Gain</label></td>
    <td><input type="range" id="gainSlider" onchange="sendData('g',this.value)" min="0" max="30" value="%GAINVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="gainValue">%GAINVALUE%</span></td>
  </tr><tr>
    <td class="labelRow"><label id="labelSquelch" for="squelchSlider">Squelch</label></td>
    <td><input type="range" id="squelchSlider" onchange="sendData('s',this.value)" min="0" max="30" value="%SQUELCHVALUE%" step="1" class="slider"></td>
    <td class="valCol"><span id="squelchValue">%SQUELCHVALUE%</span></td>
  </tr>
  </table>

  <form action="/setmode" method="POST">
    <input type="hidden" name="mode" value="stop">
    <button type="submit" class="back-button">⬛ STOP & Return to Menu</button>
  </form>

<script>
  var updateInterval;
  
  window.addEventListener('load', onLoad);
  
  function onLoad(event) {
    console.log('Page loaded, starting polling');
    // Poll for updates every 1 second
    updateInterval = setInterval(pollServerState, 1000);
  }
  
  // Poll server for current state
  function pollServerState() {
    fetch('/vuupdate', {
      method: 'POST',
      headers: {'Content-Type': 'application/x-www-form-urlencoded'},
      body: 'cmd=poll'
    })
    .then(response => response.json())
    .then(data => {
      // Update UI with server values
      document.getElementById('displayTime').value = data.displayTime;
      document.getElementById('brightnessValue').innerHTML = data.brightness;
      document.getElementById('brightnessSlider').value = data.brightness;
      document.getElementById('gainValue').innerHTML = data.gain;
      document.getElementById('gainSlider').value = data.gain;
      document.getElementById('squelchValue').innerHTML = data.squelch;
      document.getElementById('squelchSlider').value = data.squelch;
      
      // Update auto button appearance
      if (data.auto) {
        document.getElementById('autoBtn').style.backgroundColor = '#baffb3';
      } else {
        document.getElementById('autoBtn').style.backgroundColor = '';
      }
    })
    .catch(error => console.error('Polling error:', error));
  }

  function sendData(type, val) {
    var cmd = type + val;
    console.log('Sending: ' + cmd);
    
    fetch('/vuupdate', {
      method: 'POST',
      headers: {'Content-Type': 'application/x-www-form-urlencoded'},
      body: 'cmd=' + encodeURIComponent(cmd)
    })
    .then(response => response.json())
    .then(data => {
      // Update UI immediately with response
      document.getElementById('displayTime').value = data.displayTime;
      document.getElementById('brightnessValue').innerHTML = data.brightness;
      document.getElementById('brightnessSlider').value = data.brightness;
      document.getElementById('gainValue').innerHTML = data.gain;
      document.getElementById('gainSlider').value = data.gain;
      document.getElementById('squelchValue').innerHTML = data.squelch;
      document.getElementById('squelchSlider').value = data.squelch;
      
      if (data.auto) {
        document.getElementById('autoBtn').style.backgroundColor = '#baffb3';
      } else {
        document.getElementById('autoBtn').style.backgroundColor = '';
      }
    })
    .catch(error => console.error('Send error:', error));
  }
  
  // Clean up when leaving page
  window.addEventListener('beforeunload', function() {
    if (updateInterval) clearInterval(updateInterval);
  });
</script>
</body>
</html>
)rawSrting";