#include "mrb_fltk3.h"
#include <fltk3/TextBuffer.h>
#include <fltk3/TextDisplay.h>
#include <fltk3/TextEditor.h>

/*********************************************************
 * FLTK3::TextBuffer
 *********************************************************/
DECLARE_WIDGET(TextDisplay)
DECLARE_WIDGET(TextEditor)

static mrb_value
mrb_fltk3_TextBuffer_init(mrb_state* mrb, mrb_value self)
{
  mrb_int size = 0;
  mrb_get_args(mrb, "|i", &size);
  return mrb_fltk3_TextBuffer_setup(mrb, self, new fltk3::TextBuffer((int) size));
}

/* fltk3 returns malloc'ed copies of buffer text */
static mrb_value
mrb_fltk3_take_text(mrb_state* mrb, char* text)
{
  if (!text) return mrb_nil_value();
  mrb_value s = mrb_str_new_cstr(mrb, text);
  free(text);
  return s;
}

static mrb_value
mrb_fltk3_textbuffer_text_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  return mrb_fltk3_take_text(mrb, context->v->text());
}

static mrb_value
mrb_fltk3_textbuffer_text_range(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start, end;
  mrb_get_args(mrb, "ii", &start, &end);
  return mrb_fltk3_take_text(mrb, context->v->text_range((int) start, (int) end));
}

static mrb_value
mrb_fltk3_textbuffer_selection_text(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  return mrb_fltk3_take_text(mrb, context->v->selection_text());
}

static mrb_value
mrb_fltk3_textbuffer_selection_position(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  int start = 0, end = 0;
  if (!context->v->selection_position(&start, &end)) return mrb_nil_value();
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(start));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(end));
  return ret;
}

static mrb_value
mrb_fltk3_textbuffer_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int pos;
  mrb_value text;
  mrb_get_args(mrb, "iS", &pos, &text);
  context->v->insert((int) pos, RSTRING_CSTR(mrb, text));
  return self;
}

static mrb_value
mrb_fltk3_textbuffer_append(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_value text;
  mrb_get_args(mrb, "S", &text);
  context->v->append(RSTRING_CSTR(mrb, text));
  return self;
}

static mrb_value
mrb_fltk3_textbuffer_remove(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start, end;
  mrb_get_args(mrb, "ii", &start, &end);
  context->v->remove((int) start, (int) end);
  return self;
}

static mrb_value
mrb_fltk3_textbuffer_replace(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start, end;
  mrb_value text;
  mrb_get_args(mrb, "iiS", &start, &end, &text);
  context->v->replace((int) start, (int) end, RSTRING_CSTR(mrb, text));
  return self;
}

static mrb_value
mrb_fltk3_textbuffer_replace_selection(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_value text;
  mrb_get_args(mrb, "S", &text);
  context->v->replace_selection(RSTRING_CSTR(mrb, text));
  return self;
}

static mrb_value
mrb_fltk3_textbuffer_copy(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_value from;
  mrb_int start, end, to;
  mrb_get_args(mrb, "oiii", &from, &start, &end, &to);
  context->v->copy(mrb_fltk3_TextBuffer_ptr(mrb, from), (int) start, (int) end, (int) to);
  return self;
}

static mrb_value
mrb_fltk3_textbuffer_undo(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  int cursor = 0;
  if (!context->v->undo(&cursor)) return mrb_nil_value();
  return mrb_fixnum_value(cursor);
}

static mrb_value
mrb_fltk3_textbuffer_can_undo_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_bool b;
  mrb_get_args(mrb, "b", &b);
  context->v->canUndo(b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textbuffer_select(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start, end;
  mrb_get_args(mrb, "ii", &start, &end);
  context->v->select((int) start, (int) end);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textbuffer_highlight(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start, end;
  if (mrb_get_args(mrb, "|ii", &start, &end) == 0) {
    return mrb_bool_value(context->v->highlight() != 0);
  }
  context->v->highlight((int) start, (int) end);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textbuffer_highlight_position(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  int start = 0, end = 0;
  if (!context->v->highlight_position(&start, &end)) return mrb_nil_value();
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(start));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(end));
  return ret;
}

#define DEFINE_FILE_METHOD(name, call) \
static mrb_value \
mrb_fltk3_textbuffer_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(TextBuffer); \
  mrb_value file; \
  mrb_int pos = 0, buflen = 128 * 1024; \
  mrb_get_args(mrb, "S|ii", &file, &pos, &buflen); \
  return mrb_fixnum_value(call); \
}

DEFINE_FILE_METHOD(loadfile, context->v->loadfile(RSTRING_CSTR(mrb, file), (int) (pos ? pos : buflen)))
DEFINE_FILE_METHOD(appendfile, context->v->appendfile(RSTRING_CSTR(mrb, file), (int) (pos ? pos : buflen)))
DEFINE_FILE_METHOD(savefile, context->v->savefile(RSTRING_CSTR(mrb, file), (int) (pos ? pos : buflen)))
DEFINE_FILE_METHOD(insertfile, context->v->insertfile(RSTRING_CSTR(mrb, file), (int) pos, (int) buflen))

static mrb_value
mrb_fltk3_textbuffer_outputfile(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_value file;
  mrb_int start, end;
  mrb_get_args(mrb, "Sii", &file, &start, &end);
  return mrb_fixnum_value(context->v->outputfile(RSTRING_CSTR(mrb, file), (int) start, (int) end));
}

#define DEFINE_POS_METHOD(name) \
static mrb_value \
mrb_fltk3_textbuffer_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(TextBuffer); \
  mrb_int pos; \
  mrb_get_args(mrb, "i", &pos); \
  return mrb_fixnum_value(context->v->name((int) pos)); \
}

DEFINE_POS_METHOD(line_start)
DEFINE_POS_METHOD(line_end)
DEFINE_POS_METHOD(word_start)
DEFINE_POS_METHOD(word_end)
DEFINE_POS_METHOD(char_at)
DEFINE_POS_METHOD(byte_at)
DEFINE_POS_METHOD(next_char)
DEFINE_POS_METHOD(prev_char)

#define DEFINE_POS2_METHOD(name) \
static mrb_value \
mrb_fltk3_textbuffer_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(TextBuffer); \
  mrb_int a, b; \
  mrb_get_args(mrb, "ii", &a, &b); \
  return mrb_fixnum_value(context->v->name((int) a, (int) b)); \
}

DEFINE_POS2_METHOD(count_lines)
DEFINE_POS2_METHOD(skip_lines)
DEFINE_POS2_METHOD(rewind_lines)
DEFINE_POS2_METHOD(count_displayed_characters)
DEFINE_POS2_METHOD(skip_displayed_characters)

static mrb_value
mrb_fltk3_textbuffer_search_forward(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start;
  mrb_value str;
  mrb_bool match_case = FALSE;
  mrb_get_args(mrb, "iS|b", &start, &str, &match_case);
  int found = 0;
  if (!context->v->search_forward((int) start, RSTRING_CSTR(mrb, str), &found, match_case)) return mrb_nil_value();
  return mrb_fixnum_value(found);
}

static mrb_value
mrb_fltk3_textbuffer_search_backward(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_int start;
  mrb_value str;
  mrb_bool match_case = FALSE;
  mrb_get_args(mrb, "iS|b", &start, &str, &match_case);
  int found = 0;
  if (!context->v->search_backward((int) start, RSTRING_CSTR(mrb, str), &found, match_case)) return mrb_nil_value();
  return mrb_fixnum_value(found);
}

typedef struct {
  mrb_state* mrb;
  mrb_value instance;
} mrb_fltk3_textbuffer_callback_data;

/* yields (pos, inserted, deleted, restyled, deleted_text) to every block
 * registered with add_modify_callback */
static void
_mrb_fltk3_textbuffer_modify_callback(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText, void* cbArg)
{
  mrb_fltk3_textbuffer_callback_data* data = (mrb_fltk3_textbuffer_callback_data*) cbArg;
  mrb_state* mrb = data->mrb;
  mrb_value callbacks = mrb_iv_get(mrb, data->instance, mrb_intern_lit(mrb, "modify_callbacks"));
  if (!mrb_array_p(callbacks)) return;
  int ai = mrb_gc_arena_save(mrb);
  mrb_value args[5];
  args[0] = mrb_fixnum_value(pos);
  args[1] = mrb_fixnum_value(nInserted);
  args[2] = mrb_fixnum_value(nDeleted);
  args[3] = mrb_fixnum_value(nRestyled);
  args[4] = deletedText ? mrb_str_new_cstr(mrb, deletedText) : mrb_nil_value();
  for (mrb_int i = 0; i < RARRAY_LEN(callbacks); i++) {
    mrb_yield_argv(mrb, mrb_ary_ref(mrb, callbacks, i), 5, args);
  }
  mrb_gc_arena_restore(mrb, ai);
}

static mrb_value
mrb_fltk3_textbuffer_add_modify_callback(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_value b;
  mrb_get_args(mrb, "&!", &b);
  mrb_value callbacks = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "modify_callbacks"));
  if (!mrb_array_p(callbacks)) {
    callbacks = mrb_ary_new(mrb);
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "modify_callbacks"), callbacks);
    mrb_value buf = mrb_str_new(mrb, NULL, sizeof(mrb_fltk3_textbuffer_callback_data));
    mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "modify_callback_data"), buf);
    mrb_fltk3_textbuffer_callback_data* data = (mrb_fltk3_textbuffer_callback_data*) RSTRING_PTR(buf);
    data->mrb = mrb;
    data->instance = self;
    context->v->add_modify_callback(_mrb_fltk3_textbuffer_modify_callback, data);
  }
  mrb_ary_push(mrb, callbacks, b);
  return b;
}

static mrb_value
mrb_fltk3_textbuffer_remove_modify_callback(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TextBuffer);
  mrb_value b;
  mrb_get_args(mrb, "o", &b);
  mrb_value callbacks = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "modify_callbacks"));
  if (!mrb_array_p(callbacks)) return mrb_nil_value();
  mrb_value rest = mrb_ary_new(mrb);
  for (mrb_int i = 0; i < RARRAY_LEN(callbacks); i++) {
    mrb_value cb = mrb_ary_ref(mrb, callbacks, i);
    if (!mrb_obj_equal(mrb, cb, b)) mrb_ary_push(mrb, rest, cb);
  }
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "modify_callbacks"), rest);
  return mrb_nil_value();
}

/*********************************************************
 * FLTK3::TextDisplay / FLTK3::TextEditor
 *********************************************************/
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
  /* the display does not own the buffer; keep the Ruby object alive */
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "buffer"), textbuffer);
  ((fltk3::TextDisplay*) context->v)->buffer(mrb_fltk3_TextBuffer_ptr(mrb, textbuffer));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textdisplay_scroll(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int line, offset = 0;
  mrb_get_args(mrb, "i|i", &line, &offset);
  ((fltk3::TextDisplay*) context->v)->scroll((int) line, (int) offset);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textdisplay_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value text;
  mrb_get_args(mrb, "S", &text);
  ((fltk3::TextDisplay*) context->v)->insert(RSTRING_CSTR(mrb, text));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textdisplay_overstrike(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value text;
  mrb_get_args(mrb, "S", &text);
  ((fltk3::TextDisplay*) context->v)->overstrike(RSTRING_CSTR(mrb, text));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textdisplay_show_cursor(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_bool b = TRUE;
  mrb_get_args(mrb, "|b", &b);
  ((fltk3::TextDisplay*) context->v)->show_cursor(b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textdisplay_wrap_mode(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int wrap, margin = 0;
  mrb_get_args(mrb, "i|i", &wrap, &margin);
  ((fltk3::TextDisplay*) context->v)->wrap_mode((int) wrap, (int) margin);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_textdisplay_in_selection(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int x, y;
  mrb_get_args(mrb, "ii", &x, &y);
  return mrb_bool_value(((fltk3::TextDisplay*) context->v)->in_selection((int) x, (int) y) != 0);
}

static mrb_value
mrb_fltk3_textdisplay_position_to_xy(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_int pos;
  mrb_get_args(mrb, "i", &pos);
  int x = 0, y = 0;
  if (!((fltk3::TextDisplay*) context->v)->position_to_xy((int) pos, &x, &y)) return mrb_nil_value();
  mrb_value ret = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ret, mrb_fixnum_value(x));
  mrb_ary_push(mrb, ret, mrb_fixnum_value(y));
  return ret;
}

/* highlight_data(style_buffer, [[color, font, size], ...]) */
static mrb_value
mrb_fltk3_textdisplay_highlight_data(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value style_buffer, table;
  mrb_get_args(mrb, "oA", &style_buffer, &table);
  mrb_int n = RARRAY_LEN(table);
  if (n < 1) mrb_raise(mrb, E_ARGUMENT_ERROR, "style table is empty");
  mrb_value buf = mrb_str_new(mrb, NULL, sizeof(fltk3::TextDisplay::StyleTableEntry) * n);
  fltk3::TextDisplay::StyleTableEntry* entries = (fltk3::TextDisplay::StyleTableEntry*) RSTRING_PTR(buf);
  for (mrb_int i = 0; i < n; i++) {
    mrb_value e = mrb_ary_ref(mrb, table, i);
    if (!mrb_array_p(e) || RARRAY_LEN(e) < 3) mrb_raise(mrb, E_ARGUMENT_ERROR, "style entry must be [color, font, size]");
    entries[i].color = (fltk3::Color) mrb_integer(mrb_ary_ref(mrb, e, 0));
    entries[i].font = (fltk3::Font) mrb_integer(mrb_ary_ref(mrb, e, 1));
    entries[i].size = (fltk3::Fontsize) mrb_integer(mrb_ary_ref(mrb, e, 2));
    entries[i].attr = RARRAY_LEN(e) > 3 ? (unsigned) mrb_integer(mrb_ary_ref(mrb, e, 3)) : 0;
  }
  /* the display keeps pointers to both the style buffer and the table */
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "style_buffer"), style_buffer);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "style_table"), buf);
  ((fltk3::TextDisplay*) context->v)->highlight_data(mrb_fltk3_TextBuffer_ptr(mrb, style_buffer), entries, (int) n, 'A', NULL, NULL);
  return mrb_nil_value();
}

#define DEFINE_KF_METHOD(name) \
  mrb_define_method(mrb, _class_fltk3_TextEditor, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Widget); \
    mrb_int c = 0; \
    mrb_get_args(mrb, "|i", &c); \
    return mrb_fixnum_value(fltk3::TextEditor::name((int) c, (fltk3::TextEditor*) context->v)); \
  }, MRB_ARGS_OPT(1));

#define DEFINE_INT_METHOD(x, z) \
  mrb_define_method(mrb, _class_fltk3_ ## x, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Widget); \
    return mrb_fixnum_value(((fltk3::x*) context->v)->z()); \
  }, MRB_ARGS_NONE());

void
mrb_fltk3_text_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  struct RClass* _class_fltk3_TextBuffer = mrb_define_class_under(mrb, _class_fltk3, "TextBuffer", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_TextBuffer, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "initialize", mrb_fltk3_TextBuffer_init, MRB_ARGS_OPT(1));
  DEFINE_INT_GETTER(TextBuffer, TextBuffer, length);
  DEFINE_INT_PROP(TextBuffer, TextBuffer, tab_distance);
  DEFINE_STR_SETTER(TextBuffer, TextBuffer, text);
  DEFINE_BOOL_GETTER(TextBuffer, TextBuffer, selected);
  DEFINE_VOID_METHOD(TextBuffer, TextBuffer, unselect);
  DEFINE_VOID_METHOD(TextBuffer, TextBuffer, remove_selection);
  DEFINE_VOID_METHOD(TextBuffer, TextBuffer, unhighlight);
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "text", mrb_fltk3_textbuffer_text_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "to_s", mrb_fltk3_textbuffer_text_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "text_range", mrb_fltk3_textbuffer_text_range, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "selection_text", mrb_fltk3_textbuffer_selection_text, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "selection_position", mrb_fltk3_textbuffer_selection_position, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "insert", mrb_fltk3_textbuffer_insert, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "append", mrb_fltk3_textbuffer_append, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "<<", mrb_fltk3_textbuffer_append, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "remove", mrb_fltk3_textbuffer_remove, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "replace", mrb_fltk3_textbuffer_replace, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "replace_selection", mrb_fltk3_textbuffer_replace_selection, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "copy", mrb_fltk3_textbuffer_copy, MRB_ARGS_REQ(4));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "undo", mrb_fltk3_textbuffer_undo, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "can_undo=", mrb_fltk3_textbuffer_can_undo_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "select", mrb_fltk3_textbuffer_select, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "highlight", mrb_fltk3_textbuffer_highlight, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "highlight_position", mrb_fltk3_textbuffer_highlight_position, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "loadfile", mrb_fltk3_textbuffer_loadfile, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "appendfile", mrb_fltk3_textbuffer_appendfile, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "savefile", mrb_fltk3_textbuffer_savefile, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "insertfile", mrb_fltk3_textbuffer_insertfile, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "outputfile", mrb_fltk3_textbuffer_outputfile, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "line_start", mrb_fltk3_textbuffer_line_start, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "line_end", mrb_fltk3_textbuffer_line_end, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "word_start", mrb_fltk3_textbuffer_word_start, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "word_end", mrb_fltk3_textbuffer_word_end, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "char_at", mrb_fltk3_textbuffer_char_at, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "byte_at", mrb_fltk3_textbuffer_byte_at, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "next_char", mrb_fltk3_textbuffer_next_char, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "prev_char", mrb_fltk3_textbuffer_prev_char, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "count_lines", mrb_fltk3_textbuffer_count_lines, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "skip_lines", mrb_fltk3_textbuffer_skip_lines, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "rewind_lines", mrb_fltk3_textbuffer_rewind_lines, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "count_displayed_characters", mrb_fltk3_textbuffer_count_displayed_characters, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "skip_displayed_characters", mrb_fltk3_textbuffer_skip_displayed_characters, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "search_forward", mrb_fltk3_textbuffer_search_forward, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "search_backward", mrb_fltk3_textbuffer_search_backward, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "add_modify_callback", mrb_fltk3_textbuffer_add_modify_callback, MRB_ARGS_BLOCK());
  mrb_define_method(mrb, _class_fltk3_TextBuffer, "remove_modify_callback", mrb_fltk3_textbuffer_remove_modify_callback, MRB_ARGS_REQ(1));
  ARENA_RESTORE;

  DEFINE_CLASS(TextDisplay, Group);
  INHERIT_TEXT_PROPS(TextDisplay);
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "NORMAL_CURSOR", mrb_fixnum_value(fltk3::TextDisplay::NORMAL_CURSOR));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "CARET_CURSOR", mrb_fixnum_value(fltk3::TextDisplay::CARET_CURSOR));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "DIM_CURSOR", mrb_fixnum_value(fltk3::TextDisplay::DIM_CURSOR));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "BLOCK_CURSOR", mrb_fixnum_value(fltk3::TextDisplay::BLOCK_CURSOR));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "HEAVY_CURSOR", mrb_fixnum_value(fltk3::TextDisplay::HEAVY_CURSOR));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "SIMPLE_CURSOR", mrb_fixnum_value(fltk3::TextDisplay::SIMPLE_CURSOR));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "WRAP_NONE", mrb_fixnum_value(fltk3::TextDisplay::WRAP_NONE));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "WRAP_AT_COLUMN", mrb_fixnum_value(fltk3::TextDisplay::WRAP_AT_COLUMN));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "WRAP_AT_PIXEL", mrb_fixnum_value(fltk3::TextDisplay::WRAP_AT_PIXEL));
  mrb_define_const(mrb, _class_fltk3_TextDisplay, "WRAP_AT_BOUNDS", mrb_fixnum_value(fltk3::TextDisplay::WRAP_AT_BOUNDS));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "buffer", mrb_fltk3_textdisplay_buffer_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "buffer=", mrb_fltk3_textdisplay_buffer_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "scroll", mrb_fltk3_textdisplay_scroll, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "insert", mrb_fltk3_textdisplay_insert, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "overstrike", mrb_fltk3_textdisplay_overstrike, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "show_cursor", mrb_fltk3_textdisplay_show_cursor, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "wrap_mode", mrb_fltk3_textdisplay_wrap_mode, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "in_selection", mrb_fltk3_textdisplay_in_selection, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "position_to_xy", mrb_fltk3_textdisplay_position_to_xy, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TextDisplay, "highlight_data", mrb_fltk3_textdisplay_highlight_data, MRB_ARGS_REQ(2));
  DEFINE_INT_PROP(TextDisplay, Widget, insert_position);
  DEFINE_INT_PROP(TextDisplay, Widget, cursor_color);
  DEFINE_INT_PROP(TextDisplay, Widget, scrollbar_width);
  DEFINE_INT_PROP(TextDisplay, Widget, scrollbar_align);
  DEFINE_INT_PROP(TextDisplay, Widget, shortcut);
  DEFINE_INT_PROP(TextDisplay, Widget, linenumber_width);
  DEFINE_INT_PROP(TextDisplay, Widget, linenumber_font);
  DEFINE_INT_PROP(TextDisplay, Widget, linenumber_size);
  DEFINE_INT_PROP(TextDisplay, Widget, linenumber_fgcolor);
  DEFINE_INT_PROP(TextDisplay, Widget, linenumber_bgcolor);
  DEFINE_INT_PROP(TextDisplay, Widget, linenumber_align);
  DEFINE_INT_SETTER(TextDisplay, Widget, cursor_style);
  DEFINE_VOID_METHOD(TextDisplay, Widget, show_insert_position);
  DEFINE_VOID_METHOD(TextDisplay, Widget, hide_cursor);
  DEFINE_VOID_METHOD(TextDisplay, Widget, next_word);
  DEFINE_VOID_METHOD(TextDisplay, Widget, previous_word);
  DEFINE_INT_METHOD(TextDisplay, move_right);
  DEFINE_INT_METHOD(TextDisplay, move_left);
  DEFINE_INT_METHOD(TextDisplay, move_up);
  DEFINE_INT_METHOD(TextDisplay, move_down);
  ARENA_RESTORE;

  DEFINE_CLASS(TextEditor, TextDisplay);
  DEFINE_BOOL_PROP(TextEditor, Widget, insert_mode);
  DEFINE_KF_METHOD(kf_default);
  DEFINE_KF_METHOD(kf_backspace);
  DEFINE_KF_METHOD(kf_enter);
  DEFINE_KF_METHOD(kf_home);
  DEFINE_KF_METHOD(kf_end);
  DEFINE_KF_METHOD(kf_left);
  DEFINE_KF_METHOD(kf_up);
  DEFINE_KF_METHOD(kf_right);
  DEFINE_KF_METHOD(kf_down);
  DEFINE_KF_METHOD(kf_page_up);
  DEFINE_KF_METHOD(kf_page_down);
  DEFINE_KF_METHOD(kf_insert);
  DEFINE_KF_METHOD(kf_delete);
  DEFINE_KF_METHOD(kf_copy);
  DEFINE_KF_METHOD(kf_cut);
  DEFINE_KF_METHOD(kf_paste);
  DEFINE_KF_METHOD(kf_select_all);
  DEFINE_KF_METHOD(kf_undo);
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
