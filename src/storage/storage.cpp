#include "storage.hpp"

storage::storage(const std::string &sqlite3FileName) : db(sqlite3FileName,
                                                          SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
  try {

  } catch (std::exception &e) {
    std::cout << "SQLite exception: " << e.what() << std::endl;
  }
}

bool storage::isUserExists(int32_t user_id) {
  if (cashIsExists.find(user_id) != cashIsExists.end())
    return cashIsExists[user_id];
  std::stringstream query;
  query << "select * from users where user_id=" << user_id << ";";
  SQLite::Statement st(db, query.str());
  cashIsExists[user_id] = st.executeStep();
  return cashIsExists[user_id];
}

std::string storage::getUserName(int32_t user_id) {
  std::stringstream query;
  query << "select value from users inner join names_users on names_users.user_id = users.id and users.user_id="
        << user_id << " inner join names on name_id=names.id order by random() limit 1;";
  SQLite::Statement st(db, query.str());
  if (st.executeStep()) {
    return st.getColumn(0).getString();
  }
  return "I have nothing to say yet :( PLease contact real me";
}

int storage::writeLog(int32_t user_id, const std::string &msg) {
  std::stringstream command;
  command << "insert into logs (user_id, msg) values(" << user_id << ", '" << msg << "');";
  return db.exec(command.str());
}

std::list<std::pair<std::string, std::string>> storage::getCommunicationList(int32_t user_id) {
  std::stringstream query;
  query
      << "select value, name from users inner join users_categories on users_categories.user_id = users.id and users.user_id="
      << user_id << " inner join categories on users_categories.category_id=categories.id;";
  SQLite::Statement st(db, query.str());
  std::list<std::pair<std::string, std::string>> result;
  while (st.executeStep()) {
    result.emplace_back(st.getColumn(0).getString(), st.getColumn(1).getString());
  }
  return result;
}

int storage::getCategoryIdByValue(const std::string &value) {
  if (cashCategoryValue.find(value) != cashCategoryValue.end())
    return cashCategoryValue[value];
  std::stringstream query;
  query << "select id from categories where value ='" << value << "';";
  SQLite::Statement st(db, query.str());
  if (st.executeStep()) {
    cashCategoryValue[value] = st.getColumn(0).getInt();
  } else {
    return -1;
  }
  return cashCategoryValue[value];
}

std::string storage::getCommunice(int32_t user_id, const std::string &comm_value) {
  std::stringstream query;
  auto cat_id = getCategoryIdByValue(comm_value);

  if (cat_id == -1)
    return "I have nothing to say yet :( PLease contact real me";

  if (previousCommunice.find(user_id) == previousCommunice.end()) {
    query
        << "select msg, communice.id from users inner join users_communice_categories on users_communice_categories.user_id = users.id and users.user_id="
        << user_id << " and users_communice_categories.category_id=" << cat_id
        << " inner join communice on communice.id = users_communice_categories.communice_id order by random() limit 1;";
  } else {
    query
        << "select msg, communice.id from users inner join users_communice_categories on users_communice_categories.user_id = users.id and users.user_id="
        << user_id << " and users_communice_categories.category_id=" << cat_id
        << " inner join communice on communice.id = users_communice_categories.communice_id and communice.id !="
        << previousCommunice[user_id] << " order by random() limit 1;";
  }
  SQLite::Statement st(db, query.str());
  std::string result;
  if (st.executeStep()) {
    result = st.getColumn(0).getString();
    previousCommunice[user_id] = st.getColumn(1).getInt();
  } else {
    return "I have nothing to say yet :( PLease contact real me";
  }
  return result;
}

int storage::getCommuniceId(int32_t user_id, const std::string &comm_value) {
  std::stringstream query;
  auto cat_id = getCategoryIdByValue(comm_value);

  if (cat_id == -1)
    return -1;

  if (previousVoiceCommunice.find(user_id) == previousVoiceCommunice.end()) {
    query
        << "select communice.id from users inner join users_communice_categories on users_communice_categories.user_id = users.id and users.user_id="
        << user_id << " and users_communice_categories.category_id=" << cat_id
        << " inner join communice on communice.id = users_communice_categories.communice_id order by random() limit 1;";
  } else {
    query
        << "select communice.id from users inner join users_communice_categories on users_communice_categories.user_id = users.id and users.user_id="
        << user_id << " and users_communice_categories.category_id=" << cat_id
        << " inner join communice on communice.id = users_communice_categories.communice_id and communice.id !="
        << previousVoiceCommunice[user_id] << " order by random() limit 1;";
  }
  SQLite::Statement st(db, query.str());
  std::string result;
  if (st.executeStep()) {
    previousVoiceCommunice[user_id] = st.getColumn(0).getInt();
  } else {
    return -1;
  }
  return previousVoiceCommunice[user_id];
}