#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cube_command_runner.h"

typedef struct {
  pid_t pid;
  bool is_complete;
} CommandStatus;

struct _CubeCommandRunner {
  CubeCommand **commands;
  size_t commands_length;
  CommandStatus *command_status;
};

static bool is_complete(CubeCommandRunner *self) {
  for (size_t i = 0; i < self->commands_length; i++) {
    if (!self->command_status[i].is_complete) {
      return false;
    }
  }

  return true;
}

static bool have_input(CubeCommandRunner *self, const char *input) {
  for (size_t i = 0; i < self->commands_length; i++) {
    StringArray *outputs = cube_command_get_outputs(self->commands[i]);
    size_t outputs_length = string_array_get_length(outputs);

    for (size_t j = 0; j < outputs_length; j++) {
      if (strcmp(string_array_get_element(outputs, j), input) == 0) {
        if (!self->command_status[i].is_complete) {
          return false;
        }
      }
    }
  }

  return true;
}

static bool can_run(CubeCommandRunner *self, CubeCommand *command) {
  StringArray *inputs = cube_command_get_inputs(command);
  size_t inputs_length = string_array_get_length(inputs);
  for (size_t i = 0; i < inputs_length; i++) {
    const char *input = string_array_get_element(inputs, i);
    if (!have_input(self, input)) {
      return false;
    }
  }

  return true;
}

CubeCommandRunner *cube_command_runner_new(CubeCommand **commands,
                                           size_t commands_length) {
  CubeCommandRunner *self = malloc(sizeof(CubeCommandRunner));

  self->commands = malloc(sizeof(CubeCommand *) * commands_length);
  for (size_t i = 0; i < commands_length; i++) {
    self->commands[i] = cube_command_ref(commands[i]);
  }
  self->commands_length = commands_length;
  self->command_status = malloc(sizeof(CommandStatus) * commands_length);
  for (size_t i = 0; i < commands_length; i++) {
    self->command_status[i].pid = -1;
    self->command_status[i].is_complete = false;
  }

  return self;
}

void cube_command_runner_run(CubeCommandRunner *self) {
  while (!is_complete(self)) {
    // Run any commands that are ready.
    for (size_t i = 0; i < self->commands_length; i++) {
      CubeCommand *command = self->commands[i];

      // Already running.
      if (self->command_status[i].pid != -1) {
        continue;
      }

      // Awaiting other commands.
      if (!can_run(self, command)) {
        continue;
      }

      StringArray *args = cube_command_get_args(command);
      size_t args_length = string_array_get_length(args);
      char **argv = malloc(sizeof(char *) * (args_length + 1));
      for (size_t j = 0; j < args_length; j++) {
        argv[j] = strdup(string_array_get_element(args, j));
      }
      argv[args_length] = NULL;

      pid_t pid = fork();
      // FIXME: pid == -1
      if (pid == 0) {
        execvp(argv[0], argv);
        exit(0);
      }
      self->command_status[i].pid = pid;
      for (size_t j = 0; j < args_length; j++) {
        free(argv[j]);
      }
      free(argv);
    }

    // Wait for the next command to complete.
    int status;
    pid_t pid = wait(&status);
    if (pid == -1) {
      continue;
    }
    for (size_t i = 0; i < self->commands_length; i++) {
      if (self->command_status[i].pid == pid) {
        self->command_status[i].is_complete = true;
        break;
      }
    }
  }
}

void cube_command_runner_unref(CubeCommandRunner *self) {
  for (size_t i = 0; i < self->commands_length; i++) {
    cube_command_unref(self->commands[i]);
  }
  free(self->commands);
  free(self->command_status);
  free(self);
}
