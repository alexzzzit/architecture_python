#include "integrations/mongo/user_repository.h"
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Cursor.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeParser.h>

namespace db {

using namespace Poco::MongoDB;

UserRepository::UserRepository(Database& database) : database_(database) {}

Document UserRepository::toDocument(const models::User& user) const {
    Document doc;
    doc << "id" << user.id
        << "login" << user.login
        << "email" << user.email
        << "password_hash" << user.passwordHash
        << "role" << models::User::roleToString(user.role)
        << "is_active" << user.isActive
        << "created_at" << Poco::DateTimeFormatter::format(user.createdAt, Poco::DateTimeFormat::ISO8601_FORMAT)
        << "updated_at" << Poco::DateTimeFormatter::format(user.updatedAt, Poco::DateTimeFormat::ISO8601_FORMAT);
    return doc;
}

models::User UserRepository::fromDocument(const Document& doc) const {
    models::User user;
    user.id = doc.get<long long>("id");
    user.login = doc.get<std::string>("login");
    user.email = doc.get<std::string>("email");
    user.passwordHash = doc.get<std::string>("password_hash");
    user.role = models::User::roleFromString(doc.get<std::string>("role"));
    user.isActive = doc.get<bool>("is_active");

    if (doc.has("created_at")) {
        user.createdAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("created_at"));
    }
    if (doc.has("updated_at")) {
        user.updatedAt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, doc.get<std::string>("updated_at"));
    }
    return user;
}

bool UserRepository::create(const models::User& user) {
    try {
        auto collection = database_.getCollection("users");
        return collection.insert(toDocument(user)) > 0;
    } catch (...) {
        return false;
    }
}

std::optional<models::User> UserRepository::findById(long long id) {
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        selector << "id" << id;

        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<models::User> UserRepository::findByLogin(const std::string& login) {
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        selector << "login" << login;

        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<models::User> UserRepository::findByEmail(const std::string& email) {
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        selector << "email" << email;

        Cursor cursor(collection, selector);
        auto response = cursor.next();
        if (response->documents().size() > 0) {
            return fromDocument(*response->documents()[0]);
        }
    } catch (...) {}
    return std::nullopt;
}

std::vector<models::User> UserRepository::findByNamePattern(const std::string& firstName, const std::string& lastName) {
    std::vector<models::User> result;
    try {
        auto collection = database_.getCollection("users");
        Document orClause;
        orClause << "$or" << Document::Array{
            Document() << "last_name" << Document() << "$regex" << lastName << "$options" << "i",
            Document() << "first_name" << Document() << "$regex" << firstName << "$options" << "i"
        };

        Cursor cursor(collection, orClause);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

std::vector<models::User> UserRepository::findAll() {
    std::vector<models::User> result;
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        Cursor cursor(collection, selector);
        auto response = cursor.next();
        for (const auto& doc : response->documents()) {
            result.push_back(fromDocument(*doc));
        }
    } catch (...) {}
    return result;
}

bool UserRepository::update(long long id, const models::User& user) {
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        selector << "id" << id;

        Document updateDoc;
        updateDoc << "$set" << toDocument(user);

        return collection.update(selector, updateDoc) > 0;
    } catch (...) {
        return false;
    }
}

bool UserRepository::deactivate(long long id) {
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        selector << "id" << id;

        Document updateDoc;
        updateDoc << "$set" << Document() 
                  << "is_active" << false 
                  << "updated_at" << Poco::DateTimeFormatter::format(Poco::Timestamp(), Poco::DateTimeFormat::ISO8601_FORMAT);

        return collection.update(selector, updateDoc) > 0;
    } catch (...) {
        return false;
    }
}

bool UserRepository::remove(long long id) {
    try {
        auto collection = database_.getCollection("users");
        Document selector;
        selector << "id" << id;
        return collection.remove(selector) > 0;
    } catch (...) {
        return false;
    }
}

} // namespace db