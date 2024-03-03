//
// Created by csger on 2024. 02. 22..
//

#include <string>
#include "GCodeParser.h"

#include <nlohmann/json.hpp>
#include <set>
#include <iostream>

#include "../utils/helpers.h"

using json = nlohmann::json;

GCodeParser::GCodeParser() {
    this->file = std::ifstream();
    this->gCode = std::deque<std::string>();
    this->currentLine = 0;
    this->errorLine = std::numeric_limits<uint32_t>::max();
    this->parsedGCode = json();
}

void GCodeParser::openFile(const std::string &fileName) {
    if (!this->file.is_open()) {
        this->file.open(fileName);
    }
}

void GCodeParser::closeFile() {
    if (this->file.is_open()) {
        this->file.close();
    }
}

void GCodeParser::parseFile() {
    std::string line;

    while (std::getline(this->file, line)) {
        this->currentLine++;

        if (auto str = this->processLine(line); !str.empty()) {
            this->gCode.push_back(str);
        }

        if (this->errorLine != std::numeric_limits<uint32_t>::max()) {
            std::cout << "Error in line: " << this->errorLine << std::endl;
            this->gCode.clear();
            break;
        }
    }
}

GCodeParser::~GCodeParser() {
    this->closeFile();
}

std::string GCodeParser::processLine(const std::string &line) {
    if (line.empty() || line[0] == ';') {
        return "";
    }

    json data;
    //TODO: Error handling
    if (line[0] == 'M') {
        data["code"] = this->parseMCode(line);
    } else {
        data["code"] = this->parseGCode(line);
    }

    this->parsedGCode.push_back(data);

    return data.dump();
}

json GCodeParser::parseGCode(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");

    switch (std::stoi(codeSlices[0].substr(1))) {
        case 0:
        case 1: {
            return this->parseG0G1(line);
        }
        case 20: {
            return this->parseG20();
        }
        case 21: {
            return this->parseG21();
        }
        case 28: {
            return this->parseG28(line);
        }
        case 90:
        case 91: {
            return this->parseG90G91(line);
        }
        case 92: {
            return this->parseG92(line);
        }

        default: {
            this->errorLine = this->currentLine;
            return {};
        }
    }
}

json GCodeParser::parseMCode(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");

    switch (std::stoi(codeSlices[0].substr(1))) {
        case 82: {
            return this->parseM82(line);
        }
        case 84: {
            return this->parseM84(line);
        }
        case 104: {
            return this->parseM104(line);
        }
        case 109: {
            return this->parseM109(line);
        }
        case 140: {
            return this->parseM140(line);
        }
        case 190: {
            return this->parseM190(line);
        }
        case 107: {
            return this->parseM107(line);
        }
        case 106: {
            return this->parseM106(line);
        }
        default: {
            this->errorLine = this->currentLine;
            return {};
        }
    }
}

std::string GCodeParser::getGCode() {
    if (!this->gCode.empty()) {
        std::string code = this->gCode.front();
        this->gCode.pop_front();
        return code;
    }

    return "";
}

int64_t GCodeParser::parseNumber(const std::string &value) const {
    float number = std::stof(value);

    if (this->isFreedomUnits) {
        number *= GCodeParser::freedomUnitMultiplier;
    }

    return static_cast<int64_t>(number * 100000);
}

json GCodeParser::parseG0G1(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    uint8_t code = std::stoi(codeSlices[0].substr(1));
    json data = json();

    data["id"] = code;

    std::set<char> keys = {'x', 'y', 'z', 'e', 'f'};

    if (codeSlices.size() == 1) {
        this->errorLine = this->currentLine;
        return data;
    }

    for (int i = 1; i < codeSlices.size(); ++i) {
        auto slice = codeSlices[i];

        if (slice.find(';') != std::string::npos) {
            slice = slice.substr(0, slice.find(';'));
        }

        if (slice.empty()) {
            break;
        }

        std::string key = slice.substr(0, 1);
        Helpers::toLower(key);

        auto value = slice.substr(1);

        if (!(Helpers::is_number(value))) {
            this->errorLine = this->currentLine;
            return data;
        }


        if (keys.find(key[0]) != keys.end()) {
            keys.erase(key[0]);
        } else {
            this->errorLine = this->currentLine;
            return data;
        }

        data[key] = parseNumber(value);
    }

    for (auto key: keys) {
        data[std::string(1, key)] = "0";
    }

    return data;
}

json GCodeParser::parseM107(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    if (codeSlices.size() > 1) {
        this->errorLine = this->currentLine;
        return data;
    }

    data["id"] = 107;

    return data;
}

json GCodeParser::parseM190(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 190;

    if (codeSlices[1].find(';') != std::string::npos) {
        codeSlices[1] = codeSlices[1].substr(0, codeSlices[1].find(';'));
    }

    if (!codeSlices[1].empty()) {
        data["temperature"] = codeSlices[1].substr(1);
    } else {
        this->errorLine = this->currentLine;
    }

    return data;
}

json GCodeParser::parseM104(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 104;

    if (codeSlices[1].find(';') != std::string::npos) {
        codeSlices[1] = codeSlices[1].substr(0, codeSlices[1].find(';'));
    }

    if (!codeSlices[1].empty()) {
        data["temperature"] = codeSlices[1].substr(1);
    } else {
        this->errorLine = this->currentLine;
    }

    return data;
}

json GCodeParser::parseG28(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 28;

    std::set<char> keys = {'x', 'y', 'z'};

    for (int i = 1; i < codeSlices.size(); ++i) {
        auto slice = codeSlices[i];

        if (slice.find(';') != std::string::npos) {
            slice = slice.substr(0, slice.find(';'));
        }

        if (slice.empty()) {
            break;
        }

        std::string key = slice.substr(0, 1);

        if (key != slice) {
            this->errorLine = this->currentLine;
            return data;
        }

        auto value = true;

        Helpers::toLower(key);

        if (keys.find(key[0]) != keys.end()) {
            keys.erase(key[0]);
        } else {
            this->errorLine = this->currentLine;
            return data;
        }

        data[key] = value;
    }

    for (auto key: keys) {
        data[std::string(1, key)] = codeSlices.size() == 1;
    }

    return data;
}

json GCodeParser::parseM109(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 109;

    if (codeSlices[1].find(';') != std::string::npos) {
        codeSlices[1] = codeSlices[1].substr(0, codeSlices[1].find(';'));
    }

    if (!codeSlices[1].empty()) {
        data["temperature"] = codeSlices[1].substr(1);
    } else {
        this->errorLine = this->currentLine;
    }

    return data;
}

json GCodeParser::parseG90G91(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    if (codeSlices.size() > 1 && codeSlices[1][0] != ';') {
        this->errorLine = this->currentLine;
        return data;
    }

    data["id"] = codeSlices[0].substr(1) == "90" ? 90 : 91;

    return data;
}

json GCodeParser::parseG92(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 92;

    std::set<char> keys = {'x', 'y', 'z', 'e'};

    if (codeSlices.size() == 1) {
        this->errorLine = this->currentLine;
        return data;
    }

    for (int i = 1; i < codeSlices.size(); ++i) {
        auto slice = codeSlices[i];

        if (slice.find(';') != std::string::npos) {
            slice = slice.substr(0, slice.find(';'));
        }

        if (slice.empty()) {
            break;
        }

        std::string key = slice.substr(0, 1);
        auto value = slice.substr(1);

        if (value.empty() || !Helpers::is_number(value)) {
            this->errorLine = this->currentLine;
            return data;
        }

        Helpers::toLower(key);

        if (keys.find(key[0]) != keys.end()) {
            keys.erase(key[0]);
        } else {
            this->errorLine = this->currentLine;
            return data;
        }

        data[key] = parseNumber(value);
    }

    for (auto key: keys) {
        data[std::string(1, key)] = "0";
    }

    return data;
}

json GCodeParser::parseM106(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 106;

    if (codeSlices.size() == 1) {
        this->errorLine = this->currentLine;
        return data;
    }

    if (codeSlices[1].find(';') != std::string::npos) {
        codeSlices[1] = codeSlices[1].substr(0, codeSlices[1].find(';'));
    }

    if (codeSlices[1].empty()) {
        this->errorLine = this->currentLine;
    } else {
        data["fan"] = codeSlices[1].substr(1);
    }

    return data;
}

json GCodeParser::parseM82(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    if (codeSlices.size() > 1 && codeSlices[1][0] != ';'){
        this->errorLine = this->currentLine;
        return data;
    }

    data["id"] = 82;

    return data;
}

json GCodeParser::parseM84(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    if (codeSlices.size() > 1 && codeSlices[1][0] != ';') {
        this->errorLine = this->currentLine;
        return data;
    }

    data["id"] = 84;

    return data;
}

json GCodeParser::parseG20() {
    this->isFreedomUnits = true;
    return {};
}

json GCodeParser::parseG21() {
    this->isFreedomUnits = false;
    return {};
}

std::string GCodeParser::getGCodeDump() {
    return this->parsedGCode.dump();
}

void GCodeParser::jsonToGCode(const std::string &json) {
    this->parsedGCode = nlohmann::json::parse(json);

}

json GCodeParser::parseM140(const std::string &line) {
    auto codeSlices = Helpers::split(line, " ");
    json data = json();

    data["id"] = 140;

    if (codeSlices[1].find(';') != std::string::npos) {
        codeSlices[1] = codeSlices[1].substr(0, codeSlices[1].find(';'));
    }

    if (!codeSlices[1].empty()) {
        data["temperature"] = codeSlices[1].substr(1);
    } else {
        this->errorLine = this->currentLine;
    }

    return data;
}
