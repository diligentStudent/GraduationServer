#include "stdafx.h"
#include "ContentsProcess.h"

ContentsProcess::ContentsProcess()
	:packageQueue_(nullptr)
{
}

ContentsProcess::~ContentsProcess()
{
	runFuncTable_.clear();
}

void ContentsProcess::initialize()
{
	xml_t config;
	if (!loadConfig(&config))
	{
		return;
	}

	xmlNode_t *root = config.FirstChildElement("App")->FirstChildElement("Contents");
	if (!root)
	{
		SErrLog(L"* not exist process setting");
		return;
	}
	xmlNode_t *elem = root->FirstChildElement("ThreadCount");
	int processCount = 0;
	::sscanf_s(elem->GetText(), "%d", &processCount);

	if (MAX_PACKET_THREAD_ < processCount)
	{
		SErrLog(L"! processThread limit[%d], but config setting [%d]", MAX_PACKET_THREAD_, processCount);
		return;
	}

	packageQueue_ = new ThreadJobQueue<Package *>((WCHAR*)L"ContentsProcessQueue");
	for (int i = 0; i < processCount; ++i)
	{
		threadPool_[i] = MAKE_THREAD(ContentsProcess, process);
	}
	this->registDefaultPacketFunc();
}

void ContentsProcess::release()
{
	SAFE_DELETE(packageQueue_);

	for (auto thread : threadPool_)
	{
		SAFE_DELETE(thread);
	}
}

void ContentsProcess::registDefaultPacketFunc()
{
	runFuncTable_.insert(std::make_pair(E_C_NOTIFY_HEARTBEAT, &ContentsProcess::Packet_HeartBeat));
	runFuncTable_.insert(std::make_pair(E_I_NOTIFY_TERMINAL, &ContentsProcess::Packet_Notify_Terminal));
	runFuncTable_.insert(std::make_pair(E_C_REQ_EXIT, &ContentsProcess::C_REQ_EXIT));
}

void ContentsProcess::putPackage(Package *package)
{
	/*if (package->session_ == nullptr || package->packet_ == nullptr)
		return;*/

	packageQueue_->push(package);
}

void ContentsProcess::run(Package *package)
{
	PacketType type = package->packet_->type();
	auto itr = runFuncTable_.find(type);		//패킷type에 따른 함수포인터를 리턴
	if (itr == runFuncTable_.end())
	{
		SLog(L"! invaild packet runFunction. type[%d]", type);
		package->session_->onClose();
		return;
	}
	RunFunc runFunction = itr->second;
#ifdef _DEBUG
	SLog(L"*** [%d] packet run ***", type);
#endif //_DEBUG
	runFunction(package->session_, package->packet_);
}

void ContentsProcess::execute()
{
	Package *package = nullptr;
	if (packageQueue_->pop(package) == false)
	{
		return;
	}

	this->run(package);

	SAFE_DELETE(package);
}

void ContentsProcess::process()
{
	while (_shutdown == false) //shutdown요청이 있을때까지 무한 루프
	{
		this->execute();
		CONTEXT_SWITCH;
	}
}

//--------------------------------------------------------------//
//기본 패킷 기능 구현
void ContentsProcess::Packet_HeartBeat(Session *session, Packet *rowPacket)
{
	if (session->type() != SESSION_TYPE_CLIENT)
	{
		return;
	}
	session->updateHeartBeat();//현제시간
}

void ContentsProcess::Packet_Notify_Terminal(Session *session, Packet *rowPacket)
{
	session->setType(SESSION_TYPE_TERMINAL);
	SLog(L"* [%s] Terminal accepted.", session->clientAddress().c_str());
}

void ContentsProcess::C_REQ_EXIT(Session *session, Packet *rowPacket)
{
	//클라이언트 read thread 를 종료시켜 주기 위해 처리
	PK_C_REQ_EXIT *packet = (PK_C_REQ_EXIT *)rowPacket;
	//PK_S_ANS_EXIT ansPacket;
	SLog(L"* recv exit packet by [%s]", session->clientAddress().c_str());
	//session->sendPacket(&ansPacket);
	SESSIONMANAGER.forceCloseSession(session);
	return;
}
