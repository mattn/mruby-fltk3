#include "mrb_fltk3.h"
#include <mruby/error.h>
#include <fltk3/Browser.h>
#include <fltk3/Button.h>
#include <fltk3/CheckButton.h>
#include <fltk3/DoubleWindow.h>
#include <fltk3/FileChooser.h>
#include <fltk3/Group.h>
#include <fltk3/PackedGroup.h>
#include <fltk3/TabGroup.h>
#include <fltk3/TiledGroup.h>
#include <fltk3/ScrollGroup.h>
#include <fltk3/WizardGroup.h>
#include <fltk3/SingleWindow.h>
#include <fltk3/MenuWindow.h>
#include <fltk3/Input.h>
#include <fltk3/Output.h>
#include <fltk3/MultilineInput.h>
#include <fltk3/MultilineOutput.h>
#include <fltk3/IntInput.h>
#include <fltk3/FloatInput.h>
#include <fltk3/SecretInput.h>
#include <fltk3/FileInput.h>
#include <fltk3/LightButton.h>
#include <fltk3/MenuBar.h>
#include <fltk3/MenuButton.h>
#include <fltk3/Choice.h>
#include <fltk3/RadioButton.h>
#include <fltk3/RadioLightButton.h>
#include <fltk3/RadioRoundButton.h>
#include <fltk3/RepeatButton.h>
#include <fltk3/ReturnButton.h>
#include <fltk3/RoundButton.h>
#include <fltk3/SelectBrowser.h>
#include <fltk3/HoldBrowser.h>
#include <fltk3/MultiBrowser.h>
#include <fltk3/FileBrowser.h>
#include <fltk3/CheckBrowser.h>
#include <fltk3/SharedImage.h>
#include <fltk3/TextDisplay.h>
#include <fltk3/TextEditor.h>
#include <fltk3/ToggleButton.h>
#include <fltk3/ToggleLightButton.h>
#include <fltk3/ToggleRoundButton.h>
#include <fltk3/ValueOutput.h>
#include <fltk3/ValueInput.h>
#include <fltk3/Scrollbar.h>
#include <fltk3/ValueSlider.h>
#include <fltk3/Slider.h>
#include <fltk3/Counter.h>
#include <fltk3/Dial.h>
#include <fltk3/Roller.h>
#include <fltk3/Adjuster.h>
#include <fltk3/Spinner.h>
#include <fltk3/Progress.h>
#include <fltk3/Clock.h>
#include <fltk3/RoundClock.h>
#include <fltk3/Chart.h>
#include <fltk3/Positioner.h>
#include <fltk3/InputChoice.h>
#include <fltk3/ColorChooser.h>
#include <fltk3/HelpView.h>
#include <fltk3/Window.h>
#include <fltk3/ask.h>
#include <fltk3/run.h>

/*********************************************************
 * object wrapping
 *********************************************************/
#define DEFINE_TYPE(x) \
static void \
fltk3_ ## x ## _free(mrb_state *mrb, void *p) { \
  free(p); \
} \
const struct mrb_data_type \
fltk3_ ## x ## _type = { \
  "fltk3_" # x, fltk3_ ## x ## _free, \
}; \
\
mrb_fltk3_ ## x ## _context* \
mrb_fltk3_ ## x ## _context_get(mrb_state* mrb, mrb_value obj) \
{ \
  mrb_fltk3_ ## x ## _context* context = \
    (mrb_fltk3_ ## x ## _context*) mrb_data_get_ptr(mrb, obj, &fltk3_ ## x ## _type); \
  if (!context || !context->v) mrb_raise(mrb, E_RUNTIME_ERROR, "invalid " # x); \
  return context; \
} \
\
fltk3::x* \
mrb_fltk3_ ## x ## _ptr(mrb_state* mrb, mrb_value obj) \
{ \
  if (mrb_nil_p(obj)) return NULL; \
  return mrb_fltk3_ ## x ## _context_get(mrb, obj)->v; \
} \
\
static mrb_fltk3_ ## x ## _context* \
mrb_fltk3_ ## x ## _context_new(mrb_state* mrb, mrb_value instance, fltk3::x* v) \
{ \
  mrb_fltk3_ ## x ## _context* context = \
    (mrb_fltk3_ ## x ## _context*) malloc(sizeof(mrb_fltk3_ ## x ## _context)); \
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory"); \
  memset(context, 0, sizeof(mrb_fltk3_ ## x ## _context)); \
  context->instance = instance; \
  context->mrb = mrb; \
  context->v = v; \
  DATA_TYPE(instance) = &fltk3_ ## x ## _type; \
  DATA_PTR(instance) = context; \
  mrb_fltk3_register(mrb, v, instance); \
  return context; \
} \
\
mrb_value \
mrb_fltk3_ ## x ## _setup(mrb_state* mrb, mrb_value self, fltk3::x* v) \
{ \
  mrb_fltk3_ ## x ## _context_new(mrb, self, v); \
  return self; \
} \
\
mrb_value \
mrb_fltk3_ ## x ## _wrap(mrb_state* mrb, fltk3::x* v, const char* klass) \
{ \
  if (!v) return mrb_nil_value(); \
  mrb_value instance = mrb_fltk3_registered(mrb, v); \
  if (!mrb_nil_p(instance)) return instance; \
  instance = mrb_obj_value(Data_Wrap_Struct(mrb, mrb_fltk3_class(mrb, klass), &fltk3_ ## x ## _type, NULL)); \
  mrb_fltk3_ ## x ## _context_new(mrb, instance, v); \
  return instance; \
}

DEFINE_TYPE(Widget)
DEFINE_TYPE(Box)
DEFINE_TYPE(TextBuffer)
DEFINE_TYPE(Image)
DEFINE_TYPE(MenuItem)
DEFINE_TYPE(FileChooser)
DEFINE_TYPE(TreeItem)

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

void
mrb_fltk3_register(mrb_state* mrb, const void* ptr, mrb_value instance)
{
  mrb_hash_set(mrb, mrb_fltk3_registry(mrb), mrb_fltk3_registry_key(mrb, ptr), instance);
}

void
mrb_fltk3_unregister(mrb_state* mrb, const void* ptr)
{
  mrb_hash_delete_key(mrb, mrb_fltk3_registry(mrb), mrb_fltk3_registry_key(mrb, ptr));
}

mrb_value
mrb_fltk3_registered(mrb_state* mrb, const void* ptr)
{
  return mrb_hash_get(mrb, mrb_fltk3_registry(mrb), mrb_fltk3_registry_key(mrb, ptr));
}

struct RClass*
mrb_fltk3_class(mrb_state* mrb, const char* name)
{
  struct RClass* _class_fltk3 = mrb_module_get(mrb, "FLTK3");
  return mrb_class_get_under(mrb, _class_fltk3, name);
}

const char*
mrb_fltk3_widget_classname(fltk3::Widget* w)
{
  if (dynamic_cast<fltk3::DoubleWindow*>(w)) return "DoubleWindow";
  if (dynamic_cast<fltk3::SingleWindow*>(w)) return "SingleWindow";
  if (dynamic_cast<fltk3::MenuWindow*>(w)) return "MenuWindow";
  if (dynamic_cast<fltk3::Window*>(w)) return "Window";
  if (dynamic_cast<fltk3::TextEditor*>(w)) return "TextEditor";
  if (dynamic_cast<fltk3::TextDisplay*>(w)) return "TextDisplay";
  if (dynamic_cast<fltk3::FileBrowser*>(w)) return "FileBrowser";
  if (dynamic_cast<fltk3::SelectBrowser*>(w)) return "SelectBrowser";
  if (dynamic_cast<fltk3::HoldBrowser*>(w)) return "HoldBrowser";
  if (dynamic_cast<fltk3::MultiBrowser*>(w)) return "MultiBrowser";
  if (dynamic_cast<fltk3::Browser*>(w)) return "Browser";
  if (dynamic_cast<fltk3::CheckBrowser*>(w)) return "CheckBrowser";
  if (dynamic_cast<fltk3::Browser_*>(w)) return "Browser_";
  if (dynamic_cast<fltk3::PackedGroup*>(w)) return "PackedGroup";
  if (dynamic_cast<fltk3::TabGroup*>(w)) return "TabGroup";
  if (dynamic_cast<fltk3::TiledGroup*>(w)) return "TiledGroup";
  if (dynamic_cast<fltk3::ScrollGroup*>(w)) return "ScrollGroup";
  if (dynamic_cast<fltk3::WizardGroup*>(w)) return "WizardGroup";
  if (dynamic_cast<fltk3::Spinner*>(w)) return "Spinner";
  if (dynamic_cast<fltk3::Tree*>(w)) return "Tree";
  if (dynamic_cast<fltk3::InputChoice*>(w)) return "InputChoice";
  if (dynamic_cast<fltk3::ColorChooser*>(w)) return "ColorChooser";
  if (dynamic_cast<fltk3::HelpView*>(w)) return "HelpView";
  if (dynamic_cast<fltk3::Group*>(w)) return "Group";
  if (dynamic_cast<fltk3::MenuBar*>(w)) return "MenuBar";
  if (dynamic_cast<fltk3::MenuButton*>(w)) return "MenuButton";
  if (dynamic_cast<fltk3::Choice*>(w)) return "Choice";
  if (dynamic_cast<fltk3::Menu_*>(w)) return "Menu_";
  if (dynamic_cast<fltk3::RadioLightButton*>(w)) return "RadioLightButton";
  if (dynamic_cast<fltk3::RadioRoundButton*>(w)) return "RadioRoundButton";
  if (dynamic_cast<fltk3::RadioButton*>(w)) return "RadioButton";
  if (dynamic_cast<fltk3::ToggleLightButton*>(w)) return "ToggleLightButton";
  if (dynamic_cast<fltk3::ToggleRoundButton*>(w)) return "ToggleRoundButton";
  if (dynamic_cast<fltk3::ToggleButton*>(w)) return "ToggleButton";
  if (dynamic_cast<fltk3::CheckButton*>(w)) return "CheckButton";
  if (dynamic_cast<fltk3::LightButton*>(w)) return "LightButton";
  if (dynamic_cast<fltk3::RoundButton*>(w)) return "RoundButton";
  if (dynamic_cast<fltk3::RepeatButton*>(w)) return "RepeatButton";
  if (dynamic_cast<fltk3::ReturnButton*>(w)) return "ReturnButton";
  if (dynamic_cast<fltk3::Button*>(w)) return "Button";
  if (dynamic_cast<fltk3::FileInput*>(w)) return "FileInput";
  if (dynamic_cast<fltk3::SecretInput*>(w)) return "SecretInput";
  if (dynamic_cast<fltk3::FloatInput*>(w)) return "FloatInput";
  if (dynamic_cast<fltk3::IntInput*>(w)) return "IntInput";
  if (dynamic_cast<fltk3::MultilineOutput*>(w)) return "MultilineOutput";
  if (dynamic_cast<fltk3::MultilineInput*>(w)) return "MultilineInput";
  if (dynamic_cast<fltk3::Output*>(w)) return "Output";
  if (dynamic_cast<fltk3::Input*>(w)) return "Input";
  if (dynamic_cast<fltk3::ValueOutput*>(w)) return "ValueOutput";
  if (dynamic_cast<fltk3::ValueInput*>(w)) return "ValueInput";
  if (dynamic_cast<fltk3::Scrollbar*>(w)) return "Scrollbar";
  if (dynamic_cast<fltk3::ValueSlider*>(w)) return "ValueSlider";
  if (dynamic_cast<fltk3::Slider*>(w)) return "Slider";
  if (dynamic_cast<fltk3::Counter*>(w)) return "Counter";
  if (dynamic_cast<fltk3::Dial*>(w)) return "Dial";
  if (dynamic_cast<fltk3::Roller*>(w)) return "Roller";
  if (dynamic_cast<fltk3::Adjuster*>(w)) return "Adjuster";
  if (dynamic_cast<fltk3::Valuator*>(w)) return "Valuator";
  if (dynamic_cast<fltk3::Progress*>(w)) return "Progress";
  if (dynamic_cast<fltk3::RoundClock*>(w)) return "RoundClock";
  if (dynamic_cast<fltk3::Clock*>(w)) return "Clock";
  if (dynamic_cast<fltk3::ClockOutput*>(w)) return "ClockOutput";
  if (dynamic_cast<fltk3::Chart*>(w)) return "Chart";
  if (dynamic_cast<fltk3::Positioner*>(w)) return "Positioner";
  return "Widget";
}

mrb_state* mrb_fltk3_state = NULL;

static mrb_value
mrb_fltk3_pending_exception(mrb_state* mrb, mrb_value set)
{
  struct RClass* _class_fltk3 = mrb_module_get(mrb, "FLTK3");
  mrb_value exc = mrb_iv_get(mrb, mrb_obj_value(_class_fltk3), mrb_intern_lit(mrb, "__exception__"));
  mrb_iv_set(mrb, mrb_obj_value(_class_fltk3), mrb_intern_lit(mrb, "__exception__"), set);
  return exc;
}

static void
mrb_fltk3_stash_exception(mrb_state* mrb, mrb_value exc)
{
  mrb->exc = NULL;
  /* keep the first one */
  mrb_value pending = mrb_fltk3_pending_exception(mrb, exc);
  if (!mrb_nil_p(pending)) mrb_fltk3_pending_exception(mrb, pending);
}

typedef struct {
  mrb_value proc;
  mrb_value self;
  mrb_sym mid;
  mrb_int argc;
  const mrb_value* argv;
} mrb_fltk3_call_data;

static mrb_value
mrb_fltk3_call_body(mrb_state* mrb, void* p)
{
  mrb_fltk3_call_data* data = (mrb_fltk3_call_data*) p;
  if (data->mid) return mrb_funcall_argv(mrb, data->self, data->mid, data->argc, data->argv);
  return mrb_yield_argv(mrb, data->proc, data->argc, data->argv);
}

/* fltk3 calls back into Ruby while the VM is suspended inside FLTK3.run, so
 * an exception must not unwind through the native frames; catch it here. */
static mrb_value
mrb_fltk3_call_protected(mrb_state* mrb, mrb_fltk3_call_data* data)
{
  int ai = mrb_gc_arena_save(mrb);
  mrb_bool error = FALSE;
  mrb_value ret = mrb_protect_error(mrb, mrb_fltk3_call_body, data, &error);
  if (error) {
    mrb_fltk3_stash_exception(mrb, ret);
    ret = mrb_nil_value();
  }
  mrb_gc_arena_restore(mrb, ai);
  return ret;
}

mrb_value
mrb_fltk3_call(mrb_state* mrb, mrb_value proc, mrb_int argc, const mrb_value* argv)
{
  if (mrb_nil_p(proc)) return mrb_nil_value();
  mrb_fltk3_call_data data = { proc, mrb_nil_value(), 0, argc, argv };
  return mrb_fltk3_call_protected(mrb, &data);
}

mrb_value
mrb_fltk3_send(mrb_state* mrb, mrb_value self, const char* name, mrb_int argc, const mrb_value* argv)
{
  mrb_fltk3_call_data data = { mrb_nil_value(), self, mrb_intern_cstr(mrb, name), argc, argv };
  return mrb_fltk3_call_protected(mrb, &data);
}

void
mrb_fltk3_check_exception(mrb_state* mrb)
{
  mrb_value exc = mrb_fltk3_pending_exception(mrb, mrb_nil_value());
  if (!mrb_nil_p(exc)) mrb_exc_raise(mrb, exc);
}

void
mrb_fltk3_widget_forget(mrb_state* mrb, fltk3::Widget* w)
{
  if (!w) return;
  fltk3::Group* g = dynamic_cast<fltk3::Group*>(w);
  if (g) {
    for (int i = 0; i < g->children(); i++) {
      mrb_fltk3_widget_forget(mrb, g->child(i));
    }
  }
  mrb_value instance = mrb_fltk3_registered(mrb, w);
  if (!mrb_nil_p(instance)) {
    mrb_fltk3_Widget_context* context = (mrb_fltk3_Widget_context*) DATA_PTR(instance);
    if (context) context->v = NULL;
    mrb_fltk3_unregister(mrb, w);
  }
}

bool
mrb_fltk3_arg_check(const char* t, mrb_int argc, mrb_value* argv)
{
  mrb_int n, l = strlen(t);
  if (l != argc) return false;
  for (n = 0; n < l; n++) {
    if (t[n] == 'i' && !mrb_integer_p(argv[n])) return false;
    if (t[n] == 'd' && !mrb_float_p(argv[n])) return false;
    if (t[n] == 's' && !mrb_string_p(argv[n])) return false;
    if (t[n] == 'b' && !mrb_true_p(argv[n]) && !mrb_false_p(argv[n])) return false;
    if (t[n] == 'o' && mrb_nil_p(argv[n])) return false;
  }
  return true;
}

/*********************************************************
 * FLTK3::*
 *********************************************************/
/* Same loop as fltk3::run(), but stops to re-raise an exception raised in
 * a callback. */
static mrb_value
mrb_fltk3_run(mrb_state *mrb, mrb_value self)
{
  while (fltk3::first_window()) {
    fltk3::wait();
    mrb_fltk3_check_exception(mrb);
  }
  return mrb_fixnum_value(0);
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

/* set_font(face, name)  -> use the system font "name" for face
 * set_font(face, from)  -> make face an alias of another face */
static mrb_value
mrb_fltk3_set_font(mrb_state *mrb, mrb_value self)
{
  mrb_int face;
  mrb_value name;
  mrb_get_args(mrb, "io", &face, &name);
  if (mrb_string_p(name)) {
    /* fltk3 keeps the pointer, so the string must stay alive */
    mrb_value fonts = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "__fonts__"));
    if (mrb_nil_p(fonts)) {
      fonts = mrb_hash_new(mrb);
      mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "__fonts__"), fonts);
    }
    name = mrb_str_dup(mrb, name);
    mrb_hash_set(mrb, fonts, mrb_fixnum_value(face), name);
    fltk3::set_font((fltk3::Font) face, RSTRING_CSTR(mrb, name));
  } else {
    fltk3::set_font((fltk3::Font) face, (fltk3::Font) mrb_integer(name));
  }
  return mrb_nil_value();
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

extern "C"
{

void
mrb_mruby_fltk3_gem_init(mrb_state* mrb)
{
  ARENA_SAVE;
  mrb_fltk3_state = mrb;
  struct RClass* _class_fltk3 = mrb_define_module(mrb, "FLTK3");
  mrb_define_module_function(mrb, _class_fltk3, "run", mrb_fltk3_run, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, _class_fltk3, "alert", mrb_fltk3_alert, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "ask", mrb_fltk3_ask, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "choice", mrb_fltk3_choice, MRB_ARGS_REQ(3) | MRB_ARGS_OPT(1));
  mrb_define_module_function(mrb, _class_fltk3, "set_fonts", mrb_fltk3_set_fonts, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, _class_fltk3, "set_font", mrb_fltk3_set_font, MRB_ARGS_REQ(2));
  mrb_define_module_function(mrb, _class_fltk3, "font_name", mrb_fltk3_font_name, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  mrb_fltk3_constants_init(mrb, _class_fltk3);
  mrb_fltk3_app_init(mrb, _class_fltk3);
  mrb_fltk3_draw_init(mrb, _class_fltk3);
  mrb_fltk3_image_init(mrb, _class_fltk3);
  mrb_fltk3_box_init(mrb, _class_fltk3);
  mrb_fltk3_widget_init(mrb, _class_fltk3);
  mrb_fltk3_group_init(mrb, _class_fltk3);
  mrb_fltk3_button_init(mrb, _class_fltk3);
  mrb_fltk3_input_init(mrb, _class_fltk3);
  mrb_fltk3_valuator_init(mrb, _class_fltk3);
  mrb_fltk3_menu_init(mrb, _class_fltk3);
  mrb_fltk3_browser_init(mrb, _class_fltk3);
  mrb_fltk3_text_init(mrb, _class_fltk3);
  mrb_fltk3_misc_init(mrb, _class_fltk3);
  mrb_fltk3_tree_init(mrb, _class_fltk3);

  fltk3::register_images();
}

void
mrb_mruby_fltk3_gem_final(mrb_state* mrb)
{
}

}

/* vim:set et ts=2 sts=2 sw=0 tw=0: */
