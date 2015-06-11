OPENFST = /home/loach/kaldi/kaldi-trunk/tools/openfst
CXX     = g++
CXXFLAGS= $(OPENFST)/lib/libfst.a
LIBS    = -ldl
INCS    = -I$(OPENFST)/include
PROG    = fstprintallpath

all: $(PROG)

%: %.cpp
	$(CXX) $< $(CXXFLAGS) $(LIBS) $(INCS) -o $@ 

clean:
	rm -f $(PROG)
