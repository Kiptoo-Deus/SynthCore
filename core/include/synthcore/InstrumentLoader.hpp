#pragma once

#include "synthcore/SampleMap.hpp"
#include <string>

namespace synthcore {

class InstrumentLoader {
public:
    static bool loadFromDirectory(const std::string& dir, 
                                  const std::string& mapFile, 
                                  SampleMap& outMap);

private:
    static std::string combinePath(const std::string& dir, const std::string& filename);
    static bool parseMappingLine(const std::string& line, 
                                 std::string& filename,
                                 uint8_t& rootNote,
                                 uint8_t& loNote,
                                 uint8_t& hiNote,
                                 uint8_t& loVel,
                                 uint8_t& hiVel,
                                 int& rrGroup);
};

} // namespace synthcore
