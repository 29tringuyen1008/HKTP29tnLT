#pragma once
#include <Windows.h>
#include <vector>
#include <functional>

#define TASK_ADDED_SUCCESS 1
#define TASK_CALLED_SUCCESS 1

typedef void (*TaskFunction) (void* TaskParameter);

class Program;

enum class TaskType {
	Static = 0,
	Runtime = 1,
};

enum class TaskState {
	Run = 0,
	Wait = 1,
	CallToRuntime = 2,
	Finished = 3,
};
class Task
{

};