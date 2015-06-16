#ifndef RAGOWIDGET_H
#define RAGOWIDGET_H

#include "Goban.hpp"
#include "Core.hpp"
#include "Network.hpp"
#include "VirtualGoban.hpp"
#include "define.hpp"

#include "defines.h"

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <sys/time.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QWidget>

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

enum RAGoPhase { disabled, waitCalib, calibCamera, calibCameraEnd, calibProjector, calibProjectorEnd, enabled};

using namespace cv;
using namespace std;

namespace Ui {
class RAGoWidget;
}

class RAGoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RAGoWidget(QWidget *parent = 0);
    ~RAGoWidget();

public slots:
  void activateRAGo(int);
  void changeCam();
  void startCalib();
  void updateFrame();
  void goodCalib();
  void wrongCalib();
    
private:
    Ui::RAGoWidget *ui;
    Projector* proj;
    VirtualGoban* vg;
    Camera* camera;
    Goban* goban;
    Core* core;
    QTimer* timer; 
    RAGoPhase phase;
};

#endif // RAGOWIDGET_H
