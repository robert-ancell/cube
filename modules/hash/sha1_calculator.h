#pragma once

typedef struct _Sha1Calculator Sha1Calculator;

#include <stddef.h>
#include <stdint.h>

Sha1Calculator *sha1_calculator_new();

void sha1_calculator_feed(Sha1Calculator *self, const uint8_t *data,
                          size_t data_length);

const uint8_t *sha1_calculator_get_hash(Sha1Calculator *self);

Sha1Calculator *sha1_calculator_ref(Sha1Calculator *self);

void sha1_calculator_unref(Sha1Calculator *self);
