#include <string>
#include <yarp/os/Thread.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Image.h>
#include <TactSensor.h>
#include <TactSensorArray.h>
#include <yarp/os/Subscriber.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include "pid.h"

using namespace yarp::dev;
using namespace yarp::sig;

class ControlThread : public yarp::os::Thread
{
public:

    ControlThread():semStart(0) { }
    virtual ~ControlThread();
    virtual bool threadInit();
    ControlThread(yarp::os::Subscriber<TactSensorArray> *my_topic__, IEncoders *encs__,
        IPositionControl *pos__);
    virtual void setSubscriber(yarp::os::Subscriber<TactSensorArray> *my_topic__);
    virtual void run();
    void get_force(int index,yarp::sig::Vector& force);
    virtual void threadRelease();
    void EnableControl();
    void DisableControl();
private:
    yarp::os::Subscriber<TactSensorArray> *my_topic;
    yarp::sig::Vector* image;
    std::vector<TactSensor>* array;
    bool interrupted;
    yarp::os::Semaphore semStart;
    yarp::os::Semaphore semDone;
    yarp::os::Mutex guard;
    yarp::sig::Vector encoders;
    bool controlActive;

    IEncoders *encs;

    double finger_set[3];       // new force setpoints
    double finger_force[3];         // current force values
    double joint_inc[3];
    double inc_max;                           // max joint increment
    double joint_max[3];                        // max joint value for fingers (min is 0)
    double sensor_set[11];
    double force_error;                      // accepeted force error [N]
    bool make_control;
    bool naiveSumControl;
	
    Vector sensor_force;

    // Controling with a PID for each motor
    //PID pid_finger = PID(0.1, 20, -20, 3.4, 0.1, 0.5); //if they are different create the 3
    PID pid_finger = PID(0.05, 30, -30, 5.0, 0.0, 0.05); //if they are different create the 3

    IPositionControl *pos;
};
