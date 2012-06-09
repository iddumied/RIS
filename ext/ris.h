#define ERROR -1
#define RIS_STR_FIT_ERROR -2
#define RIS_IMG_LEN_ERROR -3
#define RIS_IMG_LOAD_ERROR -4
#define RIS_IMG_SAVE_ERROR -5

typedef struct {
  u_int8_t bit0:1;
  u_int8_t bit1:1;
  u_int8_t bit2:1;
  u_int8_t bit3:1;
  u_int8_t bit4:1;
  u_int8_t bit5:1;
  u_int8_t bit6:1;
  u_int8_t bit7:1;
} Byte;

int encode_str_in_str(Byte *target, u_int64_t target_len, Byte *source, u_int64_t source_len) {
  if ((target_len / 8) < source_len) 
    return RIS_STR_FIT_ERROR;
  
  u_int64_t i;
  for (i = 0; i < source_len; i++, target += 8, source++) {
    target[0].bit0 = source->bit7; 
    target[1].bit0 = source->bit6; 
    target[2].bit0 = source->bit5; 
    target[3].bit0 = source->bit4; 
    target[4].bit0 = source->bit3; 
    target[5].bit0 = source->bit2; 
    target[6].bit0 = source->bit1; 
    target[7].bit0 = source->bit0; 
  }

  return 1;
}

int encode_xor_str_in_str(Byte *target, u_int64_t target_len, Byte *source, u_int64_t source_len) {
  if ((target_len / 8) < source_len) 
    return RIS_STR_FIT_ERROR;
  
  u_int64_t i;
  for (i = 0; i < source_len; i++, target += 8, source++) {
    target[0].bit0 ^= source->bit7; 
    target[1].bit0 ^= source->bit6; 
    target[2].bit0 ^= source->bit5; 
    target[3].bit0 ^= source->bit4; 
    target[4].bit0 ^= source->bit3; 
    target[5].bit0 ^= source->bit2; 
    target[6].bit0 ^= source->bit1; 
    target[7].bit0 ^= source->bit0; 
  }

  return 1;
}

int decode_str_in_str(Byte *source, u_int64_t source_len, Byte *target, u_int64_t target_len) {
  if ((source_len / 8) < target_len) 
    return RIS_STR_FIT_ERROR;
  
  u_int64_t i;
  for (i = 0; i < target_len; i++, source += 8, target++) {
    target->bit7 = source[0].bit0;  
    target->bit6 = source[1].bit0;  
    target->bit5 = source[2].bit0;  
    target->bit4 = source[3].bit0;  
    target->bit3 = source[4].bit0;  
    target->bit2 = source[5].bit0;  
    target->bit1 = source[6].bit0;  
    target->bit0 = source[7].bit0;  
  }

  return 1;
}

int decode_xor_str_in_str(Byte *src_org, u_int64_t src_org_len, Byte *src_xor, 
                            u_int64_t src_xor_len, Byte *target, u_int64_t target_len) {

  if (src_org_len != src_xor_len)
    return RIS_IMG_LEN_ERROR;
  if ((src_xor_len / 8) < target_len) 
    return RIS_STR_FIT_ERROR;
  
  u_int64_t i;
  for (i = 0; i < target_len; i++, src_org += 8, src_xor += 8, target++) {
    target->bit7 = src_org[0].bit0 ^ src_xor[0].bit0;  
    target->bit6 = src_org[1].bit0 ^ src_xor[1].bit0;  
    target->bit5 = src_org[2].bit0 ^ src_xor[2].bit0;  
    target->bit4 = src_org[3].bit0 ^ src_xor[3].bit0;  
    target->bit3 = src_org[4].bit0 ^ src_xor[4].bit0;  
    target->bit2 = src_org[5].bit0 ^ src_xor[5].bit0;  
    target->bit1 = src_org[6].bit0 ^ src_xor[6].bit0;  
    target->bit0 = src_org[7].bit0 ^ src_xor[7].bit0;  
  }

  return 1;
}

u_int64_t img_length() {
  u_int64_t pixel_length = ilGetInteger(IL_IMAGE_WIDTH);
  pixel_length *= ilGetInteger(IL_IMAGE_HEIGHT) * ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
  return pixel_length;
}

/**
 * Encodes str in the image at path 
 */
int encode_str_in_img(const char *path, Byte *str, u_int64_t str_len) {
  if (ilLoadImage(path) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  int errcode = encode_str_in_str((Byte *) ilGetData(), img_length(), str, str_len);
  if (errcode != 1) return errcode;

  char img_save_path[256];
  strcpy ((char *) img_save_path, path);
  strcat ((char *) img_save_path, ".enc.png");

  if (ilSaveImage(img_save_path) == IL_FALSE)
    return RIS_IMG_SAVE_ERROR;

  return 1;
}

/**
 * Encodes str in the image at path using xor
 */
int encode_xor_str_in_img(const char *path, Byte *str, u_int64_t str_len) {
  if (ilLoadImage(path) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  int errcode = encode_xor_str_in_str((Byte *) ilGetData(), img_length(), str, str_len);
  if (errcode != 1) return errcode;
  

  char img_save_path[256];
  strcpy ((char *) img_save_path, path);
  strcat ((char *) img_save_path, ".enc.xor.png");

  if (ilSaveImage(img_save_path) == IL_FALSE)
    return RIS_IMG_SAVE_ERROR;

  return 1;
}

/**
 * Decodes the str in the Image at path
 */
int decode_str_in_img(const char *path, Byte **str, u_int64_t *str_len) {
  if (ilLoadImage(path) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  *str_len = (img_length() / 8);
  *str = (Byte *) malloc((size_t) *str_len);

  int errcode = decode_str_in_str((Byte *) ilGetData(), img_length(), *str, *str_len);
  if (errcode != 1) return errcode;
  
  return 1;
}

/**
 * Decodes the str in the Imge at encoded using xor with the original image at original
 */
int decode_xor_str_in_img(const char *encoded, const char *original, Byte **str, u_int64_t *str_len) {
  if (ilLoadImage(encoded) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  Byte *data = (Byte *) ilGetData();
  u_int64_t enc_len = img_length();

  Byte *enc = (Byte *) malloc((size_t) enc_len);
  memcpy(enc, data, (size_t) enc_len);

  if (ilLoadImage(original) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  Byte *org = (Byte *) ilGetData();
  u_int64_t org_len = img_length();

  *str_len = img_length() / 8;
  *str = (Byte *) malloc((size_t) *str_len);

  int errcode = decode_xor_str_in_str(org, org_len, enc, enc_len, *str, *str_len);
  if (errcode != 1) return errcode;
}

/**
 * Encodes str in the image at path saveing the str_len at the first
 * 32 Bytes in the Image
 */
int encode_len_str_in_img(const char *path, Byte *str, u_int32_t str_len) {
  if (ilLoadImage(path) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  Byte *data = (Byte *) ilGetData();
  u_int64_t img_len = img_length();
  Byte *len = (Byte *) &str_len;

  // encode the length
  int errcode = encode_str_in_str(data, img_len, len, 4);
  if (errcode != 1) return errcode;

  // encode the string
  errcode = encode_str_in_str(data + 32, img_len - 32, str, str_len);
  if (errcode != 1) return errcode;

  char img_save_path[256];
  strcpy ((char *) img_save_path, path);
  strcat ((char *) img_save_path, ".enc.len.png");

  if (ilSaveImage(img_save_path) == IL_FALSE)
    return RIS_IMG_SAVE_ERROR;

  return 1;
}

/**
 * Encodes str in the image at path using xor saveing the str_len at the first
 * 32 Bytes in the image
 */
int encode_len_xor_str_in_img(const char *path, Byte *str, u_int32_t str_len) {
  if (ilLoadImage(path) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  Byte *data = (Byte *) ilGetData();
  u_int64_t img_len = img_length();
  Byte *len = (Byte *) &str_len;

  // encode the length
  int errcode = encode_xor_str_in_str(data, img_len, len, 4);
  if (errcode != 1) return errcode;

  // encode the string
  errcode = encode_xor_str_in_str(data + 32, img_len - 32, str, str_len);
  if (errcode != 1) return errcode;

  char img_save_path[256];
  strcpy ((char *) img_save_path, path);
  strcat ((char *) img_save_path, ".enc.len.xor.png");

  if (ilSaveImage(img_save_path) == IL_FALSE)
    return RIS_IMG_SAVE_ERROR;

  return 1;
}

/**
 * Decodes the str from the image using the length given in the first 
 * 32 Byte of the Image
 */
int decode_len_str_in_img(const char *path, Byte **str, u_int32_t *str_len) {
  if (ilLoadImage(path) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;
  
  Byte len[4];
  Byte *data = (Byte *) ilGetData();
  u_int64_t img_len = img_length();

  int errcode = decode_str_in_str(data , img_len, len, 4);
  if (errcode != 1) return errcode;

  *str_len = *((u_int32_t *) len);
  *str = (Byte *) malloc((size_t) *str_len);

  errcode = decode_str_in_str(data + 32, img_len - 32, *str, *str_len); 
  if (errcode != 1) return errcode;

  return 1;
}

/**
 * Decodes the str in the Imge at encoded using xor with the original image at original
 * using the length given in the first 32 Byte of the Image
 */
int decode_len_xor_str_in_img(const char *encoded, const char *original, Byte **str, u_int32_t *str_len) {
  if (ilLoadImage(encoded) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  Byte *data = (Byte *) ilGetData();
  u_int64_t enc_len = img_length();

  Byte *enc = (Byte *) malloc((size_t) enc_len);
  memcpy(enc, data, (size_t) enc_len);

  if (ilLoadImage(original) == IL_FALSE)
    return RIS_IMG_LOAD_ERROR;

  Byte *org = (Byte *) ilGetData();
  u_int64_t org_len = img_length();
  Byte len[4];

  int errcode = decode_xor_str_in_str(org, org_len, enc, enc_len, len, 4);
  if (errcode != 1) return errcode;

  *str_len = *((u_int32_t *) len);
  *str = (Byte *) malloc((size_t) *str_len);

  errcode = decode_xor_str_in_str(org + 32, org_len - 32, enc + 32, enc_len - 32, *str, *str_len);
  if (errcode != 1) return errcode;

  return 1;
}

