#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"
#include "allocatememory.h"
#include "allocatepixelmatrix.h"
#include "loadimage.h"
#include "readp2.h"
#include "readp5.h"
#include "readp3.h"
#include "readp6.h"
#include "freeimage.h"
#include "selectpixels.h"
#include "selectall.h"
#include "allocvector.h"
#include "histogram.h"
#include "crop.h"
#include "equalize.h"
#include "rotate90.h"
#include "rotatepixels.h"
#include "rotatewhole.h"
#include "rotatergb.h"
#include "swapelem.h"
#include "rotate.h"
#include "allocatedouble.h"
#include "applyedge.h"
#include "applyboxblur.h"
#include "applysharpen.h"
#include "applygaussianblur.h"
#include "apply.h"
#include "printimage.h"
#include "saveimage.h"

int main(void)
{
	image img = {.width = 0, .height = 0, .data = NULL, .rgb = NULL};
	selection select;
	char filename[30], command[30] = "", format[30] = "";
	int x1, y1, x2, y2;
	while (strcmp(command, "EXIT") != 0) {
		scanf("%s", command);
		switch (command[0]) {
		case 'L':
			if (strcmp(command, "LOAD") == 0) {
				scanf("%s", filename);
				freeimage(&img); //eliberez imaginea anterioara
				loadimage(filename, &img);//incarc o imagine
				select.x1 = 0;//selectez intreaga imagine
				select.x2 = img.width;
				select.y1 = 0;
				select.y2 = img.height;
			}
			break;
		case 'S':
			if (strcmp(command, "SELECT") == 0) {
				getchar();
				fgets(format, sizeof(format), stdin);
				format[strcspn(format, "\n")] = '\0';
				if (strncmp(format, "ALL", 3) == 0) { //selectez totul
					selectall(img, &select);
					select.selected = 1;
				} else {
					int num = sscanf(format, "%d %d %d %d", &x1, &y1, &x2, &y2);
					if (num == 4) {
						selectpixels(img, &select, x1, y1, x2, y2);
						select.selected = 1;
					} else {
						printf("Invalid command");
						printf("\n");
					}
				}
			}
			if (strcmp(command, "SAVE") == 0) {//citesc fisierul si salvez
				scanf("%s", filename);
				saveimage(&img, filename);
			}
			break;
		case 'R':
			if (strcmp(command, "ROTATE") == 0) {
				//apelez rotate
				rotate(&img, &select);
			}
			break;
		case 'E':
			if (strcmp(command, "EXIT") == 0) {
				if (!img.data && !img.rgb) {//verific daca exista o imagine
					printf("No image loaded");
					printf("\n");
				}
			} else if (strcmp(command, "EQUALIZE") == 0) {
				//apelez equalize
				equalize(&img);
			}
			break;
		case 'H':
			if (strcmp(command, "HISTOGRAM") == 0) {
				//apelez histogram
				histogram(img);
			}
			break;
		case 'C':
			if (strcmp(command, "CROP") == 0)
				crop(&select, &img);//apelez crop
			break;
		case 'A':
			apply(&img, select, command);//apelez apply
			break;
		default:
			printf("Invalid command\n");
		}
	}

	freeimage(&img);
	
	return 0;
}
