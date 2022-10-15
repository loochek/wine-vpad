.PHONY: default

LIBS = -luuid -ldxguid

VERSIONS = 1_1 1_2 1_3 1_4 9_1_0

all: x86 x64

%_x64.o: %.c
	winegcc ${CFLAGS} -c $? -o $@

%_x86.o: %.c
	winegcc -m32 ${CFLAGS} -c $? -o $@

build/system32/xinput%.dll: specs/xinput%.dll.spec vpad/xinput_impl_x64.o vpad/vpad_pipe_x64.o
	mkdir -p build/system32
	winegcc -shared ${CFLAGS} $? ${LIBS} -o $@
	mv $@.so $@

build/syswow64/xinput%.dll: specs/xinput%.dll.spec vpad/xinput_impl_x86.o vpad/vpad_pipe_x86.o
	mkdir -p build/syswow64
	winegcc -m32 -shared ${CFLAGS} $? ${LIBS} -o $@
	mv $@.so $@

x64: $(foreach VERSION, $(VERSIONS), build/system32/xinput$(VERSION).dll)
x86: $(foreach VERSION, $(VERSIONS), build/syswow64/xinput$(VERSION).dll)

clean:
	rm -rf build
