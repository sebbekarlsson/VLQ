#ifndef VLQ_H
#define VLQ_H
#include <stdint.h>

typedef struct VLQ_SEGMENT_STRUCT {
  uint32_t *values;
  uint32_t length;
  uint32_t cont;
} VLQSegment;

typedef struct VLQ_LINE_STRUCT {
  VLQSegment **segments;
  uint32_t length;
} VLQLine;

typedef struct VLQ_DECODE_RESULT_STRUCT {
  VLQLine **lines;
  uint32_t length;
  uint32_t nr_extra_lines;
} VLQDecodeResult;

VLQDecodeResult vlq_decode(const char *vlq_str);

void vlq_decode_segment(uint32_t *intlist, uint32_t *len, const char *segment);

void vlq_decode_result_free(VLQDecodeResult *result);

void vlq_line_free(VLQLine *line);

void vlq_segment_free(VLQSegment *segment);
#endif
