## Проект Сапёр

## Участники (Группа 251-953)
- Малащенко Михаил
- Глечиков Роман
- Шишко Иван

## Что внутри
- **client** — оконный интерфейс на Qt Widgets
- **server** — TCP-сервер на `QTcpServer`
- **common** — общий протокол и утилиты безопасности
- **core** — игровая логика `GameSaper`

## Архитектура
Клиент и сервер общаются по **JSON over TCP**. Каждое сообщение — отдельный JSON-объект, оканчивающийся символом новой строки.

Пример запроса:
```json
{"cmd":"AUTH","login":"user1","password_hash":"..."}
```

Пример ответа:
```json
{"status":"ok","message":"Авторизация успешна","user_id":1,"login":"user1","role":"user"}
```

## Команды протокола
- `AUTH` — авторизация
- `REG` — регистрация
- `TOP10` — получить топ-10 по лучшему времени победы
- `SAVE_RESULT` — сохранить результат игры
- `USERS` — получить статистику всех пользователей (только admin)
- `DELETE_USER` — удалить пользователя (только admin)
- `PING` — тест соединения

## БД
По умолчанию используется **SQLite** (`server_data.sqlite`).

Таблица `users`:
- `id`
- `login`
- `password_hash`
- `role`
- `games_played`
- `games_won`
- `games_lost`
- `best_time`
- `total_score`
- `created_at`

Таблица `game_results`:
- `id`
- `user_id`
- `seconds`
- `won`
- `created_at`

## Админ по умолчанию
При первом запуске сервера создаётся админ:
- **login:** `admin`
- **password:** `Admin123!`

## Сборка
```bash
cmake -S . -B build
cmake --build build
```

После сборки будут доступны два исполняемых файла:
- `saper_server`
- `saper_client`

## Запуск
Сначала сервер:
```bash
./saper_server
```

Потом клиент:
```bash
./saper_client
```
