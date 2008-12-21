/*
  Copyright 2005 Ronald S. Burkey <info@sandroid.org>
  
  This file is part of yaAGC.

  yaAGC is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  yaAGC is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with yaAGC; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Filename:	main.c
  Purpose:	Main yaDEDA program.
  Mods:		05/30/05 RSB	I think that all of the logic is in place for 
  				basic operations, except that data isn't yet
				output to the AEA on demand.
*/

/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include <errno.h>

#include "yaAGC.h"
#include "agc_engine.h"
#include "agc_symtab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int HalfSize = 0;
#define PULSE_INTERVAL 100
gint Pulse (gpointer data);
char *Hostname = "localhost";
GtkWidget *window1 = NULL;
#ifdef WIN32
static int StartupDelay = 500;
#else
static int StartupDelay = 0;
#endif
extern int Portnum;
int ShowPackets = 0;

// This doesn't normally do anything.
#define DEBUG(x)

int
main (int argc, char *argv[])
{
  GtkWidget *MainWindow = NULL;
  GtkWidget *hCreateMainWindow (void);
  int i, j;
  void LocateWidgets (GtkWidget *widget);

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  printf ("yaDEDA Apollo DEDA simulation, ver " NVER ", built " __DATE__ " " __TIME__ "\n");
  printf ("Copyright 2005 by Ronald S. Burkey\n");
  printf ("Refer to http://www.ibiblio.org/apollo/index.html for more information.\n");
	  
  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  Portnum = 19897;
  for (i = 1; i < argc; i++)
    {
      if (!strncmp (argv[i], "--ip=", 5))
        Hostname = &argv[i][5];
      else if (!strncmp (argv[i], "--port=", 7))
        {
          Portnum = atoi (&argv[i][7]);
	  if (Portnum <= 0 || Portnum >= 0x10000)
	    {
	      printf ("The --port switch is out of range.  Must be 1-64K.\n");
	      return (1);
	    }
	}
      else if (!strcmp (argv[i], "--half-size"))
        {
	  if (MainWindow == NULL)
	    MainWindow = hCreateMainWindow ();
          HalfSize = 1;
	}
      else if (1 == sscanf (argv[i], "--delay=%d", &j))
        StartupDelay = j;
      else if (!strcmp (argv[i], "--show-packets"))
        ShowPackets = 1;
      else
        {
	  printf ("USAGE:\n");
	  printf ("\tyaDEDA [OPTIONS]\n");
	  printf ("The available options are:\n");
	  printf ("--ip=Hostname\n");
	  printf ("\tThe yaDEDA program and the yaAGS Abort Guidance System simulation\n");
	  printf ("\texist in a \"client/server\" relationship, in which the yaDEDA program\n");
	  printf ("\tneeds to be aware of the IP address or symbolic name of the host \n");
	  printf ("\tcomputer running the yaAGS program.  By default, this is \"localhost\",\n");
	  printf ("\tmeaning that both yaDEDA and yaAGS are running on the same computer.\n");
	  printf ("--port=Portnumber\n");
	  printf ("\tBy default, yaDEDA attempts to connect to the yaAGS program using port\n");
	  printf ("\tnumber %d.  However, if more than one instance of yaDEDA is being\n",
	          Portnum);
	  printf ("\trun, or if yaAGS has been configured to listen on different ports, then\n");
	  printf ("\tdifferent port settings for yaDEDA are needed.  Note that by default,\n");
	  printf ("\tyaAGS listens for new connections on ports %d-%d.\n",
	          Portnum, Portnum + 10 - 1);
	  printf ("--half-size\n");
	  printf ("\tUses a half-size version of yaDEDA, suitable for smaller graphical\n");
	  printf ("\tdisplays.\n");
	  printf ("--delay=N\n");
	  printf ("\t\"Start-up delay\", in ms.  Defaults to %d.  What the start-up\n", StartupDelay);
	  printf ("\tdelay does is to prevent yaDEDA from attempting to communicate with\n");
	  printf ("\tyaAGS for a brief time after power-up.  This option is really only\n");
	  printf ("\tuseful in Win32, to work around a problem with race-conditions in\n");
	  printf ("\tthe start-up scripts like SimFlightProgram6.  When the race problem is\n");
	  printf ("\tfixed correctly, this option will probably no longer be useful.\n");
	  printf ("--show-packets\n");
	  printf ("\tPrints messages about incoming packets from yaAGS, for debugging.\n");
	  return (0);
	}	
    }
  printf ("Hostname=%s, port=%d\n", Hostname, Portnum);	
  	
  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  DEBUG (0);
  if (MainWindow == NULL)
    MainWindow = create_MainWindow ();
  DEBUG (1);
  gtk_widget_show (MainWindow);
  LocateWidgets (MainWindow)

  // Sets up the Pulse function to be called every PULSE_INTERVAL ms.
  DEBUG (2);
  g_timeout_add (PULSE_INTERVAL, Pulse, NULL);

  DEBUG (3);
  gtk_main ();
  return 0;
}

//-------------------------------------------------------------------------
// Manage server connection.

int ServerSocket = -1;

gint
Pulse (gpointer data)
{
  static unsigned char Packet[4];
  static int PacketSize = 0;
  int i;
  unsigned char c;
  if (StartupDelay > 0)
    {
      StartupDelay -= PULSE_INTERVAL;
      return (TRUE);
    }
  // Try to connect to the server (yaAGC) if not already connected.
  if (ServerSocket == -1)
    {
      ServerSocket = CallSocket (Hostname, Portnum);
      if (ServerSocket != -1)
        printf ("yaDEDA is connected, socket=%d.\n", ServerSocket);
    }
  if (ServerSocket != -1)
    {
      for (;;)
        {
	  i = recv (ServerSocket, &c, 1, MSG_NOSIGNAL);
	  if (i == -1)
	    {
	      // The condition i==-1,errno==0 occurs only on Win32, and 
	      // I'm not sure exactly what it corresponds to---I assume 
	      // means "no data" rather than error.
	      if (errno == EAGAIN || errno == 0)
	        i = 0;
	      else
	        {	
		  printf ("yaDEDA reports server error %d\n", errno);
		  close (ServerSocket);
		  ServerSocket = -1;
		  break;
	        }
	    }
	  if (i == 0)
	    break;
	  if (0 == (0xc0 & c))
	    PacketSize = 0;
	  if (PacketSize != 0 || (0xc0 & c) == 0)	      
	    { 
	      Packet[PacketSize++] = c;
	      if (PacketSize >= 4)
		{
		  void ActOnIncomingIO (GtkWidget *widget, unsigned char *Packet);
		  ActOnIncomingIO (window1, Packet);
		  PacketSize = 0;   
		}  
	    }
	}
    }
  return (TRUE);    
}


