.PHONY: default

LIBS = -luuid -ldxguid

VERSIONS = 1_1 1_2 1_3 1_4 9_1_0

%_x64.o: %.c
	winegcc ${CFLAGS} -c $? -o $@

%_x86.o: %.c
	winegcc -m32 ${CFLAGS} -c $? -o $@

build/x64/xinput%.dll: specs/xinput%.dll.spec vpad/xinput_impl_x64.o vpad/vpad_pipe_x64.o
	mkdir -p build/x64
	winegcc -shared ${CFLAGS} $? ${LIBS} -o $@
	mv $@.so $@

build/x86/xinput%.dll: specs/xinput%.dll.spec vpad/xinput_impl_x86.o vpad/vpad_pipe_x86.o
	mkdir -p build/x86
	winegcc -m32 -shared ${CFLAGS} $? ${LIBS} -o $@
	mv $@.so $@

x64: $(foreach VERSION, $(VERSIONS), build/x64/xinput$(VERSION).dll)
x86: $(foreach VERSION, $(VERSIONS), build/x86/xinput$(VERSION).dll)

all: x64

clean:
	rm -rf build
