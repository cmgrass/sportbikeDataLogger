//------------------------------------
// CG_SBK_DataLogger_RPM_Component
// Christopher Grass - 01/05/2014
// Calibrated for 2008 Kawasaki ZX6R
//------------------------------------

// 01/05/2014: Merged with GEAR component

/*
 * RS: 8
 * EN: 9
 * D4: 4
 * D5: 5
 * D6: 6
 * D7: 7
*/

// Include Liquid Crystal Display Library:
#include <LiquidCrystal.h>

// Define Variables:
byte Column01[8] = {B10000,B10000,B10000,B10000,B10000,B10000,B10000,B10000};
byte Column02[8] = {B11000,B11000,B11000,B11000,B11000,B11000,B11000,B11000};
byte Column03[8] = {B11100,B11100,B11100,B11100,B11100,B11100,B11100,B11100};
byte Column04[8] = {B11110,B11110,B11110,B11110,B11110,B11110,B11110,B11110};
byte Column05[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};
int redlineLED = 3;
int yellowlineLED = 2;
int Backlight = 10;
int RPMFreq = 11;
int NeutralInput = 19;
int warningLEDstate = LOW;
long previousMillis = 0;
unsigned long WaveLength;
float FinalRatio1 = 15.13;      // User nonvoatile input, temporary hardcode for testing
float FinalRatio2 = 12.26;      // User nonvoatile input, temporary hardcode for testing
float FinalRatio3 = 10.31;      // User nonvoatile input, temporary hardcode for testing
float FinalRatio4 = 8.92;      // User nonvoatile input, temporary hardcode for testing
float FinalRatio5 = 7.92;      // User nonvoatile input, temporary hardcode for testing
float FinalRatio6 = 7.25;      // User nonvoatile input, temporary hardcode for testing
float Speed = 132.35;      // Input from GPS, temporary hardcode for testing
float TireCircumference = 77.9;      // User nonvoatile input, temporary hardcode for testing

// Initialize Library
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


  void setup () 
    {
      // Setup Serial:
      Serial.begin(9600);
      Serial.println("Initialize Void Setup () Routine");                               //Debug
    
      // Define Misc. I/O Assignment:
      pinMode(RPMFreq, INPUT);
      pinMode(NeutralInput, INPUT);
      pinMode(redlineLED, OUTPUT);
        digitalWrite(redlineLED, LOW);
      pinMode(yellowlineLED, OUTPUT);
        digitalWrite(yellowlineLED, LOW);
        
      // Configure LCD display:
      pinMode(Backlight, OUTPUT);      // Define "Backlight" Pin as an Output
        digitalWrite(Backlight, HIGH);      // Turn on Backlight  
      lcd.begin(16,2);      // Begin LCD Interface
      lcd.print(" CG SBK-DataSys ");      // Force Print Line on Startup

      // Create Custom Characters:
      lcd.createChar(0, Column01);
      lcd.createChar(1, Column02);
      lcd.createChar(2, Column03);
      lcd.createChar(3, Column04);
      lcd.createChar(4, Column05);
     
      delay(500);
     
      // Configure Progress Bar:
      lcd.setCursor(0,1);                  // Set Cursor to first horizontal position, and bottom vertical position
      lcd.print("                ");             // Clear Line
     
      // Display Progress Bar:
      for (int a1 = 0; a1<16; a1++)        // Initialize "For Loop" to count 16 character writes
        {
           for (int b1 = 0; b1<5; b1++)    // Initialize "For Loop" to count, to write each of the five unique characters
          {
           lcd.setCursor(a1,1);            // Move to position value stores in "a1"
           lcd.write(b1);                  // Write custom character associated with value of "b1"
           delay(12);                      // wait for 12ms
          }
        }
     
      delay(1750);                         // Hold loading screen for 1750ms
      
      //Clear Display:
      lcd.clear();
      
      Serial.println("Controller Ready");                               //Debug
      
    }
 
  void loop () 
    {

      // (1) Grab Wavelength, and convert to RPM. (Duty Cycle 50% so T=(Wavelength)(2)):
      WaveLength = pulseIn(RPMFreq, HIGH, 30000);
      unsigned int RPM = (15*(1000000/WaveLength));
      //Serial.println(RPM);                               //Debug
      
      // (2) Determine if RPM is in acceptable range:      // Returns "1" for Complete, "-1" for Error
      int a = verifyRPM(RPM);
      //Serial.println(a);                               //Debug
      
      if(a==1)
        {
          // (3) Update RPM display meter:
          int b = growingRPMmeter(RPM);      // Returns "1" for Complete, "-1" for Error
          //    Serial.println(b);                            //Debug
      
          // (4) Update Warning LEDs:
          int c = warningLEDdisplay(RPM, 37);      // Returns "1-4" for Complete, "-1" for Error
          //Serial.println(c);                               //Debug         
      
          // (5) Write RPM to LCD Display:
          int d = writeRPMtoLCD(RPM);      // Returns "1-3" for Complete, "-1" for Error
          //Serial.println(d);                               //Debug
          
          // (6) Determine Current Gear:
          int e = currentGear(RPM, Speed, TireCircumference, FinalRatio1, FinalRatio2, FinalRatio3, FinalRatio4, FinalRatio5, FinalRatio6);       // Returns current gear, "-1" for Error
          Serial.println(e);                            //Debug                              
        }

   }


//----------------[Determine if RPM is within acceptable range]----------------
  int verifyRPM(int t_RPM)
    {
      int retval;
      if(t_RPM<=99 || t_RPM>21000)      // Determine if RPM is out of range
        {
          lcd.setCursor(0,0);
          lcd.print("RPM ERROR       ");
          lcd.setCursor(0,1);
          lcd.print("                ");
          digitalWrite(yellowlineLED, LOW);
          digitalWrite(redlineLED, LOW);
          retval = -1;
        }
      else
        {
          retval = 1;
        }
      return retval;  
    }
    

//----------------[Update RPM meter]----------------    
  int growingRPMmeter(unsigned int t_RPM)
    {
      int retval;
      unsigned char t_Char;
      unsigned int t_Segment;
      float t_Divisions = 16;
      double t_percentRPM = (t_RPM/21000.0)*100.0;
      double t_RPMdisplayunits = (t_Divisions/100)*t_percentRPM;
      lcd.setCursor(0,1);
      if (t_RPMdisplayunits>=1)
        {
          for (int t_CV=1;t_CV<t_RPMdisplayunits;t_CV++)
            {
              lcd.write(4);      // Library automatically advances "cursor" to the next horizontal position after writing. This is how the loop is able to work.
              t_Char=t_CV;
            }
          t_RPMdisplayunits=(t_RPMdisplayunits-t_Char);
        }
       
      t_Segment=(t_RPMdisplayunits*5);
      
      switch (t_Segment)
        {
           case 0:
            break;
           case 1:
            lcd.write((byte)0);
            break;
           case 2:
            lcd.write((byte)1);
            break;
           case 3:
            lcd.write((byte)2);
            break;             
           case 4:
            lcd.write((byte)3);
            break; 
         }   
      
      for (int t_CV = 0;t_CV<(t_Divisions-t_Char);t_CV++)
        {
          lcd.print("  ");
        }
      retval = 1;
      return retval;
    }
    
    
//----------------[Update Warning LEDs]----------------    
  int warningLEDdisplay(int t_RPM, long t_Interval)
    {
      int retval;
      if(t_RPM<12500)
        {
          digitalWrite(yellowlineLED, LOW);
          digitalWrite(redlineLED, LOW);
          retval = 1;
        }
      else if(t_RPM>=12500 && t_RPM <16500)
        {
          digitalWrite(yellowlineLED, HIGH);
          digitalWrite(redlineLED, LOW);
          retval = 2;
        }
      else if(t_RPM>=16500 && t_RPM<18000)
        {
          digitalWrite(yellowlineLED, HIGH);
          digitalWrite(redlineLED, HIGH);
          retval = 3;
        }
      else if(t_RPM>=18000 && t_RPM<=21000)
        {
          unsigned long currentMillis = millis();
          if((currentMillis - previousMillis) > t_Interval)
            {
              previousMillis = currentMillis;
              if(warningLEDstate == LOW)
                {
                  warningLEDstate = HIGH;
                }
              else
                {
                  warningLEDstate = LOW;
                }
              digitalWrite(yellowlineLED, warningLEDstate);
              digitalWrite(redlineLED, warningLEDstate);
            }
          retval = 4;
        }
      else if(t_RPM>21000)
        {
          retval = -1;
        }
      return retval;
    }  
    
    
//----------------[Write RPM to LCD Display]----------------
  int writeRPMtoLCD(unsigned int t_RPM)
    {
      int retval;
      lcd.setCursor(0,0);
      lcd.print("RPM:");
      if(t_RPM>9999 && t_RPM<=21000)      // Determine if RPM is x10^4
        {
          lcd.setCursor(4,0);
          lcd.print(t_RPM);      // Print RPM
          retval = 1;
        }
      else if(t_RPM>999 && t_RPM<=9999)      // Determine if RPM is x10^3
        {
          lcd.setCursor(4,0);
          lcd.print(" ");
          lcd.setCursor(5,0);
          lcd.print(t_RPM);      // Print RPM
          retval = 2;
        }
      else if(t_RPM>99 && t_RPM<=999)      // Determine if RPM is x10^2
        {
          lcd.setCursor(4,0);
          lcd.print("  ");
          lcd.setCursor(6,0);
          lcd.print(t_RPM);      // Print RPM
          retval = 3;
        } 
      if(t_RPM<=99 || t_RPM>21000)      // Determine if RPM is out of range
        {
          retval = -1;      // Error
        }
      return retval;
    }


//----------------[Determine Current Gear]----------------
  int currentGear(unsigned int t_RPM, float t_Speed, float t_Circumference, float t_Drive1, float t_Drive2, float t_Drive3, float t_Drive4, float t_Drive5, float t_Drive6)
    {
      int retval;
      boolean t_Netral = 0;
      float t_GearRatioCalc = ((t_RPM*t_Circumference)/(t_Speed*1056));
      lcd.setCursor(13,0);
      lcd.print("G:");
      t_Netral = digitalRead(NeutralInput);
      if(t_Netral == HIGH)
        {
          lcd.setCursor(15,0);
          lcd.print("N");
          retval = 0;
        }
      else
        {
          if(t_GearRatioCalc>=(t_Drive1-(t_Drive1*0.03)) & t_GearRatioCalc<=(t_Drive1+(t_Drive1*0.03)))
            {
              lcd.setCursor(15,0);
              lcd.print("1");
              retval = 1;
            }
          else if(t_GearRatioCalc>=(t_Drive2-(t_Drive2*0.03)) & t_GearRatioCalc<=(t_Drive2+(t_Drive2*0.03)))
            {
              lcd.setCursor(15,0);
              lcd.print("2");
              retval = 2;
            }
          else if(t_GearRatioCalc>=(t_Drive3-(t_Drive3*0.03)) & t_GearRatioCalc<=(t_Drive3+(t_Drive3*0.03)))
            {
              lcd.setCursor(15,0);
              lcd.print("3");
              retval = 3;
            }        
          else if(t_GearRatioCalc>=(t_Drive4-(t_Drive4*0.03)) & t_GearRatioCalc<=(t_Drive4+(t_Drive4*0.03)))
            {
              lcd.setCursor(15,0);
              lcd.print("4");
              retval = 4;
            }
          else if(t_GearRatioCalc>=(t_Drive5-(t_Drive5*0.03)) & t_GearRatioCalc<=(t_Drive5+(t_Drive5*0.03)))
            {
              lcd.setCursor(15,0);
              lcd.print("5");
              retval = 5;
            }
          else if(t_GearRatioCalc>=(t_Drive6-(t_Drive6*0.03)) & t_GearRatioCalc<=(t_Drive6+(t_Drive6*0.03)))
            {
              lcd.setCursor(15,0);
              lcd.print("6");
              retval = 6;
            }            
          else
            {
              lcd.setCursor(15,0);
              lcd.print("E");
              retval = -1;
            }
        }
      return retval;      
    }
