#pragma once
class Engine
{
public:
	Engine() = default;
	~Engine() = default;
	void init();
	// Run the engine
	void run();
	// Shutdown the engine
	void shutdown();
};

