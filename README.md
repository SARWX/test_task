## Описание

Проект представляет собой **систему управления разрешениями**.<br/>
Разрешения выдаются **исполняемым файлам**, которые их **запросили**.<br/>
Непосредственно в проекте реализовано **одно разрешение**, однако добавить **новые не составит труда**.<br/>
Все разрешения хранятся в **базе данных**. При запросе разрешения, в базу вносится _новая_ запись, при обращении к какому-то ресурсу, доступ к которому возможен только при наличии разрешения, оно _будет уточнено_ в базе данных. <br/>
Общение между процессами происходит при помощи  **системы межпроцессного взаимодействия** (inter-process communication, IPC) [D-Bus](https://dbus.freedesktop.org/doc/dbus-tutorial.html).<br/>
* Для работы с системой использовалась библиотека [sdbus-c++](https://github.com/Kistler-Group/sdbus-cpp/tree/master).<br/>
* Для работы с базой данных использовалась свободно распространяемая СУБД [sqlite3](https://www.sqlite.org/). ([установка](https://stackoverflow.com/questions/28969543/fatal-error-sqlite3-h-no-such-file-or-directory))<br/>
В исходном коде оставлены _Doxygen_ комментарии, для документирования.<br/>

Cтруктура проекта следующая:
``` bash
.
├── CMakeLists.txt
├── inc
│   ├── permission_manager.h
│   └── permissions_db.h
├── lib
│   ├── permission_manager.cpp
│   └── permissions_db.cpp
├── scripts
│   ├── clear_permissions.sh
│   ├── commands.txt
│   └── print_permissions.sh
└── src
    ├── permission_server.cpp
    ├── task_3.cpp
    └── time_server.cpp
```

#### CMakeLists.txt

Файл конфигурации для сборки проекта с использованием CMake. Также определяет цель для очистки всех билд-данных.

### inc/
Каталог заголовочных файлов, который содержит:
- **permission_manager.h**: Заголовочный файл с ``enum Permissions``, а также несколькими функциями.
- **permissions_db.h**: Заголовочный файл для класса `permissions_db`, который управляет базой данных SQLite, хранящей разрешения приложений.

### lib/
Каталог исходных файлов динамических библиотек:
- **permission_manager.cpp**: Содержит реализацию функций проверки кода разрешения, а также конвертации PID в абсолютный путь.  
- **permissions_db.cpp**: Реализация класса `permissions_db`, который работает с базой данных SQLite для хранения и извлечения информации о разрешениях.

### scripts/
Каталог скриптов для автоматизации различных действий:
- **clear_permissions.sh**: Скрипт для очистки базы данных разрешений.
- **commands.txt**: Текстовый файл, содержащий пример команд DBus для взаимодействия с сервисами разрешений и времени (из задания).
- **print_permissions.sh**: Скрипт для вывода текущих разрешений в базе данных.

>[!Note]
>для того, чтобы иметь возможность выполнить скрипты их надо предварительно сделать исполняемыми
>```
>chmod +x print_permissions.sh clear_permissions.sh
>```

### src/
Каталог исходных файлов:
- **permission_server.cpp**: Реализация DBus сервиса `com.system.permissions`, который предоставляет методы для запроса и проверки разрешений.
- **time_server.cpp**: Реализация DBus сервиса `com.system.time`, который предоставляет метод для получения текущего системного времени, проверяя разрешение на доступ к времени.
- **task_3.cpp**: Файл, содержащий демонстрационную логику обращения к сервисам.

### База данных
Сама база данных состоит из одной таблицы и имеет следующую структуру:
![Pasted image 20241129021805](https://github.com/user-attachments/assets/88c6b491-132a-446a-8d1f-afa530ec9d0f)

Наложено ограничение ``CONSTRAINT unique_permission``, которое обязывает комбинацию ``app_path`` и ``permission`` быть уникальной.

команда для очистки:
``` bash
cmake --build . --target clean-all
```

## Сборка

Для сборки проекта вам понадобятся  [sdbus-c](https://github.com/Kistler-Group/sdbus-cpp/tree/master), а также [sqlite3](https://ultahost.com/knowledge-base/install-sqlite-on-ubuntu/) 

Откройте корень проекта, создайте папку ``build``, перейдите в нее, вызовите CMake для генерации правил сборки, соберите проект:
``` bash
.                   # 1 - Откройте корень проекта
mkdir build         # 2 - создайте папку ``build``
cd build            # 3 - перейдите в нее
cmake ../           # 4 - вызовите CMake для генерации правил сборки
cmake --build .     # 5 - соберите проект
```

Теперь запустите ``permission_server`` и ``time_server``, после чего запустите ``task_3``, вы увидите ошибку, а после нее системное время. Запустите ``task_3`` повторно, теперь ошибки не будет и время отобразится сразу (потому что task_3 уже получил права)
``` bash
# 6. Запустите permission_server для управления разрешениями 
./permission_server 
# 7. В другом терминале запустите time_server для получения системного времени 
./time_server 
# 8. Теперь запустите task_3, который попробует получить системное время 
./task_3 
# Вы увидите ошибку из-за отсутствия разрешения на доступ к системному времени. 
> Error while calling GetSystemTime: com.system.time.Error.UnauthorizedAccess - Unauthorized access: Application does not have permission to access system time.
# После этого приложение автоматически запросит разрешение и снова попробует получить время. 
> Current time: Fri Nov 29 03:23:28 2024
# 9. Повторно запустите task_3 
./task_3 
# На этот раз ошибка не появится, и текущее системное время отобразится сразу.
Current time: Fri Nov 29 03:23:31 2024
```
## Соблюдение требований [задания](https://github.com/SARWX/test_task/blob/master/DBusPermissionManager_task.pdf)
- Проект должен быть написан на C++ c использованием библиотек <ins>sdbus-c++</ins> или QtDBus <br/>
- Проект должен быть размещен на [GitHub](https://github.com/SARWX/test_task) с подробным README с инструкцией по сборке и использованию <br/>
- В проекте должна быть осмысленная история коммитов <br/> 
    - История начинается с тестового проекта, описанного в [документации к sdbus-c++](https://github.com/Kistler-Group/sdbus-cpp/blob/master/docs/using-sdbus-c++.md#an-example-number-concatenator). Далее она идет последовательно.<br/>
- Проект должен компилироваться под Linux (например Ubuntu 20.04/22.04)<br/>
    - Отладка проекта выполнялась на Ubuntu 22.04.4 LTS, с ядром 6.5.0-21-generic<br/>
- Проект должен компилироваться без предупреждений компилятора<br/>
    - предупреждений при сборке нет, если добавить опцию ``add_compile_options(-Wall -Wextra)`` в CMake, выводятся предупреждения о неиспользуемых параметрах функций (но они нужны, чтобы функции обладали определенной сигнатурой).<br/>
- Проект должен использовать систему сборки [CMake](https://github.com/SARWX/test_task/blob/master/CMakeLists.txt), Meson или QMake<br/>
- Проект должен быть отформатирован при помощи clang-format<br/>
    - все ``.cpp`` и ``.h`` файлы были отформатированы при помощи *clang-format*<br/>
