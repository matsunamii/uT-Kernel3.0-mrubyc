Time.new(0, 0, 0)

print("clock_task\n")

loop do
  print("clock_task1\n")
  now = Time.now

  $hour = now.hour
  $min  = now.min
  $sec  = now.sec
  delay_ms 100
end