#include <stdio.h>
#include <gtk/gtk.h>
#include "../solver/solver.h"

#define grid_size 9 //todo
const size_t grid_size_squared = grid_size * grid_size;
#define square_size 28

#define line_width 5
#define line_color 0
#define color_channels 4

#define h_margin 80
#define v_margin 50
#define term_pref "[OCR-du-turfu_1.1.4] "

const char* err_message = 
    "There was an issue with the treatment of "
   "your image. Please try again or, if the issue persists,"
   "consider taking a different image.";

const char* unsolvable_message = 
    "The grid you are trying to solve is not solvable.\n\n"
    "Note : this can be a malencontrous problem from our end.\n"
    "If you are not sure, you can always try again.";

typedef struct UI
{
    GtkImage* image;
    char* image_path;
    GtkWindow* window;
    GtkFixed* fixed;
    GtkButton* start_button;
    GtkFileChooserButton* load_button;
} UI;

typedef struct Data
{
    guchar** number_images;
    int current_number;
    UI ui;
} Data;

guchar** load_number_images()
{
    size_t image_size = square_size * square_size;
    guchar** res = calloc((grid_size + 1),sizeof(guchar*));
    for(size_t i = 0; i <= grid_size; i++)
    {
        res[i] = calloc(image_size * color_channels,sizeof(guchar));
    }
    char* path_buff = calloc(30,sizeof(char));
    
    GError* err = NULL;
    GdkPixbuf* pix = NULL;
    for(int i = 0; i <= grid_size; i++)
    {
        guchar* tmp = res[i];
        snprintf(path_buff,30,"images/%d.png\0",i);
        printf("%s\n",path_buff);
        pix = gdk_pixbuf_new_from_file_at_scale(path_buff,square_size,square_size,FALSE,&err);
        guchar* pixels = gdk_pixbuf_get_pixels(pix);
        for(size_t j = 0; j < image_size; j++)
        {
            tmp[0] = pixels[0];
            tmp[1] = pixels[1];
            tmp[2] = pixels[2];
            tmp[3] = 255;
            tmp += color_channels;
            pixels += color_channels;
        }
    }
    free(path_buff);

    return res;
}

void free_number_images(guchar** number_images)
{
    if(number_images == NULL)
        return;
    for(size_t i = 0; i <= grid_size; i++)
    {
        free(number_images[i]);
    }
    free(number_images);
}

unsigned int get_grayscale(guchar* pixel)
{
    return (pixel[0] + pixel[1] + pixel[2]) / 3;
}

void put_pixel_grayscale(guchar* pixel,guchar gray_value)
{
    pixel[0] = gray_value;
    pixel[1] = gray_value;
    pixel[2] = gray_value;
    pixel[3] = 255;
}

void put_pixel_newnumber(guchar* pixel)
{
    pixel[0] = 0;
    pixel[1] = 0;
    pixel[2] = 180;
    pixel[3] = 255;
}

size_t draw_line(guchar* pixels,size_t start,size_t len)
{
    size_t i = 0;
    size_t j = start;
    while(i < len)
    {
        put_pixel_grayscale(pixels + j,line_color);
        i += 1;
        j += color_channels;
    }
    return j;
}

size_t draw_lines(guchar* pixels,size_t start,size_t len,size_t num_lines)
{
    size_t j = start;
    for(size_t i = 0; i < num_lines; i++)
    {
        j = draw_line(pixels,j,len);
    }
    return j;
}

GdkPixbuf* sudoku_to_image(char* sudoku,guchar** number_images)
{
    size_t size = grid_size * square_size + (grid_size + 1) * line_width;
    //size_t size_bytes = size * color_channels;
    size_t size_squared = size * size;

    GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
        TRUE,
        8,
        size,
        size
    );

    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);
    
    size_t counter = 0;
    size_t square_counter = 0;
    size_t tmp_square_counter = 0;
    int line_count = 0;
    for(int i = 0; i < grid_size; i++) //9x
    {
        counter = draw_lines(pixels,counter,size,line_width);
        for(int j = 0; j < square_size; j++)
        {
            tmp_square_counter = square_counter;
            size_t test = counter;
            for(int k = 0; k < grid_size; k++)
            {
                counter = draw_line(pixels,counter,line_width);
                for(int l = 0; l < square_size; l++)
                {
                    unsigned int pix_val = get_grayscale(
                        number_images[sudoku[tmp_square_counter]] + (l + j * square_size) * color_channels);
                    //put_pixel_grayscale(pixels + counter,pix_val);
                    if(pix_val > 100 || sudoku[tmp_square_counter] == 0)
                        put_pixel_grayscale(pixels + counter,255);
                    else
                        put_pixel_grayscale(pixels + counter,pix_val);
                    counter += color_channels;
                }
                tmp_square_counter += 1;
            }
            counter = draw_line(pixels,counter,line_width);
        }
        square_counter += grid_size;
    }
    counter = draw_lines(pixels,counter,size,line_width);

    return pixbuf;
}

void resize_image(Data* data,GdkPixbuf** image)
{
    GtkWidget* widgimage = GTK_WIDGET(data->ui.image);

    //image is a pointer to the buffer of the newly loaded image. (from a file)
    int w = gdk_pixbuf_get_width(*image);
    int h = gdk_pixbuf_get_height(*image);

    int max_w = gtk_widget_get_allocated_width(widgimage) - h_margin;
    int max_h = gtk_widget_get_allocated_height(widgimage) - v_margin;
    
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

char change_image(Data* data,char* path)
{
    GError** error = NULL;
    
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(path,error);
    if(pixbuf == NULL)
    {
        g_print("%sWarning : the specified path is not an image!\n",term_pref);
        return 1; //Error
    }

    resize_image(data,&pixbuf);

    gtk_image_set_from_pixbuf(data->ui.image,pixbuf);

    //gtk_image_set_from_file(oldimage,path);
    return 0;
}

void on_load_file(GtkFileChooserButton* button,gpointer user_data)
{
    Data *data = user_data;
    char* path = gtk_file_chooser_get_filename(
        GTK_FILE_CHOOSER(data->ui.load_button));
    data->ui.image_path = path;
    g_print("%sLoading image at path %s\n",term_pref,path);
    if(change_image(data,path))
        data->ui.image_path = NULL;
}

void pop_window(Data* data,const char* message)
{
    GtkWidget* message_window = gtk_message_dialog_new_with_markup(
        data->ui.window,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        "<span weight=\"ultrabold\">Warning :\n</span>%s",
        message
    );

    GtkDialog* dialog_window = GTK_DIALOG(message_window);

    //gtk_dialog_add_buttons(dialog_window,"OK",NULL); //segfault
    gtk_dialog_run(dialog_window);
    gtk_widget_destroy(message_window);
}

//========================================

//===========LINKING FUNCTIONS============

char pre_traitement(Data* data)
{
    if(data->ui.image_path == NULL)
    {
        return 1; //Error
    }


    return 0;
}

void get_grid()
{

}

void get_grid_from_nn()
{

}

void solve_sudoku()
{
    //solveSudoku(grid);
}

//========================================

void on_start(GtkButton* button,gpointer user_data)
{
    /*g_print("%sInitializing processing...\n",term_pref);
    g_print("%sRetrieving image...\n",term_pref);
    g_print("%sApplying pre-processing on image...\n",term_pref);
    g_print("%sDetecting grid...\n",term_pref);
    g_print("%sSending found images to neural network...\n",term_pref);
    g_print("%sSolving found sudoku grid...\n",term_pref);
    g_print("%sRendering solved grid...\n",term_pref);
    g_print("%sEnding processing...\n",term_pref);
    */
    Data* data = user_data;

    pre_traitement(data);
    get_grid();
    get_grid_from_nn();
    solve_sudoku();

    //pop_window(data,unsolvable_message);

    if(data->number_images == NULL)
        data->number_images = load_number_images();


    char grid[grid_size_squared];
    loadSudoku("../solver/solvable/s00.oku",grid);
    gtk_image_set_from_pixbuf(data->ui.image,sudoku_to_image(grid,data->number_images));

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
        .number_images = NULL,
        .current_number = 0,
        .ui = 
        {
            .image = GTK_IMAGE(image),
            .image_path = NULL,
            .window = window,
            .fixed = fixed,
            .start_button = button,
            .load_button = loadbutton
        }
    };

    GdkPixbuf* pix = gdk_pixbuf_new_from_file("images/1_r.png",&error);
    gtk_image_set_from_pixbuf(data.ui.image,pix);

    g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(button,"clicked",G_CALLBACK(on_start),&data);
    g_signal_connect(loadbutton,"file-set",G_CALLBACK(on_load_file),&data);
    //g_signal_connect(window,"configure-event",G_CALLBACK(on_configure),&data);

    gtk_main();

    free_number_images(data.number_images);

    return 0;
}




















