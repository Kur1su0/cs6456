/*
 * Copyright (c) 2017, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <aes_ta.h>


#define AES_TEST_BUFFER_SIZE	4096
#define AES_TEST_KEY_SIZE	16
#define AES_BLOCK_SIZE		16

#define DECODE			0
#define ENCODE			1


#define ENC			427
#define DECT			948

/* TEE resources */
struct test_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};

void prepare_tee_session(struct test_ctx *ctx)
{
	TEEC_UUID uuid = TA_AES_UUID;
	uint32_t origin;
	TEEC_Result res;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
}

void terminate_tee_session(struct test_ctx *ctx)
{
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}

void prepare_aes(struct test_ctx *ctx, int encode)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT,
					 TEEC_NONE);

	op.params[0].value.a = TA_AES_ALGO_CTR;
	op.params[1].value.a = TA_AES_SIZE_128BIT;
	op.params[2].value.a = encode ? TA_AES_MODE_ENCODE :
					TA_AES_MODE_DECODE;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_PREPARE,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(PREPARE) failed 0x%x origin 0x%x",
			res, origin);
}

void set_key(struct test_ctx *ctx, char *key, size_t key_sz)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE, TEEC_NONE, TEEC_NONE);

	op.params[0].tmpref.buffer = key;
	op.params[0].tmpref.size = key_sz;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_SET_KEY,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY) failed 0x%x origin 0x%x",
			res, origin);
}

void set_iv(struct test_ctx *ctx, char *iv, size_t iv_sz)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					  TEEC_NONE, TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = iv;
	op.params[0].tmpref.size = iv_sz;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_SET_IV,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_IV) failed 0x%x origin 0x%x",
			res, origin);
}

void cipher_buffer(struct test_ctx *ctx, char *in, char *out, size_t sz)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = in;
	op.params[0].tmpref.size = sz;
	op.params[1].tmpref.buffer = out;
	op.params[1].tmpref.size = sz;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_CIPHER,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(CIPHER) failed 0x%x origin 0x%x",
			res, origin);
}

#define MAXSIZE 200

typedef struct img_struct{
	int height;
	int weight;
	int channel;
	unsigned char* img;
}img_struct;

char* read_image(char filepath[MAXSIZE], int* file_size){
	FILE* fp = NULL;
	fp = fopen (filepath, "rb");
	if(fp==NULL) return NULL;
	
	//img_struct* img_ptr= (img_struct**)calloc(1,sizeof(img_struct*));
	
	fseek(fp,0,SEEK_END);
	*file_size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	

	//char* img = (char**)calloc(1,(*file_size)* sizeof(char*));
	char* img =(char*)calloc(1,(*file_size)* sizeof(char));
	if(img == NULL){
		fclose(fp);
		return NULL;
	}
	
	fread(img,1,(*file_size),fp);
	fclose(fp);
	return img;
	

}

TEEC_Result read_secure_object(struct test_ctx *ctx, char *id,
			char *data, size_t data_len)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	size_t id_len = strlen(id);

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);

	op.params[0].tmpref.buffer = id;
	op.params[0].tmpref.size = id_len;

	op.params[1].tmpref.buffer = data;
	op.params[1].tmpref.size = data_len;

	res = TEEC_InvokeCommand(&ctx->sess,
				 TA_SECURE_STORAGE_CMD_READ_RAW,
				 &op, &origin);
	switch (res) {
	case TEEC_SUCCESS:
		printf("------obj--id data:%s\n",(char*)op.params[0].tmpref.buffer);
		printf("------obj--data data:%s\n",(char*)op.params[1].tmpref.buffer);
	case TEEC_ERROR_SHORT_BUFFER:
	case TEEC_ERROR_ITEM_NOT_FOUND:
		break;
	default:
		printf("Command READ_RAW failed: 0x%x / %u\n", res, origin);
	}

	return res;
}

TEEC_Result write_secure_object(struct test_ctx *ctx, char *id,
			char *data, size_t data_len)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	size_t id_len = strlen(id);

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE, TEEC_NONE);

	op.params[0].tmpref.buffer = id;
	op.params[0].tmpref.size = id_len;

	op.params[1].tmpref.buffer = data;
	op.params[1].tmpref.size = data_len;

	res = TEEC_InvokeCommand(&ctx->sess,
				 TA_SECURE_STORAGE_CMD_WRITE_RAW,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		printf("Command WRITE_RAW failed: 0x%x / %u\n", res, origin);

	switch (res) {
	case TEEC_SUCCESS:
		break;
	default:
		printf("Command WRITE_RAW failed: 0x%x / %u\n", res, origin);
	}

	return res;
}


TEEC_Result delete_secure_object(struct test_ctx *ctx, char *id)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	size_t id_len = strlen(id);

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE, TEEC_NONE, TEEC_NONE);

	op.params[0].tmpref.buffer = id;
	op.params[0].tmpref.size = id_len;

	res = TEEC_InvokeCommand(&ctx->sess,
				 TA_SECURE_STORAGE_CMD_DELETE,
				 &op, &origin);

	switch (res) {
	case TEEC_SUCCESS:
	case TEEC_ERROR_ITEM_NOT_FOUND:
		break;
	default:
		printf("Command DELETE failed: 0x%x / %u\n", res, origin);
	}

	return res;
}

void run_detection(struct test_ctx *ctx, char *in, char *out, size_t sz){
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = in;
	op.params[0].tmpref.size = sz;
	op.params[1].tmpref.buffer = out;
	op.params[1].tmpref.size = sz;


	//char obj1_id[10] = "ENCimg";		/* string identification for the object */
	
	//res = write_secure_object(&ctx, obj1_id, in, sizeof(in));
	//if (res != TEEC_SUCCESS)
	//	errx(1, "Failed to create an object in the secure storage");

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_DETECT,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(CIPHER) failed 0x%x origin 0x%x",
			res, origin);
}





int main(int argc, char* argv[])
{
	struct test_ctx ctx;
	//char key[AES_TEST_KEY_SIZE];
	//char iv[AES_BLOCK_SIZE];
	//char clear[AES_TEST_BUFFER_SIZE];
	//char ciph[AES_TEST_BUFFER_SIZE];
	//char temp[AES_TEST_BUFFER_SIZE];
	int file_size = 0,key_size;
	char filepath[MAXSIZE],outpath[MAXSIZE],key_buffer[MAXSIZE],opt_cmd[MAXSIZE];
	int what_opt = 0;

	printf("Prepare session with the TA\n");
	prepare_tee_session(&ctx);
	
	if(argc!=5){
		printf("wrong argc, need 4: [enc/dect] [in] [out] [key]\n");
		//printf("wrong argc, need 3\n");
		return 1;

	}
//arg IO
	sprintf(opt_cmd, "%s",argv[1]);
	if( strcmp("dect",opt_cmd) ==0  ){
		what_opt = DECT;
		printf("MODE: %s\n",opt_cmd);
	}else if( strcmp("enc",opt_cmd)==0){
		what_opt = ENC;
		printf("MODE: %s\n",opt_cmd);

	}else{
		printf("wrong opt cmd %s\n",opt_cmd);
		return -1;
	}

	sprintf(filepath,"%s",argv[2]);
	sprintf(outpath,"%s",argv[3]);
	sprintf(key_buffer,"%s",argv[4]);
	key_size = strlen(key_buffer);






	printf("load img at: %s\n",filepath);
	printf("output img at: %s\n",outpath);
	printf("key: %s\n",key_buffer);
	printf("key size: %d\n",key_size);
	char* img = read_image(filepath,&file_size);

	if(img != NULL){
		printf("load succeed\n");
	}

	printf("filesize :%d\n",file_size);
//


	//prepare buffer
	char ciph[file_size];//,temp[file_size];
	char iv[key_size];
	//char key[key_size];
	char *clear=img;



	if(what_opt == DECT){
		//doing dection
		printf("Prepare dect operation\n");
		//prepare_aes(&ctx, ENCODE);
		prepare_aes(&ctx,DECODE);

		printf("Load key in TA\n");
		//memset(key, 0xa5, sizeof(key)); /* Load some dummy value */
		set_key(&ctx, key_buffer, key_size);

		printf("Reset ciphering operation in TA (provides the initial vector)\n");
		memset(iv, 0, sizeof(iv)); /* Load some dummy value */
		set_iv(&ctx, iv, key_size);



		
		printf("init storage");
		char obj1_id[10] = "ENCimg";		/* string identification for the object */
	
		TEEC_Result res;
		res = write_secure_object(&ctx, obj1_id, img, file_size);
		if (res != TEEC_SUCCESS)
			errx(1, "Failed to create an object in the secure storage");
		//memset(clear, 0x5a, sizeof(clear)); /* Load some dummy value */
		printf("Encode buffer from TA\n");
		run_detection(&ctx, clear, ciph,file_size);

		printf("encrypted plate result (saved): %s\n",ciph);
		FILE* fp = NULL;
		if ( (fp=fopen(outpath,"wb+"))==NULL){
			printf("Err opening file\n");
		}
		fwrite((unsigned char*)ciph, file_size, 1, fp );
		fclose(fp);
	
		terminate_tee_session(&ctx);
		return 0;
	}



	printf("Prepare encode operation\n");
	//prepare_aes(&ctx, ENCODE);
	prepare_aes(&ctx,ENCODE);

	printf("Load key in TA\n");
	//memset(key, 0xa5, sizeof(key)); /* Load some dummy value */
	set_key(&ctx, key_buffer, key_size);

	printf("Reset ciphering operation in TA (provides the initial vector)\n");
	memset(iv, 0, sizeof(iv)); /* Load some dummy value */
	set_iv(&ctx, iv, key_size);



	printf("Encode buffer from TA\n");
	//memset(clear, 0x5a, sizeof(clear)); /* Load some dummy value */
	//
	//SAVE CIPHER IMAGE
	cipher_buffer(&ctx, clear, ciph, file_size);
	FILE* fp = NULL;
	if ( (fp=fopen(outpath,"wb+"))==NULL){
		printf("Err opening file\n");
	}
	fwrite((unsigned char*)ciph, file_size, 1, fp );
	fclose(fp);


	/*

	printf("Prepare encode operation\n");
	//prepare_aes(&ctx, DECODE);
	prepare_aes(&ctx, ENCODE);

	printf("Load key in TA\n");
	//memset(key, 0xa5, sizeof(key));
	set_key(&ctx, key, key_size);







	printf("Reset ciphering operation in TA (provides the initial vector)\n");
	memset(iv, 0, sizeof(iv)); // Load some dummy value 
	set_iv(&ctx, iv, key_size);

	printf("Decode buffer from TA\n");
	cipher_buffer(&ctx, ciph, temp, file_size);
	*/
	/*
	FILE* fp = NULL;
	if ( (fp=fopen(outpath,"wb+"))==NULL){
		printf("Err opening file\n");
	}
	fwrite((unsigned char*)temp, file_size, 1, fp );
	fclose(fp);
	*/

	//sprintf(out_path,"%s","out.jpg");
	//write_img(temp, file_size, out_path);

	/* Check decoded is the clear content */
	//if (memcmp(clear, temp, file_size))
	//	printf("Clear text and decoded text differ => ERROR\n");
	//else
	//	printf("Clear text and decoded text match\n");

	//
	//clean up
	free(img);
	img = NULL;
	terminate_tee_session(&ctx);
	return 0;
}