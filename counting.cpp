#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <algorithm>

#include "utils.h"

#define THRES 10

using namespace std;

// try to match 39-phone to word

typedef struct{
   int    start;
   int    end;
   string word;
} Chunk;

void Usage(char* progName){
   cerr << "Usage: " << progName << " phone_filepath word_filepath lexicon.txt phone.map" << endl;
   exit(-1);
}

void read(const string& file, vector<Chunk>& seq);
void readLexicon(const string &file, map<string, vector<string> > &lexicon_map,
      map<string, string> &phone_map);

int calcPhoneID(map<string, string> &phone_map, vector<string> &phoneID, map<string, int> &phoneIDMap);

void acc(const vector<Chunk> &phones, const vector<Chunk> &words,
      map<string, vector<string> > &lexicon_map, map<string, string> &phone_map,
      vector< vector<int> > &count, map<string, int> &phoneID);

void compare(const vector<string> ref, const vector<string> test,
      vector< vector<int> > &count, map<string, int> &phoneID);

      

int main(int argc, char** argv){
   if(argc != 5) Usage(argv[0]);

   string phone_file   = argv[1];
   string word_file    = argv[2];
   string lexicon_file = argv[3];
   string map_file     = argv[4];

   map<string, vector<string> > lexicon_map;
   map<string, string> phone_map;


   readMap(map_file, phone_map);
   readLexicon(lexicon_file, lexicon_map, phone_map);

   map<string, int>      phoneIDMap;
   vector<string>        phoneID;
   vector< vector<int> > count;

   int size = calcPhoneID(phone_map, phoneID, phoneIDMap);

   count.resize(size);
   for(int i = 0; i < count.size(); ++i)
      count[i].resize(size);

   ifstream fin_phn(phone_file.c_str()), fin_wrd(word_file.c_str());
   string phn_file, wrd_file;
   while( fin_phn >> phn_file && fin_wrd >> wrd_file){
      vector<Chunk> phone_seq, word_seq;

      read(phn_file, phone_seq);
      read(wrd_file,  word_seq);

      acc(phone_seq, word_seq, lexicon_map, phone_map, count, phoneIDMap);
   }

   for(int j = 0; j < phoneID.size(); ++j){
      vector<string> output;
      for(int i = 0; i < phoneID.size(); ++i){
         if(count[i][j] > THRES)
            output.push_back(phoneID[i]);
      }
      if(output.size() != 0){
         cout << phoneID[j] << " " ;
         for(int i = 0; i < output.size(); ++i){
            cout << output[i] << " ";
         }
         cout << endl;
      }
         
   }
   
   //for(int i = 0; i < phoneID.size(); ++i)
   //   cout << phoneID[i] << " ";
   //cout << endl;

   //for(int i = 0; i < count.size(); ++i){
   //   cout << phoneID[i] << " ";
   //   for(int j = 0; j < count[i].size(); ++j)
   //      cout << count[i][j] << " ";
   //   cout << "" << endl;
   //}
   
   return 0;
}

void read(const string& file, vector<Chunk>& seq){
   ifstream fin(file.c_str());
   int start, end;
   string word;
   Chunk tmp;

   while( fin >> start >> end >> word ){
      tmp.start = start;
      tmp.end   = end;
      tmp.word  = word;

      seq.push_back(tmp);
   }
}


void readLexicon(const string &file, map<string, vector<string> > &lexicon_map, 
      map<string, string> &phone_map){

   ifstream fin(file.c_str());

   string line;
   string word, phone;

   while(getline(fin, line)){
      stringstream ss(line);
      vector<string> arr;

      ss >> word;
      while( ss >> phone ) 
         arr.push_back(phone);

      trans(phone_map, arr);
      lexicon_map[word] = arr;
   }
}

int calcPhoneID(map<string, string> &phone_map, vector<string> &phoneID, map<string, int> &phoneIDMap){

   for(map<string, string>::iterator it = phone_map.begin(); it != phone_map.end(); ++it){
      string phone = it->second;

      if(!phone.empty() &&find(phoneID.begin(), phoneID.end(), phone) == phoneID.end())
         phoneID.push_back(phone);
   }

   for(int i = 0; i < phoneID.size(); ++i)
      phoneIDMap[phoneID[i]] = i;

   return phoneID.size();
}

void acc(const vector<Chunk> &phones, const vector<Chunk> &words,
      map<string, vector<string> > &lexicon_map, map<string, string> &phone_map,
      vector< vector<int> > &count, map<string, int> &phoneID){


   int start = 0;
   int end   = 0;

   for(int i = 0; i < words.size(); ++i){
      const vector<string> &ref_phones = lexicon_map[words[i].word];
      //vector<string> ref_phones = lexicon_map["wo"];

      while( phones[start].start < words[i].start ) start++;
      end = start;
      while( phones[end].end < words[i].end ) end++;

      vector<string> real_phones;
      for(int j = start; j <= end; ++j)
         real_phones.push_back(phones[j].word);

      trans(phone_map, real_phones);
      compare(ref_phones, real_phones, count, phoneID);
   }
}

// use a DP to compare two phone sequence and store the results in the count.
void compare(const vector<string> ref, const vector<string> test,
      vector< vector<int> > &count, map<string, int> &phoneID){

   //cout << " ------------------------ " << endl;
   //for(int i = 0; i < ref.size(); ++i)
   //   cout << ref[i] << " ";
   //cout << endl;

   //for(int j = 0; j < test.size(); ++j)
   //   cout << test[j] << " ";
   //cout << endl;

   vector< vector<int> > DP;
   vector< vector<int> > BT;

   DP.resize(ref.size()+1);
   BT.resize(ref.size()+1);
   for(int i = 0; i <= ref.size(); ++i){
      DP[i].resize(test.size()+1);
      BT[i].resize(test.size()+1);
   }

   DP[0][0] = 0;

   const int DEL = 1;
   const int SUB = 2;
   const int INS = 3;
   const int MAT = 4;

   // deletion
   for(int i = 1; i < ref.size(); ++i){
      DP[i][0] = i;
      BT[i][0] = DEL;
   }

   // insertion
   for(int j = 1; j < test.size(); ++j){
      DP[0][j] = j;
      DP[0][j] = INS;
   }

   for(int i = 1; i <= ref.size(); ++i)
      for(int j = 1; j <= test.size(); ++j){
         if(ref[i-1].compare(test[j-1]) == 0){
            DP[i][j] = DP[i-1][j-1];
            BT[i][j] = MAT;
         }else{
            if(DP[i-1][j] < DP[i][j-1] && DP[i-1][j] < DP[i-1][j-1]){
               DP[i][j] = DP[i-1][j] + 1;
               BT[i][j] = DEL;
               
            }else if(DP[i][j-1] < DP[i-1][j] && DP[i][j-1] < DP[i-1][j-1]){
               DP[i][j] = DP[i][j-1] + 1;
               BT[i][j] = INS;

            }else{
               DP[i][j] = DP[i-1][j-1] + 1;
               BT[i][j] = SUB;
               
            }
         }
      }

   // record back track path.
   vector< pair<int, int> > path;
   {
      int i = ref.size(), j = test.size();
      while(i != 0 && j != 0){
         int now = BT[i][j];
         path.push_back(make_pair<int, int>(i, j));

         if( now == DEL ) {
            i = i - 1;
         }else if ( now == SUB ){
            i = i - 1;
            j = j - 1;
            //cout << "SUB " << ref[i] << " for " << test[j] << endl;
         }else if ( now == INS ){
            j = j - 1;
         }else if ( now == MAT ){
            i = i - 1;
            j = j - 1;
         }else{
            assert( false );
         }
      }
   }


   // acc.
   for(int i = path.size() - 1; i >= 0; --i){
      
      int prev = (i == 0) ? MAT : BT[path[i-1].first][path[i-1].second];
      int now  = BT[path[i].first][path[i].second];
      int next = (i == path.size() - 1) ? MAT : BT[path[i+1].first][path[i+1].second];

      if( prev == MAT && now == SUB && next == MAT){
         const string &ref_phone  = ref[path[i].first - 1];
         const string &test_phone = test[path[i].second - 1];

         count[phoneID[ref_phone]][phoneID[test_phone]]++;

         //cout << ref_phone << " <-> " << test_phone << endl;
      }
   }
   //cout << endl;

}

