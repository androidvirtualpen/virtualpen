#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <linux/uinput.h>
#include<unistd.h>
#include <sstream>
#include <QtConcurrent/QtConcurrent>
#include </usr/include/libusb-1.0/libusb.h>
#include "linux-adk.h"
#include "virtualstylus.h"
using namespace QtConcurrent;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)   
    , ui(new Ui::MainWindow)
    , selectedDeviceIdentifier("")
    , selectedDevice("")
    , usbDevices(new QMap<string, string>())
    , displayScreenTranslator(new DisplayScreenTranslator())
    , pressureTranslator(new PressureTranslator())
{
    settings = new QSettings("me", "android-pen-emulator");
    ui->setupUi(this);
    initDisplayStyles();
    libUsbContext = libusb_init(NULL);
    updateUsbConnectButton();
    populateUsbDevicesList();
}

void MainWindow::captureStylusInput(){
    VirtualStylus* virtualStylus = new VirtualStylus(displayScreenTranslator, pressureTranslator);
    virtualStylus->initializeStylus();
    capture(selectedDevice, virtualStylus);
}

void MainWindow::populateUsbDevicesList(){
    ui->usbDevicesListWidget->clear();
    fetchUsbDevices();
    foreach(const string key, usbDevices->keys()){
        ui->usbDevicesListWidget->addItem(QString::fromStdString(key));
    }
}


void MainWindow::on_usbDevicesListWidget_itemClicked(QListWidgetItem *item){
    selectedDeviceIdentifier = item->text().toStdString();
    selectedDevice = usbDevices->value(selectedDeviceIdentifier);
    loadDeviceConfig();
    updateUsbConnectButton();
}

bool MainWindow::canConnectUsb(){
    if(selectedDevice != "" &&
        displayScreenTranslator->size_x != -1 &&
        displayScreenTranslator->size_y != -1){
        return true;
    }
    else{
        return false;
    }
}

void MainWindow::updateUsbConnectButton(){
    ui->connectUsbButton->setEnabled(canConnectUsb());
}


void MainWindow::on_connectUsbButton_clicked(){
    QFuture<void> ignored = QtConcurrent::run([this] { return captureStylusInput(); });
    ui->connectionStatusLabel->setText(QString::fromUtf8("Connected!"));
    ui->connectUsbButton->setEnabled(false);
    ui->refreshUsbDevices->setEnabled(false);
    ui->usbDevicesListWidget->setEnabled(false);
}

void MainWindow::fetchUsbDevices(){
    usbDevices->clear();
    libusb_device ** devicesList = NULL;
    ssize_t nbDevices = libusb_get_device_list(NULL, &devicesList);
    int err = 0;
    for(ssize_t i = 0; i < nbDevices; i++){
        libusb_device *dev = devicesList[i];
        struct libusb_device_descriptor desc;
        err = libusb_get_device_descriptor(dev, &desc);
        libusb_device_handle *handle = NULL;
        err = libusb_open(dev, &handle);
        if (err) {
            printf("Unable to open device...\n");
            continue;
        }
        unsigned char buf[100];
        int descLength = -1;
        descLength = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, buf, sizeof(buf));
        if(descLength < 0){
            continue;
        }
        string manufacturer = reinterpret_cast<char*>(buf);

        descLength = libusb_get_string_descriptor_ascii(handle, desc.iProduct, buf, sizeof(buf));
        if(descLength < 0){
            continue;
        }
        string product = reinterpret_cast<char*>(buf);

        descLength = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, buf, sizeof(buf));
        if(descLength < 0){
            continue;
        }
        string serialNumber = reinterpret_cast<char*>(buf);

        std::ostringstream ss;
        ss<< std::hex << desc.idVendor << ":" << std::hex << desc.idProduct;
        string device = ss.str();

        usbDevices->insert(manufacturer + "-" + product + " (" + serialNumber + ")", device);
    }
}

void MainWindow::initDisplayStyles(){
    ui->displayStyleComboBox->addItem("  Stretched", static_cast<int>(DisplayStyle::stretched));
    ui->displayStyleComboBox->addItem("  Fixed", static_cast<int>(DisplayStyle::fixed));
}


void MainWindow::on_deviceXSize_editingFinished()
{
    displayScreenTranslator->size_x = ui->deviceXSize->text().toInt();
    setSetting(x_device_setting_key, displayScreenTranslator->size_x);
    updateUsbConnectButton();
}


void MainWindow::on_deviceYSize_editingFinished()
{
    displayScreenTranslator->size_y = ui->deviceYSize->text().toInt();
    setSetting(y_device_setting_key, displayScreenTranslator->size_y);
    updateUsbConnectButton();
}


void MainWindow::on_displayStyleComboBox_currentIndexChanged(int index)
{
    int displayStyleInt = ui->displayStyleComboBox->currentData().toInt();
    displayScreenTranslator->displayStyle = static_cast<DisplayStyle>(displayStyleInt);
    setSetting(display_style_setting_key, displayStyleInt);
}

void MainWindow::on_pressureSensitivitySlider_valueChanged(int value)
{
    pressureTranslator->sensitivity = value;
    setSetting(pressure_sensitivity_setting_key, value);
}


void MainWindow::on_minimumPressureSlider_valueChanged(int value)
{
    pressureTranslator->minPressure = value;
    setSetting(min_pressure_setting_key, value);
}

void MainWindow::loadDeviceConfig(){
    displayScreenTranslator->size_x = getSetting(x_device_setting_key).toInt();
    displayScreenTranslator->size_y = getSetting(y_device_setting_key).toInt();
    int displayStyleInt = getSetting(display_style_setting_key).toInt();
    int index = ui->displayStyleComboBox->findData(displayStyleInt);
    if ( index != -1 ) {
        displayScreenTranslator->displayStyle = static_cast<DisplayStyle>(displayStyleInt);
        ui->displayStyleComboBox->setCurrentIndex(index);
    }
    else{
        displayScreenTranslator->displayStyle = DisplayStyle::stretched;
        ui->displayStyleComboBox->setCurrentIndex(ui->displayStyleComboBox->findData(static_cast<int>(DisplayStyle::stretched)));
    }

    ui->displayStyleComboBox->activated(0);
    pressureTranslator->minPressure = getSetting(min_pressure_setting_key).toInt();
    ui->minimumPressureSlider->setValue(pressureTranslator->minPressure);
    pressureTranslator->sensitivity = getSetting(pressure_sensitivity_setting_key).toInt();
    ui->pressureSensitivitySlider->setValue(pressureTranslator->sensitivity);
    ui->deviceXSize->setText(QString::number(displayScreenTranslator->size_x));
    ui->deviceYSize->setText(QString::number(displayScreenTranslator->size_y));
}


QVariant MainWindow::getSetting(string settingKey){
    return settings->value(QString::fromStdString(selectedDeviceIdentifier + settingKey));
}

void MainWindow::setSetting(string settingKey, QVariant value){
    return settings->setValue(QString::fromStdString(selectedDeviceIdentifier + settingKey), value);
}

void MainWindow::on_refreshUsbDevices_clicked()
{
    populateUsbDevicesList();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete usbDevices;
    delete displayScreenTranslator;
    delete pressureTranslator;
    delete settings;
}




