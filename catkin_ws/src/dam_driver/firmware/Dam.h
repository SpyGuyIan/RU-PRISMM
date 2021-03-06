#ifndef _DAM_H_
#define _DAM_H_

//includes
#include <ACS712.h>
#include <ACS712_AC.h>
#include <AccelStepper.h>
#include <HX711.h>
#include <ros.h>
#include <prismm_msgs/dam_data.h>
#include <prismm_msgs/getBool.h>
#include <MovingAverageFilter.h>
#include <Servo.h>

// 
#define STP_Y_ACCEL 2000
#define STP_X_ACCEL 800
#define STEPS_PER_REV 800

// Probe Servo
#define SERVO_ROT_PIN 9
#define SERVO_EXT_PIN 10

// X Steppers
#define STP_X_STEP_PIN 5
#define STP_X_DIR_PIN 4
#define STP_X_HOME_PIN 32  

// Probe Stepper
#define STP_PROBE_STEP_PIN 3
#define STP_PROBE_DIR_PIN 2
#define STP_PROBE_HOME_PIN 34

//Drill stepper
#define STP_DRILL_STEP_PIN 7
#define STP_DRILL_DIR_PIN 6
#define STP_DRILL_HOME_PIN 33
#define STP_DRILL_CURRENT_PIN A0

//Switches
#define PROBE_LIMMIT_PIN 31


class Dam {
    public:
        Dam(ros::NodeHandle nh);

        enum DamState{
            E_STOP = -1,
            DEFAULT_STATE = 0,
            HOMED = 1,
            HOMING = 2,
            HOMING_DRILL = 3,
            HOMING_PROBE = 4,
            DRILLING = 5,
            BOWL = 6,
        };

        bool update();// Should be run in loop and do iterative processes
        DamState getState();
        bool startDrilling();// Return false if probe not homed
        bool stopDrilling();

        bool homeProbe();
        bool gotoProbeRot(int angle);
        bool gotoProbeExt(int angle);
        bool setProbeSpeed(int max_speed);

        bool homeX();// Return false if drill and probe not homed
        bool gotoX(int pos); // Return false if distance is out of bounds
        bool setXSpeed(int max_speed);

        bool homeDrill(); 
        bool gotoDrill(int pos); // Return false if distance is out of bounds
        bool setDrillSpeed(int max_speed);


        //bool startRockwell(double max_pressure);// Press probe down and melt ice (or just heat)
        bool startBowl(float speed = 1.0);// Return false if homed (or we know we aren't near ice)
        bool stopBowl();

        bool gotoProbe(int pos); // Return false if distance is out of bounds
        bool probeNotHomed();

        prismm_msgs::dam_data getData();

        bool eStop();
        bool resume();// Continue drilling or moving with motors after e stop
        bool reset();// Resume processing but reset motor movements and state

    private:
        ros::NodeHandle nh;
        const prismm_msgs::getBoolRequest probe_srv_req;
        prismm_msgs::getBoolResponse probe_srv_resp;

        bool tool_is_drill = true;
        bool e_stopped = false;
        float bowl_speed = 1.0;
        int bowl_direction = 1;
        float rot_pos = 0;
        float ext_pos = 0;

        DamState last_state = DEFAULT_STATE;
        DamState state = DEFAULT_STATE;
        prismm_msgs::dam_data data_out;

        float probe_step_per_mm = -800/2.54;
        float drill_step_per_mm = 800/2.54;
        float x_step_per_mm = 800/100;

        float probe_home_speed = 200.0;//currently in steps per second
        float drill_home_speed = 200.0;
        float x_home_speed = 200.0;

        float probe_max_speed = 800.0;//currently in steps per second
        float drill_max_speed = 800.0;
        float x_max_speed = 800.0;

        ACS712 stp_drill_current_sensor;

        Servo servo_ext;
        Servo servo_rot;

        AccelStepper stp_x;
        AccelStepper stp_drill;
        AccelStepper stp_probe;

        MovingAverageFilter drill_current_avg;

        void iterateBowl();
        void incrementProbeHome();
        void incrementDrillHome();
        void incrementXHome();
};

#endif /** _Dam_H_ **/
