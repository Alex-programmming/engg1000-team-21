//Soccer Robots Team 21 Code Review
//z5255884 Nathan Trinh
//--------------------------------------------------------------

//Pins
//==============================================================

//L298N Motor Controller
//Motor one
int enA = 5;
int in1 = 6;
int in2 = 7;
//Motor two
int enB = 10;
int in3 = 8;
int in4 = 9;

//Piston
int pistonCharge = 13;
int pistonFire = 12;

//Pixy
#include <Pixy2.h> //Pixy2 Arduino Library
Pixy2 pixy;

//Defence/Offence switch
int defoff = 11;

//Pixy Signatures
int ballSig = 1; //signature number of the ball
int goalSig = 2; //signature number of the goal

//Globals
//==============================================================
bool moving = false; //indicator for if the robot is currently moving
long ticksSinceFired = 0; //time since fired
bool ballCaught = false;

//Initialisation
//==============================================================
void setup() {
  Serial.begin(115200);
  //Print pin locations into serial monitor
  Serial.println("Motor controller | 6-10");
  Serial.println("Compass          | A4,A5");
  Serial.println("Piston           | 12,13");
  Serial.println("Defense/Offence  | 11");

  //Initialise pin settings
  //motor strength
  pinMode(enA, OUTPUT); //integer [0,255]
  pinMode(enB, OUTPUT); //integer [0,255]
  //motor directionals
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //Set startup defaults
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
  //Verify initialisation of Pixy
  Serial.println("Initialising Pixy...");
  pixy.init();
  Serial.println("Camera Initialised.");

  //Initialise piston activator pins
  pinMode(pistonCharge, OUTPUT);
  pinMode(pistonFire, OUTPUT);
  //Set default states
  digitalWrite(pistonCharge, HIGH);
  digitalWrite(pistonFire, LOW);

  //Initialise defence/offense signal pin
  pinMode(defoff, INPUT); //HIGH = offence LOW = defence

}
//Is the ball on screen?
//=============================================================
bool ballFound() {
  //Locals
  bool result = false;
  
  //Scan for signature blocks
  pixy.ccc.getBlocks();
  
  //Enumerate any blocks
  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    
    //Is it the ball
    if (pixy.ccc.blocks[i].m_signature == ballSig) {
      result = true;
      
      }
    }
  return result;
  
}

//Location of goal posts
//=============================================================
int goalBearing() {
    //Locals
  int bearing = 0;

  //Get blocks
  pixy.ccc.getBlocks();

  //Determine where the ball in on the x-axis
  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    if (pixy.ccc.blocks[i].m_signature == goalSig) {
      bearing = pixy.ccc.blocks[i].m_x; 
    }
  }
  return bearing;
}

//Centre goal
//=============================================================
int goalbearing() {
    //Locals
  int bearing = 0;

  //Get blocks
  pixy.ccc.getBlocks();

  //Determine where the ball in on the x-axis
  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    if (pixy.ccc.blocks[i].m_signature == goalSig) {
      bearing = pixy.ccc.blocks[i].m_x; 
    }
  }
  return bearing;
}

//Ball location by bearing [0,316]
//=============================================================
int ballBearing() {
  //Locals
  int bearing = 0;

  //Get blocks
  pixy.ccc.getBlocks();

  //Determine where the ball in on the x-axis
  for (int i = 0; i < pixy.ccc.numBlocks; i++) {
    if (pixy.ccc.blocks[i].m_signature == ballSig) {
      bearing = pixy.ccc.blocks[i].m_x; 
    }
  }
  return bearing;
  
}

//Ball distance judged by width of detected block [0,316]
//=============================================================
int ballDistance() {
  //Locals
  int distance = 1;

  //Get blocks
  pixy.ccc.getBlocks();

  //Determine how wide block is
  for (int i=0; i < pixy.ccc.numBlocks; i++) {
    if (pixy.ccc.blocks[i].m_signature == ballSig) {
      distance = pixy.ccc.blocks[i].m_width;
    }
    
  }
  
  return distance;
  
}

//Piston activate
//=============================================================
void pistonActivate() {
  //Cut off charging
  digitalWrite(pistonCharge, LOW);
  delay(50);
  
  //Fire
  digitalWrite(pistonFire, HIGH);
  ticksSinceFired = 0;
  
  //Wait 0.1 sec
  delay(50);
  
  //Reset signal
  digitalWrite(pistonFire, LOW);
  digitalWrite(pistonCharge, HIGH);
  
}


//=============================================================
//WHEEL POWER PROFILES
//=============================================================


//Spin clockwise
//=============================================================
void spinAroundCW() {
  //Set slower speed for both motors so ball isn't blurry
  analogWrite(enA, 55);
  analogWrite(enB, 55);
  
  //Set forward for motor A
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  //Set backward for motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
}

//Spin anticlockwise
//=============================================================
void spinAroundACW() {
  //Set slower speed for both motors so ball isn't blurry
  analogWrite(enA, 55);
  analogWrite(enB, 55);
  
  //Set forward for motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  //Set backward for motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
}

//Turn wheels off
//=============================================================
void wheelsOFF() {
  //Set motor speeds to zero
  analogWrite(enA, 0);
  analogWrite(enB, 0);
  
  //Set all outputs to LOW
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
}

//Move forward
//=============================================================
void wheelsDEFAULT() {
  //Set motor speeds to medium speed
  analogWrite(enA, 100);
  analogWrite(enB, 85);
  
  //Set both wheels to forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
}

void wheelsFORWARD() {
  //Set motor speeds to medium speed
  analogWrite(enA, 85);
  analogWrite(enB, 70);
  
  //Set both wheels to forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

//Spin around on one wheel
void spinOneWheel() {
  //Ensure power states are equal in both wheels
  analogWrite(enA, 0);
  analogWrite(enB, 55);
  //Reverse power in right wheel
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  //Forward power in left wheel
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

//Move forward slowly
//=============================================================
void wheelsSlow() {
  analogWrite(enA, 40);
  analogWrite(enB, 40);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

}

//Turn right slowly
void turnSlowR() {
  //Reduce power in right wheel
  analogWrite(enA, 50);
  analogWrite(enB, 60);
  //Make sure both wheels are forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  
  
}

void turnSlowL() {
    //Reduce power in right wheel
  analogWrite(enA, 60);
  analogWrite(enB, 50);
  //Make sure both wheels are forward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);  
}

//Full power forward
//=============================================================
void wheelsPOWER() {
  //Set motor speeds to maximum speed
  analogWrite(enA, 250);
  analogWrite(enB, 225);
  //Set both wheels to forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
}


//Full power reverse
//=============================================================
void wheelsREVERSE() {
  //Set motor speeds to maximum speed
  analogWrite(enA, 100);
  analogWrite(enB, 90);
  //Set both wheels to backward
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
}


//Inch left
//=============================================================
void inchRIGHT() {
  //Reduce power in left wheel
  analogWrite(enA, 50);
  analogWrite(enB, 85);
  //Make sure both wheels are forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
}


//Inch right
//=============================================================
void inchLEFT() {
  //Reduce power in right wheel
  analogWrite(enA, 85);
  analogWrite(enB, 50);
  //Make sure both wheels are forward
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  
  
}

//Turn CW **needs engine power tuning**
//=============================================================
void turnCW() {
  //Ensure power states are equal in both wheels
  analogWrite(enA, 60);
  analogWrite(enB, 60);
  //Reverse power in right wheel
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  //Forward power in left wheel
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
}

//Turn ACW **needs engine power tuning**
//=============================================================
void turnACW() {
  //Ensure power states are equal in both wheels
  analogWrite(enA, 60);
  analogWrite(enB, 60);
  //Reverse power in left wheel
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  //Forward power in right wheel
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
}

//Wheel correction
//=============================================================
void wheelsCORRECT() {
  analogWrite(enA, 70);
  analogWrite(enB, 30);


  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  
}


//Main Loop
//=============================================================
void loop() {
  //Begin charging capacitor
  ticksSinceFired++; //start counting how long its been since firing
  digitalWrite(pistonCharge, HIGH);
  if (ticksSinceFired > 7500) {
    digitalWrite(pistonCharge, LOW);
    
  }
  
  //Check if switch is set to defence or offence  
  int currentMode = digitalRead(defoff);
  
  //Defence mode
  if (currentMode == 0){
  
    //Check if ball is found
    if (ballFound() == false) { //if ball is not found
      //Run find ball routine
      wheelsOFF();
      
    }
    else if (ballFound() == true) { //if ball is found

      //Find out where the ball is
      int bearing = ballBearing();
      
      //Keep ball in centre of camera
      if (bearing < 148) {
        turnCW();
        
      }
      else if (bearing > 168) {
        turnACW();
        
      }
      //If centred wait until ball is within 300mm
      else if (bearing >= 148 && bearing <= 168) {
        wheelsOFF();
        int distance = ballDistance();
        Serial.println(distance);
        if (distance >= 40) { //**NEEDS TO BE TUNED TO 300mm
          
          //Charge at the ball when in range aand return to goal using calibrated algorithm
          wheelsDEFAULT();
          delay(70);
          wheelsPOWER();
          delay(600);
          wheelsREVERSE();
          delay(40);
          wheelsOFF();
          delay(500);
          wheelsREVERSE();
          delay(1200);
          wheelsOFF();
          
          //Wait 1 second and then resume ball search
          
        } 
      } 
    }
  }
  //Offence mode
  else if (currentMode == 1) {
     
    //Check if ball has been detected
    
    if (ballFound() == false) { //if ball is not found
      //Run found ball routine
      spinAroundCW();
      moving = false; //tell system it isn't currently moving
      
    }
    else { //if ball is found
      Serial.println("found");
      
      //Centre ball with error of +-10 pixels
      int bearing = ballBearing();
      Serial.println(bearing);
      if (bearing < 138 && ballCaught == false) {
          Serial.println("left");
          inchRIGHT(); //makes slight turn instead of stopping and turning if moving
          
        
      }
      else if (bearing > 178 && ballCaught == false) {
          Serial.println("right");
          inchLEFT(); //makes slight turn instead of stopping and turning if moving
          
        
      }
      
      //If centred move forward
      else if (bearing >= 138 && bearing <= 178 && ballCaught == false) {
        Serial.println("centre");
        wheelsFORWARD(); //move toward ball
        moving = true; //tell system it is currently moving

      }
      int distance = ballDistance(); //check distance of ball
      
      //If ball is close enough
      if (distance > 245) {
        wheelsOFF();
        Serial.println("caught");
        
        ballCaught = true; //tell system that ball is caught
        int bearingGoal = goalbearing(); //begin checking for location of goal

        //If ball is centred activate piston
        if (bearingGoal >= 135 && bearingGoal <= 181) { //**EXACT PARAMETER NEEDS TO BE TUNED**
          Serial.println("goalfound");
          Serial.println(bearingGoal);
          if (ticksSinceFired > 50) {
            Serial.println("fire");
            pistonActivate();
            ballCaught = false;
          }
        }
            
          //If ball is not centred run centring routine
        else if (bearingGoal > 181) {
          turnSlowL();
              
          }
        else if (bearingGoal < 135) {
          turnSlowR();
             
        }
        else {
          turnSlowR();
        }
      }
      else {
        ballCaught = false;
      
      }
    }
  }
}
