#include "mrb_fltk3.h"
#include <fltk3/Clock.h>
#include <fltk3/RoundClock.h>
#include <fltk3/Chart.h>
#include <fltk3/Positioner.h>
#include <fltk3/InputChoice.h>
#include <fltk3/ColorChooser.h>
#include <fltk3/HelpView.h>
#include <fltk3/FileChooser.h>

/*********************************************************
 * FLTK3::Clock, Chart, Positioner, InputChoice, ColorChooser,
 * HelpView and FileChooser
 *********************************************************/
DECLARE_WIDGET(ClockOutput)
DECLARE_WIDGET(Clock)
DECLARE_WIDGET(RoundClock)
DECLARE_WIDGET(Chart)
DECLARE_WIDGET(Positioner)
DECLARE_WIDGET(InputChoice)
DECLARE_WIDGET(ColorChooser)
DECLARE_WIDGET(HelpView)

/* value          -> unix time
 * value = time   -> Time or Integer
 * value(h, m, s) */
static mrb_value
mrb_fltk3_clock_value(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value a;
  mrb_int m, s;
  mrb_int argc = mrb_get_args(mrb, "|oii", &a, &m, &s);
  fltk3::ClockOutput* c = (fltk3::ClockOutput*) context->v;
  if (argc == 0) return mrb_fixnum_value((mrb_int) c->value());
  if (argc == 3) {
    c->value((int) mrb_as_int(mrb, a), (int) m, (int) s);
  } else {
    if (!mrb_integer_p(a)) a = mrb_funcall(mrb, a, "to_i", 0);
    c->value((ulong) mrb_integer(a));
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_chart_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float val;
  mrb_value str = mrb_nil_value();
  mrb_int col = 0;
  mrb_get_args(mrb, "f|S!i", &val, &str, &col);
  ((fltk3::Chart*) context->v)->add(val, mrb_nil_p(str) ? NULL : RSTRING_CSTR(mrb, str), (unsigned) col);
  return self;
}

static mrb_value
mrb_fltk3_chart_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int ind, col = 0;
  mrb_float val;
  mrb_value str = mrb_nil_value();
  mrb_get_args(mrb, "if|S!i", &ind, &val, &str, &col);
  ((fltk3::Chart*) context->v)->insert((int) ind, val, mrb_nil_p(str) ? NULL : RSTRING_CSTR(mrb, str), (unsigned) col);
  return self;
}

static mrb_value
mrb_fltk3_chart_replace(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int ind, col = 0;
  mrb_float val;
  mrb_value str = mrb_nil_value();
  mrb_get_args(mrb, "if|S!i", &ind, &val, &str, &col);
  ((fltk3::Chart*) context->v)->replace((int) ind, val, mrb_nil_p(str) ? NULL : RSTRING_CSTR(mrb, str), (unsigned) col);
  return self;
}

static mrb_value
mrb_fltk3_chart_bounds(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float a, b;
  if (mrb_get_args(mrb, "|ff", &a, &b) == 0) {
    double lo = 0, hi = 0;
    ((fltk3::Chart*) context->v)->bounds(&lo, &hi);
    mrb_value ret = mrb_ary_new_capa(mrb, 2);
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, lo));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, hi));
    return ret;
  }
  ((fltk3::Chart*) context->v)->bounds(a, b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_positioner_value(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float x, y;
  fltk3::Positioner* p = (fltk3::Positioner*) context->v;
  if (mrb_get_args(mrb, "|ff", &x, &y) == 0) {
    mrb_value ret = mrb_ary_new_capa(mrb, 2);
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, p->xvalue()));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, p->yvalue()));
    return ret;
  }
  return mrb_bool_value(p->value(x, y) != 0);
}

#define DEFINE_BOUNDS_METHOD(name) \
static mrb_value \
mrb_fltk3_positioner_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  mrb_float a, b; \
  mrb_get_args(mrb, "ff", &a, &b); \
  ((fltk3::Positioner*) context->v)->name(a, b); \
  return mrb_nil_value(); \
}

DEFINE_BOUNDS_METHOD(xbounds)
DEFINE_BOUNDS_METHOD(ybounds)

static mrb_value
mrb_fltk3_inputchoice_value_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  fltk3::InputChoice* c = (fltk3::InputChoice*) context->v;
  if (mrb_integer_p(v)) c->value((int) mrb_integer(v));
  else if (mrb_nil_p(v)) c->value("");
  else c->value(RSTRING_CSTR(mrb, mrb_ensure_string_type(mrb, v)));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_inputchoice_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  ((fltk3::InputChoice*) context->v)->add(RSTRING_CSTR(mrb, s));
  return self;
}

static mrb_value
mrb_fltk3_inputchoice_menubutton(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Widget_wrap(mrb, ((fltk3::InputChoice*) context->v)->menubutton(), "MenuButton");
}

static mrb_value
mrb_fltk3_inputchoice_input(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Widget_wrap(mrb, ((fltk3::InputChoice*) context->v)->input(), "Input");
}

static mrb_value
mrb_fltk3_colorchooser_rgb(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float r, g, b;
  fltk3::ColorChooser* c = (fltk3::ColorChooser*) context->v;
  if (mrb_get_args(mrb, "|fff", &r, &g, &b) == 0) {
    mrb_value ret = mrb_ary_new_capa(mrb, 3);
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, c->r()));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, c->g()));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, c->b()));
    return ret;
  }
  return mrb_bool_value(c->rgb(r, g, b) != 0);
}

static mrb_value
mrb_fltk3_colorchooser_hsv(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float h, s, v;
  fltk3::ColorChooser* c = (fltk3::ColorChooser*) context->v;
  if (mrb_get_args(mrb, "|fff", &h, &s, &v) == 0) {
    mrb_value ret = mrb_ary_new_capa(mrb, 3);
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, c->hue()));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, c->saturation()));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, c->value()));
    return ret;
  }
  return mrb_bool_value(c->hsv(h, s, v) != 0);
}

static mrb_value
mrb_fltk3_helpview_load(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value f;
  mrb_get_args(mrb, "S", &f);
  return mrb_fixnum_value(((fltk3::HelpView*) context->v)->load(RSTRING_CSTR(mrb, f)));
}

static mrb_value
mrb_fltk3_helpview_find(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_int p = 0;
  mrb_get_args(mrb, "S|i", &s, &p);
  return mrb_fixnum_value(((fltk3::HelpView*) context->v)->find(RSTRING_CSTR(mrb, s), (int) p));
}

static mrb_value
mrb_fltk3_helpview_topline_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  if (mrb_integer_p(v)) ((fltk3::HelpView*) context->v)->topline((int) mrb_integer(v));
  else ((fltk3::HelpView*) context->v)->topline(RSTRING_CSTR(mrb, mrb_ensure_string_type(mrb, v)));
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::FileChooser (a dialog object, not a widget)
 *********************************************************/
static mrb_value
mrb_fltk3_FileChooser_init(mrb_state* mrb, mrb_value self)
{
  mrb_value dir = mrb_nil_value(), pattern = mrb_nil_value(), title = mrb_nil_value();
  mrb_int type = fltk3::FileChooser::SINGLE;
  mrb_get_args(mrb, "|S!S!iS!", &dir, &pattern, &type, &title);
  fltk3::FileChooser* fc = new fltk3::FileChooser(
    mrb_nil_p(dir) ? NULL : RSTRING_CSTR(mrb, dir),
    mrb_nil_p(pattern) ? NULL : RSTRING_CSTR(mrb, pattern),
    (int) type,
    mrb_nil_p(title) ? NULL : RSTRING_CSTR(mrb, title));
  return mrb_fltk3_FileChooser_setup(mrb, self, fc);
}

static void
_mrb_fltk3_filechooser_callback(fltk3::FileChooser* fc, void* d)
{
  mrb_fltk3_FileChooser_context* context = (mrb_fltk3_FileChooser_context*) d;
  mrb_state* mrb = context->mrb;
  mrb_value proc = mrb_iv_get(mrb, context->instance, mrb_intern_lit(mrb, "callback"));
  mrb_fltk3_call(mrb, proc, 1, &context->instance);
}

static mrb_value
mrb_fltk3_filechooser_callback(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(FileChooser);
  mrb_value b;
  mrb_get_args(mrb, "&!", &b);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "callback"), b);
  context->v->callback(_mrb_fltk3_filechooser_callback, context);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_filechooser_value(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(FileChooser);
  mrb_int f = 1;
  mrb_get_args(mrb, "|i", &f);
  const char* v = context->v->value((int) f);
  return v ? mrb_str_new_cstr(mrb, v) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_filechooser_values(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(FileChooser);
  mrb_value ary = mrb_ary_new(mrb);
  for (int i = 1; i <= context->v->count(); i++) {
    const char* v = context->v->value(i);
    if (v) mrb_ary_push(mrb, ary, mrb_str_new_cstr(mrb, v));
  }
  return ary;
}

#define DEFINE_FC_STR_PROP(z) \
  mrb_define_method(mrb, _class_fltk3_FileChooser, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(FileChooser); \
    const char* v = context->v->z(); \
    return v ? mrb_str_new_cstr(mrb, v) : mrb_nil_value(); \
  }, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_FileChooser, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(FileChooser); \
    mrb_value s; \
    mrb_get_args(mrb, "S!", &s); \
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "str_" # z), s); \
    context->v->z(mrb_nil_p(s) ? NULL : RSTRING_CSTR(mrb, s)); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_FC_INT_PROP(z) \
  mrb_define_method(mrb, _class_fltk3_FileChooser, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(FileChooser); \
    return mrb_fixnum_value(context->v->z()); \
  }, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_FileChooser, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(FileChooser); \
    mrb_int i; \
    mrb_get_args(mrb, "i", &i); \
    context->v->z(i); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_FC_VOID(z) \
  mrb_define_method(mrb, _class_fltk3_FileChooser, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(FileChooser); \
    context->v->z(); \
    return mrb_nil_value(); \
  }, MRB_ARGS_NONE());

#define DEFINE_FC_BOOL(z) \
  mrb_define_method(mrb, _class_fltk3_FileChooser, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(FileChooser); \
    return mrb_bool_value(context->v->z() != 0); \
  }, MRB_ARGS_NONE());

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

void
mrb_fltk3_misc_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  DEFINE_INT_CONST(SQUARE_CLOCK);
  DEFINE_INT_CONST(ROUND_CLOCK);
  DEFINE_INT_CONST(ANALOG_CLOCK);
  DEFINE_INT_CONST(BAR_CHART);
  DEFINE_INT_CONST(HORBAR_CHART);
  DEFINE_INT_CONST(LINE_CHART);
  DEFINE_INT_CONST(FILL_CHART);
  DEFINE_INT_CONST(SPIKE_CHART);
  DEFINE_INT_CONST(PIE_CHART);
  DEFINE_INT_CONST(SPECIALPIE_CHART);

  DEFINE_CLASS(ClockOutput, Widget);
  DEFINE_INT_GETTER(ClockOutput, Widget, hour);
  DEFINE_INT_GETTER(ClockOutput, Widget, minute);
  DEFINE_INT_GETTER(ClockOutput, Widget, second);
  mrb_define_method(mrb, _class_fltk3_ClockOutput, "value", mrb_fltk3_clock_value, MRB_ARGS_OPT(3));
  mrb_define_method(mrb, _class_fltk3_ClockOutput, "value=", mrb_fltk3_clock_value, MRB_ARGS_REQ(1));
  ARENA_RESTORE;
  DEFINE_CLASS(Clock, ClockOutput);
  DEFINE_CLASS(RoundClock, Clock);

  DEFINE_CLASS(Chart, Widget);
  INHERIT_TEXT_PROPS(Chart);
  DEFINE_INT_GETTER(Chart, Widget, size);
  DEFINE_INT_PROP(Chart, Widget, maxsize);
  DEFINE_BOOL_PROP(Chart, Widget, autosize);
  DEFINE_VOID_METHOD(Chart, Widget, clear);
  mrb_define_method(mrb, _class_fltk3_Chart, "add", mrb_fltk3_chart_add, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Chart, "insert", mrb_fltk3_chart_insert, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Chart, "replace", mrb_fltk3_chart_replace, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Chart, "bounds", mrb_fltk3_chart_bounds, MRB_ARGS_OPT(2));
  ARENA_RESTORE;

  DEFINE_CLASS(Positioner, Widget);
  DEFINE_FLOAT_PROP(Positioner, Widget, xvalue);
  DEFINE_FLOAT_PROP(Positioner, Widget, yvalue);
  DEFINE_FLOAT_PROP(Positioner, Widget, xminimum);
  DEFINE_FLOAT_PROP(Positioner, Widget, xmaximum);
  DEFINE_FLOAT_PROP(Positioner, Widget, yminimum);
  DEFINE_FLOAT_PROP(Positioner, Widget, ymaximum);
  DEFINE_FLOAT_SETTER(Positioner, Widget, xstep);
  DEFINE_FLOAT_SETTER(Positioner, Widget, ystep);
  mrb_define_method(mrb, _class_fltk3_Positioner, "value", mrb_fltk3_positioner_value, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Positioner, "xbounds", mrb_fltk3_positioner_xbounds, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Positioner, "ybounds", mrb_fltk3_positioner_ybounds, MRB_ARGS_REQ(2));
  ARENA_RESTORE;

  DEFINE_CLASS(InputChoice, Group);
  INHERIT_TEXT_PROPS(InputChoice);
  DEFINE_STR_GETTER(InputChoice, Widget, value);
  DEFINE_VOID_METHOD(InputChoice, Widget, clear);
  mrb_define_method(mrb, _class_fltk3_InputChoice, "value=", mrb_fltk3_inputchoice_value_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_InputChoice, "add", mrb_fltk3_inputchoice_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_InputChoice, "<<", mrb_fltk3_inputchoice_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_InputChoice, "menubutton", mrb_fltk3_inputchoice_menubutton, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_InputChoice, "input", mrb_fltk3_inputchoice_input, MRB_ARGS_NONE());
  ARENA_RESTORE;

  DEFINE_CLASS(ColorChooser, Group);
  DEFINE_INT_PROP(ColorChooser, Widget, mode);
  DEFINE_FLOAT_GETTER(ColorChooser, Widget, r);
  DEFINE_FLOAT_GETTER(ColorChooser, Widget, g);
  DEFINE_FLOAT_GETTER(ColorChooser, Widget, b);
  DEFINE_FLOAT_GETTER(ColorChooser, Widget, hue);
  DEFINE_FLOAT_GETTER(ColorChooser, Widget, saturation);
  DEFINE_FLOAT_GETTER(ColorChooser, Widget, value);
  mrb_define_method(mrb, _class_fltk3_ColorChooser, "rgb", mrb_fltk3_colorchooser_rgb, MRB_ARGS_OPT(3));
  mrb_define_method(mrb, _class_fltk3_ColorChooser, "hsv", mrb_fltk3_colorchooser_hsv, MRB_ARGS_OPT(3));
  ARENA_RESTORE;

  DEFINE_CLASS(HelpView, Group);
  INHERIT_TEXT_PROPS(HelpView);
  DEFINE_STR_PROP(HelpView, Widget, value);
  DEFINE_STR_GETTER(HelpView, Widget, title);
  DEFINE_STR_GETTER(HelpView, Widget, directory);
  DEFINE_STR_GETTER(HelpView, Widget, filename);
  DEFINE_INT_GETTER(HelpView, Widget, size);
  DEFINE_INT_GETTER(HelpView, Widget, topline);
  DEFINE_INT_PROP(HelpView, Widget, leftline);
  DEFINE_INT_PROP(HelpView, Widget, scrollbar_size);
  DEFINE_VOID_METHOD(HelpView, Widget, clear_selection);
  DEFINE_VOID_METHOD(HelpView, Widget, select_all);
  mrb_define_method(mrb, _class_fltk3_HelpView, "topline=", mrb_fltk3_helpview_topline_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_HelpView, "load", mrb_fltk3_helpview_load, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_HelpView, "find", mrb_fltk3_helpview_find, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  ARENA_RESTORE;

  struct RClass* _class_fltk3_FileChooser = mrb_define_class_under(mrb, _class_fltk3, "FileChooser", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_FileChooser, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_FileChooser, "initialize", mrb_fltk3_FileChooser_init, MRB_ARGS_OPT(4));
  mrb_define_const(mrb, _class_fltk3_FileChooser, "SINGLE", mrb_fixnum_value(fltk3::FileChooser::SINGLE));
  mrb_define_const(mrb, _class_fltk3_FileChooser, "MULTI", mrb_fixnum_value(fltk3::FileChooser::MULTI));
  mrb_define_const(mrb, _class_fltk3_FileChooser, "CREATE", mrb_fixnum_value(fltk3::FileChooser::CREATE));
  mrb_define_const(mrb, _class_fltk3_FileChooser, "DIRECTORY", mrb_fixnum_value(fltk3::FileChooser::DIRECTORY));
  mrb_define_method(mrb, _class_fltk3_FileChooser, "callback", mrb_fltk3_filechooser_callback, MRB_ARGS_BLOCK());
  mrb_define_method(mrb, _class_fltk3_FileChooser, "value", mrb_fltk3_filechooser_value, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_FileChooser, "values", mrb_fltk3_filechooser_values, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_FileChooser, "value=", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(FileChooser);
    mrb_value s;
    mrb_get_args(mrb, "S", &s);
    context->v->value(RSTRING_CSTR(mrb, s));
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));
  DEFINE_FC_STR_PROP(directory);
  DEFINE_FC_STR_PROP(filter);
  DEFINE_FC_STR_PROP(label);
  DEFINE_FC_STR_PROP(ok_label);
  DEFINE_FC_INT_PROP(type);
  DEFINE_FC_INT_PROP(filter_value);
  DEFINE_FC_INT_PROP(color);
  DEFINE_FC_INT_PROP(textcolor);
  DEFINE_FC_INT_PROP(textfont);
  DEFINE_FC_INT_PROP(textsize);
  DEFINE_FC_INT_PROP(iconsize);
  DEFINE_FC_INT_PROP(preview);
  mrb_define_method(mrb, _class_fltk3_FileChooser, "count", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(FileChooser);
    return mrb_fixnum_value(context->v->count());
  }, MRB_ARGS_NONE());
  DEFINE_FC_VOID(show);
  DEFINE_FC_VOID(hide);
  DEFINE_FC_VOID(rescan);
  DEFINE_FC_VOID(rescan_keep_filename);
  DEFINE_FC_BOOL(shown);
  DEFINE_FC_BOOL(visible);
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
