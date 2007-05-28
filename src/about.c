/******************************* LICENCE **************************************
* Any code in this file may be redistributed or modified under the terms of
* the GNU General Public Licence as published by the Free Software 
* Foundation; version 2 of the licence.
****************************** END LICENCE ***********************************/

/******************************************************************************
* Author:
* Andrew Smith, http://littlesvr.ca/misc/contactandrew.php
*
* Contributors:
* 
******************************************************************************/

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "isomaster.h"

extern GtkWidget* GBLmainWindow;

#if GTK_MINOR_VERSION >= 6

static const char* 
GBLprogramName = "ISO Master 1.0rc1";

static const char* 
GBLauthors[2] = {
"Many thanks to all the following people:\n"
"\n"
"Andrew Smith\n"
"http://littlesvr.ca/misc/contactandrew.php\n"
"Summer 2005 - Fall 2006\n"
"- author and maintainer\n"
"\n"
"Barb Czegel\n"
"http://cs.senecac.on.ca/~barb.czegel/\n"
"Summer-fall 2005\n"
"- one of my teachers from Seneca College who has been kind enough to let me\n"
"  work on ISO Master as my systems project for two semesters\n"
"\n"
"Nicolas Devillard\n"
"http://ndevilla.free.fr/iniparser/\n"
"August 2006\n"
"- the excellent iniparser, for storing and reading config files\n"
"\n"
"The Samba Project\n"
"http://samba.org/\n"
"December 2005\n"
"- most of the filename mangling code I copied from Samba\n"
"\n"
"Steffen Winterfeldt\n"
"September 2006\n"
"- helped me figure out how to work with isolinux boot records\n"
"\n"
"Roman Hubatsch (kearone)\n"
"http://kearone.deviantart.com/\n"
"December 2006 - January 2007\n"
"- the excellent 'add', 'extract' icons and the new ISO Master icon\n"
"- equally excellent 'go back', 'new folder' and 'delete' icons\n"
"\n"
"David Johnson\n"
"http://www.david-web.co.uk/\n"
"September 2006 - February 2007\n"
"- a patch to allow associating ISO Master with ISO files in file managers\n"
"- a patch to give ISO Master the --help command line parameter\n"
"- gave me access to a 32bit ARM box running Debian for testing\n"
"- gave me access to a big endian SuperSPARC II box running Debian for testing\n"
"\n"
"Ernst W. Winter\n"
"December 2006\n"
"- gave me access to an AMD64 box running OpenBSD for testing\n"
"\n"
"Tango Desktop Project\n"
"http://tango.freedesktop.org/Tango_Icon_Gallery\n"
"August 2006\n"
"- the pretty 'new folder' icon (replaced with kearone's version in 0.8)\n"
"\n"
"Packages:\n"
"\n"
"David Johnson\n"
"http://www.david-web.co.uk/\n"
"- Debian packages of ISO Master, versions 0.1 - 0.7\n"
"\n"
"Marcin Zajaczkowski\n"
"http://timeoff.wsisiz.edu.pl/rpms.html\n"
"- Fedora packages of ISO Master, versions 0.3 - 0.7\n"
"\n"
"Toni Graffy\n"
"Maintainer of many SuSE packages at PackMan\n"
"- SuSE packages of ISO Master, versions 0.4 - 0.7\n"
"\n"
"Maciej Libuda\n"
"- Arch packages of ISO Master, versions 0.3 - 0.7\n"
"\n"
"GuestToo\n"
"- Puppy packages of ISO Master, versions 0.1, 0.4 - 0.7\n"
"\n"
"vktgz\n"
"http://www.vktgz.homelinux.net/\n"
"- Gentoo ebuilds of ISO Master, versions 0.4 - 0.7\n"
"\n"
"James Bowling\n"
"http://www.jamesbowling.com/\n"
"- Slackware packages of ISO Master, versions 0.4 - 0.5\n"
"\n"
"Michael Shigorin\n"
"- Alt package of ISO Master, versions 0.5, 0.7\n"
"\n"
,
NULL};

static const char* 
GBLtranslators = 
"Dessislav Petrov\n"
"- bg (Bulgarian) translation of ISO Master versions 0.6 - 0.8\n"
"\n"
"Toni Graffy\n"
"- de (German) translation of ISO Master versions 0.6 - 0.8\n"
"\n"
"Juan García-Murga Monago\n"
"- es (Spanish) translation of ISO Master version 0.7 - 0.8\n"
"\n"
"Leif Thande\n"
"- fr (French) translation of ISO Master version 0.6\n"
"\n"
"Andrew Smith\n"
"- fr (French) translation of ISO Master version 0.7\n"
"\n"
"Bruno Bord\n"
"- fr (French) translation of ISO Master version 0.8\n"
"\n"
"F. Lombardi\n"
"- it (Italian) translation of ISO Master version 0.7 - 0.8\n"
"\n"
"Valerio Guaglianone\n"
"- it (Italian) translation of ISO Master version 0.7\n"
"  Not used because unfortunately it was made at the same time as \n"
"  F. Lombardi's translation. Thanks anyway!\n"
"\n"
"Stephen Brandt\n"
"- nl (Dutch) translation of ISO Master version 0.7 - 0.8\n"
"\n"
"Marciej Libuda\n"
"- pl (Polish) translation of ISO Master versions 0.6 - 0.8\n"
"\n"
"Joao de Ferro\n"
"- pt (Portuguese) translation of ISO Master version 0.8\n"
"\n"
"Bogdan Radulescu\n"
"- ro (Romanian) translation of ISO Master version 0.7 - 0.8\n"
"\n"
"Anton Obidin\n"
"- ru (Russian) translation of ISO Master versions 0.6 - 0.8\n"
"\n"
"Jozef Riha\n"
"- sk (Slovak) translation of ISO Master version 0.7\n"
"\n"
"Besnik Bleta\n"
"- sq (Albanian) translation of ISO Master version 0.7\n"
"\n"
"Sun Minchao\n"
"- zh_CN (Simplified Chineese) translation of ISO Master version 0.7 - 0.8\n"
"\n"
"Besnik Bleta\n"
"- zh_TW (Chineese/Taiwan) translation of ISO Master version 0.6 - 0.8\n";

static const char* 
GBLcomments = "An application for editing ISO9660 images based on the "
              "bkisofs access library and the GTK2 GUI toolkit.";

static const char* 
GBLcopyright = "Copyright 2005-2007 Andrew Smith";

static const char* 
GBLwebsite = "http://littlesvr.ca/isomaster/";

#endif /* GTK_MINOR_VERSION >= 6 */

static const char* 
GBLlicense = 
"ISO Master and bkisofs are distributed under the GNU General Public Licence\n"
"version 2, please see LICENCE.TXT for the complete text\n";

static const char*
GBLhelp = 
N_(
"The ISO Master window is split in 2 parts:\n"
" - The top file browser shows files and directories on your machine.\n"
" - The bottom file browser shows files and directories on the ISO image.\n"
"\n"
"To open an existing ISO file click on 'Image' and 'Open'. To create a new\n"
"ISO file click on 'Image' and 'New'.\n"
"\n"
"To add one or more files or directories to the ISO select them in the top\n"
"file browser and click the 'Add to the ISO' button in the middle toolbar.\n"
"\n"
"To extract one or more files or directories from the ISO select them in\n"
"the bottom file browser and click the 'Extract from ISO' button in the\n"
"middle toolbar.\n"
"\n"
"You can also delete files or directories from the ISO and create new\n"
"directories both on the ISO and your local filesystem.\n"
"\n"
"Once you made all the changes to the new ISO image, click on 'Image' and\n"
"'Save As'. You cannot overwrite the original ISO.\n"
);

gboolean helpKeyPressCbk(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
    if(event->keyval == GDK_Escape)
    {
        gtk_widget_destroy(widget);
        
        return TRUE;
    }
    
    return FALSE;
}

void showAboutWindowCbk(GtkMenuItem* menuItem, gpointer data)
{
#if GTK_MINOR_VERSION >= 6
    gtk_show_about_dialog(GTK_WINDOW(GBLmainWindow), 
                          "name", GBLprogramName,
                          "authors", GBLauthors,
                          "translator-credits", GBLtranslators,
                          "comments", GBLcomments,
                          "copyright", GBLcopyright,
                          "license", GBLlicense,
                          "website", GBLwebsite,
                          NULL);
#endif
}

/* This callback is also used by an accelerator so make sure you don't use 
* the parameters, since they may not be the menuitem parameters */
void showHelpOverviewCbk(GtkMenuItem* menuItem, gpointer data)
{
    GtkWidget* window;
    GtkWidget* label;
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), _("ISO Master Help"));
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(GBLmainWindow));
    g_signal_connect(window, "key-press-event", (GCallback)helpKeyPressCbk, NULL);
    
    label = gtk_label_new(_(GBLhelp));
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_widget_show(label);

    gtk_widget_show(window);
}
