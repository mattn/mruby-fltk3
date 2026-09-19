#include "mrb_fltk3.h"
#include <fltk3/Valuator.h>
#include <fltk3/Slider.h>
#include <fltk3/FillSlider.h>
#include <fltk3/HorSlider.h>
#include <fltk3/HorFillSlider.h>
#include <fltk3/NiceSlider.h>
#include <fltk3/HorNiceSlider.h>
#include <fltk3/ValueSlider.h>
#include <fltk3/HorValueSlider.h>
#include <fltk3/Scrollbar.h>
#include <fltk3/Counter.h>
#include <fltk3/SimpleCounter.h>
#include <fltk3/Dial.h>
#include <fltk3/FillDial.h>
#include <fltk3/LineDial.h>
#include <fltk3/Roller.h>
#include <fltk3/Adjuster.h>
#include <fltk3/ValueInput.h>
#include <fltk3/ValueOutput.h>
#include <fltk3/Spinner.h>
#include <fltk3/Progress.h>

/*********************************************************
 * FLTK3::Valuator and friends
 *********************************************************/
static mrb_value
mrb_fltk3_Valuator_init(mrb_state* mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "can't instantiate fltk3::Valuator directly");
  return self;
}

DECLARE_WIDGET(Slider)
DECLARE_WIDGET(FillSlider)
DECLARE_WIDGET(HorSlider)
DECLARE_WIDGET(HorFillSlider)
DECLARE_WIDGET(NiceSlider)
DECLARE_WIDGET(HorNiceSlider)
DECLARE_WIDGET(ValueSlider)
DECLARE_WIDGET(HorValueSlider)
DECLARE_WIDGET(Scrollbar)
DECLARE_WIDGET(Counter)
DECLARE_WIDGET(SimpleCounter)
DECLARE_WIDGET(Dial)
DECLARE_WIDGET(FillDial)
DECLARE_WIDGET(LineDial)
DECLARE_WIDGET(Roller)
DECLARE_WIDGET(Adjuster)
DECLARE_WIDGET(ValueInput)
DECLARE_WIDGET(ValueOutput)
DECLARE_WIDGET(Spinner)
DECLARE_WIDGET(Progress)

static mrb_value
mrb_fltk3_valuator_bounds(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float a, b;
  mrb_get_args(mrb, "ff", &a, &b);
  ((fltk3::Valuator*) context->v)->bounds(a, b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_valuator_range(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float a, b;
  mrb_get_args(mrb, "ff", &a, &b);
  ((fltk3::Valuator*) context->v)->range(a, b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_valuator_step(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float a;
  mrb_int b;
  mrb_int argc = mrb_get_args(mrb, "|fi", &a, &b);
  fltk3::Valuator* v = (fltk3::Valuator*) context->v;
  if (argc == 0) return mrb_float_value(mrb, v->step());
  if (argc == 1) v->step(a); else v->step(a, (int) b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_valuator_precision_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int p;
  mrb_get_args(mrb, "i", &p);
  ((fltk3::Valuator*) context->v)->precision((int) p);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_valuator_round(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float f;
  mrb_get_args(mrb, "f", &f);
  return mrb_float_value(mrb, ((fltk3::Valuator*) context->v)->round(f));
}

static mrb_value
mrb_fltk3_valuator_clamp(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float f;
  mrb_get_args(mrb, "f", &f);
  return mrb_float_value(mrb, ((fltk3::Valuator*) context->v)->clamp(f));
}

static mrb_value
mrb_fltk3_valuator_increment(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float f;
  mrb_int n;
  mrb_get_args(mrb, "fi", &f, &n);
  return mrb_float_value(mrb, ((fltk3::Valuator*) context->v)->increment(f, (int) n));
}

static mrb_value
mrb_fltk3_slider_scrollvalue(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int pos, size, first, total;
  mrb_get_args(mrb, "iiii", &pos, &size, &first, &total);
  return mrb_fixnum_value(((fltk3::Slider*) context->v)->scrollvalue(pos, size, first, total));
}

static mrb_value
mrb_fltk3_slider_slider_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Box_wrap(mrb, ((fltk3::Slider*) context->v)->slider(), "Box");
}

static mrb_value
mrb_fltk3_slider_slider_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value box;
  mrb_get_args(mrb, "o", &box);
  ((fltk3::Slider*) context->v)->slider(mrb_fltk3_Box_ptr(mrb, box));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_scrollbar_value(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int pos, size, first, total;
  mrb_int argc = mrb_get_args(mrb, "|iiii", &pos, &size, &first, &total);
  fltk3::Scrollbar* s = (fltk3::Scrollbar*) context->v;
  if (argc == 0) return mrb_fixnum_value(s->value());
  if (argc == 1) return mrb_fixnum_value(s->value((int) pos));
  if (argc != 4) mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  return mrb_fixnum_value(s->value(pos, size, first, total));
}

static mrb_value
mrb_fltk3_dial_angles(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int a, b;
  mrb_get_args(mrb, "ii", &a, &b);
  ((fltk3::Dial*) context->v)->angles((short) a, (short) b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_counter_lstep_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float f;
  mrb_get_args(mrb, "f", &f);
  ((fltk3::Counter*) context->v)->lstep(f);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_spinner_range(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_float a, b;
  mrb_get_args(mrb, "ff", &a, &b);
  ((fltk3::Spinner*) context->v)->range(a, b);
  return mrb_nil_value();
}

#define INHERIT_VALUATOR(x) \
  DEFINE_FLOAT_PROP(x, Widget, value); \
  DEFINE_FLOAT_PROP(x, Widget, minimum); \
  DEFINE_FLOAT_PROP(x, Widget, maximum); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "bounds", mrb_fltk3_valuator_bounds, MRB_ARGS_REQ(2)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "range", mrb_fltk3_valuator_range, MRB_ARGS_REQ(2)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "step", mrb_fltk3_valuator_step, MRB_ARGS_OPT(2)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "step=", mrb_fltk3_valuator_step, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "precision=", mrb_fltk3_valuator_precision_set, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "round", mrb_fltk3_valuator_round, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "clamp", mrb_fltk3_valuator_clamp, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "increment", mrb_fltk3_valuator_increment, MRB_ARGS_REQ(2)); \
  ARENA_RESTORE;

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

void
mrb_fltk3_valuator_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  DEFINE_INT_CONST(VERTICAL);
  DEFINE_INT_CONST(HORIZONTAL);
  DEFINE_INT_CONST(VERT_SLIDER);
  DEFINE_INT_CONST(HOR_SLIDER);
  DEFINE_INT_CONST(VERT_FILL_SLIDER);
  DEFINE_INT_CONST(HOR_FILL_SLIDER);
  DEFINE_INT_CONST(VERT_NICE_SLIDER);
  DEFINE_INT_CONST(HOR_NICE_SLIDER);
  DEFINE_INT_CONST(NORMAL_DIAL);
  DEFINE_INT_CONST(LINE_DIAL);
  DEFINE_INT_CONST(FILL_DIAL);
  DEFINE_INT_CONST(NORMAL_COUNTER);
  DEFINE_INT_CONST(SIMPLE_COUNTER);

  DEFINE_CLASS(Valuator, Widget);
  INHERIT_VALUATOR(Valuator);

  DEFINE_CLASS(Slider, Valuator);
  DEFINE_FLOAT_PROP(Slider, Widget, slider_size);
  mrb_define_method(mrb, _class_fltk3_Slider, "scrollvalue", mrb_fltk3_slider_scrollvalue, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, _class_fltk3_Slider, "slider", mrb_fltk3_slider_slider_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Slider, "slider=", mrb_fltk3_slider_slider_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;
  DEFINE_CLASS(FillSlider, Slider);
  DEFINE_CLASS(HorSlider, Slider);
  DEFINE_CLASS(HorFillSlider, Slider);
  DEFINE_CLASS(NiceSlider, Slider);
  DEFINE_CLASS(HorNiceSlider, Slider);
  DEFINE_CLASS(ValueSlider, Slider);
  INHERIT_TEXT_PROPS(ValueSlider);
  ARENA_RESTORE;
  DEFINE_CLASS(HorValueSlider, ValueSlider);

  DEFINE_CLASS(Scrollbar, Slider);
  DEFINE_INT_PROP(Scrollbar, Widget, linesize);
  mrb_define_method(mrb, _class_fltk3_Scrollbar, "value", mrb_fltk3_scrollbar_value, MRB_ARGS_OPT(4));
  ARENA_RESTORE;

  DEFINE_CLASS(Counter, Valuator);
  INHERIT_TEXT_PROPS(Counter);
  mrb_define_method(mrb, _class_fltk3_Counter, "lstep=", mrb_fltk3_counter_lstep_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;
  DEFINE_CLASS(SimpleCounter, Counter);

  DEFINE_CLASS(Dial, Valuator);
  DEFINE_INT_PROP(Dial, Widget, angle1);
  DEFINE_INT_PROP(Dial, Widget, angle2);
  mrb_define_method(mrb, _class_fltk3_Dial, "angles", mrb_fltk3_dial_angles, MRB_ARGS_REQ(2));
  ARENA_RESTORE;
  DEFINE_CLASS(FillDial, Dial);
  DEFINE_CLASS(LineDial, Dial);

  DEFINE_CLASS(Roller, Valuator);
  DEFINE_CLASS(Adjuster, Valuator);
  DEFINE_BOOL_PROP(Adjuster, Widget, soft);
  ARENA_RESTORE;

  DEFINE_CLASS(ValueInput, Valuator);
  INHERIT_TEXT_PROPS(ValueInput);
  DEFINE_BOOL_PROP(ValueInput, Widget, soft);
  DEFINE_INT_PROP(ValueInput, Widget, cursor_color);
  DEFINE_INT_PROP(ValueInput, Widget, shortcut);
  ARENA_RESTORE;

  DEFINE_CLASS(ValueOutput, Valuator);
  INHERIT_TEXT_PROPS(ValueOutput);
  DEFINE_BOOL_PROP(ValueOutput, Widget, soft);
  ARENA_RESTORE;

  DEFINE_CLASS(Spinner, Group);
  INHERIT_TEXT_PROPS(Spinner);
  DEFINE_FLOAT_PROP(Spinner, Widget, value);
  DEFINE_FLOAT_PROP(Spinner, Widget, minimum);
  DEFINE_FLOAT_PROP(Spinner, Widget, maximum);
  DEFINE_FLOAT_PROP(Spinner, Widget, step);
  DEFINE_STR_PROP(Spinner, Widget, format);
  mrb_define_method(mrb, _class_fltk3_Spinner, "range", mrb_fltk3_spinner_range, MRB_ARGS_REQ(2));
  ARENA_RESTORE;

  DEFINE_CLASS(Progress, Widget);
  DEFINE_FLOAT_PROP(Progress, Widget, value);
  DEFINE_FLOAT_PROP(Progress, Widget, minimum);
  DEFINE_FLOAT_PROP(Progress, Widget, maximum);
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
