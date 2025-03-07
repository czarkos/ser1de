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

#ifndef THIRD_PARTY_FLEETBENCH_PROTO_ACCESS_MESSAGE4_H_
#define THIRD_PARTY_FLEETBENCH_PROTO_ACCESS_MESSAGE4_H_

#include <cstddef>
#include <cstdint>
#include <string>

#include "Message4.pb.h"

namespace fleetbench::proto {

inline void Message4_Set_1(Message4* message, std::string* s) {
  Message4::M1* v0 = message->mutable_f_8();
  v0->set_f_0(0x85);
  Message4::M6* v2 = message->mutable_f_13();
  Message4::M6::M19* v3 = v2->mutable_f_2();
  Message4::M6::M19::M20* v4 = v3->mutable_f_3();
  v4->set_f_4(Message4::M6::M19::M20::E10_CONST_4);
  Message4::M6::M19::M30* v5 = v3->mutable_f_7();
  v5->set_f_3(s->substr(0, 10));
  v5->set_f_5(0x63);
  v5->set_f_4(0xe14a);
  Message4::M6::M19::M30::M39* v7_0 = v5->add_f_10();
  v7_0->set_f_1(0x46b20afa7523c9c1);
  v7_0->set_f_4(true);
  Message4::M6::M19::M30::M32* v7 = v5->mutable_f_9();
  v7->set_f_5(0xc2);
  v7->set_f_6(0xec);
  v7->set_f_2(s->substr(0, 3));
  v7->set_f_4(Message4::M6::M19::M30::M32::E14_CONST_4);
  v7->set_f_1(0xcb);
  v3->set_f_0(s->substr(0, 7));
  v3->set_f_1(0x100b);
  Message4::M6::M19::M24* v9_0 = v3->add_f_4();
  Message4::M6::M19::M24::M33* v9 = v9_0->mutable_f_1();
  Message4::M6::M19::M24::M33::M50* v10 = v9->mutable_f_3();
  v10->set_f_0(0xd7);
  Message4::M6::M19::M24::M33::M44* v12 = v9->mutable_f_2();
  Message4::M6::M19::M24::M33::M44::M55* v14_0 = v12->add_f_1();
  v14_0->set_f_0(0.671021);
  Message4::M6::M19::M27* v16_0 = v3->add_f_6();
  float array_1[63] = {
      0.670929, 0.030813, 0.033857, 0.536148, 0.476066, 0.890116, 0.788649,
      0.449734, 0.368322, 0.763288, 0.049087, 0.744135, 0.970534, 0.048007,
      0.215355, 0.464327, 0.136959, 0.105727, 0.526401, 0.503033, 0.734290,
      0.793799, 0.080450, 0.763201, 0.279365, 0.881636, 0.389234, 0.982766,
      0.894528, 0.795634, 0.810148, 0.585402, 0.607745, 0.069633, 0.100013,
      0.092303, 0.040779, 0.331419, 0.183539, 0.961325, 0.081249, 0.750616,
      0.606980, 0.920775, 0.509022, 0.438220, 0.858059, 0.004971, 0.126924,
      0.098630, 0.502692, 0.864441, 0.058886, 0.225434, 0.473493, 0.833862,
      0.481339, 0.849257, 0.669512, 0.888393, 0.536167, 0.518707, 0.223651};
  for (auto& value : array_1) v16_0->add_f_0(value);
  Message4::M6::M19::M27::M31* v16 = v16_0->mutable_f_3();
  Message4::M6::M19::M27::M31::M42* v17 = v16->mutable_f_4();
  v17->set_f_0(0.238834);
  v16_0->set_f_1(0.247207);
  v16_0->set_f_2(Message4::M6::M19::M27::E12_CONST_4);
  Message4::M6::M19::M26* v19 = v3->mutable_f_5();
  v19->set_f_0(0x5c385ff6);
  Message4::M2* v22_0 = message->add_f_9();
  v22_0->set_f_1(0x2b8de1c90ad299ae);
  message->set_f_5(0xd5);
  Message4::M7* v22 = message->mutable_f_14();
  Message4::M7::M15* v24_0 = v22->add_f_3();
  v24_0->set_f_0(Message4::M7::M15::E3_CONST_2);
  v24_0->set_f_5(s->substr(0, 5));
  v24_0->set_f_4(Message4::M7::M15::E4_CONST_3);
  Message4::M7::M15* v24_1 = v22->add_f_3();
  v24_1->set_f_3(0x26c55ba3b9);
  Message4::M7::M14* v24 = v22->mutable_f_1();
  Message4::M7::M14::M29* v25 = v24->mutable_f_1();
  v25->set_f_0(0x6508994);
  Message4::M7::M14::M29::M34* v26 = v25->mutable_f_4();
  v26->set_f_0(0x1a);
  Message4::M7::M14::M29::M34::M41* v27 = v26->mutable_f_4();
  Message4::M7::M14::M29::M34::M41::M58* v30_0 = v27->add_f_1();
  v30_0->set_f_2(s->substr(0, 1));
  Message4::M7::M14::M29::M34::M41::M58::M66* v30 = v30_0->mutable_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73* v31 = v30->mutable_f_3();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M76* v32 =
      v31->mutable_f_3();
  v32->set_f_0(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M76::E27_CONST_3);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77* v34_0 = v31->add_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78* v34 =
      v34_0->mutable_f_11();
  v34->set_f_1(0.036414);
  v34->set_f_0(0.398156);
  v34_0->set_f_2(s->substr(0, 8));
  v34_0->set_f_6(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::E28_CONST_1);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v37_0 =
      v34_0->add_f_13();
  v37_0->set_f_0(0x8811);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v38_0 =
      v37_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v39_0 =
      v38_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v40_0 = v39_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v40 = v40_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v41 = v40->mutable_f_2();
  v41->set_f_0(0x7ab1e6c785371870);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v42 = v41->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v43 = v42->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v45_1 = v43->add_f_3();
  v45_1->set_f_0(0x1aef);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v40_1 = v39_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v46 = v40_1->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v47 = v46->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v48 = v47->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v49 = v48->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v52_0 = v49->add_f_3();
  v52_0->set_f_0(0x3b);
  v49->set_f_0(0x9993);
  v39_0->set_f_0(s->substr(0, 3));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v38_1 =
      v37_0->add_f_2();
  v38_1->set_f_0(0x8a);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v53_0 =
      v38_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v54_0 = v53_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v54 = v54_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v55 = v54->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v56 = v55->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v57 = v56->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v60_0 = v57->add_f_3();
  v60_0->set_f_0(0xd);
  v53_0->set_f_0(s->substr(0, 72));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v37_1 =
      v34_0->add_f_13();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v61_0 =
      v37_1->add_f_2();
  v61_0->set_f_0(0xee);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v62_0 =
      v61_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v63_0 = v62_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v63 = v63_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v64 = v63->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v65 = v64->mutable_f_1();
  v65->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::
                   M84::M86::M87::M88::M89::E31_CONST_4);
  v64->set_f_0(0x295864fdf86d7a0f);
  v63_0->set_f_0(s->substr(0, 24));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v63_1 = v62_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v69 = v63_1->mutable_f_1();
  v69->set_f_0(0.197737);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v70 = v69->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v71 = v70->mutable_f_1();
  v71->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::
                   M84::M86::M87::M88::M89::E31_CONST_4);
  v34_0->set_f_3(0x2ac24e27);
  v27->set_f_0(Message4::M7::M14::M29::M34::M41::E17_CONST_5);
  Message4::M7::M16* v78 = v22->mutable_f_4();
  Message4::M7::M16::M28* v79 = v78->mutable_f_2();
  Message4::M7::M16::M28::M35* v80 = v79->mutable_f_1();
  v80->set_f_0(0xf1dd14957fd23550);
  Message4::M7::M16::M28::M35::M51* v81 = v80->mutable_f_26();
  v81->set_f_0(0x5aa891f6);
  Message4::M7::M16::M28::M35::M52* v83_0 = v80->add_f_27();
  v83_0->set_f_0(0x1f5b0b5d);
  Message4::M7::M16::M28::M35::M52::M57* v83 = v83_0->mutable_f_2();
  v83->set_f_0(s->substr(0, 31));
  v80->set_f_7(s->substr(0, 8));
  v80->set_f_3(0.722856);
  v80->set_f_5(true);
  v80->set_f_6(0x61f33d1e);
  Message4::M7::M16::M28::M35::M43* v85 = v80->mutable_f_23();
  Message4::M7::M16::M28::M35::M43::M62* v86 = v85->mutable_f_7();
  Message4::M7::M16::M28::M35::M43::M62::M70* v88_0 = v86->add_f_3();
  v88_0->set_f_0(s->substr(0, 6));
  v86->set_f_0(0xc1);
  v85->set_f_3(0x5f72a7);
  v80->set_f_2(s->substr(0, 6));
  Message4::M7::M16::M28::M35::M40* v89 = v80->mutable_f_21();
  v89->set_f_5(0xcc);
  v89->set_f_3(0x7f3b2);
  Message4::M7::M18* v90 = v22->mutable_f_6();
  Message4::M7::M18::M22* v92_0 = v90->add_f_2();
  v92_0->set_f_0(true);
  Message4::M7::M18::M22::M36* v93_0 = v92_0->add_f_1();
  v93_0->set_f_0(Message4::M7::M18::M22::M36::E16_CONST_4);
  v90->set_f_0(Message4::M7::M18::E8_CONST_4);
  Message4::M5* v93 = message->mutable_f_12();
  Message4::M5::M17* v95 = v93->mutable_f_2();
  v95->set_f_0(Message4::M5::M17::E5_CONST_3);
  v95->set_f_4(s->substr(0, 30));
  v95->set_f_1(Message4::M5::M17::E6_CONST_1);
  Message4::M8* v96 = message->mutable_f_16();
  v96->set_f_1(Message4::M8::E2_CONST_3);
  v96->set_f_0(s->substr(0, 1));
  Message4::M4* v97 = message->mutable_f_11();
  Message4::M4::M12* v99_0 = v97->add_f_2();
  Message4::M4::M12::M23* v100_0 = v99_0->add_f_3();
  Message4::M4::M12::M23::M37* v100 = v100_0->mutable_f_3();
  v100->set_f_0(0.976379);
  Message4::M3* v101 = message->mutable_f_10();
  v101->set_f_6(s->substr(0, 5));
  Message4::M3::M9* v102 = v101->mutable_f_10();
  Message4::M3::M9::M25* v104 = v102->mutable_f_3();
  v104->set_f_8(0x32);
  v104->set_f_0(0x1a49e0f3);
  v104->set_f_10(Message4::M3::M9::M25::E11_CONST_3);
  v104->set_f_2(0x789f8cc7);
  v104->set_f_4(s->substr(0, 5));
  Message4::M3::M9::M25::M38* v105 = v104->mutable_f_13();
  Message4::M3::M9::M25::M38::M49* v107 = v105->mutable_f_8();
  Message4::M3::M9::M25::M38::M49::M65* v108 = v107->mutable_f_5();
  Message4::M3::M9::M25::M38::M49::M65::M69* v110_0 = v108->add_f_12();
  Message4::M3::M9::M25::M38::M49::M65::M69::M72* v111 = v110_0->mutable_f_3();
  v111->set_f_0(0x74007f98);
  v111->set_f_1(0.300567);
  v110_0->set_f_0(0x76);
  v108->set_f_2(0xa3367180);
  v108->set_f_5(0x61776fa6899a174a);
  v108->set_f_0(0x20982df1be7ed1c);
  v108->set_f_3(0xe076d2499fb9);
  Message4::M3::M9::M25::M38::M49::M59* v112 = v107->mutable_f_2();
  v112->set_f_0(Message4::M3::M9::M25::M38::M49::M59::E20_CONST_4);
  v105->set_f_0(0.930524);
  Message4::M3::M9::M25::M38::M47* v116_0 = v105->add_f_4();
  Message4::M3::M9::M25::M38::M47::M60* v116 = v116_0->mutable_f_4();
  v116->set_f_2(0.732913);
  v116->set_f_0(0x504a3fd98e1c8018);
  Message4::M3::M9::M25::M38::M47::M53* v118 = v116_0->mutable_f_1();
  v118->set_f_0(0x36);
  Message4::M3::M9::M25::M38::M47::M53::M71* v119 = v118->mutable_f_1();
  v119->set_f_4(s->substr(0, 20));
  v119->set_f_8(0.778541);
  Message4::M3::M9::M25::M38::M47::M53::M71::M74* v121_0 = v119->add_f_23();
  v121_0->set_f_0(s->substr(0, 6));
  v121_0->set_f_4(0xad);
  v121_0->set_f_2(Message4::M3::M9::M25::M38::M47::M53::M71::M74::E26_CONST_4);
  Message4::M3::M9::M25::M38::M47* v116_1 = v105->add_f_4();
  Message4::M3::M9::M25::M38::M47::M53* v123 = v116_1->mutable_f_1();
  Message4::M3::M9::M25::M38::M47::M53::M71* v124 = v123->mutable_f_1();
  v124->set_f_2(Message4::M3::M9::M25::M38::M47::M53::M71::E24_CONST_3);
  Message4::M3::M9::M25::M38::M47::M53::M71::M74* v126_0 = v124->add_f_23();
  v126_0->set_f_1(0x7971);
  v126_0->set_f_4(0xae1a2a5387a5c8);
  v124->set_f_4(s->substr(0, 3));
  v124->set_f_6(Message4::M3::M9::M25::M38::M47::M53::M71::E25_CONST_2);
  v124->set_f_7(false);
  v124->set_f_10(0.954812);
  v124->set_f_1(0x6a649edb);
  v124->set_f_9(s->substr(0, 21));
  v104->set_f_3(0x52a4aaaa);
  v104->set_f_6(0x109cac8b);
}

inline void Message4_Set_2(Message4* message, std::string* s) {
  Message4::M6* v0 = message->mutable_f_13();
  Message4::M6::M19* v2 = v0->mutable_f_2();
  Message4::M6::M19::M27* v4_0 = v2->add_f_6();
  v4_0->set_f_1(0.421713);
  Message4::M6::M19::M27::M31* v4 = v4_0->mutable_f_3();
  Message4::M6::M19::M27::M31::M42* v5 = v4->mutable_f_4();
  Message4::M6::M19::M27::M31::M42::M54* v6 = v5->mutable_f_1();
  v6->set_f_0(Message4::M6::M19::M27::M31::M42::M54::E19_CONST_5);
  Message4::M6::M19::M30* v7 = v2->mutable_f_7();
  Message4::M6::M19::M30::M32* v8 = v7->mutable_f_9();
  v8->set_f_3(false);
  v8->set_f_0(s->substr(0, 54));
  v8->set_f_6(0x9c59);
  v8->set_f_5(0x74);
  v7->set_f_0(Message4::M6::M19::M30::E13_CONST_4);
  v7->set_f_4(0xbe57ff);
  Message4::M6::M19::M30::M39* v10_0 = v7->add_f_10();
  v10_0->set_f_4(false);
  v10_0->set_f_0(true);
  v10_0->set_f_2(true);
  v7->set_f_2(false);
  Message4::M6::M19::M20* v11 = v2->mutable_f_3();
  v11->set_f_0(0.441355);
  v11->set_f_3(Message4::M6::M19::M20::E9_CONST_4);
  v2->set_f_0(s->substr(0, 31));
  Message4::M6::M19::M24* v13_0 = v2->add_f_4();
  v13_0->set_f_0(0x3a9177f4);
  Message4::M6::M19::M24::M33* v13 = v13_0->mutable_f_1();
  v13->set_f_0(0x10);
  v0->set_f_0(0x6c);
  Message4::M4* v19 = message->mutable_f_11();
  v19->set_f_0(s->substr(0, 27));
  Message4::M4::M12* v21_0 = v19->add_f_2();
  Message4::M4::M12::M23* v22_0 = v21_0->add_f_3();
  v22_0->set_f_0(0.056454);
  Message4::M8* v25 = message->mutable_f_16();
  v25->set_f_1(Message4::M8::E2_CONST_3);
  Message4::M7* v27 = message->mutable_f_14();
  Message4::M7::M18* v28 = v27->mutable_f_6();
  Message4::M7::M18::M22* v30_0 = v28->add_f_2();
  Message4::M7::M18::M22::M36* v31_0 = v30_0->add_f_1();
  v31_0->set_f_0(Message4::M7::M18::M22::M36::E16_CONST_2);
  Message4::M7::M14* v31 = v27->mutable_f_1();
  Message4::M7::M14::M29* v32 = v31->mutable_f_1();
  Message4::M7::M14::M29::M34* v33 = v32->mutable_f_4();
  Message4::M7::M14::M29::M34::M41* v34 = v33->mutable_f_4();
  v34->set_f_0(Message4::M7::M14::M29::M34::M41::E17_CONST_2);
  Message4::M7::M14::M29::M34::M41::M58* v37_0 = v34->add_f_1();
  v37_0->set_f_2(s->substr(0, 8));
  Message4::M7::M14::M29::M34::M41::M58::M66* v37 = v37_0->mutable_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73* v38 = v37->mutable_f_3();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77* v40_0 = v38->add_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v41_0 =
      v40_0->add_f_13();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v42_0 =
      v41_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v43_0 =
      v42_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v44_0 = v43_0->add_f_2();
  v44_0->set_f_0(s->substr(0, 21));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v44 = v44_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v45 = v44->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v46 = v45->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v47 = v46->mutable_f_1();
  v47->set_f_0(0x41);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M91* v48 = v47->mutable_f_2();
  v48->set_f_0(0x5c);
  v46->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::
                   M84::M86::M87::M88::M89::E31_CONST_2);
  v41_0->set_f_0(0x595dc8cbb3e6dc7e);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78* v50 =
      v40_0->mutable_f_11();
  v50->set_f_1(0.532558);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78::M81* v51 =
      v50->mutable_f_4();
  v51->set_f_0(s->substr(0, 5));
  v40_0->set_f_2(s->substr(0, 17));
  v40_0->set_f_7(0xa940);
  v38->set_f_0(0x214a389b89ef1e);
  Message4::M7::M14::M29::M34::M41::M58* v37_1 = v34->add_f_1();
  v37_1->set_f_2(s->substr(0, 30));
  v37_1->set_f_1(s->substr(0, 15));
  Message4::M7::M14::M29::M34::M41::M58::M66* v56 = v37_1->mutable_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73* v57 = v56->mutable_f_3();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77* v59_0 = v57->add_f_4();
  v59_0->set_f_0(s->substr(0, 1));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M79* v59 =
      v59_0->mutable_f_12();
  v59->set_f_0(s->substr(0, 6));
  v59_0->set_f_5(s->substr(0, 22));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v63_0 =
      v59_0->add_f_13();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v64_0 =
      v63_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v65_0 =
      v64_0->add_f_2();
  v65_0->set_f_0(s->substr(0, 38));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v64_1 =
      v63_0->add_f_2();
  v64_1->set_f_0(0x93);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v73_0 =
      v64_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v74_0 = v73_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v74 = v74_0->mutable_f_1();
  v74->set_f_0(0.323767);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v75 = v74->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v76 = v75->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v77 = v76->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v80_0 = v77->add_f_3();
  v80_0->set_f_0(0x60b14baa);
  v76->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::
                   M84::M86::M87::M88::M89::E31_CONST_3);
  v75->set_f_0(0x545927718f8f6b67);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v73_1 =
      v64_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v81_0 = v73_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v81 = v81_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v82 = v81->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v83 = v82->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v84 = v83->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M91* v85 = v84->mutable_f_2();
  v85->set_f_0(0xd539ff82);
  v81->set_f_0(0.119992);
  v63_0->set_f_0(0x93d484a9);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v63_1 =
      v59_0->add_f_13();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v88_0 =
      v63_1->add_f_2();
  v88_0->set_f_0(0x54b6151f);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v89_0 =
      v88_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v90_0 = v89_0->add_f_2();
  v90_0->set_f_0(s->substr(0, 8));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v90 = v90_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v91 = v90->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v92 = v91->mutable_f_1();
  v92->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::
                   M84::M86::M87::M88::M89::E31_CONST_1);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v93 = v92->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v95_1 = v93->add_f_3();
  v95_1->set_f_0(0xb109395);
  v90->set_f_0(0.406817);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v88_1 =
      v63_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v97_0 =
      v88_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v98_0 = v97_0->add_f_2();
  v98_0->set_f_0(s->substr(0, 26));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v98 = v98_0->mutable_f_1();
  v98->set_f_0(0.865108);
  v97_0->set_f_0(s->substr(0, 4));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v97_1 =
      v88_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v105_0 = v97_1->add_f_2();
  v105_0->set_f_0(s->substr(0, 8));
  v59_0->set_f_7(0x4b15d7ef1cc31751);
  v59_0->set_f_6(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::E28_CONST_5);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78* v111 =
      v59_0->mutable_f_11();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78::M81* v112 =
      v111->mutable_f_4();
  v112->set_f_0(s->substr(0, 12));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77* v59_1 = v57->add_f_4();
  v59_1->set_f_2(s->substr(0, 7));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v114_0 =
      v59_1->add_f_13();
  v114_0->set_f_0(0x1963c799);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v115_0 =
      v114_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v116_0 =
      v115_0->add_f_2();
  v116_0->set_f_0(s->substr(0, 9));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v117_0 = v116_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v117 = v117_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v118 = v117->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v119 = v118->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v120 = v119->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v122_0 = v120->add_f_3();
  v122_0->set_f_0(0xd3);
  v118->set_f_0(0x78fb0a7bee816d3d);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v114_1 =
      v59_1->add_f_13();
  v114_1->set_f_0(0xf7);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v124_0 =
      v114_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v125_0 =
      v124_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v126_0 = v125_0->add_f_2();
  v126_0->set_f_0(s->substr(0, 4));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v126 = v126_0->mutable_f_1();
  v126->set_f_0(0.697790);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v127 = v126->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v128 = v127->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v129 = v128->mutable_f_1();
  v129->set_f_0(0x2d);
  v128->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::
                    M84::M86::M87::M88::M89::E31_CONST_5);
  v127->set_f_0(0x2cbede2e7ca1a963);
  v125_0->set_f_0(s->substr(0, 5));
  v59_1->set_f_0(s->substr(0, 6));
  v59_1->set_f_1(s->substr(0, 26));
  v59_1->set_f_4(true);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M79* v132 =
      v59_1->mutable_f_12();
  v132->set_f_0(s->substr(0, 3));
  v59_1->set_f_6(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::E28_CONST_2);
  v59_1->set_f_3(0x700bc13b);
  v37_1->set_f_0(0x2326daba7a78ea3f);
  Message4::M7::M15* v139_0 = v27->add_f_3();
  v139_0->set_f_2(0x77a0);
  v27->set_f_0(0.274035);
  Message4::M7::M16* v139 = v27->mutable_f_4();
  Message4::M7::M16::M28* v140 = v139->mutable_f_2();
  Message4::M7::M16::M28::M35* v141 = v140->mutable_f_1();
  Message4::M7::M16::M28::M35::M40* v142 = v141->mutable_f_21();
  v142->set_f_6(0x6d73);
  v142->set_f_5(0x4);
  v141->set_f_8(0xa481655);
  Message4::M7::M16::M28::M35::M43* v145 = v141->mutable_f_23();
  v145->add_f_1(0xaf);
  v145->add_f_1(0x4dca218b);
  v145->add_f_1(0x7c);
  v145->add_f_1(0x1e);
  v141->set_f_3(0.789689);
  v140->set_f_0(s->substr(0, 4));
  Message4::M3* v151 = message->mutable_f_10();
  v151->set_f_4(0.611981);
  v151->set_f_1(0x605e2ceba9c8467b);
  Message4::M3::M9* v152 = v151->mutable_f_10();
  v152->set_f_0(s->substr(0, 4));
  Message4::M3::M9::M25* v154 = v152->mutable_f_3();
  v154->set_f_2(0xc7379d);
  v154->set_f_6(0x1b);
  v154->set_f_10(Message4::M3::M9::M25::E11_CONST_2);
  Message4::M3::M9::M25::M38* v155 = v154->mutable_f_13();
  Message4::M3::M9::M25::M38::M47* v157_0 = v155->add_f_4();
  Message4::M3::M9::M25::M38::M47::M53* v157 = v157_0->mutable_f_1();
  Message4::M3::M9::M25::M38::M47::M53::M71* v158 = v157->mutable_f_1();
  v158->set_f_1(0x49c9d3a3);
  v158->set_f_4(s->substr(0, 13));
  v158->set_f_2(Message4::M3::M9::M25::M38::M47::M53::M71::E24_CONST_2);
  v158->set_f_11(0x6a);
  v158->set_f_9(s->substr(0, 13));
  Message4::M3::M9::M25::M38::M47::M53::M71::M74* v160_0 = v158->add_f_23();
  v160_0->set_f_2(Message4::M3::M9::M25::M38::M47::M53::M71::M74::E26_CONST_1);
  Message4::M3::M9::M25::M38::M47::M60* v160 = v157_0->mutable_f_4();
  v160->set_f_1(0x17);
  Message4::M3::M9::M25::M38::M47::M60::M67* v161 = v160->mutable_f_9();
  v161->set_f_0(s->substr(0, 20));
  v160->set_f_0(0xea);
  Message4::M3::M9::M25::M38::M49* v162 = v155->mutable_f_8();
  v162->set_f_0(s->substr(0, 23));
  Message4::M3::M9::M25::M38::M49::M65* v163 = v162->mutable_f_5();
  v163->set_f_3(0x4d4ee51d7f);
  Message4::M3::M9::M25::M38::M49::M65::M69* v165_0 = v163->add_f_12();
  Message4::M3::M9::M25::M38::M49::M65::M69::M72* v165 = v165_0->mutable_f_3();
  v165->set_f_0(0x2dcd4153);
  v165->set_f_1(0.881125);
  Message4::M3::M9::M25::M38::M49::M65::M69* v165_1 = v163->add_f_12();
  Message4::M3::M9::M25::M38::M49::M65::M69::M75* v167 = v165_1->mutable_f_4();
  v167->set_f_0(0x1617342d);
  Message4::M3::M9::M25::M38::M49::M65::M69::M72* v168 = v165_1->mutable_f_3();
  v168->set_f_0(0x35642737);
  v163->set_f_2(0x6db833ac0574f34a);
  v163->set_f_1(Message4::M3::M9::M25::M38::M49::M65::E22_CONST_1);
  v163->set_f_7(Message4::M3::M9::M25::M38::M49::M65::E23_CONST_1);
  v163->set_f_8(0x1cdbe4e1);
  Message4::M3::M9::M25::M38::M45* v172 = v155->mutable_f_2();
  v172->set_f_0(Message4::M3::M9::M25::M38::M45::E18_CONST_5);
  v154->set_f_4(s->substr(0, 28));
  v154->set_f_0(0x7346c88d);
  Message4::M5* v173 = message->mutable_f_12();
  Message4::M5::M17* v174 = v173->mutable_f_2();
  v174->set_f_4(s->substr(0, 15));
  v174->set_f_6(Message4::M5::M17::E7_CONST_5);
  v174->set_f_3(0.475976);
  v174->set_f_1(Message4::M5::M17::E6_CONST_3);
  v174->set_f_5(0x529e);
  v174->set_f_2(0x6d6a6d24);
  message->set_f_0(0x4f4bd804);
  message->set_f_5(0x56);
  message->add_f_2(0x3b9fb5);
}

inline void Message4_Set_3(Message4* message, std::string* s) {
  Message4::M2* v1_1 = message->add_f_9();
  v1_1->set_f_0(0xa678f8);
  Message4::M1* v1 = message->mutable_f_8();
  Message4::M1::M13* v3_0 = v1->add_f_1();
  v3_0->set_f_0(s->substr(0, 1));
  v1->set_f_0(0xab2e);
  Message4::M8* v3 = message->mutable_f_16();
  v3->set_f_1(Message4::M8::E2_CONST_1);
  v3->set_f_0(s->substr(0, 37));
  Message4::M5* v4 = message->mutable_f_12();
  v4->set_f_0(0x3);
  Message4::M5::M17* v6 = v4->mutable_f_2();
  v6->set_f_5(0x97);
  v6->set_f_0(Message4::M5::M17::E5_CONST_5);
  v6->set_f_3(0.967642);
  v6->set_f_6(Message4::M5::M17::E7_CONST_4);
  message->set_f_5(0x1b85);
  Message4::M6* v7 = message->mutable_f_13();
  v7->set_f_0(0xf7d0b93);
  Message4::M6::M19* v9 = v7->mutable_f_2();
  Message4::M6::M19::M27* v11_0 = v9->add_f_6();
  Message4::M6::M19::M27::M31* v11 = v11_0->mutable_f_3();
  Message4::M6::M19::M27::M31::M42* v12 = v11->mutable_f_4();
  Message4::M6::M19::M27::M31::M42::M54* v13 = v12->mutable_f_1();
  v13->set_f_0(Message4::M6::M19::M27::M31::M42::M54::E19_CONST_3);
  v12->set_f_0(0.167877);
  Message4::M6::M19::M26* v14 = v9->mutable_f_5();
  v14->set_f_0(0x5b);
  Message4::M6::M19::M20* v15 = v9->mutable_f_3();
  v15->set_f_4(Message4::M6::M19::M20::E10_CONST_3);
  v15->set_f_3(Message4::M6::M19::M20::E9_CONST_3);
  Message4::M6::M19::M24* v17_0 = v9->add_f_4();
  Message4::M6::M19::M24::M33* v17 = v17_0->mutable_f_1();
  Message4::M6::M19::M24::M33::M50* v18 = v17->mutable_f_3();
  v18->set_f_0(0x5f62e177);
  Message4::M6::M19::M24::M33::M44* v20 = v17->mutable_f_2();
  Message4::M6::M19::M24::M33::M44::M56* v21 = v20->mutable_f_4();
  v21->set_f_0(0x71);
  v20->set_f_0(s->substr(0, 1));
  Message4::M6::M19::M24::M33::M44::M55* v23_0 = v20->add_f_1();
  v23_0->set_f_0(0.019399);
  Message4::M6::M19::M24* v17_1 = v9->add_f_4();
  Message4::M6::M19::M24::M33* v23 = v17_1->mutable_f_1();
  Message4::M6::M19::M24::M33::M50* v24 = v23->mutable_f_3();
  Message4::M6::M19::M24::M33::M50::M63* v25 = v24->mutable_f_1();
  v25->set_f_0(Message4::M6::M19::M24::M33::M50::M63::E21_CONST_2);
  Message4::M6::M19::M24::M33::M44* v26 = v23->mutable_f_2();
  Message4::M6::M19::M24::M33::M44::M56* v27 = v26->mutable_f_4();
  v27->set_f_0(0xd);
  Message4::M6::M19::M24::M33::M44::M55* v29_0 = v26->add_f_1();
  v29_0->set_f_0(0.892313);
  v17_1->set_f_0(0x66295434);
  Message4::M6::M19::M30* v29 = v9->mutable_f_7();
  v29->set_f_3(s->substr(0, 5));
  Message4::M6::M19::M30::M32* v30 = v29->mutable_f_9();
  v30->set_f_3(false);
  v29->set_f_2(false);
  v29->set_f_1(0x68f2);
  Message4::M6::M19::M30::M39* v32_0 = v29->add_f_10();
  v32_0->set_f_2(true);
  v32_0->set_f_4(true);
  Message4::M4* v32 = message->mutable_f_11();
  v32->set_f_0(s->substr(0, 4));
  Message4::M4::M12* v34_0 = v32->add_f_2();
  Message4::M4::M12::M23* v35_0 = v34_0->add_f_3();
  v35_0->set_f_0(0.471608);
  Message4::M4::M12::M23::M37* v35 = v35_0->mutable_f_3();
  v35->set_f_0(0.975869);
  v34_0->set_f_0(0.641545);
  Message4::M3* v36 = message->mutable_f_10();
  v36->set_f_5(0x23b946fc);
  v36->set_f_2(0xd47722563a5500);
  v36->set_f_1(0x137c1ed2ce851766);
  v36->set_f_3(s->substr(0, 18));
  v36->set_f_0(0x16);
  Message4::M3::M9* v37 = v36->mutable_f_10();
  Message4::M3::M9::M25* v38 = v37->mutable_f_3();
  v38->set_f_5(0.468041);
  v38->set_f_10(Message4::M3::M9::M25::E11_CONST_2);
  v38->set_f_7(s->substr(0, 7));
  v38->set_f_9(0x9a);
  Message4::M3::M9::M25::M38* v39 = v38->mutable_f_13();
  v39->set_f_0(0.858560);
  v39->set_f_1(0xf51d0ad);
  Message4::M3::M9::M25::M38::M47* v41_0 = v39->add_f_4();
  Message4::M3::M9::M25::M38::M47::M60* v41 = v41_0->mutable_f_4();
  v41->set_f_4(0xa8);
  v41->set_f_3(0x21136bcc);
  v41->set_f_1(0xd);
  Message4::M3::M9::M25::M38::M47::M53* v43 = v41_0->mutable_f_1();
  Message4::M3::M9::M25::M38::M47::M53::M71* v44 = v43->mutable_f_1();
  v44->set_f_11(0x30);
  v44->set_f_3(s->substr(0, 27));
  Message4::M3::M9::M25::M38::M47::M53::M71::M74* v46_0 = v44->add_f_23();
  v46_0->set_f_2(Message4::M3::M9::M25::M38::M47::M53::M71::M74::E26_CONST_5);
  v44->set_f_8(0.382288);
  v44->set_f_5(0x17be5de6f8cb9fc6);
  v44->set_f_4(s->substr(0, 5));
  v44->set_f_0(s->substr(0, 5));
  v43->set_f_0(0xe6);
  v41_0->set_f_0(0x24);
  Message4::M3::M9::M25::M38::M48* v47_0 = v39->add_f_7();
  v47_0->set_f_0(0.984831);
  Message4::M3::M9::M25::M38::M49* v47 = v39->mutable_f_8();
  Message4::M3::M9::M25::M38::M49::M64* v48 = v47->mutable_f_4();
  v48->set_f_0(0x54);
  v47->set_f_0(s->substr(0, 1));
  Message4::M3::M9::M25::M38::M49::M65* v49 = v47->mutable_f_5();
  v49->set_f_3(0xe083);
  v49->set_f_7(Message4::M3::M9::M25::M38::M49::M65::E23_CONST_2);
  v49->set_f_5(0xe1c4);
  v49->set_f_1(Message4::M3::M9::M25::M38::M49::M65::E22_CONST_4);
  Message4::M3::M9::M25::M38::M49::M65::M69* v51_0 = v49->add_f_12();
  Message4::M3::M9::M25::M38::M49::M65::M69::M75* v51 = v51_0->mutable_f_4();
  v51->set_f_1(0x8c45e5e19dcb);
  Message4::M3::M9::M25::M38::M49::M65::M69* v51_1 = v49->add_f_12();
  v51_1->set_f_0(0x13);
  v38->set_f_1(0.209125);
  v37->set_f_0(s->substr(0, 3));
  Message4::M7* v58 = message->mutable_f_14();
  Message4::M7::M14* v62 = v58->mutable_f_1();
  Message4::M7::M14::M29* v63 = v62->mutable_f_1();
  Message4::M7::M14::M29::M34* v64 = v63->mutable_f_4();
  v64->set_f_1(0xf9);
  v64->set_f_0(0x5a);
  Message4::M7::M14::M29::M34::M41* v65 = v64->mutable_f_4();
  v65->set_f_0(Message4::M7::M14::M29::M34::M41::E17_CONST_4);
  Message4::M7::M14::M29::M34::M41::M58* v68_0 = v65->add_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66* v68 = v68_0->mutable_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73* v69 = v68->mutable_f_3();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M76* v70 =
      v69->mutable_f_3();
  v70->set_f_0(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M76::E27_CONST_5);
  v69->set_f_0(0x4def5b5c9fc9af9);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77* v72_0 = v69->add_f_4();
  v72_0->set_f_7(0x575929);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v78_0 =
      v72_0->add_f_13();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v79_0 =
      v78_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v80_0 =
      v79_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v81_0 = v80_0->add_f_2();
  v81_0->set_f_0(s->substr(0, 15));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v81 = v81_0->mutable_f_1();
  v81->set_f_0(0.320370);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v82 = v81->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v83 = v82->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v84 = v83->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M91* v86 = v84->mutable_f_2();
  v86->set_f_0(0x26c43b09781243);
  v72_0->set_f_2(s->substr(0, 72));
  v68_0->set_f_2(s->substr(0, 2));
  Message4::M7::M14::M29::M34::M41::M58* v68_1 = v65->add_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66* v87 = v68_1->mutable_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73* v88 = v87->mutable_f_3();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77* v90_0 = v88->add_f_4();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78* v90 =
      v90_0->mutable_f_11();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M78::M81* v91 =
      v90->mutable_f_4();
  v91->set_f_0(s->substr(0, 6));
  v90_0->set_f_1(s->substr(0, 17));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v93_0 =
      v90_0->add_f_13();
  v93_0->set_f_0(0x2c819fe12bf8f4c5);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v94_0 =
      v93_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v95_0 =
      v94_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v96_0 = v95_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v96 = v96_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v97 = v96->mutable_f_2();
  v97->set_f_0(0x420d8a502544df04);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80* v93_1 =
      v90_0->add_f_13();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v103_0 =
      v93_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v104_0 =
      v103_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v105_0 = v104_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v105 = v105_0->mutable_f_1();
  v105->set_f_0(0.781744);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83* v103_1 =
      v93_1->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84* v112_0 =
      v103_1->add_f_2();
  v112_0->set_f_0(s->substr(0, 30));
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86*
      v113_0 = v112_0->add_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::M87*
      v113 = v113_0->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88* v114 = v113->mutable_f_2();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89* v115 = v114->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90* v116 = v115->mutable_f_1();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M80::M83::M84::M86::
      M87::M88::M89::M90::M92* v118_0 = v116->add_f_3();
  v118_0->set_f_0(0x9);
  v93_1->set_f_0(0x3b7562b8);
  v90_0->set_f_6(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::E28_CONST_2);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M79* v119 =
      v90_0->mutable_f_12();
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M79::M82* v121_0 =
      v119->add_f_1();
  v121_0->set_f_0(Message4::M7::M14::M29::M34::M41::M58::M66::M73::M77::M79::
                      M82::E29_CONST_3);
  Message4::M7::M14::M29::M34::M41::M58::M66::M73::M76* v122 =
      v88->mutable_f_3();
  v122->set_f_0(
      Message4::M7::M14::M29::M34::M41::M58::M66::M73::M76::E27_CONST_2);
  v88->set_f_0(0x45cd4c3c0b);
  v68_1->set_f_0(0x2f99415811a0f8a1);
  v64->set_f_2(s->substr(0, 8));
  Message4::M7::M16* v123 = v58->mutable_f_4();
  v123->set_f_0(0x7c4f0f71);
  Message4::M7::M16::M28* v124 = v123->mutable_f_2();
  Message4::M7::M16::M28::M35* v125 = v124->mutable_f_1();
  v125->set_f_2(s->substr(0, 2));
  v125->set_f_5(false);
  Message4::M7::M16::M28::M35::M43* v126 = v125->mutable_f_23();
  v126->set_f_3(0x9c43c9c2fd);
  v125->set_f_6(0x3ec6b812);
  Message4::M7::M16::M28::M35::M40* v131 = v125->mutable_f_21();
  v131->set_f_3(0x5a);
  v131->set_f_4(0.370219);
  v131->set_f_0(s->substr(0, 5));
  v125->set_f_0(0x393ef612fcaa9a);
  v125->set_f_7(s->substr(0, 86));
  Message4::M7::M16::M28::M35::M52* v133_0 = v125->add_f_27();
  Message4::M7::M16::M28::M35::M52::M57* v133 = v133_0->mutable_f_2();
  v133->set_f_0(s->substr(0, 16));
  v133_0->set_f_0(0x5ec88661f201c5df);
  Message4::M7::M16::M28::M35::M51* v134 = v125->mutable_f_26();
  v134->set_f_2(s->substr(0, 9));
  Message4::M7::M15* v136_0 = v58->add_f_3();
  v136_0->set_f_2(0x243fd045);
  v136_0->set_f_4(Message4::M7::M15::E4_CONST_5);
  Message4::M7::M15* v136_1 = v58->add_f_3();
  v136_1->set_f_0(Message4::M7::M15::E3_CONST_4);
  v136_1->set_f_1(0xc31fa72f9d8ab8);
  v136_1->set_f_2(0x2b7cdb6f);
  v136_1->set_f_4(Message4::M7::M15::E4_CONST_2);
  message->set_f_0(0xee5b29);
}

}  // namespace fleetbench::proto
#endif  // THIRD_PARTY_FLEETBENCH_PROTO_ACCESS_MESSAGE4_H_
