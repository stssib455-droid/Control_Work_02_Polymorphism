#ifndef SENSOR_H
#define SENSOR_H

#include <QString>
#include <QDebug>
#include <vector>

// Абстрактный базовый класс сенсора
class Sensor {
public:
    virtual double read() = 0;
    virtual bool is_ready() const = 0;
    virtual QString name() const = 0;
    virtual double accuracy() const = 0;
    virtual ~Sensor() = default;
};

// Вариант 2: GPS
class GPS : public Sensor {
private:
    int m_satellites;
    double m_accuracy;
    bool m_ready;
public:
    explicit GPS(int satellites = 10, double acc = 1.5, bool ready = true);
    double read() override;
    bool is_ready() const override;
    QString name() const override;
    double accuracy() const override;

    void set_ready(bool ready);
    void set_satellites(int count);
    void set_accuracy(double acc);
    int satellites() const;
};

// Вариант 2: GLONASS
class GLONASS : public Sensor {
private:
    int m_satellites;
    double m_accuracy;
    bool m_ready;
public:
    explicit GLONASS(int satellites = 8, double acc = 2.0, bool ready = true);
    double read() override;
    bool is_ready() const override;
    QString name() const override;
    double accuracy() const override;

    void set_ready(bool ready);
    void set_satellites(int count);
    void set_accuracy(double acc);
    int satellites() const;
};

// Вариант 2: Visual Odometry
class VisualOdometry : public Sensor {
private:
    double m_accuracy;
    bool m_ready;
public:
    explicit VisualOdometry(double acc = 3.2, bool ready = true);
    double read() override;
    bool is_ready() const override;
    QString name() const override;
    double accuracy() const override;

    void set_ready(bool ready);
    void set_accuracy(double acc);
};

// Выбор лучшего доступного источника
Sensor* choose_best_sensor(const std::vector<Sensor*>& sensors);

#endif // SENSOR_H
