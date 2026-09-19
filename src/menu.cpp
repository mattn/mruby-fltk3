#include "mrb_fltk3.h"
#include <fltk3/Menu.h>
#include <fltk3/MenuBar.h>
#include <fltk3/MenuButton.h>

/*********************************************************
 * FLTK3::MenuItem / FLTK3::MenuBar
 *********************************************************/
DECLARE_WIDGET(MenuBar)
DECLARE_WIDGET(MenuButton)

static mrb_value
mrb_fltk3_MenuItem_init(mrb_state* mrb, mrb_value self)
{
  fltk3::MenuItem* item = new fltk3::MenuItem;
  memset(item, 0, sizeof(fltk3::MenuItem));
  return mrb_fltk3_MenuItem_setup(mrb, self, item);
}

static mrb_value
mrb_fltk3_menubar_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b = mrb_nil_value(), caption;
  mrb_int shortcut = 0;
  mrb_get_args(mrb, "&S|i", &b, &caption, &shortcut);
  ((fltk3::MenuBar*) context->v)->add(RSTRING_CSTR(mrb, caption), (int) shortcut, [] (fltk3::Widget* w, void* d) {
  });
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menubar_menu(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_MenuItem_wrap(mrb, (fltk3::MenuItem*) ((fltk3::MenuBar*) context->v)->menu(), "MenuItem");
}

void
mrb_fltk3_menu_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");
  struct RClass* _class_fltk3_Button = mrb_fltk3_class(mrb, "Button");

  struct RClass* _class_fltk3_MenuItem = mrb_define_class_under(mrb, _class_fltk3, "MenuItem", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_MenuItem, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_MenuItem, "initialize", mrb_fltk3_MenuItem_init, MRB_ARGS_NONE());
  ARENA_RESTORE;

  DEFINE_CLASS(MenuBar, Widget);
  mrb_define_method(mrb, _class_fltk3_MenuBar, "add", mrb_fltk3_menubar_add, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_MenuBar, "menu", mrb_fltk3_menubar_menu, MRB_ARGS_NONE());
  ARENA_RESTORE;

  DEFINE_CLASS(MenuButton, Button);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
