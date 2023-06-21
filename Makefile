ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), debug_env)
ifndef CC
$(error "CC environment variable is not set")
endif
ifndef ENV_LINUX
$(error "ENV_LINUX environment variable is not set")
endif
endif
endif

src_dir := src

sources := $(wildcard $(src_dir)/*.c)
objects := $(sources:.c=.o)
header_deps := $(sources:.c=.d)

cc := $(CC)

cc_opts :=
cc_opts := ${cc_opts} -g
cc_opts := ${cc_opts} -std=c11
cc_opts := ${cc_opts} -Wall
cc_opts := ${cc_opts} -Werror
cc_opts := ${cc_opts} -Wextra
# cc_opts := ${cc_opts} -pedantic         # Disabled -pedantic, because it complains about forward typedefs for enums :/
# cc_opts := ${cc_opts} -O2
cc_opts := ${cc_opts} -O3

ifeq ($(ENV_LINUX), 1)
linker_opts := -L SDL2-2.24.0/build/build/.libs -lSDL2main -lSDL2 -Wl,-rpath,${DIR}/SDL2-2.24.0/build/build/.libs -lm
main_bin := $(src_dir)/main
else
linker_opts := -L SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -mwindows
main_bin := $(src_dir)/main.exe
endif


all: $(main_bin)

$(main_bin): $(objects)
	$(cc) ${cc_opts} $(objects) -o ${main_bin} ${linker_opts}

# Generate C source files header dependencies (.d files) for "make".
%.d: %.c
	$(cc) -MM -c $(cc_opts) $< > $@

# Need to make some changes to the generated .d file.
	sed -i 's/src\/..\/envconfig.h//' $@
	mv $@ $@.tmp
	(echo -n "$(src_dir)/" && cat $@.tmp) > $@
	rm $@.tmp

# Include all .d files (header dependencies information).
ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), debug_env)
-include $(header_deps)
endif
endif

%.o: %.c %.d
	$(cc) -c $(cc_opts) $< -o $@

$(src_dir)/envconfig.h:
	echo "#define ENV_LINUX $(ENV_LINUX)" > "envconfig.h"


.PHONY: clean debug_env
clean:
	rm -rf $(objects)
	rm -rf $(header_deps)
	rm -f $(main_bin)

debug_env:
	@echo "CC = $(CC)"
	@echo "ENV_LINUX = $(ENV_LINUX)"
	@echo "main_bin = $(main_bin)"
