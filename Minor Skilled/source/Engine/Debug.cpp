#include "Debug.h"

#include <ctime>
#include <time.h>

std::vector<std::string> Debug::_Logs;

void Debug::Log(std::string message) {
	//add the new log message with a timestamp to the vector

	//get the local time
	std::time_t time = std::time(0);
	std::tm now;
	localtime_s(&now, &time);

	//insert extra 0 if the number is below 10
	std::string hour = (now.tm_hour < 10) ? ("0" + std::to_string(now.tm_hour)) : std::to_string(now.tm_hour); 
	std::string minute = (now.tm_min < 10) ? ("0" + std::to_string(now.tm_min)) : std::to_string(now.tm_min);
	std::string second = (now.tm_sec < 10) ? ("0" + std::to_string(now.tm_sec)) : std::to_string(now.tm_sec);

	std::string timestamp = "[" + hour + ":" + minute + ":" + second + "] "; //formatted timestamp

	_Logs.push_back(timestamp + message);
}

std::vector<std::string>& Debug::GetLogs() {
	return _Logs;
}

Debug::Debug() {
	_initialize();
}

Debug::~Debug() {
	glDeleteQueries(2, queryIDRendering);
	glDeleteQueries(2, queryIDEnvironment);
	glDeleteQueries(2, queryIDShadow);
	glDeleteQueries(2, queryIDGeometry);
	glDeleteQueries(2, queryIDLighting);
	glDeleteQueries(2, queryIDSSAO);
	glDeleteQueries(2, queryIDSSR);
	glDeleteQueries(2, queryIDBlending);
	glDeleteQueries(2, queryIDPostProcessing);
	glDeleteQueries(2, queryIDUI);
}

void Debug::profile() {
	GLint renderingTimerAvailable = 0;
	GLint environmentTimerAvailable = 0;
	GLint shadowTimerAvailable = 0;
	GLint depthTimerAvailable = 0;
	GLint geometryTimerAvailable = 0;
	GLint lightingTimerAvailable = 0;
	GLint ssaoTimerAvailable = 0;
	GLint ssrTimerAvailable = 0;
	GLint blendingTimerAvailable = 0;
	GLint postProcessingTimerAvailable = 0;
	GLint uiTimerAvailable = 0;

	while(!renderingTimerAvailable && !environmentTimerAvailable && !shadowTimerAvailable && !depthTimerAvailable && !geometryTimerAvailable &&
		  !lightingTimerAvailable && !ssaoTimerAvailable && !ssrTimerAvailable && !blendingTimerAvailable && !postProcessingTimerAvailable && !uiTimerAvailable) {

		glGetQueryObjectiv(queryIDRendering[1], GL_QUERY_RESULT_AVAILABLE, &renderingTimerAvailable);
		glGetQueryObjectiv(queryIDEnvironment[1], GL_QUERY_RESULT_AVAILABLE, &environmentTimerAvailable);
		glGetQueryObjectiv(queryIDShadow[1], GL_QUERY_RESULT_AVAILABLE, &shadowTimerAvailable);
		glGetQueryObjectiv(queryIDDepth[1], GL_QUERY_RESULT_AVAILABLE, &depthTimerAvailable);
		glGetQueryObjectiv(queryIDGeometry[1], GL_QUERY_RESULT_AVAILABLE, &geometryTimerAvailable);
		glGetQueryObjectiv(queryIDLighting[1], GL_QUERY_RESULT_AVAILABLE, &lightingTimerAvailable);
		glGetQueryObjectiv(queryIDSSAO[1], GL_QUERY_RESULT_AVAILABLE, &ssaoTimerAvailable);
		glGetQueryObjectiv(queryIDSSR[1], GL_QUERY_RESULT_AVAILABLE, &ssrTimerAvailable);
		glGetQueryObjectiv(queryIDBlending[1], GL_QUERY_RESULT_AVAILABLE, &blendingTimerAvailable);
		glGetQueryObjectiv(queryIDPostProcessing[1], GL_QUERY_RESULT_AVAILABLE, &postProcessingTimerAvailable);
		glGetQueryObjectiv(queryIDUI[1], GL_QUERY_RESULT_AVAILABLE, &uiTimerAvailable);
	}

	glGetQueryObjectui64v(queryIDRendering[0], GL_QUERY_RESULT, &queryRenderingStart);
	glGetQueryObjectui64v(queryIDRendering[1], GL_QUERY_RESULT, &queryRenderingEnd);
	glGetQueryObjectui64v(queryIDEnvironment[0], GL_QUERY_RESULT, &queryEnvironmentStart);
	glGetQueryObjectui64v(queryIDEnvironment[1], GL_QUERY_RESULT, &queryEnvironmentEnd);
	glGetQueryObjectui64v(queryIDShadow[0], GL_QUERY_RESULT, &queryShadowStart);
	glGetQueryObjectui64v(queryIDShadow[1], GL_QUERY_RESULT, &queryShadowEnd);
	glGetQueryObjectui64v(queryIDDepth[0], GL_QUERY_RESULT, &queryDepthStart);
	glGetQueryObjectui64v(queryIDDepth[1], GL_QUERY_RESULT, &queryDepthEnd);
	glGetQueryObjectui64v(queryIDGeometry[0], GL_QUERY_RESULT, &queryGeometryStart);
	glGetQueryObjectui64v(queryIDGeometry[1], GL_QUERY_RESULT, &queryGeometryEnd);
	glGetQueryObjectui64v(queryIDLighting[0], GL_QUERY_RESULT, &queryLightingStart);
	glGetQueryObjectui64v(queryIDLighting[1], GL_QUERY_RESULT, &queryLightingEnd);
	glGetQueryObjectui64v(queryIDSSAO[0], GL_QUERY_RESULT, &querySSAOStart);
	glGetQueryObjectui64v(queryIDSSAO[1], GL_QUERY_RESULT, &querySSAOEnd);
	glGetQueryObjectui64v(queryIDSSR[0], GL_QUERY_RESULT, &querySSRStart);
	glGetQueryObjectui64v(queryIDSSR[1], GL_QUERY_RESULT, &querySSREnd);
	glGetQueryObjectui64v(queryIDBlending[0], GL_QUERY_RESULT, &queryBlendingStart);
	glGetQueryObjectui64v(queryIDBlending[1], GL_QUERY_RESULT, &queryBlendingEnd);
	glGetQueryObjectui64v(queryIDPostProcessing[0], GL_QUERY_RESULT, &queryPostProcessingStart);
	glGetQueryObjectui64v(queryIDPostProcessing[1], GL_QUERY_RESULT, &queryPostProcessingEnd);
	glGetQueryObjectui64v(queryIDUI[0], GL_QUERY_RESULT, &queryUIStart);
	glGetQueryObjectui64v(queryIDUI[1], GL_QUERY_RESULT, &queryUIEnd);
}

void Debug::startQuery(QueryType query) {
	switch(query) {
		case QueryType::Update:
			queryUpdateStart = glfwGetTime() * 1000.0;
			break;

		case QueryType::Rendering:
			glQueryCounter(queryIDRendering[0], GL_TIMESTAMP);
			break;

		case QueryType::Environment:
			glQueryCounter(queryIDEnvironment[0], GL_TIMESTAMP);
			break;

		case QueryType::Shadow:
			glQueryCounter(queryIDShadow[0], GL_TIMESTAMP);
			break;

		case QueryType::Depth:
			glQueryCounter(queryIDDepth[0], GL_TIMESTAMP);
			break;

		case QueryType::Geometry:
			glQueryCounter(queryIDGeometry[0], GL_TIMESTAMP);
			break;

		case QueryType::Lighting:
			glQueryCounter(queryIDLighting[0], GL_TIMESTAMP);
			break;

		case QueryType::Blending:
			glQueryCounter(queryIDBlending[0], GL_TIMESTAMP);
			break;

		case QueryType::SSAO:
			glQueryCounter(queryIDSSAO[0], GL_TIMESTAMP);
			break;

		case QueryType::SSR:
			glQueryCounter(queryIDSSR[0], GL_TIMESTAMP);
			break;

		case QueryType::PostProcessing:
			glQueryCounter(queryIDPostProcessing[0], GL_TIMESTAMP);
			break;

		case QueryType::UI:
			glQueryCounter(queryIDUI[0], GL_TIMESTAMP);
			break;
	}
}

void Debug::endQuery(QueryType query) {
	switch(query) {
		case QueryType::Update:
			queryUpdateEnd = glfwGetTime() * 1000.0;
			break;

		case QueryType::Rendering:
			glQueryCounter(queryIDRendering[1], GL_TIMESTAMP);
			break;

		case QueryType::Environment:
			glQueryCounter(queryIDEnvironment[1], GL_TIMESTAMP);
			break;

		case QueryType::Shadow:
			glQueryCounter(queryIDShadow[1], GL_TIMESTAMP);
			break;

		case QueryType::Depth:
			glQueryCounter(queryIDDepth[1], GL_TIMESTAMP);
			break;

		case QueryType::Geometry:
			glQueryCounter(queryIDGeometry[1], GL_TIMESTAMP);
			break;

		case QueryType::Lighting:
			glQueryCounter(queryIDLighting[1], GL_TIMESTAMP);
			break;

		case QueryType::SSAO:
			glQueryCounter(queryIDSSAO[1], GL_TIMESTAMP);
			break;

		case QueryType::SSR:
			glQueryCounter(queryIDSSR[1], GL_TIMESTAMP);
			break;

		case QueryType::Blending:
			glQueryCounter(queryIDBlending[1], GL_TIMESTAMP);
			break;

		case QueryType::PostProcessing:
			glQueryCounter(queryIDPostProcessing[1], GL_TIMESTAMP);
			break;

		case QueryType::UI:
			glQueryCounter(queryIDUI[1], GL_TIMESTAMP);
			break;
	}
}

double Debug::getQuery(QueryType query) {
	switch(query) {
		case QueryType::Update:
			return queryUpdateEnd - queryUpdateStart;
			break;

		case QueryType::Rendering:
			return (queryRenderingEnd - queryRenderingStart) / 1000000.0;
			break;

		case QueryType::Environment:
			return (queryEnvironmentEnd - queryEnvironmentStart) / 1000000.0;
			break;

		case QueryType::Shadow:
			return (queryShadowEnd - queryShadowStart) / 1000000.0;
			break;

		case QueryType::Depth:
			return (queryDepthEnd - queryDepthStart) / 1000000.0;
			break;

		case QueryType::Geometry:
			return (queryGeometryEnd - queryGeometryStart) / 1000000.0;
			break;

		case QueryType::Lighting:
			return (queryLightingEnd - queryLightingStart) / 1000000.0;
			break;

		case QueryType::SSAO:
			return (querySSAOEnd - querySSAOStart) / 1000000.0;
			break;

		case QueryType::SSR:
			return (querySSREnd - querySSRStart) / 1000000.0;
			break;

		case QueryType::Blending:
			return (queryBlendingEnd - queryBlendingStart) / 1000000.0;
			break;

		case QueryType::PostProcessing:
			return (queryPostProcessingEnd - queryPostProcessingStart) / 1000000.0;
			break;

		case QueryType::UI:
			return (queryUIEnd - queryUIStart) / 1000000.0;
			break;

		default:
			return 0.0;
			break;
	}
}

void Debug::_initialize() {
	glGenQueries(2, queryIDRendering);
	glGenQueries(2, queryIDEnvironment);
	glGenQueries(2, queryIDShadow);
	glGenQueries(2, queryIDDepth);
	glGenQueries(2, queryIDGeometry);
	glGenQueries(2, queryIDLighting);
	glGenQueries(2, queryIDSSAO);
	glGenQueries(2, queryIDSSR);
	glGenQueries(2, queryIDBlending);
	glGenQueries(2, queryIDPostProcessing);
	glGenQueries(2, queryIDUI);
}