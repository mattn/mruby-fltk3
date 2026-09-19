#include "mrb_fltk3.h"
#include <stdarg.h>
#include <stdio.h>
#include <fltk3/run.h>
#include <fltk3/ask.h>
#include <fltk3/Window.h>
#include <fltk3/FileChooser.h>
#include <fltk3/ColorChooser.h>
#include <fltk3/Tooltip.h>
#include <fltk3/enumerations.h>

/*********************************************************
 * FLTK3 module functions: event loop, events, colors, dialogs
 *********************************************************/
static mrb_value
mrb_fltk3_wrap_widget(mrb_state* mrb, fltk3::Widget* w)
{
  return mrb_fltk3_Widget_wrap(mrb, w, mrb_fltk3_widget_classname(w));
}

/* timeouts, checks and idle handlers ---------------------------------- */

typedef struct {
  mrb_state* mrb;
  mrb_int id;
} mrb_fltk3_handler_data;

static mrb_value
mrb_fltk3_handlers(mrb_state* mrb)
{
  struct RClass* _class_fltk3 = mrb_module_get(mrb, "FLTK3");
  mrb_value h = mrb_iv_get(mrb, mrb_obj_value(_class_fltk3), mrb_intern_lit(mrb, "__handlers__"));
  if (mrb_nil_p(h)) {
    h = mrb_hash_new(mrb);
    mrb_iv_set(mrb, mrb_obj_value(_class_fltk3), mrb_intern_lit(mrb, "__handlers__"), h);
  }
  return h;
}

/* Registers a block and returns [id, data pointer]. */
static mrb_int
mrb_fltk3_handler_register(mrb_state* mrb, mrb_value proc, void** datap)
{
  static mrb_int next_id = 1;
  mrb_int id = next_id++;
  mrb_value buf = mrb_str_new(mrb, NULL, sizeof(mrb_fltk3_handler_data));
  mrb_fltk3_handler_data* data = (mrb_fltk3_handler_data*) RSTRING_PTR(buf);
  data->mrb = mrb;
  data->id = id;
  mrb_value entry = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, entry, proc);
  mrb_ary_push(mrb, entry, buf);
  mrb_hash_set(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id), entry);
  *datap = data;
  return id;
}

static void*
mrb_fltk3_handler_data_of(mrb_state* mrb, mrb_int id)
{
  mrb_value entry = mrb_hash_get(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id));
  if (!mrb_array_p(entry)) return NULL;
  return RSTRING_PTR(mrb_ary_ref(mrb, entry, 1));
}

static void
mrb_fltk3_handler_call(mrb_fltk3_handler_data* data, bool once)
{
  mrb_state* mrb = data->mrb;
  mrb_int id = data->id;
  mrb_value entry = mrb_hash_get(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id));
  if (!mrb_array_p(entry)) return;
  mrb_value proc = mrb_ary_ref(mrb, entry, 0);
  if (once) mrb_hash_delete_key(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id));
  mrb_value arg = mrb_fixnum_value(id);
  mrb_fltk3_call(mrb, proc, 1, &arg);
}

static void
_mrb_fltk3_timeout_handler(void* d)
{
  mrb_fltk3_handler_call((mrb_fltk3_handler_data*) d, true);
}

static void
_mrb_fltk3_idle_handler(void* d)
{
  mrb_fltk3_handler_call((mrb_fltk3_handler_data*) d, false);
}

/* add_timeout(seconds) { |id| ... } -> id */
static mrb_value
mrb_fltk3_add_timeout(mrb_state* mrb, mrb_value self)
{
  mrb_float t;
  mrb_value b;
  mrb_get_args(mrb, "f&!", &t, &b);
  void* data;
  mrb_int id = mrb_fltk3_handler_register(mrb, b, &data);
  fltk3::add_timeout(t, _mrb_fltk3_timeout_handler, data);
  return mrb_fixnum_value(id);
}

static mrb_value
mrb_fltk3_repeat_timeout(mrb_state* mrb, mrb_value self)
{
  mrb_float t;
  mrb_value b;
  mrb_get_args(mrb, "f&!", &t, &b);
  void* data;
  mrb_int id = mrb_fltk3_handler_register(mrb, b, &data);
  fltk3::repeat_timeout(t, _mrb_fltk3_timeout_handler, data);
  return mrb_fixnum_value(id);
}

static mrb_value
mrb_fltk3_has_timeout(mrb_state* mrb, mrb_value self)
{
  mrb_int id;
  mrb_get_args(mrb, "i", &id);
  void* data = mrb_fltk3_handler_data_of(mrb, id);
  return mrb_bool_value(data && fltk3::has_timeout(_mrb_fltk3_timeout_handler, data));
}

static mrb_value
mrb_fltk3_remove_timeout(mrb_state* mrb, mrb_value self)
{
  mrb_int id;
  mrb_get_args(mrb, "i", &id);
  void* data = mrb_fltk3_handler_data_of(mrb, id);
  if (data) {
    fltk3::remove_timeout(_mrb_fltk3_timeout_handler, data);
    mrb_hash_delete_key(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id));
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_add_check(mrb_state* mrb, mrb_value self)
{
  mrb_value b;
  mrb_get_args(mrb, "&!", &b);
  void* data;
  mrb_int id = mrb_fltk3_handler_register(mrb, b, &data);
  fltk3::add_check(_mrb_fltk3_idle_handler, data);
  return mrb_fixnum_value(id);
}

static mrb_value
mrb_fltk3_remove_check(mrb_state* mrb, mrb_value self)
{
  mrb_int id;
  mrb_get_args(mrb, "i", &id);
  void* data = mrb_fltk3_handler_data_of(mrb, id);
  if (data) {
    fltk3::remove_check(_mrb_fltk3_idle_handler, data);
    mrb_hash_delete_key(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id));
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_add_idle(mrb_state* mrb, mrb_value self)
{
  mrb_value b;
  mrb_get_args(mrb, "&!", &b);
  void* data;
  mrb_int id = mrb_fltk3_handler_register(mrb, b, &data);
  fltk3::add_idle(_mrb_fltk3_idle_handler, data);
  return mrb_fixnum_value(id);
}

static mrb_value
mrb_fltk3_has_idle(mrb_state* mrb, mrb_value self)
{
  mrb_int id;
  mrb_get_args(mrb, "i", &id);
  void* data = mrb_fltk3_handler_data_of(mrb, id);
  return mrb_bool_value(data && fltk3::has_idle(_mrb_fltk3_idle_handler, data));
}

static mrb_value
mrb_fltk3_remove_idle(mrb_state* mrb, mrb_value self)
{
  mrb_int id;
  mrb_get_args(mrb, "i", &id);
  void* data = mrb_fltk3_handler_data_of(mrb, id);
  if (data) {
    fltk3::remove_idle(_mrb_fltk3_idle_handler, data);
    mrb_hash_delete_key(mrb, mrb_fltk3_handlers(mrb), mrb_fixnum_value(id));
  }
  return mrb_nil_value();
}

/* warning / error / fatal ---------------------------------------------- */

static mrb_state* mrb_fltk3_message_mrb = NULL;

static void
mrb_fltk3_message_dispatch(const char* name, void (*fallback)(const char*, ...), const char* format, va_list ap)
{
  char buf[1024];
  vsnprintf(buf, sizeof(buf), format, ap);
  mrb_state* mrb = mrb_fltk3_message_mrb;
  mrb_value proc = mrb_nil_value();
  if (mrb) {
    struct RClass* _class_fltk3 = mrb_module_get(mrb, "FLTK3");
    proc = mrb_iv_get(mrb, mrb_obj_value(_class_fltk3), mrb_intern_cstr(mrb, name));
  }
  if (mrb_nil_p(proc)) {
    fallback("%s", buf);
    return;
  }
  mrb_value arg = mrb_str_new_cstr(mrb, buf);
  mrb_fltk3_call(mrb, proc, 1, &arg);
}

static void default_warning(const char* format, ...) { va_list ap; va_start(ap, format); vfprintf(stderr, format, ap); va_end(ap); fputc('\n', stderr); }
static void default_error(const char* format, ...) { va_list ap; va_start(ap, format); vfprintf(stderr, format, ap); va_end(ap); fputc('\n', stderr); }
static void default_fatal(const char* format, ...) { va_list ap; va_start(ap, format); vfprintf(stderr, format, ap); va_end(ap); fputc('\n', stderr); exit(1); }

static void _mrb_fltk3_warning(const char* format, ...) { va_list ap; va_start(ap, format); mrb_fltk3_message_dispatch("__warning__", default_warning, format, ap); va_end(ap); }
static void _mrb_fltk3_error(const char* format, ...) { va_list ap; va_start(ap, format); mrb_fltk3_message_dispatch("__error__", default_error, format, ap); va_end(ap); }
static void _mrb_fltk3_fatal(const char* format, ...) { va_list ap; va_start(ap, format); mrb_fltk3_message_dispatch("__fatal__", default_fatal, format, ap); va_end(ap); exit(1); }

/* warning(message)        -> report through the current handler
 * warning { |message| }   -> install a handler (nil block restores default) */
#define DEFINE_MESSAGE_FUNC(name, ivname) \
static mrb_value \
mrb_fltk3_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_value b = mrb_nil_value(), msg = mrb_nil_value(); \
  mrb_get_args(mrb, "&|S", &b, &msg); \
  if (!mrb_nil_p(msg)) { \
    fltk3::name("%s", RSTRING_CSTR(mrb, msg)); \
    return mrb_nil_value(); \
  } \
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, ivname), b); \
  mrb_fltk3_message_mrb = mrb; \
  fltk3::name = _mrb_fltk3_ ## name; \
  return b; \
}

DEFINE_MESSAGE_FUNC(warning, "__warning__")
DEFINE_MESSAGE_FUNC(error, "__error__")
DEFINE_MESSAGE_FUNC(fatal, "__fatal__")

/* event loop ------------------------------------------------------------ */

static mrb_value
mrb_fltk3_wait(mrb_state* mrb, mrb_value self)
{
  mrb_float t;
  mrb_value ret;
  if (mrb_get_args(mrb, "|f", &t) == 0) ret = mrb_fixnum_value(fltk3::wait());
  else ret = mrb_float_value(mrb, fltk3::wait(t));
  mrb_fltk3_check_exception(mrb);
  return ret;
}

static mrb_value
mrb_fltk3_first_window(mrb_state* mrb, mrb_value self)
{
  return mrb_fltk3_wrap_widget(mrb, fltk3::first_window());
}

static mrb_value
mrb_fltk3_first_window_set(mrb_state* mrb, mrb_value self)
{
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  fltk3::first_window((fltk3::Window*) mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_next_window(mrb_state* mrb, mrb_value self)
{
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  return mrb_fltk3_wrap_widget(mrb, fltk3::next_window((fltk3::Window*) mrb_fltk3_Widget_ptr(mrb, w)));
}

static mrb_value
mrb_fltk3_windows(mrb_state* mrb, mrb_value self)
{
  mrb_value ary = mrb_ary_new(mrb);
  for (fltk3::Window* w = fltk3::first_window(); w; w = fltk3::next_window(w)) {
    mrb_ary_push(mrb, ary, mrb_fltk3_wrap_widget(mrb, w));
  }
  return ary;
}

static mrb_value
mrb_fltk3_grab(mrb_state* mrb, mrb_value self)
{
  mrb_value w;
  if (mrb_get_args(mrb, "|o", &w) == 0) return mrb_fltk3_wrap_widget(mrb, fltk3::grab());
  fltk3::grab((fltk3::Window*) mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_delete_widget(mrb_state* mrb, mrb_value self)
{
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  fltk3::Widget* widget = mrb_fltk3_Widget_ptr(mrb, w);
  mrb_fltk3_widget_forget(mrb, widget);
  fltk3::delete_widget(widget);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_handle(mrb_state* mrb, mrb_value self)
{
  mrb_int e;
  mrb_value w;
  mrb_get_args(mrb, "io", &e, &w);
  return mrb_bool_value(fltk3::handle((int) e, (fltk3::Window*) mrb_fltk3_Widget_ptr(mrb, w)) != 0);
}

/* events ---------------------------------------------------------------- */

static mrb_value
mrb_fltk3_event_key(mrb_state* mrb, mrb_value self)
{
  mrb_int key;
  if (mrb_get_args(mrb, "|i", &key) == 0) return mrb_fixnum_value(fltk3::event_key());
  return mrb_bool_value(fltk3::event_key((unsigned int) key) != 0);
}

static mrb_value
mrb_fltk3_get_key(mrb_state* mrb, mrb_value self)
{
  mrb_int key;
  mrb_get_args(mrb, "i", &key);
  return mrb_bool_value(fltk3::get_key((unsigned int) key) != 0);
}

static mrb_value
mrb_fltk3_event_state(mrb_state* mrb, mrb_value self)
{
  mrb_int mask;
  if (mrb_get_args(mrb, "|i", &mask) == 0) return mrb_fixnum_value(fltk3::event_state());
  return mrb_bool_value(fltk3::event_state((unsigned) mask) != 0);
}

static mrb_value
mrb_fltk3_event_text(mrb_state* mrb, mrb_value self)
{
  const char* t = fltk3::event_text();
  return t ? mrb_str_new(mrb, t, fltk3::event_length()) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_event_inside(mrb_state* mrb, mrb_value self)
{
  mrb_value* argv;
  mrb_int argc;
  mrb_get_args(mrb, "*", &argv, &argc);
  if (argc == 1) {
    return mrb_bool_value(fltk3::event_inside(mrb_fltk3_Widget_ptr(mrb, argv[0])) != 0);
  }
  if (argc != 4) mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  return mrb_bool_value(fltk3::event_inside((int) mrb_integer(argv[0]), (int) mrb_integer(argv[1]),
                                            (int) mrb_integer(argv[2]), (int) mrb_integer(argv[3])) != 0);
}

static mrb_value
mrb_fltk3_get_mouse(mrb_state* mrb, mrb_value self)
{
  int x = 0, y = 0;
  fltk3::get_mouse(x, y);
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(x));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(y));
  return ret;
}

static mrb_value
mrb_fltk3_test_shortcut(mrb_state* mrb, mrb_value self)
{
  mrb_int s;
  mrb_get_args(mrb, "i", &s);
  return mrb_bool_value(fltk3::test_shortcut((fltk3::Shortcut) s) != 0);
}

#define DEFINE_WIDGET_ACCESSOR(name, type) \
static mrb_value \
mrb_fltk3_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  return mrb_fltk3_wrap_widget(mrb, fltk3::name()); \
} \
static mrb_value \
mrb_fltk3_ ## name ## _set(mrb_state* mrb, mrb_value self) \
{ \
  mrb_value w; \
  mrb_get_args(mrb, "o", &w); \
  fltk3::name((type*) mrb_fltk3_Widget_ptr(mrb, w)); \
  return mrb_nil_value(); \
}

DEFINE_WIDGET_ACCESSOR(focus, fltk3::Widget)
DEFINE_WIDGET_ACCESSOR(belowmouse, fltk3::Widget)
DEFINE_WIDGET_ACCESSOR(pushed, fltk3::Widget)

static mrb_value
mrb_fltk3_modal(mrb_state* mrb, mrb_value self)
{
  return mrb_fltk3_wrap_widget(mrb, fltk3::modal());
}

static mrb_value
mrb_fltk3_copy(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_int dest = 0;
  mrb_get_args(mrb, "S|i", &s, &dest);
  fltk3::copy(RSTRING_PTR(s), (int) RSTRING_LEN(s), (int) dest);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_paste(mrb_state* mrb, mrb_value self)
{
  mrb_value w;
  mrb_int source = 0;
  mrb_get_args(mrb, "o|i", &w, &source);
  fltk3::paste(*mrb_fltk3_Widget_ptr(mrb, w), (int) source);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_selection(mrb_state* mrb, mrb_value self)
{
  mrb_value w, s;
  mrb_get_args(mrb, "oS", &w, &s);
  fltk3::selection(*mrb_fltk3_Widget_ptr(mrb, w), RSTRING_PTR(s), (int) RSTRING_LEN(s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_option(mrb_state* mrb, mrb_value self)
{
  mrb_int opt;
  mrb_bool val;
  if (mrb_get_args(mrb, "i|b", &opt, &val) == 1) return mrb_bool_value(fltk3::option((fltk3::Option) opt));
  fltk3::option((fltk3::Option) opt, val);
  return mrb_nil_value();
}

/* screen and colors ----------------------------------------------------- */

static mrb_value
mrb_fltk3_screen_xywh(mrb_state* mrb, mrb_value self)
{
  mrb_value* argv;
  mrb_int argc;
  mrb_get_args(mrb, "*", &argv, &argc);
  int x = 0, y = 0, w = 0, h = 0;
  if (argc == 0) fltk3::screen_xywh(x, y, w, h);
  else if (argc == 1) fltk3::screen_xywh(x, y, w, h, (int) mrb_integer(argv[0]));
  else if (argc == 2) fltk3::screen_xywh(x, y, w, h, (int) mrb_integer(argv[0]), (int) mrb_integer(argv[1]));
  else mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(x));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(y));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(h));
  return ret;
}

static mrb_value
mrb_fltk3_screen_work_area(mrb_state* mrb, mrb_value self)
{
  mrb_int n;
  int x = 0, y = 0, w = 0, h = 0;
  if (mrb_get_args(mrb, "|i", &n) == 0) fltk3::screen_work_area(x, y, w, h);
  else fltk3::screen_work_area(x, y, w, h, (int) n);
  mrb_value ret = mrb_ary_new_capa(mrb, 4);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(x));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(y));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(h));
  return ret;
}

static mrb_value
mrb_fltk3_screen_dpi(mrb_state* mrb, mrb_value self)
{
  mrb_int n = 0;
  mrb_get_args(mrb, "|i", &n);
  float h = 0, v = 0;
  fltk3::screen_dpi(h, v, (int) n);
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_float_value(mrb, h));
  mrb_ary_push(mrb, ret, mrb_float_value(mrb, v));
  return ret;
}

static mrb_value
mrb_fltk3_rgb_color(mrb_state* mrb, mrb_value self)
{
  mrb_int r, g, b;
  if (mrb_get_args(mrb, "i|ii", &r, &g, &b) == 1) return mrb_fixnum_value(fltk3::rgb_color((uchar) r));
  return mrb_fixnum_value(fltk3::rgb_color((uchar) r, (uchar) g, (uchar) b));
}

static mrb_value
mrb_fltk3_get_color(mrb_state* mrb, mrb_value self)
{
  mrb_int c;
  mrb_get_args(mrb, "i", &c);
  uchar r = 0, g = 0, b = 0;
  fltk3::get_color((fltk3::Color) c, r, g, b);
  mrb_value ret = mrb_ary_new_capa(mrb, 3);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(r));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(g));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(b));
  return ret;
}

static mrb_value
mrb_fltk3_set_color(mrb_state* mrb, mrb_value self)
{
  mrb_int c, r, g, b;
  if (mrb_get_args(mrb, "ii|ii", &c, &r, &g, &b) == 2) {
    fltk3::set_color((fltk3::Color) c, (unsigned) r);
  } else {
    fltk3::set_color((fltk3::Color) c, (uchar) r, (uchar) g, (uchar) b);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_free_color(mrb_state* mrb, mrb_value self)
{
  mrb_int c;
  mrb_bool overlay = FALSE;
  mrb_get_args(mrb, "i|b", &c, &overlay);
  fltk3::free_color((fltk3::Color) c, overlay);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_color_average(mrb_state* mrb, mrb_value self)
{
  mrb_int c1, c2;
  mrb_float weight;
  mrb_get_args(mrb, "iif", &c1, &c2, &weight);
  return mrb_fixnum_value(fltk3::color_average((fltk3::Color) c1, (fltk3::Color) c2, (float) weight));
}

static mrb_value
mrb_fltk3_contrast(mrb_state* mrb, mrb_value self)
{
  mrb_int fg, bg;
  mrb_get_args(mrb, "ii", &fg, &bg);
  return mrb_fixnum_value(fltk3::contrast((fltk3::Color) fg, (fltk3::Color) bg));
}

#define DEFINE_COLOR_FUNC(name) \
static mrb_value \
mrb_fltk3_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_int c; \
  mrb_get_args(mrb, "i", &c); \
  return mrb_fixnum_value(fltk3::name((fltk3::Color) c)); \
}

DEFINE_COLOR_FUNC(inactive)
DEFINE_COLOR_FUNC(lighter)
DEFINE_COLOR_FUNC(darker)

#define DEFINE_RGB_FUNC(name) \
static mrb_value \
mrb_fltk3_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  mrb_int r, g, b; \
  mrb_get_args(mrb, "iii", &r, &g, &b); \
  fltk3::name((uchar) r, (uchar) g, (uchar) b); \
  return mrb_nil_value(); \
}

DEFINE_RGB_FUNC(foreground)
DEFINE_RGB_FUNC(background)
DEFINE_RGB_FUNC(background2)

static mrb_value
mrb_fltk3_scheme(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  if (mrb_get_args(mrb, "|S!", &s) == 0) {
    const char* name = fltk3::scheme();
    return name ? mrb_str_new_cstr(mrb, name) : mrb_nil_value();
  }
  return mrb_bool_value(fltk3::scheme(mrb_nil_p(s) ? NULL : RSTRING_CSTR(mrb, s)) != 0);
}

static mrb_value
mrb_fltk3_get_font(mrb_state* mrb, mrb_value self)
{
  mrb_int f;
  mrb_get_args(mrb, "i", &f);
  const char* name = fltk3::get_font((fltk3::Font) f);
  return name ? mrb_str_new_cstr(mrb, name) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_get_font_sizes(mrb_state* mrb, mrb_value self)
{
  mrb_int f;
  mrb_get_args(mrb, "i", &f);
  int* sizes = NULL;
  int n = fltk3::get_font_sizes((fltk3::Font) f, sizes);
  mrb_value ary = mrb_ary_new_capa(mrb, n);
  for (int i = 0; i < n; i++) mrb_ary_push(mrb, ary, mrb_fixnum_value(sizes[i]));
  return ary;
}

/* dialogs --------------------------------------------------------------- */

static mrb_value
mrb_fltk3_message(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  fltk3::message("%s", RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_input(mrb_state* mrb, mrb_value self)
{
  mrb_value label, deflt = mrb_nil_value();
  mrb_get_args(mrb, "S|S!", &label, &deflt);
  const char* r = fltk3::input("%s", mrb_nil_p(deflt) ? NULL : RSTRING_CSTR(mrb, deflt), RSTRING_CSTR(mrb, label));
  return r ? mrb_str_new_cstr(mrb, r) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_password(mrb_state* mrb, mrb_value self)
{
  mrb_value label, deflt = mrb_nil_value();
  mrb_get_args(mrb, "S|S!", &label, &deflt);
  const char* r = fltk3::password("%s", mrb_nil_p(deflt) ? NULL : RSTRING_CSTR(mrb, deflt), RSTRING_CSTR(mrb, label));
  return r ? mrb_str_new_cstr(mrb, r) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_beep(mrb_state* mrb, mrb_value self)
{
  mrb_int type = fltk3::BEEP_DEFAULT;
  mrb_get_args(mrb, "|i", &type);
  fltk3::beep((int) type);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_message_font(mrb_state* mrb, mrb_value self)
{
  mrb_int f, s;
  mrb_get_args(mrb, "ii", &f, &s);
  fltk3::message_font((fltk3::Font) f, (fltk3::Fontsize) s);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_message_hotspot(mrb_state* mrb, mrb_value self)
{
  mrb_bool b;
  if (mrb_get_args(mrb, "|b", &b) == 0) return mrb_bool_value(fltk3::message_hotspot() != 0);
  fltk3::message_hotspot(b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_message_title(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S!", &s);
  fltk3::message_title(mrb_nil_p(s) ? NULL : RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_message_title_default(mrb_state* mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S!", &s);
  fltk3::message_title_default(mrb_nil_p(s) ? NULL : RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_file_chooser(mrb_state* mrb, mrb_value self)
{
  mrb_value message, pattern, fname = mrb_nil_value();
  mrb_bool relative = FALSE;
  mrb_get_args(mrb, "SS|S!b", &message, &pattern, &fname, &relative);
  const char* r = fltk3::file_chooser(RSTRING_CSTR(mrb, message), RSTRING_CSTR(mrb, pattern),
    mrb_nil_p(fname) ? NULL : RSTRING_CSTR(mrb, fname), relative);
  return r ? mrb_str_new_cstr(mrb, r) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_dir_chooser(mrb_state* mrb, mrb_value self)
{
  mrb_value message, fname = mrb_nil_value();
  mrb_bool relative = FALSE;
  mrb_get_args(mrb, "S|S!b", &message, &fname, &relative);
  const char* r = fltk3::dir_chooser(RSTRING_CSTR(mrb, message),
    mrb_nil_p(fname) ? NULL : RSTRING_CSTR(mrb, fname), relative);
  return r ? mrb_str_new_cstr(mrb, r) : mrb_nil_value();
}

/* color_chooser(title, r, g, b, mode = -1) -> [r, g, b] or nil */
static mrb_value
mrb_fltk3_color_chooser(mrb_state* mrb, mrb_value self)
{
  mrb_value title;
  mrb_value rv, gv, bv;
  mrb_int mode = -1;
  mrb_get_args(mrb, "Sooo|i", &title, &rv, &gv, &bv, &mode);
  mrb_value ret = mrb_ary_new_capa(mrb, 3);
  if (mrb_float_p(rv) || mrb_float_p(gv) || mrb_float_p(bv)) {
    double r = mrb_as_float(mrb, rv), g = mrb_as_float(mrb, gv), b = mrb_as_float(mrb, bv);
    if (!fltk3::color_chooser(RSTRING_CSTR(mrb, title), r, g, b, (int) mode)) return mrb_nil_value();
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, r));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, g));
    mrb_ary_push(mrb, ret, mrb_float_value(mrb, b));
  } else {
    uchar r = (uchar) mrb_integer(rv), g = (uchar) mrb_integer(gv), b = (uchar) mrb_integer(bv);
    if (!fltk3::color_chooser(RSTRING_CSTR(mrb, title), r, g, b, (int) mode)) return mrb_nil_value();
    mrb_ary_push(mrb, ret, mrb_fixnum_value(r));
    mrb_ary_push(mrb, ret, mrb_fixnum_value(g));
    mrb_ary_push(mrb, ret, mrb_fixnum_value(b));
  }
  return ret;
}

/* tooltips -------------------------------------------------------------- */

#define DEFINE_TOOLTIP_FLOAT(name) \
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    return mrb_float_value(mrb, fltk3::Tooltip::name()); \
  }, MRB_ARGS_NONE()); \
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, # name "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    mrb_float f; \
    mrb_get_args(mrb, "f", &f); \
    fltk3::Tooltip::name((float) f); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_TOOLTIP_INT(name) \
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    return mrb_fixnum_value(fltk3::Tooltip::name()); \
  }, MRB_ARGS_NONE()); \
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, # name "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    mrb_int i; \
    mrb_get_args(mrb, "i", &i); \
    fltk3::Tooltip::name(i); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

#define DEFINE_INT_FUNC(name) \
  mrb_define_module_function(mrb, _class_fltk3, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    return mrb_fixnum_value(fltk3::name()); \
  }, MRB_ARGS_NONE());

#define DEFINE_BOOL_FUNC(name) \
  mrb_define_module_function(mrb, _class_fltk3, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    return mrb_bool_value(fltk3::name() != 0); \
  }, MRB_ARGS_NONE());

#define DEFINE_VOID_FUNC(name) \
  mrb_define_module_function(mrb, _class_fltk3, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    fltk3::name(); \
    return mrb_nil_value(); \
  }, MRB_ARGS_NONE());

void
mrb_fltk3_app_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  mrb_define_const(mrb, _class_fltk3, "VERSION", mrb_float_value(mrb, fltk3::version()));
  mrb_define_module_function(mrb, _class_fltk3, "version", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    return mrb_float_value(mrb, fltk3::version());
  }, MRB_ARGS_NONE());

  /* event loop */
  mrb_define_module_function(mrb, _class_fltk3, "wait", mrb_fltk3_wait, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "check", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    int r = fltk3::check();
    mrb_fltk3_check_exception(mrb);
    return mrb_fixnum_value(r);
  }, MRB_ARGS_NONE());
  DEFINE_INT_FUNC(ready);
  DEFINE_INT_FUNC(damage);
  DEFINE_VOID_FUNC(redraw);
  DEFINE_VOID_FUNC(flush);
  DEFINE_VOID_FUNC(release);
  DEFINE_VOID_FUNC(do_widget_deletion);
  mrb_define_module_function(mrb, _class_fltk3, "add_timeout", mrb_fltk3_add_timeout, MRB_ARGS_REQ(1) | MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "repeat_timeout", mrb_fltk3_repeat_timeout, MRB_ARGS_REQ(1) | MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "has_timeout", mrb_fltk3_has_timeout, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "remove_timeout", mrb_fltk3_remove_timeout, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "add_check", mrb_fltk3_add_check, MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "remove_check", mrb_fltk3_remove_check, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "add_idle", mrb_fltk3_add_idle, MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "has_idle", mrb_fltk3_has_idle, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "remove_idle", mrb_fltk3_remove_idle, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "first_window", mrb_fltk3_first_window, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "first_window=", mrb_fltk3_first_window_set, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "next_window", mrb_fltk3_next_window, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "windows", mrb_fltk3_windows, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "modal", mrb_fltk3_modal, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "grab", mrb_fltk3_grab, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "delete_widget", mrb_fltk3_delete_widget, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "handle", mrb_fltk3_handle, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "warning", mrb_fltk3_warning, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "error", mrb_fltk3_error, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "fatal", mrb_fltk3_fatal, MRB_ARGS_OPT(1) | MRB_ARGS_BLOCK());
  mrb_define_module_function(mrb, _class_fltk3, "option", mrb_fltk3_option, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  DEFINE_INT_CONST(OPTION_ARROW_FOCUS);
  DEFINE_INT_CONST(OPTION_VISIBLE_FOCUS);
  DEFINE_INT_CONST(OPTION_DND_TEXT);
  DEFINE_INT_CONST(OPTION_SHOW_TOOLTIPS);
  ARENA_RESTORE;

  /* events */
  DEFINE_INT_FUNC(event);
  DEFINE_INT_FUNC(event_x);
  DEFINE_INT_FUNC(event_y);
  DEFINE_INT_FUNC(event_x_root);
  DEFINE_INT_FUNC(event_y_root);
  DEFINE_INT_FUNC(event_dx);
  DEFINE_INT_FUNC(event_dy);
  DEFINE_INT_FUNC(event_clicks);
  DEFINE_INT_FUNC(event_button);
  DEFINE_INT_FUNC(event_buttons);
  DEFINE_INT_FUNC(event_original_key);
  DEFINE_INT_FUNC(event_length);
  DEFINE_BOOL_FUNC(event_is_click);
  DEFINE_BOOL_FUNC(event_shift);
  DEFINE_BOOL_FUNC(event_ctrl);
  DEFINE_BOOL_FUNC(event_alt);
  DEFINE_BOOL_FUNC(event_command);
  DEFINE_BOOL_FUNC(event_button1);
  DEFINE_BOOL_FUNC(event_button2);
  DEFINE_BOOL_FUNC(event_button3);
  mrb_define_module_function(mrb, _class_fltk3, "event_key", mrb_fltk3_event_key, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "get_key", mrb_fltk3_get_key, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "event_state", mrb_fltk3_event_state, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "event_text", mrb_fltk3_event_text, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "event_inside", mrb_fltk3_event_inside, MRB_ARGS_ANY());
  mrb_define_module_function(mrb, _class_fltk3, "get_mouse", mrb_fltk3_get_mouse, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "test_shortcut", mrb_fltk3_test_shortcut, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "focus", mrb_fltk3_focus, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "focus=", mrb_fltk3_focus_set, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "belowmouse", mrb_fltk3_belowmouse, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "belowmouse=", mrb_fltk3_belowmouse_set, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "pushed", mrb_fltk3_pushed, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "pushed=", mrb_fltk3_pushed_set, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "copy", mrb_fltk3_copy, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "paste", mrb_fltk3_paste, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "selection", mrb_fltk3_selection, MRB_ARGS_REQ(2));
  ARENA_RESTORE;

  /* screen, colors, fonts */
  DEFINE_INT_FUNC(x);
  DEFINE_INT_FUNC(y);
  DEFINE_INT_FUNC(w);
  DEFINE_INT_FUNC(h);
  DEFINE_INT_FUNC(screen_count);
  mrb_define_module_function(mrb, _class_fltk3, "screen_xywh", mrb_fltk3_screen_xywh, MRB_ARGS_ANY());
  mrb_define_module_function(mrb, _class_fltk3, "screen_work_area", mrb_fltk3_screen_work_area, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "screen_dpi", mrb_fltk3_screen_dpi, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "rgb_color", mrb_fltk3_rgb_color, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "get_color", mrb_fltk3_get_color, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "set_color", mrb_fltk3_set_color, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "free_color", mrb_fltk3_free_color, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "color_average", mrb_fltk3_color_average, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, _class_fltk3, "contrast", mrb_fltk3_contrast, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "inactive", mrb_fltk3_inactive, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "lighter", mrb_fltk3_lighter, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "darker", mrb_fltk3_darker, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "foreground", mrb_fltk3_foreground, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, _class_fltk3, "background", mrb_fltk3_background, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, _class_fltk3, "background2", mrb_fltk3_background2, MRB_ARGS_REQ(3));
  mrb_define_module_function(mrb, _class_fltk3, "scheme", mrb_fltk3_scheme, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "scheme=", mrb_fltk3_scheme, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "get_font", mrb_fltk3_get_font, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "get_font_sizes", mrb_fltk3_get_font_sizes, MRB_ARGS_REQ(1));
  DEFINE_VOID_FUNC(get_system_colors);
  DEFINE_INT_FUNC(reload_scheme);
  mrb_define_module_function(mrb, _class_fltk3, "scrollbar_size", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    return mrb_fixnum_value(fltk3::scrollbar_size());
  }, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "scrollbar_size=", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_int s;
    mrb_get_args(mrb, "i", &s);
    fltk3::scrollbar_size((int) s);
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  /* dialogs */
  DEFINE_INT_CONST(BEEP_DEFAULT);
  DEFINE_INT_CONST(BEEP_MESSAGE);
  DEFINE_INT_CONST(BEEP_ERROR);
  DEFINE_INT_CONST(BEEP_QUESTION);
  DEFINE_INT_CONST(BEEP_PASSWORD);
  DEFINE_INT_CONST(BEEP_NOTIFICATION);
  mrb_define_module_function(mrb, _class_fltk3, "message", mrb_fltk3_message, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "input", mrb_fltk3_input, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "password", mrb_fltk3_password, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "beep", mrb_fltk3_beep, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "message_font", mrb_fltk3_message_font, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "message_hotspot", mrb_fltk3_message_hotspot, MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "message_hotspot=", mrb_fltk3_message_hotspot, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "message_title=", mrb_fltk3_message_title, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "message_title_default=", mrb_fltk3_message_title_default, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "file_chooser", mrb_fltk3_file_chooser, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "dir_chooser", mrb_fltk3_dir_chooser, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3, "color_chooser", mrb_fltk3_color_chooser, MRB_ARGS_REQ(4) | MRB_ARGS_OPT(1));
  ARENA_RESTORE;

  /* tooltips */
  struct RClass* _class_fltk3_Tooltip = mrb_define_class_under(mrb, _class_fltk3, "Tooltip", mrb->object_class);
  DEFINE_TOOLTIP_FLOAT(delay);
  DEFINE_TOOLTIP_FLOAT(hoverdelay);
  DEFINE_TOOLTIP_INT(font);
  DEFINE_TOOLTIP_INT(size);
  DEFINE_TOOLTIP_INT(color);
  DEFINE_TOOLTIP_INT(textcolor);
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, "enabled", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    return mrb_bool_value(fltk3::Tooltip::enabled() != 0);
  }, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, "enable", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_bool b = TRUE;
    mrb_get_args(mrb, "|b", &b);
    fltk3::Tooltip::enable(b);
    return mrb_nil_value();
  }, MRB_ARGS_OPT(1));
  mrb_define_class_method(mrb, _class_fltk3_Tooltip, "disable", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    fltk3::Tooltip::disable();
    return mrb_nil_value();
  }, MRB_ARGS_NONE());
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
