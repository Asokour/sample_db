#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browseButton_clicked()
{
    //Create string with path to .db file
    QString s1 = QFileDialog::getOpenFileName(this, "Открыть файл", "directoryToOpen", "DB (*.db)");
    ui->pathLineEdit->insert(s1);
}

void MainWindow::on_connectButton_clicked()
{
    //Close previous connection
    mydb.close();

    //Check if the db file exists
    QString fileName(ui->pathLineEdit->text());
    if (QFileInfo::exists(fileName) && fileName != ""){
        mydb = QSqlDatabase::addDatabase("QSQLITE");
        mydb.setDatabaseName(ui->pathLineEdit->text());

        if(!mydb.open()) {
            ui->connStatus->setText("Не удалось открыть БД");
            //Error message output
            QMessageBox msgBox;
            msgBox.setText(mydb.lastError().text());
            msgBox.exec();

            //Clear tables comboBox
            ui->comboBox->clear();
            dbConnected = false;
        }
        else {
            ui->connStatus->setText("БД открыта");
            dbConnected = true;
            //Clear tables comboBox
            ui->comboBox->clear();

            QStringList list = mydb.tables();
            for ( const auto& i : list )
            {
            ui->comboBox->addItem(i);
            }
        }
    }
    else
    {
        ui->connStatus->setText("Файл БД не найден...");
    }
}


void MainWindow::on_loadTableButton_clicked()
{
    if (dbConnected)
    {
        mydb.open();
        this->model = new CustomSqlModel;        

        //qry.bindValue does not work with SELECT, have to use QString to generate query
        stringQry = "SELECT * FROM " + ui->comboBox->currentText();
        QSqlQuery qry;
        qry.prepare(stringQry);
        qry.exec();
        int idValue = qry.record().indexOf("Value");
        int idTime = qry.record().indexOf("Time");

        timeData.clear();

        //extracting data from query into vector
        while(qry.next())
        {
            QCPGraphData k;
            k.key = qry.value(idTime).toDouble();
            k.value = qry.value(idValue).toDouble();
            timeData.push_back(k);    
        }

        //passing data to model
        model->setQuery(qry);
        //column naming
        model->setHeaderData(0, Qt::Horizontal, QObject::tr("Время"));
        model->setHeaderData(1, Qt::Horizontal, QObject::tr("Значение"));
        model->setHeaderData(2, Qt::Horizontal, QObject::tr("Группа"));
        //drawing table view
        ui->tableView->setModel(model);

        //clearing plot widget every time the new data is loaded
        ui->customPlot->clearGraphs();
        ui->customPlot->replot();
    }
    else
    {
        ui->connStatus->setText("Откройте файл БД...");
    }

}

void MainWindow::on_plotButton_clicked()
{
    //draw horizontal graph
    if (ui->plotTypeBox->currentIndex() == 0) {
        plotLineGraphHorizontal(ui->customPlot, timeData);
    }
    //draw vertical graph
    else if (ui->plotTypeBox->currentIndex() == 1) {
        plotLineGraphVertical(ui->customPlot, timeData);
    }
}

void MainWindow::plotLineGraphHorizontal(QCustomPlot *customPlot, QVector<QCPGraphData> timeData)
{
    //clear any pre-existing graphs
    customPlot->clearGraphs();

    //set locale for correct date and time display
    customPlot->setLocale(QLocale(QLocale::Russian, QLocale::Russia));

    //create a graph instance
    customPlot->addGraph();

    //setup graph instance
    customPlot->graph()->setPen(QPen(Qt::blue));
    customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));

    //pass data to plot
    customPlot->graph()->data()->set(timeData);

    // configure bottom axis to show date:
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);    
    dateTicker->setDateTimeFormat("hh:mm:ss\nd. MMMM\nyyyy");
    customPlot->xAxis->setTicker(dateTicker);

    // configure left axis to show numbers:
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    customPlot->yAxis->setTicker(fixedTicker);

    // set a more compact font size for bottom and left axis tick labels:
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    customPlot->xAxis->setLabel("Время");
    customPlot->yAxis->setLabel("Значение");

    // make top and right axes visible but without ticks and labels:
    customPlot->xAxis2->setVisible(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->xAxis2->setTicks(false);
    customPlot->yAxis2->setTicks(false);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setTickLabels(false);
    customPlot->graph(0)->rescaleAxes();

    // show legend with slightly transparent background brush:
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QColor(255, 255, 255, 150));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->replot();

}

void MainWindow::plotLineGraphVertical(QCustomPlot *customPlot, QVector<QCPGraphData> timeData)
{
    //clear any pre-existing graphs
    customPlot->clearGraphs();

    //set locale for correct date and time display
    customPlot->setLocale(QLocale(QLocale::Russian, QLocale::Russia));

    //create a graph instance with reversed axis
    customPlot->addGraph(customPlot->yAxis,customPlot->xAxis);

    //setup graph instance
    customPlot->graph()->setPen(QPen(Qt::blue));
    customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));

    //pass data to plot
    customPlot->graph()->data()->set(timeData);

    // configure left axis to show date instead of number:
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm:ss\nd. MMMM\nyyyy");
    customPlot->yAxis->setTicker(dateTicker);

    // configure bottom axis to show numbers:
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    customPlot->xAxis->setTicker(fixedTicker);

    // set a more compact font size for bottom and left axis tick labels:
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    // set axis labels:
    customPlot->xAxis->setLabel("Значение");
    customPlot->yAxis->setLabel("Время");

    // make top and right axes visible but without ticks and labels:
    customPlot->xAxis2->setVisible(true);
    customPlot->yAxis2->setVisible(true);
    customPlot->xAxis2->setTicks(false);
    customPlot->yAxis2->setTicks(false);
    customPlot->xAxis2->setTickLabels(false);
    customPlot->yAxis2->setTickLabels(false);
    customPlot->graph(0)->rescaleAxes();

    // show legend with slightly transparent background brush:
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QColor(255, 255, 255, 150));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->replot();

}
