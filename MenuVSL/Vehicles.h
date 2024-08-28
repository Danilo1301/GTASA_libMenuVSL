#pragma once

#include "pch.h"

class Vehicle {
public:
    int hVehicle;
    void* pVehicle;

    Vehicle(int hVehicle, void* pVehicle)
    {
        this->hVehicle = hVehicle;
        this->pVehicle = pVehicle;
    }
};

class Vehicles {
public:
    static std::map<int, Vehicle*> m_Vehicles;

    static bool HasVehicleHandle(int hVehicle);
    static Vehicle* TryCreateVehicle(int hVehicle);
    static Vehicle* GetVehicleByHandle(int hVehicle);
    static void RemoveVehicle(int hVehicle);

    static void TryFindNewVehicles();

    static std::vector<Vehicle*> GetVehicles();
};