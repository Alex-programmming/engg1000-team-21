// Soccer Droids Team 21 
// z5248147 
// Author: HaohengDuan
// This program is used to control a robot to kick the
// ball, defend the goal and play as a part of a team.
// Date: 11/11/2019


//Add the Pixy Library to the program
#include <Pixy2.h> 
Pixy2 pixy;



#define LEFT_SIDE 148
#define RIGHT_SIDE 168


//Setting the pin of motors
int first_engine = 10;
int first_engine_01 = 8;
int first_engine_02 = 9;
int second_engine = 5;
int second_engine_01 = 7;
int second_engine_02 = 6;
//Setting the pin of pistons
int charge_piston = 2;
int fire_piston = 3;
//Setting the pin of the switch
int diffence_or_offence = 4;
//Showing whether the robot is moving or not
bool check_movement = false; 
long time_after_fired = 0;




//Setup code only run once
void setup() {
    Serial.begin(9600);
    
    //Show what the pin is connected with
    Serial.println("Pin 2 and 3 are for piston");
    Serial.println("Pin 4 is for the switch");
    Serial.println("Pin 5 to 10 are for motors");
    Serial.println("Pin A4 and A5 are for Pixy");
    
    //Setting pin mode of motors
    //Used to change strength of engines
    pinMode(first_engine, OUTPUT); 
    pinMode(second_engine, OUTPUT); 
    //Used to change directions of the wheels 
    pinMode(first_engine_01, OUTPUT);
    pinMode(first_engine_02, OUTPUT);
    pinMode(second_engine_01, OUTPUT);
    pinMode(second_engine_02, OUTPUT);
    
    //Setting the initialisation of motors
    analogWrite(first_engine, 0);
    analogWrite(second_engine, 0);
    digitalWrite(second_engine_01, LOW);
    digitalWrite(second_engine_02, LOW);
    digitalWrite(first_engine_01, LOW);
    digitalWrite(first_engine_02, LOW);

    //Setting the initialisation of Pixy camera
    Serial.println("Camera is initialising...");
    pixy.init();
    Serial.println("Pixy is Initialised.");

    //Setting pin mode of piston
    pinMode(charge_piston, OUTPUT);
    pinMode(fire_piston, OUTPUT);
    
    //Setting the initialisation of piston
    digitalWrite(charge_piston, HIGH);
    digitalWrite(fire_piston, LOW);

    //Setting pin mode of switch
    pinMode(diffence_or_offence, INPUT); 

}

//Main Loop will run over and over again
void loop() {  
    //Counting the time after fired
    time_after_fired++; 
    //Start to charge the piston
    digitalWrite(charge_piston, HIGH);    
    //Check the present state of switch  
    int present_mode = digitalRead(diffence_or_offence);
    
    //Take different action in different mode
    //It will defend if low and offend if high
    if (present_mode = LOW) {
        defence_mode();
    } else if (present_mode = HIGH) {
        offence_mode();
    }
}



//The droid will offend in this mode
void offence_mode() {
    detect_ball();  
    //Take different action in the condition whether the ball is found
    if (detect_ball() == false) {
        spin_clockwise();
        //Judge the state of movement
        check_movement = false; 
    } else if (detect_ball() != false) {
        //The position of ball on x-axis
        int position = ball_position();
        
        //The left and right side of the piston
        //Make sure the ball is at the center 
        if (position < LEFT_SIDE) {
            if (check_movement == false) {
                turn_anticlockwise();
            } else if (check_movement == true) {
                turn_left();
            }
        } else if (position > RIGHT_SIDE) {
            
            if (check_movement == false) {
                turn_clockwise();
            } else if (check_movement == true) {
                turn_right();
            }
        } else if (position >= LEFT_SIDE && position <= RIGHT_SIDE) { 
            //if the ball is adjusted to the right place then move forwards
            wheel_forward();
            check_movement = true;
            //Count the distance of the ball
            int distance = ball_distance();
            
            if (distance >= 50 && time_after_fired > 40) { 
                //When the ball approaches the droid then launch the piston part
                piston_launch();        
            }
        }
    }
}



//The droid will defend in this mode
void defence_mode() {
    detect_ball(); 
    if (detect_ball() == false) {
        spin_clockwise;
    } else if (detect_ball() == true) {
        //The position of ball on x-axis
        int position = ball_position(); 
        
        //The left and right side of the piston 
        //Make sure the ball is at the center 
        if (position < LEFT_SIDE) {
            turn_anticlockwise();
        } else if (position > RIGHT_SIDE) {
            turn_clockwise();
        } else if (position >= LEFT_SIDE && position <= RIGHT_SIDE) { 
            //Count the distance of the ball
            int distance = ball_distance();
            
            if (distance <= 300) { 
                full_power_forwards();
                delay(300);
                //The droid will reverse
                full_power_reverse();
                delay(300);
                turn_off_wheels();
                delay(1000);
            }
        }
    }
}


//This function enables the droid to find the ball
bool detect_ball() {
    //A flag used to judge
    bool flag = false;
    //Scan many blocks
    pixy.ccc.getBlocks();
    //Go through every blocks and determin whether they are the ball
    int i = 0;
    while (i < pixy.ccc.numBlocks) {
        if (pixy.ccc.blocks[i].m_signature == 1) {
            flag = true;
        }
        i++;
    }

    return flag;
}


//Using the width of block to judge ball distance 
int ball_distance() {
    int distance = 2;

    //Scan many blocks
    pixy.ccc.getBlocks();

    //Determine the width of the ball
    int i=0;
    while (i < pixy.ccc.numBlocks) {
        distance = pixy.ccc.blocks[i].m_width;
        i++;
    }  
    return distance;
}


//Determine the ball location on the x-axis
int ball_position() {
    int position = 0;

    //Scan many blocks
    pixy.ccc.getBlocks();

    int i = 0;
    while (i < pixy.ccc.numBlocks) {
        position = pixy.ccc.blocks[i].m_x; 
        i++;
    }
    
    return position;
}



//The droid will move forward at a medium speed
void wheel_forward() { 
    analogWrite(first_engine, 180);
    analogWrite(second_engine, 180);    
    //Both wheels will move forward
    digitalWrite(first_engine_01, HIGH);
    digitalWrite(first_engine_02, LOW);
    digitalWrite(second_engine_01, HIGH);
    digitalWrite(second_engine_02, LOW);
}


//The motor will turn off
void turn_off_wheels() {
    
    analogWrite(first_engine, 0);
    analogWrite(second_engine, 0);
    
    digitalWrite(first_engine_01, LOW);
    digitalWrite(first_engine_02, LOW);
    digitalWrite(second_engine_01, LOW);
    digitalWrite(second_engine_02, LOW);
}

//The droid will move forward at full speed
void full_power_forwards() {

    analogWrite(first_engine, 255);
    analogWrite(second_engine, 255);

    digitalWrite(first_engine_01, HIGH);
    digitalWrite(first_engine_02, LOW);
    digitalWrite(second_engine_01, HIGH);
    digitalWrite(second_engine_02, LOW);
}


//The droid will move backward at full speed
void full_power_reverse() {

    analogWrite(first_engine, 255);
    analogWrite(second_engine, 255);

    digitalWrite(first_engine_01, LOW);
    digitalWrite(first_engine_02, HIGH);
    digitalWrite(second_engine_01, LOW);
    digitalWrite(second_engine_02, HIGH);

}

//The droid will turn right
void turn_right() {
    
    //The power in the right motor will be reduced
    analogWrite(first_engine, 200);
    analogWrite(second_engine, 140);
    
    //Both wheels will move forward
    digitalWrite(first_engine_01, HIGH);
    digitalWrite(first_engine_02, LOW);
    digitalWrite(second_engine_01, HIGH);
    digitalWrite(second_engine_02, LOW);  
}

//The droid will turn left
void turn_left() {
    
    //The power in the left motor will be reduced
    analogWrite(first_engine, 140);
    analogWrite(second_engine, 200);
    
    //Both wheels will move forward
    digitalWrite(first_engine_01, HIGH);
    digitalWrite(first_engine_02, LOW);
    digitalWrite(second_engine_01, HIGH);
    digitalWrite(second_engine_02, LOW);
}


//The droid will spin clockwise
void spin_clockwise() {
    //Move at a low speed
    analogWrite(first_engine, 60);
    analogWrite(second_engine, 60);
    
    //Left wheel will move forward
    digitalWrite(first_engine_01, HIGH);
    digitalWrite(first_engine_02, LOW);
    //Right wheel will move backward
    digitalWrite(second_engine_01, LOW);
    digitalWrite(second_engine_02, HIGH);
}


//The droid will turn anticlockwise
void turn_anticlockwise() {
    
    //Make sure both wheels have same power supply
    analogWrite(first_engine, 100);
    analogWrite(second_engine, 100);
    
    //Backward
    digitalWrite(first_engine_01, LOW);
    digitalWrite(first_engine_02, HIGH);
    //Forward
    digitalWrite(second_engine_01, HIGH);
    digitalWrite(second_engine_02, LOW);
}

//The droid will turn clockwise
void turn_clockwise() {
    
    //Make sure both wheels have same power supply
    analogWrite(first_engine, 100);
    analogWrite(second_engine, 100);
    
    //Forward
    digitalWrite(first_engine_01, HIGH);
    digitalWrite(first_engine_02, LOW);
    //Backward
    digitalWrite(second_engine_01, LOW);
    digitalWrite(second_engine_02, HIGH);
}

//The piston will be launched
void piston_launch() {
    
    //Stop charging the capacitor
    digitalWrite(charge_piston, LOW);
    delay(50);
    //Activate the piston to shoot the ball
    digitalWrite(fire_piston, HIGH);
    time_after_fired = 0;
    delay(50);
    //Start charging and stop firing
    digitalWrite(fire_piston, LOW);
    digitalWrite(charge_piston, HIGH);
}
