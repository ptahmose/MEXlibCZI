#include "CziInstanceManager.h"
#include "dbgprint.h"

using namespace std;

/*static*/CziReaderManager CziReaderManager::instance;

/*static*/CziReaderManager& CziReaderManager::GetInstance()
{
    return CziReaderManager::instance;
}

void CziReaderManager::AddInstance(int id)
{
    VDBGPRINT((CDbg::Level::Trace, "CziReaderManager::AddInstance: add instance %i.\n", id));
    std::unique_lock<std::mutex> lck(this->mutex);
    this->map.insert(make_pair(id, make_shared< CziReader>()));
}

bool CziReaderManager::RemoveInstance(int id)
{
    std::unique_lock<std::mutex> lck(this->mutex);
    const auto noOfElementsRemoved = this->map.erase(id);
    VDBGPRINT((CDbg::Level::Trace, "CziReaderManager::RemoveInstance: remove instance %i -> %s.", id, noOfElementsRemoved > 0 ? "success" : "failure"));
    return noOfElementsRemoved > 0;
}

std::shared_ptr<CziReader> CziReaderManager::GetInstance(int id)
{
    std::unique_lock<std::mutex> lck(this->mutex);
    return this->map.at(id);
}

int CziReaderManager::CreateNewInstance()
{
    const int id = this->instanceCounter.fetch_add(1);
    this->AddInstance(id);
    return id;
}

int CziReaderManager::RemoveAllInstances()
{
    std::unique_lock<std::mutex> lck(this->mutex);
    const int noOfExistingInstances = this->map.size();
    VDBGPRINT((CDbg::Level::Trace, "CziReaderManager::RemoveAllInstances: # of existing instances is %i.\n", noOfExistingInstances));
    this->map.clear();
    return noOfExistingInstances;
}