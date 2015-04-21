#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

std::list<dvbdata> dvb;
Ui::MainWindow* ui1;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui1 = ui;
    ui->setupUi(this);
    connect(ui->comboBox , SIGNAL(currentIndexChanged(QString)),this ,SLOT(handleSelectionChanged(QString)));

    getDevices();
    //ReadStatus();
     QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));


    timer->start(5000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleSelectionChanged(QString index){

    dvbdata par = dvbdata();

    std::list<dvbdata>::iterator p = dvb.begin();
    while(p != dvb.end()) {
      if(p->info.name == index){
        par = *p;
        break;
      }
      p++;
    }

    ReadStatus(par);

}


void ReadStatus(dvbdata p){
    ui1->lineEdit_2->setText(QString::number(p.p.frequency));

    int ss = p.strength / 65536 * 100;
    if(ss>0){
    ui1->progressBar->setValue(ss);
    }else{
        ui1->progressBar->setValue(-ss);
    }

    switch(p.info.type){
        case 0:
            ui1->lineEdit->setText("DVB-S");
            break;
        case 1:
            ui1->lineEdit->setText("DVB-C");
            break;
        case 2:
            ui1->lineEdit->setText("DVB-T");
            break;

        default:
            break;
    }


    ui1->lineEdit_3->setText(QString::number(p.SNR));
    ui1->lineEdit_4->setText(QString::number(p.BER));

    switch(p.p.inversion){
        case 0:
            ui1->lineEdit_5->setText("INVERSION_OFF");
            break;
        case 1:
            ui1->lineEdit_5->setText("INVERSION_ON");
            break;
        case 2:
            ui1->lineEdit_5->setText("INVERSION_AUTO");
            break;
    default:
        break;
    }


    QString text = "";

    ui1->label_8->setVisible(false);
    ui1->lineEdit_6->setVisible(false);

    ui1->label_9->setVisible(false);
    ui1->lineEdit_7->setVisible(false);

    ui1->label_10->setVisible(false);
    ui1->lineEdit_8->setVisible(false);

    ui1->label_11->setVisible(false);
    ui1->lineEdit_9->setVisible(false);

    ui1->label_12->setVisible(false);
    ui1->lineEdit_10->setVisible(false);

    ui1->label_13->setVisible(false);
    ui1->lineEdit_11->setVisible(false);

    ui1->label_14->setVisible(false);
    ui1->lineEdit_12->setVisible(false);

    switch(p.info.type){
        case 0:
            ui1->label_8->setText("Symbol rate:");
            ui1->lineEdit_6->setText(QString::number(p.p.u.qpsk.symbol_rate));
            ui1->label_9->setText("Code rate:");
            ui1->lineEdit_7->setText(decodeCodeRate(p.p.u.qpsk.fec_inner));

            ui1->label_8->setVisible(true);
            ui1->lineEdit_6->setVisible(true);
            ui1->label_9->setVisible(true);
            ui1->lineEdit_7->setVisible(true);

            break;
        case 1:
            ui1->label_8->setText("Symbol rate:");
            ui1->lineEdit_6->setText(QString::number(p.p.u.qam.symbol_rate));
            ui1->label_9->setText("Code rate:");
            ui1->lineEdit_7->setText(decodeCodeRate(p.p.u.qam.fec_inner));
            ui1->label_10->setText("Modulation:");
            ui1->lineEdit_8->setText(decodeModulation(p.p.u.qam.modulation));

            ui1->label_8->setVisible(true);
            ui1->lineEdit_6->setVisible(true);
            ui1->label_9->setVisible(true);
            ui1->lineEdit_7->setVisible(true);
            ui1->label_10->setVisible(true);
            ui1->lineEdit_8->setVisible(true);
            break;
        case 2:
            ui1->label_8->setText("Bandwidth:");
            ui1->label_9->setText("Code rate HP:");
            ui1->label_10->setText("Code rate LP:");
            ui1->label_11->setText("Modulation:");
            ui1->label_12->setText("Transmission mode:");
            ui1->label_13->setText("Guard interval:");
            ui1->label_14->setText("Hierarchy information:");

            ui1->lineEdit_6->setText(decodeBandwidth(p.p.u.ofdm.bandwidth));
            ui1->lineEdit_7->setText(decodeCodeRate(p.p.u.ofdm.code_rate_HP));
            ui1->lineEdit_8->setText(decodeCodeRate(p.p.u.ofdm.code_rate_LP));
            ui1->lineEdit_9->setText(decodeModulation(p.p.u.ofdm.constellation));
            ui1->lineEdit_10->setText(decodeTransmit(p.p.u.ofdm.transmission_mode));
            ui1->lineEdit_11->setText(decodeGuard(p.p.u.ofdm.guard_interval));
            ui1->lineEdit_12->setText(decodeHierarchy(p.p.u.ofdm.hierarchy_information));


            ui1->label_8->setVisible(true);
            ui1->lineEdit_6->setVisible(true);

            ui1->label_9->setVisible(true);
            ui1->lineEdit_7->setVisible(true);

            ui1->label_10->setVisible(true);
            ui1->lineEdit_8->setVisible(true);

            ui1->label_11->setVisible(true);
            ui1->lineEdit_9->setVisible(true);

            ui1->label_12->setVisible(true);
            ui1->lineEdit_10->setVisible(true);

            ui1->label_13->setVisible(true);
            ui1->lineEdit_11->setVisible(true);

            ui1->label_14->setVisible(true);
            ui1->lineEdit_12->setVisible(true);
            break;
    default:
        break;


    }
}

void _ReadStatus(){
    QString index = ui1->comboBox->currentText();

    dvbdata par = dvbdata();

    std::list<dvbdata>::iterator p = dvb.begin();
    while(p != dvb.end()) {
      if(p->info.name == index){
        par = *p;
        break;
      }
      p++;
    }

    ReadStatus(par);

}


#define FRONTENDDEVICE "/dev/dvb/adapter%d/frontend%d"
#define FRONTEND 0

void getDevices(){



    for(int x = 0; x< 8; x++){

        char frdev[128];
        snprintf(frdev, sizeof(frdev), FRONTENDDEVICE, x, FRONTEND);

        int defd;

        if ((defd = open(frdev, O_RDONLY )) < 0) {
            continue;
        }else{

            dvbdata d = dvbdata();
            d.adapter = x;
            d.frontend = FRONTEND;

            ioctl(defd, FE_GET_INFO, &d.info);
            ioctl(defd, FE_GET_FRONTEND, &d.p);
            ioctl(defd, FE_READ_SIGNAL_STRENGTH, &d.strength);
            ioctl(defd, FE_READ_BER, &d.BER);
            ioctl(defd, FE_READ_SNR, &d.SNR);

            dvb.push_back(d);

            close(defd);

        }
    }

    std::list<dvbdata>::iterator p = dvb.begin();
    while(p != dvb.end()) {
      ui1->comboBox->addItem(p->info.name);
      p++;
    }

    _ReadStatus();

}

void MainWindow::refresh(){
    refreshData();
    _ReadStatus();
}



void refreshData(){

    QString index = ui1->comboBox->currentText();

    std::list<dvbdata>::iterator p = dvb.begin();
    while(p != dvb.end()) {
      if(p->info.name == index){


          char frdev[128];
          snprintf(frdev, sizeof(frdev), FRONTENDDEVICE, p->adapter, p->frontend);

          int defd;

          if ((defd = open(frdev, O_RDONLY )) < 0) {
              break;
          }else{


              //ioctl(defd, FE_GET_INFO, &p->info);
              ioctl(defd, FE_GET_FRONTEND, &p->p);
              ioctl(defd, FE_READ_SIGNAL_STRENGTH, &p->strength);
              ioctl(defd, FE_READ_BER, &p->BER);
              ioctl(defd, FE_READ_SNR, &p->SNR);

              close(defd);

          }


        break;

      }
      p++;
    }

}


QString decodeCodeRate(int x){
    switch(x){
    case 0:
        return "FEC_NONE";
    case 1:
        return "FEC_1_2";
    case 2:
        return "FEC_2_3";
    case 3:
        return "FEC_3_4";
    case 4:
        return "FEC_4_5";
    case 5:
        return"FEC_5_6";
    case 6:
        return "FEC_6_7";
    case 7:
        return "FEC_7_8";
    case 8:
        return "FEC_8_9";
    case 9:
        return "FEC_AUTO";
     default:
        return "";

    }
}

QString decodeModulation(int x){
    switch(x){
    case 0:
        return "QPSK";
    case 1:
        return "QAM_16";
    case 2:
        return "QAM_32";
    case 3:
        return "QAM_64";
    case 4:
        return "QAM_128";
    case 5:
        return "QAM_256";
    case 6:
        return "QAM_AUTO";
    default:
        return "";

    }
}

QString decodeBandwidth(int x){
    switch(x){
    case 0:
        return "BANDWIDTH_8_MHZ";
    case 1:
        return "BANDWIDTH_7_MHZ";
    case 2:
        return "BANDWIDTH_6_MHZ";
    case 3:
        return "BANDWIDTH_AUTO";
    default:
        return "";

    }
}

QString decodeTransmit(int x){
    switch(x){
    case 0:
        return "TRANSMISSION_MODE_2K";
    case 1:
        return "TRANSMISSION_MODE_8K";
    case 2:
        return "TRANSMISSION_MODE_AUTO";
    default:
        return "";
    }
}

QString decodeGuard(int x){
    switch(x){
    case 0:
        return "GUARD_INTERVAL_1_32";
    case 1:
        return "GUARD_INTERVAL_1_16";
    case 2:
        return "GUARD_INTERVAL_1_8";
    case 3:
        return "GUARD_INTERVAL_1_4";
    case 4:
        return "GUARD_INTERVAL_AUTO";
    default:
        return "";

    }
}

QString decodeHierarchy(int x){
    switch(x){
    case 0:
        return "HIERARCHY_NONE";
    case 1:
        return "HIERARCHY_1";
    case 2:
        return "HIERARCHY_2";
    case 3:
        return "HIERARCHY_4";
    case 4:
        return "HIERARCHY_AUTO";
    default:
        return "";

    }
}
