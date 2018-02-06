-- This information is used by the Wi-Fi dongle to make a wireless connection to the router in the Lab
-- or if you are using another router e.g. at home, change ID and Password appropriately
SSID = "testwifi"
SSID_PASSWORD = "12345678"

-- configure ESP as a station
wifi.setmode(wifi.STATION)
wifi.sta.config(SSID,SSID_PASSWORD)
wifi.sta.autoconnect(1)

-- pause for connection to take place - adjust time delay if necessary or repeat until connection made
tmr.delay(1000000) -- wait 1,000,000 us = 1 second

-- modify the address after boot up
URL = "http://192.168.43.177"

function led_on()

  -- remember to check the address
  http.get(URL.."/?pin=on", nil, function(code, data)
    --if (code < 0) then
    --  print("HTTP request failed")
    --else
    --  print(code, data)
    --end
  end)

end

function led_off()

  -- remember to check the address
  http.get(URL.."/?pin=off", nil, function(code, data)
    --if (code < 0) then
    --  print("HTTP request failed")
    --else
    --  print(code, data)
    --end
  end)

end
