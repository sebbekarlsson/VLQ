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

const char* EXAMPLE5 = ";QAAA;QACA;;QAEA;QACA;;QAEA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;;QAEA;QACA;;QAEA;QACA;;QAEA;QACA;QACA;;;QAGA;QACA;;QAEA;QACA;;QAEA;QACA;QACA;QACA,0CAA0C,gCAAgC;QAC1E;QACA;;QAEA;QACA;QACA;QACA,wDAAwD,kBAAkB;QAC1E;QACA,iDAAiD,cAAc;QAC/D;;QAEA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;QACA;QACA,yCAAyC,iCAAiC;QAC1E,gHAAgH,mBAAmB,EAAE;QACrI;QACA;;QAEA;QACA;QACA;QACA,2BAA2B,0BAA0B,EAAE;QACvD,iCAAiC,eAAe;QAChD;QACA;QACA;;QAEA;QACA,sDAAsD,+DAA+D;;QAErH;QACA;;;QAGA;QACA;;;;;;;;;;;;;;;AClFA,mEAA+B;AAG/B,SAAS,EAAE;IACP,OAAO,CAAC,GAAG,CAAC,IAAI,CAAC,CAAC;AACtB,CAAC;AAED,SAAS,KAAK;IACV,OAAO,CAAC,GAAG,CAAC,OAAO,CAAC,CAAC;IAErB,WAAG,CAAC,CAAC,EAAE,CAAC,CAAC,CAAC;AACd,CAAC;AAGD,EAAE,EAAE,CAAC;AACL,KAAK,EAAE,CAAC;;;;;;;;;;;;;;;;ACfK,gBAAQ,GAAU,IAAI,CAAC;AAGpC,SAAgB,GAAG,CAAC,CAAQ,EAAE,CAAQ;IACpC,OAAO,CAAC,GAAG,CAAC,GAAG,gBAAQ,CAAC;AAC1B,CAAC;AAFD,kBAEC";

int main(int argc, char *argv[]) {

  VLQDecodeResult result = vlq_decode(EXAMPLE5);


  for (uint32_t line_nr = 0; line_nr < result.length; line_nr++) {
    VLQLine *line = result.lines[line_nr];



    for (uint32_t seg_nr = 0; seg_nr < line->length; seg_nr++) {
      VLQSegment *segment = line->segments[seg_nr];

      VALUE_TYPE original_file = segment->original_source;//segment->values[1];
      if (!original_file) continue;
      printf("=== segment %d ===\n", seg_nr);
      uint32_t v = 0;

      VALUE_TYPE generated_column = segment->values[0];
      VALUE_TYPE original_line_number = segment->values[2];
      VALUE_TYPE original_column = segment->values[3];
      VALUE_TYPE original_name = segment->length >= 5 ? segment->values[4] : 0;


      if (original_file) {

      //printf("generated_column: %d\n", generated_column);
      //printf("original_column: %d\n\n", segment->original_column);
      //printf("original_file: %d\n", original_file);
        printf("original position: %d:%d\n", segment->original_line, segment->original_column);
        printf("original_name: %d\n\n", original_name);

        printf("generated position: %d:%d\n", segment->generated_line, segment->generated_column);
      }
    }
  }

  vlq_decode_result_free(&result);

  return 0;
}
