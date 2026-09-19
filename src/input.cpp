#include "mrb_fltk3.h"
#include <fltk3/Input.h>

/*********************************************************
 * FLTK3::Input
 *********************************************************/
DECLARE_WIDGET(Input)

void
mrb_fltk3_input_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_CLASS(Input, Widget);
  DEFINE_STR_PROP(Input, Widget, value);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
