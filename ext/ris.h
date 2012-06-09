#define ERROR -1

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
  if ((target_len / 8) < source_len) return ERROR;
  
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
  if ((target_len / 8) < source_len) return ERROR;
  
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
  if ((source_len / 8) < target_len) return ERROR;
  
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

  if (src_org_len != src_xor_len 
      || (src_xor_len / 8) < target_len) return ERROR;
  
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
  ilLoadImage(path);
  if (encode_str_in_str((Byte *) ilGetData(), img_length(), str, str_len) == ERROR)
    return ERROR;

  char img_save_path[256];
  strcpy (str, path);
  strcat (str, ".enc.");
  strcat (str, ".png");

  return ilSaveImage(img_save_path);
}

/**
 * Encodes str in the image at path using xor
 */
int encode_xor_str_in_img(const char *path, Byte *str, u_int64_t str_len) {
  ilLoadImage(path);
  if (encode_xor_str_in_str((Byte *) ilGetData(), img_length(), str, str_len) == ERROR)
    return ERROR;

  char img_save_path[256];
  strcpy (str, path);
  strcat (str, ".enc.xor.");
  strcat (str, ".png");

  return ilSaveImage(img_save_path);
}

/**
 * Decodes the str in the Imge at path
 */
int decode_str_in_img(const char *path, Byte **str, u_int64_t *str_len) {
  ilLoadImage(path);
  *str_len = img_length() / 8;
  *str = (Byte *) malloc(sizeof(Byte) * *str_len);

  return decode_str_in_str((Byte *) ilGetData(), img_length(), *str, *str_len);
}

/**
 * Decodes the str in the Imge at encoded using xor with the original image at original
 */
int decode_xor_str_in_img(const char *encoded, const char *original, Byte **str, u_int64_t *str_len) {
  if (ilLoadImage(encoded) == IL_FALSE)
    return ERROR;

  Byte *data = (Byte *) ilGetData();
  u_int64_t enc_len = img_length();

  Byte *enc = (Byte *) malloc(sizeof(Byte) * enc_len);
  memcpy(enc, data, enc_len);

  if (ilLoadImage(original) == IL_FALSE);
    return ERROR;

  Byte *org = (Byte *) ilGetData();
  u_int64_t org_len = img_length();

  *str_len = img_length() / 8;
  *str = (Byte *) malloc(sizeof(Byte) * *str_len);

  return decode_xor_str_in_str(org, org_len, enc, enc_len, *str, *str_len);
}

/**
 * Encodes str in the image at path saveing the str_len at the first
 * 32 Bytes in the Image
 */
int encode_len_str_in_img(const char *path, Byte *str, u_int32_t str_len) {
  ilLoadImage(path);

  Byte *data = (Byte *) ilGetData();
  u_int64_t img_len = img_length();
  Byte len[4] = (Byte *) &str_len;

  // encode the length
  if (encode_str_in_img(data, img_len, len, 4) == ERROR)
    return ERROR;

  // encode the string
  if (encode_str_in_str(data + 32, img_len - 32, str, str_len) == ERROR)
    return ERROR;

  char img_save_path[256];
  strcpy (str, path);
  strcat (str, ".enc.");
  strcat (str, ".png");

  return ilSaveImage(img_save_path);
}

/**
 * Encodes str in the image at path using xor saveing the str_len at the first
 * 32 Bytes in the image
 */
int encode_len_xor_str_in_img(const char *path, Byte *str, u_int32_t str_len) {
  ilLoadImage(path);

  Byte *data = (Byte *) ilGetData();
  u_int64_t img_len = img_length();
  Byte len[4] = (Byte *) &str_len;

  // encode the length
  if (encode_xor_str_in_img(data, img_len, len, 4) == ERROR)
    return ERROR;

  // encode the string
  if (encode_xor_str_in_str(data + 32, img_len - 32, str, str_len) == ERROR)
    return ERROR;

  char img_save_path[256];
  strcpy (str, path);
  strcat (str, ".enc.");
  strcat (str, ".png");

  return ilSaveImage(img_save_path);
}

/**
 * Decodes the str from the image using the length given in the first 
 * 32 Byte of the Image
 */
int decode_len_str_in_img(const char *path, Byte **str, u_int32_t *str_len) {
  ilLoadImage(path);
  
  u_int8_t len[4];
  Byte *data = (Byte *) ilGetData();
  u_int64_t img_len = img_length();

  if (decode_str_in_str(data , img_len, len, 8) == ERROR)
    return ERROR;

  *str_len = *((u_int32_t *) len);
  *str = (Byte *) malloc(sizeof(Byte) * str_len);

 return decode_str_in_str(data + 32, img_len - 32, *str, *str_len); 
}
