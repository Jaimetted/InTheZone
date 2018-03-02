#pragma config(Sensor, in1,    potGripper,     sensorPotentiometer)
#pragma config(Sensor, in2,    potElevation,   sensorPotentiometer)
#pragma config(Sensor, in3,    potChainbar,    sensorPotentiometer)
#pragma config(Sensor, in4,    gyroBase,       sensorGyro)
#pragma config(Sensor, in5,    accBase,        sensorAccelerometer)
#pragma config(Sensor, dgtl1,  encLeft,        sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  encRight,       sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  baseSonar,      sensorSONAR_cm)
#pragma config(Sensor, dgtl7,  coneSonar,      sensorSONAR_cm)
#pragma config(Motor,  port1,           moGoGripper,   tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           frontLeft,     tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           frontRight,    tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           chainbarLeft,  tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           elevationLeft, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           elevationRight, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           chainbarRight, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           backLeft,      tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port9,           backRight,     tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          rollers,       tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//*********************************************************************************************
//			Includes
//*********************************************************************************************

#include "BNSLib.h";

#define STOP 0
#define CHAINBAR_HORIZONTAL 1465
#define CHAINBAR_VERTICAL 3700
#define MAX_CONE_DISTANCE 30
#define MAX_ELEVATION 3170
#define MAX_CONES 17
#define DOWN 1225

int trigger = 0;
int coneCounter = 4;//Cones that can be managed with chainbar only

//*********************************************************************************************
//			Setters
//*********************************************************************************************

void setBase(int speed){
	motor[frontLeft] = speed;
	motor[frontRight] = speed;
	motor[backLeft] = speed;
	motor[backRight] = speed;
}
void setBaseOffset(int speed, int offset){
	motor[frontLeft] = speed - offset;
	motor[frontRight] = speed + offset;
	motor[backLeft] = speed - offset;
	motor[backRight] = speed + offset;
}
void setBase(int speedL, int speedR){
	motor[frontLeft] = speedL;
	motor[frontRight] = speedR;
	motor[backLeft] = speedL;
	motor[backRight] = speedR;
}
void setRollers(int speed){
	motor[rollers] = speed;
}
void setRollers(int speed, int time_ms){
	setRollers(speed);
	wait1Msec(time_ms);
	setRollers(STOP);
}
void setMOGOGripper(int speed){
	motor[moGoGripper] = speed;
}
void setElevation(int speed){
	motor[elevationLeft] = speed;
	motor[elevationRight] = speed;
}
void setChainbar(int speed){
	motor[chainbarLeft] = speed;
	motor[chainbarRight]= speed;
}

//*********************************************************************************************
//			Base Functions
//*********************************************************************************************

// This variables for the ecnoders and counter fore the PID.
int encoderL, encoderR, counter;

// Function to move the robot to the front or back.
void moveBase(int speed)
{
	setBase(speed);
}

// Funtion to correct the aligment of the robot when move to the front or back.
void moveBase(int speed, int offset)
{
	setBaseOffset(speed, offset);
}

// Rotate the base clockwise or counter-clockwise.
void rotateBase(int speed)
{
	setBase(-speed, speed);
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
void moveBaseWithFactor(int distance, int time, float factor){
	distance = inchesToTicks(distance);
	writeDebugStreamLine("Start moveBaseFront");
	writeDebugStreamLine("Target distance = %d", distance);
	int encoderAvg = 0;
	encoderR = 0;
	encoderL = 0;
	float initialGyro = SensorValue[gyroBase], actualGyro = initialGyro;
	writeDebugStreamLine("Initial Gyro Position = %f", initialGyro);
	int startEncoderValueR = SensorValue[encRight];
	int startEncoderValueL = SensorValue[encLeft];
	bool atPos=false;
	float pidMovResult;
	//float pidStraightResult;
	counter = 0;

	PID pidMovement;
	//PID pidStraight;
	PIDInit(&pidMovement, 0.5, .1, 0.25); // Set P, I, and D constants
	//PIDInit(&pidStraight, 2, 0, 0.3);//Set constants for driving straight

	clearTimer(T1);
	int timer = T1;
	while(!atPos && timer < time){
		encoderR = (SensorValue[encRight] - startEncoderValueR);
		encoderL = SensorValue[encLeft] - startEncoderValueL;
		encoderAvg = (encoderR+encoderL)/2;
		writeDebugStreamLine("encR = %d\tencL = %d", encoderR, encoderL);
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
	int startEncoderValueR = SensorValue[encRight];
	int startEncoderValueL = SensorValue[encLeft];
	bool atPos=false;
	float pidMovResult, pidStraightResult;
	counter = 0;

	PID pidMovement;
	//PID pidStraight;
	PIDInit(&pidMovement, 0.2, 0, 0.018); // Set P, I, and D constants
	//PIDInit(&pidStraight, 0.5, 0, 0);//Set constants for driving straight

	clearTimer(T1);
	int timer = T1;

	while(!atPos && timer < time)
	{
		encoderR = abs(SensorValue[encRight] - startEncoderValueR);
		encoderL = abs(SensorValue[encLeft] - startEncoderValueL);
		writeDebugStreamLine("encoder derecho = %d", encoderR);
		writeDebugStreamLine("encoder izquierdo = %d", encoderL);
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

// Configure the default values of the gyro.
void setOffsetAngle (int angle){
	offsetAngle = angle;
}

// Task for get the value of the gyro
task getGyro {
	while(1){
		gyroAngle = (SensorValue[gyroBase]/10.0 + offsetAngle);//Offset can be changed with function.
		//writeDebugStreamLine("gyro = %d", gyroAngle);
		wait1Msec(25);																	 //Default is 90.
	}
}

// Rotate the base to one side. The variable time is the maximun time for wait to the rotation to
// prevent keep going infinitely the task if the robot can�t rotate.
// The variable angle is for the finally wanted angle.
void rotateToAngle(float targetAngle, int time){
	writeDebugStreamLine("Start rotateToAngle");
	writeDebugStreamLine("Target angle = %f", targetAngle);
	bool atGyro=false;
	float pidGyroResult;
	counter = 0;

	PID pidGyro;
	PIDInit(&pidGyro, 0.1, 0, 0.3); // Set P, I, and D constants

	clearTimer(T1);
	int timer = time1[T1];
	while(!atGyro && timer < time){
		pidGyroResult = PIDCompute(&pidGyro, targetAngle - gyroAngle);
		rotateBase(18*pidGyroResult);
		if (abs(gyroAngle-targetAngle)<0.1)
			counter++;
		if (counter > 3)
			atGyro = true;
		timer = time1[T1];
		wait1Msec(20);
	}
	stopBase();
}

// Rotate the base to one side. The variable time is the maximun time for wait to the rotation to
// prevent keep going infinitely the task if the robot can�t rotate.
// The variable angle is for the amount of angle to rotate.
void rotateThisAngle(float angle, int time){
	writeDebugStreamLine("Start rotateThisAngle");
	writeDebugStreamLine("Target angle = %f", angle);
	bool atGyro = false;
	float pidGyroResult;
	counter = 0;

	PID pidGyro;
	PIDInit(&pidGyro, 0.3, 0, 0.4); // Set P, I, and D constants

	angle = (gyroAngle + angle)%360;

	clearTimer(T1);
	int timer = time1[T1];
	while(!atGyro && timer < time){
		pidGyroResult = PIDCompute(&pidGyro, angle - gyroAngle);
		rotateBase(15*pidGyroResult);
		if (abs(gyroAngle-angle)<2)
			counter++;
		if (counter > 3)
			atGyro = true;
		timer = time1[T1];
		wait1Msec(20);
	}
	stopBase();
}

void moveBase (int distance, int speed) {
	while(SensorValue(encLeft)<distance && SensorValue(encRight)) {
		if(encLeft == encRight) {
			setBase(speed);
		}
		else if(encLeft > encRight){
			setBase(speed*0.90, speed);
		}
		else{
			setBase(speed, speed*0.90);
		}
	}
}

void baseControl(void){
	setBase(vexRT(Ch3) + vexRT(Ch4),vexRT(Ch3) - vexRT(Ch4));
}

void rollerControl(){
	if(vexRT(Btn6U)){
		setRollers(127);
	}
	else if(vexRT(Btn6D)){
		setRollers(-127);
	}
	else{
		setRollers(STOP);
	}
}

void moGoControl(void){
	if(vexRT(Btn8R)){
		setMOGOGripper(80);
	}
	else if(vexRT(Btn8L)){
		setMOGOGripper(-127);
	}
	else{
		setMOGOGripper(STOP);
	}
}

void chainbarHold(void){
	if(SensorValue(potChainbar) > CHAINBAR_HORIZONTAL){
		setChainbar(-20);
	}
	else if(SensorValue(potChainbar) < CHAINBAR_HORIZONTAL){
		setChainbar(20);
	}
	else{
		setChainbar(STOP);
	}
}
void elevationHold(){
	setElevation(17);
}

void elevationControl(void){
	setElevation(vexRT(Ch2));
}

void setPositionCB(int angle){
	while(SensorValue[potChainbar] != angle){
		if(SensorValue(potChainbar)<angle){
			setChainbar(127);
			baseControl();
			elevationControl();
		}
		else if(SensorValue(potChainbar)>angle){
			setChainbar(-127);
			baseControl();
			elevationControl();
		}
	}
	setChainbar(STOP);
	setChainbar(7);
	baseControl();
}

void chainbarControl(){
	if(vexRT(Btn8U)){
		baseControl();
		setPositionCB(CHAINBAR_VERTICAL);
		setChainbar(15);
		setElevation(10);
	}
	else if(vexRT(Btn8D)){
		baseControl();
		setPositionCB(CHAINBAR_HORIZONTAL);
		setChainbar(15);
		setElevation(10);
	}
}

void setPositionDR(int angle){
	while(SensorValue[potElevation]!= angle){
		if(SensorValue(potElevation)<angle){
			setElevation(127);
		}
		else if(SensorValue(potElevation)>angle){
			setElevation(-127);
		}
	}
	elevationHold();
}
bool sonarElevation(void){
	while(SensorValue(coneSonar) < MAX_CONE_DISTANCE){
		// && coneCounter < MAX_CONES-1
		//coneCounter++;
		setElevation(127);
		if(SensorValue(potElevation) > MAX_ELEVATION - 50){//Worst Case
			setElevation(STOP);
			return true;
		}
	}
	elevationHold();
	return false;
}
void triggerHappy(void){
	if(vexRt(Btn5U)){
		setPositionCB(CHAINBAR_HORIZONTAL);
		sonarElevation();
		setPositionCB(CHAINBAR_VERTICAL);
		wait1Msec(400);
		setRollers(-127, 250);
		wait1Msec(100);//Let the Cone Settle
		setPositionCB(CHAINBAR_HORIZONTAL);
		setChainbar(25);
		setPositionDR(DOWN+100);
		wait1Msec(100);
		setChainbar(0);
	}
}

void genericControl(void){
	baseControl();
	elevationControl();
	rollerControl();
	chainbarControl();
	moGoControl();
	triggerHappy();
}

void init()
{
	BNS();

	//Initialize Gyro
	SensorType[gyroBase] = sensorNone;
	wait1Msec(1000);
	SensorType[gyroBase] = sensorGyro;
	wait1Msec(2000);
	//End Initialize Gyro

	SensorValue[encRight] = 0;
	SensorValue[encLeft] = 0;

	startTask(getGyro);

	clearDebugStream();
}

void moveBaseUntil(int distance,int time){
	int count = 0;
	bool atPos = 0;
	float pidMovResult;
	PID pidMovement;
	PIDInit(&pidMovement, 0.15, .1, 0.25); // Set P, I, and D consttime
	clearTimer(T1);
	int timer = T1;
	setPositionCB(CHAINBAR_VERTICAL);
	while(!atPos && timer < time){
		pidMovResult = PIDCompute(&pidMovement, distance - SensorValue(baseSonar));
		moveBase(pidMovResult);
		if (abs(SensorValue(baseSonar) - distance) < 5)
			count++;
		if (count >= 5)
			atPos = true;
		timer = time1[T1];
	}
	moveBase(0);
	writeDebugStreamLine("Sonar = %d", SensorValue(baseSonar));
}


task main()
{

	while(true){

		writeDebugStreamLine("%d\n",SensorValue(potElevation));
		//wait10Msec(25);
		genericControl();
		//trigger = 1;
		//sonarElevation();
	}
}

// Cuanto sube para llegar al poste
// Cuan cerca del poste debe estar (sonar value)

//subir acercar y soltar, regresar
