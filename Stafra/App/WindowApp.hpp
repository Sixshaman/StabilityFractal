#pragma once

#define OEMRESOURCE

#include <Windows.h>
#include <memory>
#include "DisplayRenderer.hpp"
#include "CommandLineArguments.hpp"
#include "..\Computing\FractalGen.hpp"
#include "StafraApp.hpp"

enum class PlayMode
{
	MODE_STOP,
	MODE_PAUSED,
	MODE_SINGLE_FRAME,
	MODE_CONTINUOUS_FRAMES
};

class WindowApp: public StafraApp
{
public:
	WindowApp(HINSTANCE hInstance, const CommandLineArguments& cmdArgs);
	~WindowApp();

	int Run();

protected:
	void Init(HINSTANCE hInstance, const CommandLineArguments& cmdArgs);

private:
	void CreateMainWindow(HINSTANCE hInstance);
	void CreateChildWindows(HINSTANCE hInstance);

	void CreateBackgroundTaskThreads();
	void CloseBackgroundTaskThreads();

	void LayoutChildWindows();
	void CalculateMinWindowSize();

	void UpdateRendererForPreview();

	int OnMenuItem(uint32_t menuItem);
	int OnHotkey(uint32_t hotkey);

	LRESULT CALLBACK AppProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

	void RenderThreadFunc();
	void TickThreadFunc();

private:
	HWND mMainWindowHandle;
	HWND mPreviewAreaHandle;
	HWND mClickRuleAreaHandle;
	HWND mLogAreaHandle;

	HWND mButtonPausePlay;
	HWND mButtonStop;
	HWND mButtonReset;
	HWND mButtonNextFrame;

	HFONT mLogAreaFont;

	DWORD  mRenderThreadID;
	HANDLE mRenderThreadHandle;
	HANDLE mCreateRenderThreadEvent;

	DWORD  mTickThreadID;
	HANDLE mTickThreadHandle;
	HANDLE mCreateTickThreadEvent;

	WNDPROC mLoggerWindowProc;

	int mMinWindowWidth;
	int mMinWindowHeight;

	uint32_t mLoggerMessageCount;

	PlayMode mPlayMode;

	bool mResizing;
};