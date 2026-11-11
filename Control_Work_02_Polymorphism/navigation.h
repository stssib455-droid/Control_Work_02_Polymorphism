#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QString>
#include <QDebug>
#include <vector>

struct Position {
    double latitude = 55.75;
    double longitude = 37.61;
    double altitude = 150.0;
};

// Базовый абстрактный класс навигационной системы
class NavigationSystem {
public:
    virtual Position get_position() const = 0;
    virtual bool is_valid() const = 0;
    virtual QString system_name() const = 0;
    virtual ~NavigationSystem() = default;
};

// GPS навигация
class GPSNavigation : public NavigationSystem {
private:
    Position m_pos;
    bool m_valid;
public:
    explicit GPSNavigation(Position pos = {55.75, 37.61, 150.0}, bool valid = true);
    Position get_position() const override;
    bool is_valid() const override;
    QString system_name() const override;
    void set_valid(bool valid);
    void set_position(Position pos);
};

// Инерциальная навигационная система (INS)
class InertialNavigation : public NavigationSystem {
private:
    Position m_pos;
    bool m_valid;
public:
    explicit InertialNavigation(Position pos = {55.75, 37.61, 150.0}, bool valid = true);
    Position get_position() const override;
    bool is_valid() const override;
    QString system_name() const override;
    void set_valid(bool valid);
    void set_position(Position pos);
};

// Визуальная навигация
class VisualNavigation : public NavigationSystem {
private:
    Position m_pos;
    bool m_valid;
public:
    explicit VisualNavigation(Position pos = {55.75, 37.61, 150.0}, bool valid = true);
    Position get_position() const override;
    bool is_valid() const override;
    QString system_name() const override;
    void set_valid(bool valid);
    void set_position(Position pos);
};

// Отказоустойчивый менеджер навигации
class NavigationManager {
private:
    NavigationSystem* active_system; // Инкапсуляция
    std::vector<NavigationSystem*> fallback_priority;

public:
    NavigationManager();
    void set_system(NavigationSystem* system);
    bool is_available() const;
    Position get_position() const;
    void print_status() const;

    void register_chain(NavigationSystem* gps, NavigationSystem* ins, NavigationSystem* visual);
    bool evaluate_failover();
    NavigationSystem* get_active_system() const;
};

#endif // NAVIGATION_H
