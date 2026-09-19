#!mruby

window = FLTK3::DoubleWindow.new(400, 300, "menu")
window.begin do
  editor = FLTK3::TextEditor.new(0, 25, 400, 275)
  editor.buffer = FLTK3::TextBuffer.new

  menubar = FLTK3::MenuBar.new(0, 0, 400, 25)
  menubar.add("&File/&Open...", "^o") do
    file = FLTK3::file_chooser("Open file", "*")
    editor.buffer.text = File.read(file) if file
  end
  menubar.add("&File/&Quit", "^q", FLTK3::MENU_DIVIDER) { window.hide }
  menubar.add("&Edit/&Clear") { editor.buffer.text = "" }
  menubar.add("&View/&Large font", 0, FLTK3::MENU_TOGGLE) do |menu, item|
    editor.textsize = item.value ? 20 : 14
    editor.redraw
  end
  menubar.add("&Help/&About") { FLTK3::alert("mruby-fltk3 menu example") }

  popup = FLTK3::MenuButton.new(0, 25, 400, 275)
  popup.type = FLTK3::MenuButton::POPUP3
  popup.add("Insert date") { editor.buffer.text = editor.buffer.text + Time.now.to_s + "\n" }
  popup.add("Clear") { editor.buffer.text = "" }

  window.resizable = editor
end
window.show

FLTK3::run
