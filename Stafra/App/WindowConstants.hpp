#pragma once

#define MENU_SAVE_CLICK_RULE       1001
#define MENU_OPEN_CLICK_RULE       1002
#define MENU_OPEN_BOARD            1003
#define MENU_SAVE_BOARD            1004
#define MENU_OPEN_RESTRICTION      1005
#define MENU_SHOW_CLICK_RULE_GRID  1006
#define MENU_HIDE_CLICK_RULE_GRID  1007
#define MENU_RESET_CLICK_RULE      1008
#define MENU_INITIAL_STATE_CORNERS 1009
#define MENU_INITIAL_STATE_SIDES   1010
#define MENU_INITIAL_STATE_CENTER  1011

#define MENU_RESET      2001
#define MENU_PAUSE      2002
#define MENU_STOP       2003
#define MENU_NEXT_FRAME 2004

#define MAIN_THREAD_APPEND_TO_LOG (WM_APP + 1)

#define RENDER_THREAD_EXIT              (WM_APP + 101)
#define RENDER_THREAD_REINIT            (WM_APP + 102)
#define RENDER_THREAD_CLICK_RULE        (WM_APP + 103)
#define RENDER_THREAD_LOAD_CLICK_RULE   (WM_APP + 104)
#define RENDER_THREAD_SAVE_CLICK_RULE   (WM_APP + 105)
#define RENDER_THREAD_LOAD_BOARD        (WM_APP + 106)
#define RENDER_THREAD_SAVE_STABILITY    (WM_APP + 107)
#define RENDER_THREAD_SAVE_VIDEO_FRAME  (WM_APP + 108)
#define RENDER_THREAD_REDRAW            (WM_APP + 109)
#define RENDER_THREAD_REDRAW_CLICK_RULE (WM_APP + 110)
#define RENDER_THREAD_COMPUTE_TICK      (WM_APP + 111)
#define RENDER_THREAD_RESIZE            (WM_APP + 112)
#define RENDER_THREAD_RESIZE_BOARD      (WM_APP + 113)
#define RENDER_THREAD_SYNC              (WM_APP + 200)

#define TICK_THREAD_EXIT (WM_APP + 201)
#define TICK_THREAD_SYNC (WM_APP + 300)

