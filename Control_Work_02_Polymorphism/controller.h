#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>

// Базовый абстрактный класс регулятора
class FlightController {
public:
    virtual double calculate_control(double error, double dt) = 0;
    virtual QString controller_name() const = 0;
    virtual ~FlightController() = default;
};

// ПИД-регулятор (kp=0.5 при error=50 выдает эталонные 25)
class PIDController : public FlightController {
private:
    double kp;
    double ki;
    double kd;
    double integral;
    double previous_error;
public:
    explicit PIDController(double p = 0.5, double i = 0.0, double d = 0.0);
    double calculate_control(double error, double dt) override;
    QString controller_name() const override;
};

// Простой пропорциональный регулятор
class SimpleController : public FlightController {
private:
    double coefficient;
public:
    explicit SimpleController(double c = 0.5);
    double calculate_control(double error, double dt) override;
    QString controller_name() const override;
};

#endif // CONTROLLER_H
