/*
write cpp code which create file 10MB with random character and number and save it
*/

#include <iostream>
#include <fstream>
#define FILE_SIZE (100 * 1024 * 1024) 
#define CHUNK_SIZE (10 * 1024 * 1024)
using namespace std;

int make_file() {
  fstream file("demo.txt", ios::out);
  if (!file) {
    cout << "Error opening file!" << endl;
    return -1;
  }
  char c = '0';
  long long  cnt = 0;
  srand(time(0));
  while (file.tellp() < FILE_SIZE) {
    if (cnt == CHUNK_SIZE) {
      cnt = 0;
      c += 1;
    }
    file.put(c);
    cnt += 1;
    if (!file) {
      cout << "Error writing to file!" << endl;
      file.close();
      return -1;
    }
  }
  file.close();
  return 0;
}

int main() {
  int result = make_file();
  if(result == -1) {
    cout << "File creation failed!" << endl;
    return -1;
  }
  cout << "File created successfully!" << endl;
  return 0;
}