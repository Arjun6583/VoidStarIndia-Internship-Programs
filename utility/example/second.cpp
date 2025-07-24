#include <iostream>
#include <fstream>
#include <pthread.h>
#include <utility>
#include <string>
#include "vsi_function.h"

#define TEN_MB (10 * 1024 * 1024) 
#define TOTAL_THREADS 10
using namespace std;

pthread_mutex_t g_file_mutex;

typedef struct fileDataNode 
{
  std::string str_file_data;
  int i_fileIndex;
  fileDataNode *next;

  fileDataNode(const std::string &d, const int &index) : 
    str_file_data(d), i_fileIndex(index),next(nullptr) {}
} FileNode;

typedef struct fileArgument
{
  // FILE *file_reader_pointer;
  HANDLE hFile;
  int i_index;
  fileDataNode **head;
  fileDataNode **tail;

  fileArgument(HANDLE hFile, int index, fileDataNode **head_ptr, fileDataNode **tail_ptr) : 
    hFile(hFile), i_index(index), head(head_ptr), tail(tail_ptr)  {}

} ReadArgs;

typedef struct fileWriteArg 
{
  std::string file_name;
  std::string str_file_data;

  fileWriteArg(const std::string &file_name, const std::string &data) : 
    file_name(file_name), str_file_data(data) {}

} WriteArgs;

void add_data_to_tail(const std::string &data, const int &index, 
                      FileNode **head, FileNode **tail) 
{

  FileNode *new_node = new FileNode(data, index);

  if (*head == nullptr) 
  {
    *head = new_node;
    *tail = new_node;
  } 
  else 
  {
    (*tail)->next = new_node;
    (*tail) = (*tail)->next;
  }
}

void* write_file(void *args) 
{
  pthread_t current_thread_id = pthread_self();
  
  WriteArgs *file_arg = (WriteArgs *)args;
  std::string str_file_name = file_arg->file_name;
  if (str_file_name.empty()) 
  {
    cerr << "Error: File name is empty." << endl;
    delete file_arg;
    return nullptr;
  }

  std::string str_file_data = file_arg->str_file_data;

  delete file_arg;

  // FILE *file_write_pointer = fopen(str_file_name.c_str(), "w");
  HANDLE hFile = vsiCreateFileA(str_file_name.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS, nullptr);
  if(hFile == INVALID_HANDLE_VALUE) 
  {
    cerr << "Error opening file for writing: " << str_file_name << endl;
    return nullptr;
  }
  cout << "\nWriting file in thread: " << (unsigned long)current_thread_id << ", File Name: " << str_file_name << endl;
  
  //size_t bytesWritten = fwrite(str_file_data.c_str(), sizeof(char), str_file_data.size(), file_write_pointer);
  long bytesWritten = 0;
  BOOL res = vsiWriteFile(hFile, str_file_data.c_str(), str_file_data.size(), &bytesWritten, nullptr);
  if(bytesWritten != str_file_data.size()) 
  {
    cerr << "Error writing to file: " << str_file_name << endl;
    vsiCloseFile(hFile);
    return nullptr;
  }

  vsiCloseFile(hFile); 
  return nullptr;
}

void display_data_list(FileNode *head) 
{
  FileNode *current = head;

  while (current != nullptr) {
    const std::string file_name = "split_part_" + to_string(current->i_fileIndex) + ".txt";
    cout << "\nFile Name: " << file_name << " ,First Character: " << current->str_file_data[0] << " ,File offset: " << current->i_fileIndex * TEN_MB << ", Index: " << current->i_fileIndex  << endl;
    current = current->next;
  }

}

void free_data_list(FileNode *head)
{
  FileNode *current = head;

  while (current != nullptr) 
  {
    FileNode *temp = current;
    current = current->next;
    delete temp;
  }
}

bool copy_data_list_with_multithreading(FileNode *head) 
{
  pthread_t threads[TOTAL_THREADS];
  int i_thread_index = 0;
  FileNode *current = head;
  if (current == nullptr) {
    cerr << "Error: No data to write." << endl;
    return false;
  }

  int total_thread_created = 0;
  while (current != nullptr) 
  {
    const std::string file_name = "split_part_" + to_string(current->i_fileIndex) + ".txt";
    WriteArgs *file_arg = new WriteArgs(file_name, current->str_file_data);
    
    int thread_create_status = pthread_create(&threads[i_thread_index], nullptr, write_file, (void *)file_arg);
    if(thread_create_status != 0) 
    {
      cerr << "Error creating thread for file: " << file_name << endl;
      return false;
    }

    total_thread_created++;
    i_thread_index++;
    current = current->next;
  }

  for (int i_thread_index = 0; i_thread_index < total_thread_created; ++i_thread_index) 
  {

    int thread_join_status = pthread_join(threads[i_thread_index], nullptr);
    if (thread_join_status != 0) 
    {
      cerr << "Error joining thread " << i_thread_index << endl;
      return false;
    }
  }
  return true;
}

bool copy_data_list_to_file(FileNode *head)
{
  FileNode *current = head;

  while (current != nullptr)
  {
    std::string file_name = "split_part_" + to_string(current->i_fileIndex) + ".txt";

    // FILE *file_write_pointer = fopen(file_name.c_str(), "w");
    HANDLE hFile = vsiCreateFileA(file_name.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
      cerr << "Error opening file for writing: " << file_name << endl;
      return false;
    }

    // size_t write_bytes = fwrite(current->str_file_data.c_str(), sizeof(char), current->str_file_data.size(), file_write_pointer);
    long write_bytes = 0;
    BOOL res = vsiWriteFile(hFile, current->str_file_data.c_str(), current->str_file_data.size() ,&write_bytes, nullptr);
    if (write_bytes != current->str_file_data.size())
    {
      cerr << "Error writing to file: " << file_name << endl;
      vsiCloseFile(hFile);
      return false;
    }
    
    vsiCloseFile(hFile);
    current = current->next;
  }
  return true;
}

void* read_file(void *args) 
{
  

  ReadArgs *arg = (ReadArgs *)args;
  pthread_t current_thread_id = pthread_self();
  HANDLE hFile = arg->hFile;
  FileNode **head = arg->head;
  FileNode **tail = arg->tail;
  char *pch_buffer = new char[TEN_MB + 1];
  int thread_index = arg->i_index;
  cout << "\nReading file in thread: " << (unsigned long)current_thread_id << endl;

  if(hFile == INVALID_HANDLE_VALUE) 
  {
    cerr << "Error opening file: " << endl;
    return nullptr;
  }

  pthread_mutex_lock(&g_file_mutex);
  // fseek(file_read_pointer, thread_index * TEN_MB, SEEK_SET);
  vsiSeekFile(hFile, thread_index * TEN_MB, FILE_BEGIN,nullptr);
  // size_t bytesRead = fread(pch_buffer, sizeof(char), TEN_MB, file_read_pointer);
  long bytesRead = 0;
  vsiReadFile(hFile, pch_buffer, TEN_MB, &bytesRead, nullptr);
  if(bytesRead == 0) 
  {
    cerr << "Error reading file: " << endl;
    delete [] pch_buffer;
    return nullptr;
  }

  pch_buffer[bytesRead] = '\0';
  add_data_to_tail(string(pch_buffer), thread_index, head, tail);
  pthread_mutex_unlock(&g_file_mutex);

  delete [] pch_buffer;
  delete arg;
  return nullptr;
}

bool split_file(HANDLE hFile, FileNode **head, FileNode **tail)
{

  pthread_t threads[TOTAL_THREADS];
  
  for (int i_thread_count = 0; i_thread_count < TOTAL_THREADS; ++i_thread_count) 
  {
    ReadArgs *file_arg = new ReadArgs(hFile, i_thread_count, head, tail);

    int i_thread_create_status = pthread_create(&threads[i_thread_count], 
    nullptr, 
    read_file,
    file_arg);
    if (i_thread_create_status != 0) 
    {
      cerr << "Error creating thread " << i_thread_count  << endl;
      return false;
    }

  }

  for (int thread_count = 0; thread_count < TOTAL_THREADS; ++thread_count)
  {
    int i_thread_join_status = pthread_join(threads[thread_count], nullptr);
    if (i_thread_join_status != 0)
    {
      cerr << "Error joining thread " << thread_count << endl;
      return false;
    }

  }
  return true;
}

FILE* get_file_pointer(const std::string &fileName) 
{
  FILE *file_read_pointer = fopen(fileName.c_str(), "r");

  if (file_read_pointer == NULL) {
    cerr << "Error opening file for writing: " << fileName << endl;
    return nullptr;
  }

  return file_read_pointer;
}

int main(int argc, char *argv[])
{
  
  if (argc < 2) 
  {
    cerr << "Error: Few Argument missing (file name)" << endl;
    return EXIT_FAILURE;
  }

  //std::string strFileName = argv[1];
  std::string strFileName = "E:\\arjun\\Programming\\C ++\\Practice\\Internship Program\\data\\demo.txt";
  FileNode *head = nullptr, *tail = nullptr;

  //FILE *file_read_pointer = get_file_pointer(strFileName);
  HANDLE hFile = vsiCreateFileA(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
  if (hFile == INVALID_HANDLE_VALUE)
  {
    cerr << "Failed to initialize file: " << strFileName << endl;
    return EXIT_FAILURE;
  }

  bool result = split_file(hFile, &head, &tail);
  if (result == false)
  {
    cerr << "Failed to split file." << endl;
    vsiCloseFile(hFile);
    return EXIT_FAILURE;
  }

  vsiCloseFile(hFile);

  result = copy_data_list_with_multithreading(head);
  if (result == false)
  {
    cout << "Failed to copy data to files." << endl;
    return EXIT_FAILURE;
  }

  display_data_list(head);
  free_data_list(head);
  head = nullptr;
  tail = nullptr;
  return EXIT_SUCCESS;
}