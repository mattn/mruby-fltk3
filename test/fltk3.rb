assert('FLTK3::Widget properties') do
  w = FLTK3::Widget.new(1, 2, 3, 4, "label")
  assert_equal 1, w.x
  assert_equal 2, w.y
  assert_equal 3, w.w
  assert_equal 4, w.h
  assert_equal "label", w.label
  w.label = "changed"
  assert_equal "changed", w.label
  w.labelsize = 20
  assert_equal 20, w.labelsize
end

assert('FLTK3::Window and Group') do
  win = FLTK3::Window.new(100, 100, "win")
  button = nil
  win.begin do
    button = FLTK3::Button.new(0, 0, 10, 10, "b")
  end
  win.resizable = button
  assert_true win.resizable.equal?(button)
end

assert('FLTK3::Input#value') do
  i = FLTK3::Input.new(0, 0, 10, 10)
  i.value = "hello"
  assert_equal "hello", i.value
end

assert('FLTK3::Browser') do
  b = FLTK3::Browser.new(0, 0, 10, 10)
  b.add "one"
  b.add "two"
  assert_equal "one", b.text(1)
  b.text(2, "TWO")
  assert_equal "TWO", b.text(2)
  b.column_widths = [10, 20]
  assert_equal [10, 20], b.column_widths
end

assert('FLTK3::TextBuffer') do
  buf = FLTK3::TextBuffer.new
  buf.text = "hello"
  assert_equal "hello", buf.text
  assert_equal 5, buf.length
  ed = FLTK3::TextEditor.new(0, 0, 10, 10)
  ed.buffer = buf
  assert_true ed.buffer.equal?(buf)
end
