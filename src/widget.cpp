#include "mrb_fltk3.h"
#include <fltk3/Box.h>
#include <fltk3/Group.h>
#include <fltk3/Window.h>

/*********************************************************
 * FLTK3::Widget
 *********************************************************/
DECLARE_WIDGET(Widget)

static mrb_value
mrb_fltk3_widget_box_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Box_wrap(mrb, context->v->box(), "Box");
}

static mrb_value
mrb_fltk3_widget_box_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value box;
  mrb_get_args(mrb, "o", &box);
  context->v->box(mrb_fltk3_Box_ptr(mrb, box));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_image_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Image_wrap(mrb, context->v->image(), mrb_fltk3_image_classname(context->v->image()));
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

static mrb_value
mrb_fltk3_widget_deimage_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Image_wrap(mrb, context->v->deimage(), mrb_fltk3_image_classname(context->v->deimage()));
}

static mrb_value
mrb_fltk3_widget_deimage_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value image;
  mrb_get_args(mrb, "o", &image);
  context->v->deimage(mrb_fltk3_Image_ptr(mrb, image));
  return mrb_nil_value();
}

static void
_mrb_fltk3_widget_callback(fltk3::Widget* v, void* data)
{
  mrb_value args[2];
  mrb_fltk3_Widget_context* context = (mrb_fltk3_Widget_context*) data;
  mrb_state* mrb = context->mrb;
  mrb_value proc = mrb_iv_get(mrb, context->instance, mrb_intern_lit(mrb, "callback"));
  mrb_value value = mrb_iv_get(mrb, context->instance, mrb_intern_lit(mrb, "user_data"));
  if (mrb_nil_p(proc)) return;
  args[0] = context->instance;
  args[1] = value;
  int ai = mrb_gc_arena_save(mrb);
  mrb_yield_argv(mrb, proc, 2, args);
  mrb_gc_arena_restore(mrb, ai);
}

/* callback { |widget, user_data| ... }
 * callback(user_data) { |widget, user_data| ... }
 * callback(proc)
 * callback  -> proc */
static mrb_value
mrb_fltk3_widget_callback(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b = mrb_nil_value();
  mrb_value v = mrb_nil_value();
  mrb_int argc = mrb_get_args(mrb, "&|o", &b, &v);
  if (mrb_nil_p(b) && argc == 1 && mrb_proc_p(v)) {
    b = v;
    v = mrb_nil_value();
    argc = 0;
  }
  if (mrb_nil_p(b)) {
    return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "callback"));
  }
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "callback"), b);
  if (argc > 0) {
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "user_data"), v);
  }
  context->v->callback(_mrb_fltk3_widget_callback, context);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_callback_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b;
  mrb_get_args(mrb, "o", &b);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "callback"), b);
  if (mrb_nil_p(b)) {
    context->v->callback((fltk3::Callback*) NULL);
  } else {
    context->v->callback(_mrb_fltk3_widget_callback, context);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_user_data_get(mrb_state *mrb, mrb_value self)
{
  return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "user_data"));
}

static mrb_value
mrb_fltk3_widget_user_data_set(mrb_state *mrb, mrb_value self)
{
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "user_data"), v);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_parent(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Widget* w = context->v->parent();
  return mrb_fltk3_Widget_wrap(mrb, w, mrb_fltk3_widget_classname(w));
}

static mrb_value
mrb_fltk3_widget_window(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Widget* w = context->v->window();
  return mrb_fltk3_Widget_wrap(mrb, w, mrb_fltk3_widget_classname(w));
}

static mrb_value
mrb_fltk3_widget_resize(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int x, y, w, h;
  mrb_get_args(mrb, "iiii", &x, &y, &w, &h);
  context->v->resize(x, y, w, h);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_position(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  context->v->position(x, y);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_size(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int w, h;
  mrb_get_args(mrb, "ii", &w, &h);
  context->v->size(w, h);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_measure_label(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  int w = 0, h = 0;
  context->v->measure_label(w, h);
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(w));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(h));
  return ret;
}

static mrb_value
mrb_fltk3_widget_contains(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value o;
  mrb_get_args(mrb, "o", &o);
  return mrb_bool_value(context->v->contains(mrb_fltk3_Widget_ptr(mrb, o)) != 0);
}

static mrb_value
mrb_fltk3_widget_inside(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value o;
  mrb_get_args(mrb, "o", &o);
  return mrb_bool_value(context->v->inside(mrb_fltk3_Widget_ptr(mrb, o)) != 0);
}

static mrb_value
mrb_fltk3_widget_active_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  if (b) context->v->activate(); else context->v->deactivate();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_visible_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  if (b) context->v->show(); else context->v->hide();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_changed_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  if (b) context->v->set_changed(); else context->v->clear_changed();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_output_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  if (b) context->v->set_output(); else context->v->clear_output();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_take_focus(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_bool_value(context->v->take_focus() != 0);
}

static mrb_value
mrb_fltk3_widget_damage_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fixnum_value(context->v->damage());
}

static mrb_value
mrb_fltk3_widget_damage_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int c;
  mrb_get_args(mrb, "i", &c);
  context->v->set_damage((uchar) c);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_clear_damage(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int c = 0;
  mrb_get_args(mrb, "|i", &c);
  context->v->clear_damage((uchar) c);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_color(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int bg, sel;
  mrb_int argc = mrb_get_args(mrb, "|ii", &bg, &sel);
  if (argc == 0) return mrb_fixnum_value(context->v->color());
  if (argc == 1) context->v->color((fltk3::Color) bg);
  else context->v->color((fltk3::Color) bg, (fltk3::Color) sel);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_labeltype_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fixnum_value((mrb_int) context->v->labeltype());
}

static mrb_value
mrb_fltk3_widget_labeltype_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int t;
  mrb_get_args(mrb, "i", &t);
  context->v->labeltype((fltk3::Labeltype) t);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_when_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fixnum_value((mrb_int) context->v->when());
}

static mrb_value
mrb_fltk3_widget_when_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int t;
  mrb_get_args(mrb, "i", &t);
  context->v->when((uchar) t);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_widget_equal(mrb_state *mrb, mrb_value self)
{
  mrb_value o;
  mrb_get_args(mrb, "o", &o);
  if (!mrb_data_p(o) || DATA_TYPE(o) != &fltk3_Widget_type) return mrb_false_value();
  mrb_fltk3_Widget_context* a = (mrb_fltk3_Widget_context*) DATA_PTR(self);
  mrb_fltk3_Widget_context* b = (mrb_fltk3_Widget_context*) DATA_PTR(o);
  return mrb_bool_value(a && b && a->v == b->v);
}

void
mrb_fltk3_widget_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_define_class_under(mrb, _class_fltk3, "Widget", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_Widget, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_Widget, "initialize", mrb_fltk3_Widget_init, MRB_ARGS_ANY());
  mrb_define_method(mrb, _class_fltk3_Widget, "==", mrb_fltk3_widget_equal, MRB_ARGS_REQ(1));
  DEFINE_VOID_METHOD(Widget, Widget, redraw);
  DEFINE_VOID_METHOD(Widget, Widget, redraw_label);
  DEFINE_VOID_METHOD(Widget, Widget, show);
  DEFINE_VOID_METHOD(Widget, Widget, hide);
  DEFINE_VOID_METHOD(Widget, Widget, activate);
  DEFINE_VOID_METHOD(Widget, Widget, deactivate);
  DEFINE_VOID_METHOD(Widget, Widget, set_changed);
  DEFINE_VOID_METHOD(Widget, Widget, clear_changed);
  DEFINE_VOID_METHOD(Widget, Widget, set_output);
  DEFINE_VOID_METHOD(Widget, Widget, clear_output);
  DEFINE_VOID_METHOD(Widget, Widget, set_visible_focus);
  DEFINE_VOID_METHOD(Widget, Widget, clear_visible_focus);
  DEFINE_VOID_METHOD(Widget, Widget, do_callback);
  DEFINE_INT_PROP(Widget, Widget, x);
  DEFINE_INT_PROP(Widget, Widget, y);
  DEFINE_INT_PROP(Widget, Widget, w);
  DEFINE_INT_PROP(Widget, Widget, h);
  DEFINE_INT_PROP(Widget, Widget, labelfont);
  DEFINE_INT_PROP(Widget, Widget, labelsize);
  DEFINE_INT_PROP(Widget, Widget, labelcolor);
  DEFINE_INT_PROP(Widget, Widget, selection_color);
  DEFINE_INT_PROP(Widget, Widget, align);
  DEFINE_INT_PROP(Widget, Widget, type);
  DEFINE_INT_PROP(Widget, Widget, argument);
  DEFINE_STR_PROP(Widget, Widget, label);
  DEFINE_STR_SETTER(Widget, Widget, copy_label);
  DEFINE_STR_PROP(Widget, Widget, tooltip);
  DEFINE_BOOL_GETTER(Widget, Widget, visible);
  DEFINE_BOOL_GETTER(Widget, Widget, visible_r);
  DEFINE_BOOL_GETTER(Widget, Widget, active);
  DEFINE_BOOL_GETTER(Widget, Widget, active_r);
  DEFINE_BOOL_GETTER(Widget, Widget, changed);
  DEFINE_BOOL_GETTER(Widget, Widget, output);
  DEFINE_BOOL_GETTER(Widget, Widget, takesevents);
  DEFINE_BOOL_PROP(Widget, Widget, visible_focus);
  mrb_define_method(mrb, _class_fltk3_Widget, "visible?", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_bool_value(context->v->visible() != 0);
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "active?", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_bool_value(context->v->active() != 0);
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "changed?", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_bool_value(context->v->changed() != 0);
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "output?", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_bool_value(context->v->output() != 0);
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "visible=", mrb_fltk3_widget_visible_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "active=", mrb_fltk3_widget_active_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "changed=", mrb_fltk3_widget_changed_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "output=", mrb_fltk3_widget_output_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "take_focus", mrb_fltk3_widget_take_focus, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "color", mrb_fltk3_widget_color, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Widget, "color=", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    mrb_int c;
    mrb_get_args(mrb, "i", &c);
    context->v->color((fltk3::Color) c);
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "labeltype", mrb_fltk3_widget_labeltype_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "labeltype=", mrb_fltk3_widget_labeltype_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "when", mrb_fltk3_widget_when_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "when=", mrb_fltk3_widget_when_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "damage", mrb_fltk3_widget_damage_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "damage=", mrb_fltk3_widget_damage_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "clear_damage", mrb_fltk3_widget_clear_damage, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "box", mrb_fltk3_widget_box_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "box=", mrb_fltk3_widget_box_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "image", mrb_fltk3_widget_image_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "image=", mrb_fltk3_widget_image_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "deimage", mrb_fltk3_widget_deimage_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "deimage=", mrb_fltk3_widget_deimage_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "callback", mrb_fltk3_widget_callback, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "callback=", mrb_fltk3_widget_callback_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "user_data", mrb_fltk3_widget_user_data_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "user_data=", mrb_fltk3_widget_user_data_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "parent", mrb_fltk3_widget_parent, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "window", mrb_fltk3_widget_window, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "resize", mrb_fltk3_widget_resize, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, _class_fltk3_Widget, "position", mrb_fltk3_widget_position, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Widget, "size", mrb_fltk3_widget_size, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Widget, "measure_label", mrb_fltk3_widget_measure_label, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Widget, "contains", mrb_fltk3_widget_contains, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Widget, "inside", mrb_fltk3_widget_inside, MRB_ARGS_REQ(1));
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
