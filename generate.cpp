#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "utils.h"

using namespace std;

void readPhones(const string &file, vector<string> &arr);
void readRules(const string& file, map<string, vector<string> > &rules);

void Usage(char* progName){
   cerr << "Usage: " << progName << " input_phones map_file rules" << endl;
   exit(-1);
}

int main(int argc, char** argv){

   if(argc != 4) Usage(argv[0]);

   string phones_file = argv[1];
   string map_file    = argv[2];
   string rule_file   = argv[3];

   vector<string>      phones;
   map<string, string> phone_map;
   map<string, vector<string> > rules;

   readPhones(phones_file, phones);
   readMap(map_file, phone_map);
   readRules(rule_file, rules);

   trans(phone_map, phones);

   for(int i = 0; i < phones.size(); ++i){
      cout << i << " " << i+1 << " "
         << phones[i] << " " << phones[i] << " 0" << endl;

      const vector<string> &arr = rules[phones[i]];
      for(int j = 0; j < arr.size(); ++j)
      cout << i << " " << i+1 << " "
         << phones[i] << " " << arr[j] << " 1" << endl;

   }

   cout << phones.size() << " 0" << endl;

   return 0;
}

void readPhones(const string &file, vector<string> &arr){
   ifstream fin(file.c_str());

   string tmp;
   while(fin >> tmp){
      arr.push_back(tmp);
   }
}

void readRules(const string& file, map<string, vector<string> > &rules){
   ifstream fin(file.c_str());
   string line;

   while(getline(fin, line)){
      stringstream ss(line);
      string phone, tmp;
      vector<string> arr;

      ss >> phone;
      while( ss >> tmp )
         arr.push_back(tmp);

      rules[phone] = arr;
   }
}
