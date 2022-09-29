#include "./ledcube/LEDCubeManager.h"
#include "./server/WebServer.h"

#ifdef _WIN32
#include "windows/ledcubewindow.h"
#endif // _WIN32

class App
{
public:
	App()
		: cubeManager(new LEDCubeManager()), wServ(new WebServer()) {
		wServ->addFriend(cubeManager);
		cubeManager->start();
		wServ->start();
	}
	~App() {
		std::cout << "Stopping server..." << std::endl;
		wServ->removeFriend(cubeManager);
		delete wServ;
		wServ = nullptr;
		delete cubeManager;
		cubeManager = nullptr;
	}
	void run() {
		wServ->join();
		cubeManager->join();
	}
	void stop() {
		wServ->stop();
		cubeManager->stop();
	}

private:
	LEDCubeManager *cubeManager = nullptr;
	WebServer *wServ = nullptr;
};

App *app = nullptr;
void signalHandler(int signum) {
	if (app != nullptr) {
		app->stop();
	}
}

int main(int argc, char **argv) {
#ifdef _WIN32
	LedCubeWindow window(GetModuleHandle(0), 10);
#endif // _WIN32

	std::cout << "Starting server..." << std::endl;
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	// If './ledcube.env' doesn't exist, create it
	if (!std::ifstream("./ledcube.env").good()) {
		std::ofstream file("./ledcube.env");
		file << "PORT=80" << std::endl;
		file << "PASSWORD=secretLedcube" << std::endl;
		file << "WEB_FOLDER=./WEB/" << std::endl;
		file << "ANIMATIONS_FOLDER=./animations/" << std::endl;
		file << "MIN_TIME_BETWEEN_RANDOM_ANIMATIONS=10000" << std::endl;
		file << "DEBUG=false" << std::endl;
		file.close();
		std::cout << "ledcube.env created with default values" << std::endl;
	}

	app = new App();

#ifdef _WIN32
	int ret = window.process();
	app->stop();
	delete app;
	app = nullptr;
	return ret;
#else
	app->run();
	delete app;
	app = nullptr;
	return 0;
#endif
}
