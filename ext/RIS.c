#include "ruby.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <IL/il.h>



static VALUE RIS;

static VALUE initialize_ris(VALUE self, VALUE args);

void Init_ris(void)
{
  OsiMaster    = rb_define_class("RIS", rb_cObject);
  
  rb_define_method(Socket, "initialize", initialize_ris, -2);

  // init DevIL
  ilInit();
};

/**
 * call-seq: 
 *  RIS.new(img_path, str, xor)
 *
 *  hides str in the image at img_path.
 *  uses xor if xor == true
 *
 * call-seq: 
 *  RIS.new(img_path) => str
 *  
 *  decodes str from the image at img_path
 *
 * call-seq: 
 *  RIS.new(img_path1, img_path2) => str
 *  
 *  decodes str from the image at img_path1 using xor
 *  with the image at img_path2
 */
VALUE initialize_socket(VALUE self, VALUE args)
{
  // geting String if passed

  if(RARRAY_LEN(args) > 3)
    rb_raise(rb_eArgError, "to many argumens: expected 1..3");  
  
  if(RARRAY_LEN(args) < 1)
    rb_raise(rb_eArgError, "to few argumens: expected 1..3");  

  switch (RARRAY_LEN(args)) {
    case 1:
      VALUE var = rb_ary_entry(param, 0);
      if(TYPE(var) != T_STRING)
        rb_raise(rb_eTypeError, "wrong Parameter: expected String");
      break;

    case 2:
      VALUE var1 = rb_ary_entry(param, 0);
      VALUE var2 = rb_ary_entry(param, 1);
      if(TYPE(var1) != T_STRING || TYPE(var2) != T_STRING)
        rb_raise(rb_eTypeError, "wrong Parameters: expected String, String");

      break;

    case 3:
      VALUE var1 = rb_ary_entry(param, 0);
      VALUE var2 = rb_ary_entry(param, 1);
      VALUE var3 = rb_ary_entry(param, 2);
      if(TYPE(var1) != T_STRING || TYPE(var2) != T_STRING 
      || !(TYPE(var3) == T_TRUE || TYPE(var3) == T_FALSE)) 
        rb_raise(rb_eTypeError, "wrong Parameters: expected String, String, true / false");
      
      break;

    default:
      rb_raise(rb_eRuntimeError, "Fatal Error");

  }

  return self;
}
