#include "CziWriterManager.h"
#include "dbgprint.h"

using namespace std;

/*static*/CziWriterManager CziWriterManager::instance;

/*static*/CziWriterManager& CziWriterManager::GetInstance()
{
    return CziWriterManager::instance;
}

void CziWriterManager::AddInstance(int id)
{
    VDBGPRINT((CDbg::Level::Trace, "CziWriterManager::AddInstance: add instance %i.\n", id));
    std::unique_lock<std::mutex> lck(this->mutex);
    this->map.insert(make_pair(id, make_shared< CziWriter>()));
}

bool CziWriterManager::RemoveInstance(int id)
{
    std::unique_lock<std::mutex> lck(this->mutex);
    const auto noOfElementsRemoved = this->map.erase(id);
    VDBGPRINT((CDbg::Level::Trace, "CziWriterManager::RemoveInstance: remove instance %i -> %s.", id, noOfElementsRemoved > 0 ? "success" : "failure"));
    return noOfElementsRemoved > 0;
}

std::shared_ptr<CziWriter> CziWriterManager::GetInstance(int id)
{
    std::unique_lock<std::mutex> lck(this->mutex);
    return this->map.at(id);
}

int CziWriterManager::CreateNewInstance()
{
    const int id = this->instanceCounter.fetch_add(1);
    this->AddInstance(id);
    return id;
}

int CziWriterManager::RemoveAllInstances()
{
    std::unique_lock<std::mutex> lck(this->mutex);
    const int noOfExistingInstances = static_cast<int>(this->map.size());
    VDBGPRINT((CDbg::Level::Trace, "CziWriterManager::RemoveAllInstances: # of existing instances is %i.\n", noOfExistingInstances));
    this->map.clear();
    return noOfExistingInstances;
}