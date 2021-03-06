#include "client.h"
#include "ui_client.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#include <functional>
#include "clienttcp.h"
#include "singleton.h"
#include "qcustomplot.h"
using namespace std;

int row = 0;
int bar = 0;
bool registered = false;

Client::Client(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);

    /* Timer for updating tables, TODO: improve by using observer and invoke from thread */
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGraphs()));
    timer->start(100);
    ui->table->setHorizontalHeaderItem(0, new QTableWidgetItem("# Packets"));
    ui->table->setHorizontalHeaderItem(1, new QTableWidgetItem("Packet Size"));
}

Client::~Client()
{
    delete ui;
}

/* Update graph values from Singelton */
void Client::updateGraphs()
{
    Singleton* s = Singleton::getInstance();
    for (int i = row; i < s->packetSizes.size(); ++i) {
        string str = s->packetSizes[i];
        char *packetSize = &str[0u];
        string str2 = s->stringLengthes[i];
        char *stringLength = &str2[0u];

        ui->table->insertRow(row++);
        ui->table->setItem(row-1, 0, new QTableWidgetItem(packetSize));
        ui->table->setItem(row-1, 1, new QTableWidgetItem(stringLength));
    }
    setupGraph();
}

/* Update BarChart graph */
void Client::setupGraph()
{
    Singleton* s = Singleton::getInstance();
    QCPBars *myBars = new QCPBars(ui->graph->xAxis, ui->graph->yAxis);
    ui->graph->addPlottable(myBars);

    myBars->setName("Bar Chart");
    QVector<double> keyData;
    QVector<double> valueData;

    for (int i = bar; i < s->packetSizes.size(); ++i) {
        string packetSize = s->packetSizes[i];
        string stringLength = s->stringLengthes[i];
        double packetSizeDouble = stod (packetSize);
        double stringLengthDouble = stod(stringLength);
        keyData << packetSizeDouble;
        valueData << stringLengthDouble;
        bar++;
    }

    myBars->setData(keyData, valueData);
    ui->graph->rescaleAxes();
    ui->graph->replot();
}
