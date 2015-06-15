#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#define SIL "sil"

using namespace std;

void readMap(const string& file, map<string, string> &phone_map);
void trans(map<string, string> &phone_map, vector<string> &phones);

void readMap(const string& file, map<string, string> &phone_map){
   ifstream fin(file.c_str());
   string line;
   string from, to;

   while(getline(fin, line)){
      stringstream ss(line);
      ss >> from >> to;
      phone_map[from] = to;
   }
}

// trans to 39 phones and remove sil
void trans(map<string, string> &phone_map, vector<string> &phones){
   for(int i = 0; i < phones.size(); ++i)
      phones[i] = phone_map[phones[i]];

   string pre;
   // remove duplicated
   for(vector<string>::iterator it = phones.begin(); it != phones.end(); ){
      if( it->empty() || it->compare(pre) == 0 || it->compare(SIL) == 0 ){
         it = phones.erase(it);
      }else{
         pre = *it;
         it++;
      }
   }
}
