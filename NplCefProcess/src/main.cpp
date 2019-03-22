#include <windows.h>
#include <string>
#include "include/cef_app.h"

int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	void* sandbox_info = NULL;
	// Provide CEF with command-line arguments.
	CefMainArgs main_args;

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, NULL, sandbox_info);
	return exit_code;
	return 0;
}
