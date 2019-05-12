#include "stdafx.h"
#include "Lock.h"

//���ϱ� 1~100���� ������� 5��
#ifdef _DEBUG
#define STERN_MODE
#endif

const int INVALID_LINE = -1;

Lock::Lock(const WCHAR *name)
	:lockId_(LockManager::getInstance().generalId()),
	name_(name),
	numberOfThreadUsing_(0)
{
	lockingFile_.clear();
	lockingLine_ = INVALID_LINE;
}

Lock::~Lock()
{
	name_.clear();
}

const size_t& Lock::numbeOfThreadUsing() const
{
	return numberOfThreadUsing_;
}

const WCHAR* Lock::name()
{
	return name_.c_str();
}

size_t Lock::lockId() const
{
	return lockId_;
}

lock_t& Lock::mutex()
{
	return mutex_;
}

void Lock::lock(LPCWSTR fileName, int lineNo)
{
	mutex_.lock();

	lockingFile_ = fileName;
	lockingLine_ = lineNo;
}

void Lock::unlock()
{
	mutex_.unlock();

	lockingFile_.clear();
	lockingLine_ = INVALID_LINE;
}

void Lock::increaseCountNumberOfThreadUsing()
{
	++numberOfThreadUsing_;
}

void Lock::decreaseCountNumberOfThreadUsing()
{
	--numberOfThreadUsing_;
}

void Lock::setThreadId(threadId_t id)
{
	threadId_ = id;
}

void Lock::setNumberOfThreadUsing(const size_t & num)
{
	numberOfThreadUsing_ = num;
}

threadId_t Lock::threadId() const
{
	return threadId_;
}

//-------------------------------------------------------//
// �����ڿ��� ���� �ɰ�, ������ ���������� ��ü ������ ���� Ǭ��.
// std::lock_guard<lock_t> guard(lock) �� ����������, 
// ����� ������ ���� ���� �����Ѵ�.
LockSafeScope::LockSafeScope(Lock *lock, LPCWSTR fileName, int lineNo)
{
	if (lock == nullptr)
	{
		return;
	}
	if (_shutdown == true)
	{
		return;
	}

	lock_ = lock;
	Lock *deadLock = LockManager::getInstance().checkDeadLock(lock_);
	if (deadLock != nullptr)
	{
#ifdef STERN_MODE
		SErrLog(L"! [%s]lock and [%s]lock is dead detecting!", deadLock->name(), lock->name());
#else
		//����� ����
		std::lock(lock_->mutex(), deadLock->mutex());
		SLog(L"!!! [%s]lock and [%s]lock is dead detecting!", deadLock->name(), lock->name());
#endif
		return;
	}

	lock_->lock(fileName, lineNo);
	lock->setThreadId(GET_CURRENT_THREAD_ID());
}

LockSafeScope::~LockSafeScope()
{
	if (!lock_)
	{
		return;
	}
	if (_shutdown == true)
	{
		return;
	}
	lock_->unlock();
	//lock_->setThreadId(nullptr);
}

//-------------------------------------------------------//
LockManager::LockManager()
	:idSeed_(0)
{
}

LockManager::~LockManager()
{
}

HRESULT LockManager::init()
{
	return S_OK;
}

void LockManager::release()
{
}

/*
http://www.npteam.net/849
http://www.boost.org/doc/libs/1_46_1/libs/graph/doc/astar_visitor.html
http://includestdio.tistory.com/m/post/12

http://www.bogotobogo.com/cplusplus/multithreaded4_cplusplus11B.php#nested_lock
���� �ڽ��� �ɸ� threadId��,
Thread������ �ɰ��ִ� Lock ��ȣ�� ����.
�̵��� ���� ���ٰ� �ڽ��� �ɷ��� lock�� ������ lock ��ȯ
�� ����� ó��.
*/
Lock* LockManager::searchLockCycle(Lock *newLock)
{
	//list ���� lock �̸��� ���� ����.?? <-����?! �������� �ּ��� �� �̻��� ������ ��

	//�� ��û�� �� �����带 ThreadManager�� ���� ������
	Thread *thread = ThreadManager::getInstance().at(GET_CURRENT_THREAD_ID());
	
	//�� ��û�� �����尡 ThreadManager�� ���ٸ� nullptr ����
	if (!thread)
	{
		return nullptr;
	}

	std::vector<Lock *> trace;	//����� ������ ���� ����
	trace.push_back(newLock);	//������ 0��°�� ��û�� ���� �־���

	//������� �������� ��ġ
	Lock *deadLock = nullptr;
	while (true)
	{
		Lock *threadLock = thread->lock();

		if (threadLock == nullptr)
		{
			break;
		}

		if (threadLock->lockId() == trace[0]->lockId())
		{
			deadLock = threadLock;
			break;
		}
		trace.push_back(threadLock);

		thread = ThreadManager::getInstance().at(threadLock->threadId());
		if (!thread)
		{
			break;
		}
	}
	trace.empty();

	return deadLock;
}

Lock* LockManager::checkDeadLock(Lock *newLock)
{
	Lock *deadLock = this->searchLockCycle(newLock);
	if (deadLock)
	{
		return deadLock;
	}

	return nullptr;
}

size_t LockManager::generalId()
{
	size_t id = idSeed_++;
	return id;
}