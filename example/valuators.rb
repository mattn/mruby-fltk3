#!mruby

window = FLTK3::DoubleWindow.new(320, 300, "valuators")
window.begin do
  output = FLTK3::ValueOutput.new(80, 10, 100, 25, "value")
  slider = FLTK3::HorValueSlider.new(80, 45, 220, 25, "slider")
  slider.bounds(0, 100)
  slider.step = 1
  slider.callback { output.value = slider.value }

  dial = FLTK3::Dial.new(80, 80, 60, 60, "dial")
  dial.bounds(0, 100)
  dial.callback { output.value = dial.value }

  counter = FLTK3::Counter.new(160, 95, 140, 25, "counter")
  counter.bounds(0, 100)
  counter.step = 1
  counter.callback { output.value = counter.value }

  spinner = FLTK3::Spinner.new(80, 150, 100, 25, "spinner")
  spinner.range(0, 100)
  spinner.callback { output.value = spinner.value }

  progress = FLTK3::Progress.new(80, 190, 220, 25, "progress")
  progress.maximum = 100
  progress.selection_color = FLTK3::BLUE

  roller = FLTK3::Roller.new(80, 230, 220, 25, "roller")
  roller.type = FLTK3::HORIZONTAL
  roller.bounds(0, 100)
  roller.callback do
    output.value = roller.value
    progress.value = roller.value
  end
end
window.show

FLTK3::run
