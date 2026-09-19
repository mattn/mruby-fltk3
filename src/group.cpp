#include "mrb_fltk3.h"
#include <fltk3/Group.h>
#include <fltk3/Window.h>
#include <fltk3/DoubleWindow.h>

/*********************************************************
 * FLTK3::Group
 *********************************************************/
DECLARE_WIDGET(Group)
DECLARE_WINDOW(Window)
DECLARE_WINDOW(DoubleWindow)

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
mrb_fltk3_group_resizable_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Widget* w = ((fltk3::Group*) context->v)->resizable();
  return mrb_fltk3_Widget_wrap(mrb, w, mrb_fltk3_widget_classname(w));
}

static mrb_value
mrb_fltk3_group_resizable_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value arg;
  mrb_get_args(mrb, "o", &arg);
  ((fltk3::Group*) context->v)->resizable(mrb_fltk3_Widget_ptr(mrb, arg));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_show(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  ((fltk3::Window*) context->v)->show(0, NULL);
  return mrb_nil_value();
}

void
mrb_fltk3_group_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_CLASS(Group, Widget);
  mrb_define_method(mrb, _class_fltk3_Group, "begin", mrb_fltk3_group_begin, MRB_ARGS_NONE());
  DEFINE_VOID_METHOD(Group, Widget, end);
  mrb_define_method(mrb, _class_fltk3_Group, "resizable", mrb_fltk3_group_resizable_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Group, "resizable=", mrb_fltk3_group_resizable_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(Window, Group);
  mrb_define_method(mrb, _class_fltk3_Window, "show", mrb_fltk3_window_show, MRB_ARGS_NONE());
  ARENA_RESTORE;

  DEFINE_CLASS(DoubleWindow, Window);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
