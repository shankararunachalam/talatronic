#include "types.h"

// carnatic taalas

const typedef struct taala {
  const char *name;
  const u8 id;
  const char *angas[8];
} taala;

const taala taala_type[7] = {
    { "Dhruva", 1, {"I", "O", "I", "I"} },
    { "Matya", 2, {"I", "O", "I"} },
    { "Rupaka", 3, {"O", "I"} },
    { "Triputa", 4, {"I", "O", "O"} },
    { "Jhampa", 5, {"I", "U", "O"} },
    { "Ata", 6, {"I", "I", "O", "O"} },
    { "Eka", 7, {"I"} }
};