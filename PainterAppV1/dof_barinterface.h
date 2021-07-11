#ifndef DOF_BARINTERFACE_H
#define DOF_BARINTERFACE_H

#include <QWidget>
#include <vector>
#include <QtMath>

class DOF_BarInterface : public QWidget
{
    Q_OBJECT
public:
    explicit DOF_BarInterface(QWidget *parent = nullptr);

    virtual void setDataForm(int _barNum, std::vector<bool> _constBar, std::vector<double> _constLength = std::vector<double>()) = 0;
    virtual void translateData(std::vector<double> _alphas, std::vector<double> _deltas) = 0;
    virtual void drawBars() const = 0;
    virtual void changePenColor(QColor _color) = 0;

signals:

};

#endif // DOF_BARINTERFACE_H
