#ifndef MRB_FLTK3_H
#define MRB_FLTK3_H

#include <string.h>
#include <stdlib.h>
#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <fltk3/Widget.h>
#include <fltk3/Box.h>
#include <fltk3/Image.h>
#include <fltk3/MenuItem.h>
#include <fltk3/TextBuffer.h>
#include <fltk3/FileChooser.h>
#include <fltk3/Tree.h>

#define DECLARE_TYPE(x) \
typedef struct { \
  fltk3::x* v; \
  mrb_value instance; \
  mrb_state* mrb; \
} mrb_fltk3_ ## x ## _context; \
extern const struct mrb_data_type fltk3_ ## x ## _type; \
mrb_value mrb_fltk3_ ## x ## _wrap(mrb_state* mrb, fltk3::x* v, const char* klass); \
fltk3::x* mrb_fltk3_ ## x ## _ptr(mrb_state* mrb, mrb_value obj); \
mrb_fltk3_ ## x ## _context* mrb_fltk3_ ## x ## _context_get(mrb_state* mrb, mrb_value obj); \
mrb_value mrb_fltk3_ ## x ## _setup(mrb_state* mrb, mrb_value self, fltk3::x* v);

DECLARE_TYPE(Widget)
DECLARE_TYPE(Box)
DECLARE_TYPE(TextBuffer)
DECLARE_TYPE(Image)
DECLARE_TYPE(MenuItem)
DECLARE_TYPE(FileChooser)
DECLARE_TYPE(TreeItem)

/* Instances of the FLTK3 classes are MRB_TT_DATA objects whose data pointer
 * is one of the context structs above.  Wrapping a raw fltk3 pointer that
 * already has a Ruby instance returns that instance. */

/* Ruby instances are kept alive in a hidden hash on the FLTK3 module,
 * keyed by the fltk3 object address, so that callbacks never touch a
 * collected object and wrapping the same pointer twice yields the same
 * instance. */
void mrb_fltk3_register(mrb_state* mrb, const void* ptr, mrb_value instance);
void mrb_fltk3_unregister(mrb_state* mrb, const void* ptr);
mrb_value mrb_fltk3_registered(mrb_state* mrb, const void* ptr);
struct RClass* mrb_fltk3_class(mrb_state* mrb, const char* name);

/* Look up the Ruby class that best matches the dynamic type of a widget. */
const char* mrb_fltk3_widget_classname(fltk3::Widget* w);
const char* mrb_fltk3_image_classname(fltk3::Image* image);

/* Detach the Ruby instances of a widget and of all its descendants before
 * the underlying fltk3 objects are deleted. */
void mrb_fltk3_widget_forget(mrb_state* mrb, fltk3::Widget* w);

bool mrb_fltk3_arg_check(const char* t, mrb_int argc, mrb_value* argv);

/* Calls a block from a native callback.  An exception raised by the block
 * is stashed and re-raised by FLTK3.run / wait / check, since there is no
 * Ruby frame to unwind into while fltk3 owns the stack. */
mrb_value mrb_fltk3_call(mrb_state* mrb, mrb_value proc, mrb_int argc, const mrb_value* argv);
mrb_value mrb_fltk3_send(mrb_state* mrb, mrb_value self, const char* name, mrb_int argc, const mrb_value* argv);
void mrb_fltk3_check_exception(mrb_state* mrb);

/* The interpreter that created the widgets, used by virtual overrides
 * that are entered from fltk3 without a Ruby context. */
extern mrb_state* mrb_fltk3_state;

/* Widgets created from Ruby are instances of this template so that draw()
 * and handle() can be overridden in Ruby. */
struct MrbOverridable {
  virtual void default_draw() = 0;
  virtual int default_handle(int event) = 0;
  /* protected helpers of fltk3::Widget made reachable from Ruby */
  virtual void default_draw_box() = 0;
  virtual void default_draw_box(fltk3::Box* b, int x, int y, int w, int h, fltk3::Color c) = 0;
  virtual void default_draw_label() = 0;
  virtual void default_draw_label(int x, int y, int w, int h, fltk3::Align a) = 0;
  virtual ~MrbOverridable() {}
};

void mrb_fltk3_widget_draw_hook(fltk3::Widget* w, MrbOverridable* o);
int mrb_fltk3_widget_handle_hook(fltk3::Widget* w, MrbOverridable* o, int event);

template <class T>
class MrbWidget : public T, public MrbOverridable {
public:
  using T::T;
  void draw() { mrb_fltk3_widget_draw_hook(this, this); }
  int handle(int event) { return mrb_fltk3_widget_handle_hook(this, this, event); }
  void default_draw() { T::draw(); }
  int default_handle(int event) { return T::handle(event); }
  void default_draw_box() { T::draw_box(); }
  void default_draw_box(fltk3::Box* b, int x, int y, int w, int h, fltk3::Color c) { T::draw_box(b, x, y, w, h, c); }
  void default_draw_label() { T::draw_label(); }
  void default_draw_label(int x, int y, int w, int h, fltk3::Align a) { T::draw_label(x, y, w, h, a); }
};

#define CONTEXT_SETUP(t) \
    mrb_fltk3_ ## t ## _context* context = mrb_fltk3_ ## t ## _context_get(mrb, self);

#define ARENA_SAVE \
  int ai = mrb_gc_arena_save(mrb);
#define ARENA_RESTORE \
  mrb_gc_arena_restore(mrb, ai);

/* Class definitions ------------------------------------------------------- */

#define DEFINE_CLASS(x, y) \
  struct RClass* _class_fltk3_ ## x = mrb_define_class_under(mrb, _class_fltk3, # x, _class_fltk3_ ## y); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "initialize", mrb_fltk3_ ## x ## _init, MRB_ARGS_ANY()); \
  ARENA_RESTORE;

/* Widgets constructed as (x, y, w, h [, label]). */
#define DECLARE_WIDGET(x)                                                 \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value *argv;                                                        \
  mrb_int argc;                                                           \
  mrb_get_args(mrb, "*", &argv, &argc);                                   \
  fltk3::Widget* w;                                                       \
  if (mrb_fltk3_arg_check("iiii", argc, argv)) {                          \
    w = new MrbWidget<fltk3::x> (                                         \
      (int) mrb_integer(argv[0]),                                         \
      (int) mrb_integer(argv[1]),                                         \
      (int) mrb_integer(argv[2]),                                         \
      (int) mrb_integer(argv[3]));                                        \
  } else if (mrb_fltk3_arg_check("iiiis", argc, argv)) {                  \
    w = new MrbWidget<fltk3::x> (                                         \
      (int) mrb_integer(argv[0]),                                         \
      (int) mrb_integer(argv[1]),                                         \
      (int) mrb_integer(argv[2]),                                         \
      (int) mrb_integer(argv[3]),                                         \
      RSTRING_CSTR(mrb, argv[4]));                                        \
  } else {                                                                \
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");                 \
  }                                                                       \
  return mrb_fltk3_Widget_setup(mrb, self, w);                            \
}

/* Windows constructed as (w, h [, label]) or (x, y, w, h [, label]). */
#define DECLARE_WINDOW(x)                                                 \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value *argv;                                                        \
  mrb_int argc;                                                           \
  mrb_get_args(mrb, "*", &argv, &argc);                                   \
  fltk3::Widget* w;                                                       \
  if (mrb_fltk3_arg_check("ii", argc, argv)) {                            \
    w = new MrbWidget<fltk3::x> (                                         \
      (int) mrb_integer(argv[0]),                                         \
      (int) mrb_integer(argv[1]));                                        \
  } else if (mrb_fltk3_arg_check("iis", argc, argv)) {                    \
    w = new MrbWidget<fltk3::x> (                                         \
      (int) mrb_integer(argv[0]),                                         \
      (int) mrb_integer(argv[1]),                                         \
      RSTRING_CSTR(mrb, argv[2]));                                        \
  } else if (mrb_fltk3_arg_check("iiii", argc, argv)) {                   \
    w = new MrbWidget<fltk3::x> (                                         \
      (int) mrb_integer(argv[0]),                                         \
      (int) mrb_integer(argv[1]),                                         \
      (int) mrb_integer(argv[2]),                                         \
      (int) mrb_integer(argv[3]));                                        \
  } else if (mrb_fltk3_arg_check("iiiis", argc, argv)) {                  \
    w = new MrbWidget<fltk3::x> (                                         \
      (int) mrb_integer(argv[0]),                                         \
      (int) mrb_integer(argv[1]),                                         \
      (int) mrb_integer(argv[2]),                                         \
      (int) mrb_integer(argv[3]),                                         \
      RSTRING_CSTR(mrb, argv[4]));                                        \
  } else {                                                                \
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");                 \
  }                                                                       \
  return mrb_fltk3_Widget_setup(mrb, self, w);                            \
}

/* Property helpers -------------------------------------------------------- */

#define DEFINE_INT_GETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    return mrb_fixnum_value(((fltk3::x*) context->v)->z()); \
  }, MRB_ARGS_NONE());

#define DEFINE_INT_SETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    mrb_int vv; \
    mrb_get_args(mrb, "i", &vv); \
    ((fltk3::x*) context->v)->z(vv); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_INT_PROP(x, y, z) \
  DEFINE_INT_GETTER(x, y, z) \
  DEFINE_INT_SETTER(x, y, z)

#define DEFINE_FLOAT_GETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    return mrb_float_value(mrb, ((fltk3::x*) context->v)->z()); \
  }, MRB_ARGS_NONE());

#define DEFINE_FLOAT_SETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    mrb_float vv; \
    mrb_get_args(mrb, "f", &vv); \
    ((fltk3::x*) context->v)->z(vv); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_FLOAT_PROP(x, y, z) \
  DEFINE_FLOAT_GETTER(x, y, z) \
  DEFINE_FLOAT_SETTER(x, y, z)

#define DEFINE_STR_GETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    const char* vv = ((fltk3::x*) context->v)->z(); \
    return vv ? mrb_str_new_cstr(mrb, vv) : mrb_nil_value(); \
  }, MRB_ARGS_NONE());

#define DEFINE_STR_SETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    mrb_value vv; \
    mrb_get_args(mrb, "S!", &vv); \
    ((fltk3::x*) context->v)->z(mrb_nil_p(vv) ? NULL : RSTRING_CSTR(mrb, vv)); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_STR_PROP(x, y, z) \
  DEFINE_STR_GETTER(x, y, z) \
  DEFINE_STR_SETTER(x, y, z)

#define DEFINE_BOOL_GETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    return mrb_bool_value(((fltk3::x*) context->v)->z() ? TRUE : FALSE); \
  }, MRB_ARGS_NONE());

#define DEFINE_BOOL_SETTER(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    mrb_bool vv; \
    mrb_get_args(mrb, "b", &vv); \
    ((fltk3::x*) context->v)->z(vv); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_BOOL_PROP(x, y, z) \
  DEFINE_BOOL_GETTER(x, y, z) \
  DEFINE_BOOL_SETTER(x, y, z)

/* A method taking no argument and returning nothing useful. */
#define DEFINE_VOID_METHOD(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    ((fltk3::x*) context->v)->z(); \
    return mrb_nil_value(); \
  }, MRB_ARGS_NONE());

/* textfont/textsize/textcolor, shared by several widget families. */
#define INHERIT_TEXT_PROPS(x) \
  DEFINE_INT_PROP(x, Widget, textfont) \
  DEFINE_INT_PROP(x, Widget, textsize) \
  DEFINE_INT_PROP(x, Widget, textcolor)

/* Per-area initializers, called from the gem init in this order. */
void mrb_fltk3_constants_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_app_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_draw_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_misc_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_tree_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_widget_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_image_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_box_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_group_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_button_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_input_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_valuator_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_menu_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_browser_init(mrb_state* mrb, struct RClass* _class_fltk3);
void mrb_fltk3_text_init(mrb_state* mrb, struct RClass* _class_fltk3);

#endif
