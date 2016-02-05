#include <string.h>
#include <jni.h>
#include "lebookSecureSrc.h"

#include <android/log.h>
#define LOG_TAG "cqEmbed"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

char* jstringToChar(JNIEnv* env, jstring jstr)
{
	char* rtn = NULL;
	jclass clsstring = (*env)->FindClass(env, "java/lang/String");
	jstring strencode = (*env)->NewStringUTF(env, "utf-8");
	jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid, strencode);
	jsize alen = (*env)->GetArrayLength(env, barr);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = (char*)malloc(alen + 1);
		if(!rtn) 
		{
			return NULL;
		}

		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	(*env)->ReleaseByteArrayElements(env, barr, ba, 0);
	return rtn;
}

jstring strTojstring(JNIEnv* env, const char* pat)
{
	jclass strClass = (*env)->FindClass(env, "Ljava/lang/String;");
	jmethodID ctorID = (*env)->GetMethodID(env, strClass, "<init>", "([BLjava/lang/String;)V");
	jbyteArray bytes = (*env)->NewByteArray(env, strlen(pat));
	(*env)->SetByteArrayRegion(env, bytes, 0, strlen(pat), (jbyte*)pat);
	jstring encoding = (*env)->NewStringUTF(env, "utf-8");
	return (jstring)(*env)->NewObject(env, strClass, ctorID, bytes, encoding);
}

jstring Java_com_example_demo_SecretKeyUtil_encrypt( JNIEnv*  env, jobject this, jstring obj1, jstring obj2, jstring obj3)
{

	char *str1 = jstringToChar(env, obj1);
	char *str2 = jstringToChar(env, obj2);
	char *str3 = jstringToChar(env, obj3);
	
	char *result = lebook_book_string_encrypt(str1, str2, str3);

	return (*env)->NewStringUTF(env, result);
}

jstring Java_com_example_demo_SecretKeyUtil_decrypt( JNIEnv*  env, jobject this, jstring obj1, jstring obj2, jstring obj3)
{
	char *str1 = jstringToChar(env, obj1);
	char *str2 = jstringToChar(env, obj2);
	char *str3 = jstringToChar(env, obj3);
	
	// LOGI("TTTTTT===decrypt==ebookid= %s===userpin=%s====key=%s", ebookid, userpin, key); 

	char *result =  lebook_book_string_decrypt(str1, str2, str3);
	
	return (*env)->NewStringUTF(env, result);
}
