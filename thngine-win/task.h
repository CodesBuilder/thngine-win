#pragma once

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
		typedef void (*TaskCallback)();
		class TaskThread;

		TaskHandle CreateTask(TaskCallback callback);

		void Tick();

		void Wait(unsigned int ticks);
		void Return();

		TaskHandle CreateTask(TaskCallback callback);
		size_t GetTaskCount();
	}
}