#include "mrb_fltk3.h"
#include <fltk3/Image.h>
#include <fltk3/RGBImage.h>
#include <fltk3/Bitmap.h>
#include <fltk3/Pixmap.h>
#include <fltk3/SharedImage.h>
#include <fltk3/TiledImage.h>
#include <fltk3/PNGImage.h>
#include <fltk3/JPEGImage.h>
#include <fltk3/GIFImage.h>
#include <fltk3/BMPImage.h>
#include <fltk3/XPMImage.h>
#include <fltk3/XBMImage.h>
#include <fltk3/PNMImage.h>

/*********************************************************
 * FLTK3::Image
 *********************************************************/
const char*
mrb_fltk3_image_classname(fltk3::Image* image)
{
  if (dynamic_cast<fltk3::SharedImage*>(image)) return "SharedImage";
  if (dynamic_cast<fltk3::TiledImage*>(image)) return "TiledImage";
  if (dynamic_cast<fltk3::PNGImage*>(image)) return "PNGImage";
  if (dynamic_cast<fltk3::JPEGImage*>(image)) return "JPEGImage";
  if (dynamic_cast<fltk3::GIFImage*>(image)) return "GIFImage";
  if (dynamic_cast<fltk3::BMPImage*>(image)) return "BMPImage";
  if (dynamic_cast<fltk3::PNMImage*>(image)) return "PNMImage";
  if (dynamic_cast<fltk3::RGBImage*>(image)) return "RGBImage";
  if (dynamic_cast<fltk3::XPMImage*>(image)) return "XPMImage";
  if (dynamic_cast<fltk3::Pixmap*>(image)) return "Pixmap";
  if (dynamic_cast<fltk3::XBMImage*>(image)) return "XBMImage";
  if (dynamic_cast<fltk3::Bitmap*>(image)) return "Bitmap";
  return "Image";
}

static mrb_value
mrb_fltk3_image_wrap(mrb_state* mrb, fltk3::Image* image)
{
  return mrb_fltk3_Image_wrap(mrb, image, mrb_fltk3_image_classname(image));
}

static mrb_value
mrb_fltk3_Image_initialize(mrb_state* mrb, mrb_value self)
{
  mrb_int w, h, d;
  mrb_get_args(mrb, "iii", &w, &h, &d);
  return mrb_fltk3_Image_setup(mrb, self, new fltk3::Image((int) w, (int) h, (int) d));
}

/* RGBImage.new(data, w, h, d = 3, ld = 0); the data string is kept alive */
static mrb_value
mrb_fltk3_RGBImage_initialize(mrb_state* mrb, mrb_value self)
{
  mrb_value data;
  mrb_int w, h, d = 3, ld = 0;
  mrb_get_args(mrb, "Sii|ii", &data, &w, &h, &d, &ld);
  mrb_int need = (ld ? ld : w * d) * h;
  if (RSTRING_LEN(data) < need) mrb_raise(mrb, E_ARGUMENT_ERROR, "image data is too short");
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "data"), data);
  return mrb_fltk3_Image_setup(mrb, self, new fltk3::RGBImage((const uchar*) RSTRING_PTR(data), (int) w, (int) h, (int) d, (int) ld));
}

/* Bitmap.new(bits, w, h) */
static mrb_value
mrb_fltk3_Bitmap_initialize(mrb_state* mrb, mrb_value self)
{
  mrb_value data;
  mrb_int w, h;
  mrb_get_args(mrb, "Sii", &data, &w, &h);
  if (RSTRING_LEN(data) < ((w + 7) / 8) * h) mrb_raise(mrb, E_ARGUMENT_ERROR, "bitmap data is too short");
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "data"), data);
  return mrb_fltk3_Image_setup(mrb, self, new fltk3::Bitmap((const uchar*) RSTRING_PTR(data), (int) w, (int) h));
}

/* Pixmap.new(["w h ncolors cpp", "c c #rrggbb", ...rows]) */
static mrb_value
mrb_fltk3_Pixmap_initialize(mrb_state* mrb, mrb_value self)
{
  mrb_value ary;
  mrb_get_args(mrb, "A", &ary);
  mrb_int n = RARRAY_LEN(ary);
  if (n < 1) mrb_raise(mrb, E_ARGUMENT_ERROR, "pixmap data is empty");
  mrb_value rows = mrb_ary_new_capa(mrb, n);
  mrb_value buf = mrb_str_new(mrb, NULL, sizeof(char*) * (n + 1));
  const char** ptrs = (const char**) RSTRING_PTR(buf);
  for (mrb_int i = 0; i < n; i++) {
    mrb_value row = mrb_str_dup(mrb, mrb_ary_ref(mrb, ary, i));
    mrb_ary_push(mrb, rows, row);
    ptrs[i] = RSTRING_CSTR(mrb, row);
  }
  ptrs[n] = NULL;
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "data"), rows);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "pointers"), buf);
  return mrb_fltk3_Image_setup(mrb, self, new fltk3::Pixmap(ptrs));
}

/* TiledImage.new(image, w = 0, h = 0) */
static mrb_value
mrb_fltk3_TiledImage_initialize(mrb_state* mrb, mrb_value self)
{
  mrb_value image;
  mrb_int w = 0, h = 0;
  mrb_get_args(mrb, "o|ii", &image, &w, &h);
  mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "image"), image);
  return mrb_fltk3_Image_setup(mrb, self, new fltk3::TiledImage(mrb_fltk3_Image_ptr(mrb, image), (int) w, (int) h));
}

#define DECLARE_FILE_IMAGE(x)                                             \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _initialize(mrb_state* mrb, mrb_value self)            \
{                                                                         \
  mrb_value filename;                                                     \
  mrb_get_args(mrb, "S", &filename);                                      \
  return mrb_fltk3_Image_setup(mrb, self, new fltk3::x(RSTRING_CSTR(mrb, filename))); \
}

DECLARE_FILE_IMAGE(PNGImage)
DECLARE_FILE_IMAGE(JPEGImage)
DECLARE_FILE_IMAGE(GIFImage)
DECLARE_FILE_IMAGE(BMPImage)
DECLARE_FILE_IMAGE(XPMImage)
DECLARE_FILE_IMAGE(XBMImage)
DECLARE_FILE_IMAGE(PNMImage)

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
    return mrb_fltk3_image_wrap(mrb, context->v->copy());
  }
  return mrb_fltk3_image_wrap(mrb, context->v->copy(width, height));
}

static mrb_value
mrb_fltk3_image_color_average(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Image);
  mrb_int c;
  mrb_float i;
  mrb_get_args(mrb, "if", &c, &i);
  context->v->color_average((fltk3::Color) c, (float) i);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_image_draw(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Image);
  mrb_int x, y, w, h, cx = 0, cy = 0;
  mrb_int argc = mrb_get_args(mrb, "ii|iiii", &x, &y, &w, &h, &cx, &cy);
  if (argc == 2) context->v->draw((int) x, (int) y);
  else if (argc >= 4) context->v->draw((int) x, (int) y, (int) w, (int) h, (int) cx, (int) cy);
  else mrb_raise(mrb, E_ARGUMENT_ERROR, "wrong number of arguments");
  return mrb_nil_value();
}

/* raw pixel data as a string (RGB images and bitmaps only) */
static mrb_value
mrb_fltk3_image_data(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Image);
  const char* const* data = context->v->data();
  if (!data || !data[0] || context->v->count() != 1) return mrb_nil_value();
  int w = context->v->w(), h = context->v->h(), d = context->v->d(), ld = context->v->ld();
  int len = d == 0 ? ((w + 7) / 8) * h : (ld ? ld : w * d) * h;
  return mrb_str_new(mrb, data[0], len);
}

static mrb_value
mrb_fltk3_image_fail(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Image);
  return mrb_bool_value(context->v->w() <= 0 || context->v->h() <= 0 || context->v->d() < 0);
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

static mrb_value
mrb_fltk3_sharedimage_find(mrb_state* mrb, mrb_value self)
{
  mrb_value filename;
  mrb_int w = 0, h = 0;
  mrb_get_args(mrb, "S|ii", &filename, &w, &h);
  fltk3::Image* image = (fltk3::Image*) fltk3::SharedImage::find(RSTRING_CSTR(mrb, filename), w, h);
  return mrb_fltk3_Image_wrap(mrb, image, "SharedImage");
}

#define DEFINE_IMAGE_CLASS(x, y) \
  struct RClass* _class_fltk3_ ## x = mrb_define_class_under(mrb, _class_fltk3, # x, _class_fltk3_ ## y); \
  mrb_define_method(mrb, _class_fltk3_ ## x, "initialize", mrb_fltk3_ ## x ## _initialize, MRB_ARGS_ANY()); \
  ARENA_RESTORE;

void
mrb_fltk3_image_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Image = mrb_define_class_under(mrb, _class_fltk3, "Image", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_Image, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_Image, "initialize", mrb_fltk3_Image_initialize, MRB_ARGS_REQ(3));
  DEFINE_INT_GETTER(Image, Image, w);
  DEFINE_INT_GETTER(Image, Image, h);
  DEFINE_INT_GETTER(Image, Image, d);
  DEFINE_INT_GETTER(Image, Image, ld);
  DEFINE_INT_GETTER(Image, Image, count);
  DEFINE_VOID_METHOD(Image, Image, inactive);
  DEFINE_VOID_METHOD(Image, Image, desaturate);
  DEFINE_VOID_METHOD(Image, Image, uncache);
  mrb_define_method(mrb, _class_fltk3_Image, "release", mrb_fltk3_image_release, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Image, "copy", mrb_fltk3_image_copy, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Image, "color_average", mrb_fltk3_image_color_average, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Image, "draw", mrb_fltk3_image_draw, MRB_ARGS_REQ(2) | MRB_ARGS_OPT(4));
  mrb_define_method(mrb, _class_fltk3_Image, "data", mrb_fltk3_image_data, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Image, "fail", mrb_fltk3_image_fail, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Image, "fail?", mrb_fltk3_image_fail, MRB_ARGS_NONE());
  ARENA_RESTORE;

  struct RClass* _class_fltk3_SharedImage = mrb_define_class_under(mrb, _class_fltk3, "SharedImage", _class_fltk3_Image);
  mrb_define_module_function(mrb, _class_fltk3_SharedImage, "get", mrb_fltk3_sharedimage_get, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  mrb_define_module_function(mrb, _class_fltk3_SharedImage, "find", mrb_fltk3_sharedimage_find, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(2));
  ARENA_RESTORE;

  DEFINE_IMAGE_CLASS(RGBImage, Image);
  DEFINE_IMAGE_CLASS(Bitmap, Image);
  DEFINE_IMAGE_CLASS(Pixmap, Image);
  DEFINE_IMAGE_CLASS(TiledImage, Image);
  DEFINE_IMAGE_CLASS(PNGImage, RGBImage);
  DEFINE_IMAGE_CLASS(JPEGImage, RGBImage);
  DEFINE_IMAGE_CLASS(GIFImage, RGBImage);
  DEFINE_IMAGE_CLASS(BMPImage, RGBImage);
  DEFINE_IMAGE_CLASS(PNMImage, RGBImage);
  DEFINE_IMAGE_CLASS(XPMImage, Pixmap);
  DEFINE_IMAGE_CLASS(XBMImage, Bitmap);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
