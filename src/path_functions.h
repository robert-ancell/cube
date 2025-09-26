#pragma once

/// Returns the directory path contains or `NULL` if no parent.
char *path_get_directory(const char *path);

char *path_get_filename(const char *path);
