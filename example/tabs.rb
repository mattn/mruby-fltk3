#!mruby

# Child widgets are positioned relative to their parent group in fltk3.

window = FLTK3::DoubleWindow.new(320, 240, "tabs")
window.begin do
  tabs = FLTK3::TabGroup.new(10, 10, 300, 220)
  tabs.begin do
    FLTK3::Group.new(0, 25, 300, 195, "Buttons").begin do |g|
      FLTK3::Button.new(10, 15, 100, 30, "Button")
      FLTK3::CheckButton.new(10, 55, 100, 30, "Check")
      FLTK3::RoundButton.new(10, 95, 100, 30, "Round")
    end
    FLTK3::Group.new(0, 25, 300, 195, "Input").begin do |g|
      input = FLTK3::Input.new(70, 15, 200, 30, "Name:")
      button = FLTK3::Button.new(70, 55, 100, 30, "Say hi")
      button.callback do
        FLTK3::alert("Hello, #{input.value}!")
      end
    end
  end
  window.resizable = tabs
end
window.show

FLTK3::run
