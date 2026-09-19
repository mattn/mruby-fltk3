#include "mrb_fltk3.h"
#include <fltk3/Tree.h>
#include <fltk3/TreeItem.h>

/*********************************************************
 * FLTK3::Tree / FLTK3::TreeItem
 *
 * Tree items are owned by the tree, so wrappers are created on each
 * access and compare by pointer; they are only valid while the item
 * exists in the tree.
 *********************************************************/
DECLARE_WIDGET(Tree)

static mrb_value
mrb_fltk3_treeitem_new(mrb_state* mrb, const fltk3::TreeItem* item)
{
  if (!item) return mrb_nil_value();
  mrb_fltk3_TreeItem_context* context =
    (mrb_fltk3_TreeItem_context*) malloc(sizeof(mrb_fltk3_TreeItem_context));
  if (!context) mrb_raise(mrb, E_RUNTIME_ERROR, "can't alloc memory");
  memset(context, 0, sizeof(mrb_fltk3_TreeItem_context));
  mrb_value instance = mrb_obj_value(Data_Wrap_Struct(mrb, mrb_fltk3_class(mrb, "TreeItem"), &fltk3_TreeItem_type, context));
  context->instance = instance;
  context->mrb = mrb;
  context->v = (fltk3::TreeItem*) item;
  return instance;
}

static mrb_value
mrb_fltk3_TreeItem_init(mrb_state* mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "tree items are created through FLTK3::Tree#add");
  return self;
}

static mrb_value
mrb_fltk3_treeitem_equal(mrb_state* mrb, mrb_value self)
{
  mrb_value o;
  mrb_get_args(mrb, "o", &o);
  if (!mrb_data_p(o) || DATA_TYPE(o) != &fltk3_TreeItem_type) return mrb_false_value();
  mrb_fltk3_TreeItem_context* a = (mrb_fltk3_TreeItem_context*) DATA_PTR(self);
  mrb_fltk3_TreeItem_context* b = (mrb_fltk3_TreeItem_context*) DATA_PTR(o);
  return mrb_bool_value(a && b && a->v == b->v);
}

static mrb_value
mrb_fltk3_treeitem_label_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  context->v->label(RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_treeitem_child(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_int i;
  mrb_get_args(mrb, "i", &i);
  if (i < 0) i += context->v->children();
  if (i < 0 || i >= context->v->children()) return mrb_nil_value();
  return mrb_fltk3_treeitem_new(mrb, context->v->child((int) i));
}

static mrb_value
mrb_fltk3_treeitem_children_ary(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_value ary = mrb_ary_new_capa(mrb, context->v->children());
  for (int i = 0; i < context->v->children(); i++) {
    mrb_ary_push(mrb, ary, mrb_fltk3_treeitem_new(mrb, context->v->child(i)));
  }
  return ary;
}

static mrb_value
mrb_fltk3_treeitem_find_child(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  if (mrb_string_p(v)) return mrb_fixnum_value(context->v->find_child(RSTRING_CSTR(mrb, v)));
  return mrb_fixnum_value(context->v->find_child(mrb_fltk3_TreeItem_ptr(mrb, v)));
}

static mrb_value
mrb_fltk3_treeitem_remove_child(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_value v;
  mrb_get_args(mrb, "o", &v);
  if (mrb_string_p(v)) return mrb_fixnum_value(context->v->remove_child(RSTRING_CSTR(mrb, v)));
  return mrb_fixnum_value(context->v->remove_child(mrb_fltk3_TreeItem_ptr(mrb, v)));
}

static mrb_value
mrb_fltk3_treeitem_swap_children(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_int a, b;
  mrb_get_args(mrb, "ii", &a, &b);
  context->v->swap_children((int) a, (int) b);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_treeitem_widget_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  fltk3::Widget* w = context->v->widget();
  return mrb_fltk3_Widget_wrap(mrb, w, mrb_fltk3_widget_classname(w));
}

static mrb_value
mrb_fltk3_treeitem_widget_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_value w;
  mrb_get_args(mrb, "o", &w);
  context->v->widget(mrb_fltk3_Widget_ptr(mrb, w));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_treeitem_usericon_get(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  fltk3::Image* image = context->v->usericon();
  return mrb_fltk3_Image_wrap(mrb, image, mrb_fltk3_image_classname(image));
}

static mrb_value
mrb_fltk3_treeitem_usericon_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_value image;
  mrb_get_args(mrb, "o", &image);
  context->v->usericon(mrb_fltk3_Image_ptr(mrb, image));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_treeitem_select(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_bool val = TRUE;
  mrb_get_args(mrb, "|b", &val);
  context->v->select(val);
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_treeitem_activate(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(TreeItem);
  mrb_bool val = TRUE;
  mrb_get_args(mrb, "|b", &val);
  context->v->activate(val);
  return mrb_nil_value();
}

#define DEFINE_ITEM_NAV(name) \
static mrb_value \
mrb_fltk3_treeitem_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(TreeItem); \
  return mrb_fltk3_treeitem_new(mrb, context->v->name()); \
}

DEFINE_ITEM_NAV(parent)
DEFINE_ITEM_NAV(next)
DEFINE_ITEM_NAV(prev)
DEFINE_ITEM_NAV(next_sibling)
DEFINE_ITEM_NAV(prev_sibling)

/*********************************************************
 * FLTK3::Tree
 *********************************************************/
static fltk3::TreeItem*
mrb_fltk3_tree_item_arg(mrb_state* mrb, fltk3::Tree* tree, mrb_value v)
{
  if (mrb_string_p(v)) {
    fltk3::TreeItem* item = tree->find_item(RSTRING_CSTR(mrb, v));
    if (!item) mrb_raisef(mrb, E_ARGUMENT_ERROR, "no such item: %v", v);
    return item;
  }
  return mrb_fltk3_TreeItem_ptr(mrb, v);
}

/* add(path) or add(item, name) */
static mrb_value
mrb_fltk3_tree_add(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value a, b = mrb_nil_value();
  mrb_get_args(mrb, "o|S", &a, &b);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  if (mrb_nil_p(b)) return mrb_fltk3_treeitem_new(mrb, tree->add(RSTRING_CSTR(mrb, mrb_ensure_string_type(mrb, a))));
  return mrb_fltk3_treeitem_new(mrb, tree->add(mrb_fltk3_TreeItem_ptr(mrb, a), RSTRING_CSTR(mrb, b)));
}

static mrb_value
mrb_fltk3_tree_insert(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item, name;
  mrb_int pos;
  mrb_get_args(mrb, "oSi", &item, &name, &pos);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  return mrb_fltk3_treeitem_new(mrb, tree->insert(mrb_fltk3_tree_item_arg(mrb, tree, item), RSTRING_CSTR(mrb, name), (int) pos));
}

static mrb_value
mrb_fltk3_tree_insert_above(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item, name;
  mrb_get_args(mrb, "oS", &item, &name);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  return mrb_fltk3_treeitem_new(mrb, tree->insert_above(mrb_fltk3_tree_item_arg(mrb, tree, item), RSTRING_CSTR(mrb, name)));
}

static mrb_value
mrb_fltk3_tree_remove(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item;
  mrb_get_args(mrb, "o", &item);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  return mrb_fixnum_value(tree->remove(mrb_fltk3_tree_item_arg(mrb, tree, item)));
}

static mrb_value
mrb_fltk3_tree_clear_children(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item;
  mrb_get_args(mrb, "o", &item);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  tree->clear_children(mrb_fltk3_tree_item_arg(mrb, tree, item));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_tree_find_item(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value path;
  mrb_get_args(mrb, "S", &path);
  return mrb_fltk3_treeitem_new(mrb, ((fltk3::Tree*) context->v)->find_item(RSTRING_CSTR(mrb, path)));
}

static mrb_value
mrb_fltk3_tree_item_pathname(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item;
  mrb_get_args(mrb, "o", &item);
  char buf[1024];
  if (((fltk3::Tree*) context->v)->item_pathname(buf, sizeof(buf), mrb_fltk3_TreeItem_ptr(mrb, item)) != 0) return mrb_nil_value();
  return mrb_str_new_cstr(mrb, buf);
}

#define DEFINE_TREE_ITEM_GETTER(name) \
static mrb_value \
mrb_fltk3_tree_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  return mrb_fltk3_treeitem_new(mrb, ((fltk3::Tree*) context->v)->name()); \
}

DEFINE_TREE_ITEM_GETTER(root)
DEFINE_TREE_ITEM_GETTER(first)
DEFINE_TREE_ITEM_GETTER(last)
DEFINE_TREE_ITEM_GETTER(item_clicked)
DEFINE_TREE_ITEM_GETTER(first_selected_item)

#define DEFINE_TREE_ITEM_NAV(name) \
static mrb_value \
mrb_fltk3_tree_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  mrb_value item = mrb_nil_value(); \
  mrb_get_args(mrb, "|o", &item); \
  fltk3::Tree* tree = (fltk3::Tree*) context->v; \
  return mrb_fltk3_treeitem_new(mrb, tree->name(mrb_nil_p(item) ? NULL : mrb_fltk3_tree_item_arg(mrb, tree, item))); \
}

DEFINE_TREE_ITEM_NAV(next)
DEFINE_TREE_ITEM_NAV(prev)
DEFINE_TREE_ITEM_NAV(next_selected_item)

/* open/close/select/deselect(item_or_path, docallback = true) -> true if changed */
#define DEFINE_TREE_ITEM_ACTION(name) \
static mrb_value \
mrb_fltk3_tree_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  mrb_value item; \
  mrb_bool docallback = TRUE; \
  mrb_get_args(mrb, "o|b", &item, &docallback); \
  fltk3::Tree* tree = (fltk3::Tree*) context->v; \
  return mrb_bool_value(tree->name(mrb_fltk3_tree_item_arg(mrb, tree, item), docallback) != 0); \
}

DEFINE_TREE_ITEM_ACTION(open)
DEFINE_TREE_ITEM_ACTION(close)
DEFINE_TREE_ITEM_ACTION(select)
DEFINE_TREE_ITEM_ACTION(deselect)
DEFINE_TREE_ITEM_ACTION(select_only)

#define DEFINE_TREE_ITEM_TOGGLE(name) \
static mrb_value \
mrb_fltk3_tree_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  mrb_value item; \
  mrb_bool docallback = TRUE; \
  mrb_get_args(mrb, "o|b", &item, &docallback); \
  fltk3::Tree* tree = (fltk3::Tree*) context->v; \
  tree->name(mrb_fltk3_tree_item_arg(mrb, tree, item), docallback); \
  return mrb_nil_value(); \
}

DEFINE_TREE_ITEM_TOGGLE(open_toggle)
DEFINE_TREE_ITEM_TOGGLE(select_toggle)

#define DEFINE_TREE_ITEM_QUERY(name) \
static mrb_value \
mrb_fltk3_tree_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  mrb_value item; \
  mrb_get_args(mrb, "o", &item); \
  fltk3::Tree* tree = (fltk3::Tree*) context->v; \
  return mrb_bool_value(tree->name(mrb_fltk3_tree_item_arg(mrb, tree, item)) != 0); \
}

DEFINE_TREE_ITEM_QUERY(is_open)
DEFINE_TREE_ITEM_QUERY(is_close)
DEFINE_TREE_ITEM_QUERY(is_selected)

#define DEFINE_TREE_ALL_ACTION(name) \
static mrb_value \
mrb_fltk3_tree_ ## name(mrb_state* mrb, mrb_value self) \
{ \
  CONTEXT_SETUP(Widget); \
  mrb_value item = mrb_nil_value(); \
  mrb_bool docallback = TRUE; \
  mrb_get_args(mrb, "|ob", &item, &docallback); \
  fltk3::Tree* tree = (fltk3::Tree*) context->v; \
  return mrb_fixnum_value(tree->name(mrb_nil_p(item) ? NULL : mrb_fltk3_tree_item_arg(mrb, tree, item), docallback)); \
}

DEFINE_TREE_ALL_ACTION(select_all)
DEFINE_TREE_ALL_ACTION(deselect_all)

static mrb_value
mrb_fltk3_tree_selected_items(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  mrb_value ary = mrb_ary_new(mrb);
  for (fltk3::TreeItem* item = tree->first_selected_item(); item; item = tree->next_selected_item(item)) {
    mrb_ary_push(mrb, ary, mrb_fltk3_treeitem_new(mrb, item));
  }
  return ary;
}

static mrb_value
mrb_fltk3_tree_each(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value blk;
  mrb_get_args(mrb, "&!", &blk);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  for (fltk3::TreeItem* item = tree->first(); item; item = tree->next(item)) {
    mrb_value arg = mrb_fltk3_treeitem_new(mrb, item);
    mrb_yield_argv(mrb, blk, 1, &arg);
  }
  return self;
}

static mrb_value
mrb_fltk3_tree_set_item_focus(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value item;
  mrb_get_args(mrb, "o", &item);
  fltk3::Tree* tree = (fltk3::Tree*) context->v;
  tree->set_item_focus(mrb_fltk3_tree_item_arg(mrb, tree, item));
  return mrb_nil_value();
}

static mrb_value
mrb_fltk3_tree_root_label_set(mrb_state* mrb, mrb_value self)
{
  CONTEXT_SETUP(Widget);
  mrb_value s;
  mrb_get_args(mrb, "S", &s);
  ((fltk3::Tree*) context->v)->root_label(RSTRING_CSTR(mrb, s));
  return mrb_nil_value();
}

#define DEFINE_TREE_IMAGE_PROP(name) \
  mrb_define_method(mrb, _class_fltk3_Tree, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Widget); \
    fltk3::Image* image = ((fltk3::Tree*) context->v)->name(); \
    return mrb_fltk3_Image_wrap(mrb, image, mrb_fltk3_image_classname(image)); \
  }, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_Tree, # name "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Widget); \
    mrb_value image; \
    mrb_get_args(mrb, "o", &image); \
    ((fltk3::Tree*) context->v)->name(mrb_fltk3_Image_ptr(mrb, image)); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_TREE_ENUM_PROP(name, type) \
  mrb_define_method(mrb, _class_fltk3_Tree, # name, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Widget); \
    return mrb_fixnum_value((mrb_int) ((fltk3::Tree*) context->v)->name()); \
  }, MRB_ARGS_NONE()); \
  mrb_define_method(mrb, _class_fltk3_Tree, # name "=", [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(Widget); \
    mrb_int v; \
    mrb_get_args(mrb, "i", &v); \
    ((fltk3::Tree*) context->v)->name((type) v); \
    return mrb_nil_value(); \
  }, MRB_ARGS_REQ(1));

#define DEFINE_ITEM_BOOL(z) \
  mrb_define_method(mrb, _class_fltk3_TreeItem, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(TreeItem); \
    return mrb_bool_value(context->v->z() != 0); \
  }, MRB_ARGS_NONE());

#define DEFINE_ITEM_VOID(z) \
  mrb_define_method(mrb, _class_fltk3_TreeItem, # z, [] (mrb_state* mrb, mrb_value self) -> mrb_value { \
    CONTEXT_SETUP(TreeItem); \
    context->v->z(); \
    return mrb_nil_value(); \
  }, MRB_ARGS_NONE());

#define DEFINE_INT_CONST(x) \
  mrb_define_const(mrb, _class_fltk3, # x, mrb_fixnum_value((mrb_int) fltk3::x));

void
mrb_fltk3_tree_init(mrb_state* mrb, struct RClass* _class_fltk3)
{
  ARENA_SAVE;
  struct RClass* _class_fltk3_Group = mrb_fltk3_class(mrb, "Group");

  DEFINE_INT_CONST(TREE_REASON_NONE);
  DEFINE_INT_CONST(TREE_REASON_SELECTED);
  DEFINE_INT_CONST(TREE_REASON_DESELECTED);
  DEFINE_INT_CONST(TREE_REASON_OPENED);
  DEFINE_INT_CONST(TREE_REASON_CLOSED);
  DEFINE_INT_CONST(TREE_SORT_NONE);
  DEFINE_INT_CONST(TREE_SORT_ASCENDING);
  DEFINE_INT_CONST(TREE_SORT_DESCENDING);
  DEFINE_INT_CONST(TREE_CONNECTOR_NONE);
  DEFINE_INT_CONST(TREE_CONNECTOR_DOTTED);
  DEFINE_INT_CONST(TREE_CONNECTOR_SOLID);
  DEFINE_INT_CONST(TREE_SELECT_NONE);
  DEFINE_INT_CONST(TREE_SELECT_SINGLE);
  DEFINE_INT_CONST(TREE_SELECT_MULTI);

  struct RClass* _class_fltk3_TreeItem = mrb_define_class_under(mrb, _class_fltk3, "TreeItem", mrb->object_class);
  MRB_SET_INSTANCE_TT(_class_fltk3_TreeItem, MRB_TT_DATA);
  mrb_define_method(mrb, _class_fltk3_TreeItem, "initialize", mrb_fltk3_TreeItem_init, MRB_ARGS_ANY());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "==", mrb_fltk3_treeitem_equal, MRB_ARGS_REQ(1));
  DEFINE_STR_GETTER(TreeItem, TreeItem, label);
  mrb_define_method(mrb, _class_fltk3_TreeItem, "label=", mrb_fltk3_treeitem_label_set, MRB_ARGS_REQ(1));
  DEFINE_INT_GETTER(TreeItem, TreeItem, x);
  DEFINE_INT_GETTER(TreeItem, TreeItem, y);
  DEFINE_INT_GETTER(TreeItem, TreeItem, w);
  DEFINE_INT_GETTER(TreeItem, TreeItem, h);
  DEFINE_INT_GETTER(TreeItem, TreeItem, children);
  DEFINE_INT_GETTER(TreeItem, TreeItem, depth);
  DEFINE_INT_PROP(TreeItem, TreeItem, labelfont);
  DEFINE_INT_PROP(TreeItem, TreeItem, labelsize);
  DEFINE_INT_PROP(TreeItem, TreeItem, labelfgcolor);
  DEFINE_INT_PROP(TreeItem, TreeItem, labelbgcolor);
  DEFINE_INT_PROP(TreeItem, TreeItem, labelcolor);
  DEFINE_ITEM_BOOL(has_children);
  DEFINE_ITEM_BOOL(is_open);
  DEFINE_ITEM_BOOL(is_close);
  DEFINE_ITEM_BOOL(is_selected);
  DEFINE_ITEM_BOOL(is_active);
  DEFINE_ITEM_BOOL(is_activated);
  DEFINE_ITEM_BOOL(is_root);
  DEFINE_ITEM_BOOL(visible);
  DEFINE_ITEM_BOOL(visible_r);
  DEFINE_ITEM_VOID(open);
  DEFINE_ITEM_VOID(close);
  DEFINE_ITEM_VOID(open_toggle);
  DEFINE_ITEM_VOID(select_toggle);
  DEFINE_ITEM_VOID(deselect);
  DEFINE_ITEM_VOID(deactivate);
  DEFINE_ITEM_VOID(clear_children);
  mrb_define_method(mrb, _class_fltk3_TreeItem, "select_all", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(TreeItem);
    return mrb_fixnum_value(context->v->select_all());
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "deselect_all", [] (mrb_state* mrb, mrb_value self) -> mrb_value {
    CONTEXT_SETUP(TreeItem);
    return mrb_fixnum_value(context->v->deselect_all());
  }, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "select", mrb_fltk3_treeitem_select, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "activate", mrb_fltk3_treeitem_activate, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "child", mrb_fltk3_treeitem_child, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "[]", mrb_fltk3_treeitem_child, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "children_items", mrb_fltk3_treeitem_children_ary, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "find_child", mrb_fltk3_treeitem_find_child, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "remove_child", mrb_fltk3_treeitem_remove_child, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "swap_children", mrb_fltk3_treeitem_swap_children, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "widget", mrb_fltk3_treeitem_widget_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "widget=", mrb_fltk3_treeitem_widget_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "usericon", mrb_fltk3_treeitem_usericon_get, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "usericon=", mrb_fltk3_treeitem_usericon_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_TreeItem, "parent", mrb_fltk3_treeitem_parent, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "next", mrb_fltk3_treeitem_next, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "prev", mrb_fltk3_treeitem_prev, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "next_sibling", mrb_fltk3_treeitem_next_sibling, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_TreeItem, "prev_sibling", mrb_fltk3_treeitem_prev_sibling, MRB_ARGS_NONE());
  ARENA_RESTORE;

  DEFINE_CLASS(Tree, Group);
  mrb_define_method(mrb, _class_fltk3_Tree, "add", mrb_fltk3_tree_add, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "<<", mrb_fltk3_tree_add, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "insert", mrb_fltk3_tree_insert, MRB_ARGS_REQ(3));
  mrb_define_method(mrb, _class_fltk3_Tree, "insert_above", mrb_fltk3_tree_insert_above, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, _class_fltk3_Tree, "remove", mrb_fltk3_tree_remove, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "clear_children", mrb_fltk3_tree_clear_children, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "find_item", mrb_fltk3_tree_find_item, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "[]", mrb_fltk3_tree_find_item, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "item_pathname", mrb_fltk3_tree_item_pathname, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "root", mrb_fltk3_tree_root, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Tree, "root_label=", mrb_fltk3_tree_root_label_set, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "first", mrb_fltk3_tree_first, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Tree, "last", mrb_fltk3_tree_last, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Tree, "next", mrb_fltk3_tree_next, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "prev", mrb_fltk3_tree_prev, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "item_clicked", mrb_fltk3_tree_item_clicked, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Tree, "first_selected_item", mrb_fltk3_tree_first_selected_item, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Tree, "next_selected_item", mrb_fltk3_tree_next_selected_item, MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "selected_items", mrb_fltk3_tree_selected_items, MRB_ARGS_NONE());
  mrb_define_method(mrb, _class_fltk3_Tree, "each", mrb_fltk3_tree_each, MRB_ARGS_BLOCK());
  mrb_define_method(mrb, _class_fltk3_Tree, "open", mrb_fltk3_tree_open, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "close", mrb_fltk3_tree_close, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "open_toggle", mrb_fltk3_tree_open_toggle, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "select", mrb_fltk3_tree_select, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "select_toggle", mrb_fltk3_tree_select_toggle, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "deselect", mrb_fltk3_tree_deselect, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "select_only", mrb_fltk3_tree_select_only, MRB_ARGS_REQ(1) | MRB_ARGS_OPT(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "select_all", mrb_fltk3_tree_select_all, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Tree, "deselect_all", mrb_fltk3_tree_deselect_all, MRB_ARGS_OPT(2));
  mrb_define_method(mrb, _class_fltk3_Tree, "is_open", mrb_fltk3_tree_is_open, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "is_close", mrb_fltk3_tree_is_close, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "is_selected", mrb_fltk3_tree_is_selected, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, _class_fltk3_Tree, "set_item_focus", mrb_fltk3_tree_set_item_focus, MRB_ARGS_REQ(1));
  DEFINE_VOID_METHOD(Tree, Widget, clear);
  DEFINE_VOID_METHOD(Tree, Widget, show_self);
  DEFINE_INT_PROP(Tree, Widget, item_labelfont);
  DEFINE_INT_PROP(Tree, Widget, item_labelsize);
  DEFINE_INT_PROP(Tree, Widget, item_labelfgcolor);
  DEFINE_INT_PROP(Tree, Widget, item_labelbgcolor);
  DEFINE_INT_PROP(Tree, Widget, connectorcolor);
  DEFINE_INT_PROP(Tree, Widget, connectorwidth);
  DEFINE_INT_PROP(Tree, Widget, marginleft);
  DEFINE_INT_PROP(Tree, Widget, margintop);
  DEFINE_INT_PROP(Tree, Widget, openchild_marginbottom);
  DEFINE_INT_PROP(Tree, Widget, showcollapse);
  DEFINE_INT_PROP(Tree, Widget, showroot);
  DEFINE_INT_PROP(Tree, Widget, vposition);
  DEFINE_INT_PROP(Tree, Widget, scrollbar_size);
  DEFINE_TREE_ENUM_PROP(connectorstyle, fltk3::TreeConnector);
  DEFINE_TREE_ENUM_PROP(sortorder, fltk3::TreeSort);
  DEFINE_TREE_ENUM_PROP(selectmode, fltk3::TreeSelect);
  DEFINE_TREE_ENUM_PROP(callback_reason, fltk3::TreeReason);
  DEFINE_TREE_IMAGE_PROP(usericon);
  DEFINE_TREE_IMAGE_PROP(openicon);
  DEFINE_TREE_IMAGE_PROP(closeicon);
  ARENA_RESTORE;
}

/* vim:set et ts=2 sts=2 sw=2 tw=0: */
