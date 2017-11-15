#include "Core.hh"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <chrono>

#include "defines.hh"

#include "Network.hh"

#include "Timer.hh"

Core::Core() : irr::IReferenceCounted()
{
	win_name = "";
	device = NULL;
	driver = NULL;
	gui_env = NULL;
	node_manager = NULL;
	event_receiver = NULL;
	camera = NULL;
}

void Core::begin(const char * winName)
{
        init_device(winName);

	Timer::init();
}

void Core::end(void)
{
	if(node_manager)
		node_manager->drop();
        if(event_receiver)
                event_receiver->drop();
        if(camera)
		camera->drop();
	if(device)
                device->drop();
}

bool Core::run(void)
{
        return device->run();
}

bool Core::active(void)
{
        return device->isWindowActive();
}

void Core::yield(void)
{
        device->yield();
}

void Core::update(void)
{
        device->setWindowCaption(irr::core::stringw(win_name + "  FPS: " + irr::core::stringc(driver->getFPS())).c_str());

        irr::core::stringw FPS = irr::core::stringw(driver->getFPS());

        if(event_receiver->keyDown(irr::KEY_ESCAPE)) {
                device->closeDevice();
        }

	node_manager->updateAll(Timer::getMs());
}

void Core::render(void)
{
        driver->beginScene(true, true, irr::video::SColor(0, 0, 0, 0));

	node_manager->renderAll(driver, camera);

        gui_env->drawAll();

        driver->endScene();
}

void Core::init_device(const char * winName)
{
        params.AntiAlias = CORE_ANTI_ALIAS;
        params.DriverType  = irr::video::EDT_OPENGL;
        params.WindowSize = irr::core::dimension2d<irr::u32>(CORE_WINDOW_WIDTH, CORE_WINDOW_HEIGHT);
        params.Stencilbuffer = false;
        params.Fullscreen = CORE_FULLSCREEN;
        params.Vsync = CORE_VSYNC;

        device = irr::createDeviceEx(params);
        if(!device) {
                printf("Could not create device!\n");
                return;
        }

        device->setWindowCaption(irr::core::stringw(winName).c_str());
        win_name = winName;

        driver = device->getVideoDriver();
        gui_env = device->getGUIEnvironment();

        event_receiver = new EventReceiver(this);

        device->setEventReceiver(event_receiver);

	node_manager = new NodeManager();

	camera = new Camera(node_manager, irr::core::position2df(0.0, 0.0), VIEWPORT_MAX);

	Network::init(this);
}
