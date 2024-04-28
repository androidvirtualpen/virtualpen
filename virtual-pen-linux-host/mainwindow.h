#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include "displayscreentranslator.h"
#include "filepermissionvalidator.h"
#include "pressuretranslator.h"

using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static bool isDebugMode;
    MainWindow(QWidget *parent = nullptr);
    void captureStylusInput();
    ~MainWindow();

private slots:

    void on_usbDevicesListWidget_itemClicked(QListWidgetItem *item);

    void on_connectUsbButton_clicked();

    void on_deviceXSize_editingFinished();

    void on_deviceYSize_editingFinished();

    void on_displayStyleComboBox_currentIndexChanged(int index);

    void on_pressureSensitivitySlider_valueChanged(int value);

    void on_minimumPressureSlider_valueChanged(int value);

    void on_refreshUsbDevices_clicked();

    void on_deviceXSize_selectionChanged();

    void on_deviceYSize_selectionChanged();

    void on_connectUsbButton_2_clicked();

private:
    const QString setting_org = "com.github.androidvirtualpen";
    const QString setting_app = "virtualpen";
    const string y_device_setting_key = "/y_size";
    const string x_device_setting_key = "/x_size";
    const string min_pressure_setting_key = "/min_pressure";
    const string pressure_sensitivity_setting_key = "/pressure_sensitivity";
    const string display_style_setting_key = "/display_style";
    QDialog * dialog;
    FilePermissionValidator * filePermissionValidator;
    const int max_device_size = 999999999;

    Ui::MainWindow *ui;
    QMessageBox * messageBox;
    QSettings * settings;
    DisplayScreenTranslator * displayScreenTranslator;
    PressureTranslator * pressureTranslator;
    QMap<string, string>* usbDevices;
    int libUsbContext;
    string selectedDeviceIdentifier;
    string selectedDevice;
    QVariant getSetting(string settingKey);
    QVariant getSetting(string settingKey, QVariant defaultValue);
    void initDisplayStyles();
    void setSetting(string settingKey, QVariant value);
    void fetchUsbDevices();
    void populateUsbDevicesList();
    bool canConnectUsb();
    void updateUsbConnectButton();
    void loadDeviceConfig();
    void manageInputBoxStyle(QLineEdit * inputBox);
    void displayUDevPermissionFixIfNeeded();
    void displayFixForUDevPermissions();
    bool canWriteToFile(QString path);
    bool canWriteToAnyUsbDevice();
};
#endif // MAINWINDOW_H
