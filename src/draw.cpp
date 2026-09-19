#include "mrb_fltk3.h"
#include <fltk3/draw.h>

/*********************************************************
 * FLTK3 drawing functions (valid inside Widget#draw)
 *
 * Coordinates are relative to the widget being drawn, as in fltk3.
 *********************************************************/
static mrb_value
mrb_fltk3_draw_color(mrb_state* mrb, mrb_value self)
{
  mrb_int c, g, b;
  mrb_int argc = mrb_get_args(mrb, "|iii", &c, &g, &b);
  if (argc == 0) return mrb_fixnum_value(fltk3::color());
  if (argc == 1) fltk3::color((fltk3::Color) c);
  else if (argc == 3) fltk3::color((uchar) c, (uchar) g, (uchar) b);
  else mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  return mrb_nil_value();
}

#define DEFINE_RECT_FUNC(name) \
static mrb_value \
mrb_fltk3_draw_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_int x, y, w, h, c; \
  if (mrb_get_args(mrb, "iiii|i", &x, &y, &w, &h, &c) == 5) fltk3::name(x, y, w, h, (fltk3::Color) c); \
  else fltk3::name(x, y, w, h); \
  return mrb_nil_value(); \
}

DEFINE_RECT_FUNC(rect)
DEFINE_RECT_FUNC(rectf)

static mrb_value
mrb_fltk3_draw_point(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  fltk3::point(x, y);
  return mrb_nil_value();
}

/* line(x, y, x1, y1 [, x2, y2]) */
static mrb_value
mrb_fltk3_draw_line(mrb_state* mrb, mrb_value self)
{
  mrb_int a[6];
  mrb_int argc = mrb_get_args(mrb, "iiii|ii", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
  if (argc == 4) fltk3::line(a[0], a[1], a[2], a[3]);
  else if (argc == 6) fltk3::line(a[0], a[1], a[2], a[3], a[4], a[5]);
  else mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  return mrb_nil_value();
}

#define DEFINE_POLY_FUNC(name) \
static mrb_value \
mrb_fltk3_draw_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_int a[8]; \
  mrb_int argc = mrb_get_args(mrb, "iiiiii|ii", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7]); \
  if (argc == 6) fltk3::name(a[0], a[1], a[2], a[3], a[4], a[5]); \
  else if (argc == 8) fltk3::name(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]); \
  else mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments"); \
  return mrb_nil_value(); \
}

DEFINE_POLY_FUNC(loop)
DEFINE_POLY_FUNC(polygon)

#define DEFINE_XYLINE_FUNC(name) \
static mrb_value \
mrb_fltk3_draw_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_int a[5]; \
  mrb_int argc = mrb_get_args(mrb, "iii|ii", &a[0], &a[1], &a[2], &a[3], &a[4]); \
  if (argc == 3) fltk3::name(a[0], a[1], a[2]); \
  else if (argc == 4) fltk3::name(a[0], a[1], a[2], a[3]); \
  else fltk3::name(a[0], a[1], a[2], a[3], a[4]); \
  return mrb_nil_value(); \
}

DEFINE_XYLINE_FUNC(xyline)
DEFINE_XYLINE_FUNC(yxline)

/* arc(x, y, w, h, a1, a2) or arc(x, y, r, start, end) in path mode */
static mrb_value
mrb_fltk3_draw_arc(mrb_state* mrb, mrb_value self)
{
  mrb_value a[6];
  mrb_int argc = mrb_get_args(mrb, "ooooo|o", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
  if (argc == 5) {
    fltk3::arc(mrb_as_float(mrb, a[0]), mrb_as_float(mrb, a[1]), mrb_as_float(mrb, a[2]), mrb_as_float(mrb, a[3]), mrb_as_float(mrb, a[4]));
  } else {
    fltk3::arc((int) mrb_as_int(mrb, a[0]), (int) mrb_as_int(mrb, a[1]), (int) mrb_as_int(mrb, a[2]), (int) mrb_as_int(mrb, a[3]),
               mrb_as_float(mrb, a[4]), mrb_as_float(mrb, a[5]));
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_pie(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y, w, h;
  mrb_float a1, a2;
  mrb_get_args(mrb, "iiiiff", &x, &y, &w, &h, &a1, &a2);
  fltk3::pie(x, y, w, h, a1, a2);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_circle(mrb_state* mrb, mrb_value self)
{
  mrb_float x, y, r;
  mrb_get_args(mrb, "fff", &x, &y, &r);
  fltk3::circle(x, y, r);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_vertex(mrb_state* mrb, mrb_value self)
{
  mrb_float x, y;
  mrb_get_args(mrb, "ff", &x, &y);
  fltk3::vertex(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_transformed_vertex(mrb_state* mrb, mrb_value self)
{
  mrb_float x, y;
  mrb_get_args(mrb, "ff", &x, &y);
  fltk3::transformed_vertex(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_curve(mrb_state* mrb, mrb_value self)
{
  mrb_float a[8];
  mrb_get_args(mrb, "ffffffff", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5], &a[6], &a[7]);
  fltk3::curve(a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_scale(mrb_state* mrb, mrb_value self)
{
  mrb_float x, y;
  if (mrb_get_args(mrb, "f|f", &x, &y) == 1) fltk3::scale(x); else fltk3::scale(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_translate(mrb_state* mrb, mrb_value self)
{
  mrb_float x, y;
  mrb_get_args(mrb, "ff", &x, &y);
  fltk3::translate(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_rotate(mrb_state* mrb, mrb_value self)
{
  mrb_float d;
  mrb_get_args(mrb, "f", &d);
  fltk3::rotate(d);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_mult_matrix(mrb_state* mrb, mrb_value self)
{
  mrb_float a[6];
  mrb_get_args(mrb, "ffffff", &a[0], &a[1], &a[2], &a[3], &a[4], &a[5]);
  fltk3::mult_matrix(a[0], a[1], a[2], a[3], a[4], a[5]);
  return mrb_nil_value();
}

#define DEFINE_TRANSFORM_FUNC(name) \
static mrb_value \
mrb_fltk3_draw_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_float x, y; \
  mrb_get_args(mrb, "ff", &x, &y); \
  return mrb_float_value(mrb, fltk3::name(x, y)); \
}

DEFINE_TRANSFORM_FUNC(transform_x)
DEFINE_TRANSFORM_FUNC(transform_y)
DEFINE_TRANSFORM_FUNC(transform_dx)
DEFINE_TRANSFORM_FUNC(transform_dy)

static mrb_value
mrb_fltk3_draw_push_clip(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y, w, h;
  mrb_get_args(mrb, "iiii", &x, &y, &w, &h);
  fltk3::push_clip(x, y, w, h);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_not_clipped(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y, w, h;
  mrb_get_args(mrb, "iiii", &x, &y, &w, &h);
  return mrb_bool_value(fltk3::not_clipped(x, y, w, h) != 0);
}

static mrb_value
mrb_fltk3_draw_clip_box(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y, w, h;
  mrb_get_args(mrb, "iiii", &x, &y, &w, &h);
  int X = 0, Y = 0, W = 0, H = 0;
  fltk3::clip_box(x, y, w, h, X, Y, W, H);
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(X));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(Y));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(W));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(H));
  return ret;
}

static mrb_value
mrb_fltk3_draw_line_style(mrb_state* mrb, mrb_value self)
{
  mrb_int style, width = 0;
  mrb_value dashes = mrb_nil_value();
  mrb_get_args(mrb, "i|iS!", &style, &width, &dashes);
  fltk3::line_style((int) style, (int) width, mrb_nil_p(dashes) ? NULL : RSTRING_PTR(dashes));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_font(mrb_state* mrb, mrb_value self)
{
  mrb_int face, size;
  if (mrb_get_args(mrb, "|ii", &face, &size) == 0) return mrb_fixnum_value(fltk3::font());
  fltk3::font((fltk3::Font) face, (fltk3::Fontsize) size);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_height(mrb_state* mrb, mrb_value self)
{
  mrb_int face, size;
  if (mrb_get_args(mrb, "|ii", &face, &size) == 0) return mrb_fixnum_value(fltk3::height());
  return mrb_fixnum_value(fltk3::height((int) face, (int) size));
}

static mrb_value
mrb_fltk3_draw_width(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "o", &s);
  if (mrb_integer_p(s)) return mrb_float_value(mrb, fltk3::width((unsigned int) mrb_integer(s)));
  mrb_value str = mrb_ensure_string_type(mrb, s);
  return mrb_float_value(mrb, fltk3::width(RSTRING_PTR(str), (int) RSTRING_LEN(str)));
}

static mrb_value
mrb_fltk3_draw_text_extents(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  int dx = 0, dy = 0, w = 0, h = 0;
  fltk3::text_extents(RSTRING_PTR(s), (int) RSTRING_LEN(s), dx, dy, w, h);
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(dx));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(dy));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(h));
  return ret;
}

/* draw(text, x, y) or draw(angle, text, x, y) or draw(text, x, y, w, h, align [, draw_symbols]) */
static mrb_value
mrb_fltk3_draw_text(mrb_state* mrb, mrb_value self)
{
  mrb_value* argv;
  mrb_int argc;
  mrb_get_args(mrb, "*", &argv, &argc);
  if (argc == 3 && mrb_string_p(argv[0])) {
    fltk3::draw(RSTRING_PTR(argv[0]), (int) RSTRING_LEN(argv[0]), (int) mrb_as_int(mrb, argv[1]), (int) mrb_as_int(mrb, argv[2]));
  } else if (argc == 4 && mrb_string_p(argv[1])) {
    fltk3::draw((int) mrb_as_int(mrb, argv[0]), RSTRING_CSTR(mrb, argv[1]), (int) mrb_as_int(mrb, argv[2]), (int) mrb_as_int(mrb, argv[3]));
  } else if ((argc == 6 || argc == 7) && mrb_string_p(argv[0])) {
    fltk3::draw(RSTRING_CSTR(mrb, argv[0]), (int) mrb_as_int(mrb, argv[1]), (int) mrb_as_int(mrb, argv[2]),
                (int) mrb_as_int(mrb, argv[3]), (int) mrb_as_int(mrb, argv[4]), (fltk3::Align) mrb_as_int(mrb, argv[5]),
                NULL, argc == 7 ? mrb_test(argv[6]) : 1);
  } else {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid arguments");
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_measure(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_int w = 0;
  mrb_bool symbols = TRUE;
  mrb_get_args(mrb, "S|ib", &s, &w, &symbols);
  int W = (int) w, H = 0;
  fltk3::measure(RSTRING_CSTR(mrb, s), W, H, symbols);
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(W));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(H));
  return ret;
}

static mrb_value
mrb_fltk3_draw_frame(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_int x, y, w, h;
  mrb_get_args(mrb, "Siiii", &s, &x, &y, &w, &h);
  fltk3::frame(RSTRING_CSTR(mrb, s), x, y, w, h);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_draw_box(mrb_state* mrb, mrb_value self)
{
  mrb_value box;
  mrb_int x, y, w, h, c;
  mrb_get_args(mrb, "oiiiii", &box, &x, &y, &w, &h, &c);
  fltk3::draw_box(mrb_fltk3_Box_ptr(mrb, box), x, y, w, h, (fltk3::Color) c);
  return mrb_nil_value();
}

/* draw_image(data, x, y, w, h, d = 3, ld = 0) */
static mrb_value
mrb_fltk3_draw_draw_image(mrb_state* mrb, mrb_value self)
{
  mrb_value data;
  mrb_int x, y, w, h, d = 3, ld = 0;
  mrb_get_args(mrb, "Siiii|ii", &data, &x, &y, &w, &h, &d, &ld);
  mrb_int need = (ld ? ld : w * d) * h;
  if (RSTRING_LEN(data) < need) mrb_raise(mrb, E_ARGUMENT_ERROR, "image data is too short");
  if (d < 3) fltk3::draw_image_mono((const uchar*) RSTRING_PTR(data), x, y, w, h, d, ld);
  else fltk3::draw_image((const uchar*) RSTRING_PTR(data), x, y, w, h, d, ld);
  return mrb_nil_value();
}

/* read_image(x, y, w, h, alpha = 0) -> RGB(A) string */
static mrb_value
mrb_fltk3_draw_read_image(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y, w, h, alpha = 0;
  mrb_get_args(mrb, "iiii|i", &x, &y, &w, &h, &alpha);
  mrb_int d = alpha ? 4 : 3;
  mrb_value buf = mrb_str_new(mrb, NULL, w * h * d);
  if (!fltk3::read_image((uchar*) RSTRING_PTR(buf), x, y, w, h, (int) alpha)) return mrb_nil_value();
  return buf;
}

static mrb_value
mrb_fltk3_draw_origin(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y;
  if (mrb_get_args(mrb, "|ii", &x, &y) == 0) {
    mrb_value ret = mrb_ary_new_capa(mrb, 2);
    mrb_ary_push(mrb, ret, mrb_fixnum_value(fltk3::origin_x()));
    mrb_ary_push(mrb, ret, mrb_fixnum_value(fltk3::origin_y()));
    return ret;
  }
  fltk3::origin(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_translate_origin(mrb_state* mrb, mrb_value self)
{
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  fltk3::translate_origin(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_draw_cursor(mrb_state* mrb, mrb_value self)
{
  mrb_int c, fg = fltk3::BLACK, bg = fltk3::WHITE;
  mrb_get_args(mrb, "i|ii", &c, &fg, &bg);
  fltk3::cursor((fltk3::Cursor) c, (fltk3::Color) fg, (fltk3::Color) bg);
  return mrb_nil_value();
}

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

#define DEFINE_VOID_FUNC(name) \
  mrb_define_module_function(mrb, _class_fltk3, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    fltk3::name(); \
    return mrb_nil_value(); \
  }, MRB_ARGS_NONE());

#define DEFINE_INT_FUNC(name) \
  mrb_define_module_function(mrb, _class_fltk3, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    return mrb_fixnum_value(fltk3::name()); \
  }, MRB_ARGS_NONE());

void
mrb_fltk3_draw_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  mrb_define_module_function(mrb, _class_fltk3, "color", mrb_fltk3_draw_color, MRB_ARGS_OPT(3));
  mrb_define_module_function(mrb, _class_fltk3, "rect", mrb_fltk3_draw_rect, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "rectf", mrb_fltk3_draw_rectf, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "point", mrb_fltk3_draw_point, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "line", mrb_fltk3_draw_line, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "loop", mrb_fltk3_draw_loop, MRB_ARGS_REQ(6) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "polygon", mrb_fltk3_draw_polygon, MRB_ARGS_REQ(6) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "xyline", mrb_fltk3_draw_xyline, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "yxline", mrb_fltk3_draw_yxline, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "arc", mrb_fltk3_draw_arc, MRB_ARGS_REQ(5) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "pie", mrb_fltk3_draw_pie, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, _class_fltk3, "circle", mrb_fltk3_draw_circle, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, _class_fltk3, "vertex", mrb_fltk3_draw_vertex, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "transformed_vertex", mrb_fltk3_draw_transformed_vertex, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "curve", mrb_fltk3_draw_curve, MRB_ARGS_REQ(8));
  mrb_define_module_function(mrb, _class_fltk3, "scale", mrb_fltk3_draw_scale, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "translate", mrb_fltk3_draw_translate, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "rotate", mrb_fltk3_draw_rotate, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "mult_matrix", mrb_fltk3_draw_mult_matrix, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, _class_fltk3, "transform_x", mrb_fltk3_draw_transform_x, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "transform_y", mrb_fltk3_draw_transform_y, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "transform_dx", mrb_fltk3_draw_transform_dx, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "transform_dy", mrb_fltk3_draw_transform_dy, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "push_clip", mrb_fltk3_draw_push_clip, MRB_ARGS_REQ(4));
  mrb_define_module_function(mrb, _class_fltk3, "not_clipped", mrb_fltk3_draw_not_clipped, MRB_ARGS_REQ(4));
  mrb_define_module_function(mrb, _class_fltk3, "clip_box", mrb_fltk3_draw_clip_box, MRB_ARGS_REQ(4));
  mrb_define_module_function(mrb, _class_fltk3, "line_style", mrb_fltk3_draw_line_style, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "font", mrb_fltk3_draw_font, MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "height", mrb_fltk3_draw_height, MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "width", mrb_fltk3_draw_width, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "text_extents", mrb_fltk3_draw_text_extents, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "draw", mrb_fltk3_draw_text, MRB_ARGS_ANY());
  mrb_define_module_function(mrb, _class_fltk3, "draw_text", mrb_fltk3_draw_text, MRB_ARGS_ANY());
  mrb_define_module_function(mrb, _class_fltk3, "measure", mrb_fltk3_draw_measure, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "frame", mrb_fltk3_draw_frame, MRB_ARGS_REQ(5));
  mrb_define_module_function(mrb, _class_fltk3, "draw_box", mrb_fltk3_draw_draw_box, MRB_ARGS_REQ(6));
  mrb_define_module_function(mrb, _class_fltk3, "draw_image", mrb_fltk3_draw_draw_image, MRB_ARGS_REQ(5) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "read_image", mrb_fltk3_draw_read_image, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "origin", mrb_fltk3_draw_origin, MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "translate_origin", mrb_fltk3_draw_translate_origin, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "cursor", mrb_fltk3_draw_cursor, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  DEFINE_VOID_FUNC(push_no_clip);
  DEFINE_VOID_FUNC(pop_clip);
  DEFINE_VOID_FUNC(restore_clip);
  DEFINE_VOID_FUNC(push_origin);
  DEFINE_VOID_FUNC(pop_origin);
  DEFINE_VOID_FUNC(push_matrix);
  DEFINE_VOID_FUNC(pop_matrix);
  DEFINE_VOID_FUNC(begin_points);
  DEFINE_VOID_FUNC(begin_line);
  DEFINE_VOID_FUNC(begin_loop);
  DEFINE_VOID_FUNC(begin_polygon);
  DEFINE_VOID_FUNC(begin_complex_polygon);
  DEFINE_VOID_FUNC(end_points);
  DEFINE_VOID_FUNC(end_line);
  DEFINE_VOID_FUNC(end_loop);
  DEFINE_VOID_FUNC(end_polygon);
  DEFINE_VOID_FUNC(end_complex_polygon);
  DEFINE_VOID_FUNC(gap);
  DEFINE_INT_FUNC(size);
  DEFINE_INT_FUNC(descent);
  ARENA_RESTORE;

  DEFINE_INT_CONST(SOLID);
  DEFINE_INT_CONST(DASH);
  DEFINE_INT_CONST(DOT);
  DEFINE_INT_CONST(DASHDOT);
  DEFINE_INT_CONST(DASHDOTDOT);
  DEFINE_INT_CONST(CAP_FLAT);
  DEFINE_INT_CONST(CAP_ROUND);
  DEFINE_INT_CONST(CAP_SQUARE);
  DEFINE_INT_CONST(JOIN_MITER);
  DEFINE_INT_CONST(JOIN_ROUND);
  DEFINE_INT_CONST(JOIN_BEVEL);
  DEFINE_INT_CONST(CURSOR_DEFAULT);
  DEFINE_INT_CONST(CURSOR_ARROW);
  DEFINE_INT_CONST(CURSOR_CROSS);
  DEFINE_INT_CONST(CURSOR_WAIT);
  DEFINE_INT_CONST(CURSOR_INSERT);
  DEFINE_INT_CONST(CURSOR_HAND);
  DEFINE_INT_CONST(CURSOR_HELP);
  DEFINE_INT_CONST(CURSOR_MOVE);
  DEFINE_INT_CONST(CURSOR_NS);
  DEFINE_INT_CONST(CURSOR_WE);
  DEFINE_INT_CONST(CURSOR_NWSE);
  DEFINE_INT_CONST(CURSOR_NESW);
  DEFINE_INT_CONST(CURSOR_NONE);
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
