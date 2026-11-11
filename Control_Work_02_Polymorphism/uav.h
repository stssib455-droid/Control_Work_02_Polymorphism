#ifndef UAV_H
#define UAV_H

#include <QString>
#include <QDebug>

// Абстрактный базовый класс БПЛА
class UAV {
public:
    virtual void takeoff() = 0;
    virtual void land() = 0;
    virtual QString type() const = 0;
    virtual int motor_count() const = 0;
    virtual ~UAV() = default;
};

// Вариант 2: Квадрокоптер (4 мотора)
class Quadcopter : public UAV {
public:
    void takeoff() override;
    void land() override;
    QString type() const override;
    int motor_count() const override;
};

// Вариант 2: Гексакоптер (6 моторов)
class Hexacopter : public UAV {
public:
    void takeoff() override;
    void land() override;
    QString type() const override;
    int motor_count() const override;
};

// Вариант 2: Октокоптер (8 моторов)
class Octocopter : public UAV {
public:
    void takeoff() override;
    void land() override;
    QString type() const override;
    int motor_count() const override;
};

// Универсальная полиморфная функция
void print_vehicle_info(const UAV& vehicle);

#endif // UAV_H
