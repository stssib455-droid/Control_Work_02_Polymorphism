#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSplitter>
#include <QDateTime>
#include <QApplication>
#include <algorithm>

QTextEdit* MainWindow::s_logViewer = nullptr;

void MainWindow::logOutput(const QString& msg) {
    if (s_logViewer) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        s_logViewer->append(QString("<span style='color:#5c6bc0;'>[%1]</span> %2").arg(timestamp, msg));
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("GCS Onyx | Автономная станция БПЛА (ПО ОНИ - Вариант 2)");
    resize(1240, 840);

    // Инициализация моделей
    m_quad = std::make_unique<Quadcopter>();
    m_hexa = std::make_unique<Hexacopter>();
    m_octo = std::make_unique<Octocopter>();

    m_gpsSensor = std::make_unique<GPS>(10, 1.5, true);
    m_glonassSensor = std::make_unique<GLONASS>(8, 2.0, true);
    m_voSensor = std::make_unique<VisualOdometry>(3.2, true);

    m_gpsNav = std::make_unique<GPSNavigation>(Position{55.75, 37.61, 150.0}, true);
    m_insNav = std::make_unique<InertialNavigation>(Position{55.75, 37.61, 150.0}, true);
    m_voNav = std::make_unique<VisualNavigation>(Position{55.75, 37.61, 150.0}, true);

    m_navManager.register_chain(m_gpsNav.get(), m_insNav.get(), m_voNav.get());
    m_pid = std::make_unique<PIDController>(0.5, 0.0, 0.0);

    setupUI();
    applyDarkTheme();
    onEvaluateNavigation();
}

MainWindow::~MainWindow() {
    s_logViewer = nullptr;
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    mainLayout->addWidget(splitter);

    // Левая панель
    QWidget* leftPanel = new QWidget(this);
    QVBoxLayout* controlLayout = new QVBoxLayout(leftPanel);

    // Блок 1: UAV
    QGroupBox* uavBox = new QGroupBox("Задание 1. Конфигурация летательного аппарата (UAV)", leftPanel);
    QHBoxLayout* uavLayout = new QHBoxLayout(uavBox);
    m_uavCombo = new QComboBox(uavBox);
    m_uavCombo->addItems({"Quadcopter (4 мотора)", "Hexacopter (6 моторов)", "Octocopter (8 моторов)"});
    QPushButton* testUavBtn = new QPushButton("Тест и print_vehicle_info()", uavBox);
    connect(testUavBtn, &QPushButton::clicked, this, &MainWindow::onRunFlightTest);
    uavLayout->addWidget(m_uavCombo);
    uavLayout->addWidget(testUavBtn);
    controlLayout->addWidget(uavBox);

    // Блок 2: Сенсоры
    QGroupBox* sensorBox = new QGroupBox("Задание 2. Навигационные сенсоры", leftPanel);
    QGridLayout* sensorGrid = new QGridLayout(sensorBox);

    m_gpsReadyCheck = new QCheckBox("GPS готов", sensorBox);
    m_gpsReadyCheck->setChecked(true);
    m_gpsAccSpin = new QDoubleSpinBox(sensorBox);
    m_gpsAccSpin->setRange(0.1, 50.0);
    m_gpsAccSpin->setValue(1.5);
    m_gpsAccSpin->setSuffix(" м");

    m_glonassReadyCheck = new QCheckBox("ГЛОНАСС готов", sensorBox);
    m_glonassReadyCheck->setChecked(true);
    m_glonassAccSpin = new QDoubleSpinBox(sensorBox);
    m_glonassAccSpin->setRange(0.1, 50.0);
    m_glonassAccSpin->setValue(2.0);
    m_glonassAccSpin->setSuffix(" м");

    m_voReadyCheck = new QCheckBox("Visual Odometry готов", sensorBox);
    m_voReadyCheck->setChecked(true);
    m_voAccSpin = new QDoubleSpinBox(sensorBox);
    m_voAccSpin->setRange(0.1, 50.0);
    m_voAccSpin->setValue(3.2);
    m_voAccSpin->setSuffix(" м");

    QPushButton* bestSensorBtn = new QPushButton("choose_best_sensor()", sensorBox);
    connect(bestSensorBtn, &QPushButton::clicked, this, &MainWindow::onSelectBestSensor);

    sensorGrid->addWidget(m_gpsReadyCheck, 0, 0);
    sensorGrid->addWidget(new QLabel("Погрешность:"), 0, 1);
    sensorGrid->addWidget(m_gpsAccSpin, 0, 2);

    sensorGrid->addWidget(m_glonassReadyCheck, 1, 0);
    sensorGrid->addWidget(new QLabel("Погрешность:"), 1, 1);
    sensorGrid->addWidget(m_glonassAccSpin, 1, 2);

    sensorGrid->addWidget(m_voReadyCheck, 2, 0);
    sensorGrid->addWidget(new QLabel("Погрешность:"), 2, 1);
    sensorGrid->addWidget(m_voAccSpin, 2, 2);

    sensorGrid->addWidget(bestSensorBtn, 3, 0, 1, 3);
    controlLayout->addWidget(sensorBox);

    // Блок 3: Отказоустойчивая навигация
    QGroupBox* navBox = new QGroupBox("Задание 3. Отказоустойчивая навигация (GPS -> INS -> Visual)", leftPanel);
    QVBoxLayout* navLayout = new QVBoxLayout(navBox);

    QHBoxLayout* navCheckLayout = new QHBoxLayout();
    m_gpsNavValidCheck = new QCheckBox("GPS Valid", navBox);
    m_gpsNavValidCheck->setChecked(true);
    m_insNavValidCheck = new QCheckBox("INS Valid", navBox);
    m_insNavValidCheck->setChecked(true);
    m_voNavValidCheck = new QCheckBox("Visual Valid", navBox);
    m_voNavValidCheck->setChecked(true);

    navCheckLayout->addWidget(m_gpsNavValidCheck);
    navCheckLayout->addWidget(m_insNavValidCheck);
    navCheckLayout->addWidget(m_voNavValidCheck);
    navLayout->addLayout(navCheckLayout);

    connect(m_gpsNavValidCheck, &QCheckBox::toggled, this, &MainWindow::onEvaluateNavigation);
    connect(m_insNavValidCheck, &QCheckBox::toggled, this, &MainWindow::onEvaluateNavigation);
    connect(m_voNavValidCheck, &QCheckBox::toggled, this, &MainWindow::onEvaluateNavigation);

    m_activeNavLabel = new QLabel("Активная навигация: GPS", navBox);
    m_activeNavLabel->setStyleSheet("font-weight: bold; color: #00e5ff; font-size: 13px;");
    navLayout->addWidget(m_activeNavLabel);
    controlLayout->addWidget(navBox);

    // Блок 4: Автопилот и 5 сценариев
    QGroupBox* apBox = new QGroupBox("Интеграция: Autopilot & 5 обязательных сценариев", leftPanel);
    QVBoxLayout* apLayout = new QVBoxLayout(apBox);

    QGridLayout* altGrid = new QGridLayout();
    m_curAltSpin = new QDoubleSpinBox(apBox);
    m_curAltSpin->setRange(-100.0, 1000.0);
    m_curAltSpin->setValue(150.0);
    m_curAltSpin->setSuffix(" м");

    m_targetAltSpin = new QDoubleSpinBox(apBox);
    m_targetAltSpin->setRange(0.0, 1000.0);
    m_targetAltSpin->setValue(200.0);
    m_targetAltSpin->setSuffix(" м");

    m_controllerAttachedCheck = new QCheckBox("Регулятор подключен (PID)", apBox);
    m_controllerAttachedCheck->setChecked(true);

    altGrid->addWidget(new QLabel("Текущая высота:"), 0, 0);
    altGrid->addWidget(m_curAltSpin, 0, 1);
    altGrid->addWidget(new QLabel("Целевая высота:"), 0, 2);
    altGrid->addWidget(m_targetAltSpin, 0, 3);
    altGrid->addWidget(m_controllerAttachedCheck, 1, 0, 1, 4);
    apLayout->addLayout(altGrid);

    QPushButton* runCycleBtn = new QPushButton("▶ ВЫПОЛНИТЬ ЦИКЛ АВТОПИЛОТА (run_cycle)", apBox);
    runCycleBtn->setStyleSheet("background-color: #00c853; color: white; font-weight: bold; padding: 8px; border-radius: 4px;");
    connect(runCycleBtn, &QPushButton::clicked, this, &MainWindow::onRunAutopilotCycle);
    apLayout->addWidget(runCycleBtn);

    QLabel* scenLabel = new QLabel("Быстрый запуск сценариев из методички:", apBox);
    scenLabel->setStyleSheet("color: #b0bec5; font-size: 11px; margin-top: 6px;");
    apLayout->addWidget(scenLabel);

    QHBoxLayout* scRow1 = new QHBoxLayout();
    QPushButton* sc1Btn = new QPushButton("1. Штатный (150м)", apBox);
    QPushButton* sc2Btn = new QPushButton("2. Отказ сенсора", apBox);
    QPushButton* sc3Btn = new QPushButton("3. Nav Invalid", apBox);
    scRow1->addWidget(sc1Btn);
    scRow1->addWidget(sc2Btn);
    scRow1->addWidget(sc3Btn);
    apLayout->addLayout(scRow1);

    QHBoxLayout* scRow2 = new QHBoxLayout();
    QPushButton* sc4Btn = new QPushButton("4. Controller = null", apBox);
    QPushButton* sc5Btn = new QPushButton("5. Отрицат. высота", apBox);
    scRow2->addWidget(sc4Btn);
    scRow2->addWidget(sc5Btn);
    apLayout->addLayout(scRow2);

    connect(sc1Btn, &QPushButton::clicked, this, [this](){ onRunPresetScenario(1); });
    connect(sc2Btn, &QPushButton::clicked, this, [this](){ onRunPresetScenario(2); });
    connect(sc3Btn, &QPushButton::clicked, this, [this](){ onRunPresetScenario(3); });
    connect(sc4Btn, &QPushButton::clicked, this, [this](){ onRunPresetScenario(4); });
    connect(sc5Btn, &QPushButton::clicked, this, [this](){ onRunPresetScenario(5); });

    controlLayout->addWidget(apBox);
    controlLayout->addStretch();

    // Правая панель: HUD и терминал
    QWidget* rightPanel = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    QGroupBox* hudBox = new QGroupBox("Телеметрия высоты БПЛА", rightPanel);
    QVBoxLayout* hudLayout = new QVBoxLayout(hudBox);
    m_altitudeBar = new QProgressBar(hudBox);
    m_altitudeBar->setRange(0, 300);
    m_altitudeBar->setValue(150);
    m_altitudeBar->setFormat("Высота: %v м / 300 м");
    m_altitudeBar->setStyleSheet("QProgressBar { border: 1px solid #37474f; border-radius: 4px; text-align: center; height: 26px; font-weight: bold; color: #fff; background-color: #1e272e; }"
                                 "QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #00b0ff, stop:1 #00e676); }");
    hudLayout->addWidget(m_altitudeBar);
    rightLayout->addWidget(hudBox);

    QGroupBox* logBox = new QGroupBox("Системный терминал (вывод через qDebug())", rightPanel);
    QVBoxLayout* logLayout = new QVBoxLayout(logBox);

    s_logViewer = new QTextEdit(logBox);
    s_logViewer->setReadOnly(true);
    s_logViewer->setStyleSheet("background-color: #0a0e14; color: #e6edf3; font-family: 'Consolas', monospace; font-size: 12px; border: 1px solid #1f2937; border-radius: 4px; padding: 6px;");
    logLayout->addWidget(s_logViewer);

    QPushButton* clearBtn = new QPushButton("Очистить терминал", logBox);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearLog);
    logLayout->addWidget(clearBtn);

    rightLayout->addWidget(logBox);

    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 5);
    splitter->setStretchFactor(1, 5);
}

void MainWindow::onRunFlightTest() {
    qDebug() << "================ FLIGHT TEST ================";
    int idx = m_uavCombo->currentIndex();
    if (idx == 0) print_vehicle_info(*m_quad);
    else if (idx == 1) print_vehicle_info(*m_hexa);
    else if (idx == 2) print_vehicle_info(*m_octo);
    qDebug() << "=============================================";
}

void MainWindow::onSelectBestSensor() {
    qDebug() << "============ SENSOR ACCURACY TEST ===========";
    m_gpsSensor->set_ready(m_gpsReadyCheck->isChecked());
    m_gpsSensor->set_accuracy(m_gpsAccSpin->value());

    m_glonassSensor->set_ready(m_glonassReadyCheck->isChecked());
    m_glonassSensor->set_accuracy(m_glonassAccSpin->value());

    m_voSensor->set_ready(m_voReadyCheck->isChecked());
    m_voSensor->set_accuracy(m_voAccSpin->value());

    std::vector<Sensor*> sensors = { m_gpsSensor.get(), m_glonassSensor.get(), m_voSensor.get() };

    for (Sensor* s : sensors) {
        if (auto* gps = dynamic_cast<GPS*>(s)) {
            qDebug().noquote() << QString("GPS: satellites = %1, accuracy = %2 m").arg(gps->satellites()).arg(gps->accuracy());
        } else if (auto* glo = dynamic_cast<GLONASS*>(s)) {
            qDebug().noquote() << QString("GLONASS: satellites = %1, accuracy = %2 m").arg(glo->satellites()).arg(glo->accuracy());
        } else {
            qDebug().noquote() << QString("Visual odometry: accuracy = %1 m").arg(s->accuracy());
        }
    }

    choose_best_sensor(sensors);
    qDebug() << "=============================================";
}

void MainWindow::onEvaluateNavigation() {
    m_gpsNav->set_valid(m_gpsNavValidCheck->isChecked());
    m_insNav->set_valid(m_insNavValidCheck->isChecked());
    m_voNav->set_valid(m_voNavValidCheck->isChecked());

    qDebug() << "--- Проверка отказоустойчивой навигации ---";
    m_navManager.evaluate_failover();

    NavigationSystem* active = m_navManager.get_active_system();
    if (active) {
        m_activeNavLabel->setText(QString("Активная навигация: %1").arg(active->system_name()));
        m_activeNavLabel->setStyleSheet("font-weight: bold; color: #00e676; font-size: 13px;");
    } else {
        m_activeNavLabel->setText("Активная навигация: НЕДОСТУПНА (CRITICAL)");
        m_activeNavLabel->setStyleSheet("font-weight: bold; color: #ff1744; font-size: 13px;");
    }
}

void MainWindow::onRunAutopilotCycle() {
    qDebug() << "---------------------------------------------";
    int clampedAlt = std::clamp(static_cast<int>(m_curAltSpin->value()), 0, 300);
    m_altitudeBar->setValue(clampedAlt);

    UAV* vehicle = nullptr;
    int idx = m_uavCombo->currentIndex();
    if (idx == 0) vehicle = m_quad.get();
    else if (idx == 1) vehicle = m_hexa.get();
    else if (idx == 2) vehicle = m_octo.get();

    m_gpsSensor->set_ready(m_gpsReadyCheck->isChecked());
    m_glonassSensor->set_ready(m_glonassReadyCheck->isChecked());
    m_voSensor->set_ready(m_voReadyCheck->isChecked());
    std::vector<Sensor*> sensors = { m_gpsSensor.get(), m_glonassSensor.get(), m_voSensor.get() };

    m_gpsNav->set_valid(m_gpsNavValidCheck->isChecked());
    m_insNav->set_valid(m_insNavValidCheck->isChecked());
    m_voNav->set_valid(m_voNavValidCheck->isChecked());
    m_navManager.evaluate_failover();

    NavigationSystem* activeNav = m_navManager.get_active_system();
    if (activeNav) {
        Position p = activeNav->get_position();
        p.altitude = m_curAltSpin->value();
        if (auto* g = dynamic_cast<GPSNavigation*>(activeNav)) g->set_position(p);
        else if (auto* in = dynamic_cast<InertialNavigation*>(activeNav)) in->set_position(p);
        else if (auto* vn = dynamic_cast<VisualNavigation*>(activeNav)) vn->set_position(p);
    }

    FlightController* ctrl = m_controllerAttachedCheck->isChecked() ? m_pid.get() : nullptr;

    Autopilot ap(vehicle, sensors, activeNav, ctrl, m_targetAltSpin->value());
    ap.run_cycle();
    qDebug() << "---------------------------------------------";
}

void MainWindow::onRunPresetScenario(int sc) {
    qDebug() << QString("========== ЗАПУСК СЦЕНАРИЯ №%1 ==========").arg(sc);
    switch (sc) {
    case 1:
        m_gpsReadyCheck->setChecked(true);
        m_glonassReadyCheck->setChecked(true);
        m_voReadyCheck->setChecked(true);
        m_gpsNavValidCheck->setChecked(true);
        m_curAltSpin->setValue(150.0);
        m_controllerAttachedCheck->setChecked(true);
        break;
    case 2:
        m_gpsReadyCheck->setChecked(false);
        break;
    case 3:
        m_gpsNavValidCheck->setChecked(false);
        m_insNavValidCheck->setChecked(false);
        m_voNavValidCheck->setChecked(false);
        break;
    case 4:
        m_controllerAttachedCheck->setChecked(false);
        break;
    case 5:
        m_gpsReadyCheck->setChecked(true);
        m_glonassReadyCheck->setChecked(true);
        m_voReadyCheck->setChecked(true);
        m_gpsNavValidCheck->setChecked(true);
        m_curAltSpin->setValue(-25.0);
        m_controllerAttachedCheck->setChecked(true);
        break;
    }
    onRunAutopilotCycle();
}

void MainWindow::clearLog() {
    if (s_logViewer) s_logViewer->clear();
}

void MainWindow::applyDarkTheme() {
    qApp->setStyle("Fusion");
    setStyleSheet(
        "QMainWindow { background-color: #0f141c; }"
        "QWidget { color: #cfd8dc; font-family: 'Segoe UI', Arial, sans-serif; }"
        "QGroupBox { font-weight: bold; border: 1px solid #263238; border-radius: 6px; margin-top: 10px; padding-top: 10px; background-color: #171f2b; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; color: #00e5ff; }"
        "QPushButton { background-color: #212d3d; border: 1px solid #37474f; border-radius: 4px; padding: 6px 12px; color: #eceff1; }"
        "QPushButton:hover { background-color: #2c3b4f; border-color: #00e5ff; }"
        "QPushButton:pressed { background-color: #1a232f; }"
        "QComboBox, QDoubleSpinBox { background-color: #10161f; border: 1px solid #37474f; border-radius: 4px; padding: 4px; color: #eceff1; }"
        "QCheckBox { spacing: 6px; }"
        "QCheckBox::indicator { width: 16px; height: 16px; border: 1px solid #37474f; background: #10161f; border-radius: 3px; }"
        "QCheckBox::indicator:checked { background: #00e5ff; border-color: #00e5ff; }"
    );
}
