#ifndef DEBUG_H
#define DEBUG_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "../Utility/QueryType.h"

class Debug {
	public:
		static void Log(std::string message);
		static std::vector<std::string>& GetLogs();

		Debug();
		~Debug();

		void profile();

		void startQuery(QueryType query);
		void endQuery(QueryType query);
		double getQuery(QueryType query);

	private:
		static std::vector<std::string> _Logs;

		void _initialize();

		double queryUpdateStart;
		double queryUpdateEnd;

		GLuint64 queryRenderingStart;
		GLuint64 queryRenderingEnd;
		GLuint64 queryEnvironmentStart;
		GLuint64 queryEnvironmentEnd;
		GLuint64 queryShadowStart;
		GLuint64 queryShadowEnd;
		GLuint64 queryGeometryStart;
		GLuint64 queryGeometryEnd;
		GLuint64 queryLightingStart;
		GLuint64 queryLightingEnd;
		GLuint64 querySSAOStart;
		GLuint64 querySSAOEnd;
		GLuint64 queryPostProcessingStart;
		GLuint64 queryPostProcessingEnd;
		GLuint64 queryUIStart;
		GLuint64 queryUIEnd;

		unsigned int queryIDRendering[2];
		unsigned int queryIDEnvironment[2];
		unsigned int queryIDShadow[2];
		unsigned int queryIDGeometry[2];
		unsigned int queryIDLighting[2];
		unsigned int queryIDSSAO[2];
		unsigned int queryIDPostProcessing[2];
		unsigned int queryIDUI[2];
};

#endif