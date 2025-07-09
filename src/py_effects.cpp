#include "py_effects.h"


void bind_AgentEffects(py::module_& m) {
    // Bind EffectType class
    py::class_<EffectType>(m, "EffectType")
        .def(py::init<int, int, int, int, float, DWORD, DWORD, DWORD>(),  // Constructor
            py::arg("skill_id"), py::arg("attribute_level"), py::arg("effect_id"),
            py::arg("agent_id"), py::arg("duration"), py::arg("timestamp"),
            py::arg("time_elapsed"), py::arg("time_remaining"))
        .def_readwrite("skill_id", &EffectType::skill_id)
        .def_readwrite("attribute_level", &EffectType::attribute_level)
        .def_readwrite("effect_id", &EffectType::effect_id)
        .def_readwrite("agent_id", &EffectType::agent_id)
        .def_readwrite("duration", &EffectType::duration)
        .def_readwrite("timestamp", &EffectType::timestamp)
        .def_readwrite("time_elapsed", &EffectType::time_elapsed)
        .def_readwrite("time_remaining", &EffectType::time_remaining);

    // Bind BuffType class
    py::class_<BuffType>(m, "BuffType")
        .def(py::init<int, int, int>(),  // Constructor
            py::arg("skill_id"), py::arg("buff_id"), py::arg("target_agent_id"))
        .def_readwrite("skill_id", &BuffType::skill_id)
        .def_readwrite("buff_id", &BuffType::buff_id)
        .def_readwrite("target_agent_id", &BuffType::target_agent_id);

    // Bind AgentEffects class
    py::class_<AgentEffects>(m, "PyEffects")
        // Constructor that takes agent_id
        .def(py::init<int>(), py::arg("agent_id"),
            "Constructor for AgentEffects with agent_id")

        // Get effects for the agent
        .def("GetEffects", &AgentEffects::GetEffects,
            "Get a list of effects for the agent")
        // Get buffs for the agent
        .def("GetBuffs", &AgentEffects::GetBuffs,
            "Get a list of buffs for the agent")

        // Get the count of effects
        .def("GetEffectCount", &AgentEffects::GetEffectCount,
            "Get the number of effects for the agent")

        // Get the count of buffs
        .def("GetBuffCount", &AgentEffects::GetBuffCount,
            "Get the number of buffs for the agent")

        // Check if a specific effect exists
        .def("EffectExists", &AgentEffects::EffectExists, py::arg("skill_id"),
            "Check if a specific effect exists for the agent")

        // Check if a specific buff exists
        .def("BuffExists", &AgentEffects::BuffExists, py::arg("skill_id"),
            "Check if a specific buff exists for the agent")

        //drop buff
        .def("DropBuff", &AgentEffects::DropBuff, py::arg("skill_id"),
            "Drop a specific buff for the agent")

        .def_static("GetAlcoholLevel", &AgentEffects::GetDrunkennessLevel,
            "Get the current alcohol level of the agent")


        .def_static("ApplyDrunkEffect", &AgentEffects::ApplyDrunkEffect,
            "Get the agent ID associated with this AgentEffects instance");
}


PYBIND11_EMBEDDED_MODULE(PyEffects, m) {
    bind_AgentEffects(m);
}

