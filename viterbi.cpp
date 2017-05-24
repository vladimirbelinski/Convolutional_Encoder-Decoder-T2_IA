#include <bits/stdc++.h>
    
using namespace std;

void init_table(map<string, map<char,string> > &table){
  table["00"] = map<char,string>();
  table["01"] = map<char,string>();
  table["10"] = map<char,string>();
  table["11"] = map<char,string>();
}

struct Context{
  string initial_state; 
  char input;
  string emitted_output, received, final_state;
  int error;
};

vector<Context> context;
map<string, map<char,string> > emitted;
map<string, map<char,string> > next_state;

string encode(string bit_sequence){
  string encoded = "";
  bit_sequence += "00";
  string state = "00";
  for(auto& bit : bit_sequence){
    encoded += emitted[state][bit];     
    state = next_state[state][bit];
  }  
  return encoded;
}

string decode(string bit_sequence){
  string decoded = "";
  //context.push_back(Context({"00","0"}));
  return decoded;
}

int main(void){
  int err_percentage;
  string bit_sequence, encoded, decoded;
  cin >> err_percentage;
  cin >> bit_sequence; 
   
  init_table(emitted);
  init_table(next_state);
  
  emitted["00"]['0'] = "00";
  emitted["00"]['1'] = "11";
  emitted["01"]['0'] = "11";
  emitted["01"]['1'] = "00";
  emitted["10"]['0'] = "10";
  emitted["10"]['1'] = "01";
  emitted["11"]['0'] = "01";
  emitted["11"]['1'] = "10";
  
  next_state["00"]['0'] = "00";
  next_state["00"]['1'] = "10";
  next_state["01"]['0'] = "00";
  next_state["01"]['1'] = "10";
  next_state["10"]['0'] = "01";
  next_state["10"]['1'] = "11";
  next_state["11"]['0'] = "01";
  next_state["11"]['1'] = "11";

  encoded = encode(bit_sequence);
  
  decoded = decode(encoded);
  cout << encoded << endl;
  return 0;
}