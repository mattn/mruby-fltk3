#include "mrb_fltk3.h"
#include <fltk3/Browser_.h>
#include <fltk3/Browser.h>
#include <fltk3/SelectBrowser.h>
#include <fltk3/HoldBrowser.h>
#include <fltk3/MultiBrowser.h>
#include <fltk3/FileBrowser.h>
#include <fltk3/CheckBrowser.h>

/*********************************************************
 * FLTK3::Browser
 *********************************************************/
DECLARE_WIDGET(Browser)
DECLARE_WIDGET(SelectBrowser)
DECLARE_WIDGET(HoldBrowser)
DECLARE_WIDGET(MultiBrowser)
DECLARE_WIDGET(FileBrowser)
DECLARE_WIDGET(CheckBrowser)

static mrb_value
mrb_fltk3_Browser__init(mrb_state* mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "can't instantiate fltk3::Browser_ directly");
  return self;
}

static mrb_value
mrb_fltk3_browser_load(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value filename;
  mrb_get_args(mrb, "S", &filename);
  return mrb_bool_value(((fltk3::Browser*) context->v)->load(RSTRING_CSTR(mrb, filename)) != 0);
}

static mrb_value
mrb_fltk3_browser_text(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line = 0;
  mrb_value text = mrb_nil_value();
  mrb_int argc = mrb_get_args(mrb, "|iS", &line, &text);
  if (argc < 2) {
    if (argc == 0) line = ((fltk3::Browser*) context->v)->value();
    const char* s = ((fltk3::Browser*) context->v)->text((int) line);
    return s ? mrb_str_new_cstr(mrb, s) : mrb_nil_value();
  }
  ((fltk3::Browser*) context->v)->text((int) line, RSTRING_CSTR(mrb, text));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_icon(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line = 0;
  mrb_value image = mrb_nil_value();
  mrb_int argc = mrb_get_args(mrb, "|io", &line, &image);
  if (argc < 2) {
    if (argc == 0) line = ((fltk3::Browser*) context->v)->value();
    fltk3::Image* image = ((fltk3::Browser*) context->v)->icon((int) line);
    return mrb_fltk3_Image_wrap(mrb, image, mrb_fltk3_image_classname(image));
  }
  ((fltk3::Browser*) context->v)->icon((int) line, mrb_fltk3_Image_ptr(mrb, image));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value text = mrb_nil_value();
  mrb_get_args(mrb, "S", &text);
  ((fltk3::Browser*) context->v)->add(RSTRING_CSTR(mrb, text));
  return self;
}

static mrb_value
mrb_fltk3_browser_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line;
  mrb_value text = mrb_nil_value();
  mrb_get_args(mrb, "iS", &line, &text);
  ((fltk3::Browser*) context->v)->insert((int) line, RSTRING_CSTR(mrb, text));
  return self;
}

static mrb_value
mrb_fltk3_browser_remove(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line;
  mrb_get_args(mrb, "i", &line);
  ((fltk3::Browser*) context->v)->remove((int) line);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_move(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int to, from;
  mrb_get_args(mrb, "ii", &to, &from);
  ((fltk3::Browser*) context->v)->move((int) to, (int) from);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_swap(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int a, b;
  mrb_get_args(mrb, "ii", &a, &b);
  ((fltk3::Browser*) context->v)->swap((int) a, (int) b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_select(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line;
  mrb_bool val = TRUE;
  mrb_get_args(mrb, "i|b", &line, &val);
  return mrb_bool_value(((fltk3::Browser*) context->v)->select((int) line, val) != 0);
}

static mrb_value
mrb_fltk3_browser_selected(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line;
  mrb_get_args(mrb, "i", &line);
  return mrb_bool_value(((fltk3::Browser*) context->v)->selected((int) line) != 0);
}

static mrb_value
mrb_fltk3_browser_deselect(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool docallbacks = FALSE;
  mrb_get_args(mrb, "|b", &docallbacks);
  return mrb_bool_value(((fltk3::Browser*) context->v)->deselect(docallbacks) != 0);
}

/* show / hide / visible take an optional line number, otherwise act on the widget */
#define DEFINE_LINE_METHOD(name, expr_line, expr_widget) \
static mrb_value \
mrb_fltk3_browser_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  fltk3::Browser* b = (fltk3::Browser*) context->v; \
  mrb_int line; \
  if (mrb_get_args(mrb, "|i", &line) == 0) { return expr_widget; } \
  return expr_line; \
}

DEFINE_LINE_METHOD(show, (b->show((int) line), mrb_nil_value()), (b->show(), mrb_nil_value()))
DEFINE_LINE_METHOD(hide, (b->hide((int) line), mrb_nil_value()), (b->hide(), mrb_nil_value()))
DEFINE_LINE_METHOD(visible, mrb_bool_value(b->visible((int) line) != 0), mrb_bool_value(b->fltk3::Widget::visible() != 0))

static mrb_value
mrb_fltk3_browser_displayed(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line;
  mrb_get_args(mrb, "i", &line);
  return mrb_bool_value(((fltk3::Browser*) context->v)->displayed((int) line) != 0);
}

static mrb_value
mrb_fltk3_browser_lineposition(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line, pos;
  mrb_get_args(mrb, "ii", &line, &pos);
  ((fltk3::Browser*) context->v)->lineposition((int) line, (fltk3::Browser::LinePosition) pos);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_selected_lines(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Browser* b = (fltk3::Browser*) context->v;
  mrb_value ary = mrb_ary_new(mrb);
  for (int i = 1; i <= b->size(); i++) {
    if (b->selected(i)) mrb_ary_push(mrb, ary, mrb_fixnum_value(i));
  }
  return ary;
}

static mrb_value
mrb_fltk3_browser_each(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value blk;
  mrb_get_args(mrb, "&!", &blk);
  fltk3::Browser* b = (fltk3::Browser*) context->v;
  for (int i = 1; i <= b->size(); i++) {
    const char* s = b->text(i);
    mrb_value args[2];
    args[0] = s ? mrb_str_new_cstr(mrb, s) : mrb_nil_value();
    args[1] = mrb_fixnum_value(i);
    mrb_yield_argv(mrb, blk, 2, args);
  }
  return self;
}

static mrb_value
mrb_fltk3_browser_column_widths_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  const int* widths = ((fltk3::Browser*) context->v)->column_widths();
  mrb_value arr = mrb_ary_new(mrb);
  for (int n = 0; widths && widths[n]; n++) {
    mrb_ary_push(mrb, arr, mrb_fixnum_value(widths[n]));
  }
  return arr;
}

static mrb_value
mrb_fltk3_browser_column_widths_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value arr = mrb_nil_value();
  mrb_get_args(mrb, "A", &arr);
  mrb_int n, len = RARRAY_LEN(arr);
  /* the array must outlive the widget; keep it in a hidden string */
  mrb_value buf = mrb_str_new(mrb, NULL, sizeof(int) * (len + 1));
  int* widths = (int*) RSTRING_PTR(buf);
  for (n = 0; n < len; n++) {
    widths[n] = (int) mrb_integer(mrb_funcall(mrb, RARRAY_PTR(arr)[n], "to_i", 0));
  }
  widths[n] = 0;
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "column_widths"), buf);
  ((fltk3::Browser*) context->v)->column_widths(widths);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_format_char(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  char c[2] = { ((fltk3::Browser*) context->v)->format_char(), 0 };
  return mrb_str_new_cstr(mrb, c);
}

static mrb_value
mrb_fltk3_browser_format_char_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  ((fltk3::Browser*) context->v)->format_char(RSTRING_LEN(s) ? RSTRING_PTR(s)[0] : 0);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser_column_char(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  char c[2] = { ((fltk3::Browser*) context->v)->column_char(), 0 };
  return mrb_str_new_cstr(mrb, c);
}

static mrb_value
mrb_fltk3_browser_column_char_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  ((fltk3::Browser*) context->v)->column_char(RSTRING_LEN(s) ? RSTRING_PTR(s)[0] : 0);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser__sort(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int flags = 0;
  mrb_get_args(mrb, "|i", &flags);
  ((fltk3::Browser_*) context->v)->sort((int) flags);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_browser__has_scrollbar_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int mode;
  mrb_get_args(mrb, "i", &mode);
  ((fltk3::Browser_*) context->v)->has_scrollbar((uchar) mode);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::FileBrowser / FLTK3::CheckBrowser
 *********************************************************/
static mrb_value
mrb_fltk3_filebrowser_load(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value dir;
  mrb_get_args(mrb, "S", &dir);
  return mrb_fixnum_value(((fltk3::FileBrowser*) context->v)->load(RSTRING_CSTR(mrb, dir)));
}

static mrb_value
mrb_fltk3_filebrowser_filter_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S!", &s);
  /* fltk3 keeps the pointer */
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "filter"), s);
  ((fltk3::FileBrowser*) context->v)->filter(mrb_nil_p(s) ? NULL : RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_checkbrowser_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_bool checked = FALSE;
  mrb_get_args(mrb, "S|b", &s, &checked);
  return mrb_fixnum_value(((fltk3::CheckBrowser*) context->v)->add((char*) RSTRING_CSTR(mrb, s), checked));
}

static mrb_value
mrb_fltk3_checkbrowser_remove(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int item;
  mrb_get_args(mrb, "i", &item);
  return mrb_fixnum_value(((fltk3::CheckBrowser*) context->v)->remove((int) item));
}

static mrb_value
mrb_fltk3_checkbrowser_checked(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int item;
  mrb_bool b;
  if (mrb_get_args(mrb, "i|b", &item, &b) == 1) {
    return mrb_bool_value(((fltk3::CheckBrowser*) context->v)->checked((int) item) != 0);
  }
  ((fltk3::CheckBrowser*) context->v)->checked((int) item, b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_checkbrowser_text(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int item;
  mrb_get_args(mrb, "i", &item);
  const char* s = ((fltk3::CheckBrowser*) context->v)->text((int) item);
  return s ? mrb_str_new_cstr(mrb, s) : mrb_nil_value();
}

static mrb_value
mrb_fltk3_checkbrowser_checked_items(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::CheckBrowser* b = (fltk3::CheckBrowser*) context->v;
  mrb_value ary = mrb_ary_new(mrb);
  for (int i = 1; i <= b->nitems(); i++) {
    if (b->checked(i)) mrb_ary_push(mrb, ary, mrb_fixnum_value(i));
  }
  return ary;
}

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

#define INHERIT_BROWSER_(x) \
  DEFINE_INT_PROP(x, Widget, position); \
  DEFINE_INT_PROP(x, Widget, hposition); \
  DEFINE_INT_PROP(x, Widget, scrollbar_size); \
  DEFINE_INT_GETTER(x, Widget, has_scrollbar); \
  DEFINE_VOID_METHOD(x, Widget, scrollbar_right); \
  DEFINE_VOID_METHOD(x, Widget, scrollbar_left); \
  INHERIT_TEXT_PROPS(x); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "has_scrollbar=", mrb_fltk3_browser__has_scrollbar_set, MRB_ARGS_REQ(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "sort", mrb_fltk3_browser__sort, MRB_ARGS_OPT(1)); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "deselect", mrb_fltk3_browser_deselect, MRB_ARGS_OPT(1)); \
  ARENA_RESTORE;

void
mrb_fltk3_browser_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  DEFINE_INT_CONST(NORMAL_BROWSER);
  DEFINE_INT_CONST(SELECT_BROWSER);
  DEFINE_INT_CONST(HOLD_BROWSER);
  DEFINE_INT_CONST(MULTI_BROWSER);

  DEFINE_CLASS(Browser_, Group);
  INHERIT_BROWSER_(Browser_);
  mrb_define_const(mrb, _class_fltk3_Browser_, "HORIZONTAL", mrb_fixnum_value(fltk3::Browser_::HORIZONTAL));
  mrb_define_const(mrb, _class_fltk3_Browser_, "VERTICAL", mrb_fixnum_value(fltk3::Browser_::VERTICAL));
  mrb_define_const(mrb, _class_fltk3_Browser_, "BOTH", mrb_fixnum_value(fltk3::Browser_::BOTH));
  mrb_define_const(mrb, _class_fltk3_Browser_, "ALWAYS_ON", mrb_fixnum_value(fltk3::Browser_::ALWAYS_ON));
  mrb_define_const(mrb, _class_fltk3_Browser_, "HORIZONTAL_ALWAYS", mrb_fixnum_value(fltk3::Browser_::HORIZONTAL_ALWAYS));
  mrb_define_const(mrb, _class_fltk3_Browser_, "VERTICAL_ALWAYS", mrb_fixnum_value(fltk3::Browser_::VERTICAL_ALWAYS));
  mrb_define_const(mrb, _class_fltk3_Browser_, "BOTH_ALWAYS", mrb_fixnum_value(fltk3::Browser_::BOTH_ALWAYS));
  mrb_define_const(mrb, _class_fltk3_Browser_, "SORTASC", mrb_fixnum_value(0));
  mrb_define_const(mrb, _class_fltk3_Browser_, "SORTDESC", mrb_fixnum_value(1));
  ARENA_RESTORE;

  DEFINE_CLASS(Browser, Browser_);
  mrb_define_const(mrb, _class_fltk3_Browser, "TOP", mrb_fixnum_value(fltk3::Browser::TOP));
  mrb_define_const(mrb, _class_fltk3_Browser, "BOTTOM", mrb_fixnum_value(fltk3::Browser::BOTTOM));
  mrb_define_const(mrb, _class_fltk3_Browser, "MIDDLE", mrb_fixnum_value(fltk3::Browser::MIDDLE));
  mrb_define_method(mrb, _class_fltk3_Browser, "load", mrb_fltk3_browser_load, MRB_ARGS_REQ(1));
  DEFINE_INT_PROP(Browser, Widget, value);
  DEFINE_INT_GETTER(Browser, Widget, size);
  DEFINE_INT_PROP(Browser, Widget, topline);
  DEFINE_INT_SETTER(Browser, Widget, bottomline);
  DEFINE_INT_SETTER(Browser, Widget, middleline);
  DEFINE_INT_SETTER(Browser, Widget, make_visible);
  DEFINE_INT_SETTER(Browser, Widget, remove_icon);
  DEFINE_VOID_METHOD(Browser, Widget, clear);
  mrb_define_method(mrb, _class_fltk3_Browser, "text", mrb_fltk3_browser_text, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "icon", mrb_fltk3_browser_icon, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "add", mrb_fltk3_browser_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "<<", mrb_fltk3_browser_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "insert", mrb_fltk3_browser_insert, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "remove", mrb_fltk3_browser_remove, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "move", mrb_fltk3_browser_move, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "swap", mrb_fltk3_browser_swap, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "select", mrb_fltk3_browser_select, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "selected", mrb_fltk3_browser_selected, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "selected?", mrb_fltk3_browser_selected, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "selected_lines", mrb_fltk3_browser_selected_lines, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Browser, "show", mrb_fltk3_browser_show, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "hide", mrb_fltk3_browser_hide, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "visible", mrb_fltk3_browser_visible, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "displayed", mrb_fltk3_browser_displayed, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "lineposition", mrb_fltk3_browser_lineposition, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "each", mrb_fltk3_browser_each, MRB_ARGS_BLOCK());
  mrb_define_method(mrb, _class_fltk3_Browser, "column_widths", mrb_fltk3_browser_column_widths_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Browser, "column_widths=", mrb_fltk3_browser_column_widths_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "format_char", mrb_fltk3_browser_format_char, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Browser, "format_char=", mrb_fltk3_browser_format_char_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "column_char", mrb_fltk3_browser_column_char, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Browser, "column_char=", mrb_fltk3_browser_column_char_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(SelectBrowser, Browser);
  DEFINE_CLASS(HoldBrowser, Browser);
  DEFINE_CLASS(MultiBrowser, Browser);

  DEFINE_CLASS(FileBrowser, Browser);
  mrb_define_const(mrb, _class_fltk3_FileBrowser, "FILES", mrb_fixnum_value(fltk3::FileBrowser::FILES));
  mrb_define_const(mrb, _class_fltk3_FileBrowser, "DIRECTORIES", mrb_fixnum_value(fltk3::FileBrowser::DIRECTORIES));
  mrb_define_method(mrb, _class_fltk3_FileBrowser, "load", mrb_fltk3_filebrowser_load, MRB_ARGS_REQ(1));
  DEFINE_STR_GETTER(FileBrowser, Widget, filter);
  mrb_define_method(mrb, _class_fltk3_FileBrowser, "filter=", mrb_fltk3_filebrowser_filter_set, MRB_ARGS_REQ(1));
  DEFINE_INT_PROP(FileBrowser, Widget, filetype);
  DEFINE_INT_PROP(FileBrowser, Widget, iconsize);
  ARENA_RESTORE;

  DEFINE_CLASS(CheckBrowser, Browser_);
  DEFINE_INT_GETTER(CheckBrowser, Widget, nitems);
  DEFINE_INT_GETTER(CheckBrowser, Widget, nchecked);
  DEFINE_INT_GETTER(CheckBrowser, Widget, value);
  DEFINE_VOID_METHOD(CheckBrowser, Widget, clear);
  DEFINE_VOID_METHOD(CheckBrowser, Widget, check_all);
  DEFINE_VOID_METHOD(CheckBrowser, Widget, check_none);
  DEFINE_INT_SETTER(CheckBrowser, Widget, set_checked);
  mrb_define_method(mrb, _class_fltk3_CheckBrowser, "add", mrb_fltk3_checkbrowser_add, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_CheckBrowser, "remove", mrb_fltk3_checkbrowser_remove, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_CheckBrowser, "checked", mrb_fltk3_checkbrowser_checked, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_CheckBrowser, "checked?", mrb_fltk3_checkbrowser_checked, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_CheckBrowser, "checked_items", mrb_fltk3_checkbrowser_checked_items, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_CheckBrowser, "text", mrb_fltk3_checkbrowser_text, MRB_ARGS_REQ(1));
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
