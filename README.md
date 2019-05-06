# WiFi visualizer with sound using ESP8266

![](https://componto2017.edublogs.org/files/2017/11/2-ya2l4d-29hgdrm-1024x710.png)![](https://componto2017.edublogs.org/files/2017/11/1-1hj1swf-w3jb3y.png)

I've always been bothered by the hidden electromagnetic wave among us. Are they beautiful, ugly or even harmful? I’ve decided to make an visualizer with the amazing and useful ESP8266. It it compatible with the Arduino IDE and there are lots of things you could do about it. It is very important for realizing internet of things. This time I used Wemos D1 mini pro which include an antenna extension connector for enhancing the signals.

![](https://componto2017.edublogs.org/files/2017/11/1-1y9hxf6-2h5051d-300x167.jpg)


# Why ESP8266?

It is a small and a low cost chip while it is programmable and has great abilities. I used it to do lots of things, including a simple web-app controllable smart home system(just the light). And you can also program it to send wireless packet. Therefore, you could flood fake access points or send authentication packets. And this time I just go for a more creative approach. Since ESP8266 can acts as both access point mode and station mode, I can use it to connect my home network can creating a simple web server providing data from the board constantly(this case is the scanned wi-fi list), and as station mode it can scan the wifi signals nearby. I would use the data of the ssid, bssid(mac address), rssi(signal strength) and the channel.

![](https://componto2017.edublogs.org/files/2017/11/2-1knrokg-1vjxo13-300x194.jpg)

For the visualizing steps, I have few different approaches. Option 1 is that I can connect a speaker to the board, but arduino can only generate simple tone. And Option two is to generate the sound from my laptop, I would just need to use the data from my arduino. At the end, I’ve written a simple webpage with javascript and some midi libraries to generate sound. I could’ve uploaded them(whole server file) to my arduino but my board only has around 4 mb of rom so I just decided to keep them on my server. But I only leave a little interface on my esp8266 server for showing the scanned wi-fi in realtime.

![](https://componto2017.edublogs.org/files/2017/11/3-2dgd9i1-1orx5a1-74x300.jpg)

## Videos

[Video Link](https://youtu.be/Z_CTco0IkAk)

## Brief Steps

1.  First I do some researches on how to setup a webserver on my ESP8266. I looked at the examples and figure it out.![](https://componto2017.edublogs.org/files/2017/11/4-1iwe1xq-1buv2ss-1024x643.jpg)
2.  Then I just need to know how to scan the wi-fi signals.![](https://componto2017.edublogs.org/files/2017/11/5-1d1di2r-2ftu0b1-1024x677.jpg)
3.  Store those signals and figure a way to let my laptop server use it.
4.  I was thinking about with json but I just go with xml because it simpler.![](https://componto2017.edublogs.org/files/2017/11/6-1qmjth1-20lybk1-768x1024.jpg)![](https://componto2017.edublogs.org/files/2017/11/7-11bc7cg-1okgz1v-1024x460.jpg)
5.  Figure out how to receive the data on the webpage at a certain interval. I used ajax method to load the xml from the esp8266 server.
6.  All the programming part with javascript using p5.js and a midi library.

## Code Explanation

There are just two major files need to be explained. First one is the arduino ide file and the other one is the html file for playing the sound and visualizing.

Little Background:

My home network is called o2-WLAN47

ESP8266 server: 192.168.1.4

Laptop server: 192.168.1.12:8000

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
It’s a really simple code. The steps are, firstly in setup, set the mode to AP + Station. Then server listen on different handling events, for root “/”, execute stuff in handleweb function, while /xml execute in handXML function. And web, Javascript, XML are the content translated to arduino ide readable format. With \n means new line.

The original code of buildweb part would be like this

    <!DOCTYPE HTML>
    <BODY onload='load()'>
    <table id='wifit'></table>
    </BODY>
    </HTML>
Really simple, just a table with id wifit so that it’s identified. There should also be javascript before <body> tag.

    <script>
    var xmlHttp=createXmlHttpObject();
    
    function createXmlHttpObject(){
     if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
     }else{
        xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
     }
     return xmlHttp;
    }
    
    function load(){
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
       xmlHttp.open('GET','xml',true);
       xmlHttp.onreadystatechange=handleServerResponse;
       xmlHttp.send(null);
     }
     setTimeout('load()',1000);
    }
      
    function handleServerResponse(){
     if(xmlHttp.readyState==4 && xmlHttp.status==200){
       xmldoc = xmlHttp.responseXML;
       var table='<tr><th>SSID</th><th>BSSID</th><th>Channel</th><th>Signal</th></tr>';
       var x = xmldoc.getElementsByTagName('wifi');
        for (i = 0; i <x.length; i++) { 
        table += '<tr><td>' +
        x[i].getElementsByTagName('ssid')[0].childNodes[0].nodeValue +
        '</td><td>' +
        x[i].getElementsByTagName('bssid')[0].childNodes[0].nodeValue +
        '</td><td>' +
        x[i].getElementsByTagName('channel')[0].childNodes[0].nodeValue +
        '</td><td>' +
        x[i].getElementsByTagName('signal')[0].childNodes[0].nodeValue +
        '</td></tr>';
      }
       document.getElementById('wifit').innerHTML = table;
     }
    }
    </script>
I’m using the Get method and the settimeout is for constantly updating the page without having to refresh it. For reading the xml files I just use getElementsByTagName mostly. At the end apply the var table into the element with id wifit.

The following is the laptop server visualizer/making-sound code

    <!DOCTYPE html>
    <html xmlns="http://www.w3.org/1999/xhtml">
    
    <head>
    	<meta http-equiv="content-type" content="text/html; charset=utf-8" />
    	<script src="https://cdnjs.cloudflare.com/ajax/libs/p5.js/0.4.23/p5.min.js" type="text/javascript"></script>
    	<script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.0.0-beta1/jquery.min.js" type="text/javascript"></script>
    	<script src="js/Base64.js" type="text/javascript"></script>
    	<script src="js/Base64binary.js" type="text/javascript"></script>
    	<script src="js/WebAudioAPI.js" type="text/javascript"></script>
    	<script src="js/MIDI.js" type="text/javascript"></script>
    	<script src="js/dom_request_xhr.js" type="text/javascript"></script>
    	<script src="js/dom_request_script.js" type="text/javascript"></script>
    </head>
    
    <body style="display:none">
    	<script type="text/javascript">
    		var loaded = false; //for making sure function draw doesn't run until the file is loaded
    		var xmlHttp=createXmlHttpObject();
    
    		var mac = []; //array for assigning notes and stuff
    		var channel = [];
    		var signal = [];
    		var ssid = [];
    		var frame;
    		//intrument array for midi.js
    		var instrumentlist = ["acoustic_grand_piano", "xylophone", "vibraphone", "agogo", "acoustic_bass"];
    
    		//wave illustration stuff
    		var x = []; //every circle has its variable for lerp (size)
    		var tx = []; //calculate the width of the text displaying the ssid then used to draw circle
    		var c = []; //every circle has its variable for lerp (opacity)
    		var play = []; //condition for whether the circle is enlarging
    
    		window.onload = function() {
    			MIDI.loadPlugin({
    				soundfontUrl: "./soundfont/",
    				instruments: instrumentlist,
    				onsuccess: function() {
    					document.body.style.display = 'block'; //make sure it shows the web only after it is done loading
    					readlist(); //for reading list
    					frame = 0;
    					loaded = true;
    					alert('ok!'); //show ok
    				}
    			});
    		};
    
    		function createXmlHttpObject(){
    			if(window.XMLHttpRequest){
    			xmlHttp=new XMLHttpRequest();
    			}else{
    			xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
    			}
    			return xmlHttp;
    		}
    
    		function textmode(size, inp, convert, x, y, a) //function for calculating/styling text
    		{
    			noStroke();
    			textSize(size);
    			if (convert == true) {
    				return textWidth(inp);
    			} else {
    				fill(a);
    				textAlign(CENTER, CENTER);
    				text(inp, x, y);
    			}
    		}
    
    		function refresh() {
    			loaded = false; //make sure to stop funntion draw running
    			readlist(); //call readlist function
    			loaded = true; //indicator for function draw to run
    		}
    
    		function readlist() {
    			channel.length = 0;
    			signal.length = 0;
    			ssid.length = 0;
    			mac.length = 0;
    			x.length = 0;
    			c.length = 0;
    			play.length = 0;
    			tx.length = 0;
    			//reset all the data before getting the file, also make sure it stops playing old data
    
    			if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
    				xmlHttp.open('GET','http://192.168.1.4/xml',true);
    				xmlHttp.onreadystatechange=handleServerResponse;
    				xmlHttp.send(null);
    			}
    			//same method used in my arduino sketch
    		}
    
    		function handleServerResponse(){
    			if(xmlHttp.readyState==4 && xmlHttp.status==200){
    				xmldoc = xmlHttp.responseXML;
    				var wlist = xmldoc.getElementsByTagName('wifi');
    				for (var i=0;i < wlist.length;i++) {
    					ssid[i] = wlist[i].getElementsByTagName('ssid')[0].childNodes[0].nodeValue;
    					mac[i] = wlist[i].getElementsByTagName('bssid')[0].childNodes[0].nodeValue;
    					channel[i] = wlist[i].getElementsByTagName('channel')[0].childNodes[0].nodeValue;
    					signal[i] = wlist[i].getElementsByTagName('signal')[0].childNodes[0].nodeValue;
    				}
    				for (i = 0; i < wlist.length; i++) { //total no. of scanned wifi
    					mac[i] = split(mac[i], ":"); //split is to split the array into smaller nested array, so that FF:FF is into mac[1][1] and mac[1][2]
    					for (j = 0; j < mac[i].length; j++) {
    						mac[i][j] = parseInt(mac[i][j], 16); //**convert all hex values to decimal for easier data manipulation
    					}
    				}
    				for (i = 0; i < mac.length; i++) {
    					x[i] = [];
    					c[i] = [];
    					play[i] = [];
    					tx[i] = textmode(30, ssid[i], true, 0, 0, 0) + 20;
    					for (j = 0; j < mac[i].length; j++) {
    						x[i][j] = tx[i];
    						c[i][j] = 255;
    						play[i][j] = false;
    					}
    				}
    			}
    		}
    
    		function setup() {
    			noiseSeed(2017); //make sure everytime it runs the same
    			createCanvas(1280, 720);
    			background(0);
    			frameRate(60);
    		}
    
    		function nmac(x, y, m) { //function for converting and fitting using noise
    			return map(noise(x, y), 0, 1, 0, m);
    		}
    
    		function draw() {
    			if (loaded == true) {
    				background(0, 20);
    				text("Frame:", 20, 40)
    				text(frame, 120, 40);
    				for (i = 0; i < mac.length; i++) {
    					for (j = 0; j < mac[i].length; j++) {
    						if (frame % (140 * 10) == int(map(mac[i][j], 0, 255, 0, 128)) * 10 || frame == int(map(mac[i][j], 0, 255, 0, 128)) * 10) {
    							play[i][j] = false;
    							x[i][j] = tx[i]; //reset the value!
    							play[i][j] = true;
    						}
    						ring(nmac(mac[i][j], i, width), nmac(mac[i][j], j, height), 300, mac[i][j], i, j, mac[i][j]);
    					}
    				}
    
    				if (frame % 1350 == 0) {
    					refresh(); //***each 1350 frame update the wifi.csv!
    				}
    				frame++;
    			}
    		}
    
    		function ring(dx, dy, size, int, n1, n2, col) {
    			if (play[n1][n2] == true) { //if it is trigger, it can start playing(enlarging the circle)
    				if (x[n1][n2] == tx[n1]) { //if it just started(the circle is its initial size), play the sound
    					playins(channel[n1], mac[n1][n2], signal[n1]);
    				}
    				//if the circle the within the range of that(*0.8), the text appear
    				if (x[n1][n2] <= tx[n1] * (map(signal[n1], 20, 400, 1.5, 5) * 0.8)) {
    					textmode(20, ssid[n1], false, dx, dy, map(signal[n1], 20, 100, 100, 255));
    				}
    				//lerp to let it change every frame.
    				x[n1][n2] = lerp(x[n1][n2], tx[n1] * map(signal[n1], 20, 400, 1.5, 5), 0.03);
    				c[n1][n2] = lerp(c[n1][n2], 0, 0.05);
    				//stroke use c as alpha channel.
    				stroke(nmac(col, 0, 255), nmac(col, 1, 255), nmac(col, 2, 255), c[n1][n2]);
    				strokeWeight(10);
    				noFill();
    				ellipse(dx, dy, x[n1][n2], x[n1][n2]);
    
    				// if it reach the max size, since it cannot reach its max value according to lerp, i have to minus 10
    				if (x[n1][n2] >= tx[n1] * map(signal[n1], 20, 400, 1.5, 5) - 10) {
    					x[n1][n2] = tx[n1]; //reset size
    					c[n1][n2] = 255; //back to 255
    					play[n1][n2] = false; //stop playing the circle(stop enlarging)
    				}
    			}
    		}
    
    		function playins(instrument, mvalue, strength) {
    			var noterange = []; //note range(usually 21-108, but i adjust it)
    			switch (instrument) {
    				case "1":
    				case "3":
    				case "5":
    					instrument = 1; //xylophone
    					noterange = [21, 108];
    					break;
    				case "7":
    				case "9":
    				case "11":
    					instrument = 2; //vibraphone
    					noterange = [21, 108];
    					break;
    				case "2":
    				case "4":
    				case "6":
    					instrument = 3; //agogo
    					noterange = [21, 108];
    					break;
    				case "8":
    				case "10":
    				case "12":
    					instrument = 4; //acoustic_bass
    					noterange = [30, 50];
    					//strength = strength/2;
    					break;
    				default: //other channel
    					instrument = 0; //piano
    					dcol = color(255, 0, 255);
    					noterange = [21, 108];
    					break;
    			}
    
    			mvalue = int(map(nmac(mvalue, 0, 255), 0, 255, noterange[0], noterange[1])); //mvalue is treated as note, then converted to fit the note range
    			MIDI.channels[instrument].instrument = MIDI.GM.byName[instrumentlist[instrument]].number; //give each channel a different instrument
    			MIDI.noteOn(instrument, mvalue, map(strength, 20, 80, 50, 127), 0); //play note
    			MIDI.noteOff(instrument, mvalue, 0.5); //it's duration
    		}
    	</script>
    </body>
    
    </html>
Comments are included next to the code. I used p5.js and a midi library called soundfont.

In order for this to work, the laptop server files must be run on a server since there’s some limitation with the p5.js that it couldn’t load local files(file:// path).

## Difficulties

Just two major problems I encountered, first thing is about the memory allocation problem. I was thinking about scannign the signals and store them into an array. However I just realize Arduino doesn’t have dynamic memory which means arrays can’t be increased in size and that is a problem. Since I don’t know how many wi-fi signals I would scan. I just give up on using array and just use the result from the library which is WiFi.scanNetworks();. It would return all the information I need. By checking it’s length I could get solve the problem.

Second problem is how to let my local server reads data from esp8266 server constantly. The reading problem is the most difficult for me, because I was using JQuery ajax to read the file, but I couldn’t get xml file with jquery since they are not on the same server. Then I just use another ajax method and it works.

## Improvements

Maybe my sense of music? I just couldn’t really think of how I can make it sound nicer 😛 ?
