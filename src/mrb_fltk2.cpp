#include <memory.h>
#include <mruby.h>
#include <mruby/proc.h>
#include <mruby/data.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/class.h>
#include <mruby/variable.h>
#include <fltk2/Window.h>
#include <fltk2/run.h>
#include <stdio.h>

#if 1
#define ARENA_SAVE \
  int ai = mrb_gc_arena_save(mrb); \
  if (ai == MRB_ARENA_SIZE) { \
    mrb_raise(mrb, E_RUNTIME_ERROR, "arena overflow"); \
  }
#define ARENA_RESTORE \
  mrb_gc_arena_restore(mrb, ai);
#else
#define ARENA_SAVE
#define ARENA_RESTORE
#endif

typedef struct {
  fltk::Widget* w;
  mrb_state* mrb;
} mrb_fltk2_context;

static void
fltk2_context_free(mrb_state *mrb, void *p) {
  free(p);
}

static const struct mrb_data_type fltk2_context_type = {
  "fltk2_context", fltk2_context_free,
};

static mrb_value
mrb_fltk2_window_init(mrb_state *mrb, mrb_value self)
{
  mrb_value *argv;
  int argc;

  mrb_get_args(mrb, "*", &argv, &argc);

  mrb_fltk2_context* context = (mrb_fltk2_context*) malloc(sizeof(mrb_fltk2_context));
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");
  memset(context, 0, sizeof(mrb_fltk2_context));
  context->mrb = mrb;
  // TODO
  context->w = (fltk::Widget*) new fltk::Window(0, 0, "mruby-fltk");

  mrb_iv_set(mrb, self, mrb_intern(mrb, "context"), mrb_obj_value(
    Data_Wrap_Struct(mrb, mrb->object_class,
    &fltk2_context_type, (void*) context)));
  return self;
}

static mrb_value
mrb_fltk2_run(mrb_state *mrb, mrb_value self)
{
  return mrb_fixnum_value(fltk::run());
}

extern "C"
{

void
mrb_mruby_fltk2_gem_init(mrb_state* mrb)
{
  ARENA_SAVE;
  struct RClass* _class_fltk2 = mrb_define_module(mrb, "FLTK2");
  mrb_define_module_function(mrb, _class_fltk2, "run", mrb_fltk2_run, ARGS_NONE());
  ARENA_RESTORE;

  struct RClass* _class_fltk2_window = mrb_define_class_under(mrb, _class_fltk2, "Window", mrb->object_class);
  mrb_define_method(mrb, _class_fltk2_window, "initialize", mrb_fltk2_window_init, ARGS_NONE());
  ARENA_RESTORE;
}

}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
