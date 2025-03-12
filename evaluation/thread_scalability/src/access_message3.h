// Copyright 2023 The Fleetbench Authors
//
// Licensed under the Apache License, Version 2.0 (the "License" );
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an " AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// WARNING: DO NOT EDIT! This file is auto-generated.

#ifndef THIRD_PARTY_FLEETBENCH_PROTO_ACCESS_MESSAGE3_H_
#define THIRD_PARTY_FLEETBENCH_PROTO_ACCESS_MESSAGE3_H_

#include <cstdint>
#include <string>

#include "Message3.pb.h"

namespace fleetbench::proto {

inline void Message3_Set_1(Message3* message, std::string* s) {
  Message3::M1* v0 = message->mutable_f_10();
  Message3::M1::M12* v2_0 = v0->add_f_2();
  Message3::M1::M12::M27* v2 = v2_0->mutable_f_1();
  v2->set_f_5(Message3::M1::M12::M27::E15_CONST_3);
  v2->set_f_1(0xfb7c7deee5);
  v2->set_f_2(s->substr(0, 24));
  Message3::M1::M12* v2_1 = v0->add_f_2();
  Message3::M1::M12::M27* v3 = v2_1->mutable_f_1();
  v3->set_f_0(s->substr(0, 59));
  v3->set_f_3(0xb656733a);
  v3->set_f_2(s->substr(0, 5));
  v3->set_f_5(Message3::M1::M12::M27::E15_CONST_5);
  Message3::M5* v4 = message->mutable_f_15();
  v4->set_f_0(0x23db025c);
  message->set_f_1(s->substr(0, 118));
  Message3::M6* v5 = message->mutable_f_16();
  Message3::M6::M21* v7_0 = v5->add_f_3();
  v7_0->set_f_29(0x8f605708e92274bb);
  v7_0->set_f_6(s->substr(0, 29));
  v7_0->set_f_22(s->substr(0, 21));
  v7_0->set_f_5(s->substr(0, 47));
  v7_0->set_f_19(s->substr(0, 87));
  v7_0->set_f_37(s->substr(0, 13));
  v7_0->set_f_31(0x5b587b9f8d110f67);
  v7_0->set_f_9(s->substr(0, 100));
  v7_0->set_f_11(0xa1);
  v7_0->set_f_33(0xd0);
  v7_0->set_f_24(0x7d);
  v7_0->set_f_13(0.318158);
  v7_0->set_f_36(0x2d2270a16b71362e);
  v7_0->set_f_30(s->substr(0, 7));
  Message3::M8* v7 = message->mutable_f_19();
  Message3::M8::M17* v8 = v7->mutable_f_5();
  Message3::M8::M17::M35* v10_0 = v8->add_f_6();
  v10_0->set_f_1(0xec);
  Message3::M8::M17::M31* v11 = v8->mutable_f_5();
  v11->set_f_1(s->substr(0, 4));
  v11->set_f_0(s->substr(0, 15));
  Message3::M8::M17::M31::M48* v12 = v11->mutable_f_2();
  v12->set_f_0(Message3::M8::M17::M31::M48::E22_CONST_1);
  v7->set_f_1(Message3::M8::E5_CONST_4);
  v7->set_f_0(Message3::M8::E4_CONST_4);
  Message3::M8::M19* v14_0 = v7->add_f_6();
  Message3::M8::M19::M26* v14 = v14_0->mutable_f_3();
  v14->set_f_0(0.854128);
  Message3::M8::M19::M26::M44* v16 = v14->mutable_f_4();
  int32_t array_1[14] = {0xf396,   0xf6, 0x26,     0xf13c5c, 0x945a,
                         0xb8917e, 0x6d, 0x93,     0x624226, 0xf042,
                         0x65,     0xa9, 0x358768, 0x30};
  for (auto& value : array_1) v16->add_f_0(value);
  Message3::M8::M19* v14_1 = v7->add_f_6();
  Message3::M8::M19::M26* v19 = v14_1->mutable_f_3();
  Message3::M8::M19::M26::M47* v22_0 = v19->add_f_6();
  v22_0->set_f_0(false);
  Message3::M7* v25 = message->mutable_f_17();
  Message3::M7::M14* v26 = v25->mutable_f_1();
  Message3::M7::M14::M38* v27 = v26->mutable_f_4();
  Message3::M7::M14::M38::M41* v28 = v27->mutable_f_2();
  v28->set_f_0(s->substr(0, 3));
  Message3::M7::M14::M36* v29 = v26->mutable_f_1();
  v29->add_f_0(s->substr(0, 10));
  v29->add_f_0(s->substr(0, 5));
  Message3::M7::M14::M37* v30 = v26->mutable_f_2();
  v30->set_f_0(0x56fd0b8e979361f0);
  Message3::M3* v31 = message->mutable_f_12();
  Message3::M3::M13* v33_0 = v31->add_f_10();
  Message3::M3::M13::M34* v34_0 = v33_0->add_f_3();
  v34_0->set_f_0(0xcef2);
  v31->set_f_0(0x149645);
  v31->set_f_6(0x30);
  v31->set_f_4(s->substr(0, 2));
  Message3::M4* v35_0 = message->add_f_13();
  Message3::M4::M15* v35 = v35_0->mutable_f_2();
  v35->set_f_0(0xa28a33);
  Message3::M4::M15::M24* v38 = v35->mutable_f_3();
  Message3::M4::M15::M24::M42* v39 = v38->mutable_f_2();
  v39->set_f_9(0.969836);
  v39->set_f_7(0x96);
  v39->set_f_11(true);
  v39->set_f_12(s->substr(0, 6));
  v38->set_f_0(Message3::M4::M15::M24::E14_CONST_3);
  v35->set_f_1(0.727328);
  Message3::M4::M16* v40 = v35_0->mutable_f_3();
  Message3::M4::M16::M29* v41 = v40->mutable_f_3();
  v41->set_f_0(0x7a5e36b48379c444);
  Message3::M4::M16::M30* v42 = v40->mutable_f_4();
  v42->set_f_0(0.866093);
  Message3::M4::M16::M30::M43* v43 = v42->mutable_f_1();
  Message3::M4::M16::M30::M43::M50* v44 = v43->mutable_f_2();
  Message3::M4::M16::M30::M43::M50::M65* v46_0 = v44->add_f_5();
  Message3::M4::M16::M30::M43::M50::M65::M76* v46 = v46_0->mutable_f_1();
  v46->set_f_4(0x3fa8);
  v46->set_f_0(0.516702);
  Message3::M4::M16::M30::M43::M50::M64* v47 = v44->mutable_f_4();
  v47->set_f_1(0xfc0606);
  Message3::M4::M16::M30::M43::M50::M72* v48 = v44->mutable_f_8();
  v48->set_f_6(0x488eb03a);
  v48->set_f_4(0xff46385);
  v48->set_f_5(Message3::M4::M16::M30::M43::M50::M72::E29_CONST_2);
  Message3::M4::M16::M30::M43::M50::M60* v50 = v44->mutable_f_3();
  v50->set_f_0(0.746744);
  Message3::M4::M9* v54 = v35_0->mutable_f_1();
  v54->set_f_0(s->substr(0, 20));
  Message3::M2* v57 = message->mutable_f_11();
  Message3::M2::M10* v58 = v57->mutable_f_2();
  v58->set_f_0(Message3::M2::M10::E7_CONST_4);
  Message3::M2::M10::M40* v60_0 = v58->add_f_11();
  v60_0->set_f_2(s->substr(0, 5));
  Message3::M2::M10::M33* v61_0 = v58->add_f_10();
  Message3::M2::M10::M33::M45* v61 = v61_0->mutable_f_1();
  Message3::M2::M10::M33::M45::M51* v62 = v61->mutable_f_12();
  float array_2[6] = {0.952476, 0.680258, 0.413054,
                      0.710040, 0.970781, 0.148426};
  for (auto& value : array_2) v62->add_f_21(value);
  Message3::M2::M10::M33::M45::M51::M62* v64 = v62->mutable_f_47();
  v64->set_f_0(Message3::M2::M10::M33::M45::M51::M62::E27_CONST_2);
  Message3::M2::M10::M33::M45::M51::M62::M79* v66_0 = v64->add_f_2();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84* v68 = v66_0->mutable_f_4();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93* v69 =
      v68->mutable_f_1();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95* v71_0 =
      v69->add_f_2();
  v71_0->set_f_0(0x1e2e6da1);
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97* v71 =
      v71_0->mutable_f_2();
  v71->set_f_0(Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97::
                   E42_CONST_2);
  v68->set_f_0(s->substr(0, 4));
  v62->set_f_13(s->substr(0, 66));
  v62->set_f_20(0x9ca7abc4b4);
  v62->set_f_8(s->substr(0, 6));
  v62->set_f_16(s->substr(0, 31));
  Message3::M2::M10::M33::M45::M51::M69* v77_0 = v62->add_f_49();
  v77_0->set_f_0(0x56304ab5);
  v77_0->set_f_3(0x292165dfd9);
  v62->set_f_2(0x95);
  v62->set_f_1(0x605360);
  v62->set_f_14(s->substr(0, 7));
  v62->set_f_9(Message3::M2::M10::M33::M45::M51::E23_CONST_2);
  Message3::M2::M10::M33::M45::M51::M58* v80 = v62->mutable_f_45();
  Message3::M2::M10::M33::M45::M51::M58::M73* v82 = v80->mutable_f_4();
  v82->set_f_0(0.104675);
  v61->set_f_7(false);
  Message3::M2::M10::M33::M45::M49* v83 = v61->mutable_f_11();
  Message3::M2::M10::M33::M45::M49::M63* v85_0 = v83->add_f_6();
  Message3::M2::M10::M33::M45::M49::M63::M81* v85 = v85_0->mutable_f_2();
  v85->set_f_3(s->substr(0, 43));
  Message3::M2::M10::M33::M45::M49::M71* v86 = v83->mutable_f_9();
  Message3::M2::M10::M33::M45::M49::M71::M75* v87 = v86->mutable_f_1();
  v87->set_f_0(0x35);
  Message3::M2::M10::M33::M45::M49::M71::M75::M88* v88 = v87->mutable_f_6();
  v88->set_f_0(Message3::M2::M10::M33::M45::M49::M71::M75::M88::E34_CONST_1);
  Message3::M2::M10::M33::M45::M49::M71::M80* v89 = v86->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M86* v90 = v89->mutable_f_2();
  v90->set_f_0(Message3::M2::M10::M33::M45::M49::M71::M80::M86::E33_CONST_1);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85* v94_0 = v89->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94* v94 =
      v94_0->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96* v95 =
      v94->mutable_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M98* v97_0 =
      v95->add_f_50();
  v97_0->set_f_0(s->substr(0, 223));
  v95->set_f_18(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E38_CONST_3);
  v95->set_f_17(0.233604);
  v95->set_f_1(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E35_CONST_3);
  v95->set_f_15(0x24b393f4);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99* v97 =
      v95->mutable_f_52();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100* v99_0 =
      v97->add_f_7();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101*
      v100_0 = v99_0->add_f_5();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102* v101_0 = v100_0->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103* v101 = v101_0->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104* v102 = v101->mutable_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v104_0 = v102->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M107* v105 = v104_0->mutable_f_3();
  v105->set_f_0(0x6b);
  v105->set_f_3(s->substr(0, 2));
  v101_0->set_f_0(0x78);
  v95->set_f_21(0xf611);
  v95->set_f_9(0.758066);
  v95->set_f_32(s->substr(0, 18));
  v94->set_f_0(0x7b);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85* v94_1 = v89->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94* v107 =
      v94_1->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96* v108 =
      v107->mutable_f_1();
  v108->set_f_27(0xe3e8);
  v108->set_f_26(0x11);
  v108->set_f_35(0x18b9411f);
  v108->set_f_29(0x80);
  v108->set_f_14(s->substr(0, 257));
  v108->set_f_11(s->substr(0, 14));
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99* v109 =
      v108->mutable_f_52();
  v109->set_f_3(0.546825);
  v109->set_f_2(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                    E43_CONST_1);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100* v111_0 =
      v109->add_f_7();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101*
      v112_0 = v111_0->add_f_5();
  v112_0->set_f_0(0xbf917d1);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102* v113_0 = v112_0->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103* v113 = v113_0->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104* v114 = v113->mutable_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v116_0 = v114->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M107* v116 = v116_0->mutable_f_3();
  v116->set_f_2(false);
  v114->set_f_3(s->substr(0, 125));
  v113->set_f_0(true);
  v113_0->set_f_0(0x80);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102* v113_1 = v112_0->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103* v118 = v113_1->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104* v119 = v118->mutable_f_1();
  v119->set_f_3(s->substr(0, 22));
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v121_0 = v119->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M106* v121 = v121_0->mutable_f_2();
  v121->set_f_0(0x64ba1f);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M107* v122 = v121_0->mutable_f_3();
  v122->set_f_3(s->substr(0, 3));
  v122->set_f_0(0x92);
  v122->set_f_2(false);
  v121_0->set_f_0(0xa3);
  v119->set_f_1(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                    M100::M101::M102::M103::M104::E44_CONST_1);
  v119->set_f_2(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                    M100::M101::M102::M103::M104::E45_CONST_1);
  v113_1->set_f_0(0x2e);
  v109->set_f_0(s->substr(0, 6));
  v108->set_f_25(0x22);
  v108->set_f_6(0.772383);
  v108->set_f_17(0.749123);
  v108->set_f_34(0x69edc71116c90359);
  v108->set_f_32(s->substr(0, 30));
  v108->set_f_18(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E38_CONST_1);
  v108->set_f_22(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E40_CONST_1);
  v108->set_f_19(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E39_CONST_3);
  v108->set_f_3(0.160461);
  v108->set_f_21(0xcb);
  v108->set_f_7(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E36_CONST_4);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M90* v125_0 =
      v94_1->add_f_1();
  v125_0->set_f_0(0xaeb6);
  Message3::M2::M10::M33::M45::M49::M71::M80::M87* v125 = v89->mutable_f_3();
  v125->set_f_0(s->substr(0, 9));
  Message3::M2::M10::M33::M45::M49::M56* v126 = v83->mutable_f_4();
  v126->set_f_0(0x716acc6c);
  Message3::M2::M10::M33::M45::M49::M67* v128 = v83->mutable_f_8();
  Message3::M2::M10::M33::M45::M49::M67::M78* v129 = v128->mutable_f_1();
  v129->set_f_0(0x266df5bc);
  Message3::M2::M10::M33::M45::M49::M67::M78::M82* v130 = v129->mutable_f_3();
  v130->set_f_2(Message3::M2::M10::M33::M45::M49::M67::M78::M82::E31_CONST_2);
  v130->set_f_0(false);
  v130->set_f_1(Message3::M2::M10::M33::M45::M49::M67::M78::M82::E30_CONST_2);
  v61->set_f_6(0xcac7c1618c445466);
  v61->set_f_2(0x75a194075e86ac87);
  v61_0->set_f_0(Message3::M2::M10::M33::E18_CONST_5);
  v57->set_f_0(Message3::M2::E2_CONST_4);
  Message3::M2::M18* v131 = v57->mutable_f_3();
  v131->set_f_0(0xc5);
}

inline void Message3_Set_2(Message3* message, std::string* s) {
  message->set_f_1(s->substr(0, 1));
  Message3::M2* v0 = message->mutable_f_11();
  Message3::M2::M10* v1 = v0->mutable_f_2();
  Message3::M2::M10::M33* v4_0 = v1->add_f_10();
  Message3::M2::M10::M33::M45* v4 = v4_0->mutable_f_1();
  v4->set_f_0(false);
  Message3::M2::M10::M33::M45::M51* v5 = v4->mutable_f_12();
  Message3::M2::M10::M33::M45::M51::M57* v6 = v5->mutable_f_44();
  v6->set_f_0(0x4afd);
  Message3::M2::M10::M33::M45::M51::M58* v7 = v5->mutable_f_45();
  v7->set_f_0(0x2ccf);
  Message3::M2::M10::M33::M45::M51::M58::M73* v8 = v7->mutable_f_4();
  v8->set_f_0(0.850707);
  Message3::M2::M10::M33::M45::M51::M53* v10 = v5->mutable_f_41();
  v10->set_f_0(0xc586);
  v5->set_f_10(0x31);
  v5->set_f_4(0x4babcb);
  v5->set_f_7(s->substr(0, 57));
  Message3::M2::M10::M33::M45::M51::M69* v12_0 = v5->add_f_49();
  v12_0->set_f_1(Message3::M2::M10::M33::M45::M51::M69::E28_CONST_3);
  v12_0->set_f_0(0xcb);
  v12_0->set_f_2(0x6e);
  Message3::M2::M10::M33::M45::M51::M69* v12_1 = v5->add_f_49();
  v12_1->set_f_0(0xa7);
  v12_1->set_f_2(0xb6);
  Message3::M2::M10::M33::M45::M51::M62* v12 = v5->mutable_f_47();
  Message3::M2::M10::M33::M45::M51::M62::M79* v14_0 = v12->add_f_2();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84* v16 = v14_0->mutable_f_4();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93* v17 =
      v16->mutable_f_1();
  v17->set_f_0(0x727b);
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95* v19_0 =
      v17->add_f_2();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97* v19 =
      v19_0->mutable_f_2();
  v19->set_f_0(Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97::
                   E42_CONST_5);
  v5->set_f_0(0x1766230);
  v5->set_f_8(s->substr(0, 7));
  Message3::M2::M10::M33::M45::M51::M61* v20 = v5->mutable_f_46();
  v20->set_f_0(0x65955c310b474dd8);
  v5->set_f_16(s->substr(0, 14));
  v5->set_f_2(0x6e);
  v5->set_f_19(0x37);
  Message3::M2::M10::M33::M45::M51::M52* v22_0 = v5->add_f_39();
  Message3::M2::M10::M33::M45::M51::M52::M77* v22 = v22_0->mutable_f_2();
  v22->set_f_0(0x4fad8e5d586e85eb);
  v4->set_f_5(0x3e);
  v4->set_f_1(0x4b);
  v4->set_f_7(false);
  Message3::M2::M10::M33::M45::M49* v24 = v4->mutable_f_11();
  Message3::M2::M10::M33::M45::M49::M63* v31_0 = v24->add_f_6();
  Message3::M2::M10::M33::M45::M49::M63::M81* v31 = v31_0->mutable_f_2();
  v31->set_f_3(s->substr(0, 2));
  v31->set_f_0(0.430693);
  Message3::M2::M10::M33::M45::M49::M71* v32 = v24->mutable_f_9();
  Message3::M2::M10::M33::M45::M49::M71::M75* v33 = v32->mutable_f_1();
  v33->set_f_1(0x51a38c04b879c989);
  v33->set_f_0(0xc9);
  Message3::M2::M10::M33::M45::M49::M71::M80* v35 = v32->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85* v39_0 = v35->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94* v39 =
      v39_0->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96* v40 =
      v39->mutable_f_1();
  v40->set_f_13(s->substr(0, 31));
  v40->set_f_14(s->substr(0, 17));
  v40->set_f_0(true);
  v40->set_f_8(0x6b7ec523819aac3f);
  v40->set_f_33(0.828816);
  v40->set_f_28(0xe4);
  v40->set_f_9(0.661055);
  v40->set_f_25(0xc5);
  v40->set_f_12(true);
  v40->set_f_26(0x2b);
  v40->set_f_30(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E41_CONST_4);
  v40->set_f_32(s->substr(0, 1));
  v40->set_f_16(0x27693ef4efba2be);
  v40->set_f_17(0.633929);
  v40->set_f_10(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E37_CONST_5);
  v40->set_f_20(0x3303b7d79b9a64);
  v40->set_f_11(s->substr(0, 14));
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99* v42 =
      v40->mutable_f_52();
  v42->set_f_3(0.858988);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100* v44_0 =
      v42->add_f_7();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101*
      v45_0 = v44_0->add_f_5();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102* v46_0 = v45_0->add_f_1();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103* v46 = v46_0->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104* v47 = v46->mutable_f_1();
  v47->set_f_2(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                   M100::M101::M102::M103::M104::E45_CONST_2);
  v47->set_f_0(0x9658e98e68);
  v47->set_f_3(s->substr(0, 7));
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v49_0 = v47->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M107* v49 = v49_0->mutable_f_3();
  v49->set_f_3(s->substr(0, 8));
  v49_0->set_f_1(s->substr(0, 2));
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v49_1 = v47->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M107* v52 = v49_1->mutable_f_3();
  v52->set_f_1(0xd1ca4b96e55675);
  v47->set_f_1(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                   M100::M101::M102::M103::M104::E44_CONST_3);
  v44_0->set_f_1(0xb3);
  v42->set_f_2(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                   E43_CONST_4);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M90* v54_0 =
      v39_0->add_f_1();
  v54_0->set_f_0(0xea41);
  v1->set_f_3(0xdb);
  v1->set_f_1(0.631550);
  Message3::M2::M18* v56 = v0->mutable_f_3();
  v56->set_f_0(0xb077);
  v0->set_f_0(Message3::M2::E2_CONST_2);
  Message3::M7* v58 = message->mutable_f_17();
  Message3::M7::M14* v59 = v58->mutable_f_1();
  Message3::M7::M14::M37* v61 = v59->mutable_f_2();
  v61->set_f_0(0x14);
  Message3::M7::M14::M38* v62 = v59->mutable_f_4();
  v62->set_f_0(true);
  Message3::M8* v64 = message->mutable_f_19();
  Message3::M8::M11* v65 = v64->mutable_f_4();
  v65->set_f_0(0x5db7d63);
  Message3::M8::M17* v72 = v64->mutable_f_5();
  Message3::M8::M17::M31* v74 = v72->mutable_f_5();
  Message3::M8::M17::M31::M48* v75 = v74->mutable_f_2();
  v75->set_f_0(Message3::M8::M17::M31::M48::E22_CONST_1);
  Message3::M8::M17::M35* v77_0 = v72->add_f_6();
  v77_0->set_f_2(0.962834);
  v77_0->set_f_1(0x7c);
  v64->set_f_0(Message3::M8::E4_CONST_2);
  Message3::M6* v77 = message->mutable_f_16();
  Message3::M6::M21* v79_0 = v77->add_f_3();
  v79_0->set_f_35(Message3::M6::M21::E13_CONST_4);
  v79_0->set_f_6(s->substr(0, 17));
  v79_0->set_f_19(s->substr(0, 8));
  v79_0->set_f_41(0x721847865f8fba52);
  v79_0->set_f_20(0x6cae9206);
  v79_0->set_f_30(s->substr(0, 29));
  v79_0->set_f_42(0x42);
  v79_0->set_f_32(s->substr(0, 431));
  v79_0->set_f_39(0xda770f);
  v79_0->set_f_38(0.376638);
  Message3::M4* v80_0 = message->add_f_13();
  Message3::M4::M15* v80 = v80_0->mutable_f_2();
  Message3::M4::M15::M24* v81 = v80->mutable_f_3();
  Message3::M4::M15::M24::M42* v82 = v81->mutable_f_2();
  v82->set_f_0(0xdb);
  v82->set_f_7(0x4a);
  v82->set_f_8(0.097055);
  v82->set_f_10(s->substr(0, 4));
  v82->set_f_1(0xcc614f6ed857b3fe);
  v82->set_f_12(s->substr(0, 73));
  v82->set_f_13(Message3::M4::M15::M24::M42::E21_CONST_2);
  v82->set_f_5(0x162c454488511a1f);
  v82->set_f_9(0.528980);
  Message3::M4::M15::M32* v83 = v80->mutable_f_4();
  v83->set_f_0(Message3::M4::M15::M32::E17_CONST_3);
  v80->set_f_1(0.999030);
  Message3::M4::M16* v85 = v80_0->mutable_f_3();
  v85->set_f_0(0x2c6677ce);
  Message3::M4::M16::M30* v87 = v85->mutable_f_4();
  Message3::M4::M16::M30::M43* v88 = v87->mutable_f_1();
  v88->set_f_0(0x3511fe9d80);
  Message3::M4::M16::M30::M43::M50* v89 = v88->mutable_f_2();
  Message3::M4::M16::M30::M43::M50::M70* v92 = v89->mutable_f_7();
  v92->set_f_0(0xd9);
  Message3::M4::M16::M30::M43::M50::M65* v94_0 = v89->add_f_5();
  Message3::M4::M16::M30::M43::M50::M65::M76* v94 = v94_0->mutable_f_1();
  v94->set_f_4(0x531);
  v94->add_f_3(s->substr(0, 7));
  v94_0->set_f_0(true);
  Message3::M4::M16::M30::M43::M50::M65* v94_1 = v89->add_f_5();
  v94_1->set_f_0(false);
  Message3::M4::M16::M30::M43::M50::M65::M76* v95 = v94_1->mutable_f_1();
  v95->add_f_3(s->substr(0, 4));
  Message3::M4::M16::M30::M43::M50::M72* v97 = v89->mutable_f_8();
  v97->set_f_4(0x2b636bec);
  v97->set_f_6(0x5157a9183db22298);
  v97->set_f_5(Message3::M4::M16::M30::M43::M50::M72::E29_CONST_1);
  v97->set_f_1(0x1a);
  Message3::M4::M16::M30::M43::M50::M64* v99 = v89->mutable_f_4();
  v99->set_f_2(s->substr(0, 17));
  v88->set_f_1(0x6e14204114768925);
  Message3::M4::M20* v100 = v80_0->mutable_f_4();
  Message3::M4::M20::M39* v102_0 = v100->add_f_2();
  v102_0->set_f_0(0xca4cccf9366be63);
  v100->set_f_0(0xb5);
  Message3::M4::M9* v102 = v80_0->mutable_f_1();
  v102->set_f_1(Message3::M4::M9::E6_CONST_3);
  Message3::M3* v104 = message->mutable_f_12();
  v104->set_f_0(0x13);
  Message3::M3::M13* v106_0 = v104->add_f_10();
  Message3::M3::M13::M34* v107_0 = v106_0->add_f_3();
  v107_0->set_f_0(0xc0);
  v104->set_f_5(Message3::M3::E3_CONST_1);
  v104->set_f_4(s->substr(0, 7));
  Message3::M1* v107 = message->mutable_f_10();
  Message3::M1::M12* v109_0 = v107->add_f_2();
  Message3::M1::M12::M27* v109 = v109_0->mutable_f_1();
  v109->set_f_2(s->substr(0, 22));
  v109->set_f_0(s->substr(0, 5));
  message->set_f_4(0x70);
}

inline void Message3_Set_3(Message3* message, std::string* s) {
  Message3::M1* v0 = message->mutable_f_10();
  Message3::M1::M12* v2_0 = v0->add_f_2();
  Message3::M1::M12::M27* v2 = v2_0->mutable_f_1();
  v2->set_f_0(s->substr(0, 6));
  v2->set_f_3(0x5f);
  Message3::M4* v4_0 = message->add_f_13();
  v4_0->set_f_0(0xc3);
  Message3::M4::M15* v4 = v4_0->mutable_f_2();
  Message3::M4::M15::M24* v5 = v4->mutable_f_3();
  Message3::M4::M15::M24::M42* v6 = v5->mutable_f_2();
  v6->set_f_11(false);
  v6->set_f_10(s->substr(0, 14));
  v6->set_f_1(0x3c65e826002c);
  v6->set_f_8(0.803266);
  v6->set_f_6(false);
  v6->set_f_13(Message3::M4::M15::M24::M42::E21_CONST_5);
  int64_t array_0[10] = {0x2a899ed15e7b353f,
                         0x254da4,
                         0xd268a0,
                         0xb3,
                         0x3c389d,
                         0xc4,
                         0x46,
                         0xaf,
                         0x51ed,
                         0x20};
  for (auto& value : array_0) v6->add_f_2(value);
  v5->set_f_0(Message3::M4::M15::M24::E14_CONST_1);
  v4->set_f_1(0.868559);
  Message3::M4::M15::M22* v7 = v4->mutable_f_2();
  v7->set_f_0(0xd38f);
  Message3::M4::M20* v9 = v4_0->mutable_f_4();
  Message3::M4::M20::M39* v11_1 = v9->add_f_2();
  v11_1->set_f_0(0x38199f4d833ff82d);
  v9->set_f_0(0x97);
  Message3::M4::M16* v11 = v4_0->mutable_f_3();
  Message3::M4::M16::M30* v12 = v11->mutable_f_4();
  Message3::M4::M16::M30::M43* v13 = v12->mutable_f_1();
  Message3::M4::M16::M30::M43::M50* v14 = v13->mutable_f_2();
  Message3::M4::M16::M30::M43::M50::M64* v15 = v14->mutable_f_4();
  v15->set_f_1(0x55);
  Message3::M4::M16::M30::M43::M50::M68* v16 = v14->mutable_f_6();
  v16->set_f_0(0x3);
  Message3::M4::M16::M30::M43::M50::M72* v17 = v14->mutable_f_8();
  v17->set_f_1(0x1aa3d4d5);
  v17->set_f_5(Message3::M4::M16::M30::M43::M50::M72::E29_CONST_1);
  v17->set_f_6(0xb1f9b0);
  v17->set_f_4(0x3ce8884e);
  Message3::M4::M16::M30::M43::M50::M65* v19_0 = v14->add_f_5();
  Message3::M4::M16::M30::M43::M50::M65::M76* v19 = v19_0->mutable_f_1();
  v19->set_f_1(s->substr(0, 311));
  Message3::M4::M16::M30::M43::M50::M70* v21 = v14->mutable_f_7();
  v21->set_f_0(0x256163e6a1fc52ba);
  Message3::M4::M16::M30::M43::M50::M60* v23 = v14->mutable_f_3();
  v23->set_f_0(0.817881);
  Message3::M4::M16::M29* v24 = v11->mutable_f_3();
  v24->set_f_0(0x57ae61c5c8e448be);
  v11->set_f_0(0x45);
  Message3::M4::M9* v25 = v4_0->mutable_f_1();
  v25->set_f_1(Message3::M4::M9::E6_CONST_5);
  message->set_f_3(Message3::E1_CONST_2);
  message->set_f_2(0x809db3);
  Message3::M2* v26 = message->mutable_f_11();
  Message3::M2::M10* v27 = v26->mutable_f_2();
  Message3::M2::M10::M40* v29_0 = v27->add_f_11();
  v29_0->set_f_2(s->substr(0, 5));
  v29_0->set_f_0(s->substr(0, 5));
  Message3::M2::M10::M40* v29_1 = v27->add_f_11();
  v29_1->set_f_1(Message3::M2::M10::M40::E19_CONST_5);
  v29_1->set_f_2(s->substr(0, 5));
  Message3::M2::M10::M33* v30_0 = v27->add_f_10();
  Message3::M2::M10::M33::M45* v30 = v30_0->mutable_f_1();
  v30->set_f_0(false);
  v30->set_f_1(0xb4be4262f42edaa3);
  v30->set_f_4(0x1e809eadf3295402);
  v30->set_f_2(0x447596);
  Message3::M2::M10::M33::M45::M49* v31 = v30->mutable_f_11();
  Message3::M2::M10::M33::M45::M49::M71* v32 = v31->mutable_f_9();
  Message3::M2::M10::M33::M45::M49::M71::M80* v33 = v32->mutable_f_2();
  v33->set_f_0(s->substr(0, 4));
  Message3::M2::M10::M33::M45::M49::M71::M80::M85* v35_0 = v33->add_f_1();
  v35_0->set_f_0(Message3::M2::M10::M33::M45::M49::M71::M80::M85::E32_CONST_1);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94* v36 =
      v35_0->mutable_f_2();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96* v37 =
      v36->mutable_f_1();
  v37->set_f_14(s->substr(0, 27));
  v37->set_f_0(false);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99* v39 =
      v37->mutable_f_52();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100* v41_0 =
      v39->add_f_7();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101*
      v42_0 = v41_0->add_f_5();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102* v43_0 = v42_0->add_f_1();
  v43_0->set_f_0(0x3d);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103* v43 = v43_0->mutable_f_2();
  v43->set_f_0(true);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104* v44 = v43->mutable_f_1();
  v44->set_f_2(Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::
                   M100::M101::M102::M103::M104::E45_CONST_1);
  v44->set_f_0(0x95ca23dadf7a339);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v46_0 = v44->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M107* v47 = v46_0->mutable_f_3();
  v47->set_f_1(0xc4);
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105* v46_1 = v44->add_f_6();
  Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::M99::M100::M101::
      M102::M103::M104::M105::M106* v49 = v46_1->mutable_f_2();
  v49->set_f_0(0x56);
  v41_0->set_f_1(0x3dfb);
  v41_0->set_f_0(0xfebc);
  v39->set_f_3(0.100527);
  v39->set_f_0(s->substr(0, 1));
  v39->set_f_1(0.268714);
  v37->set_f_21(0x75ad3d60);
  v37->set_f_19(
      Message3::M2::M10::M33::M45::M49::M71::M80::M85::M94::M96::E39_CONST_5);
  v37->set_f_15(0x6b32b6e7);
  v37->set_f_35(0x683c52a);
  v37->set_f_32(s->substr(0, 3));
  v37->set_f_27(0x44);
  v37->set_f_33(0.080252);
  v37->set_f_12(false);
  v37->set_f_13(s->substr(0, 27));
  v37->set_f_23(s->substr(0, 20));
  v36->set_f_0(0x690178ad);
  Message3::M2::M10::M33::M45::M49::M71::M80::M89* v50 = v33->mutable_f_4();
  v50->set_f_0(0x49f38b59);
  v32->add_f_0(0x4fb7b8f2);
  v32->add_f_0(0x1166);
  Message3::M2::M10::M33::M45::M49::M56* v56 = v31->mutable_f_4();
  v56->set_f_0(0x56);
  Message3::M2::M10::M33::M45::M49::M67* v57 = v31->mutable_f_8();
  Message3::M2::M10::M33::M45::M49::M67::M78* v58 = v57->mutable_f_1();
  v58->set_f_0(0x59ec4ab1);
  Message3::M2::M10::M33::M45::M49::M63* v61_0 = v31->add_f_6();
  v61_0->set_f_0(0x7c);
  Message3::M2::M10::M33::M45::M49::M63::M81* v61 = v61_0->mutable_f_2();
  v61->set_f_0(0.374355);
  v61->set_f_3(s->substr(0, 45));
  Message3::M2::M10::M33::M45::M49::M63* v61_1 = v31->add_f_6();
  Message3::M2::M10::M33::M45::M49::M63::M81* v62 = v61_1->mutable_f_2();
  v62->set_f_2(true);
  v62->set_f_1(false);
  Message3::M2::M10::M33::M45::M49::M55* v63 = v31->mutable_f_3();
  v63->set_f_0(0x35862a82);
  Message3::M2::M10::M33::M45::M51* v64 = v30->mutable_f_12();
  v64->set_f_17(0x46);
  v64->set_f_3(0x98);
  v64->set_f_7(s->substr(0, 14));
  v64->set_f_8(s->substr(0, 14));
  v64->set_f_13(s->substr(0, 19));
  float array_1[64] = {
      0.083195, 0.899389, 0.250724, 0.997598, 0.796362, 0.499071, 0.602361,
      0.281656, 0.221443, 0.574013, 0.812258, 0.717296, 0.794207, 0.255107,
      0.383320, 0.666923, 0.439448, 0.944094, 0.557086, 0.831117, 0.137958,
      0.125472, 0.107184, 0.339304, 0.438549, 0.300391, 0.786568, 0.624622,
      0.098210, 0.948744, 0.019206, 0.648428, 0.712198, 0.809277, 0.474068,
      0.008254, 0.770810, 0.078558, 0.320660, 0.861793, 0.717762, 0.473434,
      0.920703, 0.667986, 0.972345, 0.019295, 0.705028, 0.391647, 0.866105,
      0.834466, 0.965223, 0.585636, 0.599064, 0.913667, 0.225737, 0.274789,
      0.737239, 0.712544, 0.583956, 0.172329, 0.313161, 0.063089, 0.575636,
      0.504346};
  for (auto& value : array_1) v64->add_f_21(value);
  v64->set_f_18(0x185a124c);
  Message3::M2::M10::M33::M45::M51::M66* v66_0 = v64->add_f_48();
  v66_0->set_f_0(0x33d6a243d4917fc9);
  Message3::M2::M10::M33::M45::M51::M69* v67_0 = v64->add_f_49();
  v67_0->set_f_0(0x74);
  v67_0->set_f_1(Message3::M2::M10::M33::M45::M51::M69::E28_CONST_4);
  Message3::M2::M10::M33::M45::M51::M62* v67 = v64->mutable_f_47();
  Message3::M2::M10::M33::M45::M51::M62::M79* v69_0 = v67->add_f_2();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84* v71 = v69_0->mutable_f_4();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93* v72 =
      v71->mutable_f_1();
  v72->set_f_0(0xd3);
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95* v74_0 =
      v72->add_f_2();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97* v74 =
      v74_0->mutable_f_2();
  v74->set_f_0(Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97::
                   E42_CONST_5);
  v74_0->set_f_0(0x2061c2f3);
  v69_0->set_f_0(0x1171b7b115768a03);
  Message3::M2::M10::M33::M45::M51::M62::M79* v69_1 = v67->add_f_2();
  Message3::M2::M10::M33::M45::M51::M62::M79::M83* v75 = v69_1->mutable_f_3();
  v75->set_f_0(s->substr(0, 5));
  Message3::M2::M10::M33::M45::M51::M62::M79::M84* v77 = v69_1->mutable_f_4();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93* v78 =
      v77->mutable_f_1();
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95* v80_0 =
      v78->add_f_2();
  v80_0->set_f_0(0x45cd38e0);
  Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97* v80 =
      v80_0->mutable_f_2();
  v80->set_f_0(Message3::M2::M10::M33::M45::M51::M62::M79::M84::M93::M95::M97::
                   E42_CONST_1);
  Message3::M2::M10::M33::M45::M51::M58* v82 = v64->mutable_f_45();
  Message3::M2::M10::M33::M45::M51::M58::M74* v84 = v82->mutable_f_6();
  v84->set_f_0(0xd9);
  v64->set_f_12(true);
  Message3::M2::M10::M33::M45::M51::M53* v86 = v64->mutable_f_41();
  v86->set_f_0(0x5f2328a);
  v64->set_f_6(s->substr(0, 20));
  Message3::M2::M10::M33::M45::M51::M52* v88_0 = v64->add_f_39();
  Message3::M2::M10::M33::M45::M51::M52::M77* v88 = v88_0->mutable_f_2();
  v88->set_f_0(0x825643128b7e6d40);
  v30->set_f_5(0x69b8);
  v27->set_f_2(s->substr(0, 13));
  v27->set_f_4(Message3::M2::M10::E8_CONST_3);
  v27->set_f_5(Message3::M2::M10::E9_CONST_1);
  Message3::M3* v91 = message->mutable_f_12();
  Message3::M3::M13* v93_0 = v91->add_f_10();
  v93_0->set_f_0(s->substr(0, 7));
  Message3::M3::M13::M34* v94_0 = v93_0->add_f_3();
  v94_0->set_f_1(0x11f28981);
  v91->set_f_4(s->substr(0, 30));
  v91->set_f_5(Message3::M3::E3_CONST_5);
  Message3::M6* v94 = message->mutable_f_16();
  Message3::M6::M21* v96_0 = v94->add_f_3();
  v96_0->set_f_22(s->substr(0, 2));
  v96_0->set_f_42(0x9c48);
  v96_0->set_f_18(0x83);
  v96_0->set_f_20(0x402ceba);
  v96_0->set_f_35(Message3::M6::M21::E13_CONST_1);
  int32_t array_2[19] = {0x56982c, 0x25,     0x34,       0xc2,     0x21eb83be,
                         0xd9,     0x9e1ca9, 0x6061f79f, 0x56,     0x28,
                         0x55b9e2, 0xdbae4f, 0xa5,       0x8457c6, 0x3c,
                         0x7,      0x71,     0x5,        0x2};
  for (auto& value : array_2) v96_0->add_f_15(value);
  v96_0->set_f_37(s->substr(0, 1));
  v96_0->set_f_21(0xb4);
  v96_0->set_f_2(0xd4);
  v96_0->set_f_0(Message3::M6::M21::E10_CONST_2);
  v96_0->set_f_1(Message3::M6::M21::E11_CONST_4);
  v96_0->set_f_32(s->substr(0, 4));
  v96_0->set_f_25(0x573eaa0f26a94302);
  v96_0->set_f_16(0.033182);
  v96_0->set_f_26(0x4a);
  v96_0->set_f_4(true);
  v96_0->set_f_5(s->substr(0, 32));
  Message3::M8* v96 = message->mutable_f_19();
  Message3::M8::M17* v97 = v96->mutable_f_5();
  Message3::M8::M17::M31* v98 = v97->mutable_f_5();
  v98->set_f_1(s->substr(0, 28));
  v98->set_f_0(s->substr(0, 23));
  Message3::M8::M17::M35* v101_0 = v97->add_f_6();
  v101_0->set_f_2(0.425500);
  Message3::M8::M19* v104_0 = v96->add_f_6();
  v104_0->set_f_0(s->substr(0, 30));
  Message3::M7* v110 = message->mutable_f_17();
  Message3::M7::M14* v111 = v110->mutable_f_1();
  Message3::M7::M14::M38* v113 = v111->mutable_f_4();
  Message3::M7::M14::M38::M41* v114 = v113->mutable_f_2();
  v114->set_f_0(s->substr(0, 35));
  v113->set_f_0(true);
  v111->set_f_0(s->substr(0, 7));
}

}  // namespace fleetbench::proto
#endif  // THIRD_PARTY_FLEETBENCH_PROTO_ACCESS_MESSAGE3_H_
