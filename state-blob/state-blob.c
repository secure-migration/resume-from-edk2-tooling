#include <stdio.h>
#include <string.h>

typedef unsigned short UINT16;
typedef unsigned long UINT64;

// This is included from edk2
#include "State.h"

int main() {
  struct cpu_state s;
  memset(&s, 0, sizeof(s));
  memcpy(&s.magic, "CPUSTATE", 8);
  s.version = 0x0100;
#include "generated-state.i"
  memcpy(&s.end_magic, "ENDSTATE", 8);
  FILE *f = fopen("cpu_state.bin", "wb");
  fwrite(&s, sizeof(s), 1, f);
  fclose(f);
  return 0;
}
