CC=gcc
CFLAGS=-Wall -w -lm -lreadline

#TARGETS=$(shell ls *_app.c | sed 's/\.c//')
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
TARGETS_OBJS=$(filter %_app.o,$(OBJS))
TARGETS=$(patsubst %.o,%,$(TARGETS_OBJS))
TARGETS_EXEC=$(patsubst %_app,%,$(TARGETS))

all: $(TARGETS)

# pull in dependency info for *existing* .o files
-include $(OBJS:.o=.d)

# link only targets
%_app: %_app.o $(filter-out $(TARGETS_OBJS),$(OBJS))
	$(CC) $(CFLAGS) $(filter-out $(TARGETS_OBJS),$(OBJS)) $< -o $*

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d

clean:
	rm *.o *.d $(TARGETS_EXEC)
