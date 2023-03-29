#include "oepch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace OpenEngine::Math {

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranlation, glm::vec3& outRotaion, glm::vec3& outScale)
	{
		using namespace glm;

		mat4 LocalMatrix(transform);

		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<float>()))
			return false;

		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<float>(0), epsilon<float>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<float>(0), epsilon<float>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<float>(0), epsilon<float>())
			)
		{
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
			LocalMatrix[3][3] = static_cast<float>(1);
		}

		outTranlation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		for (length_t i = 0; i < 3; i++)
			for (length_t j = 0; j < 3; j++)
				Row[i][j] = LocalMatrix[i][j];

		outScale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<float>(1));
		outScale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<float>(1));
		outScale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<float>(1));

		outRotaion.y = asin(-Row[0][2]);
		if (cos(outRotaion.y) != 0)
		{
			outRotaion.x = atan2(Row[1][2], Row[2][2]);
			outRotaion.z = atan2(Row[0][1], Row[0][0]);
		}
		else
		{
			outRotaion.x = atan2(-Row[2][0], Row[1][1]);
			outRotaion.z = 0;
		}

		return true;
	}

}