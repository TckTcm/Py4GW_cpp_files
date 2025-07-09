#include "py_skillbar.h"

void bind_SkillbarSkill(py::module_& m) {
    py::class_<SkillbarSkill>(m, "SkillbarSkill")
        .def(py::init<SkillID, int, int, int, int, int>(),
            py::arg("id"),
            py::arg("adrenaline_a") = 0,
            py::arg("adrenaline_b") = 0,
            py::arg("recharge") = 0,
            py::arg("event") = 0,  // Constructor with default values
            py::arg("get_recharge") = 0)  // Constructor with default values
        .def_readonly("id", &SkillbarSkill::ID)
        .def_readonly("adrenaline_a", &SkillbarSkill::adrenaline_a)
        .def_readonly("adrenaline_b", &SkillbarSkill::adrenaline_b)
        .def_readonly("recharge", &SkillbarSkill::recharge)
        .def_readonly("event", &SkillbarSkill::event)
        .def_readonly("get_recharge", &SkillbarSkill::get_recharge);  // Read-only property
}

void bind_Skillbar(py::module_& m) {
    py::class_<Skillbar>(m, "Skillbar")
        .def(py::init<>())  // Constructor
        .def_readonly("agent_id", &Skillbar::agent_id)  // Read-only property
        .def_readonly("disabled", &Skillbar::disabled)  // Read-only property
        .def_readonly("casting", &Skillbar::casting)  // Read-only property
        .def_readonly("skills", &Skillbar::skills)  // Read-only property
        .def("GetContext", &Skillbar::GetContext)
        .def("GetSkill", &Skillbar::get_skill, py::arg("slot"), "Get the skill from the skillbar by index (1-8)")
        .def("LoadSkillTemplate", &Skillbar::LoadSkilltemplate, py::arg("skill_template"), "Load a skill template into the player's skillbar")
        .def("LoadHeroSkillTemplate", &Skillbar::LoadHeroSkillTemplate, py::arg("hero_index"), py::arg("skill_template"), "Load a skill template into a hero's skillbar")
        .def("UseSkill", &Skillbar::UseSkill, py::arg("slot"), py::arg("target"))
        .def("UseSkillTargetless", &Skillbar::PointBlankUseSkill, py::arg("slot"), "Use a skill in point-blank target")
        .def("HeroUseSkill", &Skillbar::HeroUseSkill, py::arg("target_agent_id"), py::arg("skill_number"), py::arg("hero_idx"))
        .def("ChangeHeroSecondary", &Skillbar::ChangeHeroSecondary, py::arg("hero_index"), py::arg("profession"))
        .def("GetHeroSkillbar", &Skillbar::GetHeroSkillbar, py::arg("hero_index"))
        .def("GetHoveredSkill", &Skillbar::GetHoveredSkill)
        .def("IsSkillUnlocked", &Skillbar::IsSkillUnlocked, py::arg("skill_id"))
        .def("IsSkillLearnt", &Skillbar::IsSkillLearnt, py::arg("skill_id"));
}


PYBIND11_EMBEDDED_MODULE(PySkillbar, m) {
    bind_SkillbarSkill(m);
    bind_Skillbar(m);
}
