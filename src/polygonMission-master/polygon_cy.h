#ifndef POLYGON_CY_H
#define POLYGON_CY_H

#include <QDialog>

namespace Ui {
class polygon_cy;
}

class polygon_cy : public QDialog
{
    Q_OBJECT

public:
    explicit polygon_cy(QWidget *parent = 0);
    ~polygon_cy();

private slots:
    void on_angle_grid_actionTriggered(int action);

private:
    Ui::polygon_cy *ui;
};

#endif // POLYGON_CY_H
