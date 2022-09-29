#pragma once
#include <thread>

class Thread
{
public:
	Thread();
	~Thread();

	void start();
	void stop();
	void join();
	constexpr bool isRunning() const { return running; }
	constexpr bool canRun() const { return !askedToStop; }

protected:
	virtual void run() = 0;

private:
	void runInternal();
	std::thread *thread = nullptr;
	bool running = false;
	bool askedToStop = false;
};