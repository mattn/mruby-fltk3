# mruby-fltk3

[FLTK 3](http://www.fltk.org/) bindings for [mruby](https://github.com/mruby/mruby).

## Install

Add the gem to your `build_config.rb`. `fltk3-config` must be on the `PATH`.

```ruby
conf.gem :github => 'mattn/mruby-fltk3'
```

## Example

```ruby
window = FLTK3::DoubleWindow.new(200, 100, "hello")
window.begin do
  button = FLTK3::Button.new(10, 10, 180, 30, "Click me")
  button.callback do
    FLTK3::alert("Hello, mruby!")
  end
end
window.show

FLTK3::run
```

See the `example` directory for more: buttons, form, tabs, menu, browser,
editor, tree, table, valuators, chart, canvas, pixmap, image and timer.

## API overview

Classes live under the `FLTK3` module and follow the `fltk3::` names, so
`fltk3::DoubleWindow` is `FLTK3::DoubleWindow`. Getters and setters are Ruby
style (`w.label`, `w.label = "x"`, `w.visible?`). Constants keep their fltk3
names (`FLTK3::RED`, `FLTK3::ALIGN_LEFT`, `FLTK3::WHEN_RELEASE`, `FLTK3::UP_BOX`).

### Widgets

- `Widget`: geometry, label, colors, fonts, alignment, tooltip, `box`, `image`,
  `callback { |widget, user_data| }`, `show`/`hide`, `activate`/`deactivate`,
  `parent`, `window`, `resize`, `redraw`, `draw`, `handle`
- Groups: `Group`, `PackedGroup`, `TabGroup`, `TiledGroup`, `ScrollGroup`,
  `WizardGroup`; windows: `Window`, `DoubleWindow`, `SingleWindow`, `MenuWindow`
- Buttons: `Button`, `CheckButton`, `LightButton`, `RadioButton`,
  `RadioLightButton`, `RadioRoundButton`, `RepeatButton`, `ReturnButton`,
  `RoundButton`, `ToggleButton`, `ToggleLightButton`, `ToggleRoundButton`
- Text input: `Input`, `Output`, `MultilineInput`, `MultilineOutput`,
  `IntInput`, `FloatInput`, `SecretInput`, `FileInput`, `InputChoice`
- Valuators: `Slider` (and `Hor`/`Fill`/`Nice`/`Value` variants), `Scrollbar`,
  `Counter`, `SimpleCounter`, `Dial`, `FillDial`, `LineDial`, `Roller`,
  `Adjuster`, `ValueInput`, `ValueOutput`, `Spinner`, `Progress`
- Menus: `MenuBar`, `MenuButton`, `Choice`, `MenuItem`
- Lists: `Browser`, `SelectBrowser`, `HoldBrowser`, `MultiBrowser`,
  `FileBrowser`, `CheckBrowser`, `Tree`, `TreeItem`, `Table`, `TableRow`
- Text: `TextBuffer`, `TextDisplay`, `TextEditor`
- Misc: `Clock`, `RoundClock`, `Chart`, `Positioner`, `ColorChooser`,
  `HelpView`, `FileChooser`, `Tooltip`
- Images: `Image`, `SharedImage`, `RGBImage`, `Bitmap`, `Pixmap`, `TiledImage`,
  `PNGImage`, `JPEGImage`, `GIFImage`, `BMPImage`, `XPMImage`, `XBMImage`,
  `PNMImage`

### Module functions

- Event loop: `run`, `wait`, `check`, `ready`, `flush`, `redraw`,
  `add_timeout(sec) { }`, `repeat_timeout`, `remove_timeout`, `add_idle { }`,
  `add_check { }`, `first_window`, `windows`, `delete_widget`
- Events (inside `handle`): `event`, `event_x`, `event_y`, `event_button`,
  `event_key`, `event_state`, `event_text`, `event_clicks`, `event_inside`,
  `focus`, `pushed`, `belowmouse`, `copy`, `paste`
- Dialogs: `message`, `alert`, `ask`, `choice`, `input`, `password`, `beep`,
  `file_chooser`, `dir_chooser`, `color_chooser`
- Appearance: `scheme`, `background`, `foreground`, `set_color`, `get_color`,
  `rgb_color`, `lighter`, `darker`, `contrast`, `set_font`, `font_name`,
  `set_fonts`, `option`
- Handlers: `warning { |msg| }`, `error { |msg| }`, `fatal { |msg| }`

### Drawing

Override `draw` in a `Widget` subclass (or pass a block to `widget.draw { }`)
and use the drawing functions `FLTK3::color`, `rect`, `rectf`, `line`, `loop`,
`polygon`, `arc`, `pie`, `circle`, `begin_line`/`vertex`/`end_line`, `font`,
`draw(text, x, y)`, `width`, `height`, `push_clip`/`pop_clip`, `line_style`,
`draw_image`, `read_image` and so on. Widgets also have `draw_box` and
`draw_label`.

```ruby
class Canvas < FLTK3::Widget
  def draw
    draw_box(FLTK3::FLAT_BOX, FLTK3::WHITE)
    FLTK3::color(FLTK3::RED)
    FLTK3::circle(w / 2.0, h / 2.0, 20.0)
  end

  def handle(event)
    return true if event == FLTK3::PUSH
    super
  end
end
```

`Table` subclasses define `draw_cell(context, row, col, x, y, w, h)` instead.

## Notes

- fltk3 positions child widgets **relative to their parent group**, and inside
  `handle` the values of `FLTK3::event_x` / `event_y` and all drawing
  coordinates are relative to the widget's own origin.
- Callbacks run while fltk3 owns the stack. An exception raised in a callback is
  caught and re-raised from `FLTK3::run` (or `wait` / `check`).
- Widgets created from Ruby stay alive as long as fltk3 holds them; wrappers
  for `MenuItem` and `TreeItem` are only valid until the menu or tree changes.
- `FLTK3::set_font(face, name)` takes the Xft style string used by fltk3: the
  first character selects the style (`" "` normal, `"B"` bold, `"I"` italic,
  `"P"` bold italic), e.g. `FLTK3::set_font(FLTK3::HELVETICA, " DejaVu Sans")`.
  If text renders as boxes, the font fontconfig picks for `sans` lacks the
  glyphs; check `fc-match sans` and prefer a font that covers your language.
- `XOpenIM() failed` printed at startup means the X input method named by
  `XMODIFIERS` is not running; it is harmless and can be silenced with
  `FLTK3::warning { |msg| }` or `XMODIFIERS=@im=none`.

## Tests

The tests in `test/` run with `rake test` of an mruby tree that includes this
gem and need a display (`xvfb-run rake test` works).

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a mattn)
