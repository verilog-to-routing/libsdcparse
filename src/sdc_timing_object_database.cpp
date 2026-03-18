#include "sdc_timing_object_database.h"

#include <regex>

/**
 * @brief Helper function to convert a glob pattern into a regex pattern.
 * 
 * NOTE: This function currently does not support character classes, instead
 *       it just escapes square brackets to support buses correctly.
 * 
 * TODO: Should support character classes without breaking buses.
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

std::vector<ObjectId> TimingObjectDatabase::query_pattern_match(const std::vector<std::string>& patterns,
                                                                bool is_case_insensitive,
                                                                bool is_regex,
                                                                const std::vector<ObjectType>& object_types) {

    // Pre-process the patterns.
    std::vector<std::regex> regex_patterns;
    for (const std::string& raw_pattern : patterns) {
        std::string pattern;
        if (is_regex) {
            pattern = raw_pattern;
        } else {
            // TODO: A hand-rolled glob pattern matcher would likely be substantially faster.
            //       Should investigate.
            pattern = glob_to_regex(raw_pattern);
        }
        try {
            if (is_case_insensitive) {
                regex_patterns.emplace_back(pattern, std::regex::icase|std::regex::optimize);
            } else {
                regex_patterns.emplace_back(pattern, std::regex::optimize);
            }
        } catch (const std::regex_error& e) {
            throw std::runtime_error("LibSDCParse: invalid pattern '" + raw_pattern + "': " + e.what());
        }
    }

    std::vector<ObjectId> matches;

    // Lambda to check an object against all patterns
    auto check_object = [&matches, &regex_patterns](ObjectId object_id, const std::string& object_name) {
        for (const std::regex& pattern : regex_patterns) {
            if (std::regex_match(object_name, pattern)) {
                matches.push_back(object_id);
                return; // Early exit once we find a match
            }
        }
    };

    for (ObjectType target_object_type : object_types) {
        switch (target_object_type) {
            case ObjectType::Cell:
                for (ObjectId cell_object_id : cell_objects_) {
                    check_object(cell_object_id, get_object_name(cell_object_id));
                }
                break;
            case ObjectType::Clock:
                for (ObjectId clock_object_id : clock_objects_) {
                    check_object(clock_object_id, get_object_name(clock_object_id));
                }
                break;
            case ObjectType::Net:
                for (ObjectId net_object_id : net_objects_) {
                    check_object(net_object_id, get_object_name(net_object_id));
                }
                break;
            case ObjectType::Port:
                for (ObjectId port_object_id : port_objects_) {
                    check_object(port_object_id, get_object_name(port_object_id));
                }
                break;
            case ObjectType::Pin:
                for (ObjectId pin_object_id : pin_objects_) {
                    check_object(pin_object_id, get_object_name(pin_object_id));
                }
                break;
            default:
                throw std::runtime_error("LibSDCParse: unexpected object type in query.");
                break;
        }
    }

    return matches;
}

} // namespace sdcparse