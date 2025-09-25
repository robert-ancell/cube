#pragma once

typedef struct _CubeCommandRunner CubeCommandRunner;

typedef enum {
  CUBE_COMMAND_RUNNER_ERROR_NONE,
  CUBE_COMMAND_RUNNER_ERROR_COMMAND_FAILED
} CubeCommandRunnerError;

#include "cube_command.h"
#include "cube_command_array.h"

CubeCommandRunner *cube_command_runner_new(CubeCommandArray *commands);

void cube_command_runner_run(CubeCommandRunner *self);

CubeCommandRunnerError cube_command_runner_get_error(CubeCommandRunner *self);

void cube_command_runner_unref(CubeCommandRunner *self);
