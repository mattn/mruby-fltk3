#include "mrb_fltk3.h"
#include <fltk3/SharedImage.h>

/*********************************************************
 * FLTK3::Image
 *********************************************************/
static mrb_value
mrb_fltk3_Image_initialize(mrb_state* mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "can't instantiate fltk3::Image directly");
  return self;
}

static mrb_value
mrb_fltk3_image_release(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Image);
  fltk3::Image* image = context->v;
  mrb_fltk3_unregister(mrb, image);
  context->v = NULL;
  fltk3::SharedImage* shared = dynamic_cast<fltk3::SharedImage*>(image);
  if (shared) shared->release(); else delete image;
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_image_copy(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Image);
  mrb_int width, height;
  if (mrb_get_args(mrb, "|ii", &width, &height) == 0) {
    return mrb_fltk3_Image_wrap(mrb, context->v->copy(), "Image");
  }
  return mrb_fltk3_Image_wrap(mrb, context->v->copy(width, height), "Image");
}

static mrb_value
mrb_fltk3_sharedimage_get(mrb_state* mrb, mrb_value self)
{
  mrb_value filename;
  mrb_int w = 0, h = 0;
  mrb_get_args(mrb, "S|ii", &filename, &w, &h);
  fltk3::Image* image = (fltk3::Image*) fltk3::SharedImage::get(RSTRING_CSTR(mrb, filename), w, h);
  return mrb_fltk3_Image_wrap(mrb, image, "SharedImage");
}

void
mrb_fltk3_image_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Image = mrb_define_class_under(mrb, _class_fltk3, "Image", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_Image, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_Image, "initialize", mrb_fltk3_Image_initialize, MRB_ARGS_ANY());
  DEFINE_INT_GETTER(Image, Image, w);
  DEFINE_INT_GETTER(Image, Image, h);
  DEFINE_INT_GETTER(Image, Image, d);
  DEFINE_INT_GETTER(Image, Image, ld);
  mrb_define_method(mrb, _class_fltk3_Image, "release", mrb_fltk3_image_release, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Image, "copy", mrb_fltk3_image_copy, MRB_ARGS_OPT(2));
  ARENA_RESTORE;

  struct RClass* _class_fltk3_SharedImage = mrb_define_class_under(mrb, _class_fltk3, "SharedImage", _class_fltk3_Image);
  mrb_define_module_function(mrb, _class_fltk3_SharedImage, "get", mrb_fltk3_sharedimage_get, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
