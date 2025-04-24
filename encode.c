#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo) 	//To read and validate the CLA arguements are correct or not 
{
    if(strstr(argv[2],".bmp") !=NULL)			//checking in argv[2] is .bmp file or not
	encInfo ->src_image_fname=argv[2];		//storing the file name in Stucture
    else
    {
	printf("Error:The extension for source image is invalid.Extension should be .bmp\n");	//print error
	return e_failure;
    }
    if(strrchr(argv[3],'.'))			//checking file name and storing the file name
	encInfo ->secret_fname=argv[3];
    else
    {
	printf("Error:The extension for secret image is missing\n");	//print error
	return e_failure;
    }
    char *extn = strrchr(argv[3], '.'); 	//To the extn 
    if (extn != NULL)
    {
	strcpy(encInfo->extn_secret_file, extn); // Skip the dot
    }
    else
    {
	printf("Error:There is no extension for Secret file\n"); 	//print error
	return e_failure;
    }
    if(argv[4]!=NULL)
    {
	if(strstr(argv[4],".bmp")) 	//checking that arg[4] has .bmp or not
	{
	    encInfo->stego_image_fname=argv[4];	  //storing the file name in Structure
	}
	else
	{
	    printf("Error:The file type passed for encoding is invalid\n");	//error
	    return e_failure;
	}
    }

    encInfo->stego_image_fname="stego.bmp";   //Default file name if out_file is given 
    return e_success;
}




Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
	perror("fopen");    //Printing error as compiler does
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

	return e_failure;
    }

    	// Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
	perror("fopen");	//Printing error as compiler does
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
	perror("fopen");	////Printing error as compiler does
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) ==e_success)
    {
	printf("File opened successfully.n");
    }
    else
    {
	printf("Unable to open the file.n");
	return e_failure;
    }
    if(check_capacity(encInfo)==e_success)
    {
	printf("The capacity is checked successfully.n");
    }
    else
    {
	printf("Error:Failed to check the capacity.n");
	return e_failure;
    }
    if(copy_bmp_header(encInfo ->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
	printf("The bmp header is copied successfully.n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }
    else
    {
	printf("Error:Failed to copy the header file.n");
	return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
	printf("Magic String is encoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }

    else
    {
	printf("Error:Failed to encode the Magic String.\n");
	return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(encInfo ->extn_secret_file),encInfo) ==e_success)
    {
	printf("Secret File extension size is encoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }

    else
    {
	printf("Error:Failed to encode Secret File Extension Size.\n");
	return e_failure;
    }
    if(encode_secret_file_extn(encInfo ->extn_secret_file,encInfo) == e_success)
    {
	printf("Secret file extension is encoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }
    else
    {
	printf("Error:Failed to encode Secret File Extension.\n");
	return e_failure;
    }
    if(encode_secret_file_size(encInfo ->size_secret_file,encInfo)==e_success)
    {
	printf("Secret File size is encoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }
    else
    {
	printf("Error:Failed to encode Secret File size.\n");
	return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)
    {
	printf("Secret File data is encoded successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }
    else
    {
	printf("Error:Failed to encode Secret File data\n");
	return e_failure;
    }
    if(copy_remaining_img_data(encInfo ->fptr_src_image, encInfo ->fptr_stego_image)==e_success)
    {
	printf("Remaining RGB data of beautiful.bmp is copied successfully.\n");
	printf("src = %ld\ndest = %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));
    }
    else
    {
	printf("Error:Failed to copy the remaining RGB data.\n");
	return e_failure;
    }



    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)  //checking the size the file so that image capacity should be greater than input filr.
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo -> size_secret_file = get_file_size(encInfo->fptr_secret);

    if(((strlen(MAGIC_STRING)*8)+32+(strlen(encInfo ->extn_secret_file)*8) +32 +(encInfo ->size_secret_file * 8)) < encInfo->image_capacity)
	return e_success;
    else
	return e_failure;
}
uint get_file_size(FILE *fptr)
{
    //move the fptr to the end of the file
    //return ftell(fptr)
    fseek(fptr,0,SEEK_END);    	//Seeking the cursor to the end
    return ftell(fptr);
}
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)	//Coping the 54 bytes header as it is
{
    char buffer[54];
    fseek(fptr_src_image,0,SEEK_SET); 	//Seeking the cursor to the beggining of the file

    fread(buffer,1,54,fptr_src_image);	//reading first 54 byte from src image and storing it in buffer

    fwrite(buffer,1,54,fptr_dest_image);	//Writing the 54 byte to thr dest image

    return e_success;
}
Status encode_magic_string(const char* magic_string,EncodeInfo *encInfo)
{
    char buffer_magic[8];
    int i;
    for(i=0;i<strlen(MAGIC_STRING);i++)
    {
	fread(buffer_magic,1,8,encInfo ->fptr_src_image);	//Storing the magic string data into the maigic buffer

	encode_byte_to_lsb(MAGIC_STRING[i],buffer_magic);

	fwrite(buffer_magic,1,8,encInfo ->fptr_stego_image);	//Writing the information into output file 
    }
    return e_success;
}
Status encode_byte_to_lsb(char data,char *image_buffer)
{

    for(int i=0;i<8;i++)
    {
	image_buffer[i]=(image_buffer[i] & 0xfe) |((data &(1<<(7-i)))>>(7-i));		//Doing encoding
    }
    return e_success;
}

Status encode_size_to_lsb(int size,char *image_buffer)
{
    for(int i=0;i<=31;i++)
    {
	image_buffer[i]=(image_buffer[i] &0xfe) |((size &(1<<(31-i)))>>(31-i));
    }
    return e_success;
}
Status encode_secret_file_extn_size(long size_secret_file,EncodeInfo *encInfo)		//Extracting the secret file extn and finding the size
{
    char buffer[32];


    fread(buffer,1,32,encInfo ->fptr_src_image);	

    encode_size_to_lsb(strlen(encInfo ->extn_secret_file),buffer);

    fwrite(buffer,1,32,encInfo ->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)	//Encoding the exten
{
    char buffer[8];
    for(int i=0;i<strlen(encInfo ->extn_secret_file);i++)
    {
	fread(buffer,1,8,encInfo ->fptr_src_image);

	encode_byte_to_lsb((encInfo ->extn_secret_file[i]),buffer);

	fwrite(buffer,1,8,encInfo ->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long size_secret_file,EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer,1,32,encInfo ->fptr_src_image);	//reading the 32byte from source image
    encode_size_to_lsb(size_secret_file,buffer);	
    fwrite(buffer,1,32,encInfo ->fptr_stego_image);	//Writing the 32 byte to stego image
    return e_success;
}

  
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[8];
    fseek(encInfo ->fptr_secret,0,SEEK_END);	//Seeking the cursor position to end        
    long int size=ftell(encInfo ->fptr_secret);		
    char temp[size];
    rewind(encInfo->fptr_secret);
    fread(temp, 1, size, encInfo->fptr_secret);
    for(int i = 0;i<size;i++)
    {
	fread(buffer,1,8,encInfo ->fptr_src_image); 	

	encode_byte_to_lsb(temp[i],buffer);

	fwrite(buffer,1,8,encInfo ->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest) 	//To Copy the remaining data source file to the destination file
{
    //long int non_encoded=ftell(fptr_src);
    char buffer;
    while(fread(&buffer,1,1,fptr_src) !=0)
	fwrite(&buffer,1,1,fptr_dest); 		   
    return e_success;
}

