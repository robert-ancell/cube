#pragma once

typedef struct _CubeCommandRunner CubeCommandRunner;

typedef enum { CUBE_COMMAND_RUNNER_ERROR_NONE } CubeCommandRunnerError;

#include "cube_command.h"

CubeCommandRunner *cube_command_runner_new(CubeCommand **commands,
                                           size_t commands_length);

void cube_command_runner_run(CubeCommandRunner *self);

CubeCommandRunnerError cube_command_runner_get_error(CubeCommandRunner *self);

void cube_command_runner_unref(CubeCommandRunner *self);
