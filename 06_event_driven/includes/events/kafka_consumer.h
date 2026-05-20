#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <atomic>
#include <Poco/JSON/Object.h>

namespace RdKafka {
    class Conf;
    class KafkaConsumer;
    class Message;
    class Event;
}

namespace MedicalSystem { namespace Events {

class KafkaConsumer {
public:
    KafkaConsumer(const std::string& brokers, 
                 const std::string& topic,
                 const std::string& groupId);
    
    void start();
    void stop();

private:
    void consumeLoop();
    void processMessage(RdKafka::Message* msg);
    
    bool isDuplicate(const std::string& eventId);
    void markProcessed(const std::string& eventId);
    
    void handleMedicalRecordCreated(Poco::JSON::Object::Ptr eventJson);
    void handleMedicalRecordUpdated(Poco::JSON::Object::Ptr eventJson);
    void handleMedicalRecordStatusChanged(Poco::JSON::Object::Ptr eventJson);
    
    class EventHandlerCb;
    std::unique_ptr<EventHandlerCb> eventHandler_;
    
    std::unique_ptr<RdKafka::Conf> conf_;
    std::unique_ptr<RdKafka::Conf> topicConf_;
    std::unique_ptr<RdKafka::KafkaConsumer> consumer_;
    
    std::string brokers_;
    std::string topic_;
    std::string groupId_;
    
    std::atomic<bool> running_;
    std::thread consumerThread_;
    
    std::unordered_set<std::string> processedEvents_;
    std::mutex processedMutex_;
};

}}