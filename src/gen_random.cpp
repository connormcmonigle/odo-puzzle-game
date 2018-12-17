#include<iostream>
#include<cstdlib>
#include<fstream>
#include<vector>
#include<cmath>

using namespace std;

int main(){
  size_t n_count = 0; cin >> n_count;
  size_t x, y; cin >> x >> y;
  int n_exit; cin >> n_exit;
  string fname = ""; cin >> fname;
  int max = min(x, y);
  vector<vector<bool>> v(n_count, vector<bool>(n_count, false));
  for(size_t i(0); i < v.size(); ++i){
    for(size_t j(0); j <= i; ++j){
      v[i][j] = rand() % 24 == 0;
      v[j][i] = v[i][j];
    }
  }
  for(auto& i : v){
    for(auto j : i){
      cout << j << " ";
    }
    cout << endl;
  }
  fstream file(fname);
  file << "ODO_1.0" << endl << endl;
  file << n_count << endl << endl;
  int occupied = rand() % n_count;
  for(size_t i(0); i < n_count; ++i){
    file << rand() % max << " ";
    file << rand() % max << " ";
    file << rand() % max << " ";
    if(n_exit > 0 && rand() % 2){
      file << "1 ";
      --n_exit;
    }else{
      file << "0 ";
    }
    file << bool(i == occupied); file << " " << endl;
  }
  file << endl;
  for(auto& i : v){
    for(auto j : i){
      file << j << " ";
    }
    file << endl;
  }
}
