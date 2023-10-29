#pragma once

#include <glm/glm.hpp>

namespace OpenEngine::UI {

	float GetLineHeight();

	void ShiftCursorPosX(float amount);
	void ShiftCursorPosY(float amount);
	void ShiftCursorPos(const glm::vec2& amount);

	bool DragFloat(const std::string& label, float* value, float speed = 1.0f, float min = 0.0f, float max = 100.0f, float columnWidth = 100.0f, const char* format = "%.2f");
	bool DragInt(const std::string& label, int* value, int speed = 1, int min = 0, int max = 100, float columnWidth = 100.0f);

	void Vec2Controls(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void Vec3Controls(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void ColorEdit4(const char* label, float* value);
	void ColorEdit3(const char* label, float* value);

}