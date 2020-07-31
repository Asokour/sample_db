#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QLineSeries>
#include "customsqlmodel.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseButton_clicked();

    void on_connectButton_clicked();

    void on_loadTableButton_clicked();

    void on_plotButton_clicked();

private:
    void plotLineGraphHorizontal(QCustomPlot *customPlot, QVector<QCPGraphData> timeData);
    void plotLineGraphVertical(QCustomPlot *customPlot, QVector<QCPGraphData> timeData);
    Ui::MainWindow *ui;
    CustomSqlModel *model;
    QSqlDatabase mydb;    
    QString stringQry;
    QVector<QCPGraphData> timeData;

    bool dbConnected = false;
};
#endif // MAINWINDOW_H
