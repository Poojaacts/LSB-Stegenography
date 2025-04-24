#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"





/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp") !=NULL)		// to check argv[2] is .bmp file or not
	decInfo ->enc_image_fname=argv[2];
    else
    {
	printf("Error:The extension for encoded image is invalid.It should be .bmp\n");
	return e_failure;
    }
    if(argv[3] !=NULL)
    {
	if((strchr(argv[3],'.'))!=NULL)  
	{
	    char *name=strtok(argv[3],".");

	    strcpy(decInfo ->output_image_fname,name);
	    return e_success;

	}

	else
	{

	    strcpy(decInfo ->output_image_fname,argv[3]);	//string of argv[3] is coping to the Storing in Structure
	    return e_success;
	}
    }
    else
	strcpy(decInfo ->output_image_fname,"output");	//Default file name if out_filename is not present
    return e_success;
}




Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->enc_image_fname, "r");	// Src Image file
        if (decInfo->fptr_src_image == NULL)	// Do Error handling

    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open the encoded file %s\n", decInfo->enc_image_fname);

	return e_failure;
    }

    // Secret file
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo) ==e_success)
    {
	printf("Encoded File opened successfully.\n");
    }
    else
    {
	printf("Unable to open the encoded file.\n");
	return e_failure;
    }

    if(decode_magic_string(MAGIC_STRING,decInfo)==e_success)
    {
	printf("Magic String decoding is done successfully.\n");
    }

    else
    {
	printf("Error:Failed to decode the Magic String.\n");
	return e_failure;
    }
    if(decode_secret_file_extn_size(decInfo ->size_secret_file_extn,decInfo) ==e_success)
    {
	printf("Secret File extension size is decoded successfully.\n");
    }

    else
    {
	printf("Error:Failed to decode Secret File Extension Size.\n");
	return e_failure;
    }
    if(decode_secret_file_extn(decInfo ->output_image_fname,decInfo) == e_success)
    {
	printf("Secret file extension is decoded successfully.\n");
    }
    else
    {
	printf("Error:Failed to decode Secret File Extension.\n");
	return e_failure;
    }
    if(open_files_again(decInfo) == e_success)
    {
	printf("The Output file is opened successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(decInfo->fptr_src_image), ftell(decInfo->fptr_out_image));
    }
    else
    {
	printf("Error:Failed to open the Output File.\n");
	return e_failure;
    }
    if(decode_secret_file_size(decInfo ->size_secret_file,decInfo)==e_success)
    {
	printf("Secret File size is decoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(decInfo->fptr_src_image), ftell(decInfo->fptr_out_image));
    }
    else
    {
	printf("Error:Failed to decode Secret File size.\n");
	return e_failure;
    }
    if(decode_secret_file_data(decInfo)==e_success)
    {
	printf("Secret File data is decoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(decInfo->fptr_src_image), ftell(decInfo->fptr_out_image));
    }
    else
    {
	printf("Error:Failed to decode Secret File data.\n");
	return e_failure;
    }
    if(close_file(decInfo)==e_success)
    {
	printf("The Encoded and Output files are closed successfully.\n");
    }
    else
    {
	printf("Error: Failed to close the Output and Encoded Files.\n");
	return e_failure;
    }
    return e_success;
}

Status decode_magic_string(const char* magic_string,DecodeInfo *decInfo)
{
    fseek(decInfo ->fptr_src_image,54,SEEK_CUR);	//Seeking the cursor to the 54th byte
    char buffer_magic[8];
    char ch=0;
    char data[3];
    int i;
    for(i=0;i<2;i++)
    {
	fread(buffer_magic,1,8,decInfo ->fptr_src_image);	//reading the 8 bytes 
	data[i]=decode_lsb_to_byte(&ch,buffer_magic);
    }
    data[i]='\0';
    //    printf("ms = %s\n", data);
    if((strcmp(data,"#*"))==0)
    {
	printf("The magic string has been decoded successfully.\n");
	return e_success;
    }
    else
    {
	printf("The Decoded value of Magic String is invalid.\n");
	return e_failure;
    }
}
Status decode_lsb_to_byte(char *ch,char *image_buffer)
{
    *ch=0;

    for(int i=0;i<8;i++)
    {
	*ch=*ch |((image_buffer[i] & 1) <<(7-i));
    }
    //  printf("ch = %c\n", *ch);
    return *ch;
}

Status decode_size_to_lsb(int *size,char *image_buffer)
{
    *size = 0;

    for (int i = 0; i < 32; i++)
    {
	*size =*size | ((image_buffer[i] & 1) << (31 - i));
    }
    // decInfo ->size_secret_file=size;

    return e_success;
}
Status decode_secret_file_extn_size(long size_secret_file,DecodeInfo *decInfo)
{
    char buffer[32];
    int size;

    fread(buffer,1,32,decInfo ->fptr_src_image);

    decode_size_to_lsb(&size,buffer);

    decInfo ->size_secret_file_extn=size;
    return e_success;
}

Status decode_secret_file_extn(const char *file_extn,DecodeInfo *decInfo)
{
    char buffer[8];
    char extn[decInfo ->size_secret_file_extn + 1];
    char ch=0;
    int i;
    //printf("ext size = %d\n", decInfo->size_secret_file_extn);
    for( i=0;i<decInfo ->size_secret_file_extn;i++)
    {
	fread(buffer,1,8,decInfo ->fptr_src_image);

	extn[i]=decode_lsb_to_byte(&ch,buffer);
    }
    extn[i]='\0';

    if((strchr(extn,'.'))!=NULL)
    {
	strcat(decInfo ->output_image_fname,extn);
	printf("The extension for Output File name been added successfully.\n");
	return e_success;
    }
    else
    {
	printf("The File extension could not be found in Encoded File.\n");
	return e_failure;
    }
    return e_success;
}

Status open_files_again(DecodeInfo *decInfo)
{
    decInfo->fptr_out_image = fopen(decInfo->output_image_fname, "w"); 	//Opening the output file in write mode
    
    if (decInfo->fptr_out_image == NULL)	// Do Error handling	
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open the encoded file %s\n", decInfo->output_image_fname);

	return e_failure;
    }
    return e_success;
}
Status decode_secret_file_size(long size_secret_file,DecodeInfo *decInfo)	//Decode the secret file size
{
    char buffer[32];
    int size=0;

    fread(buffer,1,32,decInfo ->fptr_src_image);

    decode_size_to_lsb(&size,buffer);

    decInfo ->size_secret_file=size;

    return e_success;
}


Status decode_secret_file_data(DecodeInfo *decInfo)	//Decode the secret file data
{
    char buffer[8];

    char temp;
    for(int i = 0;i<decInfo ->size_secret_file;i++)
    {
	fread(buffer,1,8,decInfo ->fptr_src_image);

	decode_lsb_to_byte(&temp,buffer);

	//      printf("temp = %c\n", temp);
	fwrite(&temp,1,1,decInfo ->fptr_out_image);
    }
    return e_success;
}
Status close_file(DecodeInfo *decInfo) 		//Close the opened file
{
    fclose(decInfo ->fptr_src_image);
    fclose(decInfo ->fptr_out_image);

    return e_success;
}



