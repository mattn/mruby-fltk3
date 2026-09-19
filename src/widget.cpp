#include "mrb_fltk3.h"
#include <fltk3/Box.h>
#include <fltk3/ValueOutput.h>

/*********************************************************
 * FLTK3::Widget
 *********************************************************/
DECLARE_WIDGET(Widget)
DECLARE_WIDGET(ValueOutput)

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
  context->v->box((fltk3::Box*) mrb_fltk3_Widget_ptr(mrb, box));
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
  context->v->image(mrb_fltk3_Image_ptr(mrb, image));
  return mrb_nil_value();
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

void
mrb_fltk3_widget_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_define_class_under(mrb, _class_fltk3, "Widget", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_Widget, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_Widget, "initialize", mrb_fltk3_Widget_init, MRB_ARGS_ANY());
  DEFINE_VOID_METHOD(Widget, Widget, redraw);
  DEFINE_VOID_METHOD(Widget, Widget, show);
  DEFINE_VOID_METHOD(Widget, Widget, hide);
  DEFINE_INT_PROP(Widget, Widget, x);
  DEFINE_INT_PROP(Widget, Widget, y);
  DEFINE_INT_PROP(Widget, Widget, w);
  DEFINE_INT_PROP(Widget, Widget, h);
  DEFINE_INT_PROP(Widget, Widget, labelfont);
  DEFINE_INT_PROP(Widget, Widget, labelsize);
  DEFINE_STR_PROP(Widget, Widget, label);
  DEFINE_BOOL_GETTER(Widget, Widget, visible);
  mrb_define_method(mrb, _class_fltk3_Widget, "box", mrb_fltk3_widget_box_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "box=", mrb_fltk3_widget_box_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "image", mrb_fltk3_widget_image_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "image=", mrb_fltk3_widget_image_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "callback", mrb_fltk3_widget_callback, MRB_ARGS_OPT(1));
  ARENA_RESTORE;

  DEFINE_CLASS(ValueOutput, Widget);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
