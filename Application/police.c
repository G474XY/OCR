#include <stdio.h>
#include <gtk/gtk.h>

#define img_size 28
const int img_size_squared = img_size * img_size;

unsigned int grayscale(guchar* pixel)
{
    return (pixel[0] + pixel[1] + pixel[2]) / 3;
}

void put_pixel(guchar* pixel,guchar r,guchar g, guchar b)
{
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
}

GdkPixbuf* resize_img_from_path(char* path)
{
    GError* err = NULL;
    GdkPixbuf* img = gdk_pixbuf_new_from_file_at_scale(path,
    img_size,img_size,FALSE,&err);
    guchar* pixels = gdk_pixbuf_get_pixels(img);
    int n_channels = gdk_pixbuf_get_n_channels(img);
    int rowstride = gdk_pixbuf_get_rowstride(img);
    printf("%d %d\n",n_channels,rowstride);
    guchar* p = NULL;
    for(int i = 0; i < img_size_squared; i++)
    {
        p = pixels + i * n_channels;
        if(grayscale(p) >= 60)
            put_pixel(p,255,255,255);
    }
    return img;
}

int main()
{
    char path[100];
    for(int i = 0; i < 10; i++)
    {
        GError* err = NULL;
        snprintf(path,100,"images/%d.png",i);
        GdkPixbuf* img = resize_img_from_path(path);
        snprintf(path,100,"images/%d_r.png",i);
        gdk_pixbuf_save(img,path,"png",&err);
    }
    //resize_img_from_path("images/02.png");
}