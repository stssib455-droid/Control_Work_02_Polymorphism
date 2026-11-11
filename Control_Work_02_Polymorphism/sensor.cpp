#include "sensor.h"
#include <limits>

// GPS
GPS::GPS(int satellites, double acc, bool ready)
    : m_satellites(satellites), m_accuracy(acc), m_ready(ready) {}

double GPS::read() { return m_accuracy; }
bool GPS::is_ready() const { return m_ready && (m_satellites >= 4); }
QString GPS::name() const { return "GPS"; }
double GPS::accuracy() const { return m_accuracy; }
void GPS::set_ready(bool ready) { m_ready = ready; }
void GPS::set_satellites(int count) { m_satellites = count; }
void GPS::set_accuracy(double acc) { m_accuracy = acc; }
int GPS::satellites() const { return m_satellites; }

// GLONASS
GLONASS::GLONASS(int satellites, double acc, bool ready)
    : m_satellites(satellites), m_accuracy(acc), m_ready(ready) {}

double GLONASS::read() { return m_accuracy; }
bool GLONASS::is_ready() const { return m_ready && (m_satellites >= 4); }
QString GLONASS::name() const { return "GLONASS"; }
double GLONASS::accuracy() const { return m_accuracy; }
void GLONASS::set_ready(bool ready) { m_ready = ready; }
void GLONASS::set_satellites(int count) { m_satellites = count; }
void GLONASS::set_accuracy(double acc) { m_accuracy = acc; }
int GLONASS::satellites() const { return m_satellites; }

// VisualOdometry
VisualOdometry::VisualOdometry(double acc, bool ready)
    : m_accuracy(acc), m_ready(ready) {}

double VisualOdometry::read() { return m_accuracy; }
bool VisualOdometry::is_ready() const { return m_ready; }
QString VisualOdometry::name() const { return "Visual odometry"; }
double VisualOdometry::accuracy() const { return m_accuracy; }
void VisualOdometry::set_ready(bool ready) { m_ready = ready; }
void VisualOdometry::set_accuracy(double acc) { m_accuracy = acc; }

// Алгоритм поиска сенсора с наименьшей погрешностью
Sensor* choose_best_sensor(const std::vector<Sensor*>& sensors) {
    Sensor* best = nullptr;
    double min_error = std::numeric_limits<double>::max();

    for (Sensor* s : sensors) {
        if (!s) continue;
        if (s->is_ready()) {
            if (s->accuracy() < min_error) {
                min_error = s->accuracy();
                best = s;
            }
        }
    }

    if (!best) {
        qDebug() << "Error: no ready sensors available!";
        return nullptr;
    }

    qDebug().noquote() << QString("Best sensor selected: %1 (accuracy = %2 m)")
                              .arg(best->name())
                              .arg(best->accuracy());
    return best;
}
