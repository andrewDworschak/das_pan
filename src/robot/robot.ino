#include <phys253.h>
#include <LiquidCrystal.h>

#include <pan.h>
#include <arm.h>
#include <drive.h>
#include <path.h>
#include <passenger.h>

Arm arm;
Drive drive;
Pan pan;
Passenger passenger;
Path path;

void setup()
{
#include <phys253setup.txt>
  Serial.begin(9600);

  arm.center();
  pan.leftUp();
  pan.rightUp();
}

uint8_t t;
int8_t n;
int8_t thing;
uint16_t currDistance;

uint8_t leftPassengers = 0, rightPassengers = 0;

boolean corner = false;
boolean crash = false;
boolean ccw = false;
boolean tight = false;

void loop() {
  drive.uturn(true);
}

void hileHitler()
{
  drive.go();

  if (drive.isSacked()) {
    thing = drive.describeIntersection();
    if (drive.isSacked() == 0) {
      if (thing != path.getDirections()) {
        LCD.print("We're Fucked!");
        LCD.print(thing);
        LCD.print(path.getDirections());
        drive.brake();
        while (!startbutton());
      }
      else
      {
        drive.brake();
        path.stats();
        while ( !startbutton() );

        if ( path.nearDrop() ) {
          drive.brake();
          LCD.home();
          LCD.clear();
          LCD.print("near drop");
          while ( !startbutton() );
          drive.prepareDrop();
        }

        path.update();
        switch (t)
        {
          case 0: drive.reverse();
            break;
          case 1: drive.left(tight);
            break;
          case 2: drive.straight();
            break;
          case 3: drive.right(tight);
            break;
          case 4: drive.uturn(ccw);
            break;
        }
      }
    }
  }
  else
  {
    corner = drive.intersection();
    crash = drive.collision();// && ( n != 0 ); // TODO: implement collisions with other robots and get rid of n != 0
    // TODO: handle IR detection around corners and stuff
    if ( passenger.precise() == 1 && leftPassengers < 2 )
    {
      drive.speed(50);
      /*while ( passenger.precise() != 1 ) {
        drive.go();
        passenger.stats();
        }*/
      drive.brake();

      if (leftPassengers) {
        currDistance = drive.getDistance();
        drive.speed(50);
        while (drive.getDistance() < currDistance + 3) drive.go();
        drive.brake();
        arm.leftBack();
      }
      else arm.leftFront();

      pan.leftPick();

      arm.cycle();

      arm.center();

      if (pan.leftFull(leftPassengers)) ++leftPassengers;
      path.passengers(leftPassengers + rightPassengers);
      LCD.home();
      LCD.clear();
      LCD.print("l: ");
      LCD.print(leftPassengers);
      LCD.print(" r: ");
      LCD.print(rightPassengers);
      while ( !startbutton() );

      pan.leftUp();
      drive.speed(150);
      //drive.go();
    }

    if ( passenger.precise() == 2 && rightPassengers < 2 )
    {
      drive.speed(50);
      /*while ( passenger.precise() != 2 ) {
        drive.go();
        passenger.stats();
        }*/

      drive.brake();

      pan.rightPick();

      if (rightPassengers) arm.rightBack();
      else arm.rightFront();
      arm.cycle();

      path.passengers(leftPassengers + ++rightPassengers);

      arm.center();

      pan.rightUp();
      drive.speed(150);
      //drive.go();
    }

    if ( corner || crash )
    {
      // TODO: write path.avoid()
      if ( crash ) {
        /*drive.brake(); // get rid of this
          LCD.clear();
          LCD.home();
          LCD.print("n: ");
          LCD.print(n);
          LCD.print(" c: ");
          LCD.print(crash);
          while ( !startbutton() ); // get rid of this*/
        if (( n == 2 && crash < 4 ) || ( n == 1 && crash > 5 )) {
          path.update();
          switch (t)
          {
            case 0: drive.reverse();
              break;
            case 1: drive.left(tight);
              break;
            case 2: drive.straight();
              break;
            case 3: drive.right(tight);
              break;
            case 4: drive.uturn(ccw);
              break;
          }
        }
        else {
          path.avoid();
        }
      }

      if (n == -1)
      {
        if (leftPassengers) {
          drive.brake();
          pan.leftDrop();
          delay(500);

          leftPassengers = 0;

          pan.leftUp();
        }

        if (rightPassengers) {
          drive.uturn(true);
          pan.rightDrop();
          delay(500);

          rightPassengers = 0;

          pan.rightUp();
          drive.uturn(false);
        }

        path.passengers(leftPassengers + rightPassengers);
      }
      else if (n == -2)
      {
        if (rightPassengers)
        {
          drive.brake();
          pan.rightDrop();
          delay(500);

          rightPassengers = 0;

          pan.rightUp();
        }

        if (leftPassengers)
        {
          drive.uturn(false);
          pan.leftDrop();
          delay(500);

          leftPassengers = 0;

          pan.leftUp();
          drive.uturn(true);
        }

        path.passengers(leftPassengers + rightPassengers);
      }

      //if ( path.nearEndpoint() ) drive.prepareEndpoint();

      ccw = ( n == 2 );
      tight = ( n == 3 );

      drive.go();

      n = path.find();

      drive.go();

      t = path.turn();

      if (crash)
      {

      }
      /*drive.brake();
        while(!startbutton());*/
    }
  }
}
