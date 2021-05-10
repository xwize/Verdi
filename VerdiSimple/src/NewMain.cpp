
#include "Prerequisites.h"
#include "SimpleCommandParser.h"
#include "SimpleWorkspaceCommands.h"

Verdi::Workspace* appWorkspace = NULL;
Verdi::RenderSystem* appRenderSystem = NULL;
Verdi::Timer appTimer;

bool appDone = false;
std::queue<Verdi::WorkspaceCommand> appCommandQueue;
GLFWmutex commandQueueMutex;

void initialise(GLuint w, GLuint h)
{
	appRenderSystem = new Verdi::RenderSystem();
	appWorkspace = new Verdi::Workspace();
	appTimer.reset();

	SimpleWorkspaceCommands::registerCommands(appWorkspace);
}

void renderFrame()
{
	float time = appTimer.getSeconds();

	appRenderSystem->renderFrame(0.0f,time);
}

void resize(int width, int height)
{
	appRenderSystem->resize((uint32_t)width,(uint32_t)height);
}

void commandInput(void *arg)
{
	while(!appDone)
	{
		Verdi::WorkspaceCommand newCmd;

		while(!appCommandQueue.empty()) {
			glfwLockMutex(commandQueueMutex);
			glfwUnlockMutex(commandQueueMutex);
		}

		char linebuf[512];
		std::cout << "> ";
		std::cin.getline(linebuf,512);

		bool commandOk = true;

		SimpleCommandParser parser;
		Verdi::WorkspaceCommandParser::ParseResult res = parser.parseCommandString(
			linebuf, appWorkspace, newCmd);
		
		if(res.type != Verdi::WorkspaceCommandParser::ParseResult::RC_SUCCESS) {
			std::cout << res.description << std::endl;
			commandOk = false;
		}

		glfwLockMutex(commandQueueMutex);

		if(commandOk) {
			appCommandQueue.push(newCmd);
		}

		glfwUnlockMutex(commandQueueMutex);
	}
}

void shutdown()
{
	if(appWorkspace != NULL) {
		delete appWorkspace;
		appWorkspace = NULL;
	}

	if(appRenderSystem != NULL) {
		appRenderSystem->shutdown();
		delete appRenderSystem;
		appRenderSystem = NULL;
	}
}

int main(int argc, char** argsz)
{
	atexit(shutdown);

	if(!glfwInit()) {
		printf("Couldn't start GLFW. Exiting.\n");
		exit(1);
	}

	GLuint width = APP_DEFAULT_WIDTH;
	GLuint height = APP_DEFAULT_HEIGHT;
	GLuint fullscreen = 0;

	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(resize);

	if( !glfwOpenWindow( 
		width, height, 8,8,8,8,24,8,
		GLFW_WINDOW + fullscreen) )
	{
		printf("Couldn't create a main window. Exiting.\n");
		glfwTerminate();
		return 1;
	}

	// Initialise the render system
	initialise(width,height);

	// Start the command processing
	commandQueueMutex = glfwCreateMutex();
	glfwCreateThread(commandInput,0);
	
	appDone = false;
	while(!appDone) 
	{
		// Process the queue
		glfwLockMutex(commandQueueMutex);
		while( !appCommandQueue.empty() ) {
			Verdi::WorkspaceCommand cmd = appCommandQueue.front();
			appCommandQueue.pop();
			std::cout << appWorkspace->executeCommand(cmd) << std::endl;
		}
		glfwUnlockMutex(commandQueueMutex);
		
		// Render a frame
		renderFrame();
		glfwSwapBuffers();
		glfwSleep(0.001);

		if( glfwGetKey( GLFW_KEY_ESC ) ||
			!glfwGetWindowParam( GLFW_OPENED ) )
			appDone = true;
	}

	return 0;
}
