#ifndef DOF_TOPAINTCANVAS_H
#define DOF_TOPAINTCANVAS_H

#include <QWidget>
#include <QColor>
#include <dof_barinterface.h>
#include "paintcanvas.h"

class DOF_toPaintCanvas :  public DOF_BarInterface
{
    Q_OBJECT
public:
    explicit DOF_toPaintCanvas(PaintCanvas* _canvasP ,QWidget *parent = nullptr);
    void setDataForm(int _barNum, std::vector<bool> _constBar, std::vector<double> _constLength = std::vector<double>());
    void translateData(std::vector<double> _alphas, std::vector<double> _deltas);
    void drawBars() const;
    void resetBars();
    void changePenColor(QColor _color);
signals:

private:
    std::vector<double> translateToStartEndPositions() const;
    PaintCanvas* mCanvasP;
    int mBarNum;
    std::vector<bool> mConstBar;
    std::vector<double> mAlphas;
    std::vector<double> mDeltas;
    std::vector<double> mConstDeltas;
};

#endif // DOF_TOPAINTCANVAS_H
