#include "controller.h"

PIDController::PIDController(double p, double i, double d)
    : kp(p), ki(i), kd(d), integral(0.0), previous_error(0.0) {}

double PIDController::calculate_control(double error, double dt) {
    if (dt <= 0.0) dt = 0.1;
    integral += error * dt;
    double derivative = (error - previous_error) / dt;
    double output = kp * error + ki * integral + kd * derivative;
    previous_error = error;
    return output;
}

QString PIDController::controller_name() const {
    return "PID";
}

SimpleController::SimpleController(double c) : coefficient(c) {}

double SimpleController::calculate_control(double error, double /*dt*/) {
    return coefficient * error;
}

QString SimpleController::controller_name() const {
    return "Simple";
}
