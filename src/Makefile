BINARY = cube
SOURCES = main.c \
          json_parser.c \
          json_value.c
OBJECTS = $(SOURCES:%.c=.build/%.o)
CFLAGS = -g -Wall

$(BINARY): $(OBJECTS)
	gcc $^ -o $@

DEP_SOURCES = $(SOURCES)
DEPS=$(DEP_SOURCES:%.c=.build/%.d)
-include $(DEPS)

.build/%.o: %.c
	gcc -MMD $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BINARY) $(OBJECTS) $(DEPS)
