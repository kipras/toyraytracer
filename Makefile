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
cc_opts := ${cc_opts} -D_USE_MATH_DEFINES=1		# Need to define _USE_MATH_DEFINES to get M_PI from <math.h>
cc_opts := ${cc_opts} -g
cc_opts := ${cc_opts} -std=c11
cc_opts := ${cc_opts} -Wall
cc_opts := ${cc_opts} -Werror
cc_opts := ${cc_opts} -Wextra
# cc_opts := ${cc_opts} -pedantic         # Disabled -pedantic, because it complains about forward typedefs for enums :/
# cc_opts := ${cc_opts} -O2
cc_opts := ${cc_opts} -O3

ifeq ($(ENV_LINUX), 1)
linker_opts := -Wl,--stack,16777216 -L SDL2-2.24.0/build/build/.libs -lSDL2main -lSDL2 -Wl,-rpath,${DIR}/SDL2-2.24.0/build/build/.libs -lm
main_bin := $(src_dir)/main
else
# I think the default stack size (in windows msys2 msys64/mingw64) is 1MB. This is only enough to render 200x200 images, but not 400x400
# (because we store everything on the stack, including the image buffers).
# Increasing to 16MB allows rendering images of at least up to 1024x1024.
linker_opts := -Wl,--stack,16777216 -L SDL2-devel-2.24.0-mingw/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -mwindows
main_bin := $(src_dir)/main.exe
endif


all: $(main_bin)

$(main_bin): $(objects)
	$(cc) ${cc_opts} $(objects) -o ${main_bin} ${linker_opts}

envconfig.h:
	echo "#define ENV_LINUX ${ENV_LINUX}" > "envconfig.h";

# Generate C source files header dependencies (.d files) for "make".
%.d: %.c envconfig.h
	$(cc) -MM -c $(cc_opts) $< > $@

# Need to make some changes to the generated .d file.
# sed -i 's/src\/..\/envconfig.h//' $@
	mv $@ $@.tmp
	(echo -n "$(src_dir)/" && cat $@.tmp) > $@
	rm $@.tmp

# Include all .d files (header dependencies information).
ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), debug_env)
-include $(header_deps)
endif
endif

%.o: %.c %.d envconfig.h
	$(cc) -c $(cc_opts) $< -o $@


.PHONY: clean debug_env
clean:
	rm -rf $(objects)
	rm -rf $(header_deps)
	rm -f $(main_bin)
	rm -f envconfig.h

debug_env:
	@echo "CC = $(CC)"
	@echo "ENV_LINUX = $(ENV_LINUX)"
	@echo "main_bin = $(main_bin)"
