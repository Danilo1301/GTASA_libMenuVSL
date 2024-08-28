#include "Vehicles.h"

#include "Log.h"
#include "MenuVSL.h"

#include "menu/SimpleGTA.h"

extern uintptr_t* pVehiclePool;
extern int (*GetVehicleRef)(int);
extern void* (*GetVehicleFromRef)(int);

std::map<int, Vehicle*> Vehicles::m_Vehicles;

bool Vehicles::HasVehicleHandle(int hVehicle)
{
    return m_Vehicles.find(hVehicle) != m_Vehicles.end();
}

Vehicle* Vehicles::TryCreateVehicle(int hVehicle)
{   
    if(hVehicle == 0)
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Bro, i think your code has some issues (ERROR) (Vehicles)" << std::endl;
        return NULL;
    }

	if (HasVehicleHandle(hVehicle))
    {
        Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicle already added: " << hVehicle << std::endl;
        return GetVehicleByHandle(hVehicle);
    }

    auto pVehicle = GetVehicleFromRef(hVehicle);

	Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Add vehicle " << hVehicle << ", ref: " << pVehicle << " (" << std::to_string(m_Vehicles.size() + 1) << " total)" << std::endl;

    auto vehicle = new Vehicle(hVehicle, pVehicle);

	m_Vehicles[hVehicle] = vehicle;

    return vehicle;
}

Vehicle* Vehicles::GetVehicleByHandle(int hVehicle)
{
	if (!HasVehicleHandle(hVehicle)) return NULL;
	return m_Vehicles.at(hVehicle);
}

void Vehicles::RemoveVehicle(int hVehicle)
{
    auto vehicle = m_Vehicles[hVehicle];

    Log::Level(LOG_LEVEL::LOG_BOTH) << "Vehicles: Delete vehicle " << hVehicle << ", ref: " << vehicle->pVehicle << " (" << std::to_string(m_Vehicles.size() - 1) << " total)" << std::endl;

    m_Vehicles.erase(hVehicle);
    delete vehicle;
}

void Vehicles::TryFindNewVehicles()
{
    if(!MenuVSL::m_FirstUpdated) return;

    std::vector<int> vehicleRefsThatExists;

    auto objects = *(GTAVehicleSA**)(*pVehiclePool + 0);
    tByteFlag* flags = *(tByteFlag**)(*pVehiclePool + 4);
    int size = *(int*)(*pVehiclePool + 8);

    for (int i = 0; i < size; ++i)
    {
        if (flags[i].bEmpty) continue;
        auto& ent = objects[i];
        
        //IS_CAR_SIREN_ON
        //https://github.com/AndroidModLoader/GTA_CLEOMod/blob/RUSJJ_CLEO_MOD/cleo4opcodes.cpp
        
        //bool sirenOn = ent.UInt8At(0x42D + 4) >> 7;

        //void* driver = (void*)ent.UIntAt(0x460 + 4);

        int ref = GetVehicleRef(ent.AsInt());

        vehicleRefsThatExists.push_back(ref);

        if(!HasVehicleHandle(ref))
        {
            auto vehicle = TryCreateVehicle(ref);
        }
    }

    std::vector<int> vehiclesToDelete;

    for(auto p : m_Vehicles)
    {
        auto vehicle = p.second;
        auto ref = vehicle->hVehicle;

        auto it = std::find(vehicleRefsThatExists.begin(), vehicleRefsThatExists.end(), ref);
        if (it == vehicleRefsThatExists.end())
        {
            Log::Level(LOG_LEVEL::LOG_BOTH) << "Could not find vehicle " << ref << std::endl;
            vehiclesToDelete.push_back(vehicle->hVehicle);
        }
    }

    for(auto hVehicle : vehiclesToDelete)
    {
        RemoveVehicle(hVehicle);
    }
}

std::vector<Vehicle*> Vehicles::GetVehicles()
{
    TryFindNewVehicles();

    std::vector<Vehicle*> vehicles;
    for(auto p : m_Vehicles)
    {
        auto vehicle = p.second;
        vehicles.push_back(vehicle);
    }
    return vehicles;
}