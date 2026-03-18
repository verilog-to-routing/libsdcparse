#pragma once
/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   Declaration of the timing object database used for the TCL-based
 *          SDC parser.
 */

#include <cassert>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "sdc_timing_object.h"

namespace sdcparse {

/**
 * @brief The direction of a port in the netlist.
 */
enum class PortDirection {
    INPUT,  ///< Input port.
    OUTPUT, ///< Output port.
    INOUT,  ///< Inout port.
    UNKNOWN ///< Unknown port. This should never happen.
};

/**
 * @brief Convert a string into a port direction.
 */
inline PortDirection from_string_to_port_dir(const std::string& port_type) {
    if (port_type == "INPUT")
        return PortDirection::INPUT;
    if (port_type == "OUTPUT")
        return PortDirection::OUTPUT;
    if (port_type == "INOUT")
        return PortDirection::INOUT;
    return PortDirection::UNKNOWN;
}

/**
 * @brief Database of the timing objects in the netlist.
 *
 * This is used to maintain an internal representation of the netlist that is
 * necessary for parsing SDC files. This includes timing objects such as clocks,
 * ports, and pins which are needed to perform search operations on their names.
 */
class TimingObjectDatabase {
  private:
    /// @brief A mapping between an object ID and its name.
    std::vector<std::string> object_name_;
    /// @brief A mapping between an object ID and its type.
    std::vector<ObjectType> object_type_;
    /// @brief A mapping between a port object and its direction.
    std::unordered_map<ObjectId, PortDirection> port_direction_;
    /// @brief A collection of all clock driver objects.
    std::vector<ObjectId> clock_driver_objects_;

    /// @brief A collection of all objects that have been created.
    ///        This is used to create the IDs.
    std::vector<ObjectId> all_objects_;

    /// @brief A collection of all cell objects.
    std::vector<ObjectId> cell_objects_;
    /// @brief A collection of all clock objects.
    std::vector<ObjectId> clock_objects_;
    /// @brief A collection of all port objects.
    std::vector<ObjectId> port_objects_;
    /// @brief A collection of all input port objects.
    std::vector<ObjectId> input_port_objects_;
    /// @brief A collection of all output port objects.
    std::vector<ObjectId> output_port_objects_;
    /// @brief A collection of all pin objects.
    std::vector<ObjectId> pin_objects_;
    /// @brief A collection of all net objects.
    std::vector<ObjectId> net_objects_;

  public:
    /**
     * @brief Create a cell object.
     *
     *  @param cell_name
     *      The name of the cell to create. This does not need to be unique.
     *
     *  @return The ID of the created object.
     */
    inline ObjectId create_cell_object(const std::string& cell_name) {
        ObjectId cell_object_id = ObjectId(all_objects_.size());
        all_objects_.push_back(cell_object_id);
        object_name_.push_back(cell_name);
        object_type_.push_back(ObjectType::Cell);
        cell_objects_.push_back(cell_object_id);
        return cell_object_id;
    }

    /**
     * @brief Create a clock object.
     *
     *  @param clock_name
     *      The name of the clock to create. This does not need to be unique.
     *
     *  @return The ID of the created object.
     */
    inline ObjectId create_clock_object(const std::string& clock_name) {
        ObjectId clock_object_id = ObjectId(all_objects_.size());
        all_objects_.push_back(clock_object_id);
        object_name_.push_back(clock_name);
        object_type_.push_back(ObjectType::Clock);
        clock_objects_.push_back(clock_object_id);
        return clock_object_id;
    }

    /**
     * @brief Create a port object.
     *
     *  @param port_name
     *      The name of the port to create. This does not need to be unique.
     *  @param port_direction
     *      The direction of the port (i.e. input, output, or inout).
     *  @param is_clock_driver
     *      If this port is the driver of a clock in the netlist.
     *
     *  @return The ID of the created object.
     */
    inline ObjectId create_port_object(const std::string& port_name,
                                       PortDirection port_direction,
                                       bool is_clock_driver) {
        if (port_direction == PortDirection::UNKNOWN) {
            throw std::invalid_argument("LibSDCParse: invalid port direction");
        }
        ObjectId port_object_id = ObjectId(all_objects_.size());
        all_objects_.push_back(port_object_id);
        object_name_.push_back(port_name);
        object_type_.push_back(ObjectType::Port);
        port_direction_[port_object_id] = port_direction;
        port_objects_.push_back(port_object_id);
        if (port_direction == PortDirection::INPUT || port_direction == PortDirection::INOUT)
            input_port_objects_.push_back(port_object_id);
        if (port_direction == PortDirection::OUTPUT || port_direction == PortDirection::INOUT)
            output_port_objects_.push_back(port_object_id);

        if (is_clock_driver) {
            clock_driver_objects_.push_back(port_object_id);
        }

        return port_object_id;
    }

    /**
     * @brief Create a pin object.
     *
     *  @param pin_name
     *      The name of the pin to create. This does not need to be unique.
     *  @param is_clock_driver
     *      If this port is the driver of a clock in the netlist.
     *
     *  @return The ID of the created object.
     */
    inline ObjectId create_pin_object(const std::string& pin_name,
                                      bool is_clock_driver) {
        ObjectId pin_object_id = ObjectId(all_objects_.size());
        all_objects_.push_back(pin_object_id);
        object_name_.push_back(pin_name);
        object_type_.push_back(ObjectType::Pin);
        pin_objects_.push_back(pin_object_id);

        if (is_clock_driver) {
            clock_driver_objects_.push_back(pin_object_id);
        }

        return pin_object_id;
    }

    /**
     * @brief Create a net object.
     *
     *  @param net_name
     *      The name of the net to create. This does not need to be unique.
     *
     *  @return The ID of the created object.
     */
    inline ObjectId create_net_object(const std::string& net_name) {
        ObjectId net_object_id = ObjectId(all_objects_.size());
        all_objects_.push_back(net_object_id);
        object_name_.push_back(net_name);
        object_type_.push_back(ObjectType::Net);
        net_objects_.push_back(net_object_id);

        return net_object_id;
    }

    /**
     * @brief Get the type of the given object ID.
     */
    inline ObjectType get_object_type(ObjectId object_id) {
        size_t id_val = static_cast<size_t>(object_id);
        if (!object_id.is_valid() || id_val >= object_type_.size()) {
            throw std::out_of_range("LibSDCParse: invalid object ID");
        }
        return object_type_[id_val];
    }

    /**
     * @brief Get the name of the given object.
     */
    const std::string& get_object_name(ObjectId object_id) const {
        size_t id_val = static_cast<size_t>(object_id);
        if (!object_id.is_valid() || id_val >= object_name_.size()) {
            throw std::out_of_range("LibSDCParse: invalid object ID");
        }
        return object_name_[id_val];
    }

    /**
     * @brief Get a list of the IDs of all cell objects.
     */
    inline const std::vector<ObjectId>& get_cell_objects() const {
        return cell_objects_;
    }

    /**
     * @brief Get a list of the IDs of all clock objects.
     */
    inline const std::vector<ObjectId>& get_clock_objects() const {
        return clock_objects_;
    }

    /**
     * @brief Get a list of the IDs of all port objects.
     */
    inline const std::vector<ObjectId>& get_port_objects() const {
        return port_objects_;
    }

    /**
     * @brief Get a list of the IDs of all input/inout port objects.
     */
    inline const std::vector<ObjectId>& get_input_port_objects() const {
        return input_port_objects_;
    }

    /**
     * @brief Get a list of the IDs of all output/inout port objects.
     */
    inline const std::vector<ObjectId>& get_output_port_objects() const {
        return output_port_objects_;
    }

    /**
     * @brief Get a list of the IDs of all pin objects.
     */
    inline const std::vector<ObjectId>& get_pin_objects() const {
        return pin_objects_;
    }

    /**
     * @brief Get a list of the IDs of all net objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline const std::vector<ObjectId>& get_net_objects() const {
        return net_objects_;
    }

    /**
     * @brief Get a list of the IDs of all clock driver objects.
     */
    inline const std::vector<ObjectId>& get_clock_driver_objects() const {
        return clock_driver_objects_;
    }

    /**
     * @brief Query the object database for objects of the given types with
     *        names matching the given patterns.
     *
     *  @param patterns             A list of patterns to search for.
     *  @param is_case_insensitive  If true, performs case-insensitive matching.
     *  @param is_regex             Set to true to use a regex pattern matcher, false to use a glob pattern matcher.
     *  @param object_types         The types of objects to match for.
     *
     *  @return A list of object IDs that match the query.
     */
    std::vector<ObjectId> query_pattern_match(const std::vector<std::string>& patterns,
                                              bool is_case_insensitive,
                                              bool is_regex,
                                              const std::vector<ObjectType>& object_types);
};

} // namespace sdcparse
