#!mruby

window = FLTK3::DoubleWindow.new(360, 330, "buttons")
window.begin do
  output = FLTK3::Output.new(10, 290, 340, 30)
  log = lambda { |text| output.value = text }

  FLTK3::Button.new(10, 10, 160, 30, "Button").callback { log.call("Button clicked") }

  ret = FLTK3::ReturnButton.new(190, 10, 160, 30, "ReturnButton")
  ret.callback { log.call("ReturnButton (Enter works too)") }

  toggle = FLTK3::ToggleButton.new(10, 50, 160, 30, "ToggleButton")
  toggle.callback { log.call("Toggle is #{toggle.value ? 'on' : 'off'}") }

  light = FLTK3::LightButton.new(190, 50, 160, 30, "LightButton")
  light.callback { log.call("Light is #{light.value ? 'on' : 'off'}") }

  check1 = FLTK3::CheckButton.new(10, 90, 160, 30, "CheckButton A")
  check2 = FLTK3::CheckButton.new(190, 90, 160, 30, "CheckButton B")
  check2.value = true
  [check1, check2].each do |c|
    c.callback { log.call("A=#{check1.value} B=#{check2.value}") }
  end

  group = FLTK3::Group.new(10, 130, 340, 70, "radio group")
  group.box = FLTK3::ENGRAVED_FRAME
  group.align = FLTK3::ALIGN_TOP_LEFT | FLTK3::ALIGN_INSIDE
  group.begin do
    %w(Red Green Blue).each_with_index do |name, i|
      r = FLTK3::RadioRoundButton.new(20 + i * 110, 155, 100, 30, name)
      r.value = true if i == 0
      r.callback { log.call("#{name} selected") }
    end
  end
  group.end

  repeat = FLTK3::RepeatButton.new(10, 210, 160, 30, "RepeatButton (hold)")
  count = 0
  repeat.callback { count += 1; log.call("repeated #{count} times") }

  colored = FLTK3::RoundButton.new(190, 210, 160, 30, "RoundButton")
  colored.callback { log.call("RoundButton clicked") }

  quit = FLTK3::Button.new(10, 250, 340, 30, "Quit (^q)")
  quit.shortcut = "^q"
  quit.color = FLTK3::RED
  quit.labelcolor = FLTK3::WHITE
  quit.callback do
    window.hide if FLTK3::ask("Do you really want to exit?")
  end
end
window.show

FLTK3::run
