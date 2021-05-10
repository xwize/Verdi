
#ifndef __Prerequisites_H__
#define __Prerequisites_H__

// For windowing
#include <GL/glfw.h>

// For volume rendering
#include "Verdi.h"
#include "VerdiRayCastStrategy.h"
#include "VerdiTexSliceStrategy.h"

// For command queue
#include <queue>

// Global variables
extern Verdi::Workspace* appWorkspace;
extern Verdi::RenderSystem* appRenderSystem;
extern Verdi::Timer appTimer;
extern std::queue<Verdi::WorkspaceCommand> appCommandQueue;

// Some defines
#define APP_DEFAULT_WIDTH	900
#define APP_DEFAULT_HEIGHT	600
#define APP_TITLE			"Verdi Simple"

#endif // __Prerequisites_H__