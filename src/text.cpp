#include "mrb_fltk3.h"
#include <fltk3/TextBuffer.h>
#include <fltk3/TextDisplay.h>
#include <fltk3/TextEditor.h>

/*********************************************************
 * FLTK3::TextBuffer / FLTK3::TextDisplay / FLTK3::TextEditor
 *********************************************************/
DECLARE_WIDGET(TextDisplay)
DECLARE_WIDGET(TextEditor)

static mrb_value
mrb_fltk3_TextBuffer_init(mrb_state* mrb, mrb_value self)
{
  return mrb_fltk3_TextBuffer_setup(mrb, self, new fltk3::TextBuffer);
}

static mrb_value
mrb_fltk3_textdisplay_buffer_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  return mrb_fltk3_TextBuffer_wrap(mrb, ((fltk3::TextDisplay*) context->v)->buffer(), "TextBuffer");
}

static mrb_value
mrb_fltk3_textdisplay_buffer_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value textbuffer;
  mrb_get_args(mrb, "o", &textbuffer);
  ((fltk3::TextDisplay*) context->v)->buffer(mrb_fltk3_TextBuffer_ptr(mrb, textbuffer));
  return mrb_nil_value();
}

void
mrb_fltk3_text_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  struct RClass* _class_fltk3_TextBuffer = mrb_define_class_under(mrb, _class_fltk3, "TextBuffer", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_TextBuffer, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "initialize", mrb_fltk3_TextBuffer_init, MRB_ARGS_NONE());
  DEFINE_INT_GETTER(TextBuffer, TextBuffer, length);
  DEFINE_STR_PROP(TextBuffer, TextBuffer, text);
  ARENA_RESTORE;

  DEFINE_CLASS(TextDisplay, Group);
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "buffer", mrb_fltk3_textdisplay_buffer_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "buffer=", mrb_fltk3_textdisplay_buffer_set, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(TextEditor, TextDisplay);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
