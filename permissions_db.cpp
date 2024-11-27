#include "permissions_db.h"
#include <iostream>  // Для std::cerr
#include <sstream>   // Для std::stringstream

permissions_db::permissions_db(const char* db_name) {
    // Открытие базы данных
    if (sqlite3_open(db_name, &db)) {
        std::cerr << "Не удается открыть базу данных" << std::endl;
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    // SQL-запрос для создания таблицы
    const char *sql_create =
    "CREATE TABLE IF NOT EXISTS permissions (\n"
    "request_id INTEGER PRIMARY KEY AUTOINCREMENT,  -- Уникальный идентификатор запроса\n"
    "app_path TEXT NOT NULL,                        -- Путь к исполняемому файлу приложения\n"
    "permission_code INTEGER NOT NULL,              -- Код запрашиваемого разрешения\n"
    "CONSTRAINT unique_permission                   -- Требование уникальности\n"
    "UNIQUE (app_path, permission_code)             -- Комбинации двух полей\n"
    ");";

    // Выполнение запроса на создание таблицы
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql_create, NULL, NULL, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка при создании таблицы" << std::endl;
        std::string error_msg = errMsg;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        throw std::runtime_error(error_msg);
    }
}

permissions_db::~permissions_db() {
    sqlite3_close(db);
}

void permissions_db::insert_permission(const char* app_path, int permission_code) {
    std::stringstream tmp;
    tmp << "INSERT INTO permissions (app_path, permission_code) "
        << "VALUES ('" << app_path << "', " << permission_code << ");";

    std::string sql_insert_str = tmp.str();
    const char* sql_insert = sql_insert_str.c_str();

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql_insert, NULL, NULL, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка при вставке данных" << std::endl;
        std::string error_msg = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error(error_msg);
    }
}
