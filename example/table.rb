#!mruby

# A spreadsheet-like table drawn cell by cell.

class Sheet < FLTK3::TableRow
  def initialize(*args)
    super
    @data = Array.new(20) { |r| Array.new(6) { |c| (r + 1) * (c + 1) } }
    self.rows = @data.size
    self.cols = @data[0].size
    self.row_header = 1
    self.col_header = 1
    self.row_height_all = 22
    self.col_width_all = 70
    self.type = FLTK3::TableRow::SELECT_SINGLE
  end

  def draw_header(text, x, y, w, h)
    FLTK3::push_clip(x, y, w, h)
    FLTK3::draw_box(FLTK3::THIN_UP_BOX, x, y, w, h, row_header_color)
    FLTK3::color(FLTK3::BLACK)
    FLTK3::draw(text, x, y, w, h, FLTK3::ALIGN_CENTER)
    FLTK3::pop_clip
  end

  def draw_cell(context, r, c, x, y, w, h)
    case context
    when FLTK3::Table::CONTEXT_STARTPAGE
      FLTK3::font(FLTK3::HELVETICA, 14)
    when FLTK3::Table::CONTEXT_COL_HEADER
      draw_header(("A".ord + c).chr, x, y, w, h)
    when FLTK3::Table::CONTEXT_ROW_HEADER
      draw_header((r + 1).to_s, x, y, w, h)
    when FLTK3::Table::CONTEXT_CELL
      FLTK3::push_clip(x, y, w, h)
      FLTK3::color(row_selected?(r) ? FLTK3::YELLOW : FLTK3::WHITE)
      FLTK3::rectf(x, y, w, h)
      FLTK3::color(FLTK3::BLACK)
      FLTK3::draw(@data[r][c].to_s, x, y, w, h, FLTK3::ALIGN_RIGHT | FLTK3::ALIGN_INSIDE)
      FLTK3::color(FLTK3::LIGHT2)
      FLTK3::rect(x, y, w, h)
      FLTK3::pop_clip
    end
  end
end

window = FLTK3::DoubleWindow.new(500, 400, "table")
window.begin do
  sheet = Sheet.new(10, 10, 480, 350)
  status = FLTK3::Output.new(10, 365, 480, 25)
  sheet.callback do |t|
    if t.callback_context == FLTK3::Table::CONTEXT_CELL
      status.value = "row #{t.callback_row + 1}, column #{("A".ord + t.callback_col).chr}"
    end
  end
  window.resizable = sheet
end
window.show

FLTK3::run
