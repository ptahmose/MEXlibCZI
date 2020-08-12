#include "CziInstanceManager.h"

using namespace std;

/*static*/CziReaderManager CziReaderManager::Instance = CziReaderManager();

void CziReaderManager::AddInstance(int id)
{
    //VDBGPRINT((CDbg::Level::Trace, "CziReaderManager::AddInstance: add instance %" MINTFMT ".", id));
    this->map.insert(make_pair(id, make_shared< CziReader>()));
}

void CziReaderManager::RemoveInstance(int id)
{
    //VDBGPRINT((CDbg::Level::Trace, "CziReaderManager::RemoveInstance: remove instance %" MINTFMT ".", id));
    auto noOfElementsRemoved = this->map.erase(id);
}

std::shared_ptr<CziReader> CziReaderManager::GetInstance(int id)
{
    return this->map.at(id);
}

int CziReaderManager::CreateNewInstance()
{
    int id = this->instanceCounter++;
    this->AddInstance(id);
    return id;
}