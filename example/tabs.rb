#!mruby

window = FLTK3::DoubleWindow.new(320, 240, "tabs")
window.begin do
  tabs = FLTK3::TabGroup.new(10, 10, 300, 220)
  tabs.begin do
    FLTK3::Group.new(10, 35, 300, 195, "Buttons").begin do |g|
      FLTK3::Button.new(20, 50, 100, 30, "Button")
      FLTK3::CheckButton.new(20, 90, 100, 30, "Check")
      FLTK3::RoundButton.new(20, 130, 100, 30, "Round")
    end
    FLTK3::Group.new(10, 35, 300, 195, "Input").begin do |g|
      input = FLTK3::Input.new(80, 50, 200, 30, "Name:")
      button = FLTK3::Button.new(80, 90, 100, 30, "Say hi")
      button.callback do
        FLTK3::alert("Hello, #{input.value}!")
      end
    end
  end
  window.resizable = tabs
end
window.show

FLTK3::run
