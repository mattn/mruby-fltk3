#include "mrb_fltk3.h"
#include <fltk3/Group.h>
#include <fltk3/PackedGroup.h>
#include <fltk3/TabGroup.h>
#include <fltk3/TiledGroup.h>
#include <fltk3/ScrollGroup.h>
#include <fltk3/WizardGroup.h>
#include <fltk3/Window.h>
#include <fltk3/DoubleWindow.h>
#include <fltk3/SingleWindow.h>
#include <fltk3/MenuWindow.h>

/*********************************************************
 * FLTK3::Group
 *********************************************************/
DECLARE_WIDGET(Group)
DECLARE_WIDGET(PackedGroup)
DECLARE_WIDGET(TabGroup)
DECLARE_WIDGET(TiledGroup)
DECLARE_WIDGET(ScrollGroup)
DECLARE_WIDGET(WizardGroup)
DECLARE_WINDOW(Window)
DECLARE_WINDOW(DoubleWindow)
DECLARE_WINDOW(SingleWindow)
DECLARE_WINDOW(MenuWindow)

static mrb_value
mrb_fltk3_wrap_widget(mrb_state* mrb, fltk3::Widget* w)
{
  return mrb_fltk3_Widget_wrap(mrb, w, mrb_fltk3_widget_classname(w));
}

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
mrb_fltk3_group_current(mrb_state *mrb, mrb_value self)
{
  return mrb_fltk3_wrap_widget(mrb, fltk3::Group::current());
}

static mrb_value
mrb_fltk3_group_current_set(mrb_state *mrb, mrb_value self)
{
  mrb_value g;
  mrb_get_args(mrb, "o", &g);
  fltk3::Group::current((fltk3::Group*) mrb_fltk3_Widget_ptr(mrb, g));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_group_resizable_get(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_wrap_widget(mrb, ((fltk3::Group*) context->v)->resizable());
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
mrb_fltk3_group_children(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Group* g = (fltk3::Group*) context->v;
  mrb_value ary = mrb_ary_new_capa(mrb, g->children());
  for (int i = 0; i < g->children(); i++) {
    mrb_ary_push(mrb, ary, mrb_fltk3_wrap_widget(mrb, g->child(i)));
  }
  return ary;
}

static mrb_value
mrb_fltk3_group_child(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  fltk3::Group* g = (fltk3::Group*) context->v;
  if (i < 0) i += g->children();
  if (i < 0 || i >= g->children()) return mrb_nil_value();
  return mrb_fltk3_wrap_widget(mrb, g->child(i));
}

static mrb_value
mrb_fltk3_group_find(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  return mrb_fixnum_value(((fltk3::Group*) context->v)->find(mrb_fltk3_Widget_ptr(mrb, w)));
}

static mrb_value
mrb_fltk3_group_add(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  ((fltk3::Group*) context->v)->add(mrb_fltk3_Widget_ptr(mrb, w));
  return self;
}

static mrb_value
mrb_fltk3_group_insert(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w, before;
  mrb_get_args(mrb, "oo", &w, &before);
  fltk3::Group* g = (fltk3::Group*) context->v;
  if (mrb_integer_p(before)) {
    g->insert(*mrb_fltk3_Widget_ptr(mrb, w), (int) mrb_integer(before));
  } else {
    g->insert(*mrb_fltk3_Widget_ptr(mrb, w), mrb_fltk3_Widget_ptr(mrb, before));
  }
  return self;
}

static mrb_value
mrb_fltk3_group_remove(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  fltk3::Group* g = (fltk3::Group*) context->v;
  if (mrb_integer_p(w)) {
    g->remove((int) mrb_integer(w));
  } else {
    g->remove(mrb_fltk3_Widget_ptr(mrb, w));
  }
  return self;
}

static mrb_value
mrb_fltk3_group_clear(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Group* g = (fltk3::Group*) context->v;
  for (int i = 0; i < g->children(); i++) {
    mrb_fltk3_widget_forget(mrb, g->child(i));
  }
  g->clear();
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_group_clip_children_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  ((fltk3::Group*) context->v)->clip_children(b);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::TabGroup / FLTK3::WizardGroup
 *********************************************************/
static mrb_value
mrb_fltk3_tabgroup_value(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_wrap_widget(mrb, ((fltk3::TabGroup*) context->v)->value());
}

static mrb_value
mrb_fltk3_tabgroup_value_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  ((fltk3::TabGroup*) context->v)->value(mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_tabgroup_push(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w;
  if (mrb_get_args(mrb, "|o", &w) == 0) {
    return mrb_fltk3_wrap_widget(mrb, ((fltk3::TabGroup*) context->v)->push());
  }
  ((fltk3::TabGroup*) context->v)->push(mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_tabgroup_which(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  return mrb_fltk3_wrap_widget(mrb, ((fltk3::TabGroup*) context->v)->which(x, y));
}

static mrb_value
mrb_fltk3_wizardgroup_value(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_wrap_widget(mrb, ((fltk3::WizardGroup*) context->v)->value());
}

static mrb_value
mrb_fltk3_wizardgroup_value_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  ((fltk3::WizardGroup*) context->v)->value(mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::ScrollGroup
 *********************************************************/
static mrb_value
mrb_fltk3_scrollgroup_scroll_to(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  ((fltk3::ScrollGroup*) context->v)->scroll_to(x, y);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::Window
 *********************************************************/
/* show          -> Window::show()
 * show(argv)    -> Window::show(argc, argv), which also parses the standard
 *                  fltk3 command line switches and records WM_COMMAND */
static mrb_value
mrb_fltk3_window_show(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value args = mrb_nil_value();
  mrb_get_args(mrb, "|A", &args);
  fltk3::Window* w = (fltk3::Window*) context->v;
  if (mrb_nil_p(args) || RARRAY_LEN(args) == 0) {
    w->show();
    return mrb_nil_value();
  }
  int argc = (int) RARRAY_LEN(args);
  char** argv = (char**) malloc(sizeof(char*) * (argc + 1));
  if (!argv) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");
  for (int i = 0; i < argc; i++) {
    argv[i] = (char*) RSTRING_CSTR(mrb, mrb_ary_ref(mrb, args, i));
  }
  argv[argc] = NULL;
  w->show(argc, argv);
  free(argv);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_size_range(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int minw, minh, maxw = 0, maxh = 0, dw = 0, dh = 0;
  mrb_bool aspect = FALSE;
  mrb_get_args(mrb, "ii|iiiib", &minw, &minh, &maxw, &maxh, &dw, &dh, &aspect);
  ((fltk3::Window*) context->v)->size_range(minw, minh, maxw, maxh, dw, dh, aspect);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_fullscreen_off(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int x, y, w, h;
  if (mrb_get_args(mrb, "|iiii", &x, &y, &w, &h) == 4) {
    ((fltk3::Window*) context->v)->fullscreen_off(x, y, w, h);
  } else {
    ((fltk3::Window*) context->v)->fullscreen_off();
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_hotspot(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value a, b = mrb_nil_value();
  mrb_int argc = mrb_get_args(mrb, "o|o", &a, &b);
  fltk3::Window* w = (fltk3::Window*) context->v;
  if (mrb_integer_p(a)) {
    w->hotspot((int) mrb_integer(a), (int) (argc > 1 ? mrb_integer(b) : 0));
  } else {
    w->hotspot(mrb_fltk3_Widget_ptr(mrb, a), mrb_test(b) ? 1 : 0);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_border_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  ((fltk3::Window*) context->v)->border(b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_cursor(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int c, fg = fltk3::BLACK, bg = fltk3::WHITE;
  mrb_get_args(mrb, "i|ii", &c, &fg, &bg);
  ((fltk3::Window*) context->v)->cursor((fltk3::Cursor) c, (fltk3::Color) fg, (fltk3::Color) bg);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_window_default_cursor(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int c, fg = fltk3::BLACK, bg = fltk3::WHITE;
  mrb_get_args(mrb, "i|ii", &c, &fg, &bg);
  ((fltk3::Window*) context->v)->default_cursor((fltk3::Cursor) c, (fltk3::Color) fg, (fltk3::Color) bg);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menuwindow_overlay_set(mrb_state *mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  if (b) ((fltk3::MenuWindow*) context->v)->set_overlay();
  else ((fltk3::MenuWindow*) context->v)->clear_overlay();
  return mrb_nil_value();
}

#define INHERIT_GROUP(x) \
  mrb_define_method(mrb, _class_fltk3_ ## x, "begin", mrb_fltk3_group_begin, MRB_ARGS_NONE()); \
  DEFINE_VOID_METHOD(x, Widget, end); \
  DEFINE_VOID_METHOD(x, Widget, init_sizes); \
  DEFINE_BOOL_GETTER(x, Widget, clip_children); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "clip_children=", mrb_fltk3_group_clip_children_set, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "resizable", mrb_fltk3_group_resizable_get, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "resizable=", mrb_fltk3_group_resizable_set, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "children", mrb_fltk3_group_children, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "child", mrb_fltk3_group_child, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "find", mrb_fltk3_group_find, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "add", mrb_fltk3_group_add, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "<<", mrb_fltk3_group_add, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "insert", mrb_fltk3_group_insert, MRB_ARGS_REQ(2)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "remove", mrb_fltk3_group_remove, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "clear", mrb_fltk3_group_clear, MRB_ARGS_NONE()); \
  ARENA_RESTORE;

void
mrb_fltk3_group_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_CLASS(Group, Widget);
  INHERIT_GROUP(Group);
  mrb_define_class_method(mrb, _class_fltk3_Group, "current", mrb_fltk3_group_current, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, _class_fltk3_Group, "current=", mrb_fltk3_group_current_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(PackedGroup, Group);
  DEFINE_INT_PROP(PackedGroup, Widget, spacing);
  DEFINE_BOOL_GETTER(PackedGroup, Widget, horizontal);
  mrb_define_const(mrb, _class_fltk3_PackedGroup, "VERTICAL", mrb_fixnum_value(fltk3::PackedGroup::VERTICAL));
  mrb_define_const(mrb, _class_fltk3_PackedGroup, "HORIZONTAL", mrb_fixnum_value(fltk3::PackedGroup::HORIZONTAL));
  ARENA_RESTORE;

  DEFINE_CLASS(TabGroup, Group);
  mrb_define_method(mrb, _class_fltk3_TabGroup, "value", mrb_fltk3_tabgroup_value, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TabGroup, "value=", mrb_fltk3_tabgroup_value_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TabGroup, "push", mrb_fltk3_tabgroup_push, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TabGroup, "which", mrb_fltk3_tabgroup_which, MRB_ARGS_REQ(2));
  ARENA_RESTORE;

  DEFINE_CLASS(TiledGroup, Group);

  DEFINE_CLASS(ScrollGroup, Group);
  DEFINE_INT_GETTER(ScrollGroup, Widget, xposition);
  DEFINE_INT_GETTER(ScrollGroup, Widget, yposition);
  DEFINE_INT_PROP(ScrollGroup, Widget, scrollbar_size);
  mrb_define_method(mrb, _class_fltk3_ScrollGroup, "scroll_to", mrb_fltk3_scrollgroup_scroll_to, MRB_ARGS_REQ(2));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "HORIZONTAL", mrb_fixnum_value(fltk3::ScrollGroup::HORIZONTAL));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "VERTICAL", mrb_fixnum_value(fltk3::ScrollGroup::VERTICAL));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "BOTH", mrb_fixnum_value(fltk3::ScrollGroup::BOTH));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "ALWAYS_ON", mrb_fixnum_value(fltk3::ScrollGroup::ALWAYS_ON));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "HORIZONTAL_ALWAYS", mrb_fixnum_value(fltk3::ScrollGroup::HORIZONTAL_ALWAYS));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "VERTICAL_ALWAYS", mrb_fixnum_value(fltk3::ScrollGroup::VERTICAL_ALWAYS));
  mrb_define_const(mrb, _class_fltk3_ScrollGroup, "BOTH_ALWAYS", mrb_fixnum_value(fltk3::ScrollGroup::BOTH_ALWAYS));
  ARENA_RESTORE;

  DEFINE_CLASS(WizardGroup, Group);
  DEFINE_VOID_METHOD(WizardGroup, Widget, next);
  DEFINE_VOID_METHOD(WizardGroup, Widget, prev);
  mrb_define_method(mrb, _class_fltk3_WizardGroup, "value", mrb_fltk3_wizardgroup_value, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_WizardGroup, "value=", mrb_fltk3_wizardgroup_value_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(Window, Group);
  mrb_define_method(mrb, _class_fltk3_Window, "show", mrb_fltk3_window_show, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Window, "size_range", mrb_fltk3_window_size_range, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(5));
  mrb_define_method(mrb, _class_fltk3_Window, "fullscreen_off", mrb_fltk3_window_fullscreen_off, MRB_ARGS_OPT(4));
  mrb_define_method(mrb, _class_fltk3_Window, "hotspot", mrb_fltk3_window_hotspot, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Window, "border=", mrb_fltk3_window_border_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Window, "cursor", mrb_fltk3_window_cursor, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Window, "default_cursor", mrb_fltk3_window_default_cursor, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  DEFINE_VOID_METHOD(Window, Widget, fullscreen);
  DEFINE_VOID_METHOD(Window, Widget, iconize);
  DEFINE_VOID_METHOD(Window, Widget, make_current);
  DEFINE_VOID_METHOD(Window, Widget, free_position);
  DEFINE_VOID_METHOD(Window, Widget, set_modal);
  DEFINE_VOID_METHOD(Window, Widget, set_non_modal);
  DEFINE_VOID_METHOD(Window, Widget, clear_border);
  DEFINE_VOID_METHOD(Window, Widget, set_override);
  DEFINE_VOID_METHOD(Window, Widget, set_menu_window);
  DEFINE_VOID_METHOD(Window, Widget, set_tooltip_window);
  DEFINE_BOOL_GETTER(Window, Widget, shown);
  DEFINE_BOOL_GETTER(Window, Widget, border);
  DEFINE_BOOL_GETTER(Window, Widget, override);
  DEFINE_BOOL_GETTER(Window, Widget, modal);
  DEFINE_BOOL_GETTER(Window, Widget, non_modal);
  DEFINE_BOOL_GETTER(Window, Widget, menu_window);
  DEFINE_BOOL_GETTER(Window, Widget, tooltip_window);
  DEFINE_BOOL_GETTER(Window, Widget, fullscreen_active);
  DEFINE_INT_GETTER(Window, Widget, x_root);
  DEFINE_INT_GETTER(Window, Widget, y_root);
  DEFINE_INT_GETTER(Window, Widget, decorated_w);
  DEFINE_INT_GETTER(Window, Widget, decorated_h);
  DEFINE_STR_PROP(Window, Widget, label);
  DEFINE_STR_PROP(Window, Widget, iconlabel);
  DEFINE_STR_PROP(Window, Widget, xclass);
  mrb_define_method(mrb, _class_fltk3_Window, "shown?", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_bool_value(((fltk3::Window*) context->v)->shown() != 0);
  }, MRB_ARGS_NONE());
  ARENA_RESTORE;

  DEFINE_CLASS(DoubleWindow, Window);
  DEFINE_VOID_METHOD(DoubleWindow, Widget, flush);
  DEFINE_CLASS(SingleWindow, Window);
  DEFINE_CLASS(MenuWindow, SingleWindow);
  DEFINE_BOOL_GETTER(MenuWindow, Widget, overlay);
  mrb_define_method(mrb, _class_fltk3_MenuWindow, "overlay=", mrb_fltk3_menuwindow_overlay_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
