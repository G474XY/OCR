#include <stdio.h>
#include <gtk/gtk.h>
#include "../solver/solver.h"
#include "../imageTreatment/apply_image.h"

#define grid_size 9 //todo
const size_t grid_size_squared = grid_size * grid_size;
#define square_size 56

#define line_width 3
#define line_color 0
#define color_channels 4
#define color_switch_threshold 100
#define save_image_size 420

#define h_margin 80
#define v_margin 50

#define grayscale_state 1
#define lighting_state 2
#define contrast_state 3
#define binarisation_state 4
#define invert_state 5
#define sobel_state 6
#define unsolved_state 7
#define solved_state 8

#define loaded_it_folder "../imageTreatment/"

#define term_pref "[OCR-du-turfu_1.0.0] "

const char* solved_path = "solved.png";
const char* unsolved_path = "unsolved.png";

const char* err_message = 
    "There was an issue with the treatment of "
   "your image. Please try again or, if the issue persists,"
   "consider taking a different image.";

const char* unsolvable_message = 
    "The grid you are trying to solve is not solvable.\n\n"
    "Note : this can be a malencontrous problem from our end.\n"
    "If you are not sure, you can always try again.";


//========================================

//============STRUCT FUNCTIONS============

typedef struct UI
{
    GtkImage* image;
    //char* image_path;
    GtkWindow* window;
    GtkFixed* fixed;
    GtkButton* start_button;
    GtkFileChooserButton* load_button;
    GtkButton* save_button;
    GtkButton* cycle_left;
    GtkButton* cycle_right;
} UI;

typedef struct Image
{
    char* path;
    GdkPixbuf* image;
} Image;

typedef struct Images
{
    Image loaded_image;
    Image grayscale;
    Image lighting;
    Image contrast;
    Image binarisation;
    Image invert;
    Image sobel;
    Image initial_sudoku;
    Image solved_sudoku;
} Images;

typedef struct Data
{
    guchar** number_images;
    UI ui;
    Images images;
    int state;
    int max_state;
} Data;

void set_active_start_button(Data* data,gboolean active)
{
    gtk_widget_set_sensitive(GTK_WIDGET(data->ui.start_button),active);
}

void set_active_cycle_buttons(Data* data,gboolean active)
{
    gtk_widget_set_sensitive(GTK_WIDGET(data->ui.cycle_left),active);
    gtk_widget_set_sensitive(GTK_WIDGET(data->ui.cycle_right),active);
}

void set_active_save_button(Data* data,gboolean active)
{
    gtk_widget_set_sensitive(GTK_WIDGET(data->ui.save_button),active);
}

//========================================

//============IMAGE FUNCTIONS=============

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
        snprintf(path_buff,30,"images/%d.png",i);
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
    pixel[0] = 56;
    pixel[1] = 216;
    pixel[2] = 75;
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

GdkPixbuf* sudoku_to_image(char* initialsudoku,char* solvedsudoku,guchar** number_images)
{
    size_t size = grid_size * square_size + (grid_size + 1) * line_width;

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
    char* pix_grid = solvedsudoku == NULL ? initialsudoku : solvedsudoku;
    for(int i = 0; i < grid_size; i++) //9x
    {
        counter = draw_lines(pixels,counter,size,line_width);
        for(int j = 0; j < square_size; j++)
        {
            tmp_square_counter = square_counter;
            for(int k = 0; k < grid_size; k++)
            {
                counter = draw_line(pixels,counter,line_width);
                for(int l = 0; l < square_size; l++)
                {
                    unsigned int pix_val = get_grayscale(
                        number_images[(int)pix_grid[tmp_square_counter]] + (l + j * square_size) * color_channels);
                    if(solvedsudoku != NULL && initialsudoku[tmp_square_counter] == 0)
                    {
                        if(pix_val > color_switch_threshold)
                            put_pixel_grayscale(pixels + counter,255);
                        else
                            put_pixel_newnumber(pixels + counter);
                    }
                    else
                    {
                        if(pix_val >= color_switch_threshold || initialsudoku[tmp_square_counter] == 0)
                            put_pixel_grayscale(pixels + counter,255);
                        else
                            put_pixel_grayscale(pixels + counter,0);
                    }
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

char change_image(Data* data,Image* image_to_change)
{
    GError** error = NULL;
    
    //if(image_to_change->image == NULL || 1)
    //{
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(image_to_change->path,error);
    if(pixbuf == NULL)
    {
        g_print("%sWarning : the specified path is not an image!\n",term_pref);
        return 1; //Error
    }

    resize_image(data,&pixbuf);

    image_to_change->image = pixbuf;
    //}
    gtk_image_set_from_pixbuf(data->ui.image,image_to_change->image);

    //gtk_image_set_from_file(oldimage,path);
    return 0;
}

GdkPixbuf* load_and_resize(Data* data,char* path)
{
    GError* err = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(path,&err);
    if(pixbuf == NULL)
    {
        printf("%s%s\n",term_pref,err->message);
        return NULL;
    }
    resize_image(data,&pixbuf);
    return pixbuf;
}

void load_it_images(Data* data)
{
    //Executes only if there is an image to compute

    mkdir("tmp");
    load_image(data->images.loaded_image.path);
    
    data->images.grayscale.path = grayscale_path;
    data->images.grayscale.image = load_and_resize(data,grayscale_path);

    data->images.lighting.path = lighting_path;
    data->images.lighting.image = load_and_resize(data,lighting_path);

    data->images.contrast.path = contrast_path;
    data->images.contrast.image = load_and_resize(data,contrast_path);

    data->images.binarisation.path = binarisation_path;
    data->images.binarisation.image = load_and_resize(data,binarisation_path);

    data->images.invert.path = invert_path;
    data->images.invert.image = load_and_resize(data,invert_path);

    data->images.sobel.path = sobel_path;
    data->images.sobel.image = load_and_resize(data,sobel_path);
}

void remove_it_images()
{
    remove(grayscale_path);
    remove(lighting_path);
    remove(contrast_path);
    remove(binarisation_path);
    remove(invert_path);
    remove(sobel_path);
}

void cycle_images(Data* data,int delta)
{
    int newstate = data->state + delta;
    if(newstate < 0 || newstate > data->max_state)
        return;
    
    set_active_cycle_buttons(data,TRUE);
    
    data->state = newstate;
    
    GdkPixbuf* pixbuf = NULL;
    switch (newstate)
    {
    case 0:
        pixbuf = data->images.loaded_image.image;
        gtk_widget_set_sensitive(GTK_WIDGET(data->ui.cycle_left),FALSE);
        break;
    case grayscale_state:
        pixbuf = data->images.grayscale.image;
        break;
    case lighting_state:
        pixbuf = data->images.lighting.image;
        break;
    case contrast_state:
        pixbuf = data->images.contrast.image;
        break;
    case binarisation_state:
        pixbuf = data->images.binarisation.image;
        break;
    case invert_state:
        pixbuf = data->images.invert.image;
        break;
    case sobel_state:
        pixbuf = data->images.sobel.image;
        break;
    case unsolved_state:
        pixbuf = data->images.initial_sudoku.image;
        break;
    case solved_state:
        pixbuf = data->images.solved_sudoku.image;
        gtk_widget_set_sensitive(GTK_WIDGET(data->ui.cycle_right),FALSE);
        break;
    
    default:
        break;
    }
    if(pixbuf != NULL)
    {
        gtk_image_set_from_pixbuf(data->ui.image,pixbuf);
    }
}


//========================================

//===========LINKING FUNCTIONS============

char pre_traitement(Data* data)
{
    if(data->images.loaded_image.path == NULL)
    {
        return 1; //Error
    }

    load_it_images(data);
    remove_it_images();

    return 0;
}

void get_grid()
{

}

void get_grid_from_nn()
{

}

void solve_sudoku(Data* data,char* grid)
{
    if(data->number_images == NULL)
        data->number_images = load_number_images();

    char grid_not_solved[grid_size_squared];
    for(size_t i = 0; i < grid_size_squared; i++)
    {
        grid_not_solved[i] = grid[i];
    }
    GdkPixbuf* sudoku_image_notsolved = sudoku_to_image(
        grid_not_solved,NULL,data->number_images);
    resize_image(data,&sudoku_image_notsolved);
    data->images.initial_sudoku.image = sudoku_image_notsolved;

    data->max_state = unsolved_state;
    int res = solveSudoku(grid);
    if(res != 0)
    {
        data->max_state = solved_state;
        GdkPixbuf* sudoku_image_solved = sudoku_to_image(
        grid_not_solved,grid,data->number_images);
        resize_image(data,&sudoku_image_solved);
        data->images.solved_sudoku.image = sudoku_image_solved;
    }
    else
    {
        data->images.solved_sudoku.image = NULL;
        data->images.solved_sudoku.path = NULL;
    }
    
    set_active_save_button(data,TRUE);
}

//========================================

//===========SIGNAL FUNCTIONS=============

void on_start(GtkButton* button,gpointer user_data)
{
    Data* data = user_data;

    pre_traitement(data);
    get_grid();
    get_grid_from_nn();

    
    char* grid = calloc(grid_size_squared,sizeof(char)); //TODO
    loadSudoku("../solver/solvable/s00.oku",grid); //TODO
    solve_sudoku(data,grid);
    free(grid);
    set_active_start_button(data,FALSE);
    //set_active_cycle_buttons(data,TRUE);
    cycle_images(data,0);

    if(button || user_data)
        return;
}

void on_load_file(GtkFileChooserButton* button,gpointer user_data)
{
    Data *data = user_data;

    set_active_cycle_buttons(data,FALSE);
    set_active_save_button(data,FALSE);
    data->max_state = 0;
    data->state = 0;

    char* path = gtk_file_chooser_get_filename(
        GTK_FILE_CHOOSER(data->ui.load_button));
    data->images.loaded_image.path = path;
    g_print("%sLoading image at path %s\n",term_pref,path);
    if(change_image(data,&(data->images.loaded_image))) //Error
        data->images.loaded_image.path = NULL;
    else
        gtk_widget_set_sensitive(GTK_WIDGET(data->ui.start_button),TRUE);
    
    if(button)
        return;
}

void on_cycle_left(GtkButton* button,gpointer user_data)
{
    Data* data = user_data;
    cycle_images(data,-1);
    if(button)
        return;
}

void on_cycle_right(GtkButton* button,gpointer user_data)
{
    Data* data = user_data;
    cycle_images(data,1);
    if(button)
        return;
}

void on_save(GtkButton* button,gpointer user_data)
{
    Data* data = user_data;
    GError* err = NULL;

    GdkPixbuf* to_save = NULL;
    char* solved_or_not = NULL;
    if(data->images.solved_sudoku.image != NULL)
    {
        to_save = data->images.solved_sudoku.image;
        solved_or_not = "solved";
    }
    else
    {
        to_save = data->images.initial_sudoku.image;
        solved_or_not = "unsolved";
    }
    GdkPixbuf* resized_image = gdk_pixbuf_scale_simple(
            to_save,save_image_size,save_image_size,
            GDK_INTERP_BILINEAR);
    gdk_pixbuf_save(resized_image,solved_path,"png",&err,NULL);
    printf("%sSaved %s grid to %s\n",term_pref,solved_or_not,solved_path);

    if(button)
        return;
}

//========================================


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
    gtk_widget_set_sensitive(GTK_WIDGET(button),FALSE);
    GtkFileChooserButton* loadbutton = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder,"ocr.loadfile"));
    GtkButton* savebutton = GTK_BUTTON(gtk_builder_get_object(builder,"ocr.save"));
    gtk_widget_set_sensitive(GTK_WIDGET(savebutton),FALSE);
    GtkButton* cycleleft = GTK_BUTTON(gtk_builder_get_object(builder,"ocr.cycleleft"));
    GtkButton* cycleright = GTK_BUTTON(gtk_builder_get_object(builder,"ocr.cycleright"));
    gtk_widget_set_sensitive(GTK_WIDGET(cycleleft),FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(cycleright),FALSE);

    GtkFixed* fixed = GTK_FIXED(gtk_builder_get_object(builder,"ocr.fixed"));

    GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder,"ocr.image"));

    Data data = 
    {
        .number_images = NULL,
        .max_state = 0,
        .state = 0,
        .ui = 
        {
            .image = GTK_IMAGE(image),
            .window = window,
            .fixed = fixed,
            .start_button = button,
            .load_button = loadbutton,
            .save_button = savebutton,
            .cycle_left = cycleleft,
            .cycle_right = cycleright
        },
        .images =
        {
            .loaded_image = {NULL,NULL},
            .grayscale = {NULL,NULL},
            .lighting = {NULL,NULL},
            .contrast = {NULL,NULL},
            .binarisation = {NULL,NULL},
            .invert = {NULL,NULL},
            .sobel = {NULL,NULL},
            .solved_sudoku = {"not_solved.png",NULL},
            .initial_sudoku = {"solved.png",NULL}
        }
    };

    g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(button,"clicked",G_CALLBACK(on_start),&data);
    g_signal_connect(cycleleft,"clicked",G_CALLBACK(on_cycle_left),&data);
    g_signal_connect(cycleright,"clicked",G_CALLBACK(on_cycle_right),&data);
    g_signal_connect(loadbutton,"file-set",G_CALLBACK(on_load_file),&data);
    g_signal_connect(savebutton,"clicked",G_CALLBACK(on_save),&data);
    //g_signal_connect(window,"configure-event",G_CALLBACK(on_configure),&data);
    gtk_main();

    free_number_images(data.number_images);

    return 0;
}




















