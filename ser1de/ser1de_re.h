#ifndef SER1DE_RE_H
#define SER1DE_RE_H

#include"ser1de.h"
#include <google/protobuf/message.h>
#include <string>
#include <thread>

// This is a re-entrant, thread-safe version of Ser1de using thread_local storage

class Ser1de_re {
public:
    Ser1de_re(std::string execution_path, size_t num_buffers, size_t num_schemas) : execution_path_(execution_path), NUM_BUFFERS(num_buffers), NUM_SCHEMAS(num_schemas) {}
    Ser1de_re(size_t num_buffers, size_t num_schemas) : execution_path_("Hardware"), NUM_BUFFERS(num_buffers), NUM_SCHEMAS(num_schemas) {}
    Ser1de_re(std::string execution_path) : execution_path_(execution_path), NUM_BUFFERS(16), NUM_SCHEMAS(4) {}
    Ser1de_re() : execution_path_("Hardware"), NUM_BUFFERS(16), NUM_SCHEMAS(4) {}
    ~Ser1de_re() = default;

    // API exposed to the user
    void SerializeToString(google::protobuf::Message& message, std::string* output) {
        get_thread_local_ser1de().SerializeToString(message, output);
    }
    void ParseFromString(const std::string& data, google::protobuf::Message* message) {
        get_thread_local_ser1de().ParseFromString(data, message);
    }
    size_t get_ser_gather_out_size() {
        return get_thread_local_ser1de().get_ser_gather_out_size();
    }

private:
    std::string execution_path_;
    size_t NUM_BUFFERS;
    size_t NUM_SCHEMAS;
    Ser1de& get_thread_local_ser1de() {
        thread_local Ser1de instance{execution_path_, NUM_BUFFERS, NUM_SCHEMAS};
        return instance;
    }
};

#endif
