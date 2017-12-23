/*
*
*LIBHARU Remix by Dan Lindamood III
*
*
* Compile using command:
*	LINUX	gcc -o tab_to_pdf_graph -O2 -Wall tab_to_pdf_graph.c -lhpdf -lm
*	WINDOWS	cl -FeTab_To_PDF_Graph.exe -MT -O2 -Iinclude -Iwin32\include tab_to_pdf_graph.c /link /LIBPATH:. /LIBPATH:win32\msvc libhpdf.lib libpng.lib zlib.lib
*
* Print command example: Tab_To_PDF_Graph.exe tab_file.tab pdf_file.pdf
*
* Example prints are available on the wiki page
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"
#include <math.h>
#include <time.h>

jmp_buf env;

#ifdef HPDF_DLL
void  __stdcall
#else
void
#endif
error_handler(HPDF_STATUS   error_no,
HPDF_STATUS   detail_no,
void		 *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	longjmp(env, 1);
}


//^^^^^^^^^^^^^^^ PARSE TAB ^^^^^^^^^^^^^^^^^^^^ PARSE TAB ^^^^^^^^^^^^^^^^^ PARSE TAB ^^^^^^^^^^^^^^^^^  PARSE TAB ^^^^^^^^^^^^^^^^^
const char* getfield(char* tmpline, int col_num){

  char * pch;
  static char tmp_tab[64];
  int i = 0, q = 0;
  int start = 0;
  int end;

  pch=strchr(tmpline,'\t');
  tmp_tab[0] = '\0'; //V6
  while (pch!=NULL){
	end = pch-tmpline+1;
	if (i == col_num){
		for(q = 0; (q < end-start-1)&& (q < 64); q++) tmp_tab[q] = tmpline[start+q];
		tmp_tab[q]='\0';
	}
	i++;
	start = pch-tmpline+1;
	pch=strchr(pch+1,'\t');
  }

   return tmp_tab;
}



//^^^^^^^^^^^^^^^ RANDOM PASSWORD ^^^^^^^^^^^^^^^^^^^^ RANDOM PASSWORD ^^^^^^^^^^^^^^^^^ RANDOM PASSWORD ^^^^^^^^^^^^^^^^^  RANDOM PASSWORD ^^^^^^^^^^^^^^^^^
char *rand_str(char *dst, int size)
{
	static const char text[] = "abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"1234567890";
	int i;
	for (i = 0; i < 8; ++i)
	{
		dst[i] = text[rand() % (sizeof text - 1)];
	}
	dst[i] = '\0';
	return dst;
}


//^^^^^^^^^^^^^^^  MAIN  ^^^^^^^^^^^^^^^^^^^^  MAIN  ^^^^^^^^^^^^^^^^^  MAIN  ^^^^^^^^^^^^^^^^^   MAIN  ^^^^^^^^^^^^^^^^^
int main(int argc, char **argv)
{
	HPDF_Doc  pdf;
	char pdf_file[256];
	HPDF_Page page;
	HPDF_REAL height;
//	HPDF_REAL width;


	if (argc < 3) {
		printf("\nAdd [Tab File] [PDF File]\n\n\
	 	Example: Tab_To_PDF_Graph.exe tab_file.tab pdf_file.pdf\n\n");
		return 0;
	}

	strcpy(pdf_file, argv[2]);

	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) {
		printf("error: cannot create PdfDoc object\n");
		return 1;
	}

	if (setjmp(env)) {
		HPDF_Free(pdf);
		return 1;
	}
	/* Start with the building the page. */
	page = HPDF_AddPage(pdf);
	HPDF_Font font = HPDF_GetFont(pdf, "Courier", "WinAnsiEncoding");// Set Default Font


	if (argc >= 4) HPDF_SetInfoAttr(pdf, HPDF_INFO_AUTHOR, argv[3]);
	if (argc >= 5) HPDF_SetInfoAttr(pdf, HPDF_INFO_CREATOR, argv[4]);


///////////////////  FILE SECURITY  ////////////////////////////////////////////

	char owner_passwd[10];
	srand(time(0));
	rand_str(owner_passwd, sizeof owner_passwd);//Create random password
//	printf("Password Test: %s\n", owner_passwd);//For Testing
	HPDF_SetPassword(pdf, owner_passwd, "");
	HPDF_SetPermission(pdf, HPDF_ENABLE_PRINT | HPDF_ENABLE_COPY);
	HPDF_SetEncryptionMode(pdf, HPDF_ENCRYPT_R3, 16);


// **************** START GRAPH ***************** START GRAPH *************** START GRAPH ******************//



	FILE* log_file;
	char fileline[1024];
	int rec =0;
	char graph_topline[128];
	char cyc_num[5];
	char label[10];


	log_file = fopen(argv[1], "r");
	if (log_file == NULL) printf("Log Files %s NULL", argv[1]);
	else{
		while ((fgets(fileline, 1024, log_file)) && (rec < 2)){ // Retrieve each line in the file
			rec++;
			if (rec == 1) {
				strcpy(cyc_num, getfield(fileline, 1)); // Get Cycle Number
		}

	}
	fclose(log_file);
	sprintf(graph_topline, "CYCLE %s LIBHARU TREND GRAPH EXAMPLE", cyc_num);

	double x=0;
	double y = 700; // reference point for start of graph at top of page (left in landscape)
	double z =0;



/////////// TOP LINE CYCLE HEADER /////////////////////////

	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_LETTER, HPDF_PAGE_PORTRAIT);
	height = HPDF_Page_GetHeight(page);
	HPDF_Page_BeginText(page);
	HPDF_Page_MoveTextPos(page, 100, height - 25);
	font = HPDF_GetFont(pdf, "Helvetica", "WinAnsiEncoding");
	HPDF_Page_SetFontAndSize (page, font, 16);
	HPDF_Page_ShowText (page, graph_topline);
	HPDF_Page_EndText (page);



 /////////// KEY   /////////////////////////

	float angle1;
	float rad1;

	/* Set graph to Landscape */
	angle1 = 270;				   /* A rotation of 270 degrees. */
	rad1 = angle1 / 180 * 3.141592; /* Calcurate the radian value. */

	/* Key Fonts */
	font = HPDF_GetFont(pdf, "Helvetica", "WinAnsiEncoding");
	HPDF_Page_SetFontAndSize (page, font, 8);

	/* Key Start Location on Page */
	int offset = 80;
	int key_pos = 560;
	
   /* TE01 */
	HPDF_Page_SetLineWidth (page, 2);
	HPDF_Page_SetRGBStroke (page, 0, 0, 0);
	HPDF_Page_SetRGBFill (page, 0.0, 0.0, 0.5);

	HPDF_Page_Rectangle(page, key_pos, y-offset, 15, 15);
	HPDF_Page_Fill (page);

	HPDF_Page_BeginText (page);
	HPDF_Page_SetRGBFill (page, 0, 0, 0.5);
	HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),key_pos, y-offset-10);
	sprintf(label, "TE0 (%cC)", 176);
	label[8] = '\0';
	HPDF_Page_ShowText (page, label);
	HPDF_Page_EndText (page);
	offset = offset + 80;

	   /* TE01 */
	HPDF_Page_SetLineWidth (page, 2);
	HPDF_Page_SetRGBStroke (page, 0, 0, 0);
	HPDF_Page_SetRGBFill (page, 0.0, 0.5, 0.0); //

	HPDF_Page_Rectangle(page, key_pos, y-offset, 15, 15);
	HPDF_Page_Fill (page);

	HPDF_Page_BeginText (page);
	HPDF_Page_SetRGBFill (page, 0, 0.5, 0.0);
	HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),key_pos, y-offset-10);
	sprintf(label, "TE1 (%cC)", 176);
	label[8] = '\0';
	HPDF_Page_ShowText (page, label);
	HPDF_Page_EndText (page);
	offset = offset + 80;

	   /* TE02 */
	HPDF_Page_SetLineWidth (page, 2);
	HPDF_Page_SetRGBStroke (page, 0, 0, 0);
	HPDF_Page_SetRGBFill (page, 1.0, 0.0, 0.0); // Red

	HPDF_Page_Rectangle(page, key_pos, y-offset, 15, 15);
	HPDF_Page_Fill (page);

	HPDF_Page_BeginText (page);
	HPDF_Page_SetRGBFill (page, 1.0, 0.0, 0.0);
	HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),key_pos, y-offset-10);
	sprintf(label, "TE2 (%cC)", 176);
	label[8] = '\0';
	HPDF_Page_ShowText (page, label);
	HPDF_Page_EndText (page);
	offset = offset + 80;

		   /* PE07 */
	HPDF_Page_SetLineWidth (page, 2);
	HPDF_Page_SetRGBStroke (page, 0, 0, 0);
	HPDF_Page_SetRGBFill (page, 0.5, 0.5, 0.5); // Grey

	HPDF_Page_Rectangle(page, key_pos, y-offset, 15, 15);
	HPDF_Page_Fill (page);

	HPDF_Page_BeginText (page);
	HPDF_Page_SetRGBFill (page, 0.5, 0.5, 0.5);
	HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),key_pos, y-offset-10);
	HPDF_Page_ShowText (page, "PE3 (psia)");
	HPDF_Page_EndText (page);
	offset = offset + 80;



// -------------- Axis Lines -----------------  Axis Lines --------------- Axis Lines -----------//


/////// DRAW Y AXIS PRESSURE //////////////////////////////////////////////
	HPDF_Page_SetLineWidth (page, 1.0);
	HPDF_Page_SetRGBFill (page, 0.5, 0.5, 0.5);
	HPDF_Page_SetRGBStroke (page, 0.5, 0.5, 0.5);
	HPDF_Page_MoveTo (page, 90, y + 28	);
	HPDF_Page_LineTo (page, 540, y + 28);
	HPDF_Page_Stroke (page);



//// Add 0 through 75 labels

	int temp_lines =0;
	char temp_text[3];

	while (temp_lines < 451){ // While within the scale of the graph length
		if (temp_lines % 90 == 0){
			sprintf(temp_text, "%d", (temp_lines/90)*15);
			HPDF_Page_BeginText (page);
			HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),temp_lines + 90, y+40);
			HPDF_Page_ShowText (page, temp_text);
			HPDF_Page_SetRGBFill (page,0.5, 0.5, 0.5);
			HPDF_Page_EndText (page);
		}
		temp_lines++;
	}

	HPDF_Page_BeginText (page);
	HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),550, y+45);
	HPDF_Page_ShowText (page, "psia");
	HPDF_Page_SetRGBFill (page,0.5, 0.5, 0.5);
	HPDF_Page_EndText (page);




/////// DRAW Y AXIS TEMPERATURE   //////////////////////////////////////////////
	HPDF_Page_SetLineWidth (page, 1.0);
	HPDF_Page_SetRGBFill (page, 0, 0, 0);
	HPDF_Page_SetRGBStroke (page, 0, 0, 0);
	HPDF_Page_MoveTo (page, 90, y);
	HPDF_Page_LineTo (page, 540, y);
	HPDF_Page_Stroke (page);



///// Add 0 through 150 labels

	temp_lines =0;

	while (temp_lines < 451){ // While within the scale of the graph length
		if (temp_lines % 90 == 0){
			sprintf(temp_text, "%d", (temp_lines/30)*10);
			HPDF_Page_BeginText (page);
			HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),temp_lines + 90, y+17);
			HPDF_Page_ShowText (page, temp_text);
			HPDF_Page_EndText (page);

		}
		temp_lines++;
	}
	
	sprintf(label, "%cC", 176);
	label[2] = '\0';
	HPDF_Page_BeginText (page);
	HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),550, y+17);
	HPDF_Page_ShowText (page, label);
	HPDF_Page_EndText (page);



/////////// DRAW HORIZONTAL REFERENCE LINES ////////////////////////////////////////

	temp_lines =0;

	while (temp_lines < 451){ // While within the scale of the graph length
		if (temp_lines % 30 == 0){

		HPDF_Page_SetLineWidth (page, 1.0);
		HPDF_Page_SetRGBFill (page, 0, 0, 0);
		HPDF_Page_SetRGBStroke (page, 0.9, 0.9, 0.9);
		HPDF_Page_MoveTo (page, temp_lines + 90, y);
		HPDF_Page_LineTo (page, temp_lines + 90, y-660);
		HPDF_Page_Stroke (page);
		}
		temp_lines++;
	}

/////////// DRAW X AXIS ////////////////////////////////////////

	HPDF_Page_SetLineWidth (page, 1.0);
	HPDF_Page_SetRGBFill (page, 0, 0, 0);
	HPDF_Page_SetRGBStroke (page, 0.9, 0.9, 0.9);
	HPDF_Page_MoveTo (page, temp_lines + 90, y);
	HPDF_Page_LineTo (page, temp_lines + 90, y-660);
	HPDF_Page_Stroke (page);



	int act;
	char cell[64];//V6 changed to char

///// Determine total record count of tab/csv file and max time

	int hours =0;
	int minutes =0;
 	int lines=0;
	 rec = 0;

	log_file = fopen(argv[1], "r"); // Open tab File
		while (fgets(fileline, 1024, log_file)){ // Retrieve each line in the file
			rec++;
			if (rec > 5) { //Skip first 5 rows meant for cycle details
				strcpy(cell,getfield(fileline, 0));
				if (atoi(cell)==0){
					lines++;
					strcpy(cell,getfield(fileline, 1));
					hours = atoi(cell);
					strcpy(cell,getfield(fileline, 2));
					minutes = atoi(cell);
				}
			}
		}
	fclose(log_file); // Close tab File


//////////  Add Timeline labels

	int total_mins =0;
	int interval =0;
	int ons_minute =0;

	z = (660/(double)lines);
	act = 0;
	total_mins = (hours * 60)+ minutes;

	if (total_mins < 10) interval = 1;
	else if (total_mins < 20) interval = 5;
	else if (total_mins < 60) interval = 10;
	else interval = 20;

	log_file = fopen(argv[1], "r"); // Open tab File
	while (fgets(fileline, 1024, log_file)){ // Retrieve each line in the file
		rec++;
		if (rec > 5) { //Skip first 5 rows meant for cycle details
			strcpy(cell,getfield(fileline, 0));
			if (atoi(cell)==0){
				lines++;
				strcpy(cell,getfield(fileline, 1));
				hours = atoi(cell);
				strcpy(cell,getfield(fileline, 2));
				minutes = atoi(cell);
				if (ons_minute == 0 && (((hours * 60) + minutes) % interval == 0)){
					ons_minute++;
					sprintf(temp_text, "%d:%d", hours, minutes);

					HPDF_Page_BeginText (page);
					HPDF_Page_SetTextMatrix (page, cos(rad1), sin(rad1), -sin(rad1), cos(rad1),80, y - (z*(double)act) );
					HPDF_Page_ShowText (page, temp_text);
					HPDF_Page_EndText (page);
				}
				if (((hours * 60) + minutes) % interval != 0) ons_minute = 0;
				act++;
			}
		}
	}
	fclose(log_file); // Close tab File



/* -------  Trend Lines -----------------  Trend Lines -------------------  Trend Lines ------------------ 
* 
* FOR EACH PEN, THE LIBHARU LINE DRAWING TOOL WILL DRAW A STRAIGHT LINE FROM ONE POINT TO ANOTHER.
* THE TAB FILE IS OPENED AND SCANNED ONE LINE (RECORD) AT A TIME. AS TAB EACH LINE IS SCANNED AND PARSED 
* INTO SEPARATE VALUES, THE VALUE IS SCALED TO A REFERENCE POINT ON THE PAGE AND USED AS A START AND END 
* POINT FOR THE DRAWING OF THE LINE. EACH RECORD IN THIS DATABASE HAS A CODE NUMBER IN THE FIRST COLUMN.
* A CODE NUMBER 0 INDICATES A NORMAL TIMED INTERVAL WITTEN BY THE LOGGING SYSTEM THAT
* CREATED THE DATABASE. THIS INTERVAL TRIGGERS A NEW LINE DRAW. 
* AFTER THE FILE SCAN IS COMPLETE, THE MANY LINE DRAWINGS WILL COMPILE
* A TREND LINE FOR ONE COLUMN OF VALUES. THIS IS REPEATED 4 TIMES IN THIS EXAMPLE.											
*/

	char *ptr;

	/* Default Line Attributes */
	HPDF_Page_SetLineWidth (page, 1.0);
	HPDF_Page_SetRGBFill (page, 0, 0, 0);

	////////////////  LINE 1   (TE0)


	log_file = fopen(argv[1], "r"); // Open tab File

	HPDF_Page_SetRGBStroke (page, 0, 0, 0.5); // Blue

		rec =0;
		act =0;

		while (fgets(fileline, 1024, log_file)){ // Retrieve each line in the file
			rec++;
			if (rec > 5) { //Skip first 5 rows meant for cycle details
				strcpy(cell,getfield(fileline, 0)); // Get interval trigger code from first column
				if(atoi(cell)==0){
					strcpy(cell,getfield(fileline, 8));
					x = (3*strtod(cell, &ptr)) +90;
					if (act==0) HPDF_Page_MoveTo (page, x, y);
					else if (act < (lines -1)){
						HPDF_Page_LineTo (page, x, y - (z*(double)act));
					}
					act++;
				}
			}
		}
	HPDF_Page_Stroke (page);
	fclose(log_file); // Close tab File

	////////////////  LINE 2 (TE1)


	log_file = fopen(argv[1], "r"); // Open tab File

	HPDF_Page_SetRGBStroke (page, 0, 0.5, 0); // Green

		rec =0;
		act =0;

		while (fgets(fileline, 1024, log_file)){ // Retrieve each line in the file
			rec++;
			if (rec > 5) { //Skip first 5 rows meant for cycle details
				strcpy(cell,getfield(fileline, 0)); // Get interval trigger code from first column
				if(atoi(cell)==0){
					strcpy(cell,getfield(fileline, 9));
					x = (3*strtod(cell, &ptr)) +90;
					if (act==0) HPDF_Page_MoveTo (page, x, y);
					else if (act < (lines -1)){
						HPDF_Page_LineTo (page, x, y - (z*(double)act));
					}
					act++;
				}
			}
		}
	HPDF_Page_Stroke (page);
	fclose(log_file); // Close tab File


	////////////////  LINE 3 (TE2)


	log_file = fopen(argv[1], "r"); // Open tab File

	HPDF_Page_SetRGBStroke (page, 1, 0, 0); // Red

		rec =0;
		act =0;

		while (fgets(fileline, 1024, log_file)){ // Retrieve each line in the file
			rec++;
			if (rec > 5) { //Skip first 5 rows meant for cycle details
				strcpy(cell,getfield(fileline, 0)); // Get interval trigger code from first column
				if(atoi(cell)==0){
					strcpy(cell,getfield(fileline, 10));
					x = (3*strtod(cell, &ptr)) +90;
					if (act==0) HPDF_Page_MoveTo (page, x, y);
					else if (act < (lines -1)){
						HPDF_Page_LineTo (page, x, y - (z*(double)act));
					}
					act++;
				}
			}
		}
	HPDF_Page_Stroke (page);
	fclose(log_file); // Close tab File

	////////////////  LINE 4 (PE3)


	log_file = fopen(argv[1], "r"); // Open tab File

	HPDF_Page_SetRGBStroke (page, 0.5, 0.5, 0.5); // Grey

		rec =0;
		act =0;

		while (fgets(fileline, 1024, log_file)){ // Retrieve each line in the file
			rec++;
			if (rec > 5) { //Skip first 5 rows meant for cycle details
				strcpy(cell,getfield(fileline, 0)); // Get interval trigger code from first column
				if(atoi(cell)==0){
					strcpy(cell,getfield(fileline, 15));
					x = (6*strtod(cell, &ptr)) +90;
					if (act==0) HPDF_Page_MoveTo (page, x, y);
					else if (act < (lines -1)){
						HPDF_Page_LineTo (page, x, y - (z*(double)act));
					}
					act++;
				}
			}
		}
	HPDF_Page_Stroke (page);
	fclose(log_file); // Close tab File

// *********** END GRAPH ***************** END GRAPH ******************* END GRAPH **************** END GRAPH *********//
	}



	HPDF_SaveToFile(pdf, pdf_file);
	/* clean up */
	HPDF_Free(pdf);
	return 0;
}

