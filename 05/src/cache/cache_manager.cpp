#include "cache/cache_manager.h"

namespace cache {

// Конструктор создаёт пустой кеш
CacheManager::CacheManager() {}

// Возвращает единственный экземпляр CacheManager
CacheManager& CacheManager::getInstance() {
    static CacheManager instance;
    return instance;
}

// Сохраняет значение в кеш с указанным TTL
// key: уникальный ключ для доступа к значению
// value: строковое значение для кеширования
// ttlSeconds: время жизни записи в секундах (по умолчанию 3600 = 1 час)
void CacheManager::set(const std::string& key, const std::string& value, int ttlSeconds) {
    std::lock_guard<std::mutex> lock(_mutex);
    CacheEntry entry;
    entry.value = value;
    // Вычисляем время истечения
    entry.expiresAt = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
    _cache[key] = entry;
}

// Получает значение из кеша по ключу
// key: уникальный ключ записи
// Возвращает: значение если запись существует и не истекла, иначе nullopt
std::optional<std::string> CacheManager::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _cache.find(key);
    // Проверяем существование и срок жизни
    if (it != _cache.end() && !it->second.isExpired()) {
        return it->second.value;
    }
    // Удаляем истёкшую запись
    if (it != _cache.end()) {
        _cache.erase(it);
    }
    return std::nullopt;
}

// Удаляет запись из кеша по ключу
// key: уникальный ключ записи для удаления
void CacheManager::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(_mutex);
    _cache.erase(key);
}

// Очищает весь кеш
// Используется в тестах для сброса состояния
void CacheManager::clear() {
    std::lock_guard<std::mutex> lock(_mutex);
    _cache.clear();
}

// Возвращает количество записей в кеше
// Возвращает: размер хеш-таблицы кеша
size_t CacheManager::size() const {
    std::lock_guard<std::mutex> lock(_mutex);
    return _cache.size();
}

}
