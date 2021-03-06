// post_counter.cpp
//
// The post counter widget for Rivendell
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <qpixmap.h>
#include <qpainter.h>

#include <post_counter.h>
#include <colors.h>
#include <globals.h>

PostCounter::PostCounter(QWidget *parent,const char *name)
  : QPushButton(parent,name)
{
  post_running=false;
  post_time_format="hh:mm:ss";
  post_time=QTime();
  post_start_offset=0;
  post_end_offset=0;
  post_offset_valid=false;
  post_timescale_mode=RDLogLine::TimescaleIndividual;

  //
  // Generate Fonts
  //
  post_small_font=QFont("Helvetica",12,QFont::Normal);
  post_small_font.setPixelSize(12);
  post_large_font=QFont("Helvetica",26,QFont::Normal);
  post_large_font.setPixelSize(26);

  //
  // Generate Palettes
  //
  post_idle_palette=palette();
  post_early_palette=
    QPalette(QColor(POSTPOINT_EARLY_COLOR),backgroundColor());
  post_ontime_palette=
    QPalette(QColor(POSTPOINT_ONTIME_COLOR),backgroundColor());
  post_late_palette=
    QPalette(QColor(POSTPOINT_LATE_COLOR),backgroundColor());
  post_timescale_palette=
    QPalette(QColor(POSTPOINT_TIMESCALE_COLOR),backgroundColor());

  UpdateDisplay();
}

void PostCounter::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  switch(mode) {
      case RDAirPlayConf::TwentyFourHour:
	post_time_format="hh:mm:ss";
	break;

      case RDAirPlayConf::TwelveHour:
	post_time_format="h:mm:ss ap";
	break;
  }
  UpdateDisplay();
}

QSize PostCounter::sizeHint() const
{
  return QSize(200,60);
}


QSizePolicy PostCounter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void PostCounter::setPostPoint(QTime point,int offset,bool offset_valid,
			       bool running)
{
  setPostPoint(point,offset,offset,offset_valid,running);
}


void PostCounter::setPostPoint(QTime point,int start_offset,int end_offset,
			       bool offset_valid,bool running)
{
  post_time=point;
  post_start_offset=start_offset;
  post_end_offset=end_offset;
  post_offset_valid=offset_valid;
  post_running=running;
  post_set_time=QTime::currentTime();
  UpdateDisplay();
}


void PostCounter::setTimescaleMode(RDLogLine::TimescaleMode mode)
{
  if(mode!=post_timescale_mode) {
    post_timescale_mode=mode;
    UpdateDisplay();
  }
}


void PostCounter::tickCounter()
{
  if(!post_running) {
    UpdateDisplay();
  }
}


void PostCounter::setEnabled(bool state)
{
  QWidget::setEnabled(state);
  UpdateDisplay();
}


void PostCounter::setDisabled(bool state)
{
  setEnabled(!state);
}


void PostCounter::keyPressEvent(QKeyEvent *e)
{
  e->ignore();
}


void PostCounter::mouseReleaseEvent(QMouseEvent *e)
{
  if((e->x()>=0)&&(e->x()<size().width())&&
     (e->y()>=0)&&(e->y()<size().height())) {
    switch(post_timescale_mode) {
    case RDLogLine::TimescaleIndividual:
      emit timeScalingModeRequested(1,RDLogLine::TimescaleBlock);
      break;

    case RDLogLine::TimescaleBlock:
      emit timeScalingModeRequested(1,RDLogLine::TimescaleIndividual);
      break;
    }
  }
}


void PostCounter::UpdateDisplay()
{
  QColor color=topLevelWidget()->backgroundColor();
  QColor frame_color;
  QColor system_button_text_color=palette().active().buttonText();
  QString str;
  QString point;
  QString state;
  QTime current_time=
    QTime::currentTime().addMSecs(rdstation_conf->timeOffset());
  int start_offset=post_start_offset;
  int end_offset=post_end_offset;
  if(!post_running) {
    start_offset-=current_time.msecsTo(post_set_time);
    end_offset-=current_time.msecsTo(post_set_time);
  }

  if(isEnabled()&&(!post_time.isNull())) {
    point= trUtf8("Next Timed Start") + " [" + post_time.toString(post_time_format) + "]";

    if(post_offset_valid) {
      if(start_offset<-POST_COUNTER_MARGIN) {
	state=QString().sprintf("-%s",(const char *)
				QTime().addMSecs(-start_offset).toString());
	setPalette(post_early_palette);
	color=POSTPOINT_EARLY_COLOR;
      }
      else {
	if(end_offset>POST_COUNTER_MARGIN) {
	  state=QString().sprintf("+%s",(const char *)
				  QTime().addMSecs(end_offset).toString());
	  setPalette(post_late_palette);
	  color=POSTPOINT_LATE_COLOR;
	}
	else {
	  if(post_timescale_mode==RDLogLine::TimescaleBlock) {
	    state=QString().sprintf("%s",(const char *)
				    QTime().addMSecs(-end_offset).toString());
	  }
	  else {
	    state=tr("On Time");
	  }
	  setPalette(post_ontime_palette);
	  color=POSTPOINT_ONTIME_COLOR;
	}
      }
      system_button_text_color = Qt::color1;
    }
    else {
      state="--------";
      setPalette(post_idle_palette);
    }
  }
  else {     // No postpoint/disabled
    point=tr("Next Timed Start [--:--:--]");
    state="--------";
    setPalette(post_idle_palette);
  }


  if(post_timescale_mode==RDLogLine::TimescaleBlock) {
    setPalette(post_timescale_palette);
    frame_color=
      post_timescale_palette.color(QPalette::Active,QColorGroup::Button);
  }
  else {
    frame_color=color;
  }


  QPixmap pix(sizeHint().width(),sizeHint().height());
  QPainter *p=new QPainter(&pix);
  p->fillRect(0,0,sizeHint().width(),sizeHint().height(),frame_color);
  p->fillRect(5,5,sizeHint().width()-11,sizeHint().height()-11,color);
  p->setPen(QColor(system_button_text_color));
  p->setFont(post_small_font);
  p->drawText((sizeHint().width()-p->
	       fontMetrics().width(point))/2,22,point);
  p->setFont(post_large_font);
  p->drawText((sizeHint().width()-p->
	       fontMetrics().width(state))/2,48,state);
  p->end();
  delete p;
  setPixmap(pix);    

  /*
  QPixmap pix(sizeHint().width(),sizeHint().height());
  QPainter *p=new QPainter(&pix);
  p->fillRect(0,0,sizeHint().width(),sizeHint().height(),color);
  p->setPen(QColor(system_button_text_color));
  p->setFont(post_small_font);
  p->drawText((sizeHint().width()-p->
	       fontMetrics().width(point))/2,22,point);
  p->setFont(post_large_font);
  p->drawText((sizeHint().width()-p->
	       fontMetrics().width(state))/2,48,state);
  p->end();
  delete p;
  setPixmap(pix);    
  */
}
