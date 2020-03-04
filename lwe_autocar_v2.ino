/* ************************************************
  LWE Science Club Auto Car

  Description
  
  Details, Details, Details, Details, Details, Details, Details, 
  Details, Details, Details, Details, Details, Details, Details, 
  Details, Details, vDetails, Details, Details, Details, Details, 
  Details, Details, Details, Details, Details, Details, Details, 

  https://github.com/jumblobinley/LWSC_Auto_Rob.git
  (See git examples below)

  
  --Created 2020 Dec 15
  --by Joseph Hauser
  --Modified 2020 jan 26
  --by Joseph Hauser
  --Modified 2020 feb 2
  --by Joseph Hauser
  --Modified 2020 feb 10
  --by Joseph Hauser
  --Modified 2020 feb 22
  --by Joseph Hauser
************************************************** */

// setting up global values for sensors
  
  #define MOTOR1 9
  #define MOTOR2 10
  #define CALIBRATE 7
  
  #define MAXCONTROLROWS 13
  #define MAXSENSVALS 5
  #define LEFTDOT 5
  #define RIGHTDOT 1
  #define LEFTDIR 1
  #define RIGHTDIR 2
  #define STOPPED 0
  
  int mot1speed = 0;
  int mot2speed = 0;
  int prevDir = 0;
  int motControlMatrix[MAXCONTROLROWS][7]{
           {0, 0, 0, 0, 0, 0, 0},
           {1, 0, 0, 0, 0, 100, 50},
           {0, 1, 0, 0, 0, 125, 80},
           {0, 0, 1, 0, 0, 150, 150},
           {0, 0, 0, 1, 0, 80, 125},
           {0, 0, 0, 0, 1, 50, 100},
           {1, 1, 0, 0, 0, 115, 65},
           {0, 1, 1, 0, 0, 135, 80},
           {0, 0, 1, 1, 0, 80, 135},
           {0, 0, 0, 1, 1, 65, 115},
           {1, 1, 1, 0, 0, 125, 80},
           {0, 1, 1, 1, 0, 140, 140},
           {0, 0, 1, 1, 1, 80, 125}
            }; 
            
  bool sensVals[MAXSENSVALS] = {LOW, LOW, LOW, LOW, LOW};
  int sensAn = 0;
  
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(CALIBRATE, OUTPUT);

//Calibrate Line Sensor upon startup

  digitalWrite(CALIBRATE, LOW);
  delay(3000);
  digitalWrite(CALIBRATE, HIGH);
  delay(5000);
  digitalWrite(CALIBRATE, LOW);
  delay(2000);
  digitalWrite(CALIBRATE, HIGH);
   
  //pinMode(A5, OUTPUT); but it's analog
}

// the loop function runs over and over again forever
//---------- Main Program --------------------------
//--------------------------------------------------                         
void loop() {
  
  //flashing pin 2
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(50);                       // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(50);                       // wait for a second

//Step 1. Read the line sensor  
  sensorRead();
  //delay(10);

//Step 2. process the incoming data, and determine the motor commands   
   dataFilter();

//Step 3. send out the motor commands
   motorControl(MOTOR1, mot1speed);
   motorControl(MOTOR2, mot2speed);
 
 

    //ad  a new line at the end of the output data
    Serial.println(" ");

}


// recieves information from the Cytron Maker Line
void sensorRead(void) {
  // initialize digital pin LED_BUILTIN as an output.
 sensVals[0] = digitalRead(A0);
 sensVals[1] = digitalRead(A1); 
 sensVals[2] = digitalRead(A2);
 sensVals[3] = digitalRead(A3);
 sensVals[4] = digitalRead(A4);
 sensAn = analogRead(A5);

 Serial.print(sensVals[0]);
 Serial.print(", ");
 Serial.print(sensVals[1]);
 Serial.print(", ");
 Serial.print(sensVals[2]);
 Serial.print(", ");
 Serial.print(sensVals[3]);
 Serial.print(", ");
 Serial.print(sensVals[4]);
// Serial.print(", ");
 //Serial.print(sensAn);
 Serial.print(" -- ");
}

// filters the data recieved so the motors can understand what actions to take
void dataFilter(void) {
    bool rowMatch = true;
    int currentRow = 0;
    int currentCol = 0;
    for (int row = 0; row < MAXCONTROLROWS; row++) {
      
      //compare one row of control matrix with sensor array
      //if it compares, control the motor
      for(int col = 0; col < MAXSENSVALS; col++) {
        currentRow = row;
        currentCol = col;
        if (sensVals[col] != motControlMatrix[row][col]){
          rowMatch = false;
          break;
        }
        else {
          rowMatch = true;  
        }
      } //done with inner loop 

      
      //Serial.print(currentCol);
      
      if (rowMatch == true) {
        break;
        
      } 

    }//end of outer loop
       
     // set motor speed based on the current row of the sensor matrix
     mot1speed = motControlMatrix[currentRow][5];
     mot2speed = motControlMatrix[currentRow][6];

     // Assign left and right previous direction only when the left or right sensors are lit
     if (currentRow == LEFTDOT) {prevDir = RIGHTDIR;}
     if (currentRow == RIGHTDOT){prevDir = LEFTDIR;}
     
     // if current row indicates zero on the sensors, tell the car to move in the opposite direction from previous
     if (currentRow == STOPPED) {
        if (prevDir == LEFTDIR) {
          mot1speed = 100;
          mot2speed = 50;
        } 
        else if (prevDir == RIGHTDIR){
          
          mot1speed = 50;
          mot2speed = 100;
        }
        else {
          Serial.print("Error:  set motor on track");
        }
     }
    
    // error condition - no rows matched the sensor values
    if(rowMatch == false) {
      mot1speed = 0;
      mot2speed = 0;
      Serial.println("uh oh, something went wrong >:(");
    }
}

// commmands sent to the motor controller
bool motorControl(int motorNum, int motorPow) {
  
  Serial.print(" -----> ");
  Serial.print(motorNum);
  Serial.print(", ");
  Serial.print(motorPow);
  Serial.print(" <-- ");
  Serial.print(prevDir);


  if (motorNum == MOTOR1) {
    analogWrite(MOTOR1, motorPow); 
 }
  else if (motorNum == MOTOR2) {
    analogWrite(MOTOR2, motorPow);
    
 }
  else {
    Serial.println("Error: motor nonexiststant");
    return false;
 }
return true;
}

/* GitHub Examples
 *  
 *  New Git Repo - do this one time
 *  echo "# LWSC_Auto_Rob_V1" >> README.md
 *  git init
 *  git add README.md
 *  git commit -m "first commit"
 *  git remote add origin git@github.com:jumblobinley/LWSC_Auto_Rob_V1.git
 *  git push -u origin master
 *  git add lwe_autocar_v1.ino
 *  git commit -m "Added core arduino code file v1"
 *  git push -u origin master
 *  
 *  
 *  ---Write code
 *  ---Add file to repo:
 *  git add <filename>
 *  
 *   --Attach a remote origin to your code repo, Do this one time
 *  git remote add origin git@github.com:jumblobinley/LWSC_Auto_Rob_V1.git
 *  (Note: ssh keys are set up for this computer.  Other students will have to set up thier keys)
 *  
 *  ---When done writing code, commit your code and push to github in the cloud
 *  
 *  cd ~/Documents/ArduinoSketchBook/lwe_auto_car/lwe_autocar_v1
 *  git add lwe_autocar_v1.ino
 *  git commit -m "comment"
 *  
 *  git push -u origin master
 *  
 */
