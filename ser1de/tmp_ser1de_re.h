#ifndef SER1DE_RE_H
#define SER1DE_RE_H

#include"ser1de.h"
#include <google/protobuf/message.h>
#include <string>
#include <vector>
#include <chrono> // For timing measurements
#include <fstream> // For file output
#include <iomanip> // For formatting
#include <thread> // For thread ID
#include <atomic>
#include <mutex>
#include <memory>

// This is a re-entrant, thread-safe version of Ser1de

class Ser1de_re {
public:
    Ser1de_re(size_t max_threads, std::string execution_path);
    Ser1de_re(size_t max_threads);
    ~Ser1de_re();

    // API exposed to the user
    void SerializeToString(google::protobuf::Message& message, std::string* output);
    void ParseFromString(const std::string& data, google::protobuf::Message* message);

    size_t get_ser_gather_out_size();

private:
    size_t max_threads;
    std::vector<Ser1de*> ser1de_serialize_vec;
    std::vector<Ser1de*> ser1de_parse_vec;
//    std::vector<std::unique_ptr<std::mutex>> serialize_mutexes;
//    std::vector<std::unique_ptr<std::mutex>> parse_mutexes;
    std::atomic<size_t> next_serialize_index{0};
    std::atomic<size_t> next_parse_index{0};
};

Ser1de_re::Ser1de_re(size_t max_threads, std::string execution_path) {
    this->max_threads = max_threads;
    for (size_t i = 0; i < max_threads; i++) {
        ser1de_serialize_vec.push_back(new Ser1de(execution_path));
        ser1de_parse_vec.push_back(new Ser1de(execution_path));
//        serialize_mutexes.push_back(std::make_unique<std::mutex>());
//        parse_mutexes.push_back(std::make_unique<std::mutex>());
    }
}

Ser1de_re::Ser1de_re(size_t max_threads) {
    this->max_threads = max_threads;
    std::cout << "Max threads: " << max_threads << std::endl;
    for (size_t i = 0; i < max_threads; i++) {
        ser1de_serialize_vec.push_back(new Ser1de());
        ser1de_parse_vec.push_back(new Ser1de());
//        serialize_mutexes.push_back(std::make_unique<std::mutex>());
//        parse_mutexes.push_back(std::make_unique<std::mutex>());
    }
}

Ser1de_re::~Ser1de_re() {
    for (size_t i = 0; i < max_threads; i++) {
        delete ser1de_serialize_vec[i];
        delete ser1de_parse_vec[i];
    }
    ser1de_serialize_vec.clear();
    ser1de_parse_vec.clear();
}

void Ser1de_re::SerializeToString(google::protobuf::Message& message, std::string* output) {
    size_t index = next_serialize_index.fetch_add(1, std::memory_order_relaxed) % max_threads;
//    std::lock_guard<std::mutex> lock(*serialize_mutexes[index]);
    //std::cout << "Index: " << index << std::endl;
    ser1de_serialize_vec[index]->SerializeToString(message, output);
}

void Ser1de_re::ParseFromString(const std::string& data, google::protobuf::Message* message) {
    size_t index = next_parse_index.fetch_add(1, std::memory_order_relaxed) % max_threads;
//    std::lock_guard<std::mutex> lock(*parse_mutexes[index]);
    //std::cout << "Index: " << index << std::endl;
    ser1de_parse_vec[index]->ParseFromString(data, message);
}

size_t Ser1de_re::get_ser_gather_out_size() {
    size_t index = next_parse_index.fetch_add(1, std::memory_order_relaxed) % max_threads;
//    std::lock_guard<std::mutex> lock(*parse_mutexes[index]);
    return ser1de_parse_vec[index]->get_ser_gather_out_size();
}


#endif
