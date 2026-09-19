#include "mrb_fltk3.h"
#include <fltk3/Table.h>
#include <fltk3/TableRow.h>

/*********************************************************
 * FLTK3::Table / FLTK3::TableRow
 *
 * A table draws nothing by itself; subclass it in Ruby and define
 * draw_cell(context, row, col, x, y, w, h).
 *********************************************************/
static mrb_value mrb_fltk3_table_draw_cell(mrb_state* mrb, mrb_value self);

struct MrbTableAccess {
  virtual int table_find_cell(fltk3::Table::TableContext c, int r, int col, int& x, int& y, int& w, int& h) = 0;
  virtual void table_redraw_range(int a, int b, int c, int d) = 0;
  virtual ~MrbTableAccess() {}
};

template <class T>
class MrbTable : public MrbWidget<T>, public MrbTableAccess {
public:
  using MrbWidget<T>::MrbWidget;
  int table_find_cell(fltk3::Table::TableContext c, int r, int col, int& x, int& y, int& w, int& h) { return T::find_cell(c, r, col, x, y, w, h); }
  void table_redraw_range(int a, int b, int c, int d) { T::redraw_range(a, b, c, d); }
  void draw_cell(fltk3::Table::TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
    mrb_state* mrb = mrb_fltk3_state;
    mrb_value instance = mrb ? mrb_fltk3_registered(mrb, this) : mrb_nil_value();
    if (mrb_nil_p(instance)) return;
    mrb_value proc = mrb_iv_get(mrb, instance, mrb_intern_lit(mrb, "draw_cell_proc"));
    mrb_value args[8];
    args[0] = instance;
    args[1] = mrb_fixnum_value(context);
    args[2] = mrb_fixnum_value(R);
    args[3] = mrb_fixnum_value(C);
    args[4] = mrb_fixnum_value(X);
    args[5] = mrb_fixnum_value(Y);
    args[6] = mrb_fixnum_value(W);
    args[7] = mrb_fixnum_value(H);
    if (!mrb_nil_p(proc)) {
      mrb_fltk3_call(mrb, proc, 8, args);
    } else {
      mrb_fltk3_send(mrb, instance, "draw_cell", 7, args + 1);
    }
  }
};

#define DECLARE_TABLE(x)                                                  \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_int x_, y_, w_, h_;                                                 \
  mrb_value label = mrb_nil_value();                                      \
  mrb_get_args(mrb, "iiii|S!", &x_, &y_, &w_, &h_, &label);              \
  fltk3::Widget* w = new MrbTable<fltk3::x>((int) x_, (int) y_, (int) w_, (int) h_, \
    mrb_nil_p(label) ? NULL : RSTRING_CSTR(mrb, label));                  \
  return mrb_fltk3_Widget_setup(mrb, self, w);                            \
}

DECLARE_TABLE(Table)
DECLARE_TABLE(TableRow)

/* draw_cell(context, r, c, x, y, w, h)          -> default: nothing
 * draw_cell { |table, context, r, c, x, y, w, h| } -> install a block */
static mrb_value
mrb_fltk3_table_draw_cell(mrb_state* mrb, mrb_value self)
{
  mrb_value b = mrb_nil_value();
  mrb_value* argv;
  mrb_int argc;
  mrb_get_args(mrb, "&*", &b, &argv, &argc);
  if (!mrb_nil_p(b)) mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "draw_cell_proc"), b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_table_visible_cells(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  int r1 = 0, r2 = 0, c1 = 0, c2 = 0;
  ((fltk3::Table*) context->v)->visible_cells(r1, r2, c1, c2);
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(r1));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(r2));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(c1));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(c2));
  return ret;
}

static mrb_value
mrb_fltk3_table_row_height(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int row, height;
  if (mrb_get_args(mrb, "i|i", &row, &height) == 1) return mrb_fixnum_value(((fltk3::Table*) context->v)->row_height((int) row));
  ((fltk3::Table*) context->v)->row_height((int) row, (int) height);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_table_col_width(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int col, width;
  if (mrb_get_args(mrb, "i|i", &col, &width) == 1) return mrb_fixnum_value(((fltk3::Table*) context->v)->col_width((int) col));
  ((fltk3::Table*) context->v)->col_width((int) col, (int) width);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_table_is_selected(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int r, c;
  mrb_get_args(mrb, "ii", &r, &c);
  return mrb_bool_value(((fltk3::Table*) context->v)->is_selected((int) r, (int) c) != 0);
}

static mrb_value
mrb_fltk3_table_get_selection(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  int rt = 0, cl = 0, rb = 0, cr = 0;
  ((fltk3::Table*) context->v)->get_selection(rt, cl, rb, cr);
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(rt));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(cl));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(rb));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(cr));
  return ret;
}

static mrb_value
mrb_fltk3_table_set_selection(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int rt, cl, rb, cr;
  mrb_get_args(mrb, "iiii", &rt, &cl, &rb, &cr);
  ((fltk3::Table*) context->v)->set_selection((int) rt, (int) cl, (int) rb, (int) cr);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_table_move_cursor(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int r, c;
  mrb_get_args(mrb, "ii", &r, &c);
  return mrb_bool_value(((fltk3::Table*) context->v)->move_cursor((int) r, (int) c) != 0);
}

static mrb_value
mrb_fltk3_table_find_cell(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int ctx, r, c;
  mrb_get_args(mrb, "iii", &ctx, &r, &c);
  int x = 0, y = 0, w = 0, h = 0;
  MrbTableAccess* t = dynamic_cast<MrbTableAccess*>(context->v);
  if (!t || t->table_find_cell((fltk3::Table::TableContext) ctx, (int) r, (int) c, x, y, w, h) != 0) return mrb_nil_value();
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(x));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(y));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(h));
  return ret;
}

static mrb_value
mrb_fltk3_table_redraw_range(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int a, b, c, d;
  mrb_get_args(mrb, "iiii", &a, &b, &c, &d);
  MrbTableAccess* t = dynamic_cast<MrbTableAccess*>(context->v);
  if (t) t->table_redraw_range((int) a, (int) b, (int) c, (int) d);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_table_do_callback(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int ctx, r, c;
  if (mrb_get_args(mrb, "|iii", &ctx, &r, &c) == 0) {
    context->v->do_callback();
  } else {
    ((fltk3::Table*) context->v)->do_callback((fltk3::Table::TableContext) ctx, (int) r, (int) c);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_tablerow_row_selected(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int row;
  mrb_get_args(mrb, "i", &row);
  return mrb_bool_value(((fltk3::TableRow*) context->v)->row_selected((int) row) == 1);
}

static mrb_value
mrb_fltk3_tablerow_select_row(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int row, flag = 1;
  mrb_get_args(mrb, "i|i", &row, &flag);
  return mrb_bool_value(((fltk3::TableRow*) context->v)->select_row((int) row, (int) flag) == 1);
}

static mrb_value
mrb_fltk3_tablerow_select_all_rows(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int flag = 1;
  mrb_get_args(mrb, "|i", &flag);
  ((fltk3::TableRow*) context->v)->select_all_rows((int) flag);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_tablerow_selected_rows(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::TableRow* t = (fltk3::TableRow*) context->v;
  mrb_value ary = mrb_ary_new(mrb);
  for (int r = 0; r < t->rows(); r++) {
    if (t->row_selected(r) == 1) mrb_ary_push(mrb, ary, mrb_fixnum_value(r));
  }
  return ary;
}

#define DEFINE_TABLE_FLAG_PROP(z) \
  DEFINE_INT_GETTER(Table, Widget, z) \
  DEFINE_INT_SETTER(Table, Widget, z)

void
mrb_fltk3_table_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  DEFINE_CLASS(Table, Group);
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_NONE", mrb_fixnum_value(fltk3::Table::CONTEXT_NONE));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_STARTPAGE", mrb_fixnum_value(fltk3::Table::CONTEXT_STARTPAGE));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_ENDPAGE", mrb_fixnum_value(fltk3::Table::CONTEXT_ENDPAGE));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_ROW_HEADER", mrb_fixnum_value(fltk3::Table::CONTEXT_ROW_HEADER));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_COL_HEADER", mrb_fixnum_value(fltk3::Table::CONTEXT_COL_HEADER));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_CELL", mrb_fixnum_value(fltk3::Table::CONTEXT_CELL));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_TABLE", mrb_fixnum_value(fltk3::Table::CONTEXT_TABLE));
  mrb_define_const(mrb, _class_fltk3_Table, "CONTEXT_RC_RESIZE", mrb_fixnum_value(fltk3::Table::CONTEXT_RC_RESIZE));
  mrb_define_method(mrb, _class_fltk3_Table, "draw_cell", mrb_fltk3_table_draw_cell, MRB_ARGS_ANY() | MRB_ARGS_BLOCK());
  DEFINE_INT_PROP(Table, Widget, rows);
  DEFINE_INT_PROP(Table, Widget, cols);
  DEFINE_INT_PROP(Table, Widget, row_position);
  DEFINE_INT_PROP(Table, Widget, col_position);
  DEFINE_INT_PROP(Table, Widget, top_row);
  DEFINE_INT_PROP(Table, Widget, col_header_height);
  DEFINE_INT_PROP(Table, Widget, row_header_width);
  DEFINE_INT_PROP(Table, Widget, row_header_color);
  DEFINE_INT_PROP(Table, Widget, col_header_color);
  DEFINE_INT_PROP(Table, Widget, col_resize_min);
  DEFINE_INT_PROP(Table, Widget, row_resize_min);
  DEFINE_TABLE_FLAG_PROP(row_resize);
  DEFINE_TABLE_FLAG_PROP(col_resize);
  DEFINE_TABLE_FLAG_PROP(row_header);
  DEFINE_TABLE_FLAG_PROP(col_header);
  DEFINE_INT_SETTER(Table, Widget, row_height_all);
  DEFINE_INT_SETTER(Table, Widget, col_width_all);
  DEFINE_INT_GETTER(Table, Widget, callback_row);
  DEFINE_INT_GETTER(Table, Widget, callback_col);
  DEFINE_INT_GETTER(Table, Widget, callback_context);
  DEFINE_BOOL_GETTER(Table, Widget, is_interactive_resize);
  DEFINE_VOID_METHOD(Table, Widget, clear);
  mrb_define_method(mrb, _class_fltk3_Table, "visible_cells", mrb_fltk3_table_visible_cells, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Table, "row_height", mrb_fltk3_table_row_height, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Table, "col_width", mrb_fltk3_table_col_width, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Table, "is_selected", mrb_fltk3_table_is_selected, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Table, "get_selection", mrb_fltk3_table_get_selection, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Table, "set_selection", mrb_fltk3_table_set_selection, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, _class_fltk3_Table, "move_cursor", mrb_fltk3_table_move_cursor, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Table, "find_cell", mrb_fltk3_table_find_cell, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, _class_fltk3_Table, "redraw_range", mrb_fltk3_table_redraw_range, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, _class_fltk3_Table, "do_callback", mrb_fltk3_table_do_callback, MRB_ARGS_OPT(3));
  ARENA_RESTORE;

  DEFINE_CLASS(TableRow, Table);
  mrb_define_const(mrb, _class_fltk3_TableRow, "SELECT_NONE", mrb_fixnum_value(fltk3::TableRow::SELECT_NONE));
  mrb_define_const(mrb, _class_fltk3_TableRow, "SELECT_SINGLE", mrb_fixnum_value(fltk3::TableRow::SELECT_SINGLE));
  mrb_define_const(mrb, _class_fltk3_TableRow, "SELECT_MULTI", mrb_fixnum_value(fltk3::TableRow::SELECT_MULTI));
  mrb_define_method(mrb, _class_fltk3_TableRow, "row_selected", mrb_fltk3_tablerow_row_selected, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TableRow, "row_selected?", mrb_fltk3_tablerow_row_selected, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TableRow, "select_row", mrb_fltk3_tablerow_select_row, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TableRow, "select_all_rows", mrb_fltk3_tablerow_select_all_rows, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TableRow, "selected_rows", mrb_fltk3_tablerow_selected_rows, MRB_ARGS_NONE());
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
