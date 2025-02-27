#include <iostream>
#include <string>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include"test.pb.h"
#include"scatter_gather/scatter_gather.h"
#include"iaa_comp.h"

#define BUFFER_SIZE 4*4096

tachidromos::tests::SingularMessage InitializeSingularMessage() {
    tachidromos::tests::SingularMessage singular_message;
    singular_message.set_int_field(42);
    singular_message.set_long_field(1000000);
    singular_message.set_uint_field(123);
    singular_message.set_ulong_field(456789);
    singular_message.set_sint_field(-42);
    singular_message.set_slong_field(-1000000);
    singular_message.set_fixed_field(789);
    singular_message.set_lfixed_field(987654);
    singular_message.set_sfixed_field(-789);
    singular_message.set_slfixed_field(-987654);
    singular_message.set_float_field(3.14f);
    singular_message.set_double_field(2.71828);
    singular_message.set_bool_field(true);
    singular_message.set_string_field("Hello, World!");
    singular_message.set_bytes_field("BytesField");
    //singular_message.set_status(tachidromos::tests::SingularMessage::ACTIVE);
    auto* nested_message = singular_message.mutable_nested_message();
    nested_message->set_nested_string("Nested Hello");
    //nested_message->set_nested_int(456);
    auto* nested_message2 = singular_message.mutable_nested_message2();
    nested_message2->set_nested_string1("Nested Hello2");
    nested_message2->set_nested_string2("Nested Hello3");
    //nested_message2->set_nested_int(456);
    return singular_message;
}

tachidromos::tests::RepeatedMessage InitializeRepeatedMessage() {
    tachidromos::tests::RepeatedMessage repeated_message;
    // Initialize int_fields
    for (int i = 0; i < 10; ++i) {
        repeated_message.add_int_fields(i);
    }

    // Initialize long_fields
    for (int64_t i = 0; i < 10; ++i) {
        repeated_message.add_long_fields(i * 1000);
    }
    // Initialize uint_fields
    for (uint32_t i = 0; i < 10; ++i) {
        repeated_message.add_uint_fields(i * 10);
    }

    // Initialize ulong_fields
    for (uint64_t i = 0; i < 10; ++i) {
        repeated_message.add_ulong_fields(i * 10000);
    }

    // Initialize sint_fields
    for (int32_t i = 0; i < 10; ++i) {
        repeated_message.add_sint_fields(i * -1);
    }

    // Initialize slong_fields
    for (int64_t i = 0; i < 10; ++i) {
        repeated_message.add_slong_fields(i * -1000);
    }

    // Initialize fixed_fields
    for (uint32_t i = 0; i < 10; ++i) {
        repeated_message.add_fixed_fields(i * 100);
    }

    // Initialize lfixed_fields
    for (uint64_t i = 0; i < 10; ++i) {
        repeated_message.add_lfixed_fields(i * 100000);
    }

    // Initialize sfixed_fields
    for (int32_t i = 0; i < 10; ++i) {
        repeated_message.add_sfixed_fields(i * -100);
    }

    // Initialize slfixed_fields
    for (int64_t i = 0; i < 10; ++i) {
        repeated_message.add_slfixed_fields(i * -100000);
    }

    // Initialize float_fields
    for (int i = 0; i < 10; ++i) {
        repeated_message.add_float_fields(i * 1.1f);
    }

    // Initialize double_fields
    for (int i = 0; i < 10; ++i) {
        repeated_message.add_double_fields(i * 1.1);
    }

    // Initialize bool_fields
    for (int i = 0; i < 10; ++i) {
        repeated_message.add_bool_fields(i % 2 == 0);
    }

    // Initialize string_fields
    for (int i = 0; i < 10; ++i) {
        repeated_message.add_string_fields("string_" + std::to_string(i));
    }

    // Initialize bytes_fields
    for (int i = 0; i < 10; ++i) {
        repeated_message.add_bytes_fields("bytes_" + std::to_string(i));
    }

    // Initialize nested_messages
    for (int i = 0; i < 10; ++i) {
        auto* nested_message = repeated_message.add_nested_messages();
        nested_message->set_nested_string("nested_string_" + std::to_string(i));
        nested_message->set_nested_int(i);
    }

    return repeated_message;
}

tachidromos::tests::IntRepeatedMessage InitializeIntRepeatedMessage() {
    tachidromos::tests::IntRepeatedMessage int_repeated_message;
    for (int i = 1; i <= 10; ++i) {
        int_repeated_message.add_int_fields(i);
    }
    return int_repeated_message;
}

tachidromos::tests::StringRepeatedMessage InitializeStringRepeatedMessage() {
    tachidromos::tests::StringRepeatedMessage string_repeated_message;
    string_repeated_message.add_string_fields("Hello");
    string_repeated_message.add_string_fields("World");
    string_repeated_message.add_string_fields("This");
    string_repeated_message.add_string_fields("Is");
    string_repeated_message.add_string_fields("A");
    string_repeated_message.add_string_fields("Test");
    string_repeated_message.add_string_fields("Of");
    string_repeated_message.add_string_fields("String");
    string_repeated_message.add_string_fields("Repeated");
    string_repeated_message.add_string_fields("Message");
    return string_repeated_message;
}

tachidromos::tests::NestedRepeatedMessage InitializeNestedRepeatedMessage() {
    tachidromos::tests::NestedRepeatedMessage nested_repeated_message;
    for (int i = 1; i <= 10; ++i) {
        auto* nested_message = nested_repeated_message.add_nested_messages();
        nested_message->set_nested_string("Nested String " + std::to_string(i));
        nested_message->set_nested_int(i);
    }
    return nested_repeated_message;
}

// Utility function to print a tuple
void PrintTuple(const std::tuple<uint8_t*, size_t>& t) {
    std::cout << "Pointer: " << static_cast<void*>(std::get<0>(t)) << ", Size: " << std::get<1>(t) << std::endl;
}

template <typename T>
void PrintVector(const std::vector<T>& vec) {
    std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(std::cout, " "));
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // initialize iaa
    //IAAComp iaa(qpl_path_software);
    IAAComp iaa(qpl_path_hardware);

    if (argc > 1 && std::string(argv[1]) == "rr") {
        // Initialize RepeatedMessage
        tachidromos::tests::RepeatedMessage repeated_message = InitializeRepeatedMessage();

        // Gather ptrs
        std::vector<uint8_t*> gather_ptrs;
        // Sizes for scatter
        std::vector<size_t> scatter_sizes;
        repeated_message.generate_seperated_schema(gather_ptrs, scatter_sizes);

        // GATHER + COMPRESSION
        ScatterGather scagatherer(DML_PATH_SW, 10);
        size_t out_size = 0;
        std::vector<uint8_t> gather_out(BUFFER_SIZE, 0);
        // gather
        auto outcome = scagatherer.GatherWithMemCpy(gather_ptrs, scatter_sizes, gather_out.data(), &out_size);
        if (outcome) {
            std::cerr << "Failed to gather" << std::endl;
            return -1;
        }
        // compress
        uint8_t* compressed   = new uint8_t[BUFFER_SIZE];
        uint32_t comprOutputSize = 0;
        outcome = iaa.compress(gather_out.data(), out_size, compressed, BUFFER_SIZE, &comprOutputSize);
        if (outcome) {
            std::cerr << "Failed to compress" << std::endl;
            return -1;
        }

        //std::cout << "sizes " << gather_ptrs.size() << std::endl;
        //std::cout << "sizes2 " << scatter_sizes.size() << std::endl;

        //for (size_t i = 0; i < scatter_sizes.size(); i++) {
        //    std::cout << scatter_sizes[i] << ", " << i << std::endl;
        //}

        //std::cout << "sizes " << gather_ptrs.size() << std::endl;
        //std::cout << "sizes2 " << scatter_sizes.size() << std::endl;


        // DECOMPRESSION + SCATTER
        tachidromos::tests::RepeatedMessage out_message;
        //out_message.mutable_int_fields()->Resize(repeated_message.int_fields_size(), 0);
        //out_message.mutable_long_fields()->Resize(repeated_message.long_fields_size(), 0);
        //out_message.mutable_uint_fields()->Resize(repeated_message.uint_fields_size(), 0);
        //out_message.mutable_ulong_fields()->Resize(repeated_message.ulong_fields_size(), 0);
        //out_message.mutable_sint_fields()->Resize(repeated_message.sint_fields_size(), 0);
        //out_message.mutable_slong_fields()->Resize(repeated_message.slong_fields_size(), 0);
        //out_message.mutable_fixed_fields()->Resize(repeated_message.fixed_fields_size(), 0);
        //out_message.mutable_lfixed_fields()->Resize(repeated_message.lfixed_fields_size(), 0);
        //out_message.mutable_sfixed_fields()->Resize(repeated_message.sfixed_fields_size(), 0);
        //out_message.mutable_slfixed_fields()->Resize(repeated_message.slfixed_fields_size(), 0);
        //out_message.mutable_float_fields()->Resize(repeated_message.float_fields_size(), 0);
        //out_message.mutable_double_fields()->Resize(repeated_message.double_fields_size(), 0);
        //out_message.mutable_bool_fields()->Resize(repeated_message.bool_fields_size(), 0);
        //for (int i = 0; i < repeated_message.string_fields_size(); i++) {
        //    out_message.add_string_fields("$$$$$$$#");
        //}
        //for (int i = 0; i < repeated_message.bytes_fields_size(); i++) {
        //    out_message.add_bytes_fields("$$$$$$#");
        //}
        //for (int i = 0; i < repeated_message.nested_messages_size(); i++) {
        //    auto* nested_message = out_message.add_nested_messages();
        //    nested_message->set_nested_string("$$$$$$$$$$$$$$#");
        //}

        out_message.allocate_from_sizes(scatter_sizes);

        std::vector<uint8_t*> scatter_ptrs;
        out_message.generate_scatter_ptrs(scatter_ptrs);

        uint8_t* decompressed   = new uint8_t[BUFFER_SIZE];
        uint32_t decomprOutputSize = 0;
        outcome = iaa.decompress(compressed, comprOutputSize, decompressed, BUFFER_SIZE, &decomprOutputSize);
        //outcome = decompress_with_IAA(compressed, comprOutputSize, decompressed, BUFFER_SIZE, &decomprOutputSize);
        if (outcome) {
            std::cerr << "Failed to decompress" << std::endl;
            return -1;
        }
        outcome = scagatherer.ScatterWithMemCpy(decompressed, scatter_ptrs, scatter_sizes);
        if (outcome) {
            std::cerr << "Failed to scatter" << std::endl;
            return -1;
        }

        //std::cout << int_repeated_message.DebugString() << std::endl;
        //std::cout << out_message.DebugString() << std::endl;
        google::protobuf::util::MessageDifferencer differencer;

        std::string differences;
        google::protobuf::util::DefaultFieldComparator comparator;

        differencer.ReportDifferencesToString(&differences);
        differencer.set_field_comparator(&comparator);

        bool all_correct = differencer.Compare(repeated_message, out_message);
        if (!all_correct) {
            std::cout << "Differences: " << differences << std::endl;
        }


        bool correct = google::protobuf::util::MessageDifferencer::Equivalent(repeated_message, out_message);
        std::cout << '\n' << "--------------------------------------------" << std::endl << std::endl;
        std::cout << "Messages are equivalent: " << (correct ? "true" : "false") << std::endl;
        std::cout << '\n' << "--------------------------------------------" << std::endl;
        //std::cout << "Original RepeatedMessage: " << std::endl << repeated_message.DebugString() << std::endl;
        //std::cout << '\n' << "--------------------------------------------" << std::endl;
        //std::cout << "Deserialized RepeatedMessage: "  << std::endl << out_message.DebugString() << std::endl;
        //std::cout << '\n' << "--------------------------------------------" << std::endl;

        /*
        // Serialize RepeatedMessage
        std::string repeated_serialized;
        repeated_message.SerializeToString(&repeated_serialized);

        // Deserialize RepeatedMessage
        tachidromos::tests::RepeatedMessage repeated_message_deserialized;
        repeated_message_deserialized.ParseFromString(repeated_serialized);

        // Print RepeatedMessage
        std::cout << "Deserialized RepeatedMessage: " << repeated_message_deserialized.DebugString() << std::endl;
        */
    }
    else {
        // Initialize SingularMessage
        tachidromos::tests::SingularMessage singular_message = InitializeSingularMessage();
        
        // Gather ptrs
        std::vector<uint8_t*> gather_ptrs;
        // Sizes for scatter
        std::vector<size_t> scatter_sizes;
        singular_message.generate_seperated_schema(gather_ptrs, scatter_sizes);

        //for (auto size : scatter_sizes) {
        //    std::cout << size << std::endl;
        //}
        //for (auto ptr : gather_ptrs) {
        //    std::cout << static_cast<void*>(ptr) << std::endl;
        //}

        // GATHER + COMPRESSION
        ScatterGather scagatherer(DML_PATH_SW, 10);
        size_t out_size = 0;
        std::vector<uint8_t> gather_out(BUFFER_SIZE, 0);
        // gather
        auto outcome = scagatherer.GatherWithMemCpy(gather_ptrs, scatter_sizes, gather_out.data(), &out_size);
        if (outcome) {
            std::cerr << "Failed to gather" << std::endl;
            return -1;
        }
        // compress
        uint8_t* compressed   = new uint8_t[BUFFER_SIZE];
        uint32_t comprOutputSize = 0;
        outcome = iaa.compress(gather_out.data(), out_size, compressed, BUFFER_SIZE, &comprOutputSize);
        //outcome = compress_with_IAA(gather_out.data(), out_size, compressed, BUFFER_SIZE, &comprOutputSize);
        if (outcome) {
            std::cerr << "Failed to compress" << std::endl;
            return -1;
        }

        // DECOMPRESSION + SCATTER
        tachidromos::tests::SingularMessage out_message;
        std::vector<uint8_t*> scatter_ptrs;
        //out_message.generate_scatter_ptrs_and_allocate_from_sizes(scatter_ptrs, scatter_sizes);
        out_message.allocate_from_sizes(scatter_sizes);
        out_message.generate_scatter_ptrs(scatter_ptrs);

        uint8_t* decompressed   = new uint8_t[BUFFER_SIZE];
        uint32_t decomprOutputSize = 0;
        outcome = iaa.decompress(compressed, comprOutputSize, decompressed, BUFFER_SIZE, &decomprOutputSize);
        //outcome = decompress_with_IAA(compressed, comprOutputSize, decompressed, BUFFER_SIZE, &decomprOutputSize);
        if (outcome) {
            std::cerr << "Failed to decompress" << std::endl;
            return -1;
        }
        outcome = scagatherer.ScatterWithMemCpy(decompressed, scatter_ptrs, scatter_sizes);
        if (outcome) {
            std::cerr << "Failed to scatter" << std::endl;
            return -1;
        }

        //for (auto size : scatter_sizes) {
        //    std::cout << size << std::endl;
        //}

        google::protobuf::util::MessageDifferencer differencer;

        std::string differences;
        google::protobuf::util::DefaultFieldComparator comparator;

        differencer.ReportDifferencesToString(&differences);
        differencer.set_field_comparator(&comparator);

        bool all_correct = differencer.Compare(singular_message, out_message);
        if (!all_correct) {
            std::cout << "Differences: " << differences << std::endl;
        }

        bool correct = google::protobuf::util::MessageDifferencer::Equivalent(singular_message, out_message);
        //std::cout << '\n' << "--------------------------------------------" << std::endl << std::endl;
        //std::cout << "Original SingularMessage: " << std::endl << singular_message.DebugString() << std::endl;
        std::cout << '\n' << "--------------------------------------------" << std::endl << std::endl;
        std::cout << "Messages are equivalent: " << (correct ? "true" : "false") << std::endl;
        std::cout << '\n' << "--------------------------------------------" << std::endl;
        //std::cout << "Deserialized SingularMessage: "  << std::endl << out_message.DebugString() << std::endl;
        //std::cout << '\n' << "--------------------------------------------" << std::endl;

        /*
        // Serialize SingularMessage
        std::string singular_serialized;
        singular_message.SerializeToString(&singular_serialized);

        // Deserialize SingularMessage
        tachidromos::tests::SingularMessage singular_message_deserialized;
        singular_message_deserialized.ParseFromString(singular_serialized);

        // Print SingularMessage
        std::cout << "Deserialized SingularMessage: " << singular_message_deserialized.DebugString() << std::endl;
        */
    }

    return 0;
}
