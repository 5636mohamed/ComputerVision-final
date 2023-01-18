/*
* This code is designed to build a dashboard that lively share readings and graphs for climatic data
* The algorithm is designed by: Mohamed Elharery
* The code is authorized and written by: Mohamed Elharery
* Last edition on the code: 13/1/2023
* The code package version: 1.9 (unminified version)
*/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <FS.h>
#include <DHT.h>
// Replace with your network credentials
const char* ssid = "RA3";
const char* password = "1234567890";
// Define variables for the whole parameters that are measured by the station
float newT;
float newH;
#define DHTPIN 4     // Digital pin connected to the DHT sensor according to the datasheet. D2 => Pin 4

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 2 seconds
const long interval = 2000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP8266 DHT Server</h2>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <hr>
  <div id="chart-temperature" class="container"></div>
    <div id="chart-humidity" class="container"></div>
  <script src="https://code.highcharts.com/highcharts.js"></script>
<script>
  var chartT = new Highcharts.Chart({
  chart:{ renderTo : 'temp_graph' },
  title: { text: 'DHT11 Temperature' },
  series: [{
    showInLegend: false,
    data: []
  }],
  plotOptions: {
    line: { animation: false,
      dataLabels: { enabled: true }
    },
    series: { color: '#059e8a' }
  },
  xAxis: { type: 'datetime',
    dateTimeLabelFormats: { second: '%H:%M:%S' }
  },
  yAxis: {
    title: { text: 'Temperature (Celsius)' }
    //title: { text: 'Temperature (Fahrenheit)' }
  },
  credits: { enabled: false }
});
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var x = (new Date()).getTime(),
          y = parseFloat(this.responseText);
      //console.log(this.responseText);
      if(chartT.series[0].data.length > 40) {
        chartT.series[0].addPoint([x, y], true, true, true);
      } else {
        chartT.series[0].addPoint([x, y], true, false, true);
      }
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 30000 ) ;
</script>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 2000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 2000 ) ;
</script>
<script>
    var chartT = new Highcharts.Chart({
    chart:{ renderTo : 'chart-temperature' },
    title: { text: 'DHT11 Temperature' },
    series: [{
        showInLegend: false,
        data: []
    }],
    plotOptions: {
        line: { animation: false,
        dataLabels: { enabled: true }
        },
        series: { color: '#059e8a' }
    },
    xAxis: { type: 'datetime',
        dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
        title: { text: 'Temperature (Celsius)' }
        //title: { text: 'Temperature (Fahrenheit)' }
    },
    credits: { enabled: false }
    });
    setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        var x = (new Date()).getTime(),
            y = parseFloat(this.responseText);
        //console.log(this.responseText);
        if(chartT.series[0].data.length > 40) {
            chartT.series[0].addPoint([x, y], true, true, true);
        } else {
            chartT.series[0].addPoint([x, y], true, false, true);
        }
        }
    };
    xhttp.open("GET", "/temperature", true);
    xhttp.send();
    }, 2000 ) ;

    var chartH = new Highcharts.Chart({
    chart:{ renderTo:'chart-humidity' },
    title: { text: 'DHT11 Humidity' },
    series: [{
        showInLegend: false,
        data: []
    }],
    plotOptions: {
        line: { animation: false,
        dataLabels: { enabled: true }
        }
    },
    xAxis: {
        type: 'datetime',
        dateTimeLabelFormats: { second: '%H:%M:%S' }
    },
    yAxis: {
        title: { text: 'Humidity (%)' }
    },
    credits: { enabled: false }
    });
    setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        var x = (new Date()).getTime(),
            y = parseFloat(this.responseText);
        //console.log(this.responseText);
        if(chartH.series[0].data.length > 40) {
            chartH.series[0].addPoint([x, y], true, true, true);
        } else {
            chartH.series[0].addPoint([x, y], true, false, true);
        }
        }
    };
    xhttp.open("GET", "/humidity", true);
    xhttp.send();
    }, 2000 ) ;
    </script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  // Intiating the Temperature and Humidity Sensor
  dht.begin();
  
  // Connection to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Print ESP8266 Local IP Address
  Serial.print("The ESP8266 is connected to WiFi successfully. The IP address for the ESP is : ");
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
}


void loop(){
//  server.handleClient();
   
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { 
    // m = 17.3;
    //s=233;
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }
  }
}
