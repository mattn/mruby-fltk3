#include "mrb_fltk3.h"
#include <fltk3/Browser.h>
#include <fltk3/Button.h>
#include <fltk3/CheckButton.h>
#include <fltk3/DoubleWindow.h>
#include <fltk3/FileChooser.h>
#include <fltk3/Group.h>
#include <fltk3/Input.h>
#include <fltk3/LightButton.h>
#include <fltk3/MenuBar.h>
#include <fltk3/MenuButton.h>
#include <fltk3/RadioButton.h>
#include <fltk3/RadioLightButton.h>
#include <fltk3/RadioRoundButton.h>
#include <fltk3/RepeatButton.h>
#include <fltk3/ReturnButton.h>
#include <fltk3/RoundButton.h>
#include <fltk3/SelectBrowser.h>
#include <fltk3/SharedImage.h>
#include <fltk3/TextDisplay.h>
#include <fltk3/TextEditor.h>
#include <fltk3/ToggleButton.h>
#include <fltk3/ToggleLightButton.h>
#include <fltk3/ToggleRoundButton.h>
#include <fltk3/ValueOutput.h>
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
  if (dynamic_cast<fltk3::Window*>(w)) return "Window";
  if (dynamic_cast<fltk3::TextEditor*>(w)) return "TextEditor";
  if (dynamic_cast<fltk3::TextDisplay*>(w)) return "TextDisplay";
  if (dynamic_cast<fltk3::SelectBrowser*>(w)) return "SelectBrowser";
  if (dynamic_cast<fltk3::Browser*>(w)) return "Browser";
  if (dynamic_cast<fltk3::Group*>(w)) return "Group";
  if (dynamic_cast<fltk3::MenuBar*>(w)) return "MenuBar";
  if (dynamic_cast<fltk3::MenuButton*>(w)) return "MenuButton";
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
  if (dynamic_cast<fltk3::Input*>(w)) return "Input";
  if (dynamic_cast<fltk3::ValueOutput*>(w)) return "ValueOutput";
  return "Widget";
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

static mrb_value
mrb_fltk3_file_chooser(mrb_state* mrb, mrb_value self)
{
  mrb_value message, pattern;
  mrb_get_args(mrb, "SS", &message, &pattern);
  const char *fname = fltk3::file_chooser(RSTRING_CSTR(mrb, message), RSTRING_CSTR(mrb, pattern), NULL);
  if (fname) {
    return mrb_str_new_cstr(mrb, fname);
  }
  return mrb_nil_value();
}

extern "C"
{

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
  mrb_define_module_function(mrb, _class_fltk3, "file_chooser", mrb_fltk3_file_chooser, MRB_ARGS_REQ(2));
  ARENA_RESTORE;

  mrb_fltk3_constants_init(mrb, _class_fltk3);
  mrb_fltk3_image_init(mrb, _class_fltk3);
  mrb_fltk3_box_init(mrb, _class_fltk3);
  mrb_fltk3_widget_init(mrb, _class_fltk3);
  mrb_fltk3_group_init(mrb, _class_fltk3);
  mrb_fltk3_button_init(mrb, _class_fltk3);
  mrb_fltk3_input_init(mrb, _class_fltk3);
  mrb_fltk3_menu_init(mrb, _class_fltk3);
  mrb_fltk3_browser_init(mrb, _class_fltk3);
  mrb_fltk3_text_init(mrb, _class_fltk3);

  fltk3::register_images();
}

void
mrb_mruby_fltk3_gem_final(mrb_state* mrb)
{
}

}

/* vim:set et ts=2 sts=2 sw=0 tw=0: */
