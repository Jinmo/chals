DEPS = $(shell pkg-config --cflags --libs dbus-1 dbus-glib-1 gio-unix-2.0)
CFLAGS = -g -fno-pie -no-pie

all: ../dist/server

../dist/server: server.c gen/interface.c
	gcc $^ -I"." -o $@ $(CFLAGS) $(DEPS)

gen/interface.c: interface.xml
	gdbus-codegen interface.xml \
		--interface-prefix=org.babydbus. \
		--generate-c-code=gen/interface

clean:
	rm -f ../dist/server ../dist/server-introspection.o gen/*
