#include <memory.h>
#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <fltk3/DoubleWindow.h>
#include <fltk3/Window.h>
#include <fltk3/Box.h>
#include <fltk3/Button.h>
#include <fltk3/Input.h>
#include <fltk3/message.h>
#include <fltk3/ask.h>
#include <fltk3/run.h>
#include <stdio.h>

#if 1
#define ARENA_SAVE \
  int ai = mrb_gc_arena_save(mrb); \
  if (ai == MRB_ARENA_SIZE) { \
    mrb_raise(mrb, E_RUNTIME_ERROR, "arena overflow"); \
  }
#define ARENA_RESTORE \
  mrb_gc_arena_restore(mrb, ai);
#else
#define ARENA_SAVE
#define ARENA_RESTORE
#endif

typedef struct {
  fltk3::Widget* w;
  mrb_value instance;
  mrb_state* mrb;
} mrb_fltk3_context;

static void
fltk3_context_free(mrb_state *mrb, void *p) {
  free(p);
}

static const struct mrb_data_type fltk3_context_type = {
  "fltk3_context", fltk3_context_free,
};

static bool
arg_check(const char* t, int argc, mrb_value* argv)
{
  int n, l = strlen(t);
  for (n = 0; n < l; n++) {
    if (n >= argc) return false;
    if (t[n] == 'i' && mrb_type(argv[n]) != MRB_TT_FIXNUM) return false;
    if (t[n] == 'd' && mrb_type(argv[n]) != MRB_TT_FLOAT) return false;
    if (t[n] == 's' && mrb_type(argv[n]) != MRB_TT_STRING) return false;
    if (t[n] == 'b' && mrb_type(argv[n]) != MRB_TT_TRUE && mrb_type(argv[n]) != MRB_TT_FALSE) return false;
  }
  return true;
}

/*********************************************************
 * FLTK3::Widget
 *********************************************************/
static mrb_value
mrb_fltk3_widget_w_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_fixnum_value(context->w->w());
}

static mrb_value
mrb_fltk3_widget_w_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value w;
  mrb_get_args(mrb, "i", &w);
  context->w->w(mrb_fixnum(w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_h_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_fixnum_value(context->w->h());
}

static mrb_value
mrb_fltk3_widget_h_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value h;
  mrb_get_args(mrb, "i", &h);
  context->w->h(mrb_fixnum(h));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_x_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_fixnum_value(context->w->x());
}

static mrb_value
mrb_fltk3_widget_x_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value x;
  mrb_get_args(mrb, "i", &x);
  context->w->x(mrb_fixnum(x));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_y_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_fixnum_value(context->w->y());
}

static mrb_value
mrb_fltk3_widget_y_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value y;
  mrb_get_args(mrb, "i", &y);
  context->w->y(mrb_fixnum(y));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_label_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_str_new_cstr(mrb, context->w->label());
}

static mrb_value
mrb_fltk3_widget_label_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value label;
  mrb_get_args(mrb, "S", &label);
  context->w->label(RSTRING_PTR(label));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_labelfont_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_fixnum_value(context->w->labelfont());
}

static mrb_value
mrb_fltk3_widget_labelfont_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value labelfont;
  mrb_get_args(mrb, "i", &labelfont);
  context->w->labelfont(mrb_fixnum(labelfont));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_box_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  struct RClass* _class_fltk3 = mrb_class_get(mrb, "FLTK3");
  struct RClass* _class_fltk3_Box = mrb_class_ptr(mrb_const_get(mrb, mrb_obj_value(_class_fltk3), mrb_intern(mrb, "Box")));
  mrb_value args[1];
  args[0] = mrb_obj_value(
    Data_Wrap_Struct(mrb, mrb->object_class,
    &fltk3_context_type, (void*) context->w->box()));
  return mrb_class_new_instance(mrb, 1, args, _class_fltk3_Box);
}

static mrb_value
mrb_fltk3_widget_box_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value box;
  mrb_get_args(mrb, "o", &box);
  mrb_value box_value_context;
  mrb_fltk3_context* box_context = NULL;
  box_value_context = mrb_iv_get(mrb, box, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, box_value_context, &fltk3_context_type, box_context);
  context->w->box((fltk3::Box*) box_context->w);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_show(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  context->w->show();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_hide(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  context->w->hide();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_visible(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return context->w->visible() ? mrb_true_value() : mrb_false_value();
}

static void
_mrb_fltk3_widget_callback(fltk3::Widget* w, void* v)
{
  mrb_value args[2];
  mrb_fltk3_context* context = (mrb_fltk3_context*) v;
  mrb_state* mrb = context->mrb;
  mrb_value proc = mrb_iv_get(mrb, context->instance, mrb_intern(mrb, "callback"));
  mrb_value value = mrb_iv_get(mrb, context->instance, mrb_intern(mrb, "value"));
  args[0] = context->instance;
  args[1] = value;
  mrb_yield_argv(mrb, proc, 2, args);
}

static mrb_value
mrb_fltk3_widget_callback(mrb_state *mrb, mrb_value self)
{
  mrb_value b = mrb_nil_value();
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value v = mrb_nil_value();
  mrb_get_args(mrb, "&|o", &b, &v);
  if (!mrb_nil_p(b)) {
    mrb_iv_set(mrb, self, mrb_intern(mrb, "callback"), b);
    mrb_iv_set(mrb, self, mrb_intern(mrb, "value"), v);
    context->w->callback(_mrb_fltk3_widget_callback, context);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_show(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  ((fltk3::Window*) context->w)->show(0, NULL);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::Input
 *********************************************************/
static mrb_value
mrb_fltk3_input_value_get(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  return mrb_str_new_cstr(mrb, ((fltk3::Input*) context->w)->value());
}

static mrb_value
mrb_fltk3_input_value_set(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  mrb_value text;
  mrb_get_args(mrb, "S", &text);
  ((fltk3::Input*) context->w)->value(RSTRING_PTR(text));
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::Group
 *********************************************************/
static mrb_value
mrb_fltk3_group_begin(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  ((fltk3::Group*)context->w)->begin();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_group_end(mrb_state *mrb, mrb_value self)
{
  mrb_value value_context;
  mrb_fltk3_context* context = NULL;
  value_context = mrb_iv_get(mrb, self, mrb_intern(mrb, "context"));
  Data_Get_Struct(mrb, value_context, &fltk3_context_type, context);
  ((fltk3::Group*)context->w)->end();
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
  fltk3::alert(RSTRING_PTR(s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_ask(mrb_state *mrb, mrb_value self)
{
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  return fltk3::ask(RSTRING_PTR(s)) ? mrb_true_value() : mrb_false_value();
}

#define DECLARE_WIDGET(x)                                               \
static mrb_value                                                        \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                \
{                                                                       \
  mrb_value *argv;                                                      \
  int argc;                                                             \
  mrb_get_args(mrb, "*", &argv, &argc);                                 \
  mrb_fltk3_context* context =                                          \
    (mrb_fltk3_context*) malloc(sizeof(mrb_fltk3_context));             \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");  \
  memset(context, 0, sizeof(mrb_fltk3_context));                        \
  context->instance = self;                                             \
  context->mrb = mrb;                                                   \
  if (arg_check("iiiis", argc, argv)) {                                 \
    context->w = (fltk3::Widget*) new fltk3::x (                        \
      (int) mrb_fixnum(argv[0]),                                        \
      (int) mrb_fixnum(argv[1]),                                        \
      (int) mrb_fixnum(argv[2]),                                        \
      (int) mrb_fixnum(argv[3]),                                        \
      RSTRING_PTR(argv[4]));                                            \
  } else {                                                              \
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");               \
  }                                                                     \
  mrb_iv_set(mrb, self, mrb_intern(mrb, "context"), mrb_obj_value(      \
    Data_Wrap_Struct(mrb, mrb->object_class,                            \
    &fltk3_context_type, (void*) context)));                            \
  return self;                                                          \
}

#define DECLARE_WINDOW(x)                                               \
static mrb_value                                                        \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                \
{                                                                       \
  mrb_value *argv;                                                      \
  int argc;                                                             \
  mrb_get_args(mrb, "*", &argv, &argc);                                 \
  mrb_fltk3_context* context =                                          \
    (mrb_fltk3_context*) malloc(sizeof(mrb_fltk3_context));             \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");  \
  memset(context, 0, sizeof(mrb_fltk3_context));                        \
  context->instance = self;                                             \
  context->mrb = mrb;                                                   \
  if (arg_check("iis", argc, argv)) {                                   \
    context->w = (fltk3::Widget*) new fltk3::x (                        \
      (int) mrb_fixnum(argv[0]),                                        \
      (int) mrb_fixnum(argv[1]),                                        \
      RSTRING_PTR(argv[2]));                                            \
  } else if (arg_check("iiiis", argc, argv)) {                          \
    context->w = (fltk3::Widget*) new fltk3::x (                        \
      (int) mrb_fixnum(argv[0]),                                        \
      (int) mrb_fixnum(argv[1]),                                        \
      (int) mrb_fixnum(argv[2]),                                        \
      (int) mrb_fixnum(argv[3]),                                        \
      RSTRING_PTR(argv[4]));                                            \
  } else {                                                              \
    mrb_raise(mrb, E_ARGUMENT_ERROR, "invalid argument");               \
  }                                                                     \
  mrb_iv_set(mrb, self, mrb_intern(mrb, "context"), mrb_obj_value(      \
    Data_Wrap_Struct(mrb, mrb->object_class,                            \
    &fltk3_context_type, (void*) context)));                            \
  return self;                                                          \
}

static mrb_value
mrb_fltk3_Box_init(mrb_state *mrb, mrb_value self)
{
  mrb_value arg = mrb_nil_value();
  mrb_get_args(mrb, "|o", &arg);
  if (!mrb_nil_p(arg)) {
    if (strcmp(mrb_obj_classname(mrb, arg), "fltk3_context"))
      mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc fltk3::Box");
    mrb_iv_set(mrb, self, mrb_intern(mrb, "context"), arg);
  }
  return self;
}

#define DECLARE_BOX(x)                                                  \
static mrb_value                                                        \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                \
{                                                                       \
  mrb_value arg = mrb_nil_value();                                      \
  int argc;                                                             \
  mrb_get_args(mrb, "|S", &arg);                                        \
  mrb_fltk3_context* context =                                          \
    (mrb_fltk3_context*) malloc(sizeof(mrb_fltk3_context));             \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");  \
  memset(context, 0, sizeof(mrb_fltk3_context));                        \
  context->instance = self;                                             \
  context->mrb = mrb;                                                   \
  context->w = (fltk3::Widget*) new fltk3::x (                          \
    mrb_nil_p(arg) ? NULL : RSTRING_PTR(arg));                          \
  mrb_iv_set(mrb, self, mrb_intern(mrb, "context"), mrb_obj_value(      \
    Data_Wrap_Struct(mrb, mrb->object_class,                            \
    &fltk3_context_type, (void*) context)));                            \
  return self;                                                          \
}

DECLARE_WIDGET(Widget)
DECLARE_WINDOW(DoubleWindow)
DECLARE_WINDOW(Window)
DECLARE_WIDGET(Button)
DECLARE_WIDGET(Input)

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
  mrb_define_method(mrb, _class_fltk3_ ## x, "begin", mrb_fltk3_group_begin, ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "end", mrb_fltk3_group_end, ARGS_NONE()); \
  ARENA_RESTORE;

#define INHERIT_INPUT_VALUE(x) \
  mrb_define_method(mrb, _class_fltk3_ ## x, "value", mrb_fltk3_input_value_get, ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "value=", mrb_fltk3_input_value_set, ARGS_NONE()); \
  ARENA_RESTORE;

#define DEFINE_WIDGET(x) \
  struct RClass* _class_fltk3_ ## x = mrb_define_class_under(mrb, _class_fltk3, # x, _class_fltk3_Widget); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "initialize", mrb_fltk3_ ## x ## _init, ARGS_ANY()); \
  ARENA_RESTORE;

#define DEFINE_WINDOW(x) \
  struct RClass* _class_fltk3_ ## x = mrb_define_class_under(mrb, _class_fltk3, # x, _class_fltk3_Window); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "initialize", mrb_fltk3_ ## x ## _init, ARGS_ANY()); \
  ARENA_RESTORE;

#define DEFINE_BOX(x) \
  struct RClass* _class_fltk3_ ## x = mrb_define_class_under(mrb, _class_fltk3, # x, _class_fltk3_Box); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "initialize", mrb_fltk3_ ## x ## _init, ARGS_ANY()); \
  ARENA_RESTORE;

void
mrb_mruby_fltk3_gem_init(mrb_state* mrb)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3 = mrb_define_module(mrb, "FLTK3");
  mrb_define_module_function(mrb, _class_fltk3, "run", mrb_fltk3_run, ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "alert", mrb_fltk3_alert, ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "ask", mrb_fltk3_ask, ARGS_REQ(1));
  ARENA_RESTORE;

  struct RClass* _class_fltk3_Widget = mrb_define_class_under(mrb, _class_fltk3, "Widget", mrb->object_class);
  mrb_define_method(mrb, _class_fltk3_Widget, "initialize", mrb_fltk3_Widget_init, ARGS_ANY());
  mrb_define_method(mrb, _class_fltk3_Widget, "show", mrb_fltk3_widget_show, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "x", mrb_fltk3_widget_x_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "x=", mrb_fltk3_widget_x_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "y", mrb_fltk3_widget_y_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "y=", mrb_fltk3_widget_y_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "w", mrb_fltk3_widget_w_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "w=", mrb_fltk3_widget_w_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "h", mrb_fltk3_widget_h_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "h=", mrb_fltk3_widget_h_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "label", mrb_fltk3_widget_label_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "label=", mrb_fltk3_widget_label_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "labelfont", mrb_fltk3_widget_labelfont_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "labelfont=", mrb_fltk3_widget_labelfont_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "box", mrb_fltk3_widget_box_get, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "box=", mrb_fltk3_widget_box_set, ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "hide", mrb_fltk3_widget_hide, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "visible", mrb_fltk3_widget_visible, ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "callback", mrb_fltk3_widget_callback, ARGS_OPT(1));
  ARENA_RESTORE;

  DEFINE_WIDGET(Button);
  DEFINE_WIDGET(Input);
  INHERIT_INPUT_VALUE(Input);

  struct RClass* _class_fltk3_Window = mrb_define_class_under(mrb, _class_fltk3, "Window", _class_fltk3_Widget);
  mrb_define_method(mrb, _class_fltk3_Window, "initialize", mrb_fltk3_Window_init, ARGS_ANY());
  mrb_define_method(mrb, _class_fltk3_Window, "show", mrb_fltk3_window_show, ARGS_OPT(1));
  INHERIT_GROUP(Window);

  DEFINE_WINDOW(DoubleWindow);

  struct RClass* _class_fltk3_Box = mrb_define_class_under(mrb, _class_fltk3, "Box", _class_fltk3_Widget);
  mrb_define_method(mrb, _class_fltk3_Box, "initialize", mrb_fltk3_Box_init, ARGS_ANY());
  ARENA_RESTORE;

  DEFINE_BOX(NoBox);
  DEFINE_BOX(FlatBox);
  DEFINE_BOX(UpBox);
  DEFINE_BOX(DownBox);
  DEFINE_BOX(ThinUpBox);
  DEFINE_BOX(ThinDownBox);
  DEFINE_BOX(EngravedBox);
  DEFINE_BOX(EmbossedBox);
  DEFINE_BOX(BorderBox);
  DEFINE_BOX(ShadowBox);
  DEFINE_BOX(RoundedBox);
  DEFINE_BOX(RShadowBox);
  DEFINE_BOX(RFlatBox);
  DEFINE_BOX(RoundUpBox);
  DEFINE_BOX(RoundDownBox);
  DEFINE_BOX(DiamondUpBox);
  DEFINE_BOX(DiamondDownBox);
  DEFINE_BOX(OvalBox);
  DEFINE_BOX(OShadowBox);
  DEFINE_BOX(OFlatBox);
  DEFINE_BOX(PlasticUpBox);
  DEFINE_BOX(PlasticDownBox);
  DEFINE_BOX(PlasticThinUpBox);
  DEFINE_BOX(PlasticThinDownBox);
  DEFINE_BOX(PlasticRoundUpBox);
  DEFINE_BOX(PlasticRoundDownBox);
  DEFINE_BOX(ClassicUpBox);
  DEFINE_BOX(ClassicDownBox);
  DEFINE_BOX(ClassicThinUpBox);
  DEFINE_BOX(ClassicThinDownBox);
  DEFINE_BOX(ClassicRoundUpBox);
  DEFINE_BOX(ClassicRoundDownBox);
  DEFINE_BOX(BorderFrame);
  DEFINE_BOX(UpFrame);
  DEFINE_BOX(DownFrame);
  DEFINE_BOX(ThinUpFrame);
  DEFINE_BOX(ThinDownFrame);
  DEFINE_BOX(EngravedFrame);
  DEFINE_BOX(EmbossedFrame);
  DEFINE_BOX(ShadowFrame);
  DEFINE_BOX(RoundedFrame);
  DEFINE_BOX(OvalFrame);
  DEFINE_BOX(PlasticUpFrame);
  DEFINE_BOX(PlasticDownFrame);
  DEFINE_BOX(ClassicUpFrame);
  DEFINE_BOX(ClassicDownFrame);
  DEFINE_BOX(ClassicThinUpFrame);
  DEFINE_BOX(ClassicThinDownFrame);
}

}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
