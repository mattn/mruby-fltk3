#!mruby

# A small text editor with a status line and simple keyword highlighting.

window = FLTK3::DoubleWindow.new(500, 400, "editor")
buffer = FLTK3::TextBuffer.new
style = FLTK3::TextBuffer.new
keywords = %w(def end class module if else elsif unless while do begin rescue return)

restyle = lambda do
  text = buffer.text
  styles = "A" * text.length
  pos = 0
  text.split(/\b/).each do |word|
    styles[pos, word.length] = "B" * word.length if keywords.include?(word)
    pos += word.length
  end
  style.text = styles
end

window.begin do
  editor = FLTK3::TextEditor.new(0, 25, 500, 350)
  editor.buffer = buffer
  editor.textfont = FLTK3::COURIER
  editor.linenumber_width = 40
  editor.wrap_mode(FLTK3::TextDisplay::WRAP_AT_BOUNDS, 0)
  editor.highlight_data(style, [
    [FLTK3::BLACK, FLTK3::COURIER, 14],
    [FLTK3::BLUE, FLTK3::COURIER_BOLD, 14],
  ])

  status = FLTK3::Output.new(0, 375, 500, 25)
  buffer.add_modify_callback do |pos, inserted, deleted, restyled, text|
    restyle.call if inserted > 0 || deleted > 0
    status.value = "#{buffer.length} bytes, #{buffer.count_lines(0, buffer.length) + 1} lines"
  end

  menubar = FLTK3::MenuBar.new(0, 0, 500, 25)
  menubar.add("&File/&Open...", "^o") do
    file = FLTK3::file_chooser("Open", "*")
    buffer.loadfile(file) if file
  end
  menubar.add("&File/&Save as...", "^s") do
    file = FLTK3::file_chooser("Save as", "*")
    buffer.savefile(file) if file
  end
  menubar.add("&File/&Quit", "^q") { window.hide }
  menubar.add("&Edit/&Undo", "^z") { buffer.undo }
  menubar.add("&Edit/Cu&t", "^x") { editor.kf_cut }
  menubar.add("&Edit/&Copy", "^c") { editor.kf_copy }
  menubar.add("&Edit/&Paste", "^v") { editor.kf_paste }
  menubar.add("&Edit/Select &all", "^a") { editor.kf_select_all }
  menubar.add("&Search/&Find...", "^f") do
    word = FLTK3::input("Find:", "") rescue nil
    if word && !word.empty?
      pos = buffer.search_forward(editor.insert_position, word)
      if pos
        buffer.select(pos, pos + word.length)
        editor.insert_position = pos + word.length
        editor.show_insert_position
      end
    end
  end

  window.resizable = editor
end
buffer.text = "def hello\n  puts 'hello'\nend\n"
window.show

FLTK3::run
