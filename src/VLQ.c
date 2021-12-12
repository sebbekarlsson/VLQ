#include <VLQ/VLQ.h>
#include <b64/b64.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *TABLE =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

int char_to_int(char c) {
  uint32_t len = strlen(TABLE);
  for (uint32_t i = 0; i < len; i++) {
    if (c == TABLE[i])
      return i;
  }

  return 0;
}

// long int decode_char(char c) { return ((c & 0x3f) - 1); }

void vlq_decode_segment(uint32_t *intlist, uint32_t *len, const char *segment) {
  uint32_t string_length = strlen(segment);
  uint32_t value = 0;
  uint32_t shift = 0;
  for (uint32_t i = 0; i < string_length; i++) {
    char c = segment[i];
    int integer = char_to_int(c);

    unsigned int has_continuation_bit = integer & 32;

    integer &= 31;

    value += integer << shift;

    if (has_continuation_bit) {
      shift += 5;
    } else {
      value = value >> 1;
      *len += 1;
      intlist[*len - 1] = value;
      value = shift = 0;
    }
  }
}

VLQDecodeResult vlq_decode(const char *vlq_str) {
  VLQDecodeResult result ={};
  vlq_decode_into(&result, vlq_str);
  return result;
}

void vlq_decode_into(VLQDecodeResult* result, const char *vlq_str) {
  VLQLine **lines = 0;
  char *line = 0;
  char *str = strdup(vlq_str);
  char *tmp = str;
  uint32_t nr_extra_lines = 0;
  if (str[0] == ';')
    nr_extra_lines += 1;

  uint32_t nr_lines = 0;
  while ((line = strtok_r(tmp, ";", &tmp))) {
    char *seg = 0;
    char *ptr = line;
    char *rest = ptr;

    VLQSegment **segments = 0;
    VLQLine *l = (VLQLine *)calloc(1, sizeof(VLQLine));

    while ((seg = strtok_r(rest, ",", &rest))) {
      VLQSegment *segment = (VLQSegment *)calloc(1, sizeof(VLQSegment));
      uint32_t i = 0;
      char k = seg[2];
      uint32_t seglen = strlen(seg);
      uint32_t intlist[256];
      uint32_t lenlen = 0;
      vlq_decode_segment(intlist, &lenlen, seg);
      segment->values = (uint32_t *)calloc(lenlen, sizeof(uint32_t));
      memcpy(&segment->values[0], &intlist[0], lenlen * sizeof(uint32_t));
      segment->length = lenlen;
      l->length += 1;
      l->segments =
          (VLQSegment **)realloc(l->segments, l->length * sizeof(VLQSegment *));
      l->segments[l->length - 1] = segment;
    }

    nr_lines += 1;
    lines = (VLQLine **)realloc(lines, nr_lines * sizeof(VLQLine *));
    lines[nr_lines - 1] = l;
  }

  free(str);

  result->lines = lines;
  result->nr_extra_lines =  nr_extra_lines;
  result->length = nr_lines;
}

void vlq_decode_result_free(VLQDecodeResult *result) {
  for (uint32_t i = 0; i < result->length; i++) {
    vlq_line_free(result->lines[i]);
  }

  free(result->lines);
  result->lines = 0;
  result->length = 0;
}

void vlq_line_free(VLQLine *line) {
  for (uint32_t i = 0; i < line->length; i++) {
    vlq_segment_free(line->segments[i]);
  }

  free(line->segments);
  line->segments = 0;
  line->length = 0;
  free(line);
}

void vlq_segment_free(VLQSegment *segment) {
  free(segment->values);
  free(segment);
}
