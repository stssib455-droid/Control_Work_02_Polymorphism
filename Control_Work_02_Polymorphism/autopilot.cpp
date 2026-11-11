#include "autopilot.h"

Autopilot::Autopilot(UAV* vehicle,
                     const std::vector<Sensor*>& sensors,
                     NavigationSystem* navigation,
                     FlightController* controller,
                     double target_alt)
    : vehicle(vehicle),
      sensors(sensors),
      navigation(navigation),
      controller(controller),
      target_altitude(target_alt) {}

bool Autopilot::run_cycle() {
    qDebug() << "Autopilot cycle started";

    // 1. Проверка наличия аппарата
    if (!vehicle) {
        qDebug() << "Error: vehicle is not set";
        return false;
    }

    // 2. Проверка сенсоров
    if (sensors.empty()) {
        qDebug() << "Autopilot cycle aborted / Reason: sensor failure";
        return false;
    }
    for (Sensor* s : sensors) {
        if (!s || !s->is_ready()) {
            qDebug() << "Autopilot cycle aborted / Reason: sensor failure";
            return false;
        }
    }
    qDebug() << "All sensors are ready";

    // 3. Проверка навигации
    if (!navigation || !navigation->is_valid()) {
        qDebug() << "Navigation data invalid";
        return false;
    }

    // 4. Получение координат и проверка на валидность (отрицательная высота)
    Position pos = navigation->get_position();
    if (pos.altitude < 0.0) {
        qDebug() << "Error: invalid navigation position";
        return false;
    }

    // 5. Проверка регулятора
    if (!controller) {
        qDebug() << "Error: flight controller is not set";
        return false;
    }

    // 6. Расчет ошибки и вычисление управляющего воздействия
    double error = target_altitude - pos.altitude;
    double control = controller->calculate_control(error, 0.1);

    // 7. Эталонный вывод
    qDebug().noquote() << QString("Navigation: %1").arg(navigation->system_name());
    qDebug().noquote() << QString("Current altitude: %1 m").arg(pos.altitude);
    qDebug().noquote() << QString("Target altitude: %1 m").arg(target_altitude);
    qDebug().noquote() << QString("Altitude error: %1 m").arg(error);
    qDebug().noquote() << QString("Controller output: %1").arg(control);
    qDebug() << "Autopilot cycle completed";

    return true;
}
