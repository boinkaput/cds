CC      = gcc
CFLAGS  = -g -pedantic -Wall -Wextra -Wno-override-init -Wno-override-init-side-effects
VFLAGS  = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
BASEDIR = $(dir $(abspath $(firstword $(MAKEFILE_LIST))))
SRCDIR  = $(BASEDIR)src
TESTDIR = $(BASEDIR)tests
OBJDIR  = $(TESTDIR)/obj
BINDIR  = $(TESTDIR)/bin
LOGDIR  = $(TESTDIR)/log
TESTS   = $(patsubst $(TESTDIR)/%.c, $(BINDIR)/%, $(wildcard $(TESTDIR)/*.c))

.PHONY: all clean

all: test test_mem

test: $(OBJDIR) $(BINDIR) $(TESTS)
	@for test in $(TESTS); do                                                  \
		if $$test; then                                                        \
			echo "\e[32mPassed $$(basename $$test)\e[0m";                      \
		else                                                                   \
			echo "\e[31mFailed $$(basename $$test)\e[0m";                      \
		fi                                                                     \
	done

test_mem: $(OBJDIR) $(BINDIR) $(LOGDIR) $(TESTS)
	@for test in $(TESTS); do                                                  \
		log_file=$(LOGDIR)/$$(basename $$test);								   \
		valgrind $(VFLAGS) --log-file=$$log_file $$test;					   \
		echo "\e[34m$$(basename $$test) - $$log_file";			   		   	   \
		if grep -Eq "no leaks are possible" $$log_file; then				   \
			echo -n "\e[32m";												   \
		else																   \
			echo -n "\e[31m";												   \
		fi;																	   \
		sed "1, $$((														   \
			$$(grep -n 'HEAP SUMMARY' $$log_file | cut -d: -f 1)	   		   \
			- 1																   \
		)) d" $$log_file | sed -E 's/^==.*==\s//g'; 				   		   \
		echo "\e[0m";														   \
	done

$(OBJDIR):
	@mkdir $@

$(BINDIR):
	@mkdir $@

$(LOGDIR):
	@mkdir $@

$(BINDIR)/option_test: $(TESTDIR)/option_test.c $(OBJDIR)/option.o
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/vector_test: $(TESTDIR)/vector_test.c $(OBJDIR)/vector.o			   \
					   $(OBJDIR)/allocator.o $(OBJDIR)/option.o				   \
					   $(OBJDIR)/iterator.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(BASEDIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rf $(BINDIR) $(OBJDIR) $(LOGDIR)
