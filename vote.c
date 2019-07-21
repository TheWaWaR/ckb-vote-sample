#include <string.h>
#include "ckb_syscalls.h"

#define HASH_SIZE 32
#define MAX_OPTION_LEN 128
/* #define DEBUG */

#ifdef DEBUG
#include <stdio.h>
char debug_message[128];
#endif

int check_option(char* options[], size_t option_size, unsigned char voter_lock_hash[]);

int main() {
  unsigned char voters[][HASH_SIZE] =
    {
     // 0xa430e37ac773c6c9c0b3dc44330a39fda04b6c703ea218ec26e6622375c58dd8
     {0xa4, 0x30, 0xe3, 0x7a, 0xc7, 0x73, 0xc6, 0xc9, 0xc0, 0xb3, 0xdc, 0x44, 0x33, 0x0a, 0x39, 0xfd, 0xa0, 0x4b, 0x6c, 0x70, 0x3e, 0xa2, 0x18, 0xec, 0x26, 0xe6, 0x62, 0x23, 0x75, 0xc5, 0x8d, 0xd8},
     // 0xba2e8c0c8c39a58a606d1e1ca668c60f4b5289f3a02b23920b6762afde6b45c1
     {0xba, 0x2e, 0x8c, 0x0c, 0x8c, 0x39, 0xa5, 0x8a, 0x60, 0x6d, 0x1e, 0x1c, 0xa6, 0x68, 0xc6, 0x0f, 0x4b, 0x52, 0x89, 0xf3, 0xa0, 0x2b, 0x23, 0x92, 0x0b, 0x67, 0x62, 0xaf, 0xde, 0x6b, 0x45, 0xc1},
     // 0x3b11cf84a6a268066ba579d09e1fedea875204491cabe3c292421104f872a588
     {0x3b, 0x11, 0xcf, 0x84, 0xa6, 0xa2, 0x68, 0x06, 0x6b, 0xa5, 0x79, 0xd0, 0x9e, 0x1f, 0xed, 0xea, 0x87, 0x52, 0x04, 0x49, 0x1c, 0xab, 0xe3, 0xc2, 0x92, 0x42, 0x11, 0x04, 0xf8, 0x72, 0xa5, 0x88},
    };
  char* options[] =
    {
     "apple",
     "banana",
     "orange",
    };

  size_t voter_size = sizeof(voters) / HASH_SIZE;
  size_t option_size = sizeof options / sizeof options[0];

  volatile uint64_t hash_size = HASH_SIZE;
  int ret;
  unsigned char lock_hash[HASH_SIZE];
  size_t input_index = 0;
  while (1) {
    ret = ckb_load_cell_by_field(lock_hash, &hash_size, 0, input_index,
                                 CKB_SOURCE_INPUT, CKB_CELL_FIELD_LOCK_HASH);
    if (ret != CKB_SUCCESS) {
#ifdef DEBUG
      sprintf(debug_message, "load input#%d's lock hash failed", input_index);
      ckb_debug(debug_message);
#endif
      return ret;
    }
    for (size_t i = 0; i < voter_size; i++) {
      if (memcmp(lock_hash, voters[i], HASH_SIZE) == 0) {
        return check_option(options, option_size, lock_hash);
      }
    }
    input_index += 1;
  }
}

inline int check_option(char* options[], size_t option_size, unsigned char voter_lock_hash[]) {
  volatile uint64_t max_option_len = 0;
  size_t option_len;
  for (size_t i = 0; i < option_size; i++) {
    option_len = strlen(options[i]);
    if (option_len > max_option_len) {
      max_option_len = option_len;
    }
  }

  volatile uint64_t hash_size = HASH_SIZE;
  int ret;
  char option[MAX_OPTION_LEN];
  unsigned char lock_hash[HASH_SIZE];
  size_t output_index = 0;
  while (1) {
    // Output lock hash must match the input lock hash
    ret = ckb_load_cell_by_field(lock_hash, &hash_size, 0, output_index,
                                 CKB_SOURCE_OUTPUT, CKB_CELL_FIELD_LOCK_HASH);
    if (ret != CKB_SUCCESS) {
      return ret;
    }
    if (memcmp(lock_hash, voter_lock_hash, HASH_SIZE) == 0) {
      // Option is in cell output's data field must in options
      ret = ckb_load_cell_by_field(option, &max_option_len, 0, output_index,
                                   CKB_SOURCE_OUTPUT, CKB_CELL_FIELD_DATA);
      if (ret != CKB_SUCCESS) {
#ifdef DEBUG
        sprintf(debug_message, "load output#%d's data failed", output_index);
        ckb_debug(debug_message);
#endif
        return ret;
      }

      for (size_t i = 0; i < option_size; i++) {
        if (memcmp(option, options[i], strlen(options[i])) == 0) {
          return 0;
        }
      }
    }

    output_index += 1;
  }
}

void _start(int argc, char* argv[])
{
  register long a0 asm("a0") = main(argc, argv);
  register long syscall_id asm("a7") = 93;

  asm volatile ("scall" : "+r"(a0) : "r"(syscall_id));
}
