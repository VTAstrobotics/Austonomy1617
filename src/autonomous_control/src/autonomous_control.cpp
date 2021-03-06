#include <ros/ros.h>
#include <autonomous_control/autonomous_control.h>
#include <boost/foreach.hpp>
#include <XmlRpcException.h>
#include <ros/console.h>
#include <log4cxx/logger.h>
#include <iostream>
namespace autonomous_control{

	AutonomousControl::AutonomousControl(ros::NodeHandle& nh)
	{
		pub = nh.advertise<apriltags_ros::MetaPose>("/tag_check",1);
		motor_command_ = nh.advertise<robot_msgs::Autonomy>("/robot/autonomy",1);
		cali_command_ = nh.advertise<std_msgs::Bool>("cali_command",1);
		mappingSignal = nh.advertise<std_msgs::Bool>("startMapping",1);
		camSub = nh.subscribe("filteredCamData", 1, &AutonomousControl::tag_seen,this);
		imuSub = nh.subscribe("imu/converted", 1, &AutonomousControl::getImu,this);
	//	lidarSweep = nh.subscribe("lidarSweep", 1, &AutonomousControl::getLidar,this);
		stripeArray = nh.subscribe("stripe_obs_array", 1, &AutonomousControl::getStripe,this);
		idle = nh.subscribe("robot/autonomy/enable", 1, &AutonomousControl::Idleing,this);
		feedback = nh.subscribe("robot/motor/feedback", 1, &AutonomousControl::Feedback, this);
		ir0sub = nh.subscribe("IR0", 1, &AutonomousControl::IR0, this);
		ir1sub = nh.subscribe("IR1", 1, &AutonomousControl::IR1, this);

		ROSCONSOLE_AUTOINIT;
		log4cxx::LoggerPtr my_logger = log4cxx::Logger::getLogger(ROSCONSOLE_DEFAULT_NAME);
		my_logger->setLevel(ros::console::g_level_lookup[ros::console::levels::Debug]);


		/*motor_command.leftRatio = 0.0;
		motor_command.rightRatio = 0.0;
		motor_command.digCmd = 0.0;
		motor_command.dumpCmd = 0.0;*/

		posX = 0.0;
		posY = 0.0;
		posZ = 0.0;
		oX = 0.0;
		oY = 0.0;
		oZ = 0.0;
		oW = 0.0;
		pX = 0.0;
		pY = 0.0;
		obsFieldStart = 1.5;
		detected=false;
		imuX=0.0;
		imuY=0.0;
		imuZ=0.0;
		imuW=0.0;
		prevZ=0.0;
		state = Prep;
		prevState = state;
		LorR = 0;
		numRot = 0;
		imuForward = 0;
		angleTargeted= false;
		faceForward = false;  // bool used to check whether bot is facing forward
		forwardRatio = 0.14;
		backwardRatio = -0.14;
		brake = 0.0;

		inObsField = false;	//boolean used to switch back to obstacle field case after relocating beacon
		count = 0;
		waitComplete = false;
		cycleCount = 0;

		mapPub.data = false;

		go = false;
		startup = true;

		drumForward = 1.0;
		drumReverse = -1.0;

		liftLowerLimit = 150;
		liftUpperLimit = 50;

		PathFinder path(); // this will create the path object that we will use

		//ros::Duration(3.0).sleep();
		ROS_DEBUG_ONCE("Starting Autonomous Control");

	}

	void AutonomousControl::tag_seen(const apriltags_ros::MetaPose& pose){
		posX = pose.pose.position.x;
		posY = pose.pose.position.y;
		posZ = pose.pose.position.z;
		oX = pose.pose.orientation.x;
		oY = pose.pose.orientation.y;
		oZ = pose.pose.orientation.z;
		oW = pose.pose.orientation.w;
		pX = pose.px;
		pY = pose.py;
		detected = pose.detected;
		cali.data=true;
		//ROS_INFO_STREAM("Field Position is x = " << posX << ",  y = " << posY);
	}

	void AutonomousControl::getImu(const sensor_msgs::Imu& imu){
		imuX=imu.orientation.x;
		imuY=imu.orientation.y;
		imuZ=imu.orientation.z;
		imuW=imu.orientation.w;
	}
/*
	void AutonomousControl::getLidar(const std_msgs::Lidar& lidar){

	}
*/
	void AutonomousControl::getStripe(const std_msgs::Int8MultiArray& stripe){

	}

	void AutonomousControl::Idleing(const std_msgs::Bool& cmd){
		go = cmd.data;
		ROS_INFO_STREAM("go command is "<<go);
		if(cmd.data && startup){
			ROS_INFO_STREAM("go command is "<<go);
			state=FindBeacon;
			startup = false;
		}
	}

	void AutonomousControl::Feedback(const robot_msgs::MotorFeedback& mf){
		leftRPM = mf.leftTreadRPM;
		rightRPM = mf.rightTreadRPM;
		drumRPM = mf.drumRPM;
		liftPos = mf.liftPos;
	}

	void AutonomousControl::IR0(const std_msgs::Int8& val){
		ir0=val.data;
	}

	void AutonomousControl::IR1(const std_msgs::Int8& val){
		ir1=val.data;
	}

	void AutonomousControl::primary(){
		updateIMU();
                // std::cout << state << "before case"<< std::endl;
		if(!go) {
			state = Idle; // changes back to idle as long as not go
		}
		switch(state) {
			case Idle:
				halt();
			      	waitComplete = true;
     				prevState = state;
				state = Prep;
			break;

			case Prep:
				motor_command.liftDown=false;
				motor_command.liftUp=true;
				motor_command.liftSpeed = 5000;
				hold(20);
				if(waitComplete){
					waitComplete=false;
					count=0;
					prevState=state;
					state=FindBeacon;
				}
			break;
                       // std::cout << state << "before case" << std::endl;
			case FindBeacon:
				if(oW != 1.0){
					motor_command.leftRatio=forwardRatio;
					motor_command.rightRatio=backwardRatio;
					cali_command_.publish(cali);
					cali.data=false;
					ROS_DEBUG_ONCE("Looking for Target");
					ROS_DEBUG_ONCE("Calibrating lidar");
				}
				else{
					//ros::Duration(2.0).sleep(); // sleep for two seconds
					imuForward = imuZ + 180 - oZ;  //Calculate forward IMU Angle
					LOrR();
					turn = true;
					waitComplete = false;
					prevState = state;
					state=Wait;
					ROS_DEBUG_ONCE("Found Target");
					halt();
					break;
				}
			break;

			case Wait:
				hold(20);
				if (waitComplete){
					updateIMU();
					updateTag();
					prevState = state;
					state = Orient90;
					tempZ = imuZ;  //Store IMU Angle at halt
					ROS_DEBUG_STREAM("Tag angle " << oZ);
					ROS_DEBUG_STREAM("IMU angle " << imuZ);
					waitComplete = false;
					count = 0;
				}
			break;

			if (moveComplete == false) {
				case Orient90:
					switch (LorR) {
						case 0:
						prevState = state;
							state=Orient180;
						break;

						case -1:
							if ( oZStore < 270 - 3 && oZStore > 90) { //This works
								//Right hand side, facing quadrant 1 or 2
								target90L(270 - oZStore);
								ROS_DEBUG_ONCE("Turning Left 1");
								if(newZ < targetAng){
									motor_command.rightRatio = forwardRatio;
									motor_command.leftRatio = backwardRatio;
								}
								else{
									halt();
									prevState = state;
									state = DriveToCenter;
									moveComplete = true;
								}
							}
							else if ( oZStore > 270 + 3) { //will likely never happen
								// Right Hand side, facing quadrant 3
								target90R(oZStore - 180);
								ROS_DEBUG_ONCE("Turning Right 2");
								if(newZ > targetAng){
									motor_command.rightRatio = backwardRatio;
									motor_command.leftRatio = forwardRatio;
								}
								else{
									halt();
									state = DriveToCenter;
									prevState = state;
									moveComplete = true;
								}
							}
							else if (oZStore < 90) { //This works 4/3
								ROS_DEBUG_ONCE("Turning Right 3");
								// Right Hand Side, Facing Quadrant 4
								target90R(90 + oZStore);
								if(newZ > targetAng){
									motor_command.rightRatio = backwardRatio;
									motor_command.leftRatio = forwardRatio;
								}
								else{
									halt();
									prevState = state;
									state = DriveToCenter;
									moveComplete = true;
								}
							}
						break;

						case 1:
							if ( oZStore < 270 && oZStore > 90 + 3) { //this works
								// Left hand side, facing Quadrant 1 or 2
								target90R(oZStore - 90);
								ROS_DEBUG_ONCE("Turning Right 4");
								if(newZ > targetAng){
									motor_command.rightRatio = backwardRatio;
									motor_command.leftRatio = forwardRatio;
								}
								else{
									halt();
									prevState = state;
									state = DriveToCenter;
									moveComplete = true;
								}
							}
							else if ( oZStore> 270) {
								// Left Hand Side, Facing Quadrant 3
								target90L(450 - oZStore);
								ROS_DEBUG_ONCE("Turning Left 5");
								if(newZ < targetAng){
									motor_command.rightRatio = forwardRatio;
									motor_command.leftRatio = backwardRatio;
								}
								else{
									halt();
									prevState = state;
									state = DriveToCenter;
									moveComplete = true;
								}
							}
							else if (oZStore < 90 - 3) {
								// Left Hand Side, Facing Quadrant 4
								target90L(90 - oZStore);
								ROS_DEBUG_ONCE("Turning Left 6");
								if(newZ < targetAng){
									motor_command.rightRatio = forwardRatio;
									motor_command.leftRatio = backwardRatio;
								}
								else{
									halt();
									prevState = state;
									state = DriveToCenter;
									moveComplete = true;
								}
							}
						break;
						}
					}
				break;

			case DriveToCenter:
				ROS_DEBUG_ONCE("I'm driving to the center");
				if(LorR == -1){
					if(posX < 0.0 ){
						motor_command.rightRatio=forwardRatio;
						motor_command.leftRatio=forwardRatio;
					}
					else{
						halt();
						prevState = state;
						state = Orient180;
					}
				}
				else if(LorR == 1){
					if(posX > 0.0){
						motor_command.rightRatio=forwardRatio;
						motor_command.leftRatio=forwardRatio;
					}
					else{
						halt();
						prevState = state;
						state = Orient180;
					}
				}
				else{
					halt();
					prevState = state;
					state=Orient180;
				}
			break;

			case Orient180:  //This case is called to target the forward angle when apriltags are in sight
				if(oZ < 177){
					target180(180-oZ);
					if(newZ < targetAng){
					ROS_DEBUG_ONCE("Turning Left to Face Forward");
					motor_command.rightRatio = forwardRatio;
					motor_command.leftRatio = backwardRatio;
					}
					else{
						halt();
						state = DriveToObsField;
						updateIMU();
						imuForward = newZ;
						faceForward = true;
						prevState = Orient180;
						/* this is getting run after the if statements
						if(inObsField){
							state = DriveToMine;
						}
						*/
					}
				}
				else if(oZ > 183) {
					target180(180-oZ);
					if(newZ > targetAng){
					ROS_DEBUG_ONCE("Turning Right to Face Forward");
					motor_command.rightRatio = backwardRatio;
					motor_command.leftRatio = forwardRatio;
					}
					else{
						halt();
						//state=DriveToObsField;
						state = DriveToObsField;
						updateIMU();
						imuForward = newZ;
						faceForward = true;
						prevState = Orient180;
						/* this is getting run after the if statements
						if(inObsField){
							state = DriveToMine;
						}
						*/
					}
				}
				// being run and checked here
				// check to see if we're in the obstacle
				if(inObsField){
					prevState = state;
					state = DriveToMine;
				}
				else{ // we are not in the obstacle field
					prevState = state;
					state = DriveToObsField;
				}
			break;

			case Orient180imu:  //This case is called to target the forward angle when apriltags are NOT in sight.
								//Note that this can only be called once the imu is calibrated (using previous vision of the tag)
				updateIMU();
			//	if(oZ < 177){
				if(newZ < (imuForward-7.0)){
					target180(imuForward - newZ);
					if(newZ < imuForward){
					ROS_DEBUG_ONCE("Turning Left to Face Forward");
					motor_command.rightRatio = forwardRatio;
					motor_command.leftRatio = backwardRatio;
					}
					else{
					halt();
					state=prevState;
					prevState = Orient180imu;
					}
				}
			//	else if(oZ > 183) {
				else if(newZ>(imuForward+7.0)){
					target180(imuForward - newZ);
					if(newZ > imuForward){
					ROS_DEBUG_ONCE("Turning Right to Face Forward");
					motor_command.rightRatio = backwardRatio;
					motor_command.leftRatio = forwardRatio;
					}
					else{
					halt();
					//state=DriveToObsField;
					state = prevState;
					prevState = Orient180imu;
					}
				}
			break;
		//Arduino sends lidar data from the NUC and that sends data to the Beaglebone black
		//have to write ROS code in obstacle algo v10
			case DriveToObsField:
				ROS_DEBUG_ONCE("Driving to Start of Obstacle Field");
				updateIMU();
				if(posY < obsFieldStart){
					motor_command.rightRatio=forwardRatio;
					motor_command.leftRatio=forwardRatio;
				}
				else{
					halt();
					state=DriveToMine;
				}
			break;

			case DriveToMine:
				// if oW exists, use orient 180 case, otherwise use orient180imu case.
				//prevState = state;
				updateIMU();
				inObsField = true;
				path->autonomyAlgorithm(); // this will create the best path and run the robot
				prevState = state;
				state = Mining;

				/*if(posY < 5){
					motor_command.rightRatio = forwardRatio;
					motor_command.leftRatio = forwardRatio;
					hold(10);
					if (waitComplete){

						waitComplete = false;
						count = 0;
						halt();
						if (oW){
							state = Orient180;
							break;
						}
						else{
							state = Orient180imu;
							break;
						}
					}

				}*/
				// if imu slips more than 10 degrees from recored forward value, reorient
				// if posx is more than 80 cm, recenter
				/*
				if(posY < 5){
					motor_command.rightRatio = forwardRatio;
					motor_command.leftRatio = forwardRatio;
					if(newZ<(imuForward-10) || newZ>(imuForward+10)){
						halt();
						state = Orient180imu;
						break;
					}
//					else if (newZ>(imuForward+10)){
//						halt();
//						state = Orient180imu;
//						break;
//					}
					else if (posX>1.0 || posX <-1.0){
						halt();
						state = FindBeacon;
						break;
					}
//					else if (posX<-1.0){
//						halt();
//						state = FindBeacon;
//						break;
//					}
				}
				else{
					halt();
					prevState = state;
					state = Mining;
				}
				*/
			break;



			case Mining:
				//forward drum
				motor_command.drumRatio = drumForward;
				//lift down
				if (liftPos>liftLowerLimit){
					motor_command.liftDown = true;
					motor_command.liftUp = false;
					motor_command.liftSpeed = 5000;
				}
				hold(40);
				if(waitComplete){
					motor_command.drumRatio=brake;
					motor_command.liftDown = false;
					motor_command.liftUp = true;
					motor_command.liftSpeed = 5000;
					state = Deposit;
				}
			break;

			case DepositPrep:
				motor_command.liftUp = true;
				motor_command.liftDown = false;
				hold(30);
				if(waitComplete){
					waitComplete = false;
					count = 0;
					motor_command.liftUp = false;
					motor_command.liftDown = false;
					prevState = state;
					state = Deposit;
				}
			break;

			case Deposit:
				//reverse drum
				motor_command.drumRatio=drumReverse;
				hold(30);
				if(waitComplete){
					halt();
					prevState = state;
					state = ReturnToObs;
					waitComplete = false;
					count = 0;
				}
			break;

			case ReturnToObs:
				/*if(posY > 4.44){
					motor_command.rightRatio = backwardRatio;
					motor_command.leftRatio = backwardRatio;
				}
				else{
					halt();
					state = ReturnToBin;
				}
				*/
				path->runBackwards();
				state = ReturnToBin;
			break;

			case ReturnToBin:
				if(posY > 1){
					motor_command.rightRatio = backwardRatio;
					motor_command.leftRatio = backwardRatio;
				}
				else if(ir0 > 30 && ir1 >30){
					motor_command.rightRatio = backwardRatio;
					motor_command.leftRatio = backwardRatio;
				}
				else{
					halt();
					state = DeadMan;
				}
			break;

			case DeadMan:
				motor_command.rightRatio = 0.05;
				motor_command.leftRatio = 0.05;
				hold(30);
				if(waitComplete){
					state = DumpPrep;
					waitComplete = false;
					count = 0;
				}
			break;

			case DumpPrep:
				//dump
				motor_command.liftDown=true;
				motor_command.liftUp=false;
				hold(30);
				if(waitComplete){
					state = Dump;
					motor_command.liftDown=false;
					motor_command.liftUp=false;
					waitComplete = false;
					count = 0;
				}
			break;

			case Dump:
				motor_command.storageUp = true;
				motor_command.storageDown = false;
				hold(30);
				if(waitComplete){
					cycleCount++;
					state = DumpFinish;
					waitComplete=false;
					count=0;
				}
			break;

			case DumpFinish:
				motor_command.storageUp=false;
				motor_command.storageDown=true;
				hold(30);
				if(waitComplete){
					waitComplete=false;
					count=0;
					halt();
					//state = TravelPrep;
				}
			break;

			case TravelPrep:
				motor_command.liftUp=true;
				motor_command.liftDown=false;
				hold(30);
				if(waitComplete){
					halt();
					waitComplete=false;
					count=0;
					if(cycleCount < 2){
						state = DriveToMine;
					}
					else{
						state = Halt;
					}
				}
			break;

			case Halt:
				ROS_DEBUG_ONCE("HALT!");
				halt();
			break;


			default:
				halt();
			break;
		}
		mappingSignal.publish(mapPub);  // Mapping Signal for Arduino
		motor_command_.publish(motor_command);
		oW = 0.0;  // Reset the apriltag boolean so we can track whether tags are in vision or not

		/*if(!detected){
			motor_command.leftRatio = 150;
			motor_command.rightRatio = 150;
			//motor_command.turn=true;
			motor_command_.publish(motor_command);
			ROS_INFO_STREAM("Turning");
		}
		else{
			motor_command.leftRatio = 90;
			motor_command.rightRatio = 90;
			//motor_command.turn=false;
			motor_command_.publish(motor_command);
			ROS_INFO_STREAM("I see it");
		}*/
		//oW = 0.0;
	}

	void AutonomousControl::halt(){
		ROS_DEBUG_ONCE("Halt Command Called");
		motor_command.leftRatio=brake;
		motor_command.rightRatio=brake;
		motor_command.drumRatio=brake;
		motor_command.liftUp=false;
		motor_command.liftDown=false;
		motor_command.storageUp=false;
		motor_command.storageDown=false;
		motor_command_.publish(motor_command);
	}

	void AutonomousControl::LOrR(){
		if (posX < -0.15) {     //right
			LorR = -1;
			ROS_DEBUG_ONCE("I'm on the right (negative x)");
		}
		else if (posX > 0.15) { //left
			LorR = 1;
			ROS_DEBUG_ONCE("I'm on the left (positive x)");
		}
		else {          //center
			LorR = 0;
			ROS_DEBUG_ONCE("I'm already at the center");
		}
	}

	void AutonomousControl::target90R(float desired){
		if(turn){
			targetAng = newZ - desired;
			ROS_DEBUG_STREAM("Target angle " << targetAng);
		}
		updateIMU();

		turn = false;
	}

	void AutonomousControl::target90L(float desired){
		if(turn){
			targetAng = newZ + desired;
			ROS_DEBUG_STREAM("Target angle " << targetAng);
		}
				updateIMU();

		turn = false;
	}



	void AutonomousControl::updateIMU(){
		prevZ = tempZ -360*numRot;
		tempZ = imuZ;



		if ((prevZ >= 270.00 && prevZ <= 360.00)&&(tempZ>=0.0 && tempZ <=90.0)) {
			numRot = numRot+1;
			newZ = tempZ + 360*numRot;
			ROS_DEBUG_ONCE("IMU positive turn over");
		}
		else if ((prevZ <=90.0 && prevZ>=0.0)&&(tempZ<=360 && tempZ >=270)) {
			numRot = numRot -1;
			newZ = tempZ + 360*numRot;
			ROS_DEBUG_ONCE("IMU negative turn over");
		}
		else{
			newZ = tempZ + 360*numRot;
		}
		//ROS_INFO_STREAM("Updated IMU Angle is " << newZ);
	}

	void AutonomousControl::updateTag(){
		oZStore = oZ;
		ROS_DEBUG_STREAM("Storing Orientation of " << oZStore);
		moveComplete = false;
	}

	void AutonomousControl::target180(float desired){
		updateIMU();
		if(!angleTargeted){
			targetAng = newZ + desired;
			ROS_DEBUG_STREAM("Turning Forward by " << targetAng);
		}
		angleTargeted = true;

	}


	void AutonomousControl::hold(int waitTime){
		ROS_INFO_STREAM("waitTime " << waitTime);
		ROS_INFO_STREAM("count " << count);
		if(count < waitTime){
			count++;
		}
		else{
			waitComplete = true;
		}
	}


}
