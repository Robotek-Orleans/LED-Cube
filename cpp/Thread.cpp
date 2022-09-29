#include "Thread.h"

Thread::Thread() {
}

Thread::~Thread() {
	if (thread) {
		stop();
		join();
		delete thread;
		thread = nullptr;
	}
}

void Thread::start() {
	if (running) return;
	running = true;
	askedToStop = false;
	if (thread != nullptr) {
		delete thread;
	}
	thread = new std::thread(&Thread::runInternal, this);
}

void Thread::stop() {
	askedToStop = true;
}

void Thread::join() {
	if (thread != nullptr) {
		thread->join();
	}
}

void Thread::runInternal() {
	running = true;
	while (!askedToStop) {
		run();
	}
	thread = nullptr;
	running = false;
}