#include "uav.h"

// Quadcopter
void Quadcopter::takeoff() {
    qDebug() << "Quadcopter: vertical takeoff";
}
void Quadcopter::land() {
    qDebug() << "Quadcopter: vertical landing";
}
QString Quadcopter::type() const {
    return "Quadcopter";
}
int Quadcopter::motor_count() const {
    return 4;
}

// Hexacopter
void Hexacopter::takeoff() {
    qDebug() << "Hexacopter: heavy vertical takeoff";
}
void Hexacopter::land() {
    qDebug() << "Hexacopter: stable vertical landing";
}
QString Hexacopter::type() const {
    return "Hexacopter";
}
int Hexacopter::motor_count() const {
    return 6;
}

// Octocopter
void Octocopter::takeoff() {
    qDebug() << "Octocopter: ultra-heavy vertical takeoff";
}
void Octocopter::land() {
    qDebug() << "Octocopter: redundant multi-rotor landing";
}
QString Octocopter::type() const {
    return "Octocopter";
}
int Octocopter::motor_count() const {
    return 8;
}

// Вывод информации без проверки конкретного типа через if
void print_vehicle_info(const UAV& vehicle) {
    qDebug().noquote() << QString("%1: %2 motors").arg(vehicle.type()).arg(vehicle.motor_count());
    const_cast<UAV&>(vehicle).takeoff();
    const_cast<UAV&>(vehicle).land();
}
