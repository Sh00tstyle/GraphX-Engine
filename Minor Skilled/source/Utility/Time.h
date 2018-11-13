#ifndef TIME_H
#define TIME_H

class Time {
public:
	static float DeltaTime;
	static float CurrentTime;
	static float LastTime;

	static void UpdateTime();
	static float GetFPS();

};

#endif