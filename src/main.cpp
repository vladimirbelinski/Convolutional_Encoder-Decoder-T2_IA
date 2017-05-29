/* Arquivo: main.cpp
   Autores: Gabriel Batista Galli, Matheus Antonio Venancio Dall'Rosa e Vladimir Belinski
   Descrição: o presente arquivo faz parte da resolução do Trabalho II do CCR Inteligência Artificial,
              2017-1, do curso de Ciência da Computação da Universidade Federal da Fronteira Sul - UFFS,
              o qual consiste na implementação de um codificador/decodificador convolucional usando o
              algoritmo de Viterbi.
              --> main.cpp é o arquivo principal do trabalho, onde é encontrada a implementação do
              codificador/decodificador convolucional usando o algoritmo de Viterbi.
*/

#include <bits/stdc++.h>

using namespace std;

/* Inicializa tabela de transições e emissões. */
void init_table(map<string, map<char,string> > &table){
  table["00"] = map<char,string>();
  table["01"] = map<char,string>();
  table["10"] = map<char,string>();
  table["11"] = map<char,string>();
}

/*Estado do algoritmo em um determinado momento.*/
struct Context{
  string initial_state;
  char input;
  string emitted_output, received, final_state;
  int error;
};

vector<Context> context;

/*Tabela de emissão.*/
map<string, map<char,string> > emitted;

/*Tabela de transição.*/
map<string, map<char,string> > next_state;

/*Gerador de números aleatórios para desempate e ruído.*/
default_random_engine generator = default_random_engine(chrono::system_clock::now().time_since_epoch().count());  

/*Distribuições uniformes.*/
uniform_int_distribution<int> dices[4];

/*Codificação da entrada.*/
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

int diff(string str1, string str2){
  return (str1[0] != str2[0]) + (str1[1] != str2[1]);
}

/*
  Função utilizada para encontrar o indice do menor erro.
  Caso exista mais de um índice com o menor erro é tomado
  um índice de forma aleatória dentre os índices de menor erro.
*/
int lowest_error(vector<int> &vi){
  vector<int> ties;
  ties.push_back(0);
  for(int i = 1; i < (int)vi.size(); i++){
    if(vi[i] < vi[ties[0]]){      
      ties.clear();
      ties.push_back(i);
    }
    else if(vi[i] == vi[ties[0]]) ties.push_back(i);    
  }
  return ties[0];
  //if(ties.size() == 1) return ties[0];      
  //return ties[dices[ties.size()-1](generator)];
}

/*Decodificação.*/
string decode(string bit_sequence){
  string decoded = "";
  string prefix = bit_sequence.substr(0,2);

  /*step 1.*/
  int err00 = diff(prefix,"00"), err10 = diff(prefix,"11");
  decoded += err00 < err10 ? '0': '1';

  /*step 2.*/
  prefix = bit_sequence.substr(2,2);
  vector<int> diffs({diff(prefix,"00")+err00,diff(prefix,"11")+err00,diff(prefix,"10")+err10,diff(prefix,"01")+err10});
  int lowest_error_index = lowest_error(diffs);
  decoded += (lowest_error_index & 1) + '0';  

  /*General step.*/
  /*[index]=state: [0]=00, [1]=10, [2]=01, [3]=11*/
  for(int i = 4; i < (int)bit_sequence.size(); i += 2){
    prefix = bit_sequence.substr(i,2);
    vector<int> cur_diffs(4);
    cur_diffs[0] = min(diff(prefix,"00")+diffs[0],diff(prefix,"11")+diffs[2]);
    cur_diffs[1] = min(diff(prefix,"11")+diffs[0],diff(prefix,"00")+diffs[2]);
    cur_diffs[2] = min(diff(prefix,"10")+diffs[1],diff(prefix,"01")+diffs[3]);
    cur_diffs[3] = min(diff(prefix,"01")+diffs[1],diff(prefix,"10")+diffs[3]);
    int lowest_error_index = lowest_error(cur_diffs);    
    decoded += (lowest_error_index & 1) + '0';
    for(int k = 0; k < 4; k++) diffs[k] = cur_diffs[k];
  }
  return decoded.substr(0,decoded.size()-2);
}

int comp(string a,string b){
  int total_diff = 0;
  for(int i = 0; i < (int)a.size(); i++) total_diff += (a[i] != b[i]);
  return total_diff;
}

/*Adição de ruído com distribuiçao uniforme na entrada codificada.*/
string noise(string encoded_sequence,int err_percentage){   
  list<int> check;
  for(int i = 0; i < (int)encoded_sequence.size(); i++) check.push_back(i);
  
  /*calculo da quantidade de bits(utilizando arredondamento(inteiro mais próximo))*/
  int quantity = ((double)(err_percentage*encoded_sequence.size())/100.) + 0.5;     
  while(quantity--){        
    auto it = check.begin();
    advance(it,rand()%check.size());
    encoded_sequence[*it] = (encoded_sequence[*it] == '0') ? '1' : '0';
    check.erase(it);
  }
  return encoded_sequence;
}

int main(void){     
  srand(time(NULL)); 
  for(int i = 0; i < 4; i++)
    dices[i] = uniform_int_distribution<int> (0,i);
  
  int err_percentage;
  string bit_sequence, encoded_sequence, decoded_sequence, disturbed_sequence;
  cin >> err_percentage;
  cin >> bit_sequence;

  init_table(emitted);
  init_table(next_state);
  
  /*Construção da tabela de emissão.*/
  emitted["00"]['0'] = "00";
  emitted["00"]['1'] = "11";
  emitted["01"]['0'] = "11";
  emitted["01"]['1'] = "00";
  emitted["10"]['0'] = "10";
  emitted["10"]['1'] = "01";
  emitted["11"]['0'] = "01";
  emitted["11"]['1'] = "10";

  /*Construção da tabela de transição.*/
  next_state["00"]['0'] = "00";
  next_state["00"]['1'] = "10";
  next_state["01"]['0'] = "00";
  next_state["01"]['1'] = "10";
  next_state["10"]['0'] = "01";
  next_state["10"]['1'] = "11";
  next_state["11"]['0'] = "01";
  next_state["11"]['1'] = "11";
  
  encoded_sequence = encode(bit_sequence);
  vector<int> zeros(101,0);
  for(int i = 0; i <= 10; i++){          
    for(int j = 0; j < 100000; j++){        
      disturbed_sequence = noise(encoded_sequence,i);  
      decoded_sequence = decode(disturbed_sequence);
      zeros[i] += (comp(bit_sequence,decoded_sequence) == 0);           
    }
    cout << i << " " << zeros[i]/1000.0 << "%" << endl;
  }
  /*  
  encoded_sequence = encode(bit_sequence);
  disturbed_sequence = noise(encoded_sequence,err_percentage);  
  decoded_sequence = decode(disturbed_sequence);
  cout << "Initial sequence: " << bit_sequence << endl;
  cout << "Encoded sequence: " << encoded_sequence << endl;
  cout << "Disturbed sequence: " << disturbed_sequence << endl; 
  cout << "Decoded sequence: " << decoded_sequence << endl;  
  cout << "Difference between input and output: " << comp(bit_sequence,decoded_sequence) << endl;*/
  return 0;
}
