#pragma once
/**
 * @file
 * @author  Alex Singer
 * @date    January 2026
 * @brief   Declaration of the timing object database used for the TCL-based
 *          SDC parser.
 */

#include <cassert>
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
    std::unordered_map<ObjectId, std::string> object_name;
    /// @brief A mapping between a port object and its direction.
    std::unordered_map<PortObjectId, PortDirection> port_direction_;
    /// @brief A collection of all clock driver objects.
    std::vector<ObjectId> clock_driver_objects_;

    /// @brief A collection of all cell objects.
    std::vector<CellObjectId> cell_objects;
    /// @brief A collection of all clock objects.
    std::vector<ClockObjectId> clock_objects;
    /// @brief A collection of all port objects.
    std::vector<PortObjectId> port_objects;
    /// @brief A collection of all pin objects.
    std::vector<PinObjectId> pin_objects;

  public:
    /**
     * @brief Create a cell object.
     *
     *  @param cell_name
     *      The name of the cell to create. This does not need to be unique.
     *
     *  @return The ID of the created object.
     */
    inline CellObjectId create_cell_object(const std::string& cell_name) {
        CellObjectId cell_object_id = CellObjectId("__vtr_obj_cell_" + std::to_string(cell_objects.size()));
        assert(object_name.count(cell_object_id) == 0);
        object_name[cell_object_id] = cell_name;
        cell_objects.push_back(cell_object_id);
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
    inline ClockObjectId create_clock_object(const std::string& clock_name) {
        ClockObjectId clock_object_id = ClockObjectId("__vtr_obj_clock_" + std::to_string(clock_objects.size()));
        assert(object_name.count(clock_object_id) == 0);
        object_name[clock_object_id] = clock_name;
        clock_objects.push_back(clock_object_id);
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
    inline PortObjectId create_port_object(const std::string& port_name,
                                           PortDirection port_direction,
                                           bool is_clock_driver) {
        assert(port_direction != PortDirection::UNKNOWN);
        PortObjectId port_object_id = PortObjectId("__vtr_obj_port_" + std::to_string(port_objects.size()));
        assert(object_name.count(port_object_id) == 0);
        object_name[port_object_id] = port_name;
        port_direction_[port_object_id] = port_direction;
        port_objects.push_back(port_object_id);

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
    inline PinObjectId create_pin_object(const std::string& pin_name,
                                         bool is_clock_driver) {
        PinObjectId pin_object_id = PinObjectId("__vtr_obj_pin_" + std::to_string(pin_objects.size()));
        assert(object_name.count(pin_object_id) == 0);
        object_name[pin_object_id] = pin_name;
        pin_objects.push_back(pin_object_id);

        if (is_clock_driver) {
            clock_driver_objects_.push_back(pin_object_id);
        }

        return pin_object_id;
    }

    /**
     * @brief Check if the given string represents an object.
     *
     * Since we are using strings to identify objects, we need to prepend all
     * ID strings with a unique substring that will never appear in an actual
     * netlist.
     *
     *  @param object_id
     *      The string that we want to check.
     *
     *  @return True of the given string is an ID, false otherwise.
     */
    static inline bool is_object_id(const std::string& object_id) {
        if (object_id.rfind("__vtr_obj_", 0) == 0)
            return true;
        return false;
    }

    /**
     * @brief The type of object ID that the given string identifies.
     */
    static inline ObjectType get_object_type(const std::string& object_id) {
        if (object_id.rfind("__vtr_obj_cell_", 0) == 0)
            return ObjectType::Cell;
        if (object_id.rfind("__vtr_obj_clock_", 0) == 0)
            return ObjectType::Clock;
        if (object_id.rfind("__vtr_obj_port_", 0) == 0)
            return ObjectType::Port;
        if (object_id.rfind("__vtr_obj_pin_", 0) == 0)
            return ObjectType::Pin;

        return ObjectType::Unknown;
    }

    /**
     * @brief Get the name of the given object.
     */
    inline std::string get_object_name(ObjectId object_id) const {
        auto it = object_name.find(object_id);
        assert(it != object_name.end());
        return it->second;
    }

    /**
     * @brief Get a list of the IDs of all cell objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_cell_objects() const {
        std::vector<std::string> all_cells;
        all_cells.reserve(cell_objects.size());
        for (const CellObjectId& cell_id : cell_objects) {
            all_cells.push_back(cell_id.to_string());
        }
        return all_cells;
    }

    /**
     * @brief Get a list of the IDs of all clock objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_clock_objects() const {
        std::vector<std::string> all_clocks;
        all_clocks.reserve(clock_objects.size());
        for (const ClockObjectId& clock_id : clock_objects) {
            all_clocks.push_back(clock_id.to_string());
        }
        return all_clocks;
    }

    /**
     * @brief Get a list of the IDs of all port objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_port_objects() const {
        std::vector<std::string> all_ports;
        all_ports.reserve(port_objects.size());
        for (const PortObjectId& port_id: port_objects) {
            all_ports.push_back(port_id.to_string());
        }
        return all_ports;
    }

    /**
     * @brief Get a list of the IDs of all input/inout port objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_input_port_objects() const {
        std::vector<std::string> all_inputs;
        all_inputs.reserve(port_objects.size());
        for (const PortObjectId& input: port_objects) {
            auto it = port_direction_.find(input);
            assert(it != port_direction_.end());
            if (it->second == PortDirection::INPUT || it->second == PortDirection::INOUT) {
                all_inputs.push_back(input.to_string());
            }
        }

        return all_inputs;
    }

    /**
     * @brief Get a list of the IDs of all output/inout port objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_output_port_objects() const {
        std::vector<std::string> all_outputs;
        all_outputs.reserve(port_objects.size());
        for (const PortObjectId& output: port_objects) {
            auto it = port_direction_.find(output);
            assert(it != port_direction_.end());
            if (it->second == PortDirection::OUTPUT || it->second == PortDirection::INOUT) {
                all_outputs.push_back(output.to_string());
            }
        }

        return all_outputs;
    }

    /**
     * @brief Get a list of the IDs of all pin objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_pin_objects() const {
        std::vector<std::string> all_pins;
        all_pins.reserve(pin_objects.size());
        for (const PinObjectId& pin_id : pin_objects) {
            all_pins.push_back(pin_id.to_string());
        }
        return all_pins;
    }

    /**
     * @brief Get a list of the IDs of all clock driver objects.
     *
     * This returns a vector of strings to make it more convenient for the
     * parser.
     */
    inline std::vector<std::string> get_clock_driver_objects() const {
        std::vector<std::string> all_clock_drivers;
        all_clock_drivers.reserve(clock_driver_objects_.size());
        for (const ObjectId& object_id : clock_driver_objects_) {
            all_clock_drivers.push_back(object_id.to_string());
        }
        return all_clock_drivers;
    }
};

} // namespace sdcparse
