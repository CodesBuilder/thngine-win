#pragma once
#include <Windows.h>

enum TaskStates
{
	TASK_STATE_READY=1,
	TASK_STATE_RUNNING,
	TASK_STATE_TERMINATED
};

namespace Thngine
{
	namespace Task
	{
		typedef unsigned int TaskHandle;
		typedef LPFIBER_START_ROUTINE TaskCallback;
		class TaskThread;

		TaskHandle CreateTask(TaskCallback callback);

		void Init();
		void Deinit();

		void Tick();

		void Wait(unsigned int ticks);
		void Return();

		TaskHandle CreateTask(TaskCallback callback);
		size_t GetTaskCount();
	}
}