#!mruby

window = FLTK3::DoubleWindow.new(260, 140, "timer")
window.begin do
  clock = FLTK3::Output.new(10, 10, 240, 40)
  clock.textsize = 24
  clock.textfont = FLTK3::COURIER_BOLD
  progress = FLTK3::Progress.new(10, 60, 240, 20)
  progress.maximum = 60
  button = FLTK3::ToggleButton.new(10, 90, 240, 40, "Pause")

  tick = nil
  tick = Proc.new do
    unless button.value
      now = Time.now
      clock.value = "%02d:%02d:%02d" % [now.hour, now.min, now.sec]
      progress.value = now.sec
    end
    FLTK3::repeat_timeout(1.0, &tick)
  end
  tick.call
end
window.show

FLTK3::run
