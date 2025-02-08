#ifndef SER1DE_H
#define SER1DE_H

#include"iaa_comp.h"
#include"scatter_gather.h"
#include <google/protobuf/message.h>
#include <string>
#include <vector>

class Ser1de {
public:
    Ser1de(std::string execution_path);
    Ser1de();
    ~Ser1de();
    void SerializeToString(const google::protobuf::Message& message, std::string* output);
    bool ParseFromString(const std::string& data, google::protobuf::Message* message);

private:
    // IAA
    IAAComp* iaa_compressor;
    IAAComp* iaa_decompressor;
    // DSA
    ScatterGather* dsa_gatherer;
    ScatterGather* dsa_scatterer;
    // Utility variables
    size_t BUFFER_SIZE = 4096; // 4KB
    size_t SCHEMA_SIZE = 1024; // 1KB
    // Intermediate buffers
    std::vector<uint8_t> gather_buffer;
    std::vector<uint8_t> decompression_buffer;
    ScatterGather::Schema gather_schema;
    ScatterGather::Schema scatter_schema;
};

Ser1de::Ser1de(std::string execution_path) {
    if (execution_path == "Hardware") {
        iaa_compressor = new IAAComp(qpl_path_hardware, 1, 16);
        iaa_decompressor = new IAAComp(qpl_path_hardware, 1, 16);
        dsa_gatherer = new ScatterGather(DML_PATH_HARDWARE, 350);
        dsa_scatterer = new ScatterGather(DML_PATH_HARDWARE, 350);
    } else if (execution_path == "Software") {
        iaa_compressor = new IAAComp(qpl_path_software, 1, 16);
        iaa_decompressor = new IAAComp(qpl_path_software, 1, 16);
        dsa_gatherer = new ScatterGather(DML_PATH_SOFTWARE, 350);
        dsa_scatterer = new ScatterGather(DML_PATH_SOFTWARE, 350);
    } else {
        throw std::invalid_argument("Invalid execution path");
    }
    gather_buffer.resize(BUFFER_SIZE);
    decompression_buffer.resize(BUFFER_SIZE);
    gather_schema.resize(SCHEMA_SIZE);
    scatter_schema.resize(SCHEMA_SIZE);
}

Ser1de::Ser1de() {
    iaa_compressor = new IAAComp(qpl_path_hardware, 1, 16);
    iaa_decompressor = new IAAComp(qpl_path_hardware, 1, 16);
    dsa_gatherer = new ScatterGather(DML_PATH_HARDWARE, 350);
    dsa_scatterer = new ScatterGather(DML_PATH_HARDWARE, 350);
    gather_buffer.resize(BUFFER_SIZE);
    decompression_buffer.resize(BUFFER_SIZE);
    gather_schema.resize(SCHEMA_SIZE);
    scatter_schema.resize(SCHEMA_SIZE);
}

Ser1de::~Ser1de() {
    delete iaa_compressor;
    delete iaa_decompressor;
    delete dsa_gatherer;
    delete dsa_scatterer;
}

void Ser1de::SerializeToString(const google::protobuf::Message& message, std::string* output) {
    // Serialize the message to a string
    message.SerializeToString(output);
}

void Ser1de::ParseFromString(const std::string& data, google::protobuf::Message* message) {
    // Parse the string to a message
    message->ParseFromString(data);
}



#endif