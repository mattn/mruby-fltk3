#include "mrb_fltk3.h"
#include <fltk3/Browser.h>
#include <fltk3/SelectBrowser.h>

/*********************************************************
 * FLTK3::Browser
 *********************************************************/
DECLARE_WIDGET(Browser)
DECLARE_WIDGET(SelectBrowser)

static mrb_value
mrb_fltk3_browser_load(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value filename;
  mrb_get_args(mrb, "S", &filename);
  return mrb_fixnum_value(((fltk3::Browser*) context->v)->load(RSTRING_CSTR(mrb, filename)));
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
    return mrb_fltk3_Image_wrap(mrb, ((fltk3::Browser*) context->v)->icon((int) line), "Image");
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
  return mrb_nil_value();
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

void
mrb_fltk3_browser_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  DEFINE_CLASS(Browser, Group);
  mrb_define_method(mrb, _class_fltk3_Browser, "load", mrb_fltk3_browser_load, MRB_ARGS_REQ(1));
  DEFINE_INT_PROP(Browser, Widget, value);
  mrb_define_method(mrb, _class_fltk3_Browser, "text", mrb_fltk3_browser_text, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "icon", mrb_fltk3_browser_icon, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Browser, "add", mrb_fltk3_browser_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Browser, "column_widths", mrb_fltk3_browser_column_widths_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Browser, "column_widths=", mrb_fltk3_browser_column_widths_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(SelectBrowser, Browser);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
