#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

const char* ssid = "o2-WLAN47";
const char* password = "";

String web,javaScript,XML;

void buildweb(){
  buildJavascript();
  web="<!DOCTYPE HTML>\n";
  web+=javaScript;
  web+="<BODY onload='load()'>\n";
  web+="<table id='wifit'></table>\n";
  web+="</BODY>\n";
  web+="</HTML>\n";
}

void buildJavascript(){
  javaScript="<SCRIPT>\n";
  javaScript+="var xmlHttp=createXmlHttpObject();\n";

  javaScript+="function createXmlHttpObject(){\n";
  javaScript+=" if(window.XMLHttpRequest){\n";
  javaScript+="    xmlHttp=new XMLHttpRequest();\n";
  javaScript+=" }else{\n";
  javaScript+="    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript+=" }\n";
  javaScript+=" return xmlHttp;\n";
  javaScript+="}\n";

  javaScript+="function load(){\n";
  javaScript+=" if(xmlHttp.readyState==0 || xmlHttp.readyState==4){\n";
  javaScript+="   xmlHttp.open('GET','xml',true);\n";
  javaScript+="   xmlHttp.onreadystatechange=handleServerResponse;\n";
  javaScript+="   xmlHttp.send(null);\n";
  javaScript+=" }\n";
  javaScript+=" setTimeout('load()',1000);\n";
  javaScript+="}\n";
  
  javaScript+="function handleServerResponse(){\n";
  javaScript+=" if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript+="   xmldoc = xmlHttp.responseXML;\n";
  javaScript+="   var table='<tr><th>SSID</th><th>BSSID</th><th>Channel</th><th>Signal</th></tr>';\n";
  javaScript+="   var x = xmldoc.getElementsByTagName('wifi');\n";
  javaScript+="    for (i = 0; i <x.length; i++) { \n";
  javaScript+="    table += '<tr><td>' +\n";
  javaScript+="    x[i].getElementsByTagName('ssid')[0].childNodes[0].nodeValue +\n";
  javaScript+="    '</td><td>' +\n";
  javaScript+="    x[i].getElementsByTagName('bssid')[0].childNodes[0].nodeValue +\n";
  javaScript+="    '</td><td>' +\n";
  javaScript+="    x[i].getElementsByTagName('channel')[0].childNodes[0].nodeValue +\n";
  javaScript+="    '</td><td>' +\n";
  javaScript+="    x[i].getElementsByTagName('signal')[0].childNodes[0].nodeValue +\n";
  javaScript+="    '</td></tr>';\n";
  javaScript+="  }\n";
  javaScript+="   document.getElementById('wifit').innerHTML = table;\n";
  javaScript+=" }\n";
  javaScript+="}\n";
  javaScript+="</SCRIPT>\n";
}

void buildXML(){
  int n = WiFi.scanNetworks();
  XML="<?xml version='1.0'?>";
  XML+="<wifilist>";
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    for (int i = 0; i < n; ++i)
    {
      XML+="<wifi>";
      XML+="<ssid>";
      XML+=WiFi.SSID(i);
      XML+="</ssid>";
      XML+="<bssid>";
      XML+=WiFi.BSSIDstr(i);
      XML+="</bssid>";
      XML+="<channel>";
      XML+=String(WiFi.channel(i));
      XML+="</channel>";
      XML+="<signal>";
      XML+=String(abs(WiFi.RSSI(i)));
      XML+="</signal>";
      XML+="</wifi>";
      delay(10);
    }
  }
  XML+="</wifilist>";
}

void handleweb(){
  buildweb();
  server.send(200,"text/html",web);
}

void handleXML(){
  buildXML();
  server.send(200,"text/xml",XML);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA); //AP + Station
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password); //connect to my home network

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/",handleweb);
  server.on("/xml",handleXML);
  server.begin();  
}

void loop() {
  server.handleClient();
}
