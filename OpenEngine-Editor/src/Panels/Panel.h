#pragma once

namespace OpenEngine {

	class Panel
	{
	public:
		//virtual std::string GetName() = 0;
		virtual void OnImGuiRender() = 0;
	};

}