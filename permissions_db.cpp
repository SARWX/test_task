#include "permissions_db.h"
#include <iostream>  // Для std::cerr
#include <sstream>   // Для std::stringstream

int record_exists = 0;

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

bool permissions_db::check_permission(const char* app_path, int permission_code) {
    record_exists = 0;

    std::stringstream tmp;
    tmp << "SELECT * FROM permissions WHERE app_path = '" << app_path 
                << "' AND permission_code = " << permission_code << ";";

    std::string sql_select_str = tmp.str();
    const char* sql_select = sql_select_str.c_str();

    // TEST
    std::cout << sql_select_str << std::endl;

    // bool record_exist = false;
    // callback функция для проверки наличия записей
    auto check_exist_callback =[](void *unused, int count, char **data, char **columns) -> int
    {
        
        if (count > 0)
            record_exists = 1;

        std::cout << "unused : " << unused << std::endl;
        std::cout << "count : " << count << std::endl;
        std::cout << "data : " << data << std::endl;
        std::cout << "columns : " << columns << std::endl;
        return(0);
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql_select, check_exist_callback, &record_exists, &errMsg) != SQLITE_OK) {
    // if (sqlite3_exec(db, sql_select, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка при выполнении SELECT запроса" << std::endl;
        std::string error_msg = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error(error_msg);
    }

    // TEST
    std::cout << record_exists << std::endl;
    return (record_exists != 0);
}
