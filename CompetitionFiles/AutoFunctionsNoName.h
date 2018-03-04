#define AutoFunctionsNoName.h
#pragma systemFile

//*********************************************************************************************
//			Includes
//*********************************************************************************************

#include "../BNSLib.h";

#define STOP 0
#define CHAINBAR_HORIZONTAL 285
#define CHAINBAR_VERTICAL 1850
#define CHAINBAR_DOWN 60
#define MAX_CONE_DISTANCE 30
#define MAX_ELEVATION 2900
#define MAX_CONES 17
#define DOWN_DR 1120
#define DROP_MOGO 3000
#define GET_MOGO 940
#define VERTICAL_MOGO 1950

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
	PIDInit(&pidMovement, 0.3, 0, 1.5); // Set P, I, and D constants
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
	PIDInit(&pidMovement, 0.3, 0, 1.5); // Set P, I, and D constants
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
	PIDInit(&pidGyro, 0.25, 0, 1.8); // Set P, I, and D constants

	clearTimer(T1);
	int timer = time1[T1];
	while(!atGyro && timer < time){
		writeDebugStreamLine("Current angle = %f", gyroAngle);
		pidGyroResult = PIDCompute(&pidGyro, targetAngle - gyroAngle);
		rotateBase(18*pidGyroResult);
		if (abs(gyroAngle-targetAngle)<0.1)
			counter++;
		if (counter > 10)
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
		setRollers(15);
	}
}

void moGoControl(void){
	if(vexRT(Btn8R)){
		setMOGOGripper(127);
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
	if(SensorValue[potChainbar] < angle)
	{
		while(SensorValue[potChainbar] < angle)
		{
			setChainbar(127);
			baseControl();
			elevationControl();
		}
	}
	else
	{
		while(SensorValue(potChainbar)>angle)
		{
			setChainbar(-127);
			baseControl();
			elevationControl();
		}
	}
	setChainbar(15);
	baseControl();
	elevationControl();
}

void setPositionCBHappy(int angle){
	if(SensorValue[potChainbar] < angle)
	{
		while(SensorValue[potChainbar] < angle)
		{
			setChainbar(127);
			baseControl();
		}
	}
	else
	{
		while(SensorValue(potChainbar)>angle)
		{
			setChainbar(-127);
			baseControl();
		}
	}
	if(angle == CHAINBAR_DOWN) setChainbar(-15);
	else setChainbar(15);
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
		setPositionCB(CHAINBAR_DOWN);
		setChainbar(0);
		setElevation(10);
	}
}

void setPositionDR(int angle){
	clearTimer(T2);
	if(SensorValue[potElevation] < angle)
	{
		while(SensorValue[potElevation] < angle && time1(T2) < 3000)
		{
			setElevation(127);
		}
	}
	else
	{
		while(SensorValue(potElevation)>angle && time1(T2) < 3000)
		{
			setElevation(-127);
		}
	}
	elevationHold();
}

void setPositionMogo(int angle){
	clearTimer(T2);
	if(SensorValue[potGripper] < angle)
	{
		while(SensorValue[potGripper] < angle && time1(T2) < 1000)
		{
			setMOGOGripper(127);
		}
	}
	else
	{
		while(SensorValue(potGripper)>angle && time1(T2) < 1000)
		{
			setMOGOGripper(-127);
		}
	}
	setMOGOGripper(0);
}

bool sonarElevation(void){
	while(SensorValue(coneSonar) < MAX_CONE_DISTANCE){
		// && coneCounter < MAX_CONES-1
		//coneCounter++;
		setElevation(127);
		if(SensorValue(potElevation) > MAX_ELEVATION - 50){//Worst Case
			elevationHold();
			return true;
		}
	}
	elevationHold();
	return false;
}
void triggerHappy(void){
		setPositionCB(CHAINBAR_DOWN);
		sonarElevation();
		setPositionCBHappy(CHAINBAR_VERTICAL);
		setChainbar(25);
		wait1Msec(400);
		setRollers(-127, 250);
		wait1Msec(100);//Let the Cone Settle
		setPositionCBHappy(CHAINBAR_HORIZONTAL);
		setChainbar(25);
		setPositionDR(DOWN_DR+75);
		wait1Msec(100);
		setChainbar(0);
}

void triggerHappyAuto(void){
		sonarElevation();
		setPositionCBHappy(CHAINBAR_VERTICAL);
		setChainbar(25);
		wait1Msec(400);
		setRollers(-127, 250);
		wait1Msec(100);//Let the Cone Settle
		setPositionCBHappy(CHAINBAR_DOWN);
		setPositionDR(DOWN_DR+350);
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
	clearTimer(T1);
	int timer = T1;
	setChainbar(15);
	if(SensorValue(baseSonar) > distance)
	{
		while(SensorValue(baseSonar) > distance  && timer < time){
			writeDebugStreamLine("%d\n",SensorValue(baseSonar));
			moveBase(127);
			timer = time1[T1];
		}
		moveBase(-10);
	}
	else
	{
		while(SensorValue(baseSonar) < distance  && timer < time){
			writeDebugStreamLine("%d\n",SensorValue(baseSonar));
			moveBase(-127);
			timer = time1[T1];
		}
		moveBase(10);
	}
	wait1Msec(100);
	moveBase(0);
	writeDebugStreamLine("Sonar = %d", SensorValue(baseSonar));
}

// Starts Red side to short cones sides
void auto1()
{
	setOffsetAngle(315);
	// Deploy
	setRollers(50);
	wait1Msec(200);
	setPositionDR(MAX_ELEVATION-900);
	setChainbar(-127);
	wait1Msec(700);
	setChainbar(0);
	//setPositionCBHappy(CHAINBAR_HORIZONTAL);
	// Crayola cone
  moveBaseUntil(45,3000);//era 57
  wait1Msec(200);
	setRollers(-127,1000);
	// First cone
	moveBaseBack(3,1000,1);
	//moveBaseUntil(44,1000);
	setPositionCB(CHAINBAR_VERTICAL);
	setPositionDR(DOWN_DR+400);
	rotateToAngle(375,2000);
	moveBaseWithFactor(31,2000,1);
	setPositionCB(CHAINBAR_DOWN);
	setRollers(127);
	setPositionDR(DOWN_DR);
	wait1Msec(600);
	setRollers(10);
	//Mogo
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(12);
	rotateToAngle(384,700);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(13,2000,1);
	wait1Msec(50);
	setPositionMogo(GET_MOGO);
	// Drop first cone
	setRollers(-127,400);
	wait1Msec(100);
	// Second cone
	rotateToAngle(398,700);
	moveBaseBack(11,1000,1);
	wait1Msec(100);
	rotateToAngle(362,2000);
	wait1Msec(100);
	moveBaseWithFactor(5,1000,1);
	setRollers(127);
	setPositionCBHappy(CHAINBAR_DOWN);
	wait1Msec(500);
	setRollers(10);
	triggerHappyAuto();
	// Third cone
	moveBaseWithFactor(6,1000,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(400);
  setRollers(10);
	triggerHappyAuto();
	// Fourth cone
	rotateToAngle(362,2000);
	moveBaseWithFactor(9,1000,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(400);
  setRollers(10);
	triggerHappyAuto();
	// Fifth cone
	moveBaseWithFactor(6,1000,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(400);
  setRollers(10);
  moveBaseBack(5,500,1);
	// triggerHappyAuto(); With arm up
  sonarElevation();
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(25);
	wait1Msec(400);
	setRollers(-127, 250);
	wait1Msec(100);//Let the Cone Settle
  // End trigger happy with arm up
	moveBaseBack(57,4000,1);
	rotateToAngle(222,2000);
	moveBaseWithFactor(5,500,1);
	rotateToAngle(138,2000);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-30);
	setBase(127);
	wait1Msec(1500);
	setBase(0);
	wait1Msec(100);
	setPositionMogo(VERTICAL_MOGO+700);
	setMOGOGripper(-30);
	setBase(-127);
	wait1Msec(600);
	setBase(0);
}

// Starts Red side to short cones sides
void auto2()
{
	setOffsetAngle(315);
	// Deploy
	setRollers(50);
	wait1Msec(200);
	setPositionDR(MAX_ELEVATION-1000);
	setPositionCBHappy(CHAINBAR_DOWN);
	// Crayola cone
  moveBaseUntil(47,3000);//era 57
  wait1Msec(200);
	setRollers(-127,1000);
	// First cone
	moveBaseBack(3,1000,1);
	//moveBaseUntil(44,1000);
	setPositionCB(CHAINBAR_VERTICAL);
	setPositionDR(DOWN_DR+350);
	rotateToAngle(375,1500);
	moveBaseWithFactor(31,2000,0.75);
	setRollers(127);
	setPositionCB(CHAINBAR_DOWN);
	setPositionDR(DOWN_DR);
	wait1Msec(500);
	setRollers(10);
	//Mogo
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(12);
	rotateToAngle(384,700);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(13,1000,1);
	wait1Msec(50);
	setPositionMogo(GET_MOGO);
	// Drop first cone
	setRollers(-127,400);
	wait1Msec(100);
	// Second cone
	rotateToAngle(398,700);
	moveBaseBack(11,1000,1);
	wait1Msec(100);
	rotateToAngle(362,700);
	wait1Msec(100);
	moveBaseWithFactor(5,500,1);
	setRollers(127);
	setPositionCBHappy(CHAINBAR_DOWN);
	setPositionDR(DOWN_DR);
	wait1Msec(500);
	setRollers(10);
	triggerHappyAuto();
	// Third cone
	moveBaseWithFactor(8,500,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(500);
  setRollers(10);
	triggerHappyAuto();
	// Fourth cone
	rotateToAngle(362,500);
	moveBaseWithFactor(9,500,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(500);
  setRollers(10);
	// triggerHappyAuto(); With arm up
  sonarElevation();
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(25);
	wait1Msec(300);
	setRollers(-127, 250);
	wait1Msec(100);//Let the Cone Settle
  // End trigger happy with arm up
	moveBaseBack(55,2500,1);
	rotateToAngle(222,2000);
	moveBaseWithFactor(5,500,1);
	rotateToAngle(138,2000);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-30);
	setBase(127);
	wait1Msec(1500);
	setBase(0);
	wait1Msec(100);
	setPositionMogo(VERTICAL_MOGO+700);
	setMOGOGripper(-30);
	setBase(-127);
	wait1Msec(600);
	setBase(0);
}

void auto3()
{
	setOffsetAngle(315);
	// Deploy
	setRollers(50);
	wait1Msec(200);
	setPositionDR(MAX_ELEVATION-900);
	setChainbar(-127);
	wait1Msec(700);
	setChainbar(0);
	//setPositionCBHappy(CHAINBAR_HORIZONTAL);
	// Crayola cone
  moveBaseUntil(45,3000);//era 57
  wait1Msec(200);
	setRollers(-127,1000);
	// First cone
	moveBaseBack(3,1000,1);
	//moveBaseUntil(44,1000);
	setPositionCB(CHAINBAR_VERTICAL);
	setPositionDR(DOWN_DR+350);
	rotateToAngle(376,1500);
	moveBaseWithFactor(31,2000,0.75);
	setPositionCB(CHAINBAR_DOWN);
	setRollers(127);
	wait1Msec(500);
	setRollers(10);
	//Mogo
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(12);
	rotateToAngle(384,700);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(13,1000,1);
	wait1Msec(50);
	setPositionMogo(GET_MOGO);
	// Drop first cone
	setRollers(-127,400);
	wait1Msec(100);
	// Second cone
	rotateToAngle(398,700);
	moveBaseBack(11,1000,1);
	wait1Msec(100);
	rotateToAngle(362,700);
	wait1Msec(100);
	moveBaseWithFactor(5,500,1);
	setRollers(127);
	setPositionCBHappy(CHAINBAR_DOWN);
	wait1Msec(500);
	setRollers(10);
	triggerHappyAuto();
	// Third cone
	moveBaseWithFactor(8,500,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(500);
  setRollers(10);
	// triggerHappyAuto(); With arm up, no drop
  sonarElevation();
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(25);
	wait1Msec(100);//Let the Cone Settle
  // End trigger happy with arm up
	//
	moveBaseBack(45,2500,1);
	rotateToAngle(222,1500);
	moveBaseWithFactor(7,500,1);
	rotateToAngle(133,1000);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-15);
	setBase(127);
	wait1Msec(750);
	//setMOGOGripper(127);
	wait1Msec(750);
	setBase(0);
	wait1Msec(100);
	setMOGOGripper(-127);
	setBase(-127);
	wait1Msec(1000);
	setBase(0);
	setPositionMogo(GET_MOGO);
	//Position
	moveBaseWithFactor(10,2000,0.25);
	moveBaseBack(6,500,1);
	rotateToAngle(222,1000);
	moveBaseWithFactor(20,1000,1);
	rotateToAngle(270,1000);
	//Get 2nd MoGo
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(12);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(20,2000,1);
	wait1Msec(50);
	setPositionMogo(GET_MOGO);
	//Drop cone
	setRollers(-127,500);
	//Drop MOGO
	moveBaseBack(50,2500,1);
	rotateToAngle(222,1500);
	moveBaseBack(7,500,1);
	rotateToAngle(133,1000);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-30);
	setBase(127);
	wait1Msec(750);
	setMOGOGripper(127);
	wait1Msec(750);
	setBase(0);
	wait1Msec(100);
	setMOGOGripper(GET_MOGO);
	setBase(-127);
	wait1Msec(1000);
	setBase(0);
}

// Rojo mirando al lado largo de conos/ Defensa
void auto4()
{
	setOffsetAngle(270);
	setRollers(30);
	//Get 2nd MoGo
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(12);
	setPositionMogo(DROP_MOGO);
	moveBaseWithFactor(35,2000,1);
	wait1Msec(50);
	setPositionMogo(GET_MOGO);
	rotateToAngle(270,300);
	//Drop cone
	setRollers(-127,500);

	// Second cone
	moveBaseWithFactor(5,500,1);
	setRollers(127);
	setPositionCBHappy(CHAINBAR_DOWN);
	wait1Msec(500);
	setRollers(10);
	triggerHappyAuto();
	// Third cone
	moveBaseWithFactor(8,500,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(500);
  setRollers(10);
	triggerHappyAuto();
	// Fourth cone
	moveBaseWithFactor(8,500,1);
	setRollers(127);
	setPositionDR(DOWN_DR);
  wait1Msec(500);
  setRollers(10);
  moveBaseBack(5,400,1);
  rotateToAngle(270,500);
	// triggerHappyAuto(); With arm up, no drop
  sonarElevation();
	setPositionCBHappy(CHAINBAR_VERTICAL);
	setChainbar(25);
	wait1Msec(100);//Let the Cone Settle
  // End trigger happy with arm up

	//Drop MOGO
	moveBaseBack(45,2500,1);
	rotateToAngle(222,1500);
	moveBaseBack(25,500,1);
	rotateToAngle(133,1000);
	setPositionMogo(VERTICAL_MOGO);
	setMOGOGripper(-30);
	setBase(127);
	wait1Msec(750);
	wait1Msec(750);
	setBase(0);
	wait1Msec(100);
	setPositionMogo(VERTICAL_MOGO+700);
	setMOGOGripper(-30);
	setBase(-127);
	wait1Msec(600);
	setBase(0);

	//Crayola
	setPositionMogo(GET_MOGO);
	rotateToAngle(313,1000);
	setPositionDR(MAX_ELEVATION-900);
	setChainbar(-127);
	wait1Msec(700);
	setChainbar(0);
	//setPositionCBHappy(CHAINBAR_HORIZONTAL);
	// Crayola cone
  moveBaseUntil(45,3000);//era 57
  wait1Msec(200);
	setRollers(-127,1000);
}

//*********************************************************************************************
//			LCD Functions
//*********************************************************************************************

string mainBattery, secondBattery;

//Wait for Press--------------------------------------------------
void waitForPress()
{
	while(nLCDButtons == 0 && bIfiRobotDisabled)
	{
		wait1Msec(5);
	}
}
//----------------------------------------------------------------

//Wait for Release------------------------------------------------
void waitForRelease()
{
	while(nLCDButtons != 0)
	{
		wait1Msec(5);
	}
}
//----------------------------------------------------------------

void preautonomous_selector()
{
	const short leftButton = 1;
	const short centerButton = 2;
	const short rightButton = 4;

	//------------- Beginning of User Interface Code ---------------
	//Clear LCD
	clearLCDLine(0);
	clearLCDLine(1);

	//Loop while robot is disabled
	while(bIfiRobotDisabled)
	{
		//Display Battery Levels On Top Row
		displayLCDString(0, 0, "    B1: ");
		sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0);
		displayNextLCDString(mainBattery);
		displayNextLCDString("          ");
		//Switch case that allows the user to choose from 8 different options
		switch(count){
		case 0:
			displayLCDCenteredString(1, "< Auto 1R >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count = 6;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count++;
			}
			break;
		case 1:
			displayLCDCenteredString(1, "< Auto 1B >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count--;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count++;
			}
			break;
		case 2:
			displayLCDCenteredString(1, "< Auto 2R >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count--;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count++;
			}
			break;
		case 3:
			displayLCDCenteredString(1, "< Auto 2B >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count--;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count++;
			}
			break;
		case 4:
			displayLCDCenteredString(1, "< Auto 3R >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count--;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count++;
			}
			break;
		case 5:
			displayLCDCenteredString(1, "< Auto 3B >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count--;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count++;
			}
			break;
		case 6:
			displayLCDCenteredString(1, "< No Auto >");
			waitForPress();
			if(nLCDButtons == leftButton)
			{
				waitForRelease();
				count--;
			}
			else if(nLCDButtons == rightButton)
			{
				waitForRelease();
				count = 0;
			}
			break;
		default:
			count = 0;
			break;
		}
	}
}

void autonomous_selector(int auto_selection)
{
	//------------- Beginning of Robot Movement Code ---------------
	//Clear LCD
	clearLCDLine(0);
	clearLCDLine(1);
	//runs the user choice
	if(auto_selection==0)
	{
		displayLCDCenteredString(0, "Auto 1");
		displayLCDCenteredString(1, "is running!");
		wait1Msec(100);
		auto1();
	}
	else if(auto_selection == 1)
	{
		//If count = 1, run the code correspoinding with choice 2
		displayLCDCenteredString(0, "Auto 2");
		displayLCDCenteredString(1, "is running!");
		wait1Msec(100);
		//auto1b();
	}
	else if(auto_selection == 2)
	{
		//If count = 2, run the code correspoinding with choice 3
		displayLCDCenteredString(0, "Auto 2_M");
		displayLCDCenteredString(1, "is running!");
		wait1Msec(100);
		auto2();
	}
	else if(auto_selection == 3)
	{
		//If count = 3, run the code correspoinding with choice 4
		displayLCDCenteredString(0, "Auto 3");
		displayLCDCenteredString(1, "is running!");
		wait1Msec(100);
		//auto2b();
	}
	else if(auto_selection == 4)
	{
		displayLCDCenteredString(0, "Auto 4");
		displayLCDCenteredString(1, "is running!");
		wait1Msec(100);
		auto3();
	}
	else if(auto_selection == 5)
	{
		displayLCDCenteredString(0, "Prog Skills");
		wait1Msec(100);
		//auto3b();
	}
	else if(auto_selection == 6)
	{
		displayLCDCenteredString(0, "No auto");
		wait1Msec(100);
		//NoAuto
	}
	else
	{
		displayLCDCenteredString(0, "No valid choice");
		displayLCDCenteredString(1, "was made!");
	}
	//------------- End of Robot Movement Code -----------------------
}

void display_battery_levels()
{
	//Display Battery Levels
	displayLCDString(0, 0, "B1:");
	sprintf(mainBattery, "%1.2f%c", nImmediateBatteryLevel/1000.0);
	displayNextLCDString(mainBattery);
	//displayNextLCDString("  B2:");
	//sprintf(secondBattery, "%1.2f%c", SensorValue[Expander]/280.0);
	//displayNextLCDString(secondBattery);
	displayLCDCenteredString(1, "Kentuckyta");
}

void genericControl(void){
	display_battery_levels();
	baseControl();
	elevationControl();
	rollerControl();
	chainbarControl();
	moGoControl();
	if(vexRT(Btn5U)) triggerHappy();
}
