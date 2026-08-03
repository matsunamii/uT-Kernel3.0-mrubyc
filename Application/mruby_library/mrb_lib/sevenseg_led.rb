class SevensegLed
  PATTERNS = [
    [0, 0, 0, 0, 0, 0, 1, 1], # 0
    [1, 0, 0, 1, 1, 1, 1, 1], # 1
    [0, 0, 1, 0, 0, 1, 0, 1], # 2
    [0, 0, 0, 0, 1, 1, 0, 1], # 3
    [1, 0, 0, 1, 1, 0, 0, 1], # 4
    [0, 1, 0, 0, 1, 0, 0, 1], # 5
    [0, 1, 0, 0, 0, 0, 0, 1], # 6
    [0, 0, 0, 1, 1, 1, 1, 1], # 7
    [0, 0, 0, 0, 0, 0, 0, 1], # 8
    [0, 0, 0, 0, 1, 0, 0, 1]  # 9
  ]

  def initialize
    @digits   = [DIG1, DIG2, DIG3, DIG4, DIG5, DIG6]
    @segments = [A, B, C, D, E, F, G, DP]

    all_segments_off
    all_digits_off
  end

  def all_digits_off
    # 共通アノード：LOWで桁を無効化
    @digits.each do |digit|
      digit.write(0)
    end
  end

  def all_segments_off
    # 共通アノード：HIGHで消灯
    @segments.each do |segment|
      segment.write(1)
    end
  end

  def display_digit(number, decimal_point = false)
    pattern = PATTERNS[number]

    @segments.each_with_index do |segment, index|
      if index == 7
        # DPはLOWで点灯
        segment.write(decimal_point ? 0 : 1)
      else
        segment.write(pattern[index])
      end
    end
  end

  def scan_digit(index, number, decimal_point = false)
    # ゴースト表示防止
    all_digits_off

    display_digit(number, decimal_point)

    # 共通アノード：HIGHで対象桁を有効化
    @digits[index].write(1)

    # 1桁の点灯時間
    delay_ms 0.1

    # 次の桁へ移る前に消灯
    @digits[index].write(0)
  end

  def scan_time(numbers)
    numbers.each_with_index do |number, index|
      # HH.MM.SSの区切りとして2桁目と4桁目のDPを点灯
      point = (index == 1 || index == 3)

      scan_digit(index, number, point)
    end
  end
end