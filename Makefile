PRG=gnu.exe
GCC=g++

GCCFLAGS=-std=c++17 -pedantic-errors -Wall -Wextra -Werror -O2

VALGRIND_OPTIONS=-q --leak-check=full
DIFF_OPTIONS=-y --strip-trailing-cr --suppress-common-lines

OBJECTS0=spvector.cpp
DRIVER0=driver.cpp

gcc0:
	$(GCC) $(DRIVER0) $(OBJECTS0) $(GCCFLAGS) $(CYGWIN) -o $(PRG)
0 1 2 3 4 5 6 7 8 9 10 11 12 13:
	echo "running test$@"
	@echo "should run in less than 200 ms"
	./$(PRG) $@ >studentout$@
	@echo "lines after the next are mismatches with master output -- see out$@"
	diff out$@ studentout$@ $(DIFF_OPTIONS)
mem0 mem1 mem2 mem3 mem4 mem5 mem6 mem7 mem8 mem9 mem10 mem11 mem12 mem13:
	echo "running memory test $@"
	@echo "should run in less than 2000 ms"
	valgrind $(VALGRIND_OPTIONS) ./$(PRG) $(subst mem,,$@) 1>/dev/null 2>difference$@
	@echo "lines after this are memory errors"; cat difference$@
clean:
	rm -f *.exe *.o *.obj *.tds *.exe.manifest student* difference*
