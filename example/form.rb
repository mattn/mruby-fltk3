#!mruby

window = FLTK3::DoubleWindow.new(340, 260, "form")
window.begin do
  name = FLTK3::Input.new(100, 10, 220, 25, "Name:")
  age = FLTK3::IntInput.new(100, 45, 80, 25, "Age:")
  password = FLTK3::SecretInput.new(100, 80, 220, 25, "Password:")
  note = FLTK3::MultilineInput.new(100, 115, 220, 60, "Note:")
  note.wrap = true
  result = FLTK3::MultilineOutput.new(100, 185, 220, 60, "Result:")

  button = FLTK3::ReturnButton.new(10, 220, 80, 30, "Submit")
  button.callback do
    result.value = "#{name.value} (#{age.value})\n#{note.value}"
    result.textcolor = password.value.empty? ? FLTK3::RED : FLTK3::BLACK
    result.redraw
  end
end
window.show

FLTK3::run
