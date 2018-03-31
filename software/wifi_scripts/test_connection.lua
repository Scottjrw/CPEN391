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

tmr.alarm(0, 1000, 1, function()
   if wifi.sta.getip() == nil then
      --print("Connecting to AP...\n")
   else
      --print("connected\n")
      tmr.stop(0)
   end
end)

-- modify the address after boot up
address = "http://192.168.43.196"
header = 'Content-Type: application/json\r\n'
URL = 'https://use1-wap.tplinkcloud.com/?token=2d2c8422-A6AWd80Sco51JaW7L7yQ5B0'

raw_picture = ''

function led_on()

  -- remember to check the address
  http.get(address.."/?pin=on", nil, function(code, data)
    --if (code < 0) then
    --  print("HTTP request failed")
    --else
    --  print(code, data)
    --end
  end)

end

function led_off()

  -- remember to check the address
  http.get(address.."/?pin=off", nil, function(code, data)
    --if (code < 0) then
    --  print("HTTP request failed")
    --else
    --  print(code, data)
    --end
  end)

end

function light_on()

    ok, json1 = pcall(cjson.encode, {deviceId="800616E52D10AABA186645E5A5BFFB6617ECFA24", requestData="{\"system\":{\"set_relay_state\":{\"state\":1}}}"})
    if ok then
      print(json1)
    else
      print("failed to encode!")
    end
    ok, data_on = pcall(cjson.encode, {method="passthrough", params=cjson.decode(json1)})
    if ok then
      print(data_on)
    else
      print("failed to encode!")
    end

    http.request(URL,"POST",header,data_on,
    function(code, data)
    if (code < 0) then
      print("HTTP request failed")
    else
      print(code, data)
    end
  end)

end

function light_off()

  ok, json1 = pcall(cjson.encode, {deviceId="800616E52D10AABA186645E5A5BFFB6617ECFA24", requestData="{\"system\":{\"set_relay_state\":{\"state\":0}}}"})
    if ok then
      print(json1)
    else
      print("failed to encode!")
    end
    ok, data_off = pcall(cjson.encode, {method="passthrough", params=cjson.decode(json1)})
    if ok then
      print(data_off)
    else
      print("failed to encode!")
    end

  http.request(URL,"POST",header,data_off,
  function(code, data)
    if (code < 0) then
      print("HTTP request failed")
    else
      print(code, data)
    end
  end)

end

function send_username(username, password)
    url = "http://104.198.97.189:6000/loginByPassword"
    header = "Content-Type:application/json\r\n"
    ok, data = pcall(cjson.encode, {username = username, password = password})

    http.request(url,"POST",header,data,
        function(code, data)
        -- print some space to flush the previous outputs
        if data=="Success" then
            for i = 1,255 do 
                print()
            end
            print("$done")
        else
            for i = 1,255 do 
                print()
            end
            print("$fail")
        end
    end)
end

function send_picture_part(subpicture)
    raw_picture = raw_picture .. subpicture
end

function send_picture()
    url = "http://104.198.97.189:6000/pictureSegment"
    header = "Content-Type:application/json\r\n"
    data ='{"picture_segment":"'..raw_picture..'"}'

    http.request(url,"POST",header,data,
        function(code, data)
        
    end)

    raw_picture = ''
end

function send_finish()
    url = "http://104.198.97.189:6000/loginByFaceHexSegments"
    header = "Content-Type:application/json\r\n"
    data ='{"hex_string":"done"}'

    http.request(url,"POST",header,data,
        function(code, data)
        if data=="Success" then
            print("$done")
        else
            print("$fail")
        end
--        print(code, data)
    end)
end

-- send to local server; 
-- for debugging only
function send_test()
    url = "http://192.168.43.79:8080/Servlet/login"
    header = "Content-Type:application/json\r\n"
    data ='{"hex_string":"'..raw_picture..'"}'

    http.request(url,"POST",header,data,
        function(code, data)
          print(code, data)
    end)

    raw_picture = ''
    
end
