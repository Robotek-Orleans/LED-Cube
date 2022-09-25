//
//  base64 encoding and decoding with C++.
//  Version: 2.rc.08 (release candidate) (edited)
//
// Copyright (C) 2004-2017, 2020, 2021 René Nyffenegger
//

#ifndef BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A
#define BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A

#include <string>

std::string base64_encode     (std::string const& s, bool url = false);

std::string base64_decode(std::string const& s, bool remove_linebreaks = false);
std::string base64_encode(unsigned char const*, size_t len, bool url = false);

#endif /* BASE64_H_C0CE2A47_D10E_42C9_A27C_C883944E704A */