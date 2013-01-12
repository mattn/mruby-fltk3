#!mruby

w = FLTK3::Window.new(100, 100, 100, 100, "mruby-fltk")
w.begin
  b = FLTK3::Button.new(10, 10, 80, 80, "Click Me!")
  b.callback do
    if FLTK3::ask "Do you really want to exit?"
      w.hide
    end
  end
w.end
w.show

FLTK3::run
