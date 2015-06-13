#include <iostream>

#include <string>
#include <vector>
#include <fst/fstlib.h>

using namespace std;
using namespace fst;
typedef StdArc::StateId StateId;

void Usage(const char* progName){
   cerr << "Usage: " << progName << " input_file [output_symbol]" << endl
      << endl
      << "print output symbols along all path in a fst." << endl;
   exit(-1);
}

void DFS(StdFst *fst, vector<int32> &path, vector<string> &osym, StateId s);

int main(int argc, char** argv){
   if(argc != 2 && argc != 3) Usage(argv[0]);

   vector<string> osym;
   // output symbol
   if(argc == 3){
      ifstream fin(argv[2]);
      string str; int id;
      while(fin >> str >> id){
         if( osym.size() <= id)
            osym.resize(id + 1);
         osym[id] = str;
      }
   }
   
   
   string in_name = strcmp(argv[1], "-") != 0 ? argv[1] : "";
   StdFst *fst = StdFst::Read(in_name);
   if (!fst) Usage(argv[0]);
   
   vector<int32> path;
   DFS(fst, path, osym, fst->Start());

}

void DFS(StdFst *fst, vector<int32> &path, vector<string> &osym, StateId s){
   ArcIterator<StdFst> aiter(*fst, s);

   if(aiter.Done()){
      if(osym.size() != 0){
         for(int i = 0; i < path.size(); ++i)
            cout << osym[path[i]] << " ";
      }else{
         for(int i = 0; i < path.size(); ++i)
            cout << path[i] << " ";
      }
      cout << endl;
      return;
   }
   for(; !aiter.Done(); aiter.Next()){
      const StdArc &arc = aiter.Value();
      if( arc.olabel != 0 )
         path.push_back(arc.olabel);

      DFS(fst, path, osym, arc.nextstate);
      if( arc.olabel != 0)
         path.pop_back();
   }
}
