void buildMainToolbar(GtkWidget* boxToPackInto);
void buildMenu(GtkWidget* boxToPackInto);
void buildMiddleToolbar(GtkWidget* boxToPackInto);
void caseSensitiveSortCbk(GtkButton *button, gpointer data);
gboolean closeMainWindowCbk(GtkWidget *widget, GdkEvent *event);
void loadAppIcon(GdkPixbuf** appIcon);
void loadIcons(void);
void loadIcon(GtkWidget** destIcon, const char* srcFile, int size);
void rejectDialogCbk(GtkWidget *widget, GdkEvent *event);
void sortDirsFirstCbk(GtkButton *button, gpointer data);
