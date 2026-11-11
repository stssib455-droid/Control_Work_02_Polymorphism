#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QProgressBar>
#include <QLabel>
#include <memory>

#include "uav.h"
#include "sensor.h"
#include "navigation.h"
#include "controller.h"
#include "autopilot.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    static void logOutput(const QString& msg);

private slots:
    void onRunFlightTest();
    void onSelectBestSensor();
    void onEvaluateNavigation();
    void onRunAutopilotCycle();
    void onRunPresetScenario(int scenarioIndex);
    void clearLog();

private:
    void setupUI();
    void applyDarkTheme();

    static QTextEdit* s_logViewer;

    // Модели данных
    std::unique_ptr<Quadcopter> m_quad;
    std::unique_ptr<Hexacopter> m_hexa;
    std::unique_ptr<Octocopter> m_octo;

    std::unique_ptr<GPS> m_gpsSensor;
    std::unique_ptr<GLONASS> m_glonassSensor;
    std::unique_ptr<VisualOdometry> m_voSensor;

    std::unique_ptr<GPSNavigation> m_gpsNav;
    std::unique_ptr<InertialNavigation> m_insNav;
    std::unique_ptr<VisualNavigation> m_voNav;
    NavigationManager m_navManager;

    std::unique_ptr<PIDController> m_pid;

    // Виджеты управления
    QComboBox* m_uavCombo;
    QCheckBox* m_gpsReadyCheck;
    QDoubleSpinBox* m_gpsAccSpin;
    QCheckBox* m_glonassReadyCheck;
    QDoubleSpinBox* m_glonassAccSpin;
    QCheckBox* m_voReadyCheck;
    QDoubleSpinBox* m_voAccSpin;

    QCheckBox* m_gpsNavValidCheck;
    QCheckBox* m_insNavValidCheck;
    QCheckBox* m_voNavValidCheck;

    QDoubleSpinBox* m_curAltSpin;
    QDoubleSpinBox* m_targetAltSpin;
    QCheckBox* m_controllerAttachedCheck;

    QLabel* m_activeNavLabel;
    QProgressBar* m_altitudeBar;
};

#endif // MAINWINDOW_H
