#ifndef window_h
#define window_h

#include <stdbool.h>

#include "isomaster.h"
#include "iniparser-2.17/src/iniparser.h"

#define ISOMASTER_DEFAULT_WINDOW_WIDTH 500
#define ISOMASTER_DEFAULT_WINDOW_HEIGHT 550
#define ISOMASTER_DEFAULT_TOPPANE_HEIGHT 170

/* not putting this in the makefile because i really can't think of a
* distro that doesn't have a writeable /tmp directory */
#define DEFAULT_TEMP_DIR "/tmp"

typedef struct
{
    /* stuff only read from the config file */
    int windowWidth;
    int windowHeight;
    int topPaneHeight;
    char* fsCurrentDir;
    int isoSortColumnId;
    int isoSortDirection;
    int fsSortColumnId;
    int fsSortDirection;
    char* editor;
    char* viewer;
    char* tempDir;
    
    /* stuff read from the config file that will also be written back from here */
    bool showHiddenFilesFs;
    bool sortDirectoriesFirst;
    bool scanForDuplicateFiles;
    bool followSymLinks;
    char* lastIsoDir;
    bool appendExtension;
    char* lastBootRecordDir;
    
    /* stuf that's never in the config file, but is a setting */
    int filenameTypesToWrite;
    
} AppSettings;

void buildImagePropertiesWindow(GtkWidget *widget, GdkEvent *event);
void changeEditorCbk(GtkButton *button, gpointer data);
void changeTempDirCbk(GtkButton *button, gpointer data);
void changeViewerCbk(GtkButton *button, gpointer data);
void findHomeDir(void);
void followSymLinksCbk(GtkButton *button, gpointer data);
void getDefaultEditor(char** destStr);
void getDefaultTempDir(char** destStr);
void getDefaultViewer(char** destStr);
void openConfigFile(char* configFilePathAndName);
void loadSettings(void);
void scanForDuplicatesCbk(GtkButton *button, gpointer data);
void writeSettings(void);

#endif
