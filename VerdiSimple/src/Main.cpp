#include "Verdi.h"


#include "boost/xpressive/xpressive.hpp"

#include <GL/glfw.h>

#include "VerdiRayCastStrategy.h"
#include "VerdiTexSliceStrategy.h"

#define max(a,b) ((a>b) ? a : b)
#define min(a,b) ((a>b) ? b : a)

#define APP_DEFAULT_WIDTH	800
#define APP_DEFAULT_HEIGHT	600
#define APP_TITLE			"Verdi Test"

Verdi::RenderSystem* renderSystem;
Verdi::RenderContext* renderContext;
Verdi::Camera* camera;
Verdi::TransferFunction* transferFunction;
Verdi::HardwareTransferFunction* hardwareTransferFunction;
Verdi::DataSet* dataSet;
Verdi::DataSet* gradientDataSet;
Verdi::HardwareDataSet* hardwareDataSet;
Verdi::HardwareDataSet* hardwareGradientDataSet;
Verdi::MinMaxOctree* emptySpaceTree;

float _time = 0.0f;

Verdi::String filename = "data/Bruce.pvm";

void testGrammar() 
{
	// [a-z]+\((([a-z]+|".*"),)*(([a-z]+|".*"))\);

	Verdi::String nameStr = "([a-z][a-zA-z0-9]*)";
	Verdi::String argStr = "(([a-zA-z]*)|\".*\")";
	Verdi::String funcStr = nameStr + "\\((" + argStr + ",)*" + argStr + "\\);";

	Verdi::String str = "aaaaa;";

	boost::xpressive::sregex grmFunc = 
		boost::xpressive::sregex::compile(funcStr);

	// Math the string with the language definition
	boost::xpressive::smatch what;
	if( boost::xpressive::regex_match(str,what,grmFunc) ) {

		// Find the first token (the function name)
		int const subs[] = { 1 };
		boost::xpressive::sregex_token_iterator itr( str.begin(), str.end(), grmFunc, subs );
		boost::xpressive::sregex_token_iterator end;
		if(itr != end) {

			// Extract function name
			Verdi::String functionName = itr->str();

			// Extract arguments string
			size_t oldSize = str.size();
			size_t funSize = functionName.size();

			// Ignore brackets and terminator
			str = str.substr(funSize+1,(oldSize-funSize)-3);

			// If we can't find a , there is only one argument
			if( str.find_first_of(',') != std::string::npos ) {

				Verdi::StringVector args;
				Verdi::StringUtility::split(str,',',args);

				for(size_t i = 0; i < args.size(); ++i) {
					
					// Remove the delimiters
					if(args[i].at(args[i].size()-1) == ',')
						args[i] = args[i].substr(0,args[i].size()-1);

					// Remove any quotations
					if(args[i].at(0) == '"')
						args[i] = args[i].substr(1,args[i].size()-2);

					printf("Argument: %s\n",args[i].c_str());
				}

			} else {
				printf("Only argument is: %s\n",str.c_str());
			}
		}
	}
}

void initialise(GLuint w, GLuint h)
{
	testGrammar();
	//system("pause");
	//exit(1);

	renderSystem = new Verdi::RenderSystem();

	try {
		Verdi::DataSetReaderFactory factory;
		Verdi::DataSetReaderPtr reader = factory.getDataSetReader(Verdi::DataSetReaderFactory::DSRF_PVM);

		Verdi::StringMap args;
		args["width"] = "512";
		args["height"] = "512";
		args["depth"] = "174";
		args["format"] = Verdi::StringUtility::toString(Verdi::DataSet::DSCF_UBYTE8);
		args["quantise"] = "0";

		dataSet = reader->readFromFile(filename,&args);

	} catch (const Verdi::Exception& e)
	{
		printf("%s\n",Verdi::StringUtility::toString(e).c_str());
		return;
	}

	printf("data set ptr %d \n",dataSet);

	gradientDataSet = new Verdi::DataSet(dataSet->getWidth(),
		dataSet->getHeight(),dataSet->getDepth(),1,	Verdi::DataSet::DSCF_UBYTE8);

	Verdi::DataSet::Window win;

	win.x = 0; 
	win.y = 0;
	win.z = 0;

	win.width = dataSet->getWidth();
	win.height = dataSet->getHeight();
	win.depth = dataSet->getDepth();

	printf("%d %d %d\n",win.width,win.height,win.depth);

	printf("Computing gradient mags...\n");
	Verdi::DataSet::computeGradientMagnitudes(win,dataSet,0,gradientDataSet,0);
	printf("Finished computing gradient mags...\n");

	//gradientDataSet = dataSet->computeGradients();

	hardwareDataSet = new Verdi::HardwareDataSet();
	hardwareDataSet->clear();
	hardwareDataSet->updateFromDataSet(dataSet);

	//hardwareGradientDataSet = new Verdi::HardwareDataSet();
	//hardwareGradientDataSet->clear();
	//hardwareGradientDataSet->updateFromDataSet(gradientDataSet);

	Verdi::CosineSpline spl;
	spl.clear();
	spl.addControlPoint(Verdi::Spline::ControlPoint(0.0f,Verdi::Vec4(0.0f,0.0f,0.0f,0.00f)));
	spl.addControlPoint(Verdi::Spline::ControlPoint(0.1f,Verdi::Vec4(0.0f,0.0f,0.0f,0.00f)));
	spl.addControlPoint(Verdi::Spline::ControlPoint(0.19f,Verdi::Vec4(0.5f,0.0f,0.0f,0.01f)));
	spl.addControlPoint(Verdi::Spline::ControlPoint(0.27f,Verdi::Vec4(1.0f,1.0f,0.8f,0.025f)));
	spl.addControlPoint(Verdi::Spline::ControlPoint(0.29f,Verdi::Vec4(0.5f,0.0f,0.0f,0.05f)));
	spl.addControlPoint(Verdi::Spline::ControlPoint(0.30001f,Verdi::Vec4(0.0f,1.0f,0.0f,0.00f)));

	spl.addControlPoint(Verdi::Spline::ControlPoint(1.0f,Verdi::Vec4(0.0f,0.0f,0.0f,0.0f)));

	printf("Minimal resolution estimate: %d\n",spl.getMinimalResolutionEstimate());

	transferFunction = new Verdi::TransferFunction();
	transferFunction->fillFromSpline(spl);

	hardwareTransferFunction = new Verdi::HardwareTransferFunction();
	hardwareTransferFunction->clear();
	hardwareTransferFunction->updateFromTransferFunction(transferFunction);

	printf("Computing empty space tree...\n");
	emptySpaceTree = new Verdi::MinMaxOctree();
	emptySpaceTree->create(dataSet,0,gradientDataSet,0,transferFunction,5);
	printf("Finished computing empty space tree...\n");

	camera = new Verdi::Camera();
	camera->setIdentity();
	camera->lookAt(Verdi::Vec3(0.0f,0.0f,-10.0f),Verdi::Vec3(0.0f,0.0f,0.0f),Verdi::Vec3(0.0f,1.0f,0.0f));

	renderContext = new Verdi::RenderContext();
	renderContext->setBackgroundColour(Verdi::Vec4(0.0f,0.0f,0.0f,0.0f));
	renderContext->setCamera(camera);
	renderContext->setHardwareDataSet(hardwareDataSet);
	renderContext->setHardwareTransferFunction(hardwareTransferFunction);
	renderContext->setDimensionRatio(dataSet->getCorrectedDimensionRatio());
	renderContext->setMinMaxOctree(emptySpaceTree);

	Verdi::Vec3 v = dataSet->getCorrectedDimensionRatio();
	printf("Corrected dimension ratio: %f %f %f\n",v.x,v.y,v.z);

	renderSystem->registerContext("TestContext",renderContext);
	//renderSystem->registerStrategy("TestStrategy1", new Verdi::RayCastStrategy() );
	renderSystem->registerStrategy("TestStrategy1", new Verdi::TexSliceStrategy() );

	renderSystem->useContext("TestContext");
	renderSystem->useStrategy("TestStrategy1");

	try {
		renderSystem->initialise(w,h);
	} catch(const Verdi::FileLoadFailedException& e)
	{
		printf("%s\n",e.getDescription().c_str());
	}
}

float t = 0.0f;

float zoom = 0.0f;

float velry = 0.1f;
float velrx = 0.1f;

float roty = 0.0f;
float rotx = 0.0f;

int pmx = 0;
int pmy = 0;

float deltaTime = 1.0f;

bool mouseDown = false;

void renderFrame()
{
	float f = 0.0f+(1.0f+sinf(t*0.25f))*0.5f*6.0f;
	camera->setNearDistance(0.1f);
	camera->setIdentity();
	camera->lookAt(Verdi::Vec3(cosf(-roty*0.25f)*zoom,sinf(-rotx*0.25f)*zoom,sinf(-roty*0.25f)*zoom),
		Verdi::Vec3(0.0f,0.0f,0.0f),Verdi::Vec3(0.0f,1.0f,0.0f));

	t += 0.01f;

	if(glfwGetKey(GLFW_KEY_DOWN)) {
		renderSystem->useStrategy("TestStrategy1");
	}

	if(glfwGetKey(GLFW_KEY_UP)) {
		renderSystem->useStrategy("TestStrategy2");
	}

	if(glfwGetKey(GLFW_KEY_LEFT)) {
		renderContext->setLightingEnabled(false);
	} else
	{
		renderContext->setLightingEnabled(true);
	}

	if(glfwGetKey(GLFW_KEY_SPACE))
	{
		renderContext->setCompositor(Verdi::Compositor(Verdi::Compositor::CCM_MIP));
	} else
	{
		renderContext->setCompositor(Verdi::Compositor(Verdi::Compositor::CCM_EMISSION_ABSORPTION));
	}

	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1))
	{
		int mx;
		int my; 
		glfwGetMousePos(&mx,&my);

		float dmx = (float)(pmx - mx);
		float dmy = (float)(pmy - my);

		velry += dmx;
		velrx += dmy;

		velry = max(-300.0f,min(velry,300.0f));
		velrx = max(-300.0f,min(velrx,300.0f));

		pmx = mx;
		pmy = my;
		renderContext->setNumIterations(400);
		
		renderSystem->beginFrame(0.0f,_time);
		renderSystem->render();
		renderSystem->endFrame();

		mouseDown = true;

	}
	else
	{
		renderContext->setNumIterations(900);
		
		renderSystem->beginFrame(0.0f,_time);
		renderSystem->render();
		renderSystem->endFrame();

		glfwGetMousePos(&pmx,&pmy);
		mouseDown = false;
	}

	zoom = -5.0f+((float)glfwGetMouseWheel())*-0.025f;

	velrx *= powf(0.025f,deltaTime);
	velry *= powf(0.025f,deltaTime);

	rotx += velrx*deltaTime;
	roty += velry*deltaTime;

	_time += 0.01f;
}

void shutdown()
{
	renderSystem->shutdown();
	delete renderSystem;
}

int main(int argc, char** argsz)
{
	if(argc > 1)
	{
		filename = argsz[1];
	}

	if(!glfwInit())
	{
		printf("Couldn't start glfw\n");
		return 1;
	}

	GLuint width = APP_DEFAULT_WIDTH;
	GLuint height = APP_DEFAULT_HEIGHT;
	GLuint fullscreen = 0;

	glfwSwapInterval(1);

	if( !glfwOpenWindow( 
		width, height, 8,8,8,8,24,8,
		GLFW_WINDOW + fullscreen) )
	{
		printf("Couldn't create a main window\n");
		glfwTerminate();
		return 1;
	}

	initialise(width,height);

	/*float dfx = 2.0f/(float)(9999);
	float sfx = -1.0f + dfx * 0.0f;
	float fx = sfx;

	for(int i = 0; i < 10000; ++i) {
		
		printf("%f\n", sfx + dfx * (float)i);
		//printf("%f\n",fx);
		//fx += dfx;
	}*/


	bool done = false;
	while(!done)
	{
		renderFrame();
		glfwSwapBuffers();
		glfwSleep(0.001);

		if( glfwGetKey( GLFW_KEY_ESC ) ||
			!glfwGetWindowParam( GLFW_OPENED ) )
			done = true;
	}

	shutdown();
	return 0;
}
