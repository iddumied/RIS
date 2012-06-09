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

int encode_str_in_str(Byte *target, long target_len, Byte *source, long source_len) {
  if ((target_len / 8) < source_len) return ERROR;
  
  long i;
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

int encode_xor_str_in_str(Byte *target, long target_len, Byte *source, long source_len) {
  if ((target_len / 8) < source_len) return ERROR;
  
  long i;
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

int decode_str_in_str(Byte *source, long source_len, Byte *target, long target_len) {
  if ((source_len / 8) < target_len) return ERROR;
  
  long i;
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

int decode_xor_str_in_str(Byte *src_org, long src_org_len, Byte *src_xor, 
                            long src_xor_len, Byte *target, long target_len) {

  if (src_org_len != src_xor_len 
      || (src_xor_len / 8) < target_len) return ERROR;
  
  long i;
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
