/*
  ESP32 HTML WebServer Page Code
 http:://www.electronicwings.com
*/

const char html_page[] PROGMEM = R"rawSrting(
<!DOCTYPE html>
<html>
  <style>
    body {font-family: sans-serif;}
    h1 {text-align: center; font-size: 30px;}
    p {text-align: center; color: #4CAF50; font-size: 40px;}
  </style>
 
<body>
  <h1>DS18B20 Temperature Monitoring With ESP32</h1><br>
  <p>Temperature: <span id="TempValue">0</span>&degC</p><br>

  <script>
    setInterval(function() {
      var xhttp = new XMLHttpRequest();
     xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
         document.getElementById("TempValue").innerHTML = this.responseText;
        }
      };
     xhttp.open("GET", "readTemp", true);
      xhttp.send();
    },50);
  </script>
</body>
</html>
)rawSrting";
    