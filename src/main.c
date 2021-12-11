#include <VLQ/VLQ.h>
#include <stdio.h>
#include <string.h>

const char *EXAMPLE =
    "AAAA,MAAM,CAAC,MAAM,SAAS,GAAG,OAAO,CAAC;AAEjC,MAAM,CAAC,MAAM,KAAK,GAAG,"
    "GAAG,EAAE;IACtB,OAAO,CAAC,GAAG,CAAC,SAAS,CAAC,CAAC;AAC3B,CAAC,CAAA";
const char *EXAMPLE2 = ";AAAA,OAAO,CAAC,GAAG,CAAC,OAAO,CAAC,CAAC";
const char *EXAMPLE3 = ";AAAA,SAAS,KAAK;IACV,OAAO,CAAC,GAAG,CAAC,OAAO,CAAC,"
                       "CAAC;AACzB,CAAC;AAED,KAAK,EAAE,CAAC";
const char *EXAMPLE4 = ";AAAA,OAAO,CAAC,GAAG,CAAC,OAAO,CAAC,CAAC";

int main(int argc, char *argv[]) {

  VLQDecodeResult result = vlq_decode(EXAMPLE4);

  for (uint32_t line_nr = 0; line_nr < result.length; line_nr++) {
    VLQLine *line = result.lines[line_nr];

    printf("======== line %d ========\n", line_nr + result.nr_extra_lines);

    for (uint32_t seg_nr = 0; seg_nr < line->length; seg_nr++) {
      VLQSegment *segment = line->segments[seg_nr];

      printf("=== segment %d ===\n", seg_nr);
      uint32_t v = 0;

      uint32_t generated_column = segment->values[0];
      uint32_t original_file = segment->values[1];
      uint32_t original_line_number = segment->values[2];
      uint32_t original_column = segment->values[3];
      uint32_t original_name = segment->length >= 5 ? segment->values[4] : 0;

      printf("generated_column: %d\n", generated_column);
      printf("original_file: %d\n", original_file);
      printf("original_line_number: %d\n", original_line_number);
      printf("original_column: %d\n", original_column);
      printf("original_name: %d\n\n", original_name);
    }
  }

  vlq_decode_result_free(&result);

  return 0;
}
