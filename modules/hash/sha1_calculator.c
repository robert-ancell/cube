#include <stdlib.h>

#include "hash_calculator.h"

struct _Sha1Calculator {
  int ref;
};

Sha1Calculator *sha1_calculator_new() {
  Sha1Calculator *self = malloc(sizeof(Sha1Calculator));

  self->ref = 1;

  return self;
}

void sha1_calculator_feed(Sha1Calculator *self, uint8_t *data,
                          size_t data_length) {
  // FIXME
}

const uint8_t *sha1_calculator_get_hash(Sha1Calculator *self) {
  // FIXME
  return NULL;
}

Sha1Calculator *sha1_calculator_ref(Sha1Calculator *self) {
  self->ref++;
  return self;
}

void sha1_calculator_unref(Sha1Calculator *self) {
  if (--self->ref != 0) {
    return;
  }

  free(self);
}
