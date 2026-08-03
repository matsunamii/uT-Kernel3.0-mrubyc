BUTTON = GPIO.new("PC13", GPIO::IN)

$hour = 0
$min  = 0
$sec  = 0

running = false
previous = 0
press_count = 0

Time.reset

loop do
  button = BUTTON.read

  if button == 1
    press_count += 1
    print("press_count: #{press_count}\n")

  elsif previous == 1
    if press_count >= 10
      Time.reset
      running = false
      print("reset\n")
    else
      if running
        Time.stop
        running = false
        print("stop\n")
      else
        Time.start
        running = true
        print("start\n")
      end
    end

    press_count = 0
  end

  previous = button

  elapsed = Time.elapsed

  $hour = elapsed.hour
  $min  = elapsed.min
  $sec  = elapsed.sec

  delay_ms 10
end