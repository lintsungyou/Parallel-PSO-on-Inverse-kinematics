#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "paintcanvas.h"
#include "dof_barinterface.h"
#include "dof_topaintcanvas.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTimerEvent>
#include <QStringList>
#include <QPainter>
#include <QMouseEvent>
#include <chrono>
#include <ctime>
#include <QProcess>



const int LINE_BARNUM = 0;
const int LINE_CONSTBAR = 1;
const int LINE_CONSTDELTAS = 2;
const int LINE_ALPHAS =3;
const int LINE_DELTAS = 4;
const int LINE_TIMPESTAMPS = 5;
const int SLOW_RATIO = 1;
const int DELAY_AFTER_CLICK = 4;
const int RADIUS = 5;
const int ZOOM_RATIO = 50;
const int PI_IN_DEGREE = 180;
const double X_FRAME_OFFSET = 265.0;
const double Y_FRAME_OFFSET = 35.0;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void penWidthChanged(int width);
    void changePenColor();
    void changeFillColor();
    void changeFillProperty();
    void readBarDatas();
    void readBarDatasFromFile();
    void insertLinelistToDataVec(QStringList _lineList);
    void resetData();
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    Ui::MainWindow *ui;
    PaintCanvas * canvas;
    DOF_BarInterface *dofBars;
    QPushButton * penColorButton;
    QPushButton * fillColorButton;
    QPushButton * drawBarsBtn;
    QCheckBox * fillCheckBox;
    QLineEdit * mBarNumberLineEdit;
    QTextEdit * mPositionsTextEdit;
    QVBoxLayout * mPositionsLayout;

    std::vector<double> timeStamps;
    std::vector<double> alphasList;
    std::vector<double> deltasList;
    std::vector<double> constDeltas;
    std::vector<bool> constBar = std::vector<bool>();
    int                 barNum;

    std::string       taskInfoStr;

    //for timer event
    std::chrono::steady_clock::time_point startTime;
    int timestampsIt;
    int timestampsItFinished;
    int timerId;
    int hasDrawnLineNumber;
    bool isTracing;
    bool finishedTracing;
    bool firstTraceAbandoned; //the first trace is trival should be abandoned

    //mouse tracking
    bool mouseDrawing;
    QPoint lastPoint;


    // QProcess
    QProcess *getAnswerProc;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
