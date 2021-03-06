CC=gcc
CFLAGS=-pthread

UNAME := $(shell uname -a)
ifeq ($(findstring x86_64,$(UNAME)),x86_64)
    CFLAGS += -mcx16
endif

ifeq ($(findstring armv7l,$(UNAME)),armv7l)
    CFLAGS += -march=armv7-a
endif

SOURCES=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SOURCES))

GO_SOURCES=$(wildcard *.go)
GO_PROGS=$(patsubst %.go,%,$(GO_SOURCES))

JAVA_SOURCES=$(wildcard *.java)
JAVA_PROGS=$(patsubst %.java,%.class,$(JAVA_SOURCES))

all: $(PROGS) $(GO_PROGS) $(JAVA_PROGS)

%: %.c
	- $(CC) $(CFLAGS) -o $@ $<

%.class: %.java
	- javac $<

%: %.go
	- go build $<

EXES=$(basename $(wildcard *.c *.go ))

.PHONY: clean
clean:
	-rm -f $(PROGS) $(EXES) *.class *~

print-%:
	@echo $* = $($*)
