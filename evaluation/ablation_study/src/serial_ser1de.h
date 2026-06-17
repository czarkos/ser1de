#ifndef SERIAL_SER1DE_H
#define SERIAL_SER1DE_H

#include "iaa_comp.h"
#include "sw_only_scatter_gather.h"
#include <google/protobuf/message.h>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>

class Ser1deBase {
protected:
    IAAComp* iaa_comp;
    ScatterGather* scagatherer;

    size_t BUFFER_SIZE = 256 * 4096;
    size_t SCHEMA_SIZE = 256 * 1024;

    size_t ser_gather_out_size;
    uint32_t serComprOutputSize;
    uint32_t serComprSizesSize;
    std::vector<uint8_t> ser_gather_buffer;
    std::unique_ptr<uint8_t[]> ser_compressed_out;
    std::unique_ptr<uint8_t[]> ser_compressed_sizes_out;
    std::vector<uint8_t*> ser_ptrs;
    std::vector<size_t> ser_sizes;

    uint32_t deserComprOutputSize = 0;
    size_t deser_gather_out_size = 0;
    const uint8_t* deser_compressed;
    std::vector<uint8_t> deser_decompress_out;
    std::vector<uint8_t*> deser_ptrs;
    std::vector<size_t> deser_sizes_for_scatter;

    std::vector<std::vector<uint64_t>> ser_latencies;
    std::vector<std::vector<uint64_t>> parse_latencies;
    std::vector<size_t> ser_gather_out_sizes_history;
    std::vector<size_t> deser_gather_out_sizes_history;

    Ser1deBase(std::string execution_path);
    Ser1deBase();
    ~Ser1deBase();

    inline void make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, uint32_t& comprSizesSize, size_t& num_sizes, std::unique_ptr<uint8_t[]>& compressed_sizes, std::unique_ptr<uint8_t[]>& compressed_data);
    void exportLatenciesToCSV(const std::string& serializeFilename, const std::string& parseFilename) const;

public:
    const std::vector<std::vector<uint64_t>>& getSerializeLatencies() const { return ser_latencies; }
    const std::vector<std::vector<uint64_t>>& getParseLatencies() const { return parse_latencies; }
    uint32_t get_ser_gather_out_size() { return ser_gather_out_size; }
    uint32_t get_serComprOutputSize() { return serComprOutputSize; }
};

Ser1deBase::Ser1deBase(std::string execution_path) {
    if (execution_path == "Hardware") {
        iaa_comp = new IAAComp(qpl_path_hardware, 1, 2);
    } else if (execution_path == "Software") {
        iaa_comp = new IAAComp(qpl_path_software, 1, 2);
    } else {
        throw std::invalid_argument("Invalid execution path");
    }
    scagatherer = new ScatterGather();

    ser_compressed_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_compressed_sizes_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_gather_buffer.resize(BUFFER_SIZE, 0);
    deser_decompress_out.resize(BUFFER_SIZE, 0);
    ser_ptrs.reserve(SCHEMA_SIZE);
    ser_sizes.reserve(SCHEMA_SIZE);
    deser_ptrs.reserve(SCHEMA_SIZE);
    deser_sizes_for_scatter.reserve(SCHEMA_SIZE);

    ser_latencies.resize(5);
    parse_latencies.resize(5);
}

Ser1deBase::Ser1deBase() {
    iaa_comp = new IAAComp(qpl_path_hardware, 1, 2);
    scagatherer = new ScatterGather();

    ser_compressed_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_compressed_sizes_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_gather_buffer.resize(BUFFER_SIZE, 0);
    deser_decompress_out.resize(BUFFER_SIZE, 0);
    ser_ptrs.reserve(SCHEMA_SIZE);
    ser_sizes.reserve(SCHEMA_SIZE);
    deser_ptrs.reserve(SCHEMA_SIZE);
    deser_sizes_for_scatter.reserve(SCHEMA_SIZE);

    ser_latencies.resize(5);
    parse_latencies.resize(5);
}

Ser1deBase::~Ser1deBase() {
    delete iaa_comp;
    delete scagatherer;
}

inline void Ser1deBase::make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, uint32_t& comprSizesSize, size_t& num_sizes, std::unique_ptr<uint8_t[]>& compressed_sizes, std::unique_ptr<uint8_t[]>& compressed_data) {
    ser1de_ser_out.reserve(sizeof(out_size) + sizeof(comprOutputSize) + comprSizesSize + comprOutputSize * sizeof(uint8_t));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&out_size), sizeof(out_size));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&comprOutputSize), sizeof(comprOutputSize));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&comprSizesSize), sizeof(comprSizesSize));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&num_sizes), sizeof(num_sizes));
    ser1de_ser_out.append(reinterpret_cast<const char*>(compressed_sizes.get()), comprSizesSize);
    ser1de_ser_out.append(reinterpret_cast<const char*>(compressed_data.get()), comprOutputSize);
}

void Ser1deBase::exportLatenciesToCSV(const std::string& serializeFilename, const std::string& parseFilename) const {
    if (!ser_latencies.empty() && !ser_latencies[0].empty()) {
        std::ofstream serFile(serializeFilename);
        if (serFile.is_open()) {
            serFile << "MessageSize,SchemaGeneration,GatherAndFirstCompression,SecondCompression,WaitForCompressions,MakeHeader,Total\n";

            size_t numIterations = ser_latencies[0].size();
            if (ser_gather_out_sizes_history.size() != numIterations) {
                std::cerr << "Warning: Mismatch between latency array size (" << numIterations
                          << ") and size history array size (" << ser_gather_out_sizes_history.size()
                          << ") for serialization" << std::endl;
            }

            for (size_t i = 0; i < numIterations; i++) {
                if (i < ser_gather_out_sizes_history.size()) {
                    serFile << ser_gather_out_sizes_history[i] << ",";
                } else {
                    serFile << "0,";
                }

                uint64_t total = 0;
                for (size_t j = 0; j < ser_latencies.size(); j++) {
                    if (ser_latencies[j].size() != numIterations) {
                        std::cerr << "Warning: Mismatch in serialization latency array sizes" << std::endl;
                    }

                    if (i < ser_latencies[j].size()) {
                        serFile << ser_latencies[j][i];
                        total += ser_latencies[j][i];
                    } else {
                        serFile << "0";
                    }
                    serFile << ",";
                }
                serFile << total << "\n";
            }
            serFile.close();
            std::cout << "Serialization latencies exported to " << serializeFilename << std::endl;
        } else {
            std::cerr << "Unable to open file " << serializeFilename << " for writing" << std::endl;
        }
    } else {
        std::cout << "No serialization latency data to export" << std::endl;
    }

    if (!parse_latencies.empty() && !parse_latencies[0].empty()) {
        std::ofstream parseFile(parseFilename);
        if (parseFile.is_open()) {
            parseFile << "MessageSize,ReadHeader,Decompress,AllocateAndCreateSchema,WaitForDecompression,Scatter,Total\n";

            size_t numIterations = parse_latencies[0].size();
            if (deser_gather_out_sizes_history.size() != numIterations) {
                std::cerr << "Warning: Mismatch between latency array size (" << numIterations
                          << ") and size history array size (" << deser_gather_out_sizes_history.size()
                          << ") for deserialization" << std::endl;
            }

            for (size_t i = 0; i < numIterations; i++) {
                if (i < deser_gather_out_sizes_history.size()) {
                    parseFile << deser_gather_out_sizes_history[i] << ",";
                } else {
                    parseFile << "0,";
                }

                uint64_t total = 0;
                for (size_t j = 0; j < parse_latencies.size(); j++) {
                    if (parse_latencies[j].size() != numIterations) {
                        std::cerr << "Warning: Mismatch in deserialization latency array sizes" << std::endl;
                    }

                    if (i < parse_latencies[j].size()) {
                        parseFile << parse_latencies[j][i];
                        total += parse_latencies[j][i];
                    } else {
                        parseFile << "0";
                    }
                    parseFile << ",";
                }
                parseFile << total << "\n";
            }
            parseFile.close();
            std::cout << "Parsing latencies exported to " << parseFilename << std::endl;
        } else {
            std::cerr << "Unable to open file " << parseFilename << " for writing" << std::endl;
        }
    } else {
        std::cout << "No parsing latency data to export" << std::endl;
    }
}

class Ser1de : public Ser1deBase {
public:
    Ser1de(std::string execution_path) : Ser1deBase(execution_path) {}
    Ser1de() : Ser1deBase() {}

    void SerializeToString(google::protobuf::Message& message, std::string* output);
    void ParseFromString(const std::string& data, google::protobuf::Message* message);
    void SerializeToStringDebug(google::protobuf::Message& message, std::string* output);
    void ParseFromStringDebug(const std::string& data, google::protobuf::Message* message);
    using Ser1deBase::exportLatenciesToCSV;

private:
    inline void read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed);
};

void Ser1de::SerializeToString(google::protobuf::Message& message, std::string* output) {
    size_t job_id1;
    size_t job_id2;

    message.generate_seperated_schema(ser_ptrs, ser_sizes);
    job_id1 = iaa_comp->compress_non_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t) * 1);
    scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
    job_id2 = iaa_comp->compress_non_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size + 512);
    while (iaa_comp->poll_job(job_id1) != QPL_STS_OK) {}
    while (iaa_comp->poll_job(job_id2) != QPL_STS_OK) {}
    serComprSizesSize = iaa_comp->get_job_compr_size(job_id1);
    serComprOutputSize = iaa_comp->get_job_compr_size(job_id2);

    size_t num_sizes = ser_sizes.size();
    make_header(*output, ser_gather_out_size, serComprOutputSize, serComprSizesSize, num_sizes, ser_compressed_sizes_out, ser_compressed_out);
    ser_ptrs.clear();
    ser_sizes.clear();
}

void Ser1de::ParseFromString(const std::string& data, google::protobuf::Message* message) {
    size_t job_id;
    uint32_t deserComprOutputSizeLocal = 0;

    read_header(data, deser_gather_out_size, deserComprOutputSizeLocal, deser_sizes_for_scatter, deser_compressed);
    job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSizeLocal, deser_decompress_out.data(), deser_gather_out_size);
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    while (iaa_comp->poll_job(job_id) != QPL_STS_OK) {}
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);

    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

inline void Ser1de::read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed) {
    size_t job_id;
    const char* data = ser1de_ser_out.data();
    size_t offset = 0;
    size_t num_sizes = 0;
    uint32_t comprSizesSize = 0;

    out_size = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(out_size);
    comprOutputSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprOutputSize);
    comprSizesSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprSizesSize);
    num_sizes = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(num_sizes);

    const uint8_t* compressed_sizes_data = reinterpret_cast<const uint8_t*>(data + offset);
    sizes_for_scatter.resize(num_sizes);
    job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(compressed_sizes_data), comprSizesSize, reinterpret_cast<uint8_t*>(sizes_for_scatter.data()), num_sizes * sizeof(size_t));
    offset += comprSizesSize;

    compressed = reinterpret_cast<const uint8_t*>(data + offset);
    while (iaa_comp->poll_job(job_id) != QPL_STS_OK) {}
    assert(iaa_comp->poll_job(job_id) == QPL_STS_OK);
}

void Ser1de::SerializeToStringDebug(google::protobuf::Message& message, std::string* output) {
    size_t job_id1;
    size_t job_id2;
    std::vector<uint64_t> current_latencies(5, 0);

    auto start = std::chrono::high_resolution_clock::now();
    message.generate_seperated_schema(ser_ptrs, ser_sizes);
    job_id1 = iaa_comp->compress_non_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t));
    auto end = std::chrono::high_resolution_clock::now();
    current_latencies[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    ser_gather_out_sizes_history.push_back(ser_gather_out_size);

    start = std::chrono::high_resolution_clock::now();
    job_id2 = iaa_comp->compress_non_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size + 512);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[2] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    while (iaa_comp->poll_job(job_id1) != QPL_STS_OK) {}
    serComprSizesSize = iaa_comp->get_job_compr_size(job_id1);
    while (iaa_comp->poll_job(job_id2) != QPL_STS_OK) {}
    serComprOutputSize = iaa_comp->get_job_compr_size(job_id2);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[3] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    size_t num_sizes = ser_sizes.size();
    make_header(*output, ser_gather_out_size, serComprOutputSize, serComprSizesSize, num_sizes, ser_compressed_sizes_out, ser_compressed_out);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[4] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t i = 0; i < 5; i++) {
        ser_latencies[i].push_back(current_latencies[i]);
    }

    ser_ptrs.clear();
    ser_sizes.clear();
}

void Ser1de::ParseFromStringDebug(const std::string& data, google::protobuf::Message* message) {
    size_t job_id;
    uint32_t deserComprOutputSizeLocal = 0;
    std::vector<uint64_t> current_latencies(5, 0);

    auto start = std::chrono::high_resolution_clock::now();
    read_header(data, deser_gather_out_size, deserComprOutputSizeLocal, deser_sizes_for_scatter, deser_compressed);
    auto end = std::chrono::high_resolution_clock::now();
    current_latencies[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    deser_gather_out_sizes_history.push_back(deser_gather_out_size);

    start = std::chrono::high_resolution_clock::now();
    job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSizeLocal, deser_decompress_out.data(), deser_gather_out_size);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[2] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    while (iaa_comp->poll_job(job_id) != QPL_STS_OK) {}
    assert(iaa_comp->poll_job(job_id) == QPL_STS_OK);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[3] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[4] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t i = 0; i < 5; i++) {
        parse_latencies[i].push_back(current_latencies[i]);
    }

    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

class Serial_Ser1de : public Ser1deBase {
public:
    Serial_Ser1de(std::string execution_path) : Ser1deBase(execution_path) {}
    Serial_Ser1de() : Ser1deBase() {}

    void SerializeToString(google::protobuf::Message& message, std::string* output);
    void ParseFromString(const std::string& data, google::protobuf::Message* message);
    void SerializeToStringDebug(google::protobuf::Message& message, std::string* output);
    void ParseFromStringDebug(const std::string& data, google::protobuf::Message* message);
    using Ser1deBase::exportLatenciesToCSV;

private:
    inline void read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed);
};

void Serial_Ser1de::SerializeToString(google::protobuf::Message& message, std::string* output) {
    message.generate_seperated_schema(ser_ptrs, ser_sizes);
    iaa_comp->compress_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t) * 1, &serComprSizesSize);
    scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
    iaa_comp->compress_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size + 512, &serComprOutputSize);

    size_t num_sizes = ser_sizes.size();
    make_header(*output, ser_gather_out_size, serComprOutputSize, serComprSizesSize, num_sizes, ser_compressed_sizes_out, ser_compressed_out);
    ser_ptrs.clear();
    ser_sizes.clear();
}

void Serial_Ser1de::ParseFromString(const std::string& data, google::protobuf::Message* message) {
    uint32_t deserComprOutputSizeLocal = 0;
    uint32_t actual_decompressed_size = 0;

    read_header(data, deser_gather_out_size, deserComprOutputSizeLocal, deser_sizes_for_scatter, deser_compressed);
    iaa_comp->decompress_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSizeLocal, deser_decompress_out.data(), deser_gather_out_size, &actual_decompressed_size);
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);

    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

inline void Serial_Ser1de::read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed) {
    const char* data = ser1de_ser_out.data();
    size_t offset = 0;
    size_t num_sizes = 0;
    uint32_t comprSizesSize = 0;
    uint32_t decomprSizesSize = 0;

    out_size = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(out_size);
    comprOutputSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprOutputSize);
    comprSizesSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprSizesSize);
    num_sizes = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(num_sizes);

    const uint8_t* compressed_sizes_data = reinterpret_cast<const uint8_t*>(data + offset);
    sizes_for_scatter.resize(num_sizes);
    iaa_comp->decompress_blocking(const_cast<uint8_t*>(compressed_sizes_data), comprSizesSize, reinterpret_cast<uint8_t*>(sizes_for_scatter.data()), num_sizes * sizeof(size_t), &decomprSizesSize);
    offset += comprSizesSize;

    compressed = reinterpret_cast<const uint8_t*>(data + offset);
}

void Serial_Ser1de::SerializeToStringDebug(google::protobuf::Message& message, std::string* output) {
    std::vector<uint64_t> current_latencies(5, 0);

    auto start = std::chrono::high_resolution_clock::now();
    message.generate_seperated_schema(ser_ptrs, ser_sizes);
    auto end = std::chrono::high_resolution_clock::now();
    current_latencies[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    iaa_comp->compress_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t), &serComprSizesSize);
    scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    ser_gather_out_sizes_history.push_back(ser_gather_out_size);

    start = std::chrono::high_resolution_clock::now();
    iaa_comp->compress_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size + 512, &serComprOutputSize);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[2] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    size_t num_sizes = ser_sizes.size();
    make_header(*output, ser_gather_out_size, serComprOutputSize, serComprSizesSize, num_sizes, ser_compressed_sizes_out, ser_compressed_out);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[4] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t i = 0; i < 5; i++) {
        ser_latencies[i].push_back(current_latencies[i]);
    }

    ser_ptrs.clear();
    ser_sizes.clear();
}

void Serial_Ser1de::ParseFromStringDebug(const std::string& data, google::protobuf::Message* message) {
    uint32_t deserComprOutputSizeLocal = 0;
    uint32_t actual_decompressed_size = 0;
    std::vector<uint64_t> current_latencies(5, 0);

    auto start = std::chrono::high_resolution_clock::now();
    read_header(data, deser_gather_out_size, deserComprOutputSizeLocal, deser_sizes_for_scatter, deser_compressed);
    auto end = std::chrono::high_resolution_clock::now();
    current_latencies[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    deser_gather_out_sizes_history.push_back(deser_gather_out_size);

    start = std::chrono::high_resolution_clock::now();
    iaa_comp->decompress_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSizeLocal, deser_decompress_out.data(), deser_gather_out_size, &actual_decompressed_size);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[2] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[4] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    for (size_t i = 0; i < 5; i++) {
        parse_latencies[i].push_back(current_latencies[i]);
    }

    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

#endif
