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
    Net,    ///< Net type. This is a net connecting a set of ports/pins.
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
        case ObjectType::Net: return "net";
        default: return "unknown";
    }
}

inline ObjectType object_type_from_string(const std::string& object_type_str) {
    if (object_type_str == "cell") return ObjectType::Cell;
    if (object_type_str == "clock") return ObjectType::Clock;
    if (object_type_str == "port") return ObjectType::Port;
    if (object_type_str == "pin") return ObjectType::Pin;
    if (object_type_str == "net") return ObjectType::Net;
    return ObjectType::Unknown;
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

    bool operator!=(const ObjectId& rhs) const {
        return rhs.value != value;
    }

    bool operator<(const ObjectId& rhs) const {
        return value < rhs.value;
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
struct CellObjectId final : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a clock object.
 */
struct ClockObjectId final : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a port object.
 */
struct PortObjectId final : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a pin object.
 */
struct PinObjectId final : ObjectId {
    using ObjectId::ObjectId;
};

/**
 * @brief A strong identifier to a net object.
 */
struct NetObjectId final : ObjectId {
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
struct hash<sdcparse::CellObjectId> {
    std::size_t operator()(const sdcparse::CellObjectId& obj) const noexcept {
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
struct hash<sdcparse::PortObjectId> {
    std::size_t operator()(const sdcparse::PortObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};
template<>
struct hash<sdcparse::PinObjectId> {
    std::size_t operator()(const sdcparse::PinObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};
template<>
struct hash<sdcparse::NetObjectId> {
    std::size_t operator()(const sdcparse::NetObjectId& obj) const noexcept {
        return std::hash<std::string>{}(obj.to_string());
    }
};

} // namespace std
