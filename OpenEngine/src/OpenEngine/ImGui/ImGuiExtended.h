#pragma once

#include <glm/glm.hpp>

namespace ImGui {

	float GetLineHeight();
	bool OEDragFloat(const std::string& label, float* value, float speed = 1.0f, float min = 0.0f, float max = 100.0f, float columnWidth = 100.0f, const char* format = "%.2f");
	void OEVec3Controls(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void OEColorEdit4(const char* label, float* value);
	void OEColorEdit3(const char* label, float* value);

}