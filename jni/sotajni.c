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

#include "jansson.h"

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define VEH_CONF_MAX 6
#define BASE_FILE_PATH "/sdcard/mota"
#define VEH_CONFIG_FILE "./files/client_info.json"
#define SERVER_IP_ADDRESS "122.165.96.181"
#define CMDLINE_ARGS 7

char VehConf[VEH_CONF_MAX][256];
static JavaVM *java_vm;
static jstring jmsg;

extern void main(int, char**);
#if 0
extern void stop_wikiclient(void);
extern void new_location_data(double lat, double lon);
extern char *get_ui_message(void);
#endif


/*##########################################################################
 * Functions
 *##########################################################################*/
#if 0
jstring wikijni_get_ui_message(JNIEnv* env, jobject thiz)
{
	char *msg;

//	msg = get_ui_message();

	__android_log_print(ANDROID_LOG_INFO, "MOTA", "%s() - msg: %s", __func__, msg);
	jmsg = (*env)->NewStringUTF(env, msg);

	return jmsg;
}


void wikijni_newlocation(JNIEnv *env, jobject thiz, jdouble lat, jdouble lon)
{
	__android_log_print(ANDROID_LOG_INFO, "MOTA", "lat = %lf, lon = %lf", lat, lon);
//	new_location_data(lat, lon);
}


void wikijni_stop(JNIEnv *env, jobject thiz)
{
//	stop_wikiclient();
}


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


void receive_veh_configs(JNIEnv *env, jobject thiz, jint len, jobjectArray stringArray)
{
	int i;
	jstring jstr;
	const char *nstr;

	__android_log_print (ANDROID_LOG_INFO, "MOTA", "Received %d Vehicle Configuration", len);
	for(i = 0; i < len; i++) {
		jstr = (*env)->GetObjectArrayElement(env, stringArray, i);
		nstr = (*env)->GetStringUTFChars(env, jstr, NULL);
		strcpy(VehConf[i], nstr);
		__android_log_print(ANDROID_LOG_INFO, "MOTA", "%s", VehConf[i]);
		(*env)->ReleaseStringUTFChars(env, jstr, nstr);
	}

	//create_config_json_file(VEH_CONFIG_FILE);
}

#define USE_FORK_IN_ANDROID 0

void sotajni_main(JNIEnv *env, jobject thiz)
{
	char **argv;
	int argc, i;
	pid_t pid;
	char pwd[1024];

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
		strcpy(argv[2], VEH_CONFIG_FILE);
		strcpy(argv[3], "-s");
		strcpy(argv[4], SERVER_IP_ADDRESS); /* Server IP */
		strcpy(argv[5], "-t");
		strcpy(argv[6], ""); /* tmp files in current folder */
		argc = CMDLINE_ARGS;

		__android_log_print(ANDROID_LOG_INFO, "MOTA", "Starting sotaclient...");
		main(argc, argv);
		__android_log_print(ANDROID_LOG_INFO, "MOTA", "End of sotaclient!");

		for(i = 0; i < CMDLINE_ARGS; i++)
			free(argv[i]);
		free(argv);
	}
}



static JNINativeMethod native_methods[] = {
	{ "nativeSotaMain", "()V", (void*) sotajni_main}
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
