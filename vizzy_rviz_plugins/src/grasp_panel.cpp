/*
João Avelino
December, 2018
*/

#include <stdio.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <std_msgs/Int16.h>
#include "../include/vizzy_rviz_plugins/grasp_panel.hpp"


namespace vizzy_rviz_plugins {

GraspPanel::GraspPanel(QWidget *parent)
  : rviz::Panel(parent)
{

  //Configure the push buttons for the handshakes
  home_button = new QPushButton("Home position", this);
  grab_button = new QPushButton("Grab", this);
  release_button = new QPushButton("Release", this);
  putinbox_button = new QPushButton("Put in box", this);
  giveaway_button = new QPushButton("Give away", this);


  // Next we lay out the "output topic" text entry field using a
  // QLabel and a QLineEdit in a QHBoxLayout.
  QHBoxLayout* topic_layout = new QHBoxLayout();
  topic_layout->addWidget( new QLabel( "Output Topic:" ));
  output_topic_editor_ = new QLineEdit;
  output_topic_editor_->setText("/vizzyGrasp/command");
  topic_layout->addWidget(output_topic_editor_ );

  QVBoxLayout* gestures_layout= new QVBoxLayout();
  gestures_layout->addWidget(home_button);
  gestures_layout->addWidget(grab_button);
  gestures_layout->addWidget(release_button);
  gestures_layout->addWidget(putinbox_button);
  gestures_layout->addWidget(giveaway_button);


  QVBoxLayout* panel_layout = new QVBoxLayout();
  panel_layout->addLayout(topic_layout);
  panel_layout->addLayout(gestures_layout);

  setLayout( panel_layout );

  //Connect objects with signals
  connect(home_button, SIGNAL (released()), this, SLOT(home()));
  connect(grab_button, SIGNAL (released()), this, SLOT (grab()));
  connect(release_button, SIGNAL (released()), this, SLOT (release()));
  connect(putinbox_button, SIGNAL (released()), this, SLOT (putinbox()));
  connect(giveaway_button, SIGNAL (released()), this, SLOT (giveaway()));
  connect( output_topic_editor_, SIGNAL( editingFinished() ), this, SLOT( updateTopic() ));

  updateTopic();
}

void GraspPanel::home()
{
  std_msgs::Int16 command;
  command.data=0;
  vizzy_arm_publisher.publish(command);
  return;
}

void GraspPanel::grab()
{
  std_msgs::Int16 command;
  command.data=1;
  vizzy_arm_publisher.publish(command);
  return;
}

void GraspPanel::release()
{
  std_msgs::Int16 command;
  command.data=2;
  vizzy_arm_publisher.publish(command);
}

void GraspPanel::putinbox()
{
  std_msgs::Int16 command;
  command.data=3;
  vizzy_arm_publisher.publish(command);
}

void GraspPanel::giveaway()
{
  std_msgs::Int16 command;
  command.data=4;
  vizzy_arm_publisher.publish(command);  
}

void GraspPanel::updateTopic()
{
  setTopic( output_topic_editor_->text() );
}

void GraspPanel::setTopic(const QString &new_topic)
{
  // Only take action if the name has changed.
  if( new_topic != output_topic_ )
  {
    output_topic_ = new_topic;
    // If the topic is the empty string, don't publish anything.
    if( output_topic_ == "" )
    {
      vizzy_arm_publisher.shutdown();
    }
    else
    {
      vizzy_arm_publisher = nh_.advertise<std_msgs::Int16>( output_topic_.toStdString(), 1 );
    }

    Q_EMIT configChanged();
  }
}

void GraspPanel::save( rviz::Config config ) const
{
  rviz::Panel::save( config );
  config.mapSetValue( "Topic", output_topic_ );
}

// Load all configuration data for this panel from the given Config object.
void GraspPanel::load( const rviz::Config& config )
{
  rviz::Panel::load( config );
  QString topic;
  if( config.mapGetString( "Topic", &topic ))
  {
    output_topic_editor_->setText( topic );
    updateTopic();
  }
}

}

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(vizzy_rviz_plugins::GraspPanel, rviz::Panel)
