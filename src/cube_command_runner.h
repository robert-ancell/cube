#pragma once

typedef struct _CubeCommandRunner CubeCommandRunner;

#include "cube_command.h"

CubeCommandRunner *cube_command_runner_new(CubeCommand **commands,
                                           size_t commands_length);

void cube_command_runner_run(CubeCommandRunner *self);

void cube_command_runner_unref(CubeCommandRunner *self);
