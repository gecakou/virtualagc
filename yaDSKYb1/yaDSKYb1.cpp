/*
 * Copyright 2016 Ronald S. Burkey <info@sandroid.org>
 *
 * This file is part of yaAGC.
 *
 * yaAGC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * yaAGC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with yaAGC; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * In addition, as a special exception, Ronald S. Burkey gives permission to
 * link the code of this program with the Orbiter SDK library (or with
 * modified versions of the Orbiter SDK library that use the same license as
 * the Orbiter SDK library), and distribute linked combinations including
 * the two. You must obey the GNU General Public License in all respects for
 * all of the code used other than the Orbiter SDK library. If you modify
 * this file, you may extend this exception to your version of the file,
 * but you are not obligated to do so. If you do not wish to do so, delete
 * this exception statement from your version.
 *
 * Filename:    yaDSKYb1.cpp
 * Purpose:     Common functionality for the different versions of the  Block
 *              1 DSKY simulation programs, yaDSKYb1-main and yaDSKYb1-nav.
 * Compiler:    GNU gcc.
 * Contact:     Ron Burkey <info@sandroid.org>
 * Reference:   http://www.ibiblio.org/apollo/index.html
 * Mods:        2016-09-04 RSB  Began.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <wx/utils.h>
#include "yaDSKYb1.h"
extern int
CallSocket(char *hostname, unsigned short portnum);

// Stuff for the timer we use for reading the socket interface.
static char DefaultHostname[] = "localhost";
char *Hostname = DefaultHostname;
static char NonDefaultHostname[129];
#ifdef WIN32
static int StartupDelay = 500;
#else
static int StartupDelay = 0;
#endif
static int VerbNounFlashing = 0;
static int ServerSocket = -1;
void
OutputKeycode(int Keycode);

///////////////////////////////////////////////////////////////////////////////////////
// Some debugging functions

void
MyFrame::setAllNumbers(wxBitmap& bitmap)
{
  Digit1Reg1->SetBitmap(bitmap);
  Digit2Reg1->SetBitmap(bitmap);
  Digit3Reg1->SetBitmap(bitmap);
  Digit4Reg1->SetBitmap(bitmap);
  Digit5Reg1->SetBitmap(bitmap);
  Digit1Reg2->SetBitmap(bitmap);
  Digit2Reg2->SetBitmap(bitmap);
  Digit3Reg2->SetBitmap(bitmap);
  Digit4Reg2->SetBitmap(bitmap);
  Digit5Reg2->SetBitmap(bitmap);
  Digit1Reg3->SetBitmap(bitmap);
  Digit2Reg3->SetBitmap(bitmap);
  Digit3Reg3->SetBitmap(bitmap);
  Digit4Reg3->SetBitmap(bitmap);
  Digit5Reg3->SetBitmap(bitmap);
  digitProgramLeft->SetBitmap(bitmap);
  digitProgramRight->SetBitmap(bitmap);
  digitNounLeft->SetBitmap(bitmap);
  digitNounRight->SetBitmap(bitmap);
  digitVerbLeft->SetBitmap(bitmap);
  digitVerbRight->SetBitmap(bitmap);
}
void
MyFrame::setAllSigns(wxBitmap& bitmap)
{
  PlusMinusReg1->SetBitmap(bitmap);
  PlusMinusReg2->SetBitmap(bitmap);
  PlusMinusReg3->SetBitmap(bitmap);
}

///////////////////////////////////////////////////////////////////////////////////////
// These are the events for the UI.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(ID_VERBBUTTON, MyFrame::on_VerbButton_pressed)
EVT_BUTTON(ID_NOUNBUTTON, MyFrame::on_NounButton_pressed)
EVT_BUTTON(ID_PLUSBUTTON, MyFrame::on_PlusButton_pressed)
EVT_BUTTON(ID_MINUSBUTTON, MyFrame::on_MinusButton_pressed)
EVT_BUTTON(ID_ZEROBUTTON, MyFrame::on_ZeroButton_pressed)
EVT_BUTTON(ID_SEVENBUTTON, MyFrame::on_SevenButton_pressed)
EVT_BUTTON(ID_FOURBUTTON, MyFrame::on_FourButton_pressed)
EVT_BUTTON(ID_ONEBUTTON, MyFrame::on_OneButton_pressed)
EVT_BUTTON(ID_EIGHTBUTTON, MyFrame::on_EightButton_pressed)
EVT_BUTTON(ID_FIVEBUTTON, MyFrame::on_FiveButton_pressed)
EVT_BUTTON(ID_TWOBUTTON, MyFrame::on_TwoButton_pressed)
EVT_BUTTON(ID_NINEBUTTON, MyFrame::on_NineButton_pressed)
EVT_BUTTON(ID_SIXBUTTON, MyFrame::on_SixButton_pressed)
EVT_BUTTON(ID_THREEBUTTON, MyFrame::on_ThreeButton_pressed)
EVT_BUTTON(ID_CLEARBUTTON, MyFrame::on_ClearButton_pressed)
EVT_BUTTON(ID_KEYRLSEBUTTON, MyFrame::on_KeyRlseButton_pressed)
EVT_BUTTON(ID_ENTERBUTTON, MyFrame::on_EnterButton_pressed)
EVT_BUTTON(ID_ERRORRESETBUTTON, MyFrame::on_ErrorResetButton_pressed)
EVT_BUTTON(ID_UPTELSWITCH, MyFrame::on_UpTelButton_pressed)
END_EVENT_TABLE();

void
MyFrame::on_VerbButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(021);
}

void
MyFrame::on_NounButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(037);
}

void
MyFrame::on_PlusButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(032);
}

void
MyFrame::on_MinusButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(033);
}

void
MyFrame::on_ZeroButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(020);
}

void
MyFrame::on_OneButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(001);
}

void
MyFrame::on_TwoButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(002);
}

void
MyFrame::on_ThreeButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(003);
}

void
MyFrame::on_FourButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(004);
}

void
MyFrame::on_FiveButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(005);
}

void
MyFrame::on_SixButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(006);
}

void
MyFrame::on_SevenButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(007);
}

void
MyFrame::on_EightButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(010);
}

void
MyFrame::on_NineButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(011);
}

void
MyFrame::on_KeyRlseButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(031);
}

void
MyFrame::on_EnterButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(034);
}

void
MyFrame::on_ErrorResetButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(022);
}

void
MyFrame::on_ClearButton_pressed(wxCommandEvent &event)
{
  OutputKeycode(036);
}

void
MyFrame::on_UpTelButton_pressed(wxCommandEvent &event)
{
  UpTelAccept = !UpTelAccept;
  if (UpTelAccept)
    SwitchUpTel->SetBitmapLabel(imageUpTelAccept);
  else
    SwitchUpTel->SetBitmapLabel(imageUpTelBlock);
  // FIXME
}

///////////////////////////////////////////////////////////////////////
// For the socket-interface timer.

//-------------------------------------------------------------------------
// This function is called every PULSE_INTERVAL milliseconds.  It manages
// the server connection, and causes display-updates based on input from
// yaAGC.

void
TimerClass::Notify()
{
  static unsigned char Packet[4];
  static int PacketSize = 0;
  static int FlashCounter = 1, FlashStatus = 0;
  int i;
  unsigned char c;

  if (StartupDelay > 0)
    {
      StartupDelay -= PULSE_INTERVAL;
      return;
    }
  // If the noun/verb-flash flag is set, then flash them.
  if (frame->flashing)
    {
      frame->flashCounter++;
      if (frame->flashCounter >= 1000 / PULSE_INTERVAL)
        {
          frame->flashCounter = 0;
          frame->flashStateLit = !frame->flashStateLit;
          if (frame->flashStateLit)
            {
              frame->labelVerb->SetBitmap(frame->imageVerbLabelOn);
              frame->labelNoun->SetBitmap(frame->imageNounLabelOn);
            }
          else
            {
              frame->labelVerb->SetBitmap(frame->imageVerbLabelOff);
              frame->labelNoun->SetBitmap(frame->imageNounLabelOff);
            }
        }
    }
  // Try to connect to the server (yaAGC) if not already connected.
  if (ServerSocket == -1)
    {
      ServerSocket = CallSocket(Hostname, Portnum);
      if (ServerSocket != -1)
        printf("yaDSKY is connected.\n");
    }
  if (ServerSocket != -1)
    {
      for (;;)
        {
          i = recv(ServerSocket, (char *) &c, 1, MSG_NOSIGNAL);
          if (i == -1)
            {
              // The conditions i==-1,errno==0 or 9 occur only on Win32,
              // and I'm not sure exactly what they corresponds to---but
              // empirically I find that ignoring them makes no difference
              // to the operation of the program.
              if (errno == EAGAIN || errno == 0 || errno == 9)
                i = 0;
              else
                {
                  printf("yaDSKYb1:  server error %d\n", errno);
                  close(ServerSocket);
                  ServerSocket = -1;
                  break;
                }
            }
          if (i == 0)
            break;
          // This (newer) code will accept any packet signature of the form
          // 00 XX XX XX.
          if (0 == (0xc0 & c))
            PacketSize = 0;
          if (PacketSize != 0 || (0xc0 & c) == 0)
            {
              Packet[PacketSize++] = c;
              if (PacketSize >= 4)
                {
                  ActOnIncomingIO(Packet);
                  PacketSize = 0;
                }
            }
        }
    }
}

void
OutputKeycode(int Keycode)
{
  unsigned char Packet[8];
  int j;
  if (ServerSocket != -1)
    {
      FormIoPacket(0404, 077, Packet); // Mask for lowest 6 data bits.
      FormIoPacket(04, 040 | Keycode, &Packet[4]); // Data.
      j = send(ServerSocket, (const char *) Packet, 8, MSG_NOSIGNAL);
      if (j == SOCKET_ERROR && SOCKET_BROKEN)
        {
          close(ServerSocket);
          ServerSocket = -1;
        }
      else
        {
          wxMilliSleep(200);
          FormIoPacket(04, 0, &Packet[4]); // Data.
          j = send(ServerSocket, (const char *) Packet, 8, MSG_NOSIGNAL);
          if (j == SOCKET_ERROR && SOCKET_BROKEN)
            {
              close(ServerSocket);
              ServerSocket = -1;
            }
        }
    }
}

void
whatDigit(int selector, wxBitmap& digit)
{
  switch (selector)
    {
  case 0:
    digit = frame->image7Seg0;
    break;
  case 3:
    digit = frame->image7Seg3;
    break;
  case 15:
    digit = frame->image7Seg15;
    break;
  case 19:
    digit = frame->image7Seg19;
    break;
  case 21:
    digit = frame->image7Seg21;
    break;
  case 25:
    digit = frame->image7Seg25;
    break;
  case 27:
    digit = frame->image7Seg27;
    break;
  case 28:
    digit = frame->image7Seg28;
    break;
  case 29:
    digit = frame->image7Seg29;
    break;
  case 30:
    digit = frame->image7Seg30;
    break;
  case 31:
    digit = frame->image7Seg31;
    break;
  default:
    digit = frame->image7Seg0;
    break;
    }
}

void
setSign(uint8_t selector, wxStaticBitmap *sign)
{
  if (selector == 2)
    sign->SetBitmap(frame->imagePlusMinusPlus);
  else if (selector == 1)
    sign->SetBitmap(frame->imagePlusMinusMinus);
  else
    sign->SetBitmap(frame->imagePlusMinusOff);
}

void
TimerClass::ActOnIncomingIO(unsigned char *Packet)
{
  static uint16_t lastReceived[16] =
    { 0 }, lastOUT1 = 0;
  static uint8_t signs[3] =
    { 0 };
  int Channel, Value, uBit, i;
  // Check to see if the message has a yaAGC signature.  If not,
  // ignore it.  The yaAGC signature is 00 01 10 11 in the
  // 2 most-significant bits of the packet's bytes.  We are
  // guaranteed that the first byte is signed 00, so we don't
  // need to check it.
  if (0x40 != (Packet[1] & 0xc0) || 0x80 != (Packet[2] & 0xc0)
      || 0xc0 != (Packet[3] & 0xc0))
    {
      printf("yaDSKYb1:  illegal packet: %02X %02X %02X %02X\n", Packet[0],
          Packet[1], Packet[2], Packet[3]);
      goto Error;
    }
  if (ParseIoPacket(Packet, &Channel, &Value, &uBit))
    {
      printf("yaDSKYb1:  illegal values: %02X %02X %02X %02X\n", Packet[0],
          Packet[1], Packet[2], Packet[3]);
      goto Error;
    }
  //printf("yaDSKYb1:  received channel=%02o value=%04o ubit=%o\n", Channel,
  //    Value, uBit);
  if (uBit)
    return;
  if (Channel == 011 && (Value & 037) != lastOUT1) // OUT1
    {
      printf("yaDSKYb1:  received channel=%02o value=%04o ubit=%o\n", Channel,
          Value, uBit);
      lastOUT1 = Value & 037;
      // Ignore B1 for now: main-panel DSKY doesn't have a PROG ALM indicator.
      frame->indicatorCompFail->SetBitmap(
          (0 == (Value & 02)) ?
              frame->imageCompFailOff : frame->imageCompFailOn);
      // Ignore B3 for now: main-panel DSKY doesn't have a KEY RLSE indicator.
      // Ignore B4 for now: main-panel DSKY doesn't have a SCALER FAIL indicator.
      frame->indicatorCheckFail->SetBitmap(
          (0 == (Value & 020)) ?
              frame->imageCheckFailOff : frame->imageCheckFailOn);
    }
  else if (Channel == 010) // OUT0
    {
      int relayword = (Value >> 11) & 017;
      int received = Value & 03777;
      int bit11 = (Value >> 10) & 1;
      int bits10_6 = (Value >> 5) & 037;
      int bits5_1 = Value & 037;
      if (received != lastReceived[relayword])
        {
          lastReceived[relayword] = received;
          wxBitmap leftDigit, rightDigit;
          whatDigit(bits10_6, leftDigit);
          whatDigit(bits5_1, rightDigit);
          switch (relayword)
            {
          case 0: // Normal state, implying no change.
            break;
          case 11: //   n/a  MD1  MD2
            frame->digitProgramLeft->SetBitmap(leftDigit);
            frame->digitProgramRight->SetBitmap(rightDigit);
            break;
          case 10: // FLASH  VD1  VD2
            frame->digitVerbLeft->SetBitmap(leftDigit);
            frame->digitVerbRight->SetBitmap(rightDigit);
            if (frame->flashing && bit11 == 0)
              {
                printf("Turn flashing off.\n");
                frame->flashing = false;
                frame->flashStateLit = true;
                frame->labelNoun->SetBitmap(frame->imageNounLabelOn);
                frame->labelVerb->SetBitmap(frame->imageVerbLabelOn);
                frame->flashCounter = 0;
              }
            else if (!frame->flashing && bit11 != 0)
              {
                printf("Turn flashing on.\n");
                frame->flashing = true;
                frame->flashStateLit = false;
                frame->labelNoun->SetBitmap(frame->imageNounLabelOff);
                frame->labelVerb->SetBitmap(frame->imageVerbLabelOff);
                frame->flashCounter = 0;
              }
            break;
          case 9: //    n/a  ND1  ND2
            frame->digitNounLeft->SetBitmap(leftDigit);
            frame->digitNounRight->SetBitmap(rightDigit);
            break;
          case 8: //  UPACT  n/a R1D1
            frame->indicatorUpTl->SetBitmap(
                bit11 ? frame->imageUptlOn : frame->imageUptlOff);
            frame->Digit1Reg1->SetBitmap(rightDigit);
            break;
          case 7: //   +R1S R1D2 R1D3
            frame->Digit2Reg1->SetBitmap(leftDigit);
            frame->Digit3Reg1->SetBitmap(rightDigit);
            signs[0] = (signs[0] & ~2) | (bit11 << 1);
            setSign(signs[0], frame->PlusMinusReg1);
            break;
          case 6: //   -R1S R1D4 R1D5
            frame->Digit4Reg1->SetBitmap(leftDigit);
            frame->Digit5Reg1->SetBitmap(rightDigit);
            signs[0] = (signs[0] & ~1) | bit11;
            setSign(signs[0], frame->PlusMinusReg1);
            break;
          case 5: //   +R2S R2D1 R2D2
            frame->Digit1Reg2->SetBitmap(leftDigit);
            frame->Digit2Reg2->SetBitmap(rightDigit);
            signs[1] = (signs[1] & ~2) | (bit11 << 1);
            setSign(signs[1], frame->PlusMinusReg2);
            break;
          case 4: //   -R2S R2D3 R2D4
            frame->Digit3Reg2->SetBitmap(leftDigit);
            frame->Digit4Reg2->SetBitmap(rightDigit);
            signs[1] = (signs[1] & ~1) | bit11;
            setSign(signs[1], frame->PlusMinusReg2);
            break;
          case 3: //    n/a R2D5 R3D1
            frame->Digit5Reg2->SetBitmap(leftDigit);
            frame->Digit1Reg3->SetBitmap(rightDigit);
            break;
          case 2: //   +R3S R3D2 R3D3
            frame->Digit2Reg3->SetBitmap(leftDigit);
            frame->Digit3Reg3->SetBitmap(rightDigit);
            signs[2] = (signs[2] & ~2) | (bit11 << 1);
            setSign(signs[2], frame->PlusMinusReg3);
            break;
          case 1: //   -R3S R3D4 R3D5
            frame->Digit4Reg3->SetBitmap(leftDigit);
            frame->Digit5Reg3->SetBitmap(rightDigit);
            signs[2] = (signs[2] & ~1) | bit11;
            setSign(signs[2], frame->PlusMinusReg3);
            break;
          default:
            printf("Unused relayword %05o (%02o %1o %2o %2o)\n", received,
                relayword, bit11, bits10_6, bits5_1);
            break;
            }
        }

    }
  return;
  Error: IoErrorCount++;
}
