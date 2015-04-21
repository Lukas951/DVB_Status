#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dvbData.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
       void handleSelectionChanged(QString index);
       void refresh();

private:
    Ui::MainWindow *ui;
};

void ReadStatus(dvbdata p);
void getDevices();
void refreshData();
QString decodeCodeRate(int x);
QString decodeCodeRate(int x);
QString decodeModulation(int x);
QString decodeBandwidth(int x);
QString decodeTransmit(int x);
QString decodeGuard(int x);
QString decodeHierarchy(int x);
#endif // MAINWINDOW_H
