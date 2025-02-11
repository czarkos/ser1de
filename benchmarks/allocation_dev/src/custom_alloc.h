#ifndef CUSTOM_ALLOC_H
#define CUSTOM_ALLOC_H

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <vector>


#include"person.pb.h"
#include"proto_init.h"

void allocate_with_sizes_hacky(M& m, std::vector<size_t>& sizes_for_scatter, std::string& dummy_str) {
    // allocate space for the strings
    //dummy_str = std::string("a", sizes_for_scatter[1]);
    // <------------ STRING SETTERS ------>
    string_generated_setters(m, dummy_str);
}

void allocate_with_sizes_hacky_2(M& m, std::vector<size_t>& sizes_for_scatter, std::string& dummy_str) {
  const google::protobuf::Descriptor* descriptor = m.GetDescriptor();
  const google::protobuf::Reflection* reflection = m.GetReflection();

  size_t size_index = 0;

  for (int i = 0; i < descriptor->field_count(); i++) {
    const google::protobuf::FieldDescriptor* field = descriptor->field(i);
  }
}

/*
void AllocateFieldsWithSizes(google::protobuf::Message* message, const std::vector<size_t>& sizes) {
  const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
  const google::protobuf::Reflection* reflection = message->GetReflection();
  
  size_t size_index = 0;
  
  for (int i = 0; i < descriptor->field_count(); i++) {
    const google::protobuf::FieldDescriptor* field = descriptor->field(i);
    
    if (size_index >= sizes.size()) break;
    
    switch (field->cpp_type()) {
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING: {
        std::string* str = reflection->MutableString(message, field);
        str->reserve(sizes[size_index++]);
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE: {
        if (field->is_repeated()) {
          google::protobuf::RepeatedPtrField<google::protobuf::Message>* repeated_field =
              reflection->MutableRepeatedPtrField<google::protobuf::Message>(message, field);
          repeated_field->Reserve(sizes[size_index++]);
        } else {
          google::protobuf::Message* nested_message = reflection->MutableMessage(message, field);
          // Recursively allocate for nested message
          AllocateFieldsWithSizes(nested_message, std::vector<size_t>(sizes.begin() + size_index, sizes.end()));
          size_index += nested_message->GetDescriptor()->field_count();
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
        if (field->is_repeated()) {
          google::protobuf::RepeatedField<int>* repeated_field =
              reflection->MutableRepeatedField<int>(message, field);
          repeated_field->Reserve(sizes[size_index++]);
        }
        break;
    }
  }
}
*/

#endif
