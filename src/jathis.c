#include "types.h"

// carnatic taala jathis (jathi laghus)

const typedef struct jathi {
  const char *name;
  const u8 id;
  const u8 aksharas;
} jathi;

const jathi jathi_type[7] = {
    { "Tisra", 1, 3 },
    { "Chatusra", 1, 3 },
    { "Khanda", 1, 3 },
    { "Misra", 1, 3 },
    { "Sankirna", 1, 3 }                
};