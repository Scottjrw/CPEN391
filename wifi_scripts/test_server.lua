-- Network Variables
ssid = "testwifi"
pass = "12345678"

-- Configure Wireless Internet
wifi.setmode(wifi.STATION)
print('set mode=STATION (mode='..wifi.getmode()..')\n')
print('MAC Address: ',wifi.sta.getmac())
print('Chip ID: ',node.chipid())
print('Heap Size: ',node.heap(),'\n')

-- Configure WiFi
wifi.sta.config(ssid,pass)
wifi.sta.autoconnect(1)
tmr.delay(1000000) -- wait 1,000,000 us = 1 second

-- led
led1 = 3
led2 = 4
gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)

-- Connect
tmr.alarm(0, 1000, 1, function()
   if wifi.sta.getip() == nil then
      print("Connecting to AP...\n")
   else
      ip, nm, gw=wifi.sta.getip()
      print("IP Info: \nIP Address: ",ip)
      print("Netmask: ",nm)
      print("Gateway Addr: ",gw,'\n')
      tmr.stop(0)
   end
end)

 -- Start a simple http server
 srv=net.createServer(net.TCP)
 srv:listen(80,function(conn)
     conn:on("receive", function(client,request)
         local buf = "";
         local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
         if(method == nil)then
             _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
         end
         local _GET = {}
         if (vars ~= nil)then
             for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
                 _GET[k] = v
             end
         end
         buf = buf.."<h1> ESP8266 Web Server</h1>";
         buf = buf.."<p>GPIO0 <a href=\"?pin=on\"><button>ON</button></a>&nbsp;<a href=\"?pin=off\"><button>OFF</button></a></p>";
         -- buf = buf.."<p>GPIO2 <a href=\"?pin=ON2\"><button>ON</button></a>&nbsp;<a href=\"?pin=OFF2\"><button>OFF</button></a></p>";
         local _on,_off = "",""
         if(_GET.pin == "on")then
               gpio.write(led1, gpio.HIGH);
         elseif(_GET.pin == "off")then
               gpio.write(led1, gpio.LOW);
         -- elseif(_GET.pin == "ON2")then
         --      gpio.write(led2, gpio.HIGH);
         -- elseif(_GET.pin == "OFF2")then
         --      gpio.write(led2, gpio.LOW);
         end
         client:send(buf);
         client:close();
         collectgarbage();
     end)
 end)
