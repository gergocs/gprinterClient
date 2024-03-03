//
// Created by csger on 2024. 02. 22..
//

#ifndef GPRINTERCLIENT_GCODEPARSER_H
#define GPRINTERCLIENT_GCODEPARSER_H

#include <fstream>
#include <deque>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GCodeParser {
public:
    GCodeParser();

    void openFile(const std::string &fileName);

    void jsonToGCode(const std::string& json);

    void closeFile();

    void parseFile();

    std::string getGCode();

    std::string getGCodeDump();

    virtual ~GCodeParser();

private:
    std::string processLine(const std::string &line);

    json parseGCode(const std::string &line);
    json parseMCode(const std::string &line);

    json parseG0G1(const std::string &line);
    json parseG20();
    json parseG21();
    json parseG28(const std::string &line);
    json parseG90G91(const std::string &line);
    json parseG92(const std::string &line);


    json parseM82(const std::string &line);
    json parseM84(const std::string &line);
    json parseM104(const std::string &line);
    json parseM106(const std::string &line);
    json parseM107(const std::string &line);
    json parseM109(const std::string &line);
    json parseM140(const std::string &line);
    json parseM190(const std::string &line);

    [[nodiscard]] int64_t parseNumber(const std::string& value) const;

    std::ifstream file;
    std::deque<std::string> gCode;
    json parsedGCode;
    uint32_t currentLine;
    uint32_t errorLine;
    bool isFreedomUnits = false;
    constexpr static double freedomUnitMultiplier = 25.4;
};


#endif //GPRINTERCLIENT_GCODEPARSER_H
