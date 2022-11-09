#include <stdio.h>

int h = 50;
int w = 100;

int is_valid(int x, int y, int key)
{
    if (x < h && y < w && x >= 0 && y >= 0 && key != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void propage(int n1, int n2, int mask[],int index)
{
	for (size_t i = 0; i < index; i++)
	{
		if (mask[i] == n2)
			mask[i] = n1;

		if (mask[i] > n2)
			mask[i] -= 1;
	}
}

void check_neighbors(int x, int y, int mask[])
{
	int left = mask[x*w+y-1];
	int top = mask[(x-1)*w+y];
    if ((is_valid(x-1, y, left) == 0) && (is_valid(x-1, y, top) == 0))	
		mask[x*w+y] = 0;
	
	else if ((is_valid(x-1, y, left) == 1) && (is_valid(x-1, y, top) == 1))
	{
		mask[x*w+y] = left;
		propage(left, top, mask, x*w+y);
	}
	
	else if ((is_valid(x-1, y, left) == 1))
		mask[x*w+y] = left;

	else	
		mask[x*w+y] = top;
}

void resize_grid(int arr[], int values[])
{
    int mask[h*w];
	for (int k = 0; k < h*w; k++)
	{
		mask[k] = 0;
	}

	int index = 1;
    for (int i = 0; i < h;i++)
    {
        for(int j = 0; j < w;j++)
        {
            int actual = arr[i*w+j];
            if (actual != 0)
            {
                check_neighbors(i, j, mask);
                if (mask[i*w+j] == 0)
                {
                    mask[i*w+j] = index;
                    index += 1;
                }
            }
			else
			{
				mask[i*w+j] = 0;
			}
        }
    }

	//creer un histogramme avec le nombre de valeurs par zones
	int histo[index];
	for (int i = 0; i < index;i++)
	{
		histo[i] = 0;
	}

    for (int k = 0; k < h;k++)
    {
        for(int l = 0; l < w;l++)
        {
			int actual = mask[k*w+l];
			histo[actual] += 1;
		}
	}

	// trouve le nombre de la zone la plus grande
	int i = 1;
	int max = 0;
	int number = 0;
	while (i < index)
	{
		if (histo[i] > max)
		{
			max = histo[i];
			number = i;
		}
		i+=1;
	}

	//garde que la plus grande zone
    for (int i = 0; i < h;i++)
    {
        for(int j = 0; j < w;j++)
        {
			if (mask[i*w+j] != number)
				mask[i*w+j] = 0;
		}
	}
	
	//ne garde que la matrice delimitee a la zone
	int h1 = h;
	int w1 = w;
	int h2 = 0;
	int w2 = 0;
    for (int i = 0; i < h;i++)
    {
        for(int j = 0; j < w;j++)
        {
			if (mask[i*w+j] == 1)
			{
				if (i < h1)
					h1 = i;
				if (i > h2)
					h2 = i;
			}
		}
	}

    for (int k = 0; k < h;k++)
    {
        for(int l = 0; l < w;l++)
        {
			if (mask[k*w+l] == 1)
			{
				if (l < w1)
					w1 = l-1;
				if (l > w2)
					w2 = l-1;
			}
		}
	}
	values[0] = h1;
	values[1] = w1;
	values[2] = h2;
	values[3] = w2;
}



int main()
{
	int mat[5000];
	for (size_t i = 0; i < 5000; i++)
	{
		mat[i] = 0;
	}
	mat[3] = 1;
	mat[13] = 1;
	mat[14] = 1;
	mat[15] = 1;
	mat[27] = 1;
	mat[28] = 1;
	mat[32] = 1;
	mat[37] = 1;
	mat[41] = 1;
	mat[49] = 1;

	int values[4];
	resize_grid(mat, values);
	for (int i = 0; i< 4; i++)
	{
		printf("%d\n", values[i]);
	}

    return 0;
}
