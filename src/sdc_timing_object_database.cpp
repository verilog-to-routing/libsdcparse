#include "sdc_timing_object_database.h"

/**
 * @brief Helper function to convert a glob pattern into a regex pattern.
 * 
 *  @param glob The glob pattern to convert.
 *
 *  @return The equivalent regex pattern.
 */
static inline std::string glob_to_regex(const std::string& glob) {
    std::string res = "^"; // Ensure we match the whole string
    for (char c : glob) {
        switch (c) {
            case '*':  res += ".*";  break;
            case '?':  res += ".";   break;
            // Escape regex special characters
            case '.': case '+': case '(': case ')': 
            case '[': case ']': case '{': case '}':
            case '^': case '$': case '|': case '\\':
                res += '\\';
                res += c;
                break;
            default:   res += c;     break;
        }
    }
    res += "$"; // Ensure we match until the end
    return res;
}

namespace sdcparse {

std::vector<std::string> TimingObjectDatabase::query_pattern_match(const std::vector<std::string>& patterns,
                                                                   bool nocase,
                                                                   bool regexp,
                                                                   const std::vector<ObjectType>& object_types) {

    // Pre-process the patterns.
    std::vector<std::regex> regex_patterns;
    for (const std::string& raw_pattern : patterns) {
        std::string pattern;
        if (regexp) {
            pattern = raw_pattern;
        } else {
            pattern = glob_to_regex(raw_pattern);
        }
        if (nocase) {
            regex_patterns.push_back(std::regex(pattern, std::regex::icase|std::regex::optimize));
        } else {
            regex_patterns.push_back(std::regex(pattern, std::regex::optimize));
        }
    }

    std::vector<std::string> matches;

    // Lambda to check an object against all patterns
    auto check_object = [&matches, &regex_patterns](const ObjectId& object_id, const std::string& object_name) {
        for (const std::regex& pattern : regex_patterns) {
            if (std::regex_match(object_name, pattern)) {
                matches.push_back(object_id.to_string());
                return; // Early exit once we find a match
            }
        }
    };

    for (ObjectType target_object_type : object_types) {
        switch (target_object_type) {
            case ObjectType::Cell:
                for (const CellObjectId& cell_object_id : cell_objects) {
                    check_object(cell_object_id, get_object_name(cell_object_id));
                }
                break;
            case ObjectType::Clock:
                for (const ClockObjectId& clock_object_id : clock_objects) {
                    check_object(clock_object_id, get_object_name(clock_object_id));
                }
                break;
            case ObjectType::Net:
                for (const NetObjectId& net_object_id : net_objects) {
                    check_object(net_object_id, get_object_name(net_object_id));
                }
                break;
            case ObjectType::Port:
                for (const PortObjectId& port_object_id : port_objects) {
                    check_object(port_object_id, get_object_name(port_object_id));
                }
                break;
            case ObjectType::Pin:
                for (const PinObjectId& pin_object_id : pin_objects) {
                    check_object(pin_object_id, get_object_name(pin_object_id));
                }
                break;
            default:
                assert(false);
                break;
        }
    }

    return matches;
}

} // namespace sdcparse