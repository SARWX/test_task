#include "permissions_db.h"
#include <iostream>
#include <sdbus-c++/sdbus-c++.h>
#include <sstream>
#include <string>

int record_exists = 0; /* result of searching record */

/**
 * @brief Represents a database for storing and checking permissions.
 *
 * This class handles the connection to the SQLite database, allows inserting
 * permissions for applications, and provides functionality for checking if
 * a given application has a specific permission.
 */
permissions_db::permissions_db(const char *db_name) {
  // open database
  if (sqlite3_open(db_name, &db)) {
    std::cerr << "Не удается открыть базу данных" << std::endl;
    throw std::runtime_error(sqlite3_errmsg(db));
  }

  // SQL querry to create (if not exist) table
  const char *sql_create = "CREATE TABLE IF NOT EXISTS permissions (\n"
                           "request_id INTEGER PRIMARY KEY AUTOINCREMENT,\n"
                           "app_path TEXT NOT NULL,\n"
                           "permission_code INTEGER NOT NULL,\n"
                           "CONSTRAINT unique_permission\n"
                           "UNIQUE (app_path, permission_code)\n"
                           ");";

  // execution of table creation querry
  char *errMsg = nullptr;
  if (sqlite3_exec(db, sql_create, NULL, NULL, &errMsg) != SQLITE_OK) {
    std::cerr << "Ошибка при создании таблицы" << std::endl;
    std::string error_msg = errMsg;
    sqlite3_free(errMsg);
    sqlite3_close(db);
    throw std::runtime_error(error_msg);
  }
}

/**
 * @brief Closes the database connection when the object is destroyed.
 */
permissions_db::~permissions_db() { sqlite3_close(db); }

/**
 * @brief Inserts a permission record into the database.
 *
 * This function inserts an application path and a permission code into the
 * `permissions` table of the database.
 *
 * @param app_path The path of the application requesting permission.
 * @param permission_code The code representing the permission being requested.
 *
 * @throws std::runtime_error If the insert operation fails.
 *
 * @note Only unique pair (path + permission) will be successfully inserted
 * @retval None.
 */
void permissions_db::insert_permission(const char *app_path,
                                       int permission_code) {
  std::stringstream tmp;
  tmp << "INSERT INTO permissions (app_path, permission_code) "
      << "VALUES ('" << app_path << "', " << permission_code << ");";

  std::string sql_insert_str = tmp.str();
  const char *sql_insert = sql_insert_str.c_str();

  char *errMsg = nullptr;
  if (sqlite3_exec(db, sql_insert, NULL, NULL, &errMsg) != SQLITE_OK) {
    std::cerr << "Ошибка при вставке данных" << std::endl;
    std::string error_msg = errMsg;
    sqlite3_free(errMsg);
    throw std::runtime_error(error_msg);
  }
}

/**
 * @brief Checks if an application has a specific permission.
 *
 * This function checks the `permissions` table in the database to see if
 * a record exists for the given application path and permission code.
 *
 * @param app_path The path of the application to check.
 * @param permission_code The permission code to check for.
 *
 * @throws std::runtime_error If the select operation fails.
 *
 * @retval true If the application has the specified permission.
 * @retval false If the application does not have the specified permission.
 */
bool permissions_db::check_permission(const char *app_path,
                                      int permission_code) {
  record_exists = 0;

  std::stringstream tmp;
  tmp << "SELECT * FROM permissions WHERE app_path = '" << app_path
      << "' AND permission_code = " << permission_code << ";";

  std::string sql_select_str = tmp.str();
  const char *sql_select = sql_select_str.c_str();

#ifdef DEBUG
  std::cout << sql_select_str << std::endl;
#endif

  // callback function to check if result is not empty
  auto check_exist_callback = [](void *unused, int count, char **data,
                                 char **columns) -> int {
    if (count > 0)
      record_exists = 1;
    return (0);
  };

  char *errMsg = nullptr;
  if (sqlite3_exec(db, sql_select, check_exist_callback, &record_exists,
                   &errMsg) != SQLITE_OK) {
    std::cerr << "Ошибка при выполнении SELECT запроса" << std::endl;
    std::string error_msg = errMsg;
    sqlite3_free(errMsg);
    throw std::runtime_error(error_msg);
  }

#ifdef DEBUG
  std::cout << record_exists << std::endl;
#endif
  return (record_exists != 0);
}
