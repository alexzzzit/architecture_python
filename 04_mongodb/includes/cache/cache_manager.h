#pragma once
#include <string>
#include <optional>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace cache {

// Структура CacheEntry представляет одну запись в кеше
// Содержит значение и время истечения срока жизни (TTL)
struct CacheEntry {
    std::string value;                                    // Закэшированное значение (JSON строка)
    std::chrono::steady_clock::time_point expiresAt;      // Время истечения TTL
    
    // Проверяет, истёк ли срок жизни записи
    // Возвращает: true если текущее время больше expiresAt
    bool isExpired() const;
};

// Класс CacheManager управляет in-memory кешем для часто запрашиваемых данных
// Реализован как Singleton для единой точки доступа к кешу
// Соответствует компоненту "Cache Server" в архитектуре
class CacheManager {
public:
    // Возвращает единственный экземпляр CacheManager (Singleton паттерн)
    static CacheManager& getInstance();
    
    // Сохраняет значение в кеш с указанным TTL
    // key: уникальный ключ для доступа к значению
    // value: строковое значение для кеширования
    // ttlSeconds: время жизни записи в секундах (по умолчанию 1 час)
    void set(const std::string& key, const std::string& value, int ttlSeconds = 3600);
    
    // Получает значение из кеша по ключу
    // key: уникальный ключ записи
    // Возвращает: значение если запись существует и не истекла, иначе nullopt
    std::optional<std::string> get(const std::string& key);
    
    // Удаляет запись из кеша по ключу
    // key: уникальный ключ записи для удаления
    void remove(const std::string& key);
    
    // Очищает весь кеш (используется в тестах)
    void clear();
    
    // Возвращает количество записей в кеше
    // Возвращает: размер хеш-таблицы кеша
    size_t size() const;

private:
    // Конструктор создаёт пустой кеш
    CacheManager();
    
    // Запрещаем копирование экземпляра Singleton
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;
    
    mutable std::mutex _mutex;                              // Мьютекс для потокобезопасности
    std::unordered_map<std::string, CacheEntry> _cache;     // Хеш-таблица для хранения записей
};

}
