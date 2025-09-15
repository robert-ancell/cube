BINARY = cube
SOURCES = cube_program.c \
          cube_project.c \
          main.c \
          json_parser.c \
          json_value.c \
          utf8.c
OBJECTS = $(SOURCES:%.c=.build/%.o)
CFLAGS = -g -Wall

$(BINARY): $(OBJECTS)
	gcc $^ -lm -o $@

DEP_SOURCES = $(SOURCES)
DEPS=$(DEP_SOURCES:%.c=.build/%.d)
-include $(DEPS)

.build/%.o: %.c
	gcc -MMD $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BINARY) $(OBJECTS) $(DEPS)
