#remove all implicit rules
MAKEFLAGS += -r

BENCHNAME ?= $(shell basename `pwd`)

DFG_TARGETS = $(BENCHNAME)_cdfg.ll

build: $(DFG_TARGETS)

## FOR BUILDING ##

$(DFG_TARGETS): $(BENCHNAME)_gvn.ll
	opt -enable-new-pm=0 -load ../../app-compiler/build/llvm-pass/libCDFGPass.so -fn $(BENCHNAME) -cdfg $(BENCHNAME)_gvn.ll -S -o $(BENCHNAME)_cdfg.ll

$(BENCHNAME)_gvn.ll: $(BENCHNAME).ll
	opt -gvn -mem2reg -memdep -memcpyopt -lcssa -loop-simplify -licm -loop-deletion -indvars -simplifycfg -mergereturn -indvars $(BENCHNAME).ll -S -o $(BENCHNAME)_gvn.ll

$(BENCHNAME).ll: $(BENCHNAME)_ori.ll
	opt -loop-unroll -unroll-count=3 $(BENCHNAME)_ori.ll -S -o $(BENCHNAME).ll

$(BENCHNAME)_ori.ll: $(BENCHNAME).c
	clang -std=gnu89 -D CGRA_COMPILER -target i386-unknown-linux-gnu -c -emit-llvm -O2 -fno-tree-vectorize -fno-unroll-loops $(BENCHNAME).c -S -o $(BENCHNAME)_ori.ll

clean:
	rm -f '$(BENCHNAME).bc' '$(BENCHNAME).ll' $(DFG_TARGETS) '$(BENCHNAME).tag' '$(BENCHNAME).tagged.c'
