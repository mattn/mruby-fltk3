#include "mrb_fltk3.h"
#include <fltk3/Box.h>

/*********************************************************
 * FLTK3::Box
 *
 * Box types are fltk3::Symbol objects, not widgets.  The standard box
 * types are exposed as constants (FLTK3::UP_BOX etc.); the classes can
 * also be instantiated to create new box objects.
 *********************************************************/
static mrb_value
mrb_fltk3_Box_init(mrb_state* mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "can't instantiate fltk3::Box directly");
  return self;
}

#define DECLARE_BOX(x)                                                    \
static mrb_value                                                          \
mrb_fltk3_ ## x ## _init(mrb_state *mrb, mrb_value self)                  \
{                                                                         \
  mrb_value arg = mrb_nil_value();                                        \
  mrb_get_args(mrb, "|S", &arg);                                          \
  fltk3::Box* box = new fltk3::x (                                        \
    mrb_nil_p(arg) ? NULL : RSTRING_CSTR(mrb, arg));                      \
  return mrb_fltk3_Box_setup(mrb, self, box);                             \
}

DECLARE_BOX(NoBox)
DECLARE_BOX(FlatBox)
DECLARE_BOX(UpBox)
DECLARE_BOX(DownBox)
DECLARE_BOX(ThinUpBox)
DECLARE_BOX(ThinDownBox)
DECLARE_BOX(EngravedBox)
DECLARE_BOX(EmbossedBox)
DECLARE_BOX(BorderBox)
DECLARE_BOX(ShadowBox)
DECLARE_BOX(RoundedBox)
DECLARE_BOX(RShadowBox)
DECLARE_BOX(RFlatBox)
DECLARE_BOX(RoundUpBox)
DECLARE_BOX(RoundDownBox)
DECLARE_BOX(DiamondUpBox)
DECLARE_BOX(DiamondDownBox)
DECLARE_BOX(OvalBox)
DECLARE_BOX(OShadowBox)
DECLARE_BOX(OFlatBox)
DECLARE_BOX(PlasticUpBox)
DECLARE_BOX(PlasticDownBox)
DECLARE_BOX(PlasticThinUpBox)
DECLARE_BOX(PlasticThinDownBox)
DECLARE_BOX(PlasticRoundUpBox)
DECLARE_BOX(PlasticRoundDownBox)
DECLARE_BOX(ClassicUpBox)
DECLARE_BOX(ClassicDownBox)
DECLARE_BOX(ClassicThinUpBox)
DECLARE_BOX(ClassicThinDownBox)
DECLARE_BOX(ClassicRoundUpBox)
DECLARE_BOX(ClassicRoundDownBox)
DECLARE_BOX(BorderFrame)
DECLARE_BOX(UpFrame)
DECLARE_BOX(DownFrame)
DECLARE_BOX(ThinUpFrame)
DECLARE_BOX(ThinDownFrame)
DECLARE_BOX(EngravedFrame)
DECLARE_BOX(EmbossedFrame)
DECLARE_BOX(ShadowFrame)
DECLARE_BOX(RoundedFrame)
DECLARE_BOX(OvalFrame)
DECLARE_BOX(PlasticUpFrame)
DECLARE_BOX(PlasticDownFrame)
DECLARE_BOX(ClassicUpFrame)
DECLARE_BOX(ClassicDownFrame)
DECLARE_BOX(ClassicThinUpFrame)
DECLARE_BOX(ClassicThinDownFrame)

#define DEFINE_BOX_GETTER(z) \
  mrb_define_method(mrb, _class_fltk3_Box, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Box); \
    return mrb_fltk3_Box_wrap(mrb, context->v->z(), "Box"); \
  }, MRB_ARGS_NONE());

#define DEFINE_BOX_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fltk3_Box_wrap(mrb, fltk3::x, "Box")); \
  ARENA_RESTORE;

void
mrb_fltk3_box_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Box = mrb_define_class_under(mrb, _class_fltk3, "Box", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_Box, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_Box, "initialize", mrb_fltk3_Box_init, MRB_ARGS_ANY());
  DEFINE_STR_GETTER(Box, Box, name);
  DEFINE_INT_GETTER(Box, Box, dx);
  DEFINE_INT_GETTER(Box, Box, dy);
  DEFINE_INT_GETTER(Box, Box, dw);
  DEFINE_INT_GETTER(Box, Box, dh);
  DEFINE_BOOL_GETTER(Box, Box, fills_rectangle);
  DEFINE_BOX_GETTER(upbox);
  DEFINE_BOX_GETTER(downbox);
  DEFINE_BOX_GETTER(upframe);
  DEFINE_BOX_GETTER(downframe);
  DEFINE_BOX_GETTER(box);
  ARENA_RESTORE;

  DEFINE_CLASS(NoBox, Box);
  DEFINE_CLASS(FlatBox, Box);
  DEFINE_CLASS(UpBox, Box);
  DEFINE_CLASS(DownBox, Box);
  DEFINE_CLASS(ThinUpBox, Box);
  DEFINE_CLASS(ThinDownBox, Box);
  DEFINE_CLASS(EngravedBox, Box);
  DEFINE_CLASS(EmbossedBox, Box);
  DEFINE_CLASS(BorderBox, Box);
  DEFINE_CLASS(ShadowBox, Box);
  DEFINE_CLASS(RoundedBox, Box);
  DEFINE_CLASS(RShadowBox, Box);
  DEFINE_CLASS(RFlatBox, Box);
  DEFINE_CLASS(RoundUpBox, Box);
  DEFINE_CLASS(RoundDownBox, Box);
  DEFINE_CLASS(DiamondUpBox, Box);
  DEFINE_CLASS(DiamondDownBox, Box);
  DEFINE_CLASS(OvalBox, Box);
  DEFINE_CLASS(OShadowBox, Box);
  DEFINE_CLASS(OFlatBox, Box);
  DEFINE_CLASS(PlasticUpBox, Box);
  DEFINE_CLASS(PlasticDownBox, Box);
  DEFINE_CLASS(PlasticThinUpBox, Box);
  DEFINE_CLASS(PlasticThinDownBox, Box);
  DEFINE_CLASS(PlasticRoundUpBox, Box);
  DEFINE_CLASS(PlasticRoundDownBox, Box);
  DEFINE_CLASS(ClassicUpBox, Box);
  DEFINE_CLASS(ClassicDownBox, Box);
  DEFINE_CLASS(ClassicThinUpBox, Box);
  DEFINE_CLASS(ClassicThinDownBox, Box);
  DEFINE_CLASS(ClassicRoundUpBox, Box);
  DEFINE_CLASS(ClassicRoundDownBox, Box);
  DEFINE_CLASS(BorderFrame, Box);
  DEFINE_CLASS(UpFrame, Box);
  DEFINE_CLASS(DownFrame, Box);
  DEFINE_CLASS(ThinUpFrame, Box);
  DEFINE_CLASS(ThinDownFrame, Box);
  DEFINE_CLASS(EngravedFrame, Box);
  DEFINE_CLASS(EmbossedFrame, Box);
  DEFINE_CLASS(ShadowFrame, Box);
  DEFINE_CLASS(RoundedFrame, Box);
  DEFINE_CLASS(OvalFrame, Box);
  DEFINE_CLASS(PlasticUpFrame, Box);
  DEFINE_CLASS(PlasticDownFrame, Box);
  DEFINE_CLASS(ClassicUpFrame, Box);
  DEFINE_CLASS(ClassicDownFrame, Box);
  DEFINE_CLASS(ClassicThinUpFrame, Box);
  DEFINE_CLASS(ClassicThinDownFrame, Box);

  DEFINE_BOX_CONST(NO_BOX);
  DEFINE_BOX_CONST(FLAT_BOX);
  DEFINE_BOX_CONST(UP_BOX);
  DEFINE_BOX_CONST(DOWN_BOX);
  DEFINE_BOX_CONST(THIN_UP_BOX);
  DEFINE_BOX_CONST(THIN_DOWN_BOX);
  DEFINE_BOX_CONST(ROUND_UP_BOX);
  DEFINE_BOX_CONST(ROUND_DOWN_BOX);
  DEFINE_BOX_CONST(ENGRAVED_BOX);
  DEFINE_BOX_CONST(EMBOSSED_BOX);
  DEFINE_BOX_CONST(BORDER_BOX);
  DEFINE_BOX_CONST(SHADOW_BOX);
  DEFINE_BOX_CONST(ROUNDED_BOX);
  DEFINE_BOX_CONST(RSHADOW_BOX);
  DEFINE_BOX_CONST(RFLAT_BOX);
  DEFINE_BOX_CONST(DIAMOND_UP_BOX);
  DEFINE_BOX_CONST(DIAMOND_DOWN_BOX);
  DEFINE_BOX_CONST(OVAL_BOX);
  DEFINE_BOX_CONST(OSHADOW_BOX);
  DEFINE_BOX_CONST(OFLAT_BOX);
  DEFINE_BOX_CONST(UP_FRAME);
  DEFINE_BOX_CONST(DOWN_FRAME);
  DEFINE_BOX_CONST(THIN_UP_FRAME);
  DEFINE_BOX_CONST(THIN_DOWN_FRAME);
  DEFINE_BOX_CONST(ENGRAVED_FRAME);
  DEFINE_BOX_CONST(EMBOSSED_FRAME);
  DEFINE_BOX_CONST(BORDER_FRAME);
  DEFINE_BOX_CONST(SHADOW_FRAME);
  DEFINE_BOX_CONST(ROUNDED_FRAME);
  DEFINE_BOX_CONST(OVAL_FRAME);
  DEFINE_BOX_CONST(PLASTIC_UP_BOX);
  DEFINE_BOX_CONST(PLASTIC_DOWN_BOX);
  DEFINE_BOX_CONST(PLASTIC_THIN_UP_BOX);
  DEFINE_BOX_CONST(PLASTIC_THIN_DOWN_BOX);
  DEFINE_BOX_CONST(PLASTIC_ROUND_UP_BOX);
  DEFINE_BOX_CONST(PLASTIC_ROUND_DOWN_BOX);
  DEFINE_BOX_CONST(PLASTIC_UP_FRAME);
  DEFINE_BOX_CONST(PLASTIC_DOWN_FRAME);
  DEFINE_BOX_CONST(CLASSIC_UP_BOX);
  DEFINE_BOX_CONST(CLASSIC_DOWN_BOX);
  DEFINE_BOX_CONST(CLASSIC_THIN_UP_BOX);
  DEFINE_BOX_CONST(CLASSIC_THIN_DOWN_BOX);
  DEFINE_BOX_CONST(CLASSIC_ROUND_UP_BOX);
  DEFINE_BOX_CONST(CLASSIC_ROUND_DOWN_BOX);
  DEFINE_BOX_CONST(CLASSIC_UP_FRAME);
  DEFINE_BOX_CONST(CLASSIC_DOWN_FRAME);
  DEFINE_BOX_CONST(CLASSIC_THIN_UP_FRAME);
  DEFINE_BOX_CONST(CLASSIC_THIN_DOWN_FRAME);
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
