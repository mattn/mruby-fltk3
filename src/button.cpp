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

void
mrb_fltk3_button_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_CLASS(Button, Widget);
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
