#ifndef PROTO_INIT_H
#define PROTO_INIT_H

#include"person.pb.h"

void person_generated_setters(M& m, size_t index);

void string_generated_setters(M& m, std::string& dummy_str);

#endif // PROTO_INIT_H