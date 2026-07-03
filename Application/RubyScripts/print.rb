$sleep_time = 1000

led = GPIO.new("PA5", GPIO::OUT)

while true
  state = led.read

  if state == 1
    print "LED: ON\n"
  else
    print "LED: OFF\n"
  end

  sleep_ms $sleep_time
end