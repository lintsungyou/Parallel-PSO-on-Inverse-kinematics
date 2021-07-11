#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDebug>
#include <QSpinBox>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <sstream>
#include <QThread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    canvas = new PaintCanvas(this);
    isTracing = false;
    finishedTracing = true;
    mouseDrawing = false;
    firstTraceAbandoned = false;

    setCentralWidget(canvas);

    dofBars = new DOF_toPaintCanvas(canvas, this);
    std::vector<bool> constLength = {false};

    //Pen width
    QLabel * penWidthLabel = new QLabel("Pen Width",this);
    QSpinBox * penWidthSpinBox = new QSpinBox(this);
    penWidthSpinBox->setValue(2);
    penWidthSpinBox->setRange(1,15);

    //Pen color
    QLabel * penColorLabel = new QLabel("Pen Color",this);
    penColorButton = new QPushButton(this);

    //Fill Color
    QLabel * fillColorLabel = new QLabel("Fill Color",this);
    fillColorButton = new QPushButton(this);

    //Fill
    fillCheckBox = new QCheckBox("Fill Shape",this);


    //Tool Buttons
    QPushButton * rectButton = new QPushButton(this);
    rectButton->setVisible(false);
    rectButton->setIcon(QIcon(":/images/rectangle.png"));

    QPushButton * penButton = new QPushButton(this);
    penButton->setVisible(false);
    penButton->setIcon(QIcon(":/images/pen.png"));

    QPushButton * ellipseButton = new QPushButton(this);
    ellipseButton->setVisible(false);
    ellipseButton->setIcon(QIcon(":/images/circle.png"));


    QPushButton * eraserButton = new QPushButton(this);
    eraserButton->setIcon(QIcon(":/images/eraser.png"));

    QLabel * barNumberLbl = new QLabel( "bar Number:", this);
    mBarNumberLineEdit = new QLineEdit(this);

    //positionsLayout
    drawBarsBtn = new QPushButton("draw bars", this);

    //positionLineEdit
    mPositionsTextEdit = new QTextEdit(this);


    connect(rectButton,&QPushButton::clicked,[=](){
        //Set current tool to rect
        canvas->setTool(PaintCanvas::Rect);
        statusBar()->showMessage("Current tool : Rect");
    });

    connect(penButton,&QPushButton::clicked,[=](){
            //Set current tool to rect
            canvas->setTool(PaintCanvas::Pen);
            statusBar()->showMessage("Current tool : Pen");

        });


    connect(ellipseButton,&QPushButton::clicked,[=](){
           //Set current tool to rect
           canvas->setTool(PaintCanvas::Ellipse);
           statusBar()->showMessage("Current tool : Ellipse");

       });


    connect(eraserButton,&QPushButton::clicked,[=](){
            //Set current tool to rect
            canvas->setTool(PaintCanvas::Eraser);
            statusBar()->showMessage("Current tool : Eraser");

        });

    connect(drawBarsBtn,&QPushButton::clicked,[=](){
            readBarDatas();

            QThread::currentThread()->sleep(DELAY_AFTER_CLICK);
            dofBars->setDataForm(barNum, constBar,constDeltas);
            if(timeStamps.size() == 0 )
            {
                dofBars->translateData(alphasList, deltasList);
                dofBars->drawBars();
                resetData();
            }
            else
            {
                timestampsIt = 0;
                timestampsItFinished = -1;
                hasDrawnLineNumber = 0;
                startTime = std::chrono::steady_clock::now();
                timerId = startTimer(20);
            }

        });








    //Slots connections
    connect(penWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(penWidthChanged(int)));
    connect(penColorButton, SIGNAL(clicked()), this, SLOT(changePenColor()));
    connect(fillColorButton, SIGNAL(clicked()), this, SLOT(changeFillColor()));
    connect(fillCheckBox, SIGNAL(clicked()), this, SLOT(changeFillProperty()));


    ui->mainToolBar->addWidget(penWidthLabel);
    ui->mainToolBar->addWidget(penWidthSpinBox);
    ui->mainToolBar->addWidget(penColorLabel);
    ui->mainToolBar->addWidget(penColorButton);
    ui->mainToolBar->addWidget(fillColorLabel);
    ui->mainToolBar->addWidget(fillColorButton);
    ui->mainToolBar->addWidget(fillCheckBox);
    ui->mainToolBar->addSeparator();
//    ui->mainToolBar->addWidget(penButton);
//    ui->mainToolBar->addWidget(rectButton);
//    ui->mainToolBar->addWidget(ellipseButton);
    ui->mainToolBar->addWidget(eraserButton);
    ui->mainToolBar->addWidget(barNumberLbl);
    ui->mainToolBar->addWidget(mBarNumberLineEdit);

    ui->leftBar->addWidget(drawBarsBtn);
    ui->leftBar->addWidget(mPositionsTextEdit);

    QString css = QString("background-color : %1").arg(canvas->getPenColor().name());
    penColorButton->setStyleSheet(css);

    css = QString("background-color : %1").arg(canvas->getFillColor().name());
    fillColorButton->setStyleSheet(css);

    resetData();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::penWidthChanged(int width)
{
    canvas->setPenWidth(width);
}

void MainWindow::changePenColor()
{
    QColor color = QColorDialog::getColor(canvas->getPenColor());
    if(color.isValid()){
        canvas->setPenColor(color);
        QString css = QString("background-color : %1").arg(color.name());
        penColorButton->setStyleSheet(css);
    }

}

void MainWindow::changeFillColor()
{
    QColor color = QColorDialog::getColor(canvas->getPenColor());
    if(color.isValid()){
        canvas->setFillColor(color);
        QString css = QString("background-color : %1").arg(color.name());
        fillColorButton->setStyleSheet(css);
    }

}

void MainWindow::changeFillProperty()
{
    canvas->setFill(fillCheckBox->isChecked());
}

void MainWindow::readBarDatas()
{
    QStringList lineList = mPositionsTextEdit->toPlainText().split('\n');

    if(lineList.size() < 5)
    {
        readBarDatasFromFile();
    }
    else
    {
        insertLinelistToDataVec(lineList);

    }


}

void MainWindow::readBarDatasFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("open Data"), "./");

    QFile datafile(fileName);
    datafile.open(QFile::ReadOnly);
    QString line = "";

    QTextStream stream(&datafile);
    QStringList lineList;
    while(stream.readLineInto(&line))
    {
        lineList.append(line);
    }

    insertLinelistToDataVec(lineList);

}

void MainWindow::insertLinelistToDataVec(QStringList _lineList)
{
    QString line = "";
    taskInfoStr = "";

    for(int lineIt = 0; lineIt < _lineList.size(); lineIt++)
    {
        QStringList inlineList = _lineList.at(lineIt).split(' ');
        switch(lineIt)
        {
            case LINE_BARNUM:{ //barNum
            this->barNum = _lineList.at(0).toInt();
            break;
           }
            case LINE_CONSTBAR:{ //constBar
                for(int i = 0; i < inlineList.size(); i++)
                {
                    (inlineList.at(i).toInt() >= 1)? constBar.push_back(true) : constBar.push_back(false);

                }

            break;
            }
            case LINE_CONSTDELTAS:{ //constDeltas
                for(int i = 0; i < inlineList.size(); i++)
                {
                   constDeltas.push_back(inlineList.at(i).toDouble());
                }
            break;
            }
            case LINE_ALPHAS:{ //
                for(int i = 0; i < inlineList.size(); i++)
                {
                    alphasList.push_back(inlineList.at(i).toDouble());
                }
            break;
            }
            case LINE_DELTAS:{
                for(int i = 0; i < inlineList.size(); i++)
                {
                    deltasList.push_back(inlineList.at(i).toDouble());
                }
            break;
            }
            case LINE_TIMPESTAMPS:{
                for(int i =0; i < inlineList.size(); i++)
                {
                    timeStamps.push_back(inlineList.at(i). toDouble());
                }
            break;
            }
            default:{
                taskInfoStr += _lineList.at(lineIt).toStdString();
                taskInfoStr += "\n";
            break;
            }
        }

    }

}

void MainWindow::resetData()
{
    timeStamps = std::vector<double>();
    alphasList = std::vector<double>();
    deltasList = std::vector<double>();
    constDeltas = std::vector<double>();
    constBar = std::vector<bool>();
    barNum = 0;
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();

    double timeSpan = std::chrono::duration<double>(nowTime - startTime).count();


    while(timeSpan > timeStamps[timestampsIt] * SLOW_RATIO && timestampsIt < timeStamps.size())
    {
        timestampsIt++;
    }
    //qDebug() << "timerstamp: " << timeStamps[timestampsIt] << ", timespan: " << timeSpan << "\n";
    if(timestampsIt < timeStamps.size() && timestampsIt > timestampsItFinished)
    {
        std::vector<double> alphas = std::vector<double>
                (alphasList.begin() + (barNum * timestampsIt), alphasList.begin() + (barNum * (timestampsIt + 1)));
        int deltaNum = barNum - constDeltas.size();
        std::vector<double> deltas = std::vector<double>
                (deltasList.begin() + (deltaNum * timestampsIt), deltasList.begin() + (deltaNum * (timestampsIt + 1)));


        if(hasDrawnLineNumber != 0)
            dofBars->drawBars();
        dofBars->translateData(alphas, deltas);
        dofBars->changePenColor(Qt::red);
        dofBars->drawBars();

        dofBars->changePenColor(Qt::green);
        hasDrawnLineNumber++;
        timestampsItFinished = timestampsIt;
    }

    if(timestampsIt >= timeStamps.size())
    {
        mPositionsTextEdit->setPlainText(QString::fromStdString(taskInfoStr));
        resetData();
        killTimer(timerId);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    isTracing = true;
    resetData();
    constBar = std::vector<bool>({true, false, true});
    barNum = 3;
    constDeltas = std::vector<double>({2, 2});
    dofBars->setDataForm(barNum, constBar,constDeltas);
    if (event->button() == Qt::LeftButton) {
        lastPoint = QPoint(event->pos().x() - X_FRAME_OFFSET, event->pos().y() - Y_FRAME_OFFSET);
        mouseDrawing = true;
    }
    startTime = std::chrono::steady_clock::now();


}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
     if (event->button() == Qt::LeftButton && mouseDrawing){
         mouseDrawing = false;
         canvas->setPenColor(Qt::red);
         canvas->drawLine(lastPoint, QPoint(event->pos().x() - X_FRAME_OFFSET, event->pos().y() - Y_FRAME_OFFSET));
         canvas->setPenColor(Qt::green);

     }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{

    std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
    double xOrg =  (double)(event->pos().x() - X_FRAME_OFFSET) ;
    double yOrg = (double)(event->pos().y() - Y_FRAME_OFFSET) ;

    qDebug() << "moveTraceOrg:" << xOrg << ", " <<   yOrg << "\n";
     if ((event->buttons() & Qt::LeftButton) && mouseDrawing){
         canvas->setPenColor(Qt::red);
        canvas->drawLine(lastPoint,QPoint(xOrg, yOrg));
        canvas->setPenColor(Qt::green);

        lastPoint = QPoint(xOrg, yOrg);
     }

    double timeSpan = std::chrono::duration<double>(nowTime - startTime).count();
    if(timeSpan > 1 && finishedTracing == true){
        qDebug() << "traceOrg:" << xOrg << ", " <<   yOrg << "\n";

        qDebug() << "trace:" <<xOrg/ ZOOM_RATIO << ", " << yOrg/ ZOOM_RATIO << "\n";

        getAnswerProc = new QProcess();
        getAnswerProc->startDetached("search.o", {QString::number(xOrg/ZOOM_RATIO,'g',4), QString::number(yOrg/ZOOM_RATIO,'g',4)});
        finishedTracing = false;
    }

    if(timeSpan > 0.5 && finishedTracing == false)
    {


         QFile file("resultBar.txt");
         file.open( QIODevice::ReadOnly);
         QStringList lineLIst;
         QTextStream textStream(&file);
         while (true)
         {
             QString line = textStream.readLine();
             if (line.isNull())
                 break;
             else
                 lineLIst.append(line);
         }
               // write to stderr
         file.close();

         if(lineLIst.size() < 3)
             return;


         QStringList inLIneList = lineLIst[0].split(" ");

         if(inLIneList.size() != 4)
             return;

         qDebug() << "get: " << lineLIst[1] <<"fit:" << lineLIst[2] <<"\n";
         qDebug() << "{" << inLIneList[0].toDouble()  << " " << inLIneList[1].toDouble()  << " " << inLIneList[2].toDouble()  << " " << inLIneList[3].toDouble();

         alphasList = std::vector<double>({inLIneList[0].toDouble(), inLIneList[1].toDouble(), inLIneList[2].toDouble()});

         deltasList = std::vector<double>({inLIneList[3].toDouble()});

         dofBars->translateData(alphasList, deltasList);
         if(!firstTraceAbandoned)
         {
             finishedTracing = true;

             delete getAnswerProc;


             startTime = std::chrono::steady_clock::now();
             firstTraceAbandoned = true;
             return;
         }
         dofBars->drawBars();

         finishedTracing = true;

         delete getAnswerProc;


         startTime = std::chrono::steady_clock::now();


    }

}
