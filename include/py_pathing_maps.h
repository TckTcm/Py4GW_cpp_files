#pragma once
#include "Headers.h"

#include "py_pathing_maps.h"


namespace GW {

    typedef void(__cdecl* FindPath_pt)(PathPoint* start, PathPoint* goal, float range, uint32_t maxCount, uint32_t* count, PathPoint* pathArray);
    static FindPath_pt FindPath_Func = nullptr;

    static void InitPathfinding() {
        //FindPath_Func = (FindPath_pt)Scanner::Find("\x8b\x70\x14\xd9", "xxxx", -0x14);
        FindPath_Func = (FindPath_pt)GW::Scanner::Find("\x8b\x5d\x0c\x8b\x7d\x08\x8b\x70\x14\xd9", "xxxxxxxxxx", -0xE);

        Logger::AssertAddress("FindPath_Func", (uintptr_t)FindPath_Func);
    }

}

class PathPlanner {
public:
    enum class Status {
        Idle,
        Pending,
        Ready,
        Failed
    };

    PathPlanner() {
        GW::InitPathfinding();
    }

    void PlanPath(float start_x, float start_y, float start_z,
        float goal_x, float goal_y, float goal_z) {
        status.store(Status::Pending);
        planned_path.clear();

        if (!GW::FindPath_Func) {
            Logger::Instance().LogError("FindPath_Func not initialized.");
            status.store(Status::Failed);
            return;
        }

        const GW::GamePos start_pos(start_x, start_y, start_z);
        const GW::GamePos goal_pos(goal_x, goal_y, goal_z);

        GW::GameThread::Enqueue([this, start_pos, goal_pos]() {
            std::array<GW::PathPoint, 30> pathArray;
            uint32_t pathCount = 0;
            uint32_t maxPoints = static_cast<uint32_t>(pathArray.size());

            GW::PathPoint start{ start_pos, nullptr };
            GW::PathPoint goal{ goal_pos, nullptr };

            GW::FindPath_Func(&start, &goal, 10000.0f, maxPoints, &pathCount, pathArray.data());

            planned_path.clear();
            if (pathCount == 0 ||
                (pathCount == 1 && (pathArray[0].pos.x != goal_pos.x || pathArray[0].pos.y != goal_pos.y))) {
                //Logger::Instance().LogError("PathPlanner: no valid path found.");
                status.store(Status::Failed);
                return;
            }

            for (uint32_t i = 0; i < pathCount; ++i) {
                GW::GamePos& p = pathArray[i].pos;

                planned_path.emplace_back(p.x, p.y, p.zplane);
            }

            status.store(Status::Ready);
            });
    }

    std::vector<std::tuple<float, float, float>> ComputePathImmediate(float start_x, float start_y, float start_z,
        float goal_x, float goal_y, float goal_z) {
        std::vector<std::tuple<float, float, float>> result;

        if (!GW::FindPath_Func) {
            Logger::Instance().LogError("FindPath_Func not initialized.");
            return result;
        }

        const GW::GamePos start_pos(start_x, start_y, start_z);
        const GW::GamePos goal_pos(goal_x, goal_y, goal_z);

        std::array<GW::PathPoint, 30> pathArray;
        uint32_t pathCount = 0;
        uint32_t maxPoints = static_cast<uint32_t>(pathArray.size());

        GW::PathPoint start{ start_pos, nullptr };
        GW::PathPoint goal{ goal_pos, nullptr };

        GW::FindPath_Func(&start, &goal, 10000.0f, maxPoints, &pathCount, pathArray.data());

        if (pathCount == 0 ||
            (pathCount == 1 && (pathArray[0].pos.x != goal_pos.x || pathArray[0].pos.y != goal_pos.y))) {
            //Logger::Instance().LogError("PathPlanner: no valid path found.");
            status.store(Status::Failed);
            return result;
        }

        for (uint32_t i = 0; i < pathCount; ++i) {
            const GW::GamePos& p = pathArray[i].pos;
            result.emplace_back(p.x, p.y, p.zplane);
        }

        return result;
    }

    Status GetStatus() const {
        return status.load();
    }

    bool IsReady() const {
        return status.load() == Status::Ready;
    }

    bool WasSuccessful() const {
        Status s = status.load();
        return s == Status::Ready;
    }

    const std::vector<std::tuple<float, float, float>>& GetPath() const {
        return planned_path;
    }

    void Reset() {
        planned_path.clear();
        status.store(Status::Idle);
    }

private:
    std::vector<std::tuple<float, float, float>> planned_path;
    std::atomic<Status> status = Status::Idle;
};  // <-- this was missing


PYBIND11_EMBEDDED_MODULE(PyPathing, m) {

    py::enum_<PathPlanner::Status>(m, "PathStatus")
        .value("Idle", PathPlanner::Status::Idle)
        .value("Pending", PathPlanner::Status::Pending)
        .value("Ready", PathPlanner::Status::Ready)
        .value("Failed", PathPlanner::Status::Failed)
        .export_values();

    py::class_<PathPlanner>(m, "PathPlanner")
        .def(py::init<>())  // Default constructor

        .def("plan", &PathPlanner::PlanPath,
            py::arg("start_x"), py::arg("start_y"), py::arg("start_z"),
            py::arg("goal_x"), py::arg("goal_y"), py::arg("goal_z"),
            "Submit a path planning task to the game thread")

        .def("compute_immediate", &PathPlanner::ComputePathImmediate,
            py::arg("start_x"), py::arg("start_y"), py::arg("start_z"),
            py::arg("goal_x"), py::arg("goal_y"), py::arg("goal_z"),
            "Compute the path immediately and return it as a list of (x, y, z) tuples")

        .def("get_status", &PathPlanner::GetStatus,
            "Get current planning status (Idle, Pending, Ready, Failed)")

        .def("is_ready", &PathPlanner::IsReady,
            "Check if the planned path is ready")

        .def("was_successful", &PathPlanner::WasSuccessful,
            "Check if the path planning was successful")

        .def("get_path", &PathPlanner::GetPath,
            py::return_value_policy::reference,
            "Retrieve the calculated path as a list of (x, y, z) tuples")

        .def("reset", &PathPlanner::Reset,
            "Reset the planner to Idle state");

}
