CC      = gcc
CFLAGS  = -g -Wall
BASEDIR = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
SRCDIR  = $(BASEDIR)src
TESTDIR = $(BASEDIR)tests
OBJDIR  = $(TESTDIR)/obj
BINDIR  = $(TESTDIR)/bin
TESTS   = $(patsubst $(TESTDIR)/%.c, $(BINDIR)/%, $(wildcard $(TESTDIR)/*.c))

.PHONY: all clean

all: $(OBJDIR) $(BINDIR) $(TESTS)

test: $(OBJDIR) $(BINDIR) $(TESTS)
    @for test in $(TESTS); do                                                  \
        if $$test; then                                                        \
            echo "\e[32mPassed $$(basename $$test)\e[0m";                      \
        else                                                                   \
            echo "\e[31mFailed $$(basename $$test)\e[0m";                      \
        fi                                                                     \
    done

$(OBJDIR):
    @mkdir $@

$(BINDIR):
    @mkdir $@

$(BINDIR)/option_test: $(TESTDIR)/option_test.c $(OBJDIR)/option.o
    $(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(BASEDIR)/%.h
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    $(RM) -rf $(BINDIR) $(OBJDIR)
