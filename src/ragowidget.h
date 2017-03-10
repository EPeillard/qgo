#ifndef RAGOWIDGET_H
#define RAGOWIDGET_H

#include "define.hpp"

#include "defines.h"

#include "Goban.hpp"
#include "Core.hpp"
#include "Network.hpp"
#include "VirtualGoban.hpp"
#include "Projector.hpp"
#include "Stone.hpp"

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

class qGoBoard;

enum RAGoPhase { disabled, waitCalib, calibCamera, calibCameraEnd, calibProjector, calibProjectorEnd, enabled};

namespace Ui {
class RAGoWidget;
}

class RAGoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RAGoWidget(QWidget *parent = 0);
    ~RAGoWidget();
    float getZoom();
    float getLum();
    int getGobanSize();
    RAGoPhase getPhase();
    void makeMove(StoneColor,int,int);
    void addNewMoveMark(StoneColor, int, int);
    void initGoban();
    void attachBoard(qGoBoard*);
    void detachBoard(qGoBoard*);

public slots:
  void activateRAGo(bool);
  void changeCam();
  void startCalib();
  void updateFrame();
  void goodCalib();
  void wrongCalib();
  void changeLum(int);
  void changeZoom(int);
  void searchNewMove();
  void resetReferenceFrame();
  void blockRago(bool);
    
private:
    Ui::RAGoWidget *ui;
    rago::Projector* proj;
    rago::VirtualGoban* vg;
    rago::Camera* camera;
    rago::Goban* goban;
    rago::Core* core;
    QTimer* timer; 
    RAGoPhase phase;
    float zoom=1.0;
    float lum=1.0;
    int count=0;
    qGoBoard* attachedBoard;
    pair<int,int> precMove;
    bool block=false;
    rago::Stone newStone = rago::Stone();
    
signals:
  void playMove(int color, int x, int y);

};

#endif // RAGOWIDGET_H
