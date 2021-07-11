#include "dof_topaintcanvas.h"
#include <QDebug>
const int RADIUS = 5;
const int ZOOM_RATIO = 50;
const int PI_IN_DEGREE = 180;

const bool DEGREE_OR_RADIAN = false;

DOF_toPaintCanvas::DOF_toPaintCanvas(PaintCanvas* _canvasP ,QWidget *parent)
    :DOF_BarInterface(parent),mCanvasP(_canvasP)
{
   resetBars();
}

void DOF_toPaintCanvas::setDataForm(int _barNum, std::vector<bool> _constBar, std::vector<double> _constLength)
{
    mBarNum = _barNum;
    mConstBar = _constBar;
    mConstDeltas = _constLength;
}


void DOF_toPaintCanvas::translateData(std::vector<double> _alphas, std::vector<double> _deltas)
{
    mAlphas = _alphas;
    mDeltas = _deltas;
}

void DOF_toPaintCanvas::drawBars() const
{
    std::vector<double> resultXYs = translateToStartEndPositions();
    int lastX=0, lastY=0;
    for(int i = 0; i < resultXYs.size(); i=i+2)
    {
        int x =(int)(ZOOM_RATIO * resultXYs[i]);
        int y =(int)(ZOOM_RATIO * resultXYs[i+1]);
        int offset = RADIUS;
        mCanvasP->drawRect(QPoint(x + lastX -offset, y +lastY-offset), QPoint(offset, offset), true);

        if(i != 0)
        {
            qDebug() << "drawLine: " << lastX << "," << lastY << "," << x + lastX <<  "," << y + lastY;
            mCanvasP->drawLine(QPoint(lastX, lastY), QPoint(x + lastX, y + lastY));
        }
        lastX += x;
        lastY += y;

    }

}

void DOF_toPaintCanvas::resetBars()
{
    mBarNum = 0;
    mConstBar = std::vector<bool>();
    mAlphas = std::vector<double>();
    mDeltas = std::vector<double>();
    mConstDeltas = std::vector<double>();

}

void DOF_toPaintCanvas::changePenColor(QColor _color)
{
    mCanvasP->setPenColor(_color);
}

std::vector<double> DOF_toPaintCanvas::translateToStartEndPositions() const
{
    std::vector<double> resultXYs;
    if(mBarNum <= 0)return resultXYs;

    resultXYs.reserve(2 * mBarNum + 2);
    double x=0.0, y=0.0;
    resultXYs.push_back(x);
    resultXYs.push_back(y);


    int barIndex = 0;
    int varBarIndex = 0;
    int constBarIndex = 0;
    for(barIndex = 0; barIndex < mBarNum; barIndex++)
    {
        double length = (mConstBar.at(barIndex))? mConstDeltas.at(constBarIndex): mDeltas.at(varBarIndex);

        double angle = mAlphas.at(barIndex);

        (mConstBar.at(barIndex))? constBarIndex++ : varBarIndex++;

        if(DEGREE_OR_RADIAN)
        {
            x = length * qCos(angle / PI_IN_DEGREE * M_PI);
            y = length * qSin(angle/ PI_IN_DEGREE * M_PI);

        }else
        {
            x = length * qCos(angle );
            y = length * qSin(angle);

        }

        resultXYs.push_back(x);
        resultXYs.push_back(y);
    }
    return resultXYs;
}
