#pragma once

typedef struct _CubeCommandRunner CubeCommandRunner;

typedef enum {
  CUBE_COMMAND_RUNNER_ERROR_NONE,
  CUBE_COMMAND_RUNNER_ERROR_COMMAND_FAILED
} CubeCommandRunnerError;

#include "cube_command.h"
#include "cube_command_array.h"

typedef struct {
  void (*command_started)(CubeCommandRunner *self, CubeCommand *command,
                          void *user_data);
} CubeCommandRunnerCallbacks;

CubeCommandRunner *
cube_command_runner_new(CubeCommandArray *commands,
                        const CubeCommandRunnerCallbacks *callbacks,
                        void *user_data);

CubeCommandRunner *
cube_command_runner_new_take(CubeCommandArray *commands,
                             const CubeCommandRunnerCallbacks *callbacks,
                             void *user_data);

void cube_command_runner_run(CubeCommandRunner *self);

CubeCommandRunnerError cube_command_runner_get_error(CubeCommandRunner *self);

void cube_command_runner_unref(CubeCommandRunner *self);
