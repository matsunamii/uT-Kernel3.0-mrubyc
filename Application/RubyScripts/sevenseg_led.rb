A    = GPIO.new("PB15", GPIO::OUT)
B    = GPIO.new("PB7", GPIO::OUT)
C    = GPIO.new("PB6", GPIO::OUT)
D    = GPIO.new("PB3",  GPIO::OUT)
E    = GPIO.new("PB5",  GPIO::OUT)
F    = GPIO.new("PB4", GPIO::OUT)
G    = GPIO.new("PB10",  GPIO::OUT)
DP   = GPIO.new("PA8",  GPIO::OUT)

DIG1 = GPIO.new("PC7", GPIO::OUT)
DIG2 = GPIO.new("PC6", GPIO::OUT)
DIG3 = GPIO.new("PC9", GPIO::OUT)
DIG4 = GPIO.new("PA7", GPIO::OUT)
DIG5 = GPIO.new("PA6", GPIO::OUT)
DIG6 = GPIO.new("PA5", GPIO::OUT)
$hour = 0
$min  = 0
$sec  = 0
led = SevensegLed.new
loop do
  numbers = [
    $hour / 10,
    $hour % 10,
    $min / 10,
    $min % 10,
    $sec / 10,
    $sec % 10
  ]
  led.scan_time(numbers)
end