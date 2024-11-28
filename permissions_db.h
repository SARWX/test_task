#ifndef PERMISSIONS_DB_H
#define PERMISSIONS_DB_H

#include <sqlite3.h>

class permissions_db {
public:
  permissions_db(const char *db_name);

  ~permissions_db();

  void insert_permission(const char *app_path, int permission_code);
  bool check_permission(const char *app_path, int permission_code);

private:
  sqlite3 *db; // pointer to the data base object
};

#endif // PERMISSIONS_DB_H
