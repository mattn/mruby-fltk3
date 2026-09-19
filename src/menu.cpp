#include "mrb_fltk3.h"
#include <fltk3/MenuItem.h>
#include <fltk3/Menu_.h>
#include <fltk3/MenuBar.h>
#include <fltk3/MenuButton.h>
#include <fltk3/Choice.h>

/*********************************************************
 * FLTK3::MenuItem
 *
 * Menu items live inside the array owned by their fltk3::Menu_ widget,
 * so wrappers are created fresh on each access instead of being kept
 * in the registry; they are only valid until the menu is modified.
 *********************************************************/
static mrb_value
mrb_fltk3_menuitem_new(mrb_state* mrb, const fltk3::MenuItem* item)
{
  if (!item) return mrb_nil_value();
  mrb_fltk3_MenuItem_context* context =
    (mrb_fltk3_MenuItem_context*) malloc(sizeof(mrb_fltk3_MenuItem_context));
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");
  memset(context, 0, sizeof(mrb_fltk3_MenuItem_context));
  mrb_value instance = mrb_obj_value(Data_Wrap_Struct(mrb, mrb_fltk3_class(mrb, "MenuItem"), &fltk3_MenuItem_type, context));
  context->instance = instance;
  context->mrb = mrb;
  context->v = (fltk3::MenuItem*) item;
  return instance;
}

static mrb_value
mrb_fltk3_MenuItem_init(mrb_state* mrb, mrb_value self)
{
  mrb_value label = mrb_nil_value();
  mrb_int shortcut = 0, flags = 0;
  mrb_get_args(mrb, "|S!ii", &label, &shortcut, &flags);
  /* allocate a terminator too so the item forms a valid one-entry menu */
  fltk3::MenuItem* item = new fltk3::MenuItem[2];
  memset(item, 0, sizeof(fltk3::MenuItem) * 2);
  if (!mrb_nil_p(label)) {
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "label"), label);
    item->text = RSTRING_CSTR(mrb, label);
  }
  item->shortcut_ = (unsigned int) shortcut;
  item->flags = (int) flags;
  return mrb_fltk3_MenuItem_setup(mrb, self, item);
}

static mrb_value
mrb_fltk3_menuitem_label_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(MenuItem);
  mrb_value label;
  mrb_get_args(mrb, "S!", &label);
  /* keep the string alive as long as the item wrapper */
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "label"), label);
  context->v->label(mrb_nil_p(label) ? NULL : RSTRING_CSTR(mrb, label));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menuitem_next(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(MenuItem);
  mrb_int n = 1;
  mrb_get_args(mrb, "|i", &n);
  const fltk3::MenuItem* item = context->v->next((int) n);
  if (!item || !item->text) return mrb_nil_value();
  return mrb_fltk3_menuitem_new(mrb, item);
}

static mrb_value
mrb_fltk3_menuitem_flags_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(MenuItem);
  mrb_int f;
  mrb_get_args(mrb, "i", &f);
  context->v->flags = (int) f;
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menuitem_image_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(MenuItem);
  mrb_value image;
  mrb_get_args(mrb, "o", &image);
  context->v->image(*mrb_fltk3_Image_ptr(mrb, image));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menuitem_do_callback(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(MenuItem);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  if (context->v->callback()) context->v->do_callback(mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menuitem_popup(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(MenuItem);
  mrb_int x, y;
  mrb_value title = mrb_nil_value();
  mrb_get_args(mrb, "ii|S!", &x, &y, &title);
  const fltk3::MenuItem* picked = context->v->popup(x, y, mrb_nil_p(title) ? NULL : RSTRING_CSTR(mrb, title));
  return mrb_fltk3_menuitem_new(mrb, picked);
}

/*********************************************************
 * FLTK3::Menu_ and subclasses
 *********************************************************/
DECLARE_WIDGET(MenuBar)
DECLARE_WIDGET(MenuButton)
DECLARE_WIDGET(Choice)

static mrb_value
mrb_fltk3_Menu__init(mrb_state* mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "can't instantiate fltk3::Menu_ directly");
  return self;
}

typedef struct {
  mrb_state* mrb;
  mrb_value instance;
  mrb_int index;
} mrb_fltk3_menu_callback_data;

/* Called by fltk3 with the item's user_data; yields (menu, item). */
static void
_mrb_fltk3_menu_callback(fltk3::Widget* w, void* d)
{
  mrb_fltk3_menu_callback_data* data = (mrb_fltk3_menu_callback_data*) d;
  mrb_state* mrb = data->mrb;
  mrb_value callbacks = mrb_iv_get(mrb, data->instance, mrb_intern_lit(mrb, "menu_callbacks"));
  if (!mrb_array_p(callbacks)) return;
  mrb_value entry = mrb_ary_ref(mrb, callbacks, data->index);
  if (!mrb_array_p(entry)) return;
  mrb_value proc = mrb_ary_ref(mrb, entry, 0);
  if (mrb_nil_p(proc)) return;
  /* find the item this callback belongs to; mvalue() is only right when
   * the user picked it from the menu */
  fltk3::Menu_* menu = (fltk3::Menu_*) w;
  const fltk3::MenuItem* item = menu->mvalue();
  if (!item || item->user_data() != d) {
    const fltk3::MenuItem* items = menu->menu();
    for (int i = 0; items && i < menu->size(); i++) {
      if (items[i].user_data() == d) { item = items + i; break; }
    }
  }
  mrb_value args[2];
  args[0] = data->instance;
  args[1] = mrb_fltk3_menuitem_new(mrb, item);
  mrb_fltk3_call(mrb, proc, 2, args);
}

/* Store a block for a menu item and return the user_data to attach. */
static void*
mrb_fltk3_menu_callback_register(mrb_state* mrb, mrb_value self, mrb_value proc)
{
  if (mrb_nil_p(proc)) return NULL;
  mrb_value callbacks = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "menu_callbacks"));
  if (!mrb_array_p(callbacks)) {
    callbacks = mrb_ary_new(mrb);
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "menu_callbacks"), callbacks);
  }
  mrb_value buf = mrb_str_new(mrb, NULL, sizeof(mrb_fltk3_menu_callback_data));
  mrb_fltk3_menu_callback_data* data = (mrb_fltk3_menu_callback_data*) RSTRING_PTR(buf);
  data->mrb = mrb;
  data->instance = self;
  data->index = RARRAY_LEN(callbacks);
  mrb_value entry = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, entry, proc);
  mrb_ary_push(mrb, entry, buf);
  mrb_ary_push(mrb, callbacks, entry);
  return data;
}

static unsigned int
mrb_fltk3_menu_shortcut(mrb_state* mrb, mrb_value shortcut)
{
  if (mrb_nil_p(shortcut)) return 0;
  if (mrb_string_p(shortcut)) return fltk3::old_shortcut(RSTRING_CSTR(mrb, shortcut));
  return (unsigned int) mrb_integer(shortcut);
}

/* add(label, shortcut = 0, flags = 0) { |menu, item| } -> index */
static mrb_value
mrb_fltk3_menu_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b = mrb_nil_value(), label, shortcut = mrb_nil_value();
  mrb_int flags = 0;
  mrb_get_args(mrb, "&S|oi", &b, &label, &shortcut, &flags);
  void* data = mrb_fltk3_menu_callback_register(mrb, self, b);
  int index = ((fltk3::Menu_*) context->v)->add(RSTRING_CSTR(mrb, label),
    mrb_fltk3_menu_shortcut(mrb, shortcut), data ? _mrb_fltk3_menu_callback : NULL, data, (int) flags);
  return mrb_fixnum_value(index);
}

/* insert(index, label, shortcut = 0, flags = 0) { |menu, item| } -> index */
static mrb_value
mrb_fltk3_menu_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value b = mrb_nil_value(), label, shortcut = mrb_nil_value();
  mrb_int at, flags = 0;
  mrb_get_args(mrb, "&iS|oi", &b, &at, &label, &shortcut, &flags);
  void* data = mrb_fltk3_menu_callback_register(mrb, self, b);
  int index = ((fltk3::Menu_*) context->v)->insert((int) at, RSTRING_CSTR(mrb, label),
    mrb_fltk3_menu_shortcut(mrb, shortcut), data ? _mrb_fltk3_menu_callback : NULL, data, (int) flags);
  return mrb_fixnum_value(index);
}

static mrb_value
mrb_fltk3_menu_remove(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  ((fltk3::Menu_*) context->v)->remove((int) i);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menu_clear_submenu(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  return mrb_fixnum_value(((fltk3::Menu_*) context->v)->clear_submenu((int) i));
}

static mrb_value
mrb_fltk3_menu_replace(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_value text;
  mrb_get_args(mrb, "iS", &i, &text);
  ((fltk3::Menu_*) context->v)->replace((int) i, RSTRING_CSTR(mrb, text));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menu_shortcut_set_at(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_value s;
  mrb_get_args(mrb, "io", &i, &s);
  ((fltk3::Menu_*) context->v)->shortcut((int) i, mrb_fltk3_menu_shortcut(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menu_mode(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i, fl;
  if (mrb_get_args(mrb, "i|i", &i, &fl) == 1) {
    return mrb_fixnum_value(((fltk3::Menu_*) context->v)->mode((int) i));
  }
  ((fltk3::Menu_*) context->v)->mode((int) i, (int) fl);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menu_value(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fixnum_value(((fltk3::Menu_*) context->v)->value());
}

static mrb_value
mrb_fltk3_menu_value_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  fltk3::Menu_* m = (fltk3::Menu_*) context->v;
  if (mrb_integer_p(v)) m->value((int) mrb_integer(v));
  else m->value(mrb_fltk3_MenuItem_ptr(mrb, v));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menu_mvalue(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_menuitem_new(mrb, ((fltk3::Menu_*) context->v)->mvalue());
}

static mrb_value
mrb_fltk3_menu_text(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  fltk3::Menu_* m = (fltk3::Menu_*) context->v;
  const char* text = mrb_get_args(mrb, "|i", &i) == 0 ? m->text() : m->text((int) i);
  return text ? mrb_str_new_cstr(mrb, text) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_menu_item(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  fltk3::Menu_* m = (fltk3::Menu_*) context->v;
  if (i < 0 || i >= m->size() || !m->menu()) return mrb_nil_value();
  return mrb_fltk3_menuitem_new(mrb, m->menu() + i);
}

static mrb_value
mrb_fltk3_menu_items(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Menu_* m = (fltk3::Menu_*) context->v;
  mrb_value ary = mrb_ary_new(mrb);
  const fltk3::MenuItem* items = m->menu();
  for (int i = 0; items && i < m->size(); i++) {
    mrb_ary_push(mrb, ary, mrb_fltk3_menuitem_new(mrb, items + i));
  }
  return ary;
}

static mrb_value
mrb_fltk3_menu_find_index(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  fltk3::Menu_* m = (fltk3::Menu_*) context->v;
  if (mrb_string_p(v)) return mrb_fixnum_value(m->find_index(RSTRING_CSTR(mrb, v)));
  return mrb_fixnum_value(m->find_index(mrb_fltk3_MenuItem_ptr(mrb, v)));
}

static mrb_value
mrb_fltk3_menu_find_item(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value name;
  mrb_get_args(mrb, "S", &name);
  return mrb_fltk3_menuitem_new(mrb, ((fltk3::Menu_*) context->v)->find_item(RSTRING_CSTR(mrb, name)));
}

static mrb_value
mrb_fltk3_menu_item_pathname(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item = mrb_nil_value();
  mrb_get_args(mrb, "|o", &item);
  char name[1024];
  int r = ((fltk3::Menu_*) context->v)->item_pathname(name, sizeof(name),
    mrb_nil_p(item) ? NULL : mrb_fltk3_MenuItem_ptr(mrb, item));
  if (r != 0) return mrb_nil_value();
  return mrb_str_new_cstr(mrb, name);
}

static mrb_value
mrb_fltk3_menu_test_shortcut(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_menuitem_new(mrb, ((fltk3::Menu_*) context->v)->test_shortcut());
}

static mrb_value
mrb_fltk3_menu_down_box_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_Box_wrap(mrb, ((fltk3::Menu_*) context->v)->down_box(), "Box");
}

static mrb_value
mrb_fltk3_menu_down_box_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value box;
  mrb_get_args(mrb, "o", &box);
  ((fltk3::Menu_*) context->v)->down_box(mrb_fltk3_Box_ptr(mrb, box));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_menubutton_popup(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_menuitem_new(mrb, ((fltk3::MenuButton*) context->v)->popup());
}

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

#define DEFINE_ITEM_BOOL(z) \
  mrb_define_method(mrb, _class_fltk3_MenuItem, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(MenuItem); \
    return mrb_bool_value(context->v->z() != 0); \
  }, MRB_ARGS_NONE());

#define DEFINE_ITEM_VOID(z) \
  mrb_define_method(mrb, _class_fltk3_MenuItem, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(MenuItem); \
    context->v->z(); \
    return mrb_nil_value(); \
  }, MRB_ARGS_NONE());

void
mrb_fltk3_menu_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");

  DEFINE_INT_CONST(MENU_INACTIVE);
  DEFINE_INT_CONST(MENU_TOGGLE);
  DEFINE_INT_CONST(MENU_VALUE);
  DEFINE_INT_CONST(MENU_RADIO);
  DEFINE_INT_CONST(MENU_INVISIBLE);
  DEFINE_INT_CONST(SUBMENU_POINTER);
  DEFINE_INT_CONST(SUBMENU);
  DEFINE_INT_CONST(MENU_DIVIDER);
  DEFINE_INT_CONST(MENU_HORIZONTAL);

  struct RClass* _class_fltk3_MenuItem = mrb_define_class_under(mrb, _class_fltk3, "MenuItem", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_MenuItem, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_MenuItem, "initialize", mrb_fltk3_MenuItem_init, MRB_ARGS_OPT(3));
  DEFINE_STR_GETTER(MenuItem, MenuItem, label);
  mrb_define_method(mrb, _class_fltk3_MenuItem, "label=", mrb_fltk3_menuitem_label_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_MenuItem, "text", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(MenuItem);
    return context->v->text ? mrb_str_new_cstr(mrb, context->v->text) : mrb_nil_value();
  }, MRB_ARGS_NONE());
  DEFINE_INT_PROP(MenuItem, MenuItem, shortcut);
  DEFINE_INT_PROP(MenuItem, MenuItem, labelfont);
  DEFINE_INT_PROP(MenuItem, MenuItem, labelsize);
  DEFINE_INT_PROP(MenuItem, MenuItem, labelcolor);
  DEFINE_INT_PROP(MenuItem, MenuItem, argument);
  DEFINE_INT_GETTER(MenuItem, MenuItem, size);
  mrb_define_method(mrb, _class_fltk3_MenuItem, "labeltype", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(MenuItem);
    return mrb_fixnum_value((mrb_int) context->v->labeltype());
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_MenuItem, "labeltype=", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(MenuItem);
    mrb_int t;
    mrb_get_args(mrb, "i", &t);
    context->v->labeltype((fltk3::Labeltype) t);
    return mrb_nil_value();
  }, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_MenuItem, "flags", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(MenuItem);
    return mrb_fixnum_value(context->v->flags);
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_MenuItem, "flags=", mrb_fltk3_menuitem_flags_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_MenuItem, "next", mrb_fltk3_menuitem_next, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_MenuItem, "image=", mrb_fltk3_menuitem_image_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_MenuItem, "do_callback", mrb_fltk3_menuitem_do_callback, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_MenuItem, "popup", mrb_fltk3_menuitem_popup, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
  DEFINE_ITEM_BOOL(submenu);
  DEFINE_ITEM_BOOL(checkbox);
  DEFINE_ITEM_BOOL(radio);
  DEFINE_ITEM_BOOL(value);
  DEFINE_ITEM_BOOL(visible);
  DEFINE_ITEM_BOOL(active);
  DEFINE_ITEM_BOOL(activevisible);
  DEFINE_ITEM_BOOL(checked);
  DEFINE_ITEM_VOID(set);
  DEFINE_ITEM_VOID(clear);
  DEFINE_ITEM_VOID(setonly);
  DEFINE_ITEM_VOID(show);
  DEFINE_ITEM_VOID(hide);
  DEFINE_ITEM_VOID(activate);
  DEFINE_ITEM_VOID(deactivate);
  DEFINE_ITEM_VOID(check);
  DEFINE_ITEM_VOID(uncheck);
  ARENA_RESTORE;

  DEFINE_CLASS(Menu_, Widget);
  INHERIT_TEXT_PROPS(Menu_);
  DEFINE_INT_GETTER(Menu_, Widget, size);
  DEFINE_INT_PROP(Menu_, Widget, down_color);
  DEFINE_VOID_METHOD(Menu_, Widget, clear);
  DEFINE_VOID_METHOD(Menu_, Widget, global);
  mrb_define_method(mrb, _class_fltk3_Menu_, "add", mrb_fltk3_menu_add, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Menu_, "insert", mrb_fltk3_menu_insert, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Menu_, "remove", mrb_fltk3_menu_remove, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "clear_submenu", mrb_fltk3_menu_clear_submenu, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "replace", mrb_fltk3_menu_replace, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Menu_, "shortcut", mrb_fltk3_menu_shortcut_set_at, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Menu_, "mode", mrb_fltk3_menu_mode, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "value", mrb_fltk3_menu_value, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Menu_, "value=", mrb_fltk3_menu_value_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "mvalue", mrb_fltk3_menu_mvalue, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Menu_, "text", mrb_fltk3_menu_text, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "item", mrb_fltk3_menu_item, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "[]", mrb_fltk3_menu_item, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "items", mrb_fltk3_menu_items, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Menu_, "menu", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(Widget);
    return mrb_fltk3_menuitem_new(mrb, ((fltk3::Menu_*) context->v)->menu());
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Menu_, "find_index", mrb_fltk3_menu_find_index, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "find_item", mrb_fltk3_menu_find_item, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "item_pathname", mrb_fltk3_menu_item_pathname, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Menu_, "test_shortcut", mrb_fltk3_menu_test_shortcut, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Menu_, "down_box", mrb_fltk3_menu_down_box_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Menu_, "down_box=", mrb_fltk3_menu_down_box_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(MenuBar, Menu_);
  DEFINE_CLASS(MenuButton, Menu_);
  mrb_define_method(mrb, _class_fltk3_MenuButton, "popup", mrb_fltk3_menubutton_popup, MRB_ARGS_NONE());
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP1", mrb_fixnum_value(fltk3::MenuButton::POPUP1));
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP2", mrb_fixnum_value(fltk3::MenuButton::POPUP2));
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP12", mrb_fixnum_value(fltk3::MenuButton::POPUP12));
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP3", mrb_fixnum_value(fltk3::MenuButton::POPUP3));
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP13", mrb_fixnum_value(fltk3::MenuButton::POPUP13));
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP23", mrb_fixnum_value(fltk3::MenuButton::POPUP23));
  mrb_define_const(mrb, _class_fltk3_MenuButton, "POPUP123", mrb_fixnum_value(fltk3::MenuButton::POPUP123));
  ARENA_RESTORE;
  DEFINE_CLASS(Choice, Menu_);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
