#include "py_pathing_maps.h"


std::vector<float> GetMapBoundaries() {
    GW::MapContext* m = GW::GetMapContext();
    std::vector<float> boundaries;
    for (int i = 0; i < 5; i++) {
        boundaries.push_back(m->map_boundaries[i]);
    }
    return boundaries;
};

struct PathingTrapezoid {
    uint32_t id;
    std::vector<uint32_t> neighbor_ids; // Indices of adjacent trapezoids
    uint16_t portal_left;
    uint16_t portal_right;
    float XTL;
    float XTR;
    float YT;
    float XBL;
    float XBR;
    float YB;

    PathingTrapezoid()
        : id(0), portal_left(0xFFFF), portal_right(0xFFFF),
        XTL(0), XTR(0), YT(0), XBL(0), XBR(0), YB(0) {
    }
};

struct PathingMap {
    uint32_t zplane;
    uint32_t h0004;
    uint32_t h0008;
    uint32_t h000C;
    uint32_t h0010;
    std::vector<PathingTrapezoid> trapezoids;
    std::vector<SinkNode> sink_nodes;
    std::vector<XNode> x_nodes;
    std::vector<YNode> y_nodes;
    uint32_t h0034;
    uint32_t h0038;
    std::vector<Portal> portals;
    uint32_t root_node_id;

    PathingMap()
        : zplane(UINT32_MAX), h0004(0), h0008(0), h000C(0), h0010(0), h0034(0), h0038(0), root_node_id(UINT32_MAX) {
    }
};





PathingTrapezoid ConvertTrapezoid(const GW::PathingTrapezoid& original) {
    PathingTrapezoid trapezoid;
    trapezoid.id = original.id;
    trapezoid.XTL = original.XTL;
    trapezoid.XTR = original.XTR;
    trapezoid.YT = original.YT;
    trapezoid.XBL = original.XBL;
    trapezoid.XBR = original.XBR;
    trapezoid.YB = original.YB;
    trapezoid.portal_left = original.portal_left;
    trapezoid.portal_right = original.portal_right;

    // Map adjacent trapezoids by their IDs
    for (int i = 0; i < 4; ++i) {
        if (original.adjacent[i]) {
            trapezoid.neighbor_ids.push_back(original.adjacent[i]->id);
        }
    }

    return trapezoid;
}



PathingMap ConvertPathingMap(const GW::PathingMap& original) {
    PathingMap pathing_map;
    pathing_map.zplane = original.zplane;

    // Convert trapezoids
    for (uint32_t i = 0; i < original.trapezoid_count; ++i) {
        pathing_map.trapezoids.push_back(ConvertTrapezoid(original.trapezoids[i]));
    }

    // Convert SinkNodes
    /*
    for (uint32_t i = 0; i < original.sink_node_count; ++i) {
        SinkNode sink_node;
        sink_node.id = original.sink_nodes[i].id;

        // Safely iterate through the null-terminated trapezoid list
        if (original.sink_nodes[i].trapezoid) { // Ensure the trapezoid list exists
            const GW::PathingTrapezoid* const* trapezoid_ptr = original.sink_nodes[i].trapezoid;

            // Iterate until the null-terminator is reached
            while (*trapezoid_ptr != nullptr) {
                sink_node.trapezoid_ids.push_back((*trapezoid_ptr)->id);
                ++trapezoid_ptr; // Move to the next pointer in the array
            }
        }

        // Add the converted SinkNode to the destination map
        pathing_map.sink_nodes.push_back(sink_node);
    }
    */

    // Convert XNodes
    for (uint32_t i = 0; i < original.x_node_count; ++i) {
        XNode x_node;
        x_node.id = original.x_nodes[i].id;
        x_node.pos = { original.x_nodes[i].pos.x, original.x_nodes[i].pos.y };
        x_node.dir = { original.x_nodes[i].dir.x, original.x_nodes[i].dir.y };
        if (original.x_nodes[i].left) x_node.left_id = original.x_nodes[i].left->id;
        if (original.x_nodes[i].right) x_node.right_id = original.x_nodes[i].right->id;

        pathing_map.x_nodes.push_back(x_node);
    }

    // Convert YNodes
    for (uint32_t i = 0; i < original.y_node_count; ++i) {
        YNode y_node;
        y_node.id = original.y_nodes[i].id;
        y_node.pos = { original.y_nodes[i].pos.x, original.y_nodes[i].pos.y };
        if (original.y_nodes[i].left) y_node.left_id = original.y_nodes[i].left->id;
        if (original.y_nodes[i].right) y_node.right_id = original.y_nodes[i].right->id;

        pathing_map.y_nodes.push_back(y_node);
    }

    // Convert Portals
    for (uint32_t i = 0; i < original.portal_count; ++i) {
        Portal portal;
        portal.left_layer_id = original.portals[i].left_layer_id;
        portal.right_layer_id = original.portals[i].right_layer_id;
        portal.h0004 = original.portals[i].h0004;

        // Map trapezoids connected by the portal
        for (uint32_t j = 0; j < original.portals[i].count; ++j) {
            portal.trapezoid_indices.push_back(original.portals[i].trapezoids[j]->id);
        }

        // Determine the pair index manually
        portal.pair_index = UINT32_MAX; // Default to invalid
        if (original.portals[i].pair) {
            for (uint32_t k = 0; k < original.portal_count; ++k) {
                if (&original.portals[k] == original.portals[i].pair) {
                    portal.pair_index = k;
                    break;
                }
            }
        }

        pathing_map.portals.push_back(portal);
    }

    return pathing_map;
}


std::vector<PathingMap> GetPathingMaps() {
    const auto* pathing_map_array = GW::Map::GetPathingMap();

    std::vector<PathingMap> converted_maps;

    if (pathing_map_array) {
        for (size_t i = 0; i < pathing_map_array->size(); ++i) {
            converted_maps.push_back(ConvertPathingMap((*pathing_map_array)[i]));
        }
    }

    return converted_maps;
}

namespace GW {

    typedef void(__cdecl* FindPath_pt)(PathPoint* start, PathPoint* goal, float range, uint32_t maxCount, uint32_t* count, PathPoint* pathArray);
    static FindPath_pt FindPath_Func = nullptr;

    static void InitPathfinding() {
        FindPath_Func = (FindPath_pt)Scanner::Find("\x8b\x70\x14\xd9", "xxxx", -0x14);

        Logger::AssertAddress("FindPath_Func", (uintptr_t)FindPath_Func);
    }

}

class PathPlanner {
public:
	PathPlanner() {
		// Initialize the pathfinding function
		GW::InitPathfinding();
	}

    void PlanPath(float start_x, float start_y, float start_z,
        float goal_x, float goal_y, float goal_z) {
        planned_path_ready = false;

        if (!GW::FindPath_Func) {
            GW::InitPathfinding();
			planned_path.clear();
			planned_path_ready = true;
			return;
		}

        GW::PathPoint pathArray[30];
        uint32_t pathCount = 30;

        GW::GamePos game_pos(start_x, start_y, start_z);
        GW::PathPoint start{ game_pos, nullptr };

        GW::GamePos goal(goal_x, goal_y, goal_z);
        GW::PathPoint goal_point{ goal, nullptr };

        GW::FindPath_Func(&start, &goal_point, 4500.0f, pathCount, &pathCount, pathArray);

        planned_path.clear();
        for (uint32_t i = 0; i < pathCount; ++i) {
            const GW::GamePos& p = pathArray[i].pos;
            planned_path.emplace_back(p.x, p.y, p.zplane);
        }

        planned_path_ready = true;

    }

    bool IsReady() const {
        return planned_path_ready;
    }

    const std::vector<std::tuple<float, float, float>>& GetPath() const {
        return planned_path;
    }

private:
    std::vector<std::tuple<float, float, float>> planned_path;
    bool planned_path_ready = false;
};






PYBIND11_EMBEDDED_MODULE(PyPathing, m) {

    py::class_<PathPlanner>(m, "PathPlanner")
        .def(py::init<>())  // Default constructor
        .def("plan", &PathPlanner::PlanPath,
            py::arg("start_x"), py::arg("start_y"), py::arg("start_z"),
            py::arg("goal_x"), py::arg("goal_y"), py::arg("goal_z"),
            "Submit a path planning task to the game thread")
        .def("is_ready", &PathPlanner::IsReady,
            "Check if the planned path is ready")
        .def("get_path", &PathPlanner::GetPath,
            "Retrieve the calculated path as list of (x, y, z) tuples");


    // Bind PathingTrapezoid
    py::class_<PathingTrapezoid>(m, "PathingTrapezoid")
        .def(py::init<>())  // Default constructor
        .def_readwrite("id", &PathingTrapezoid::id)
        .def_readwrite("XTL", &PathingTrapezoid::XTL)
        .def_readwrite("XTR", &PathingTrapezoid::XTR)
        .def_readwrite("YT", &PathingTrapezoid::YT)
        .def_readwrite("XBL", &PathingTrapezoid::XBL)
        .def_readwrite("XBR", &PathingTrapezoid::XBR)
        .def_readwrite("YB", &PathingTrapezoid::YB)
        .def_readwrite("portal_left", &PathingTrapezoid::portal_left)
        .def_readwrite("portal_right", &PathingTrapezoid::portal_right)
        .def_readwrite("neighbor_ids", &PathingTrapezoid::neighbor_ids);  // List of adjacent trapezoid IDs

    // Bind Portal
    py::class_<Portal>(m, "Portal")
        .def(py::init<>())
        .def_readwrite("left_layer_id", &Portal::left_layer_id)
        .def_readwrite("right_layer_id", &Portal::right_layer_id)
        .def_readwrite("h0004", &Portal::h0004)
        .def_readwrite("pair_index", &Portal::pair_index)  // Paired portal index
        .def_readwrite("trapezoid_indices", &Portal::trapezoid_indices);  // Vector of trapezoid IDs


    // Bind Node as base class
    py::class_<Node>(m, "Node")
        .def(py::init<>())
        .def_readwrite("type", &Node::type)
        .def_readwrite("id", &Node::id);

    // Bind XNode, YNode, and SinkNode as derived classes
    py::class_<XNode, Node>(m, "XNode")
        .def(py::init<>())
        .def_readwrite("pos", &XNode::pos)   // Python-safe 2D position
        .def_readwrite("dir", &XNode::dir)   // Direction vector
        .def_readwrite("left_id", &XNode::left_id)
        .def_readwrite("right_id", &XNode::right_id);

    py::class_<YNode, Node>(m, "YNode")
        .def(py::init<>())
        .def_readwrite("pos", &YNode::pos)
        .def_readwrite("left_id", &YNode::left_id)
        .def_readwrite("right_id", &YNode::right_id);

    py::class_<SinkNode, Node>(m, "SinkNode")
        .def(py::init<>())
        .def_readwrite("trapezoid_ids", &SinkNode::trapezoid_ids);  // List of trapezoid IDs

    // Bind PathingMap
    py::class_<PathingMap>(m, "PathingMap")
        .def(py::init<>())  // Default constructor
        .def_readwrite("zplane", &PathingMap::zplane)
        .def_readwrite("h0004", &PathingMap::h0004)
        .def_readwrite("h0008", &PathingMap::h0008)
        .def_readwrite("h000C", &PathingMap::h000C)
        .def_readwrite("h0010", &PathingMap::h0010)
        .def_readwrite("trapezoids", &PathingMap::trapezoids)  // Vector of PathingTrapezoid
        .def_readwrite("sink_nodes", &PathingMap::sink_nodes)  // Vector of SinkNode
        .def_readwrite("x_nodes", &PathingMap::x_nodes)        // Vector of XNode
        .def_readwrite("y_nodes", &PathingMap::y_nodes)        // Vector of YNode
        .def_readwrite("h0034", &PathingMap::h0034)
        .def_readwrite("h0038", &PathingMap::h0038)
        .def_readwrite("portals", &PathingMap::portals)        // Vector of Portal
        .def_readwrite("root_node_id", &PathingMap::root_node_id);

    // Bind the GetPathingMaps function
    m.def("get_map_boundaries", &GetMapBoundaries, "Retrieve map boundaries");
    m.def("get_pathing_maps", &GetPathingMaps, "Retrieve and convert all pathing maps");
}
