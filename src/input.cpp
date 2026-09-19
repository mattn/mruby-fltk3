#include "mrb_fltk3.h"
#include <fltk3/Input_.h>
#include <fltk3/Input.h>
#include <fltk3/Output.h>
#include <fltk3/MultilineInput.h>
#include <fltk3/MultilineOutput.h>
#include <fltk3/IntInput.h>
#include <fltk3/FloatInput.h>
#include <fltk3/SecretInput.h>
#include <fltk3/FileInput.h>

/*********************************************************
 * FLTK3::Input
 *********************************************************/
DECLARE_WIDGET(Input)
DECLARE_WIDGET(Output)
DECLARE_WIDGET(MultilineInput)
DECLARE_WIDGET(MultilineOutput)
DECLARE_WIDGET(IntInput)
DECLARE_WIDGET(FloatInput)
DECLARE_WIDGET(SecretInput)
DECLARE_WIDGET(FileInput)

static mrb_value
mrb_fltk3_input_value_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S!", &s);
  if (mrb_nil_p(s)) {
    ((fltk3::Input_*) context->v)->value("");
  } else {
    ((fltk3::Input_*) context->v)->value(RSTRING_PTR(s), (int) RSTRING_LEN(s));
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_input_position(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int p, m;
  mrb_int argc = mrb_get_args(mrb, "|ii", &p, &m);
  fltk3::Input_* in = (fltk3::Input_*) context->v;
  if (argc == 0) return mrb_fixnum_value(in->position());
  if (argc == 1) return mrb_fixnum_value(in->position((int) p));
  return mrb_fixnum_value(in->position((int) p, (int) m));
}

static mrb_value
mrb_fltk3_input_position_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int p;
  mrb_get_args(mrb, "i", &p);
  ((fltk3::Input_*) context->v)->position((int) p);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_input_mark_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int m;
  mrb_get_args(mrb, "i", &m);
  ((fltk3::Input_*) context->v)->mark((int) m);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_input_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  return mrb_bool_value(((fltk3::Input_*) context->v)->insert(RSTRING_PTR(s), (int) RSTRING_LEN(s)) != 0);
}

static mrb_value
mrb_fltk3_input_replace(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int a, b;
  mrb_value s;
  mrb_get_args(mrb, "iiS!", &a, &b, &s);
  fltk3::Input_* in = (fltk3::Input_*) context->v;
  int r = mrb_nil_p(s) ? in->replace((int) a, (int) b, NULL, 0)
                       : in->replace((int) a, (int) b, RSTRING_PTR(s), (int) RSTRING_LEN(s));
  return mrb_bool_value(r != 0);
}

static mrb_value
mrb_fltk3_input_cut(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int a, b;
  mrb_int argc = mrb_get_args(mrb, "|ii", &a, &b);
  fltk3::Input_* in = (fltk3::Input_*) context->v;
  int r;
  if (argc == 0) r = in->cut();
  else if (argc == 1) r = in->cut((int) a);
  else r = in->cut((int) a, (int) b);
  return mrb_bool_value(r != 0);
}

static mrb_value
mrb_fltk3_input_copy(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int clipboard = 1;
  mrb_get_args(mrb, "|i", &clipboard);
  return mrb_bool_value(((fltk3::Input_*) context->v)->copy((int) clipboard) != 0);
}

static mrb_value
mrb_fltk3_input_undo(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_bool_value(((fltk3::Input_*) context->v)->undo() != 0);
}

static mrb_value
mrb_fltk3_input_copy_cuts(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_bool_value(((fltk3::Input_*) context->v)->copy_cuts() != 0);
}

static mrb_value
mrb_fltk3_input_index(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  return mrb_fixnum_value(((fltk3::Input_*) context->v)->index((int) i));
}

static mrb_value
mrb_fltk3_input_readonly_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  ((fltk3::Input_*) context->v)->readonly(b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_input_wrap_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  ((fltk3::Input_*) context->v)->wrap(b);
  return mrb_nil_value();
}

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

void
mrb_fltk3_input_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_INT_CONST(NORMAL_INPUT);
  DEFINE_INT_CONST(FLOAT_INPUT);
  DEFINE_INT_CONST(INT_INPUT);
  DEFINE_INT_CONST(HIDDEN_INPUT);
  DEFINE_INT_CONST(MULTILINE_INPUT);
  DEFINE_INT_CONST(SECRET_INPUT);
  DEFINE_INT_CONST(INPUT_TYPE);
  DEFINE_INT_CONST(INPUT_READONLY);
  DEFINE_INT_CONST(NORMAL_OUTPUT);
  DEFINE_INT_CONST(MULTILINE_OUTPUT);
  DEFINE_INT_CONST(INPUT_WRAP);
  DEFINE_INT_CONST(MULTILINE_INPUT_WRAP);
  DEFINE_INT_CONST(MULTILINE_OUTPUT_WRAP);

  DEFINE_CLASS(Input, Widget);
  INHERIT_TEXT_PROPS(Input);
  DEFINE_STR_GETTER(Input, Widget, value);
  mrb_define_method(mrb, _class_fltk3_Input, "value=", mrb_fltk3_input_value_set, MRB_ARGS_REQ(1));
  DEFINE_INT_GETTER(Input, Widget, size);
  DEFINE_INT_PROP(Input, Widget, maximum_size);
  DEFINE_INT_PROP(Input, Widget, cursor_color);
  DEFINE_INT_PROP(Input, Widget, shortcut);
  DEFINE_INT_PROP(Input, Widget, input_type);
  DEFINE_INT_GETTER(Input, Widget, mark);
  DEFINE_BOOL_GETTER(Input, Widget, readonly);
  DEFINE_BOOL_GETTER(Input, Widget, wrap);
  mrb_define_method(mrb, _class_fltk3_Input, "readonly=", mrb_fltk3_input_readonly_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Input, "wrap=", mrb_fltk3_input_wrap_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Input, "position", mrb_fltk3_input_position, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Input, "position=", mrb_fltk3_input_position_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Input, "mark=", mrb_fltk3_input_mark_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Input, "insert", mrb_fltk3_input_insert, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Input, "replace", mrb_fltk3_input_replace, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, _class_fltk3_Input, "cut", mrb_fltk3_input_cut, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Input, "copy", mrb_fltk3_input_copy, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Input, "undo", mrb_fltk3_input_undo, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Input, "copy_cuts", mrb_fltk3_input_copy_cuts, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Input, "index", mrb_fltk3_input_index, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(Output, Input);
  DEFINE_CLASS(MultilineInput, Input);
  DEFINE_CLASS(MultilineOutput, Output);
  DEFINE_CLASS(IntInput, Input);
  DEFINE_CLASS(FloatInput, Input);
  DEFINE_CLASS(SecretInput, Input);
  DEFINE_CLASS(FileInput, Input);
  DEFINE_INT_PROP(FileInput, Widget, errorcolor);
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
