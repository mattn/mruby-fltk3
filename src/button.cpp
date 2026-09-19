#include "mrb_fltk3.h"
#include <fltk3/Button.h>
#include <fltk3/CheckButton.h>
#include <fltk3/LightButton.h>
#include <fltk3/RadioButton.h>
#include <fltk3/RadioLightButton.h>
#include <fltk3/RadioRoundButton.h>
#include <fltk3/RepeatButton.h>
#include <fltk3/ReturnButton.h>
#include <fltk3/RoundButton.h>
#include <fltk3/ToggleButton.h>
#include <fltk3/ToggleLightButton.h>
#include <fltk3/ToggleRoundButton.h>

/*********************************************************
 * FLTK3::Button
 *********************************************************/
DECLARE_WIDGET(Button)
DECLARE_WIDGET(CheckButton)
DECLARE_WIDGET(LightButton)
DECLARE_WIDGET(RadioButton)
DECLARE_WIDGET(RadioLightButton)
DECLARE_WIDGET(RadioRoundButton)
DECLARE_WIDGET(RepeatButton)
DECLARE_WIDGET(ReturnButton)
DECLARE_WIDGET(RoundButton)
DECLARE_WIDGET(ToggleButton)
DECLARE_WIDGET(ToggleLightButton)
DECLARE_WIDGET(ToggleRoundButton)

static mrb_value
mrb_fltk3_button_value_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_bool_value(((fltk3::Button*) context->v)->value() != 0);
}

static mrb_value
mrb_fltk3_button_value_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  ((fltk3::Button*) context->v)->value(mrb_integer_p(v) ? (mrb_integer(v) != 0) : mrb_test(v));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_button_down_box_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Box_wrap(mrb, ((fltk3::Button*) context->v)->down_box(), "Box");
}

static mrb_value
mrb_fltk3_button_down_box_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value box;
  mrb_get_args(mrb, "o", &box);
  ((fltk3::Button*) context->v)->down_box(mrb_fltk3_Box_ptr(mrb, box));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_button_shortcut_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "o", &s);
  if (mrb_string_p(s)) {
    ((fltk3::Button*) context->v)->shortcut(RSTRING_CSTR(mrb, s));
  } else {
    ((fltk3::Button*) context->v)->shortcut((unsigned int) mrb_integer(s));
  }
  return mrb_nil_value();
}

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

void
mrb_fltk3_button_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_INT_CONST(NORMAL_BUTTON);
  DEFINE_INT_CONST(TOGGLE_BUTTON);
  DEFINE_INT_CONST(RADIO_BUTTON);
  DEFINE_INT_CONST(HIDDEN_BUTTON);

  DEFINE_CLASS(Button, Widget);
  mrb_define_method(mrb, _class_fltk3_Button, "value", mrb_fltk3_button_value_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Button, "value?", mrb_fltk3_button_value_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Button, "value=", mrb_fltk3_button_value_set, MRB_ARGS_REQ(1));
  DEFINE_VOID_METHOD(Button, Widget, set);
  DEFINE_VOID_METHOD(Button, Widget, clear);
  DEFINE_VOID_METHOD(Button, Widget, setonly);
  DEFINE_INT_GETTER(Button, Widget, shortcut);
  mrb_define_method(mrb, _class_fltk3_Button, "shortcut=", mrb_fltk3_button_shortcut_set, MRB_ARGS_REQ(1));
  DEFINE_INT_PROP(Button, Widget, down_color);
  mrb_define_method(mrb, _class_fltk3_Button, "down_box", mrb_fltk3_button_down_box_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Button, "down_box=", mrb_fltk3_button_down_box_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(CheckButton, Button);
  DEFINE_CLASS(LightButton, Button);
  DEFINE_CLASS(RadioButton, Button);
  DEFINE_CLASS(RadioLightButton, Button);
  DEFINE_CLASS(RadioRoundButton, Button);
  DEFINE_CLASS(RepeatButton, Button);
  DEFINE_CLASS(ReturnButton, Button);
  DEFINE_CLASS(RoundButton, Button);
  DEFINE_CLASS(ToggleButton, Button);
  DEFINE_CLASS(ToggleLightButton, Button);
  DEFINE_CLASS(ToggleRoundButton, Button);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
