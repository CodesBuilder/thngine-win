#include "task.h"
#include <memory>
#include <map>
#include <stdexcept>

#include <Windows.h>

namespace Thngine
{
	namespace Task
	{
		struct Context
		{
			unsigned long _ebx;
			unsigned long _ecx;
			unsigned long _edx;
			unsigned long _esi;
			unsigned long _edi;
			unsigned long _esp;
			unsigned long _ebp;
			unsigned long _eip;
			unsigned long _eflags;
		};

		static std::map<TaskHandle, std::unique_ptr<TaskThread>> taskList;
		static TaskThread* currentTask;
		static Context dispatcherBreakpoint = { 0 };

		static bool switchingContext = false;
		static int saveContext(Context* dest);
		static void restoreContext(Context* src);

		class TaskThread
		{
		public:
			Context breakpoint;
			TaskStates state;
			TaskCallback callback;
			char* stackBuffer;

			TaskThread(TaskCallback callback);
			~TaskThread();
		};

		TaskThread::TaskThread(TaskCallback callback)
		{
			memset(&breakpoint, 0, sizeof(breakpoint));
			state = TASK_STATE_READY;

			stackBuffer = new char[4096];
			memset(stackBuffer, 0, 4096);
		}

		TaskThread::~TaskThread()
		{
			delete[] stackBuffer;
		}

		TaskHandle CreateTask(TaskCallback callback)
		{
			for (TaskHandle i = 1; i < UINT_MAX; i++)
			{
				if (taskList.count(i) != 0)
					continue;
				taskList[i] = std::make_unique<TaskThread>(callback);
				taskList[i]->callback = callback;
				return i;
			}

			return NULL;
		}

		size_t GetTaskCount()
		{
			return taskList.size();
		}

		void Tick()
		{
			for (auto i = taskList.begin(); i != taskList.end();)
			{
				TaskThread* t = (*i).second.get();

				currentTask = t;

				if (!saveContext(&dispatcherBreakpoint))
				{
					switch (t->state)
					{
					case TASK_STATE_READY:
					{
						t->state = TASK_STATE_RUNNING;
						Context ctxt = { 0 };
						ctxt._esp = (long)(t->stackBuffer + 4096);
						ctxt._ebp = ctxt._esp;
						ctxt._eip = (long)(t->callback);
						restoreContext(&ctxt);
						break;
					}
					case TASK_STATE_RUNNING:
						restoreContext(&(t->breakpoint));
						break;
					case TASK_STATE_TERMINATED:
						taskList.erase(i++);
						continue;
					default:
						throw new std::runtime_error(__FUNCTION__);
					}
				}

				i++;
			}
		}

		void Return()
		{
			currentTask->state = TASK_STATE_TERMINATED;
			restoreContext(&dispatcherBreakpoint);
		}

		void Wait(unsigned int frames)
		{
			for (unsigned int i = 0; i < frames; i++)
			{
				if (!saveContext(&(currentTask->breakpoint)))
					restoreContext(&dispatcherBreakpoint);
			}
		}

		static int saveContext(Context* dest)
		{
			unsigned long _ebx;
			unsigned long _ecx;
			unsigned long _edx;
			unsigned long _esp;
			unsigned long _ebp;
			unsigned long _esi;
			unsigned long _edi;
			unsigned long _eip;
			unsigned long _eflags;

			__asm
			{
				mov _ebx, ebx
				mov _ecx, ecx
				mov _edx, edx

				mov _esp, esp
				mov _ebp, ebp

				mov _esi, esi
				mov _edi, edi

				pushfd
				pop _eflags
			}

			dest->_ebx = _ebx;
			dest->_ecx = _ecx;
			dest->_edx = _edx;
			dest->_esp = _esp;
			dest->_ebp = _ebp;
			dest->_esi = _esi;
			dest->_edi = _edi;
			dest->_eflags = _eflags;

			__asm
			{
				call saveEip
			}

			if (switchingContext)
			{
				switchingContext = false;
				return 1;
			}

			dest->_eip = _eip;
			return 0;

			__asm
			{
			saveEip:
				cmp switchingContext,1
				je skipSaveEip
				mov eax,[esp]
				mov _eip,eax
				skipSaveEip:
				ret
			}
		}

		static void restoreContext(Context* src)
		{
			switchingContext = true;
			unsigned long _ebx;
			unsigned long _ecx;
			unsigned long _edx;
			unsigned long _esp;
			unsigned long _ebp;
			unsigned long _esi;
			unsigned long _edi;
			static unsigned long _eip;
			unsigned long _eflags;

			_ebx = src->_ebx;
			_ecx = src->_ecx;
			_edx = src->_edx;
			_esp = src->_esp;
			_ebp = src->_ebp;
			_esi = src->_esi;
			_edi = src->_edi;
			_eip = src->_eip;
			_eflags = src->_eflags;

			__asm
			{
				mov ebx, _ebx
				mov ecx, _ecx
				mov edx, _edx
				mov esi, _esi
				mov edi, _edi
				push _eflags
				popfd
				mov esp,_esp
				mov ebp,_ebp

				jmp _eip
			}
		}
	}
}
