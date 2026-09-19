#!mruby

# A widget that draws itself and reacts to mouse events.

class Canvas < FLTK3::Widget
  def initialize(*args)
    super
    @strokes = []
  end

  def draw
    draw_box(FLTK3::FLAT_BOX, FLTK3::WHITE)
    FLTK3::color(FLTK3::BLACK)
    FLTK3::font(FLTK3::HELVETICA, 12)
    FLTK3::draw("click and drag to draw, right click to clear", 5, 15)
    FLTK3::color(FLTK3::BLUE)
    FLTK3::line_style(FLTK3::SOLID | FLTK3::CAP_ROUND | FLTK3::JOIN_ROUND, 3)
    @strokes.each do |stroke|
      FLTK3::begin_line
      stroke.each { |x, y| FLTK3::vertex(x, y) }
      FLTK3::end_line
    end
    FLTK3::line_style(0)
  end

  def handle(event)
    case event
    when FLTK3::PUSH
      if FLTK3::event_button == FLTK3::RIGHT_MOUSE
        @strokes.clear
      else
        @strokes << [[FLTK3::event_x - x, FLTK3::event_y - y]]
      end
      redraw
      true
    when FLTK3::DRAG
      @strokes.last << [FLTK3::event_x - x, FLTK3::event_y - y]
      redraw
      true
    else
      super
    end
  end
end

window = FLTK3::DoubleWindow.new(400, 300, "canvas")
window.begin do
  canvas = Canvas.new(10, 10, 380, 280)
  window.resizable = canvas
end
window.show

FLTK3::run
