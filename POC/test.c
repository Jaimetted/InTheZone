#pragma config(Sensor, in1,    gyroBase,       sensorGyro)
#pragma config(Sensor, in2,    potGripper,     sensorPotentiometer)
#pragma config(Sensor, in3,    linetracker1,   sensorReflection)
#pragma config(Sensor, in4,    linetracker2,   sensorReflection)
#pragma config(Sensor, dgtl1,  left,           sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  right,          sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  arduinoReset,   sensorDigitalOut)
#pragma config(Sensor, dgtl6,  baseSonar,      sensorSONAR_cm)
#pragma config(Motor,  port2,           left1,         tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           left2,         tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port4,           left3,         tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           mogoL,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           mogoR,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,           right1,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           right2,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           right3,        tmotorVex393TurboSpeed_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//*********************************************************************************************
//			Includes
//*********************************************************************************************

#include "../BNSLib.h";

#define STOP 0
#define DROP_MOGO 2100
#define GET_MOGO 460
#define VERTICAL_MOGO 1200
#define CHARS_PER_MESS 8
#define CHARS_PER_VAL 8
#define X_START 0
#define LINE_COLOR 2300

int rcvChar;
int startChar = 120; // Delimeter for messages char value = 'x'
int encoderR = 0,encoderL = 0, counter = 0;
float z=0;
//bool disableAPS = false; // Make true when you dont want the movement to
// affect position, e.g. when turning.

void setBase(int speed){
	motor[left1] = speed;
	motor[left2] = speed;
	motor[left3] = speed;
	motor[right1] = speed;
	motor[right2] = speed;
	motor[right3] = speed;
}

void setBase(int speedR, int speedL){
	motor[left1] = speedL;
	motor[left2] = speedL;
	motor[left3] = speedL;
	motor[right1] = speedR;
	motor[right2] = speedR;
	motor[right3] = speedR;
}

void setMOGOGripper(int speed){
	motor[mogoL] = speed;
	motor[mogoR] = speed;
}

//*********************************************************************************************
//			Base Functions
//*********************************************************************************************


// Function to move the robot to the front or back.
void moveBase(int speed)
{
	setBase(speed);
}

// Rotate the base clockwise or counter-clockwise.
void rotateBase(int speed)
{
	setBase(speed, -speed);
}

// Stop moving the robot.
void stopBase()
{
	setBase(0);
}

// Convert the distance of ticks (the value of the encoder) in inches.
float ticksToInches(int ticks)
{
	float inches = ticks*((2.75*PI)/360.0)/0.9;
	return inches;
}

float inchesToTicks(int inches)
{
	float ticks = inches*(360.0/(2.75*PI))*0.9;
	return ticks;
}


// Move the base to the front. Request the distance for know how much need to move,
// the variable time is the maximun time for wait to the movement to
// prevent keep going infinitely the task if the robot can�t move and
// finally, the variable factor to reduce the velocity of the robot.
// Note: The distance in inches.
void moveBaseWithFactor(float distance, int time, float factor){
	distance = inchesToTicks(distance);
	//writeDebugStreamLine("Start moveBaseFront");
	//writeDebugStreamLine("Target distance = %d", distance);
	int encoderAvg = 0;
	encoderR = 0;
	encoderL = 0;
	float initialGyro = SensorValue[gyroBase], actualGyro = initialGyro;
	writeDebugStreamLine("Initial Gyro Position = %f", initialGyro);
	int startEncoderValueR = SensorValue[right];
	int startEncoderValueL = SensorValue[left];
	bool atPos=false;
	float pidMovResult;
	//float pidStraightResult;
	counter = 0;

	PID pidMovement;
	//PID pidStraight;
	PIDInit(&pidMovement, 0.3, 0, 1.5); // Set P, I, and D constants
	//PIDInit(&pidStraight, 2, 0, 0.3);//Set constants for driving straight

	clearTimer(T1);
	int timer = T1;
	while(!atPos && timer < time){
		encoderR = (SensorValue[right] - startEncoderValueR);
		encoderL = SensorValue[left] - startEncoderValueL;
		encoderAvg = (encoderR+encoderL)/2;
		//writeDebugStreamLine("encR = %d\tencL = %d", encoderR, encoderL);
		pidMovResult = PIDCompute(&pidMovement, distance - encoderAvg);
		//pidStraightResult = PIDCompute(&pidStraight, initialGyro - actualGyro);
		//writeDebugStreamLine("Actual Gyro Position = %f   PID = %f", actualGyro, pidStraightResult);
		moveBase(pidMovResult);
		if (abs(encoderAvg-distance)<2)
			counter++;
		if (counter >= 4)
			atPos = true;
		timer = time1[T1];
		wait1Msec(25);
		actualGyro = SensorValue[gyroBase];
	}
	moveBase(0);
	writeDebugStreamLine("Encoder at position = %d", encoderAvg);
}

bool isOnLine(int sensor){
	/// change comparinson operator when line color changes
	if(sensor > LINE_COLOR){
		return true;
	}
	return false;
}

void moveToLine(int speed){
	bool found1 = false, found2 = false, end1 = false, end2 = false;

	while(!end1 || !end2){
		if(isOnLine(SensorValue(linetracker1))){
			found1 = true;
			clearTimer(T2);
		}
		else{
			motor[left1] = speed;
			motor[left2] = speed;
			motor[left3] = speed;
		}
		if(found1){
			if(time1[T2]<100){
				motor[left1] = -speed;
				motor[left2] = -speed;
				motor[left3] = -speed;
			}
			else{
				end1=true;
				motor[left1] = 0;
				motor[left2] = 0;
				motor[left3] = 0;
			}
		}

		if(isOnLine(SensorValue(linetracker2))){
			found2 = true;
			clearTimer(T3);
		}
		else{
			motor[right1] = speed;
			motor[right2] = speed;
			motor[right3] = speed;
		}
		if(found1){
			if(time1[T3]<100){
				motor[right1] = -speed;
				motor[right2] = -speed;
				motor[right3] = -speed;
			}
			else{
				end2 = true;
				motor[right1] = 0;
				motor[right2] = 0;
				motor[right3] = 0;
			}
		}
	}
}

float getX(char *message){
	char x_str[CHARS_PER_MESS];
	memcpy(x_str, message + X_START + 1 /* Offset */, 8 /* Length */);
	float x = atof(x_str);
	if(message[X_START] == '-') x*=-1;
	return x;
}

// Move the base to the back. Request the distance for know how much need to move,
// the variable time is the maximun time for wait to the movement to
// prevent keep going infinitely the task if the robot can�t move and
// finally, the variable factor to reduce the velocity of the robot.
// Note: The distance in inches.
void moveBaseBack(int distance, int time, int slowFactor)
{
	distance = inchesToTicks(distance);

	writeDebugStreamLine("Start moveBaseFront");
	writeDebugStreamLine("Target distance = %d", distance);

	int encoderAvg = 0;
	encoderR = 0;
	encoderL = 0;
	int startEncoderValueR = SensorValue[right];
	int startEncoderValueL = SensorValue[left];
	bool atPos=false;
	float pidMovResult, pidStraightResult;
	counter = 0;

	PID pidMovement;
	//PID pidStraight;
	PIDInit(&pidMovement, 0.3, 0, 1.5); // Set P, I, and D constants
	//PIDInit(&pidStraight, 0.5, 0, 0);//Set constants for driving straight

	clearTimer(T1);
	int timer = T1;

	while(!atPos && timer < time)
	{
		encoderR = abs(SensorValue[right] - startEncoderValueR);
		encoderL = abs(SensorValue[left] - startEncoderValueL);
		//writeDebugStreamLine("encoder derecho = %d", encoderR);
		//writeDebugStreamLine("encoder izquierdo = %d", encoderL);
		encoderAvg = (encoderR+encoderL)/2;
		pidMovResult = PIDCompute(&pidMovement, distance - encoderAvg);
		//pidStraightResult = PIDCompute(&pidStraight, encoderL - encoderR);
		pidMovResult = pidMovResult >  127 ? 127 : pidMovResult;

		moveBase(-pidMovResult);

		if (abs(encoderAvg-distance)<2)
			counter++;
		if (counter >= 4)
			atPos = true;
		timer = time1[T1];
		wait1Msec(25);
	}
	stopBase();
	writeDebugStreamLine("Encoder at position = %d", encoderAvg);
}

// Default values for configure the gyro.
float gyroAngle;
int offsetAngle = 90;


// Task for get the value of the gyro
task getGyro {
	while(1){
		gyroAngle = (SensorValue[gyroBase]/10.0 + offsetAngle);//Offset can be changed with function.
		//writeDebugStreamLine("gyro = %d", gyroAngle);
		wait1Msec(25);																	 //Default is 90.
	}
}

task arduinoComm()
{
	char message[CHARS_PER_MESS];

	setBaudRate(uartOne, baudRate115200);

	while (getChar(uartOne) != -1) // Purge existing chars from buffer
	{}

	while (true)
	{
		rcvChar = getChar(uartOne);

		if (rcvChar == -1)
		{
			// No character available

			wait1Msec(2); // Don't want to consume too much CPU time. Waiting eliminates CPU consumption for this task.
			continue;
		}
		if (rcvChar == startChar){
			int charCounter = 0;

			while(charCounter < CHARS_PER_MESS){
				rcvChar = getChar(uartOne);
				if (rcvChar == -1)
				{
					// No character available

					wait1Msec(2); // Don't want to consume too much CPU time. Waiting eliminates CPU consumption for this task.
					continue;
				}
				//writeDebugStream("%c", rcvChar);
				message[charCounter] = (char)rcvChar;
				charCounter++;
			}
			//writeDebugStream("\n");
			z = 360-getX(message)==360 ? 0 : 360-getX(message);// Aclaracion
			//writeDebugStreamLine("Angle = %f", z);
		}
	}
	wait1Msec(1);
}



void rotateToAngle(float targetAngle, int time,float P, float I, float D){
	writeDebugStreamLine("Start rotateToAngle");
	writeDebugStreamLine("Target angle = %f", targetAngle);
	bool atGyro=false, isCW=false;
	float pidGyroResult, offset, currAngle;
	int counter = 0;

	// Change negative target angle to 0-360 range
	if(targetAngle < 0){
		targetAngle += 360;
	}

	// Set offset as current angle
	offset = z;

	// Offset target
	targetAngle -= offset;
	if(targetAngle < 0)
		targetAngle += 360;

	// Is CW
	if(targetAngle > 180)
		isCW = true;

	// PID Constant cases
	PID pidGyro;
	PIDInit(&pidGyro, P,I,D); // Set P, I, and D constants
	// giro de 30 PIDInit(&pidGyro, 1.3, 0.3, 4); // Set P, I, and D constants
	clearTimer(T1);
	while(!atGyro && time1[T1] < time){
		// Calculate current angle
		currAngle = z - offset;
		if(currAngle < 0)
			currAngle += 360;
		if(isCW && currAngle < 180) // Un poquito de marron
			currAngle = 360;
		writeDebugStreamLine("Current angle = %f", currAngle);

		// Calculate error
		float error = targetAngle - currAngle;
		writeDebugStreamLine("error=%f",error);

		// Compute PID
		pidGyroResult = PIDCompute(&pidGyro, error);
		rotateBase(pidGyroResult);

		// Condition to end
		if (abs(z-targetAngle)<0.25)
			counter++;
		if (counter > 100)
			atGyro = true;
		wait1Msec(1);
	}
	writeDebugStreamLine("Final angle = %f", z);
	stopBase();
}

// Rotate the base to one side. The variable time is the maximun time for wait to the rotation to
// prevent keep going infinitely the task if the robot can�t rotate.
// The variable angle is for the finally wanted angle.
void rotateToAngle(float targetAngle, int time){
	writeDebugStreamLine("Start rotateToAngle");
	writeDebugStreamLine("Target angle = %f", targetAngle);
	bool atGyro=false, isCW=false;
	float pidGyroResult, offset, currAngle;
	int counter = 0;

	// Change negative target angle to 0-360 range
	if(targetAngle < 0){
		targetAngle += 360;
	}

	// Set offset as current angle
	offset = z;

	// Offset target
	targetAngle -= offset;
	if(targetAngle < 0)
		targetAngle += 360;

	// Is CW
	if(targetAngle > 180)
		isCW = true;

	// PID Constant cases
	PID pidGyro;
	PIDInit(&pidGyro, 2,0,10); // Set P, I, and D constants
	// giro de 30 PIDInit(&pidGyro, 1.3, 0.3, 4); // Set P, I, and D constants
	clearTimer(T1);
	while(!atGyro && time1[T1] < time){
		// Calculate current angle
		currAngle = z - offset;
		if(currAngle < 0)
			currAngle += 360;
		if(isCW && currAngle == 0)
			currAngle = 360;
		writeDebugStreamLine("Current angle = %f", currAngle);

		// Calculate error
		float error = targetAngle - currAngle;
		writeDebugStreamLine("error=%f",error);

		// Compute PID
		pidGyroResult = PIDCompute(&pidGyro, error);
		rotateBase(pidGyroResult);

		// Condition to end
		if (abs(z-targetAngle)<0.25)
			counter++;
		if (counter > 100)
			atGyro = true;
		wait1Msec(1);
	}
	writeDebugStreamLine("Final angle = %f", z);
	stopBase();
}

void genericControl(void){
	motor[left1] = vexRT[Ch3] + vexRT[Ch4];
	motor[left2] = vexRT[Ch3] + vexRT[Ch4];
	motor[left3] = vexRT[Ch3] + vexRT[Ch4];
	motor[right1] = vexRT[Ch3] - vexRT[Ch4];
	motor[right2] = vexRT[Ch3] - vexRT[Ch4];
	motor[right3] = vexRT[Ch3] - vexRT[Ch4];
	if (vexRT[Btn8R]){
		motor[mogoL] = 127;
		motor[mogoR] = 127;
	}
	else if (vexRT[Btn8L]){
		motor[mogoL] = -127;
		motor[mogoR] = -127;
	}
	else{
		motor[mogoL] = 0;
		motor[mogoR] = 0;
	}
}

void setPositionMogo(int angle){
	clearTimer(T2);
	if(SensorValue[potGripper] < angle)
	{
		while(SensorValue[potGripper] < angle && time1(T2) < 4000)
		{
			setMOGOGripper(127);
		}
	}
	else
	{
		while(SensorValue(potGripper)>angle && time1(T2) < 4000)
		{
			setMOGOGripper(-127);
		}
	}
	setMOGOGripper(0);
}

void initializeSensors (){
	BNS();
	//Initialize Encoders
	SensorValue[right] = 0;
	SensorValue[left] = 0;
	//Reset Arduino
	SensorValue[arduinoReset] = 0;
	wait1Msec(500);
	SensorValue[arduinoReset] = 1;
	wait1Msec(5000);
}

void moveBaseUntil(int distance,int time){
	clearTimer(T1);
	int timer = T1;
	if(SensorValue(baseSonar) > distance)
	{
		while(SensorValue(baseSonar) > distance  && timer < time){
			//writeDebugStreamLine("%d\n",SensorValue(baseSonar));
			moveBase(127);
			timer = time1[T1];
		}
		moveBase(-10);
	}
	else
	{
		while(SensorValue(baseSonar) < distance  && timer < time){
			//writeDebugStreamLine("%d\n",SensorValue(baseSonar));
			moveBase(-127);
			timer = time1[T1];
		}
		moveBase(10);
	}
	wait1Msec(100);
	moveBase(0);
	//writeDebugStreamLine("Sonar = %d", SensorValue(baseSonar));
}


void progSkills()
{
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(20,2000,1);
	setPositionMogo(GET_MOGO);
	moveBaseWithFactor(5,2000,1);
	rotateToAngle(70,2000,0.78,0,1.8);
	moveBaseWithFactor(4.5,1000,1);
	rotateToAngle(10,1000,1.5,0,1.8);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-15);
	moveBaseWithFactor(8,700,1);
	setPositionMogo(VERTICAL_MOGO+700);
	setMOGOGripper(0);
	moveBaseBack(20,2000,1);
	setPositionMogo(GET_MOGO);
	//rotateToAngle(0,2000);
	wait1Msec(200);
	moveToLine(30);
	wait1Msec(200);
  //moveBaseBack(5,2000,1);
	rotateToAngle(-75,2000,1,0,1.8);
	moveBaseUntil(65,5000);
	//moveBaseWithFactor(5,2000,1);
	rotateToAngle(-130,2000,2,0,1.8);
	setPositionMogo(DROP_MOGO);
	//wait1Msec(500000);
	moveBaseWithFactor(13,2000,1);
	//moveBaseBack(4,2000,1);
	setPositionMogo(VERTICAL_MOGO+685);
	rotateToAngle(-180,2000,2,0,1.8);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(8,2000,1);
	rotateToAngle(90,2000,0.78,0,1.8);
	moveBaseWithFactor(20,2000,1);
	//moveBaseBack(4,2000,1);
	setPositionMogo(GET_MOGO);
	wait1Msec(20000);
	moveBaseWithFactor(7,1000,1);//UUUUUUUUGGGGGGHHHH
	wait1Msec(200);
	rotateToAngle(274,1000);
	//Repeat
	moveBase(-50);
	wait1Msec(750);
	moveBase(0);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(20,2000,1);
	setPositionMogo(GET_MOGO);
	moveBaseWithFactor(10,1000,1);
	rotateToAngle(360, 1000);
	moveBaseWithFactor(6,1000,1);
	rotateToAngle(270,2000);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-15);
	moveBaseWithFactor(15,700,1);
	setPositionMogo(VERTICAL_MOGO+700);
	setMOGOGripper(0);
	moveBaseBack(20,2000,1);
	setPositionMogo(GET_MOGO);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

task main()
{
	initializeSensors();
	startTask(arduinoComm);


	progSkills();


}
