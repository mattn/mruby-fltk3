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

assert('FLTK3::Widget colors, alignment and state') do
  w = FLTK3::Widget.new(0, 0, 10, 10, "label")
  w.color = FLTK3::RED
  assert_equal FLTK3::RED, w.color
  w.color(FLTK3::BLUE, FLTK3::GREEN)
  assert_equal FLTK3::BLUE, w.color
  assert_equal FLTK3::GREEN, w.selection_color
  w.labelcolor = FLTK3::WHITE
  assert_equal FLTK3::WHITE, w.labelcolor
  w.align = FLTK3::ALIGN_LEFT | FLTK3::ALIGN_INSIDE
  assert_equal FLTK3::ALIGN_LEFT | FLTK3::ALIGN_INSIDE, w.align
  w.tooltip = "tip"
  assert_equal "tip", w.tooltip
  w.when = FLTK3::WHEN_RELEASE
  assert_equal FLTK3::WHEN_RELEASE, w.when
  assert_true w.active?
  w.deactivate
  assert_false w.active?
  w.active = true
  assert_true w.active?
  w.resize(1, 2, 3, 4)
  assert_equal [1, 2, 3, 4], [w.x, w.y, w.w, w.h]
  w.position(5, 6)
  w.size(7, 8)
  assert_equal [5, 6, 7, 8], [w.x, w.y, w.w, w.h]
end

assert('FLTK3::Widget#box') do
  w = FLTK3::Widget.new(0, 0, 10, 10)
  w.box = FLTK3::UP_BOX
  assert_true w.box.equal?(FLTK3::UP_BOX)
  assert_equal "upBox", w.box.name
  w.box = FLTK3::DownBox.new("custom")
  assert_equal "custom", w.box.name
  assert_kind_of FLTK3::Box, w.box
end

assert('FLTK3::Widget#callback') do
  w = FLTK3::Widget.new(0, 0, 10, 10)
  called = nil
  w.callback(42) { |widget, data| called = [widget == w, data] }
  w.do_callback
  assert_equal [true, 42], called
  assert_equal 42, w.user_data
  w.callback = Proc.new { called = :proc }
  w.do_callback
  assert_equal :proc, called
  w.callback = nil
  assert_nil w.callback
end

assert('FLTK3::Widget#parent') do
  win = FLTK3::Window.new(100, 100)
  b = nil
  win.begin { b = FLTK3::Button.new(0, 0, 10, 10) }
  assert_true b.parent == win
  assert_kind_of FLTK3::Window, b.window
  assert_true win.contains(b)
  assert_true b.inside(win)
end

assert('FLTK3::Group children') do
  g = FLTK3::Group.new(0, 0, 100, 100)
  a = b = nil
  g.begin do
    a = FLTK3::Button.new(0, 0, 10, 10, "a")
    b = FLTK3::Button.new(0, 0, 10, 10, "b")
  end
  assert_equal ["a", "b"], g.children.map(&:label)
  assert_true g.child(0) == a
  assert_true g.child(-1) == b
  assert_nil g.child(2)
  assert_equal 1, g.find(b)
  g.remove(b)
  assert_equal 1, g.children.size
  assert_nil b.parent
  g << b
  assert_equal 2, g.children.size
  g.insert(b, 0)
  assert_true g.child(0) == b
  g.clear
  assert_equal 0, g.children.size
  assert_raise(RuntimeError) { a.label }
end

assert('FLTK3::TabGroup and WizardGroup') do
  t = FLTK3::TabGroup.new(0, 0, 100, 100)
  p1 = p2 = nil
  t.begin do
    p1 = FLTK3::Group.new(0, 20, 100, 80, "1"); p1.end
    p2 = FLTK3::Group.new(0, 20, 100, 80, "2"); p2.end
  end
  assert_true t.value == p1
  t.value = p2
  assert_true t.value == p2

  w = FLTK3::WizardGroup.new(0, 0, 100, 100)
  q1 = q2 = nil
  w.begin do
    q1 = FLTK3::Group.new(0, 0, 100, 100); q1.end
    q2 = FLTK3::Group.new(0, 0, 100, 100); q2.end
  end
  assert_true w.value == q1
  w.next
  assert_true w.value == q2
  w.prev
  assert_true w.value == q1
end

assert('FLTK3::PackedGroup') do
  g = FLTK3::PackedGroup.new(0, 0, 100, 100)
  g.type = FLTK3::PackedGroup::HORIZONTAL
  g.spacing = 5
  g.end
  assert_equal 5, g.spacing
  assert_true g.horizontal
end

assert('FLTK3::Window properties') do
  w = FLTK3::DoubleWindow.new(100, 100, "title")
  w.end
  assert_equal "title", w.label
  w.label = "changed"
  assert_equal "changed", w.label
  assert_false w.shown?
  assert_true w.border
  w.size_range(50, 50, 200, 200)
  w.set_modal
  assert_true w.modal
end
