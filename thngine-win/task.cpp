#include "task.h"
#include <memory>
#include <map>
#include <stdexcept>

namespace Thngine
{
	namespace Task
	{
		static std::map<TaskHandle, std::unique_ptr<TaskThread>> taskList;
		static TaskThread* currentTask;
		static LPVOID dispatcherContext;

		/// <summary>
		/// A private class for task abstraction.
		/// </summary>
		class TaskThread
		{
		public:
			LPVOID fiberContext;
			TaskStates state;

			TaskThread(TaskCallback callback);
			~TaskThread();
		};

		/// <summary>
		/// Constructor of TaskThread class.
		/// </summary>
		/// <param name="callback">Task body to execute</param>
		TaskThread::TaskThread(TaskCallback callback)
		{
			fiberContext = CreateFiberEx(4096,4096,FIBER_FLAG_FLOAT_SWITCH, callback, NULL);
			state = TASK_STATE_READY;
		}

		/// <summary>
		/// Destructor of TaskThread class.
		/// </summary>
		TaskThread::~TaskThread()
		{
			DeleteFiber(fiberContext);
		}

		/// <summary>
		/// Create a task with a callback.
		/// </summary>
		/// <param name="callback">Task callback to execute.</param>
		/// <returns>A non-null task handle</returns>
		TaskHandle CreateTask(TaskCallback callback)
		{
			for (TaskHandle i = 1; i < UINT_MAX; i++)
			{
				if (taskList.count(i) != 0)
					continue;
				taskList[i] = std::make_unique<TaskThread>(callback);
				return i;
			}

			return NULL;
		}

		/// <summary>
		/// Get task count.
		/// </summary>
		/// <returns>Task count</returns>
		size_t GetTaskCount()
		{
			return taskList.size();
		}

		/// <summary>
		/// Initialize task system.
		/// </summary>
		void Init()
		{
			dispatcherContext = ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
		}

		/// <summary>
		/// Cleanup task system.
		/// </summary>
		void Deinit()
		{
			ConvertFiberToThread();
		}

		/// <summary>
		/// Execute a task tick.
		/// </summary>
		void Tick()
		{
			for (auto i = taskList.begin(); i != taskList.end();)
			{
				TaskThread* t = (*i).second.get();

				currentTask = t;
				
				switch (t->state)
				{
				case TASK_STATE_READY:
				case TASK_STATE_RUNNING:
					SwitchToFiber(t->fiberContext);
					break;
				case TASK_STATE_TERMINATED:
					taskList.erase(i++);
					break;
				default:
					throw new std::runtime_error(__FUNCTION__);
				}

				i++;
			}
		}

		/// <summary>
		/// Return function for tasks.
		/// Do not let the task function return, call this instead.
		/// </summary>
		void Return()
		{
			currentTask->state = TASK_STATE_TERMINATED;
			SwitchToFiber(dispatcherContext);
		}

		/// <summary>
		/// Wait for N ticks.
		/// </summary>
		/// <param name="tick">Ticks to wait.</param>
		void Wait(unsigned int tick)
		{
			for (unsigned int i = 0; i < tick; i++)
				SwitchToFiber(dispatcherContext);
		}
	}
}
