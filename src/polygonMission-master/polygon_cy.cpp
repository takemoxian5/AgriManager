#include "polygon_cy.h"
#include "ui_polygon_cy.h"

polygon_cy::polygon_cy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::polygon_cy)
{
    ui->setupUi(this);
}

polygon_cy::~polygon_cy()
{
    delete ui;
}

void polygon_cy::on_angle_grid_actionTriggered(int action)
{

}
