void buildFsBrowser(GtkWidget* boxToPackInto);
void changeFsDirectory(char* newDirStr);
void fsGoUpDirTree(GtkButton *button, gpointer data);
void fsRowDblClickCbk(GtkTreeView* treeview, GtkTreePath* path,
                      GtkTreeViewColumn* col, gpointer data);
void refreshFsView(void);
void showHiddenCbk(GtkButton *button, gpointer data);
