#include <memory.h>
#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <fltk3/Box.h>
#include <fltk3/Browser.h>
#include <fltk3/Button.h>
#include <fltk3/CheckButton.h>
#include <fltk3/DoubleWindow.h>
#include <fltk3/FileChooser.h>
#include <fltk3/Image.h>
#include <fltk3/SharedImage.h>
#include <fltk3/Input.h>
#include <fltk3/LightButton.h>
#include <fltk3/Menu.h>
#include <fltk3/MenuBar.h>
#include <fltk3/MenuButton.h>
#include <fltk3/RadioButton.h>
#include <fltk3/RadioLightButton.h>
#include <fltk3/RadioRoundButton.h>
#include <fltk3/RepeatButton.h>
#include <fltk3/ReturnButton.h>
#include <fltk3/RoundButton.h>
#include <fltk3/SelectBrowser.h>
#include <fltk3/TextDisplay.h>
#include <fltk3/TextEditor.h>
#include <fltk3/ToggleButton.h>
#include <fltk3/ToggleLightButton.h>
#include <fltk3/ToggleRoundButton.h>
#include <fltk3/ValueOutput.h>
#include <fltk3/Window.h>
#include <fltk3/message.h>
#include <fltk3/ask.h>
#include <fltk3/run.h>
#include <stdio.h>

#if 1
#define ARENA_SAVE \
  int ai = mrb_gc_arena_save(mrb); \
  if (ai == MRB_GC_ARENA_SIZE) { \
    mrb_raise(mrb, E_RUNTIME_ERROR, "arena overflow"); \
  }
#define ARENA_RESTORE \
  mrb_gc_arena_restore(mrb, ai);
#else
#define ARENA_SAVE
#define ARENA_RESTORE
#endif

#define DECLARE_TYPE(x) \
typedef struct { \
  fltk3::x* v; \
  mrb_value instance; \
  mrb_state* mrb; \
} mrb_fltk3_ ## x ## _context; \
\
static void \
fltk3_ ## x ## _free(mrb_state *mrb, void *p) { \
  free(p); \
} \
static const struct mrb_data_type \
fltk3_ ## x ## _type = { \
  "fltk3_" # x, fltk3_ ## x ## _free, \
};

DECLARE_TYPE(Widget);
DECLARE_TYPE(TextBuffer);
DECLARE_TYPE(Image);
DECLARE_TYPE(MenuItem);

static bool
arg_check(const char* t, int argc, mrb_value* argv)
{
  int n, l = strlen(t);
  if (l != argc) return false;
  for (n = 0; n < l; n++) {
    if (n >= argc) return false;
    if (t[n] == 'i' && mrb_type(argv[n]) != MRB_TT_FIXNUM) return false;
    if (t[n] == 'd' && mrb_type(argv[n]) != MRB_TT_FLOAT) return false;
    if (t[n] == 's' && mrb_type(argv[n]) != MRB_TT_STRING) return false;
    if (t[n] == 'b' && mrb_type(argv[n]) != MRB_TT_TRUE && mrb_type(argv[n]) != MRB_TT_FALSE) return false;
  }
  return true;
}

#define CONTEXT_SETUP(t) \
    mrb_value value_context; \
    mrb_fltk3_ ## t ## _context* context = NULL; \
    value_context = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "context")); \
    Data_Get_Struct(mrb, value_context, &fltk3_ ## t ## _type, context); \
    if (!context || !context->v) mrb_raise(mrb, E_RUNTIME_ERROR, "invalid " # t);

/* Ruby instances are kept alive in a hidden hash on the FLTK3 module,
 * keyed by the fltk3 object address, so that callbacks never touch a
 * collected object and wrapping the same pointer twice yields the same
 * instance. */
static mrb_value
mrb_fltk3_registry(mrb_state* mrb)
{
  struct RClass* _class_fltk3 = mrb_module_get(mrb, "FLTK3");
  mrb_value reg = mrb_iv_get(mrb, mrb_obj_value(_class_fltk3), mrb_intern_lit(mrb, "__objects__"));
  if (mrb_nil_p(reg)) {
    reg = mrb_hash_new(mrb);
    mrb_iv_set(mrb, mrb_obj_value(_class_fltk3), mrb_intern_lit(mrb, "__objects__"), reg);
  }
  return reg;
}

static mrb_value
mrb_fltk3_registry_key(mrb_state* mrb, const void* ptr)
{
  return mrb_str_new(mrb, (const char*) &ptr, sizeof(ptr));
}

static void
mrb_fltk3_register(mrb_state* mrb, const void* ptr, mrb_value instance)
{
  mrb_hash_set(mrb, mrb_fltk3_registry(mrb), mrb_fltk3_registry_key(mrb, ptr), instance);
}

static void
mrb_fltk3_unregister(mrb_state* mrb, const void* ptr)
{
  mrb_hash_delete_key(mrb, mrb_fltk3_registry(mrb), mrb_fltk3_registry_key(mrb, ptr));
}

static mrb_value
mrb_fltk3_registered(mrb_state* mrb, const void* ptr)
{
  return mrb_hash_get(mrb, mrb_fltk3_registry(mrb), mrb_fltk3_registry_key(mrb, ptr));
}

static struct RClass*
mrb_fltk3_class(mrb_state* mrb, const char* name)
{
  struct RClass* _class_fltk3 = mrb_module_get(mrb, "FLTK3");
  return mrb_class_get_under(mrb, _class_fltk3, name);
}

#define DECLARE_WRAP(t) \
static mrb_value \
mrb_fltk3_ ## t ## _wrap(mrb_state* mrb, fltk3::t* v, const char* klass) \
{ \
  if (!v) return mrb_nil_value(); \
  mrb_value instance = mrb_fltk3_registered(mrb, v); \
  if (!mrb_nil_p(instance)) return instance; \
  mrb_fltk3_ ## t ## _context* context = \
    (mrb_fltk3_ ## t ## _context*) malloc(sizeof(mrb_fltk3_ ## t ## _context)); \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory"); \
  memset(context, 0, sizeof(mrb_fltk3_ ## t ## _context)); \
  instance = mrb_obj_value(mrb_obj_alloc(mrb, MRB_TT_OBJECT, mrb_fltk3_class(mrb, klass))); \
  context->instance = instance; \
  context->mrb = mrb; \
  context->v = v; \
  mrb_iv_set(mrb, instance, mrb_intern_lit(mrb, "context"), mrb_obj_value( \
    Data_Wrap_Struct(mrb, mrb->object_class, &fltk3_ ## t ## _type, (void*) context))); \
  mrb_fltk3_register(mrb, v, instance); \
  return instance; \
}

DECLARE_WRAP(Widget)
DECLARE_WRAP(TextBuffer)
DECLARE_WRAP(Image)
DECLARE_WRAP(MenuItem)

/*********************************************************
 * FLTK3::Widget
 *********************************************************/
static mrb_value
mrb_fltk3_widget_box_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Widget_wrap(mrb, (fltk3::Widget*) context->v->box(), "Box");
}

static mrb_value
mrb_fltk3_widget_box_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value box;
  mrb_get_args(mrb, "o", &box);
  if (!mrb_nil_p(box)) {
    mrb_value box_value_context;
    mrb_fltk3_Widget_context* box_context = NULL;
    box_value_context = mrb_iv_get(mrb, box, mrb_intern_lit(mrb, "context"));
    Data_Get_Struct(mrb, box_value_context, &fltk3_Widget_type, box_context);
    context->v->box((fltk3::Box*) box_context->v);
  } else
    context->v->box(NULL);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_image_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Image_wrap(mrb, context->v->image(), "Image");
}

static mrb_value
mrb_fltk3_widget_image_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value image;
  mrb_get_args(mrb, "o", &image);
  mrb_fltk3_Image_context* image_context = NULL;
  if (!mrb_nil_p(image)) {
    mrb_value image_value_context;
    image_value_context = mrb_iv_get(mrb, image, mrb_intern_lit(mrb, "context"));
    Data_Get_Struct(mrb, image_value_context, &fltk3_Image_type, image_context);
    context->v->image((fltk3::Image*) image_context->v);
  } else
    context->v->image(NULL);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_show(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  context->v->show();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_hide(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  context->v->hide();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_visible(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return context->v->visible() ? mrb_true_value() : mrb_false_value();
}

static void
_mrb_fltk3_widget_callback(fltk3::Widget* v, void* data)
{
  mrb_value args[2];
  mrb_fltk3_Widget_context* context = (mrb_fltk3_Widget_context*) data;
  mrb_state* mrb = context->mrb;
  mrb_value proc = mrb_iv_get(mrb, context->instance, mrb_intern_lit(mrb, "callback"));
  mrb_value value = mrb_iv_get(mrb, context->instance, mrb_intern_lit(mrb, "value"));
  if (mrb_nil_p(proc)) return;
  args[0] = context->instance;
  args[1] = value;
  int ai = mrb_gc_arena_save(mrb);
  mrb_yield_argv(mrb, proc, 2, args);
  mrb_gc_arena_restore(mrb, ai);
}

static mrb_value
mrb_fltk3_widget_callback(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b = mrb_nil_value();
  mrb_value v = mrb_nil_value();
  mrb_get_args(mrb, "&|o", &b, &v);
  if (!mrb_nil_p(b)) {
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "callback"), b);
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "value"), v);
    context->v->callback(_mrb_fltk3_widget_callback, context);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_show(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  ((fltk3::Window*) context->v)->show(0, NULL);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::Group
 *********************************************************/
static mrb_value
mrb_fltk3_group_begin(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b = mrb_nil_value();
  mrb_get_args(mrb, "&", &b);
  if (!mrb_nil_p(b)) {
    mrb_value args[1];
    args[0] = self;
    ((fltk3::Group*)context->v)->begin();
    mrb_yield_argv(mrb, b, 1, args);
    ((fltk3::Group*)context->v)->end();
  } else
    ((fltk3::Group*)context->v)->begin();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_group_end(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  ((fltk3::Group*)context->v)->end();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_group_resizable_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Widget_wrap(mrb, ((fltk3::Group*) context->v)->resizable(), "Widget");
}

static mrb_value
mrb_fltk3_group_resizable_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value arg;
  mrb_get_args(mrb, "o", &arg);
  mrb_value arg_value_context;
  mrb_fltk3_Widget_context* arg_context = NULL;
  arg_value_context = mrb_iv_get(mrb, arg, mrb_intern_lit(mrb, "context"));
  Data_Get_Struct(mrb, arg_value_context, &fltk3_Widget_type, arg_context);
  ((fltk3::Group*) context->v)->resizable(arg_context->v);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::*
 *********************************************************/
static mrb_value
mrb_fltk3_run(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(fltk3::run());
}

static mrb_value
mrb_fltk3_alert(mrb_state *mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  fltk3::alert("%s", RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_choice(mrb_state *mrb, mrb_value self)
{
  mrb_value args[4];
  args[3] = mrb_nil_value();
  mrb_get_args(mrb, "SSS|S", &args[0], &args[1], &args[2], &args[3]);
  return mrb_fixnum_value(fltk3::choice("%s",
    RSTRING_CSTR(mrb, args[1]),
    RSTRING_CSTR(mrb, args[2]),
    mrb_nil_p(args[3]) ? NULL : RSTRING_CSTR(mrb, args[3]),
    RSTRING_CSTR(mrb, args[0])));
}

static mrb_value
mrb_fltk3_ask(mrb_state *mrb, mrb_value self)
{
  mrb_value arg;
  mrb_get_args(mrb, "S", &arg);
  return fltk3::ask("%s", RSTRING_CSTR(mrb, arg)) ? mrb_true_value() : mrb_false_value();
}

static mrb_value
mrb_fltk3_set_fonts(mrb_state *mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  return mrb_fixnum_value(fltk3::set_fonts(RSTRING_CSTR(mrb, s)));
}

static mrb_value
mrb_fltk3_font_name(mrb_state *mrb, mrb_value self)
{
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  int font_type = 0;
  const char *name = fltk3::get_font_name((fltk3::Font) i, &font_type);
  return name ? mrb_str_new_cstr(mrb, name) : mrb_nil_value();
}

#define DECLARE_WIDGET(x)                                                 \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value *argv;                                                        \
  mrb_int argc;                                                           \
  mrb_get_args(mrb, "*", &argv, &argc);                                   \
  mrb_fltk3_Widget_context* context =                                     \
    (mrb_fltk3_Widget_context*) malloc(sizeof(mrb_fltk3_Widget_context)); \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");    \
  memset(context, 0, sizeof(mrb_fltk3_Widget_context));                   \
  context->instance = self;                                               \
  context->mrb = mrb;                                                     \
  if (arg_check("iiii", argc, argv)) {                                    \
    context->v = (fltk3::Widget*) new fltk3::x (                          \
      (int) mrb_fixnum(argv[0]),                                          \
      (int) mrb_fixnum(argv[1]),                                          \
      (int) mrb_fixnum(argv[2]),                                          \
      (int) mrb_fixnum(argv[3]));                                         \
  } else if (arg_check("iiiis", argc, argv)) {                            \
    context->v = (fltk3::Widget*) new fltk3::x (                          \
      (int) mrb_fixnum(argv[0]),                                          \
      (int) mrb_fixnum(argv[1]),                                          \
      (int) mrb_fixnum(argv[2]),                                          \
      (int) mrb_fixnum(argv[3]),                                          \
      RSTRING_CSTR(mrb, argv[4]));                                        \
  } else {                                                                \
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");                 \
  }                                                                       \
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), mrb_obj_value(    \
    Data_Wrap_Struct(mrb, mrb->object_class,                              \
    &fltk3_Widget_type, (void*) context)));                               \
  mrb_fltk3_register(mrb, context->v, self);                              \
  return self;                                                            \
}

#define DECLARE_WINDOW(x)                                                 \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value *argv;                                                        \
  mrb_int argc;                                                           \
  mrb_get_args(mrb, "*", &argv, &argc);                                   \
  mrb_fltk3_Widget_context* context =                                     \
    (mrb_fltk3_Widget_context*) malloc(sizeof(mrb_fltk3_Widget_context)); \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");    \
  memset(context, 0, sizeof(mrb_fltk3_Widget_context));                   \
  context->instance = self;                                               \
  context->mrb = mrb;                                                     \
  if (arg_check("iis", argc, argv)) {                                     \
    context->v = (fltk3::Widget*) new fltk3::x (                          \
      (int) mrb_fixnum(argv[0]),                                          \
      (int) mrb_fixnum(argv[1]),                                          \
      RSTRING_CSTR(mrb, argv[2]));                                        \
  } else if (arg_check("iiiis", argc, argv)) {                            \
    context->v = (fltk3::Widget*) new fltk3::x (                          \
      (int) mrb_fixnum(argv[0]),                                          \
      (int) mrb_fixnum(argv[1]),                                          \
      (int) mrb_fixnum(argv[2]),                                          \
      (int) mrb_fixnum(argv[3]),                                          \
      RSTRING_CSTR(mrb, argv[4]));                                        \
  } else {                                                                \
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");                 \
  }                                                                       \
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), mrb_obj_value(    \
    Data_Wrap_Struct(mrb, mrb->object_class,                              \
    &fltk3_Widget_type, (void*) context)));                               \
  mrb_fltk3_register(mrb, context->v, self);                              \
  return self;                                                            \
}

#define DECLARE_HIDDEN_OBJECT(x, y)                                       \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value arg = mrb_nil_value();                                        \
  mrb_get_args(mrb, "|o", &arg);                                          \
  if (mrb_nil_p(arg) || !DATA_PTR(arg) || DATA_TYPE(arg) != &fltk3_ ## y ## _type) \
    mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc fltk3::" # x);           \
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), arg);             \
  return self;                                                            \
}

#define DECLARE_BOX(x)                                                    \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value arg = mrb_nil_value();                                        \
  mrb_get_args(mrb, "|S", &arg);                                          \
  mrb_fltk3_Widget_context* context =                                     \
    (mrb_fltk3_Widget_context*) malloc(sizeof(mrb_fltk3_Widget_context)); \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");    \
  memset(context, 0, sizeof(mrb_fltk3_Widget_context));                   \
  context->instance = self;                                               \
  context->mrb = mrb;                                                     \
  context->v = (fltk3::Widget*) new fltk3::x (                            \
    mrb_nil_p(arg) ? NULL : RSTRING_CSTR(mrb, arg));                      \
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), mrb_obj_value(    \
    Data_Wrap_Struct(mrb, mrb->object_class,                              \
    &fltk3_Widget_type, (void*) context)));                               \
  mrb_fltk3_register(mrb, context->v, self);                              \
  return self;                                                            \
}

DECLARE_HIDDEN_OBJECT(SharedImage, Image)

DECLARE_WIDGET(Widget)
DECLARE_WIDGET(Browser)
DECLARE_WIDGET(SelectBrowser)
DECLARE_WIDGET(ValueOutput)
DECLARE_WIDGET(Input)
DECLARE_WIDGET(MenuBar)

DECLARE_WIDGET(Button)
DECLARE_WIDGET(CheckButton)
DECLARE_WIDGET(LightButton)
DECLARE_WIDGET(MenuButton)
DECLARE_WIDGET(RadioButton)
DECLARE_WIDGET(RadioLightButton)
DECLARE_WIDGET(RadioRoundButton)
DECLARE_WIDGET(RepeatButton)
DECLARE_WIDGET(ReturnButton)
DECLARE_WIDGET(RoundButton)
DECLARE_WIDGET(ToggleButton)
DECLARE_WIDGET(ToggleLightButton)
DECLARE_WIDGET(ToggleRoundButton)

DECLARE_HIDDEN_OBJECT(Group, Widget)
DECLARE_WIDGET(TextDisplay)
DECLARE_WIDGET(TextEditor)

DECLARE_WINDOW(DoubleWindow)
DECLARE_WINDOW(Window)

DECLARE_HIDDEN_OBJECT(Box, Widget)
DECLARE_BOX(NoBox)
DECLARE_BOX(FlatBox)
DECLARE_BOX(UpBox)
DECLARE_BOX(DownBox)
DECLARE_BOX(ThinUpBox)
DECLARE_BOX(ThinDownBox)
DECLARE_BOX(EngravedBox)
DECLARE_BOX(EmbossedBox)
DECLARE_BOX(BorderBox)
DECLARE_BOX(ShadowBox)
DECLARE_BOX(RoundedBox)
DECLARE_BOX(RShadowBox)
DECLARE_BOX(RFlatBox)
DECLARE_BOX(RoundUpBox)
DECLARE_BOX(RoundDownBox)
DECLARE_BOX(DiamondUpBox)
DECLARE_BOX(DiamondDownBox)
DECLARE_BOX(OvalBox)
DECLARE_BOX(OShadowBox)
DECLARE_BOX(OFlatBox)
DECLARE_BOX(PlasticUpBox)
DECLARE_BOX(PlasticDownBox)
DECLARE_BOX(PlasticThinUpBox)
DECLARE_BOX(PlasticThinDownBox)
DECLARE_BOX(PlasticRoundUpBox)
DECLARE_BOX(PlasticRoundDownBox)
DECLARE_BOX(ClassicUpBox)
DECLARE_BOX(ClassicDownBox)
DECLARE_BOX(ClassicThinUpBox)
DECLARE_BOX(ClassicThinDownBox)
DECLARE_BOX(ClassicRoundUpBox)
DECLARE_BOX(ClassicRoundDownBox)
DECLARE_BOX(BorderFrame)
DECLARE_BOX(UpFrame)
DECLARE_BOX(DownFrame)
DECLARE_BOX(ThinUpFrame)
DECLARE_BOX(ThinDownFrame)
DECLARE_BOX(EngravedFrame)
DECLARE_BOX(EmbossedFrame)
DECLARE_BOX(ShadowFrame)
DECLARE_BOX(RoundedFrame)
DECLARE_BOX(OvalFrame)
DECLARE_BOX(PlasticUpFrame)
DECLARE_BOX(PlasticDownFrame)
DECLARE_BOX(ClassicUpFrame)
DECLARE_BOX(ClassicDownFrame)
DECLARE_BOX(ClassicThinUpFrame)
DECLARE_BOX(ClassicThinDownFrame)

extern "C"
{
#define INHERIT_GROUP(x) \
  mrb_define_method(mrb, _class_fltk3_ ## x, "begin", mrb_fltk3_group_begin, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "end", mrb_fltk3_group_end, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "resizable", mrb_fltk3_group_resizable_get, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "resizable=", mrb_fltk3_group_resizable_set, MRB_ARGS_REQ(1)); \
  ARENA_RESTORE;

#define DEFINE_FIXNUM_PROP_READONLY(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    return mrb_fixnum_value(((fltk3::x*) context->v)->z()); \
  }, MRB_ARGS_NONE());

#define DEFINE_FIXNUM_PROP(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    return mrb_fixnum_value(((fltk3::x*) context->v)->z()); \
  }, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    mrb_int vv; \
    mrb_get_args(mrb, "i", &vv); \
    ((fltk3::x*) context->v)->z(vv); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1)); \
  ARENA_RESTORE;

#define DEFINE_STR_PROP(x, y, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    const char* vv = ((fltk3::x*) context->v)->z(); \
    return vv ? mrb_str_new_cstr(mrb, vv) : mrb_nil_value(); \
  }, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(y); \
    mrb_value vv; \
    mrb_get_args(mrb, "S", &vv); \
    ((fltk3::x*) context->v)->z(RSTRING_CSTR(mrb, vv)); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1)); \
  ARENA_RESTORE;

#define INHERIT_INPUT_VALUE(x) \
  mrb_define_method(mrb, _class_fltk3_ ## x, "value", mrb_fltk3_input_value_get, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "value=", mrb_fltk3_input_value_set, MRB_ARGS_REQ(1)); \
  ARENA_RESTORE;

#define DEFINE_CLASS(x, y) \
  struct RClass* _class_fltk3_ ## x = mrb_define_class_under(mrb, _class_fltk3, # x, _class_fltk3_ ## y); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "initialize", mrb_fltk3_ ## x ## _init, MRB_ARGS_ANY()); \
  ARENA_RESTORE;

void
mrb_mruby_fltk3_gem_init(mrb_state* mrb)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3 = mrb_define_module(mrb, "FLTK3");
  mrb_define_module_function(mrb, _class_fltk3, "run", mrb_fltk3_run, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "alert", mrb_fltk3_alert, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "ask", mrb_fltk3_ask, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "choice", mrb_fltk3_choice, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "set_fonts", mrb_fltk3_set_fonts, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "font_name", mrb_fltk3_font_name, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "file_chooser", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_value message, pattern;
    mrb_get_args(mrb, "SS", &message, &pattern);
    const char *fname = fltk3::file_chooser(RSTRING_CSTR(mrb, message), RSTRING_CSTR(mrb, pattern), NULL);
    if (fname) {
      return mrb_str_new_cstr(mrb, fname);
    }
    return mrb_nil_value();
  }, MRB_ARGS_REQ(2));
  ARENA_RESTORE;

  struct RClass* _class_fltk3_Image = mrb_define_class_under(mrb, _class_fltk3, "Image", mrb->object_class);
  mrb_define_method(mrb, _class_fltk3_Image, "initialize", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_value arg;
    mrb_get_args(mrb, "o", &arg);
    if (!DATA_PTR(arg) || DATA_TYPE(arg) != &fltk3_Image_type)
      mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), arg);
    return self;
  }, MRB_ARGS_REQ(1));
  DEFINE_FIXNUM_PROP_READONLY(Image, Image, w);
  DEFINE_FIXNUM_PROP_READONLY(Image, Image, h);
  DEFINE_FIXNUM_PROP_READONLY(Image, Image, d);
  DEFINE_FIXNUM_PROP_READONLY(Image, Image, ld);
  mrb_define_method(mrb, _class_fltk3_Image, "release", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Image);
    fltk3::Image* image = context->v;
    mrb_fltk3_unregister(mrb, image);
    context->v = NULL;
    fltk3::SharedImage* shared = dynamic_cast<fltk3::SharedImage*>(image);
    if (shared) shared->release(); else delete image;
    return mrb_nil_value();
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Image, "copy", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Image);
    mrb_int width, height;
    mrb_get_args(mrb, "ii", &width, &height);
    return mrb_fltk3_Image_wrap(mrb, context->v->copy(width, height), "Image");
  }, MRB_ARGS_REQ(2));
  DEFINE_CLASS(SharedImage, Image);
  mrb_define_module_function(mrb, _class_fltk3_SharedImage, "get", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_value filename;
    mrb_get_args(mrb, "S", &filename);
    fltk3::Image* image = (fltk3::Image*) fltk3::SharedImage::get(RSTRING_CSTR(mrb, filename));
    return mrb_fltk3_Image_wrap(mrb, image, "SharedImage");
  }, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  struct RClass* _class_fltk3_Widget = mrb_define_class_under(mrb, _class_fltk3, "Widget", mrb->object_class);
  mrb_define_method(mrb, _class_fltk3_Widget, "initialize", mrb_fltk3_Widget_init, MRB_ARGS_ANY());
  mrb_define_method(mrb, _class_fltk3_Widget, "redraw", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    context->v->redraw();
    return mrb_nil_value();
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "show", mrb_fltk3_widget_show, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "hide", mrb_fltk3_widget_hide, MRB_ARGS_NONE());
  DEFINE_FIXNUM_PROP(Widget, Widget, x);
  DEFINE_FIXNUM_PROP(Widget, Widget, y);
  DEFINE_FIXNUM_PROP(Widget, Widget, w);
  DEFINE_FIXNUM_PROP(Widget, Widget, h);
  DEFINE_FIXNUM_PROP(Widget, Widget, labelfont);
  DEFINE_FIXNUM_PROP(Widget, Widget, labelsize);
  DEFINE_STR_PROP(Widget, Widget, label);
  mrb_define_method(mrb, _class_fltk3_Widget, "box", mrb_fltk3_widget_box_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "box=", mrb_fltk3_widget_box_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "image", mrb_fltk3_widget_image_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "image=", mrb_fltk3_widget_image_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "visible", mrb_fltk3_widget_visible, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "callback", mrb_fltk3_widget_callback, MRB_ARGS_OPT(1));
  ARENA_RESTORE;

  DEFINE_CLASS(ValueOutput, Widget);

  DEFINE_CLASS(Button, Widget);
  DEFINE_CLASS(CheckButton, Button);
  DEFINE_CLASS(LightButton, Button);
  DEFINE_CLASS(MenuButton, Button);
  DEFINE_CLASS(RadioButton, Button);
  DEFINE_CLASS(RadioLightButton, Button);
  DEFINE_CLASS(RadioRoundButton, Button);
  DEFINE_CLASS(RepeatButton, Button);
  DEFINE_CLASS(ReturnButton, Button);
  DEFINE_CLASS(RoundButton, Button);
  DEFINE_CLASS(ToggleButton, Button);
  DEFINE_CLASS(ToggleLightButton, Button);
  DEFINE_CLASS(ToggleRoundButton, Button);

  DEFINE_CLASS(Input, Widget);
  DEFINE_STR_PROP(Input, Widget, value);

  struct RClass* _class_fltk3_MenuItem = mrb_define_class_under(mrb, _class_fltk3, "MenuItem", mrb->object_class);
  mrb_define_method(mrb, _class_fltk3_MenuItem, "initialize", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_fltk3_MenuItem_context* context =
      (mrb_fltk3_MenuItem_context*) malloc(sizeof(mrb_fltk3_MenuItem_context));
    if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");
    memset(context, 0, sizeof(mrb_fltk3_MenuItem_context));
    context->instance = self;
    context->mrb = mrb;
    context->v = new fltk3::MenuItem;
    memset(context->v, 0, sizeof(fltk3::MenuItem));
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), mrb_obj_value(
      Data_Wrap_Struct(mrb, mrb->object_class,
      &fltk3_MenuItem_type, (void*) context)));
    mrb_fltk3_register(mrb, context->v, self);
    return self;
  }, MRB_ARGS_NONE());

  DEFINE_CLASS(MenuBar, MenuItem);
  mrb_define_method(mrb, _class_fltk3_MenuBar, "add", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_value b = mrb_nil_value(), caption;
    mrb_int shortcut = 0;
    mrb_get_args(mrb, "&S|i", &b, &caption, &shortcut);
    ((fltk3::MenuBar*) context->v)->add(RSTRING_CSTR(mrb, caption), (int) shortcut, [] (fltk3::Widget* w, void* d) {
    });
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_MenuBar, "menu", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_fltk3_MenuItem_wrap(mrb, (fltk3::MenuItem*) ((fltk3::MenuBar*) context->v)->menu(), "MenuItem");
  }, MRB_ARGS_NONE());

  DEFINE_CLASS(Group, Widget);
  INHERIT_GROUP(Group);

  DEFINE_CLASS(Browser, Group);
  mrb_define_method(mrb, _class_fltk3_Browser, "load", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_value filename;
    mrb_get_args(mrb, "S", &filename);
    return mrb_fixnum_value(((fltk3::Browser*) context->v)->load(RSTRING_CSTR(mrb, filename)));
  }, MRB_ARGS_REQ(1));
  DEFINE_FIXNUM_PROP(Browser, Widget, value);
  mrb_define_method(mrb, _class_fltk3_Browser, "text", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_int line = 0;
    mrb_value text = mrb_nil_value();
    mrb_int argc = mrb_get_args(mrb, "|iS", &line, &text);
    if (argc < 2) {
      if (argc == 0) line = ((fltk3::Browser*) context->v)->value();
      const char* s = ((fltk3::Browser*) context->v)->text((int) line);
      return s ? mrb_str_new_cstr(mrb, s) : mrb_nil_value();
    }
    ((fltk3::Browser*) context->v)->text((int) line, RSTRING_CSTR(mrb, text));
    return mrb_nil_value();
  }, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "icon", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_int line = 0;
    mrb_value image = mrb_nil_value();
    mrb_int argc = mrb_get_args(mrb, "|io", &line, &image);
    if (argc < 2) {
      if (argc == 0) line = ((fltk3::Browser*) context->v)->value();
      return mrb_fltk3_Image_wrap(mrb, ((fltk3::Browser*) context->v)->icon((int) line), "Image");
    }
    if (mrb_nil_p(image)) {
      ((fltk3::Browser*) context->v)->icon((int) line, NULL);
    } else {
      mrb_value image_value_context = mrb_iv_get(mrb, image, mrb_intern_lit(mrb, "context"));
      mrb_fltk3_Image_context* image_context = NULL;
      Data_Get_Struct(mrb, image_value_context, &fltk3_Image_type, image_context);
      ((fltk3::Browser*) context->v)->icon((int) line, image_context->v);
    }
    return mrb_nil_value();
  }, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "add", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_value text = mrb_nil_value();
    mrb_get_args(mrb, "S", &text);
    ((fltk3::Browser*) context->v)->add(RSTRING_CSTR(mrb, text));
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "column_widths", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    const int* widths = ((fltk3::Browser*) context->v)->column_widths();
    mrb_value arr = mrb_ary_new(mrb);
    for (int n = 0; widths && widths[n]; n++) {
      mrb_ary_push(mrb, arr, mrb_fixnum_value(widths[n]));
    }
    return arr;
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Browser, "column_widths=", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_value arr = mrb_nil_value();
    mrb_get_args(mrb, "A", &arr);
    mrb_int n, len = RARRAY_LEN(arr);
    /* the array must outlive the widget; keep it in a hidden string */
    mrb_value buf = mrb_str_new(mrb, NULL, sizeof(int) * (len + 1));
    int* widths = (int*) RSTRING_PTR(buf);
    for (n = 0; n < len; n++) {
      widths[n] = (int) mrb_integer(mrb_funcall(mrb, RARRAY_PTR(arr)[n], "to_i", 0));
    }
    widths[n] = 0;
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "column_widths"), buf);
    ((fltk3::Browser*) context->v)->column_widths(widths);
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));

  DEFINE_CLASS(SelectBrowser, Browser);

  DEFINE_CLASS(TextDisplay, Group);
  DEFINE_CLASS(TextEditor, TextDisplay);

  struct RClass* _class_fltk3_Window = mrb_define_class_under(mrb, _class_fltk3, "Window", _class_fltk3_Widget);
  mrb_define_method(mrb, _class_fltk3_Window, "initialize", mrb_fltk3_Window_init, MRB_ARGS_ANY());
  mrb_define_method(mrb, _class_fltk3_Window, "show", mrb_fltk3_window_show, MRB_ARGS_OPT(1));
  INHERIT_GROUP(Window);

  DEFINE_CLASS(DoubleWindow, Window);

  DEFINE_CLASS(Box, Widget);
  DEFINE_CLASS(NoBox, Box);
  DEFINE_CLASS(FlatBox, Box);
  DEFINE_CLASS(UpBox, Box);
  DEFINE_CLASS(DownBox, Box);
  DEFINE_CLASS(ThinUpBox, Box);
  DEFINE_CLASS(ThinDownBox, Box);
  DEFINE_CLASS(EngravedBox, Box);
  DEFINE_CLASS(EmbossedBox, Box);
  DEFINE_CLASS(BorderBox, Box);
  DEFINE_CLASS(ShadowBox, Box);
  DEFINE_CLASS(RoundedBox, Box);
  DEFINE_CLASS(RShadowBox, Box);
  DEFINE_CLASS(RFlatBox, Box);
  DEFINE_CLASS(RoundUpBox, Box);
  DEFINE_CLASS(RoundDownBox, Box);
  DEFINE_CLASS(DiamondUpBox, Box);
  DEFINE_CLASS(DiamondDownBox, Box);
  DEFINE_CLASS(OvalBox, Box);
  DEFINE_CLASS(OShadowBox, Box);
  DEFINE_CLASS(OFlatBox, Box);
  DEFINE_CLASS(PlasticUpBox, Box);
  DEFINE_CLASS(PlasticDownBox, Box);
  DEFINE_CLASS(PlasticThinUpBox, Box);
  DEFINE_CLASS(PlasticThinDownBox, Box);
  DEFINE_CLASS(PlasticRoundUpBox, Box);
  DEFINE_CLASS(PlasticRoundDownBox, Box);
  DEFINE_CLASS(ClassicUpBox, Box);
  DEFINE_CLASS(ClassicDownBox, Box);
  DEFINE_CLASS(ClassicThinUpBox, Box);
  DEFINE_CLASS(ClassicThinDownBox, Box);
  DEFINE_CLASS(ClassicRoundUpBox, Box);
  DEFINE_CLASS(ClassicRoundDownBox, Box);
  DEFINE_CLASS(BorderFrame, Box);
  DEFINE_CLASS(UpFrame, Box);
  DEFINE_CLASS(DownFrame, Box);
  DEFINE_CLASS(ThinUpFrame, Box);
  DEFINE_CLASS(ThinDownFrame, Box);
  DEFINE_CLASS(EngravedFrame, Box);
  DEFINE_CLASS(EmbossedFrame, Box);
  DEFINE_CLASS(ShadowFrame, Box);
  DEFINE_CLASS(RoundedFrame, Box);
  DEFINE_CLASS(OvalFrame, Box);
  DEFINE_CLASS(PlasticUpFrame, Box);
  DEFINE_CLASS(PlasticDownFrame, Box);
  DEFINE_CLASS(ClassicUpFrame, Box);
  DEFINE_CLASS(ClassicDownFrame, Box);
  DEFINE_CLASS(ClassicThinUpFrame, Box);
  DEFINE_CLASS(ClassicThinDownFrame, Box);

  struct RClass* _class_fltk3_TextBuffer = mrb_define_class_under(mrb, _class_fltk3, "TextBuffer", mrb->object_class);
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "initialize", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    mrb_fltk3_TextBuffer_context* context =
      (mrb_fltk3_TextBuffer_context*) malloc(sizeof(mrb_fltk3_TextBuffer_context));
    if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");
    memset(context, 0, sizeof(mrb_fltk3_TextBuffer_context));
    context->instance = self;
    context->mrb = mrb;
    context->v = new fltk3::TextBuffer;
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "context"), mrb_obj_value(
      Data_Wrap_Struct(mrb, mrb->object_class,
      &fltk3_TextBuffer_type, (void*) context)));
    mrb_fltk3_register(mrb, context->v, self);
    return self;
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "buffer", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_fltk3_TextBuffer_wrap(mrb, ((fltk3::TextDisplay*) context->v)->buffer(), "TextBuffer");
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "buffer=", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_value textbuffer;
    mrb_get_args(mrb, "o", &textbuffer);
    mrb_value textbuffer_value_context;
    mrb_fltk3_TextBuffer_context* textbuffer_context = NULL;
    textbuffer_value_context = mrb_iv_get(mrb, textbuffer, mrb_intern_lit(mrb, "context"));
    Data_Get_Struct(mrb, textbuffer_value_context, &fltk3_TextBuffer_type, textbuffer_context);
    ((fltk3::TextDisplay*) context->v)->buffer(textbuffer_context->v);
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));
  DEFINE_FIXNUM_PROP_READONLY(TextBuffer, TextBuffer, length);
  DEFINE_STR_PROP(TextBuffer, TextBuffer, text);
  ARENA_RESTORE;

  fltk3::register_images();
}

void
mrb_mruby_fltk3_gem_final(mrb_state* mrb)
{
}

}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
