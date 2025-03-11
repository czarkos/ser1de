#ifndef SER1DE_H
#define SER1DE_H

#include"iaa_comp.h"
#include"sw_only_scatter_gather.h"
#include <google/protobuf/message.h>
#include <string>
#include <vector>
#include <chrono> // For timing measurements
#include <fstream> // For file output
#include <iomanip> // For formatting

class Ser1de {
public:
    Ser1de(std::string execution_path);
    Ser1de();
    ~Ser1de();
    // size_t SerializeToString(std::string* output);
    void SerializeToString(google::protobuf::Message& message, std::string* output);
    void ParseFromString(const std::string& data, google::protobuf::Message* message);
    
    // Debug versions that measure latency
    void SerializeToStringDebug(google::protobuf::Message& message, std::string* output);
    void ParseFromStringDebug(const std::string& data, google::protobuf::Message* message);
    
    // Export latency data to CSV files
    void exportLatenciesToCSV(const std::string& serializeFilename, const std::string& parseFilename) const;
    
    // Getters for latency measurements
    const std::vector<std::vector<uint64_t>>& getSerializeLatencies() const { return ser_latencies; }
    const std::vector<std::vector<uint64_t>>& getParseLatencies() const { return parse_latencies; }

    uint32_t get_ser_gather_out_size() { return ser_gather_out_size; }
    uint32_t get_serComprOutputSize() { return serComprOutputSize; }
private:
    // IAA
    IAAComp* iaa_comp;
    // DSA
    ScatterGather* scagatherer;
    // Utility variables
    size_t BUFFER_SIZE = 256*4096; // 16KB
    size_t SCHEMA_SIZE = 256*1024; // 1KB
    /*
    4 intermediate buffers : 2 for serialization, 2 for deserialization
    4 schema buffers: 2 for serialization, 2 for deserialization

    In Serialization:
        4 pre-allocated (gather output, compression output, gather ptrs, gather sizes)
    
    In Deserialization:
        1 zero-copy buffer (compressed data)
        1 zero-copy schema buffer (sizes for scatter)
        2 pre-allocated (decompression output, scatter ptrs)
    */
    // ser intermediate variables
    size_t ser_gather_out_size;                         // variable
    uint32_t serComprOutputSize;                        // variable
    uint32_t serComprSizesSize;                         // variable
    std::vector<uint8_t> ser_gather_buffer;             // buffer (gather output, compression input)
    std::unique_ptr<uint8_t[]> ser_compressed_out;      // buffer (compress output)
    std::unique_ptr<uint8_t[]> ser_compressed_sizes_out;      // buffer (compress output)
    std::vector<uint8_t*> ser_ptrs;                     // schema
    std::vector<size_t> ser_sizes;                      // schema
    // deser intermediate variables
    uint32_t deserComprOutputSize = 0;                  // variable
    size_t deser_gather_out_size = 0;                   // variable
    const uint8_t* deser_compressed;                    // buffer (zero copy, space for it is allocated in the header)
    std::vector<uint8_t> deser_decompress_out;          // buffer (decompression output, scatter input)
    std::vector<uint8_t*> deser_ptrs;                   // schema
    std::vector<size_t> deser_sizes_for_scatter;        // schema (zero, space for it is allocated in the header)
    
    // Latency measurement arrays (in nanoseconds)
    // Each vector corresponds to a specific operation and contains all measurements
    std::vector<std::vector<uint64_t>> ser_latencies;  // Latencies for SerializeToStringDebug
    std::vector<std::vector<uint64_t>> parse_latencies; // Latencies for ParseFromStringDebug

    std::vector<size_t> ser_gather_out_sizes_history;
    std::vector<size_t> deser_gather_out_sizes_history;

    inline void make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, uint32_t& comprSizesSize, size_t& num_sizes, std::unique_ptr<uint8_t[]>& compressed_sizes, std::unique_ptr<uint8_t[]>& compressed_data);
    //inline void make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, size_t& num_sizes, std::vector<size_t>& sizes, std::unique_ptr<uint8_t[]>& compressed);
    inline void read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed);
};

Ser1de::Ser1de(std::string execution_path) {
    // Initialize IAA with the appropriate path
    if (execution_path == "Hardware") {
        iaa_comp = new IAAComp(qpl_path_hardware, 1, 2);
    } else if (execution_path == "Software") {
        iaa_comp = new IAAComp(qpl_path_software, 1, 2);
    } else {
        throw std::invalid_argument("Invalid execution path");
    }
    scagatherer = new ScatterGather();

    // pre-allocate the required intermediate buffers
    ser_compressed_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_compressed_sizes_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_gather_buffer.resize(BUFFER_SIZE, 0);
    deser_decompress_out.resize(BUFFER_SIZE, 0);
    ser_ptrs.reserve(SCHEMA_SIZE);
    ser_sizes.reserve(SCHEMA_SIZE);
    deser_ptrs.reserve(SCHEMA_SIZE);
    deser_sizes_for_scatter.reserve(SCHEMA_SIZE);
    
    // Initialize latency measurement arrays with 5 empty vectors each
    ser_latencies.resize(5);
    parse_latencies.resize(5);
}

// Default constructor initializes IAA with hardware path
Ser1de::Ser1de() {
    iaa_comp = new IAAComp(qpl_path_hardware, 1, 2);
    scagatherer = new ScatterGather();

    // pre-allocate the required intermediate buffers
    ser_compressed_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_compressed_sizes_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_gather_buffer.resize(BUFFER_SIZE, 0);
    deser_decompress_out.resize(BUFFER_SIZE, 0);
    ser_ptrs.reserve(SCHEMA_SIZE);
    ser_sizes.reserve(SCHEMA_SIZE);
    deser_ptrs.reserve(SCHEMA_SIZE);
    deser_sizes_for_scatter.reserve(SCHEMA_SIZE);
    
    // Initialize latency measurement arrays with 5 empty vectors each
    ser_latencies.resize(5);
    parse_latencies.resize(5);
}

Ser1de::~Ser1de() {
    delete iaa_comp;
    delete scagatherer;
}

// Serialize the message to a string
void Ser1de::SerializeToString(google::protobuf::Message& message, std::string* output) {
    size_t job_id1;
    size_t job_id2;
    // <------------ GATHER SCHEMA ------>
    message.generate_seperated_schema(ser_ptrs, ser_sizes);
    // <------------ GATHER ------>
    job_id1 = iaa_comp->compress_non_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t));
    scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
    // <------------ COMPRESS ------>
    //iaa_comp->compress_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t), &serComprSizesSize);
    //iaa_comp->compress_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size+512, &serComprOutputSize);
    job_id2 = iaa_comp->compress_non_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size+512);
    while (iaa_comp->poll_job(job_id1) != QPL_STS_OK){}
    //assert(iaa_comp->poll_job(job_id1) == QPL_STS_OK);
    while (iaa_comp->poll_job(job_id2) != QPL_STS_OK){}
    //assert(iaa_comp->poll_job(job_id2) == QPL_STS_OK);
    serComprSizesSize = iaa_comp->get_job_compr_size(job_id1);
    serComprOutputSize = iaa_comp->get_job_compr_size(job_id2);
    // <------------ MAKE HEADER ------>
    size_t num_sizes = ser_sizes.size();
    make_header(*output, ser_gather_out_size, serComprOutputSize, serComprSizesSize, num_sizes, ser_compressed_sizes_out, ser_compressed_out);
    // Clear vectors for next iteration
    ser_ptrs.clear();
    ser_sizes.clear();
}

// Parse the string to a message
void Ser1de::ParseFromString(const std::string& data, google::protobuf::Message* message) {
    size_t job_id;
    uint32_t deserComprOutputSize = 0;
    // <------------ READ HEADER ------>
    read_header(data, deser_gather_out_size, deserComprOutputSize, deser_sizes_for_scatter, deser_compressed);
    // <------------ DECOMPRESS ------>
    //job_id = iaa_comp->decompress_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSize, deser_decompress_out.data(), deser_gather_out_size, &deserComprOutputSize);
    job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSize, deser_decompress_out.data(), deser_gather_out_size);
    // <------------ ALLOCATE AND CREATE SCATTER SCHEMA ------>
    //message->allocate_from_sizes(deser_sizes_for_scatter);
    //message->generate_scatter_ptrs(deser_ptrs);
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    // <------------ SCATTER ------>
    // first make sure decompression is finished
    while (iaa_comp->poll_job(job_id) != QPL_STS_OK){}
    //assert(iaa_comp->poll_job(job_id) == QPL_STS_OK);
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);

    // clear intermediate buffers for the next request
    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

inline void Ser1de::make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, uint32_t& comprSizesSize, size_t& num_sizes, std::unique_ptr<uint8_t[]>& compressed_sizes, std::unique_ptr<uint8_t[]>& compressed_data) {
//inline void Ser1de::make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, size_t& num_sizes, std::vector<size_t>& sizes, std::unique_ptr<uint8_t[]>& compressed) {
    ser1de_ser_out.reserve(sizeof(out_size) + sizeof(comprOutputSize) + comprSizesSize + comprOutputSize * sizeof(uint8_t));
    // Append the size values
    ser1de_ser_out.append(reinterpret_cast<const char*>(&out_size), sizeof(out_size));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&comprOutputSize), sizeof(comprOutputSize));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&comprSizesSize), sizeof(comprSizesSize));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&num_sizes), sizeof(num_sizes));
    // Append the compress sizes data array
    ser1de_ser_out.append(reinterpret_cast<const char*>(compressed_sizes.get()), comprSizesSize);
    // Append the compressed data
    ser1de_ser_out.append(reinterpret_cast<const char*>(compressed_data.get()), comprOutputSize);
    //std::cout << "************************************************************************************************" << std::endl;
    //std::cout << "out_size: " << out_size << " comprOutputSize: " << comprOutputSize << " num_sizes: " << num_sizes << " comprSizesSize (bytes): " << comprSizesSize << std::endl;
    //std::cout << "************************************************************************************************" << std::endl;
    //iaa_comp->compress_blocking(reinterpret_cast<uint8_t*>(sizes.data()), sizes.size() * sizeof(size_t), ser_compressed_out.get(), sizes.size() * sizeof(size_t)+512, &serComprOutputSize);
    //std::cout << "compressed sizes size (bytes): " << serComprOutputSize << std::endl;
    //for (auto size : sizes)
    //    std::cout << size << " ";
    //std::cout << std::endl;
}

inline void Ser1de::read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed) {
    size_t job_id;
    const char* data = ser1de_ser_out.data();
    size_t offset = 0;
    size_t num_sizes = 0;
    uint32_t comprSizesSize = 0;
    uint32_t decomprSizesSize = 0;

    // Directly point to the first variable
    out_size = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(out_size);
    // Directly point to the second variable
    comprOutputSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprOutputSize);
    // Directly point to the third variable
    comprSizesSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprSizesSize);
    // Directly point to the fourth variable
    num_sizes = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(num_sizes);

    // Recreate the vector of pointers
    //ptrs.resize(*num_sizes);
    const uint8_t* compressed_sizes_data = reinterpret_cast<const uint8_t*>(data + offset);
    //std::tie(queue_index, job_index) = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(compressed_sizes_data), comprSizesSize, reinterpret_cast<uint8_t*>(sizes_for_scatter.data()), num_sizes * sizeof(size_t));
    //job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(compressed_sizes_data), comprSizesSize, reinterpret_cast<uint8_t*>(sizes_for_scatter.data()), num_sizes * sizeof(size_t), &decomprSizesSize);
    sizes_for_scatter.resize(num_sizes);
    job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(compressed_sizes_data), comprSizesSize, reinterpret_cast<uint8_t*>(sizes_for_scatter.data()), num_sizes * sizeof(size_t));
    offset += comprSizesSize;
    
    // Recreate the compressed data array
    compressed = reinterpret_cast<const uint8_t*>(data + offset);

    // wait for the sizes list to actually be decompressed before returning
    while (iaa_comp->poll_job(job_id) != QPL_STS_OK){}
    assert(iaa_comp->poll_job(job_id) == QPL_STS_OK);
    //for (auto size : sizes_for_scatter)
    //    std::cout << size << ", ";
    //std::cout << std::endl;
    //std::cout << "************************************************************************************************" << std::endl;
    //std::cout << "out_size: " << out_size << " comprOutputSize: " << comprOutputSize << " num_sizes: " << num_sizes << " comprSizesSize (byte): " << comprSizesSize << std::endl;
    //std::cout << "************************************************************************************************" << std::endl;
    //for (auto size : sizes_for_scatter)
    //    std::cout << size << " ";
    //std::cout << std::endl;
}

// Export latency data to CSV files
void Ser1de::exportLatenciesToCSV(const std::string& serializeFilename, const std::string& parseFilename) const {
    // Export serialization latencies
    if (!ser_latencies.empty() && !ser_latencies[0].empty()) {
        std::ofstream serFile(serializeFilename);
        if (serFile.is_open()) {
            // Write CSV header - removed Iteration column
            serFile << "MessageSize,SchemaGeneration,GatherAndFirstCompression,SecondCompression,WaitForCompressions,MakeHeader,Total\n";
            
            // Write data rows
            size_t numIterations = ser_latencies[0].size();
            
            // Verify that all arrays have the same size
            if (ser_gather_out_sizes_history.size() != numIterations) {
                std::cerr << "Warning: Mismatch between latency array size (" << numIterations 
                          << ") and size history array size (" << ser_gather_out_sizes_history.size() 
                          << ") for serialization" << std::endl;
            }
            
            for (size_t i = 0; i < numIterations; i++) {
                // Removed iteration number column
                
                // Add message size (with bounds check)
                if (i < ser_gather_out_sizes_history.size()) {
                    serFile << ser_gather_out_sizes_history[i] << ",";
                } else {
                    serFile << "0,";
                }
                
                uint64_t total = 0;
                for (size_t j = 0; j < ser_latencies.size(); j++) {
                    // Ensure all latency arrays have the same size
                    if (ser_latencies[j].size() != numIterations) {
                        std::cerr << "Warning: Mismatch in serialization latency array sizes" << std::endl;
                    }
                    
                    // Add latency value (with bounds check)
                    if (i < ser_latencies[j].size()) {
                        serFile << ser_latencies[j][i];
                        total += ser_latencies[j][i];
                    } else {
                        serFile << "0";
                    }
                    serFile << ",";
                }
                serFile << total << "\n";  // Total latency
            }
            serFile.close();
            std::cout << "Serialization latencies exported to " << serializeFilename << std::endl;
        } else {
            std::cerr << "Unable to open file " << serializeFilename << " for writing" << std::endl;
        }
    } else {
        std::cout << "No serialization latency data to export" << std::endl;
    }
    
    // Export parsing latencies
    if (!parse_latencies.empty() && !parse_latencies[0].empty()) {
        std::ofstream parseFile(parseFilename);
        if (parseFile.is_open()) {
            // Write CSV header - removed Iteration column
            parseFile << "MessageSize,ReadHeader,Decompress,AllocateAndCreateSchema,WaitForDecompression,Scatter,Total\n";
            
            // Write data rows
            size_t numIterations = parse_latencies[0].size();
            
            // Verify that all arrays have the same size
            if (deser_gather_out_sizes_history.size() != numIterations) {
                std::cerr << "Warning: Mismatch between latency array size (" << numIterations 
                          << ") and size history array size (" << deser_gather_out_sizes_history.size() 
                          << ") for deserialization" << std::endl;
            }
            
            for (size_t i = 0; i < numIterations; i++) {
                // Removed iteration number column
                
                // Add message size (with bounds check)
                if (i < deser_gather_out_sizes_history.size()) {
                    parseFile << deser_gather_out_sizes_history[i] << ",";
                } else {
                    parseFile << "0,";
                }
                
                uint64_t total = 0;
                for (size_t j = 0; j < parse_latencies.size(); j++) {
                    // Ensure all latency arrays have the same size
                    if (parse_latencies[j].size() != numIterations) {
                        std::cerr << "Warning: Mismatch in deserialization latency array sizes" << std::endl;
                    }
                    
                    // Add latency value (with bounds check)
                    if (i < parse_latencies[j].size()) {
                        parseFile << parse_latencies[j][i];
                        total += parse_latencies[j][i];
                    } else {
                        parseFile << "0";
                    }
                    parseFile << ",";
                }
                parseFile << total << "\n";  // Total latency
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

// Debug version of SerializeToString that measures latency of each operation
// ser_latencies vectors will contain times (in nanoseconds) for:
// [0] - generate_seperated_schema
// [1] - first compression and GatherWithMemCpy
// [2] - second compression
// [3] - waiting for compressions to finish
// [4] - make_header
void Ser1de::SerializeToStringDebug(google::protobuf::Message& message, std::string* output) {
    size_t job_id1;
    size_t job_id2;
    std::vector<uint64_t> current_latencies(5, 0);
    
    // <------------ GATHER SCHEMA ------>
    auto start = std::chrono::high_resolution_clock::now();
    message.generate_seperated_schema(ser_ptrs, ser_sizes);
    job_id1 = iaa_comp->compress_non_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t));
    auto end = std::chrono::high_resolution_clock::now();
    current_latencies[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // <------------ GATHER & FIRST COMPRESSION ------>
    start = std::chrono::high_resolution_clock::now();
    //job_id1 = iaa_comp->compress_non_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t));
    scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    ser_gather_out_sizes_history.push_back(ser_gather_out_size);
    
    // <------------ SECOND COMPRESSION ------>
    start = std::chrono::high_resolution_clock::now();
    //job_id1 = iaa_comp->compress_blocking(reinterpret_cast<uint8_t*>(ser_sizes.data()), ser_sizes.size() * sizeof(size_t), ser_compressed_sizes_out.get(), ser_sizes.size() * sizeof(uint16_t), &serComprSizesSize);
    //job_id2 = iaa_comp->compress_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size+512, &serComprOutputSize);
    job_id2 = iaa_comp->compress_non_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size+512);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[2] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // <------------ WAITING FOR COMPRESSIONS ------>
    start = std::chrono::high_resolution_clock::now();
    while (iaa_comp->poll_job(job_id1) != QPL_STS_OK){}
    serComprSizesSize = iaa_comp->get_job_compr_size(job_id1);
    //assert(iaa_comp->poll_job(job_id1) == QPL_STS_OK);
    while (iaa_comp->poll_job(job_id2) != QPL_STS_OK){}
    serComprOutputSize = iaa_comp->get_job_compr_size(job_id2);
    //assert(iaa_comp->poll_job(job_id2) == QPL_STS_OK);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[3] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // <------------ MAKE HEADER ------>
    start = std::chrono::high_resolution_clock::now();
    size_t num_sizes = ser_sizes.size();
    make_header(*output, ser_gather_out_size, serComprOutputSize, serComprSizesSize, 
               num_sizes, ser_compressed_sizes_out, ser_compressed_out);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[4] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // Store the measurements in the history
    for (size_t i = 0; i < 5; i++) {
        ser_latencies[i].push_back(current_latencies[i]);
    }
    
    // Clear vectors for next iteration
    ser_ptrs.clear();
    ser_sizes.clear();
}

// Debug version of ParseFromString that measures latency of each operation
// parse_latencies vectors will contain times (in nanoseconds) for:
// [0] - read_header
// [1] - decompress_non_blocking
// [2] - allocate_from_sizes and generate_scatter_ptrs
// [3] - waiting for decompression to finish
// [4] - ScatterWithMemCpy
void Ser1de::ParseFromStringDebug(const std::string& data, google::protobuf::Message* message) {
    size_t job_id;
    uint32_t deserComprOutputSize = 0;
    std::vector<uint64_t> current_latencies(5, 0);
    
    // <------------ READ HEADER ------>
    auto start = std::chrono::high_resolution_clock::now();
    read_header(data, deser_gather_out_size, deserComprOutputSize, deser_sizes_for_scatter, deser_compressed);
    auto end = std::chrono::high_resolution_clock::now();
    current_latencies[0] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    deser_gather_out_sizes_history.push_back(deser_gather_out_size);
    // <------------ DECOMPRESS ------>
    start = std::chrono::high_resolution_clock::now();
    job_id = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(deser_compressed), 
                                              deserComprOutputSize, 
                                              deser_decompress_out.data(), 
                                              deser_gather_out_size);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[1] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // <------------ ALLOCATE AND CREATE SCATTER SCHEMA ------>
    start = std::chrono::high_resolution_clock::now();
    //message->allocate_from_sizes(deser_sizes_for_scatter);
    //message->generate_scatter_ptrs(deser_ptrs);
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[2] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // <------------ WAIT FOR DECOMPRESSION ------>
    start = std::chrono::high_resolution_clock::now();
    while (iaa_comp->poll_job(job_id) != QPL_STS_OK){}
    assert(iaa_comp->poll_job(job_id) == QPL_STS_OK);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[3] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // <------------ SCATTER ------>
    start = std::chrono::high_resolution_clock::now();
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);
    end = std::chrono::high_resolution_clock::now();
    current_latencies[4] = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // Store the measurements in the history
    for (size_t i = 0; i < 5; i++) {
        parse_latencies[i].push_back(current_latencies[i]);
    }

    // clear intermediate buffers for the next request
    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

#endif