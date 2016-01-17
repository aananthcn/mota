/*
 * sotajni.c : The interface file from Java world to SOTA client's C world
 *
 *  Created on: Jan 8, 2016
 *      Author: caananth
 */
#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <pthread.h>

#include "jansson.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

static JavaVM *java_vm;
static jobject this_obj;
static jmethodID set_message_method_id;


#define VEH_CONF_MAX 6
char SotaConf[VEH_CONF_MAX][256];
extern void main(int, char**);

#if 0
extern void stop_wikiclient(void);
extern void new_location_data(double lat, double lon);
extern char *get_ui_message(void);
#endif


/*##########################################################################
 * Functions
 *##########################################################################*/
void set_ui_message(const char *msg) {
	char lmsg[4*1024];
	JNIEnv *env;

	strcpy(lmsg, msg);
	if((*java_vm)->GetEnv(java_vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		__android_log_print (ANDROID_LOG_ERROR, "MOTA",
				"Could not retrieve JNIEnv");
		return;
	}
	jstring jmessage = (*env)->NewStringUTF(env, lmsg);

	__android_log_print(ANDROID_LOG_INFO, "MOTA", "%s(): %s", __func__, lmsg);

	(*env)->CallVoidMethod(env, this_obj, set_message_method_id, jmessage);
	if ((*env)->ExceptionCheck(env)) {
		(*env)->ExceptionClear(env);
	}

	(*env)->DeleteLocalRef(env, jmessage);
}


#if 0
int sj_add_string(json_t **root, char *name, char *value)
{
        json_t *new;

        new = json_pack("{\nss\n}", name, value);
        if(new  == NULL)
                return -1;

        return json_object_update(*root, new);
}


/*************************************************************************
 * function: sj_store_file
 *
 * This function stores the data in RAM pointed by json_t* to the storage
 * media path passed as argument
 *
 * arg1: file path
 * arg2: json_t pointer
 *
 * return: positive or negative number
 */
int sj_store_file(json_t *root, char *file)
{
        int flags, ret;

        if(!file) {
                __android_log_print(ANDROID_LOG_ERROR, "MOTA",
                		"%s(), invalid file passed!\n", __FUNCTION__);
                return -1;
        }

        /* dump to a temporary file */
        flags = JSON_INDENT(8);
        ret = json_dump_file(root, file, flags);
        if(ret < 0) {
                __android_log_print(ANDROID_LOG_ERROR, "MOTA",
                		"%s(), json_dump failed!\n", __FUNCTION__);
                return -1;
        }

        __android_log_print(ANDROID_LOG_INFO, "MOTA", "Successfully stored %s", file);

        return 1;
}


void create_config_json_file(char *str)
{
	json_t *jp;

	jp = json_object();

        sj_add_string(&jp, "vin", VehConf[0]);
        sj_add_string(&jp, "name", VehConf[1]);
        sj_add_string(&jp, "phone", VehConf[2]);
        sj_add_string(&jp, "email", VehConf[3]);

        sj_store_file(jp, str);
        json_decref(jp);

        __android_log_print(ANDROID_LOG_INFO, "MOTA", "executing %s", __func__);
}
#endif


void receive_sota_configs(JNIEnv *env, jobject thiz, jint len, jobjectArray stringArray)
{
	int i;
	jstring jstr;
	const char *nstr;

	__android_log_print (ANDROID_LOG_INFO, "MOTA", "Received %d SOTA Configurations", len);
	for(i = 0; i < len; i++) {
		jstr = (*env)->GetObjectArrayElement(env, stringArray, i);
		nstr = (*env)->GetStringUTFChars(env, jstr, NULL);
		strcpy(SotaConf[i], nstr);
		__android_log_print(ANDROID_LOG_INFO, "MOTA", "  %d) %s", i, SotaConf[i]);
		(*env)->ReleaseStringUTFChars(env, jstr, nstr);
	}

	//create_config_json_file(VEH_CONFIG_FILE);
}



#define USE_FORK_IN_ANDROID 	0
#define CMDLINE_ARGS 		11
/******************************************************************************
 * MOTA NDK main function
 */
void sotajni_main(JNIEnv *env, jobject thiz)
{
	char **argv;
	int argc, i;
	pid_t pid;

	this_obj = thiz;

#if USE_FORK_IN_ANDROID
	if((pid = fork()) == -1) {
		__android_log_print(ANDROID_LOG_ERROR, "MOTA",
				"Could not fork wikijni_main");
		return;
	}
#else
	pid = 0;
#endif

	if(pid == 0) {
		/* child process */
		argv = malloc(CMDLINE_ARGS*sizeof(char*));
		for(i = 0; i < CMDLINE_ARGS; i++)
			argv[i] = malloc(128);

		strcpy(argv[0], "motaclient");
		strcpy(argv[1], "-i");
		strcpy(argv[2], SotaConf[2]); /* client_info.json */
		strcpy(argv[3], "-s");
		strcpy(argv[4], SotaConf[3]); /* Server IP */
		strcpy(argv[5], "-t");
		strcpy(argv[6], SotaConf[0]); /* tmp files folder */
		strcpy(argv[7], "-p");
		strcpy(argv[8], SotaConf[1]); /* main storage folder */
		strcpy(argv[9], "-c");
		sprintf(argv[10], "%ld", (long int)((void *)set_ui_message)); /* call back functions */
		argc = CMDLINE_ARGS;

		__android_log_print(ANDROID_LOG_INFO, "MOTA", "Starting sotaclient...");
		main(argc, argv);
		__android_log_print(ANDROID_LOG_INFO, "MOTA", "End of sotaclient!");

		for(i = 0; i < CMDLINE_ARGS; i++)
			free(argv[i]);
		free(argv);
	}
}


/* Static class initializer: retrieve method and field IDs */
static jboolean sota_native_class_init(JNIEnv* env, jclass klass) {
	set_message_method_id = (*env)->GetMethodID(env, klass, "setSotaMessage", "(Ljava/lang/String;)V");

	if (!set_message_method_id) {
		/* We emit this message through the Android log instead of the GStreamer log because the later
		 * has not been initialized yet.
		 */
		__android_log_print (ANDROID_LOG_ERROR, "MOTA",
				"The calling class does not implement all necessary interface methods");
		return JNI_FALSE;
	}
	return JNI_TRUE;
}


static JNINativeMethod native_methods[] = {
	{ "sendSotaConfigs", "(I[Ljava/lang/String;)V", (void*) receive_sota_configs},
	{ "nativeSotaMain", "()V", (void*) sotajni_main},
	{ "nativeClassInit", "()Z", (void*) sota_native_class_init}
};


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	JNIEnv *env = NULL;

	java_vm = vm;

	if((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		__android_log_print (ANDROID_LOG_ERROR, "MOTA",
				"Could not retrieve JNIEnv");
		return 0;
	}

	jclass klass = (*env)->FindClass (env, "com/example/mota/FullscreenActivity");
	(*env)->RegisterNatives(env, klass, native_methods, sizeof(native_methods)/sizeof(native_methods[0]));

	return JNI_VERSION_1_4;
}
