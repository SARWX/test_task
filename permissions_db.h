#ifndef PERMISSIONS_DB_H
#define PERMISSIONS_DB_H

#include <sqlite3.h> // Для работы с SQLite

class permissions_db {
public:
    // Конструктор: открытие базы данных и создание таблицы
    permissions_db(const char* db_name);

    // Деструктор: закрытие базы данных
    ~permissions_db();

    // Публичный метод для вставки данных
    void insert_permission(const char* app_path, int permission_code);

private:
    sqlite3 *db;  // Указатель на объект базы данных
};

#endif // PERMISSIONS_DB_H
