#include "navigation.h"

// GPSNavigation
GPSNavigation::GPSNavigation(Position pos, bool valid) : m_pos(pos), m_valid(valid) {}
Position GPSNavigation::get_position() const { return m_pos; }
bool GPSNavigation::is_valid() const { return m_valid; }
QString GPSNavigation::system_name() const { return "GPS"; }
void GPSNavigation::set_valid(bool valid) { m_valid = valid; }
void GPSNavigation::set_position(Position pos) { m_pos = pos; }

// InertialNavigation
InertialNavigation::InertialNavigation(Position pos, bool valid) : m_pos(pos), m_valid(valid) {}
Position InertialNavigation::get_position() const { return m_pos; }
bool InertialNavigation::is_valid() const { return m_valid; }
QString InertialNavigation::system_name() const { return "INS"; }
void InertialNavigation::set_valid(bool valid) { m_valid = valid; }
void InertialNavigation::set_position(Position pos) { m_pos = pos; }

// VisualNavigation
VisualNavigation::VisualNavigation(Position pos, bool valid) : m_pos(pos), m_valid(valid) {}
Position VisualNavigation::get_position() const { return m_pos; }
bool VisualNavigation::is_valid() const { return m_valid; }
QString VisualNavigation::system_name() const { return "Visual Navigation"; }
void VisualNavigation::set_valid(bool valid) { m_valid = valid; }
void VisualNavigation::set_position(Position pos) { m_pos = pos; }

// NavigationManager
NavigationManager::NavigationManager() : active_system(nullptr) {}

void NavigationManager::set_system(NavigationSystem* system) {
    active_system = system;
}

bool NavigationManager::is_available() const {
    return active_system != nullptr && active_system->is_valid();
}

Position NavigationManager::get_position() const {
    if (is_available()) {
        return active_system->get_position();
    }
    qDebug() << "Critical Error: No valid navigation system active!";
    return {0.0, 0.0, -1.0};
}

void NavigationManager::print_status() const {
    if (active_system) {
        qDebug().noquote() << QString("Active navigation system: %1").arg(active_system->system_name());
    } else {
        qDebug() << "Active navigation system: None";
    }
}

void NavigationManager::register_chain(NavigationSystem* gps, NavigationSystem* ins, NavigationSystem* visual) {
    fallback_priority.clear();
    if (gps) fallback_priority.push_back(gps);
    if (ins) fallback_priority.push_back(ins);
    if (visual) fallback_priority.push_back(visual);
    evaluate_failover();
}

bool NavigationManager::evaluate_failover() {
    for (NavigationSystem* sys : fallback_priority) {
        if (sys) {
            qDebug().noquote() << QString("%1: %2").arg(sys->system_name(), sys->is_valid() ? "valid" : "invalid");
            if (sys->is_valid() && (!active_system || active_system != sys)) {
                active_system = sys;
                print_status();
                return true;
            } else if (sys->is_valid() && active_system == sys) {
                return true;
            }
        }
    }
    active_system = nullptr;
    qDebug() << "Critical navigation error: all systems invalid!";
    return false;
}

NavigationSystem* NavigationManager::get_active_system() const {
    return active_system;
}
