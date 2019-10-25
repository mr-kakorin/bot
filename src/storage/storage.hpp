#ifndef WEBRTCSRECORDER_storage_HPP
#define WEBRTCSRECORDER_storage_HPP

#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <list>

class storage {
  SQLite::Database db;
  std::map<int32_t, bool> cashIsExists;
  std::map<std::string, int> cashCategoryValue;
  std::map<int32_t, int> previousCommunice;
  std::map<int32_t, int> previousVoiceCommunice;

  int getCategoryIdByValue(const std::string &);

public:
  explicit storage(const std::string &sqlite3FileName);

  bool isUserExists(int32_t);

  std::string getUserName(int32_t);

  std::list<std::pair<std::string, std::string>> getCommunicationList(int32_t);

  std::string getCommunice(int32_t, const std::string &);

  int getCommuniceId(int32_t, const std::string &);

  int writeLog(int32_t, const std::string &);

  ~storage() = default;
};


#endif //WEBRTCSRECORDER_storage_HPP
