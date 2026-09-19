#include "mrb_fltk3.h"
#include <fltk3/Box.h>

/*********************************************************
 * FLTK3::Box
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
  return mrb_fltk3_Widget_setup(mrb, self, (fltk3::Widget*) box);         \
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

void
mrb_fltk3_box_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Widget = mrb_fltk3_class(mrb, "Widget");
  DEFINE_CLASS(Box, Widget);
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
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
