#pragma once
/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   Declaration of the timing object used by the TCL-based SDC parser.
 */

#include <functional>
#include <string>

namespace sdcparse {

/**
 * @brief The type of object that is being referenced.
 */
enum class ObjectType {
    Cell,   ///< Cell type.
    Clock,  ///< Clock type. This is created by commands such as create_clock.
    Port,   ///< Port type. This is a top-level pin.
    Pin,    ///< Pin type. This is a pin on a cell.
    Unknown ///< Unknown type. Should never see this.
};

/**
 * @brief Converts the given object type to a string.
 */
inline std::string to_string(ObjectType object_type) {
    switch (object_type) {
        case ObjectType::Cell: return "cell";
        case ObjectType::Clock: return "clock";
        case ObjectType::Port: return "port";
        case ObjectType::Pin: return "pin";
        default: return "unknown";
    }
}

/**
 * @brief A strong identifier to an object.
 *
 * Objects are uniquely identified by a string of the format:
 *      __vtr_obj_<type>_<id>
 */
struct ObjectId {
    /**
     * @brief Constructor using a string.
     *
     * This constructor is explicit to make this a strong id.
     */
    explicit ObjectId(const std::string& v) noexcept
        : value(v) {}

    bool operator==(const ObjectId& rhs) const {
        return rhs.value == value;
    }

    std::string to_string() const {
        return value;
    }

    // This is used by the std hash function which uses the private members.
    friend std::hash<ObjectId>;

  private:
    /// @brief The internal string that is used by this ID.
    // TODO: Consider changing this to be an integer and a type.
    std::string value;
};

/**
 * @brief A strong identifier to a cell object.
 */
struct CellObjectId : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a clock object.
 */
struct ClockObjectId : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a port object.
 */
struct PortObjectId : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a pin object.
 */
struct PinObjectId : ObjectId {
    using ObjectId::ObjectId;
};

} // namespace sdcparse

// The following are hash functions for the strong IDs above to allow them to
// be used within std::unordered_map.
namespace std {

template<>
struct hash<sdcparse::ObjectId> {
    std::size_t operator()(const sdcparse::ObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};
template<>
struct hash<sdcparse::PortObjectId> {
    std::size_t operator()(const sdcparse::PortObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};
template<>
struct hash<sdcparse::ClockObjectId> {
    std::size_t operator()(const sdcparse::ClockObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};
template<>
struct hash<sdcparse::PinObjectId> {
    std::size_t operator()(const sdcparse::PinObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};

} // namespace std
