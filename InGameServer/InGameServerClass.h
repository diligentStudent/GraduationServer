#pragma once
class SystemReport : public Work
{
public:
	SystemReport() {};
	~SystemReport() {};

	void tick()
	{
		Monitoring &moniter = Monitoring::getInstance();
		SLog(L"### cpu usage : %2.2f%%, memory usage : %uByte", moniter.processCpuUsage(), moniter.processMemUsage());
	}
};

class InGameServerClass
{
private:
	void serverProcess();

public:
	InGameServerClass();
	~InGameServerClass();

	HRESULT init();
	void release();

	void run();
};
