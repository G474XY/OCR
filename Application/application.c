#include <stdio.h>
#include <gtk/gtk.h>

#define term_pref "[OCR-du-turfu_1.1.4] "


typedef struct UI
{
    GtkImage* image;
    GtkWindow* window;
    GtkFixed* fixed;
    GtkButton* start_button;
    GtkFileChooserButton* load_button;
} UI;

typedef struct Data
{
    UI ui;
} Data;

void resize_image(Data* data,GdkPixbuf** image)
{
    //image is a pointer to the buffer of the newly loaded image. (from a file)
    int w = gdk_pixbuf_get_width(*image);
    int h = gdk_pixbuf_get_height(*image);

    int max_w = gtk_widget_get_allocated_width(GTK_WIDGET(data->ui.image));
    int max_h = gtk_widget_get_allocated_height(GTK_WIDGET(data->ui.image));
    
    float rap = (float) w / h;
    float max_rap = (float) max_w / max_h;

    int width = 0;
    int height = 0;
    if(rap >= max_rap)
    {
        //Image is 'larger' than its container
        width = max_w;
        height = (int)(h * ((float) max_w / w));
    }
    else
    {
        //Image is 'higher' than its container
        width = (int)(w * ((float) max_h / h));
        height = max_h;
    }

    *image = gdk_pixbuf_scale_simple(*image,width,height,GDK_INTERP_BILINEAR);
}

void change_image(Data* data,char* path)
{
    GError** error = NULL;
    
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(path,error);
    if(pixbuf == NULL)
    {
        g_print("%sWarning : the specified path is not an image!\n",term_pref);
        return;
    }

    resize_image(data,&pixbuf);

    gtk_image_set_from_pixbuf(data->ui.image,pixbuf);

    //gtk_image_set_from_file(oldimage,path);
}

void on_load_file(GtkFileChooserButton* button,gpointer user_data)
{
    Data *data = user_data;
    char* path = gtk_file_chooser_get_filename(
        GTK_FILE_CHOOSER(data->ui.load_button));
    g_print("%sLoading image at path %s\n",term_pref,path);
    change_image(data,path);
}


void on_start(GtkButton* button,gpointer user_data)
{
    g_print("%sInitializing processing...\n",term_pref);
    g_print("%sRetrieving image...\n",term_pref);
    g_print("%sApplying pre-processing on image...\n",term_pref);
    g_print("%sDetecting grid...\n",term_pref);
    g_print("%sSending found images to neural network...\n",term_pref);
    g_print("%sSolving found sudoku grid...\n",term_pref);
    g_print("%sRendering solved grid...\n",term_pref);
    g_print("%sEnding processing...\n",term_pref);
    if(button || user_data)
        return;
}


int main()
{
    gtk_init(NULL,NULL);
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if(gtk_builder_add_from_file(builder,"ocr.glade",&error) == 0)
    {
        g_printerr("Error loading from file: %s\n",error->message);
        g_clear_error(&error);
        return 1;
    }
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder,"ocr.window"));
    GtkButton* button = GTK_BUTTON(gtk_builder_get_object(builder,"ocr.solvebutton"));
    //GtkPaned* paned = GTK_PANED(gtk_builder_get_object(builder,"ocr.paned"));
    GtkFileChooserButton* loadbutton = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder,"ocr.loadfile"));
    GtkFixed* fixed = GTK_FIXED(gtk_builder_get_object(builder,"ocr.fixed"));

    GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder,"ocr.image"));

    Data data = 
    {
        .ui = 
        {
            .image = GTK_IMAGE(image),
            .window = window,
            .fixed = fixed,
            .start_button = button,
            .load_button = loadbutton
        }
    };

    g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(button,"clicked",G_CALLBACK(on_start),&data);
    g_signal_connect(loadbutton,"file-set",G_CALLBACK(on_load_file),&data);
    //g_signal_connect(window,"configure-event",G_CALLBACK(on_configure),&data);

    gtk_main();

    return 0;
}




















