#include <VLQ/VLQ.h>
#include <b64/b64.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

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

void slice_str(char ***slices, uint32_t *len, const char *str, char delim) {
  uint32_t length = strlen(str);

  char **l_slices = 0;
  uint32_t nr_slices = 0;

  char *tmp = strdup("");
  for (uint32_t i = 0; i < length; i++) {
    char c = str[i];

    if (c != delim) {
      uint32_t tmp_len = tmp ? strlen(tmp) : 0;
      tmp = (char *)realloc(tmp, ((tmp_len ? tmp_len : 2) + 2) * sizeof(char));
      memset(&tmp[tmp_len], '\0', 2);
      char buff[2];
      buff[0] = c;
      buff[1] = '\0';
      strcat(tmp, buff);
    } else {
      nr_slices++;
      l_slices = (char **)realloc(l_slices, (nr_slices + 1) * sizeof(char *));
      l_slices[MAX(0, nr_slices - 1)] = strdup(tmp ? tmp : "");

      if (tmp) {
        free(tmp);
      }
      tmp = 0;
    }
  }

  if (tmp)
    free(tmp);

  *slices = l_slices;
  *len = nr_slices;
}

// long int decode_char(char c) { return ((c & 0x3f) - 1); }

/*
 *
*     print(vlqval)
    """Decode Base64 VLQ value"""
    results = []
    add = results.append
    shiftsize, flag, mask = _shiftsize, _flag, _mask
    shift = value = 0
    # use byte values and a table to go from base64 characters to integers
    for v in map(_b64table.__getitem__, vlqval.encode("ascii")):
        value += (v & mask) << shift
        if v & flag:
            shift += shiftsize
            continue
        # determine sign and add to results
        add((value >> 1) * (-1 if value & 1 else 1))
        shift = value = 0

    print(results)
    return results
 */

VALUE_TYPE _shiftsize = 5;
VALUE_TYPE _flag = 1 << 5;
VALUE_TYPE _mask = (1 << 5) - 1;
void vlq_decode_segment(VALUE_TYPE *intlist, uint32_t *len,
                        const char *segment) {

  uint32_t string_length = strlen(segment);
  int value = 0;
  int shiftsize = _shiftsize;
  int flag = _flag;
  int mask = _mask;
  int shift = value = 0;
  uint32_t count = 0;

  for (uint32_t i = 0; i < string_length; i++) {
    char c = segment[i];
    int v = char_to_int(c);
    value += (v & mask) << shift;

    if (v & flag) {
      shift += shiftsize;
      continue;
    }

    count++;
    int final_value = ((value >> 1) * (value & 1 ? -1 : 1));
    intlist[count - 1] = final_value;
    shift = value = 0;
  }

  *len = count;

  return;
  /*printf("SEGMENT: %s\n", segment);
  uint32_t string_length = strlen(segment);
  VALUE_TYPE value = 0;
  VALUE_TYPE shift = 0;
  VALUE_TYPE mask = _mask;
  for (uint32_t i = 0; i < string_length; i++) {
    char c = segment[i];
    int integer = char_to_int(c);

    unsigned int has_continuation_bit = integer & 32;

    //integer &= 31;

    value += (integer & mask) << shift;//integer << shift;

    if (has_continuation_bit) {
      shift += 5;
    } else {
      value = value >> 1 * (value & 1 ? -1 : 1);
      *len += 1;
      intlist[*len - 1] = value;
      printf("%d\n", value);
      value = shift = 0;
    }
  }*/
}

VLQDecodeResult vlq_decode(const char *vlq_str) {
  VLQDecodeResult result = {};
  vlq_decode_into(&result, vlq_str);
  return result;
}

void vlq_decode_into(VLQDecodeResult *result, const char *vlq_str) {
  VLQLine **lines = 0;
  char *line = 0;
  char *str = strdup(vlq_str);
  uint32_t nr_extra_lines = 0;

  if (str[0] == ';')
    nr_extra_lines += 1;

  VALUE_TYPE sline = 0;
  VALUE_TYPE spos = 0;
  VALUE_TYPE scol = 0;
  // VALUE_TYPE npos;
  uint32_t nr_lines = 0;
  int i = 0;

  char **line_slices = 0;
  uint32_t lines_len = 0;
  slice_str(&line_slices, &lines_len, vlq_str, ';');
  char **sliceptr = line_slices;

  if (!sliceptr) {
    fprintf(stderr, "VLQ: sliceptr is NULL.\n");
    return;
  }

  while ((line = *sliceptr++)) {
    i++;
    if (!line) {
      fprintf(stderr, "VLQ: line is NULL.\n");
      continue;
    };
    char *seg = 0;
    char *ptr = line;
    char *rest = ptr;

    VLQLine *l = (VLQLine *)calloc(1, sizeof(VLQLine));

    uint32_t gcol = 0;
    while ((seg = strtok_r(rest, ",", &rest))) {
      l->length += 1;

      if (!seg) {
        fprintf(stderr, "VLQ: seg is NULL.\n");
        continue;
      }

      VLQSegment *segment = (VLQSegment *)calloc(1, sizeof(VLQSegment));
      if (!segment) {
        fprintf(stderr, "VLQ: failed to allocate segment.\n");
        continue;
      }
      VALUE_TYPE intlist[256];
      uint32_t lenlen = 0;
      vlq_decode_segment(intlist, &lenlen, seg);

      if (!lenlen) {
        fprintf(stderr, "VLQ: lenlen is 0.\n");
        continue;
      }

      segment->values = (VALUE_TYPE *)calloc(lenlen, sizeof(VALUE_TYPE));
      if (!segment->values) {
        fprintf(stderr, "VLQ: failed to allocate segment->values.\n");
      }
      memcpy(&segment->values[0], &intlist[0], lenlen * sizeof(VALUE_TYPE));
      segment->length = lenlen;

      VALUE_TYPE sd = segment->values[1];
      VALUE_TYPE sld = segment->values[2];
      VALUE_TYPE scd = segment->values[3];
      // VALUE_TYPE namedelta = segment->values[4];
      gcol += segment->values[1];

      spos = spos + sd; // source pos
      sline = sline + sld;
      scol = scol + scd;

      segment->original_line = sline;
      segment->original_column = scol;
      segment->original_source = spos;
      segment->original_file = segment->original_source;
      segment->generated_line = nr_lines;
      segment->generated_column = gcol;

      l->segments =
        (VLQSegment **)realloc(l->segments, l->length * sizeof(VLQSegment *));

      if (!l->segments) {
        fprintf(stderr, "VLQ: Failed to reallocate l->segments.\n");
      }
      l->segments[MAX(0, l->length - 1)] = segment;
    }

    nr_lines += 1;
    lines = (VLQLine **)realloc(lines, nr_lines * sizeof(VLQLine *));
    if (!lines) {
        fprintf(stderr, "VLQ: Failed to reallocate lines.\n");
      }
    lines[MAX(0, nr_lines - 1)] = l;
  }

  for (uint32_t i = 0; i < lines_len; i++) {
    char *v = line_slices[i];
    if (v) {
      free(v);
    }
  }

  if (line_slices) {
    free(line_slices);
  }

  if (str) {
    free(str);
  }

  result->lines = lines;
  result->nr_extra_lines = nr_extra_lines;
  result->length = nr_lines;
}

void vlq_decode_result_free(VLQDecodeResult *result) {
  if (!result) return;
  for (uint32_t i = 0; i < result->length; i++) {
    if (!result->lines[i]) continue;

    vlq_line_free(result->lines[i]);
  }

  free(result->lines);
  result->lines = 0;
  result->length = 0;
}

void vlq_line_free(VLQLine *line) {
  if (!line) return;
  for (uint32_t i = 0; i < line->length; i++) {
    if (!line->segments[i]) continue;

    vlq_segment_free(line->segments[i]);
  }

  if (line->segments) {
    free(line->segments);
  }
  line->segments = 0;
  line->length = 0;
  free(line);
}

void vlq_segment_free(VLQSegment *segment) {
  if (!segment) return;

  if (segment->values) {
    free(segment->values);
  }
  free(segment);
}
