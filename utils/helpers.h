//
// Created by csger on 2024. 02. 22..
//

#ifndef GPRINTERCLIENT_HELPERS_H
#define GPRINTERCLIENT_HELPERS_H

#include <string>
#include <vector>

class Helpers {
public:
    static std::vector<std::string> split(const std::string &str, const std::string &delim) {
        std::vector<std::string> tokens;
        size_t prev = 0;
        size_t pos;
        do {
            if (pos = str.find(delim, prev); pos == std::string::npos) {
                pos = str.length();
            }

            if (std::string token = str.substr(prev, pos - prev); !token.empty()) {
                tokens.push_back(token);
            }

            prev = pos + delim.length();
        } while (pos < str.length() && prev < str.length());

        return tokens;
    }

    static void toLower(std::string &str) {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c) { return std::tolower(c); });
    }

    static bool is_number(const std::string &s) {
        return !s.empty() && std::find_if(s.begin(),
                                          s.end(),
                                          [](unsigned char c) { return !(std::isdigit(c) || c != '-' || c != '.'); }) ==
                             s.end();
    }
};

#endif //GPRINTERCLIENT_HELPERS_H
