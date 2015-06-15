OPENFST = /home/loach/kaldi/kaldi-trunk/tools/openfst
CXX     = g++
CXXFLAGS= -g $(OPENFST)/lib/libfst.a
LIBS    = -ldl
INCS    = -I$(OPENFST)/include
PROG    = fstprintallpath counting generate

all: $(PROG)

%: %.cpp
	$(CXX) $< $(CXXFLAGS) $(LIBS) $(INCS) -o $@ 

test: all
	bash -c "./counting <(ls /corpus/timit/train/*/*/*.phn | grep -v sa) <(ls /corpus/timit/train/*/*/*.wrd | grep -v sa) lexicon.txt phones.60-39.map | tee rule.out"
	bash -c "./generate example phones.60-39.map rule.out > example.log"
clean:
	rm -f $(PROG)
