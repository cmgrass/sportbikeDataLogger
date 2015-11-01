//------------------------------------
// CG_SBK_DataLogger_GEAR_Component
// Christopher Grass - 01/05/2014
// Calibrated for 2008 Kawasaki ZX6R
//------------------------------------

/*
 * RS: 8
 * EN: 9
 * D4: 4
 * D5: 5
 * D6: 6
 * D7: 7
*/

// Include LCD Library:
#include <LiquidCrystal.h>

// Define Variables:
int Backlight = 10;
int RPMFreq = 11;
int Neutral = 19;
unsigned long WaveLength;
float final1 = 15.13;
float final2 = 12.26;
float final3 = 10.31;
float final4 = 8.92;
float final5 = 7.92;
float final6 = 7.25;

byte Column01[8] = {B10000,B10000,B10000,B10000,B10000,B10000,B10000,B10000};
byte Column02[8] = {B11000,B11000,B11000,B11000,B11000,B11000,B11000,B11000};
byte Column03[8] = {B11100,B11100,B11100,B11100,B11100,B11100,B11100,B11100};
byte Column04[8] = {B11110,B11110,B11110,B11110,B11110,B11110,B11110,B11110};
byte Column05[8] = {B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111};

// Initialize Library
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


  void setup () 
    {
      // Setup Serial:
      Serial.begin(9600);
      Serial.println("Initialize Void Setup () Routine");                               //Debug
    
      // Define Misc. I/O Assignment:
      pinMode(RPMFreq, INPUT);
      pinMode(Neutral, INPUT);
        
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
      
      // (2) Determine if RPM is in acceptable range:
      int a = verifyRPM(RPM);
      //Serial.println(a);                               //Debug
      
      if(a==1)
        {       
          // (5) Write RPM to LCD Display:
          int d = writeRPMtoLCD(RPM);      // Returns "1" for Complete, "-1" for Error
          //Serial.println(d);                               //Debug

          
          // (6) Determine Gear
          boolean NeutralVal = 0;
          float Speed = 44.67;      // Input from GPS, temporary hardcode for testing
          float TireCircumference = 77.9;      // User nonvoatile input, temporary hardcode for testing
          float GearRatioCalc = ((RPM*TireCircumference)/(Speed*1056));
          Serial.println(GearRatioCalc);
          lcd.setCursor(13,0);
          lcd.print("G:");
          NeutralVal = digitalRead(Neutral);
          Serial.println(NeutralVal);
          if(NeutralVal == HIGH)
            {
              lcd.setCursor(15,0);
              lcd.print("N");
            }
          else
            {
              if(GearRatioCalc>=(final1-(final1*0.03)) & GearRatioCalc<=(final1+(final1*0.03)))
                {
                  lcd.setCursor(15,0);
                  lcd.print("1");
                }
              else if(GearRatioCalc>=(final2-(final2*0.03)) & GearRatioCalc<=(final2+(final2*0.03)))
                {
                  lcd.setCursor(15,0);
                  lcd.print("2");
                }
              else if(GearRatioCalc>=(final3-(final3*0.03)) & GearRatioCalc<=(final3+(final3*0.03)))
                {
                  lcd.setCursor(15,0);
                  lcd.print("3");
                }        
              else if(GearRatioCalc>=(final4-(final4*0.03)) & GearRatioCalc<=(final4+(final4*0.03)))
                {
                  lcd.setCursor(15,0);
                  lcd.print("4");
                }
              else if(GearRatioCalc>=(final5-(final5*0.03)) & GearRatioCalc<=(final5+(final5*0.03)))
                {
                  lcd.setCursor(15,0);
                  lcd.print("5");
                }
              else if(GearRatioCalc>=(final6-(final6*0.03)) & GearRatioCalc<=(final6+(final6*0.03)))
                {
                  lcd.setCursor(15,0);
                  lcd.print("6");
                }            
              else
                {
                  lcd.setCursor(15,0);
                  lcd.print("E");
                }
            }
          
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
          retval = -1;
        }
      else
        {
          retval = 1;
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
