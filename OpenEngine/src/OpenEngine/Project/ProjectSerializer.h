#pragma once

#include "OpenEngine/Serialization/Serializer.h"

namespace OpenEngine {

	class ProjectSerializer
	{
	public:
		ProjectSerializer()
		{
			//using namespace Serializer;
			std::string text;

			/*Serializer::BeginFile();
			Serailizer::EndFile();*/

			std::ofstream file;
			file.open("assets/Test.json");
			file << text;
			file.close();
		}
	};

}