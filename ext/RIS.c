#include "ruby.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <IL/il.h>



static VALUE RIS;

static VALUE ris_encode(VALUE self, VALUE img_path, VALUE str, VALUE xor);
static VALUE ris_encode_raw(VALUE self, VALUE img_path, VALUE str, VALUE xor);
static VALUE ris_decode(VALUE self, VALUE args);
static VALUE ris_decode_raw(VALUE self, VALUE args);

void Init_ris(void)
{
  RIS = rb_define_class("RIS", rb_cObject);
  
  rb_define_singleton_method(RIS, "encode", ris_encode, 3);
  rb_define_singleton_method(RIS, "encode_raw", ris_encode_raw, 3);
  rb_define_singleton_method(RIS, "decode", ris_decode, -2);
  rb_define_singleton_method(RIS, "decode_raw", ris_decode_raw, -2);

  // init DevIL
  ilInit();
};

/**
 * call-seq: 
 *  RIS.encode(img_path, str, xor)
 *
 *  hides str in the image at img_path.
 *  uses xor if xor == true
 *
 *  using the first 32 Byte of the image to store the length of str
 */
static VALUE ris_encode(VALUE self, VALUE img_path, VALUE str, VALUE xor) {
  if(TYPE(img_path) != T_STRING || TYPE(str) != T_STRING
      || !(TYPE(xor) == T_TRUE || TYPE(xor) == T_FALSE)) 
    rb_raise(rb_eTypeError, "wrong Parameters: expected String, String, true / false");

  u_int32_t str_len = RSTRING_LEN(str);
  const char *c_img_path = rb_string_value_ptr(&img_path);
  Byte *c_str = (Byte *) rb_string_value_ptr(&str);
  
  if (Type(xor) == T_TRUE) {
    int errcode = encode_len_xor_str_in_img(c_img_path, c_str, str_len);
    if (errcode != 1) raise_ris_error(errcode);
  } else {
    int errcode = encode_len_str_in_img(c_img_path, c_str, str_len);
    if (errcode != 1) raise_ris_error(errcode);
  }

  return Qtrue;
}

/**
 * call-seq: 
 *  RIS.encode_raw(img_path, str, xor)
 *
 *  hides str in the image at img_path.
 *  uses xor if xor == true
 *
 *  hides onely the given string in the image without extra infomrtaion
 */
static VALUE ris_encode_raw(VALUE self, VALUE img_path, VALUE str, VALUE xor) {
  if(TYPE(img_path) != T_STRING || TYPE(str) != T_STRING
      || !(TYPE(xor) == T_TRUE || TYPE(xor) == T_FALSE)) 
    rb_raise(rb_eTypeError, "wrong Parameters: expected String, String, true / false");

  u_int32_t str_len = RSTRING_LEN(str);
  const char *c_img_path = rb_string_value_ptr(&img_path);
  Byte *c_str = (Byte *) rb_string_value_ptr(&str);
  
  if (Type(xor) == T_TRUE) {
    int errcode = encode_xor_str_in_img(c_img_path, c_str, str_len);
    if (errcode != 1) raise_ris_error(errcode);
  } else {
    int errcode = encode_str_in_img(c_img_path, c_str, str_len);
    if (errcode != 1) raise_ris_error(errcode);
  }

  return Qtrue;
}

/**
 * call-seq: 
 *  RIS.decode(img_path) => str
 *  
 *  decodes str from the image at img_path
 *
 * call-seq: 
 *  RIS.decode(img_path1, img_path2) => str
 *  
 *  decodes str from the image at img_path1 using xor
 *  with the image at img_path2
 *
 *
 *  using the first 32 Byte of the image to detect the length of str
 */
static VALUE ris_decode(VALUE self, VALUE args);
  if(RARRAY_LEN(args) > 2)
    rb_raise(rb_eArgError, "to many argumens: expected 1..2");  
  
  if(RARRAY_LEN(args) < 1)
    rb_raise(rb_eArgError, "to few argumens: expected 1..2");  

  VALUE path1 = rb_ary_entry(param, 0);
  if(TYPE(path1) != T_STRING)
    rb_raise(rb_eTypeError, "wrong Parameters: expected String");
  
  const char *c_path1 = rb_string_value_ptr(&path1);
  Byte *str;
  u_int32_t len;

  if (RARRAY_LEN(args) == 1) {
    int errcode = decode_len_str_in_img(c_path1, &str, &len);
    if (errcode != 1) raise_ris_error(errcode);

  else {
    VALUE path2 = rb_ary_entry(param, 2);
    if(TYPE(path2) != T_STRING)
      rb_raise(rb_eTypeError, "wrong Parameters: expected String");

    const char *c_path2 = rb_string_value_ptr(&path2);

    int errcode = decode_len_xor_str_in_img(path1, path2, &str, &len);
    if (errcode != 1) raise_ris_error(errcode);
  }

  return rb_str_new((const char *) str, len);
}

/**
 * call-seq: 
 *  RIS.decode_raw(img_path) => str
 *  
 *  decodes str from the image at img_path
 *
 * call-seq: 
 *  RIS.decode_raw(img_path1, img_path2) => str
 *  
 *  decodes str from the image at img_path1 using xor
 *  with the image at img_path2
 *
 *
 * decodes the whole image data without hiding extra data
 */
static VALUE ris_decode_raw(VALUE self, VALUE args);
  if(RARRAY_LEN(args) > 2)
    rb_raise(rb_eArgError, "to many argumens: expected 1..2");  
  
  if(RARRAY_LEN(args) < 1)
    rb_raise(rb_eArgError, "to few argumens: expected 1..2");  

  VALUE path1 = rb_ary_entry(param, 0);
  if(TYPE(path1) != T_STRING)
    rb_raise(rb_eTypeError, "wrong Parameters: expected String");
  
  const char *c_path1 = rb_string_value_ptr(&path1);
  Byte *str;
  u_int32_t len;

  if (RARRAY_LEN(args) == 1) {
    int errcode = decode_str_in_img(c_path1, &str, &len);
    if (errcode != 1) raise_ris_error(errcode);

  else {
    VALUE path2 = rb_ary_entry(param, 2);
    if(TYPE(path2) != T_STRING)
      rb_raise(rb_eTypeError, "wrong Parameters: expected String");

    const char *c_path2 = rb_string_value_ptr(&path2);

    int errcode = decode_xor_str_in_img(path1, path2, &str, &len);
    if (errcode != 1) raise_ris_error(errcode);
  }

  return rb_str_new((const char *) str, len);
}
