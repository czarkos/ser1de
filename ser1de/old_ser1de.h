#ifndef SER1DE_H
#define SER1DE_H

#include"old_iaa_comp.h"
#include"sw_only_scatter_gather.h"
#include <google/protobuf/message.h>
#include <string>
#include <vector>

class Ser1de {
public:
    Ser1de(std::string execution_path);
    Ser1de();
    ~Ser1de();
    void SerializeToString(google::protobuf::Message& message, std::string* output);
    void ParseFromString(const std::string& data, google::protobuf::Message* message);

    uint32_t get_ser_gather_out_size() { return ser_gather_out_size; }
    uint32_t get_serComprOutputSize() { return serComprOutputSize; }
private:
    // IAA
    IAAComp* iaa_comp;
    IAAComp* iaa_comp_sw;
    // DSA
    ScatterGather* scagatherer;
    // Utility variables
    size_t BUFFER_SIZE = 16*4096; // 32KB
    size_t SCHEMA_SIZE = 1024; // 1KB
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
    std::vector<uint8_t> ser_gather_buffer;             // buffer (gather output, compression input)
    std::unique_ptr<uint8_t[]> ser_compressed_out;      // buffer (compress output)
    std::vector<uint8_t*> ser_ptrs;                     // schema
    std::vector<size_t> ser_sizes;                      // schema
    // deser intermediate variables
    uint32_t deserComprOutputSize = 0;                  // variable
    size_t deser_gather_out_size = 0;                   // variable
    const uint8_t* deser_compressed;                    // buffer (zero copy, space for it is allocated in the header)
    std::vector<uint8_t> deser_decompress_out;          // buffer (decompression output, scatter input)
    std::vector<uint8_t*> deser_ptrs;                   // schema
    std::vector<size_t> deser_sizes_for_scatter;        // schema (zero, space for it is allocated in the header)

    inline void make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, size_t& num_sizes, std::vector<size_t>& sizes, std::unique_ptr<uint8_t[]>& compressed);
    inline void read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed);

};

Ser1de::Ser1de(std::string execution_path) {
    // Initialize IAA with the appropriate path
    if (execution_path == "Hardware") {
        iaa_comp = new IAAComp(qpl_path_hardware, 1, 16);
    } else if (execution_path == "Software") {
        iaa_comp = new IAAComp(qpl_path_software, 1, 16);
    } else {
        throw std::invalid_argument("Invalid execution path");
    }
    scagatherer = new ScatterGather();

    // pre-allocate the required intermediate buffers
    ser_compressed_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_gather_buffer.resize(BUFFER_SIZE, 0);
    deser_decompress_out.resize(BUFFER_SIZE, 0);
    ser_ptrs.reserve(SCHEMA_SIZE);
    ser_sizes.reserve(SCHEMA_SIZE);
    deser_ptrs.reserve(SCHEMA_SIZE);
}

// Default constructor initializes IAA with hardware path
Ser1de::Ser1de() {
    iaa_comp = new IAAComp(qpl_path_hardware, 1, 16);
    iaa_comp_sw = new IAAComp(qpl_path_software, 1, 16);
    //scagatherer = new ScatterGather(DML_PATH_HW, 350);
    scagatherer = new ScatterGather();

    // pre-allocate the required intermediate buffers
    ser_compressed_out = std::make_unique<uint8_t[]>(BUFFER_SIZE);
    ser_gather_buffer.resize(BUFFER_SIZE, 0);
    deser_decompress_out.resize(BUFFER_SIZE, 0);
    ser_ptrs.reserve(SCHEMA_SIZE);
    ser_sizes.reserve(SCHEMA_SIZE);
    deser_ptrs.reserve(SCHEMA_SIZE);
}

Ser1de::~Ser1de() {
    delete iaa_comp;
    delete scagatherer;
}

// Serialize the message to a string
void Ser1de::SerializeToString(google::protobuf::Message& message, std::string* output) {
//void Ser1de::SerializeToString(const google::protobuf::Message& message, std::string* output) {
        // <------------ GATHER SCHEMA ------>
        message.generate_seperated_schema(ser_ptrs, ser_sizes);
        // <------------ GATHER ------>
        scagatherer->GatherWithMemCpy(ser_ptrs, ser_sizes, ser_gather_buffer.data(), &ser_gather_out_size);
        // <------------ COMPRESS ------>
        //iaa_comp->compress_non_blocking(ser_sizes);
        iaa_comp_sw->compress_blocking(ser_gather_buffer.data(), ser_gather_out_size, ser_compressed_out.get(), ser_gather_out_size+512, &serComprOutputSize);
        // <------------ MAKE HEADER ------>
        size_t num_sizes = ser_sizes.size();
        make_header(*output, ser_gather_out_size, serComprOutputSize, num_sizes, ser_sizes, ser_compressed_out);
        // Clear vectors for next iteration
        ser_ptrs.clear();
        ser_sizes.clear();
}

// Parse the string to a message
void Ser1de::ParseFromString(const std::string& data, google::protobuf::Message* message) {
    size_t queue_index, job_index;
    // <------------ READ HEADER ------>
    read_header(data, deser_gather_out_size, deserComprOutputSize, deser_sizes_for_scatter, deser_compressed);
    // <------------ DECOMPRESS ------>
    std::tie(queue_index, job_index) = iaa_comp->decompress_non_blocking(const_cast<uint8_t*>(deser_compressed), deserComprOutputSize, deser_decompress_out.data(), deser_gather_out_size);
    // <------------ ALLOCATE AND CREATE SCATTER SCHEMA ------>
    //message->allocate_from_sizes(deser_sizes_for_scatter);
    //message->generate_scatter_ptrs(deser_ptrs);
    message->generate_scatter_ptrs_and_allocate_from_sizes(deser_ptrs, deser_sizes_for_scatter);
    // <------------ SCATTER ------>
    // first make sure decompression is finished
    while (iaa_comp->poll_job(queue_index, job_index) != QPL_STS_OK){}
    assert(iaa_comp->poll_job(queue_index, job_index) == QPL_STS_OK);
    scagatherer->ScatterWithMemCpy(deser_decompress_out.data(), deser_ptrs, deser_sizes_for_scatter);

    // clear intermediate buffers for the next request
    deser_ptrs.clear();
    deser_sizes_for_scatter.clear();
}

inline void Ser1de::make_header(std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, size_t& num_sizes, std::vector<size_t>& sizes, std::unique_ptr<uint8_t[]>& compressed) {
    ser1de_ser_out.reserve(sizeof(out_size) + sizeof(comprOutputSize) + sizes.size() * sizeof(uint8_t*) + comprOutputSize * sizeof(uint8_t));
    // Append the size values
    ser1de_ser_out.append(reinterpret_cast<const char*>(&out_size), sizeof(out_size));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&comprOutputSize), sizeof(comprOutputSize));
    ser1de_ser_out.append(reinterpret_cast<const char*>(&num_sizes), sizeof(num_sizes));
    // Append the pointers vector
    ser1de_ser_out.append(reinterpret_cast<const char*>(sizes.data()), sizes.size() * sizeof(size_t));
    // Append the compressed data
    ser1de_ser_out.append(reinterpret_cast<const char*>(compressed.get()), comprOutputSize);
    //std::cout << "out_size: " << out_size << " comprOutputSize: " << comprOutputSize << " num_sizes: " << num_sizes << " sizes spece (bytes): " << sizes.size() * sizeof(size_t) << std::endl;
    //std::cout << "************************************************************************************************" << std::endl;
    //iaa_comp->compress_blocking(reinterpret_cast<uint8_t*>(sizes.data()), sizes.size() * sizeof(size_t), ser_compressed_out.get(), sizes.size() * sizeof(size_t)+512, &serComprOutputSize);
    //std::cout << "compressed sizes size (bytes): " << serComprOutputSize << std::endl;
    //for (auto size : sizes)
    //    std::cout << size << " ";
    //std::cout << std::endl;
}

inline void Ser1de::read_header(const std::string& ser1de_ser_out, size_t& out_size, uint32_t& comprOutputSize, std::vector<size_t>& sizes_for_scatter, const uint8_t*& compressed) {
    const char* data = ser1de_ser_out.data();
    size_t offset = 0;
    size_t num_sizes = 0;

    // Directly point to first sizeof(size_t) bytes for out_size
    out_size = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(out_size);
    // Directly point to second sizeof(size_t) bytes for out_size
    comprOutputSize = *reinterpret_cast<const uint32_t*>(data + offset);
    offset += sizeof(comprOutputSize);
    // Directly point to third sizeof(size_t) bytes for out_size
    num_sizes = *reinterpret_cast<const size_t*>(data + offset);
    offset += sizeof(num_sizes);

    // Recreate the vector of pointers
    //ptrs.resize(*num_sizes);
    const size_t* vec_data = reinterpret_cast<const size_t*>(data + offset);
    sizes_for_scatter = std::vector<size_t>(vec_data, vec_data + num_sizes);
    offset += num_sizes * sizeof(size_t);
    
    // Recreate the compressed data array
    compressed = reinterpret_cast<const uint8_t*>(data + offset);
    //std::cout << "************************************************************************************************" << std::endl;
    //std::cout << "out_size: " << out_size << " comprOutputSize: " << comprOutputSize << " num_sizes: " << num_sizes << std::endl;
    //std::cout << "************************************************************************************************" << std::endl;
    //for (auto size : sizes_for_scatter)
    //    std::cout << size << " ";
    //std::cout << std::endl;
}

#endif