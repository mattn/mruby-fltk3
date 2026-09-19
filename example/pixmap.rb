#!mruby

smile = FLTK3::Pixmap.new([
  "16 16 3 1",
  "  c None",
  ". c #ffcc00",
  "x c #000000",
  "     ......     ",
  "   ..........   ",
  "  ............  ",
  " .............. ",
  " ....xx..xx.... ",
  "......xx..xx....",
  "................",
  "................",
  "................",
  "...x........x...",
  " ...x......x... ",
  " ....xxxxxx.... ",
  "  ............  ",
  "   ..........   ",
  "     ......     ",
  "                ",
])

window = FLTK3::DoubleWindow.new(240, 160, "pixmap")
window.begin do
  big = smile.copy(64, 64)
  box = FLTK3::Widget.new(20, 20, 80, 80)
  box.box = FLTK3::THIN_DOWN_BOX
  box.image = big
  gray = big.copy
  gray.desaturate
  box.deimage = gray

  button = FLTK3::ToggleButton.new(120, 40, 100, 40, "Toggle")
  button.image = smile
  button.align = FLTK3::ALIGN_LEFT | FLTK3::ALIGN_INSIDE | FLTK3::ALIGN_IMAGE_NEXT_TO_TEXT
  button.callback do
    button.value ? box.deactivate : box.activate
  end

  tiled = FLTK3::Widget.new(20, 110, 200, 30)
  tiled.box = FLTK3::FLAT_BOX
  tiled.image = FLTK3::TiledImage.new(smile)
  tiled.align = FLTK3::ALIGN_INSIDE | FLTK3::ALIGN_TOP_LEFT
end
window.show

FLTK3::run
