SRCS= main.c
REL_TARGET=release/check
DBG_TARGET=debug/check
DBG_OBJS= $(SRCS:%.c=debug/%.o)
REL_OBJS= $(SRCS:%.c=release/%.o)

CPPFLAGS+=-I../include
WFLAGS= -Wall
WFLAGS+=-Werror
WFLAGS+=-Wno-unknown-pragmas
CFLAGS+=$(WFLAGS)

.PHONY: all clean test release debug

all: release debug

release: $(REL_TARGET) 

debug: $(DBG_TARGET)

$(REL_TARGET): $(REL_OBJS)
	$(CC) -o $@ $^

$(DBG_TARGET): $(DBG_OBJS)
	$(CC) -o $@ $^

$(DBG_OBJS): debug/%.o: %.c
	@mkdir -p debug
	$(CC) $(CPPFLAGS) $(CFLAGS) -D__DEBUG -c -o $@ $<

$(REL_OBJS): release/%.o: %.c
	@mkdir -p release
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(DBG_OBJS) $(DBG_TARGET)
	$(RM) $(REL_OBJS) $(REL_TARGET)
	rmdir release
	rmdir debug

test: all
	$(DBG_TARGET)
	$(REL_TARGET)
