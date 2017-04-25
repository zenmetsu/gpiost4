/*******************************************************************************
  Copyright(c) 2012 Jasem Mutlaq. All rights reserved.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.

  The full GNU General Public License is included in this distribution in the
  file called LICENSE.
*******************************************************************************/

/**********************************************************************
  Crude kludge to use Pine64 GPIO libraries instead of Arduino

  Code tested on Pine64 2GB model running 64bit Armbian distribution.

  Jason Westervelt - 20170422

**********************************************************************/

#include "gpiost4driver.h"
#include <indidevapi.h>
#include "pinout.h"
#include "gpio.h"

using namespace std;
using namespace Pine64;

GPIO* st4 = new GPIO();

GPIOST4Driver::GPIOST4Driver()
{
    guideCMD[0]=0;
    debug=false;
}

GPIOST4Driver::~GPIOST4Driver()
{
    //dtor
    // usb_close(usb_handle);
}



bool GPIOST4Driver::Connect()
{
  if (st4->setup()) {
    std::cout << "Failed to configure GPIO\n" << std::endl;
    return 0;
  }
  std::cout << " Setting GPIO pin directions\n" << std::endl;
  st4->pinMode(RA_plus, OUTPUT);
  st4->pinMode(RA_minus, OUTPUT);
  st4->pinMode(DEC_plus, OUTPUT);
  st4->pinMode(DEC_minus, OUTPUT);
  std::cout << " GPIO ready\n" << std::endl;
  return 1;
}

bool GPIOST4Driver::Disconnect()
{
  //write_blocking("DISCONNECT#");

  //  close(this->fd);
  //  return true;
}

bool GPIOST4Driver::startPulse(int direction)
{
    switch (direction)
    {
      case GPIOST4_NORTH:
        if (debug) IDLog("Start North\n");
          st4->digitalWrite(RA_plus, HIGH);
        break;

        case GPIOST4_WEST:
        if (debug) IDLog("Start West\n");
          st4->digitalWrite(DEC_plus, HIGH);
        break;

        case GPIOST4_SOUTH:
        if (debug) IDLog("Start South\n");
          st4->digitalWrite(RA_minus, HIGH);
        break;

        case GPIOST4_EAST:
        if (debug) IDLog("Start East\n");
          st4->digitalWrite(DEC_minus, HIGH);
        break;
    }
    return 1;
   
}

bool GPIOST4Driver::stopPulse(int direction)
{
    switch (direction)
    {
        case GPIOST4_NORTH:
        if (debug) IDLog("Stop North\n");
          st4->digitalWrite(RA_plus, LOW);
        break;

        case GPIOST4_WEST:
        if (debug) IDLog("Stop West\n");
          st4->digitalWrite(DEC_plus, LOW);
        break;

        case GPIOST4_SOUTH:
        if (debug) IDLog("Stop South\n");
          st4->digitalWrite(RA_minus, LOW);
        break;

        case GPIOST4_EAST:
        if (debug) IDLog("Stop East\n");
          st4->digitalWrite(DEC_minus, LOW);
        break;
    }
    return 1;
}


bool GPIOST4Driver::write_blocking(std::string s){
  this->write(s);
  std::string res = this->wait_read();
  return res == "INITIALIZED#" || res == "OK#";
}

void GPIOST4Driver::write(std::string s){
  const char *cmd = s.c_str();
  int n_written = 0,
  spot = 0;

  do {
    n_written = ::write( fd, &cmd[spot], 1 );
    spot += n_written;
  } while (spot != s.length() && n_written > 0);
}

std::string GPIOST4Driver::wait_read(){
  int n = 0;
  int spot = 0;
  char buf = '\0';

  /* Whole response*/
  char response[1024];
  memset(response, '\0', sizeof response);

  do {
    n = read( fd, &buf, 1 );
    sprintf( &response[spot], "%c", buf );
    spot += n;
  } while( buf != '#' && n > 0);
  return std::string(response);
}
