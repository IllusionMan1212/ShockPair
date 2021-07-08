#include <jni.h>
#include <android/log.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

void initial_connection(){
    // connect with sdp and setting timeout
    struct sockaddr_l2 loc_addr = {}, rem_addr = {};
    int opt = sizeof(rem_addr);
    char buf[1024] = {};

    int s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

    memset(&loc_addr, 0, sizeof(loc_addr));
    loc_addr.l2_family = AF_BLUETOOTH;
    loc_addr.l2_bdaddr = *BDADDR_ANY;
    loc_addr.l2_psm = htobs(0x01);

    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    listen(s, 1);

//    printf("accepting sdp...\n");
    __android_log_print(ANDROID_LOG_DEBUG, "initial", "accepting sdp...\n");
    int csdp = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str(&rem_addr.l2_bdaddr, buf);
    __android_log_print(ANDROID_LOG_DEBUG, "initial", "%s\n", buf);
//    fprintf(stderr, "%s\n", buf);

    memset(buf, 0, sizeof(buf));
    __android_log_print(ANDROID_LOG_DEBUG, "initial", "getting sdp request...\n");
//    printf("getting sdp request...\n");
    int bytes_read;

    bytes_read = recv(csdp, buf, sizeof(buf), 0);
    __android_log_print(ANDROID_LOG_DEBUG, "initial", "%d\n", bytes_read);
//    printf("%d\n", bytes_read);

    // printf("sending sdp...\n");
    // int x = send(csdp, sdp_ds4, sizeof(sdp_ds4), 0);

}

int scan_for_devices() {
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    mkdir("/sdcard/shockpair", S_IRWXU | S_IRWXG | S_IRWXO);
    FILE *log = fopen("/sdcard/shockpair/log.txt", "a+");

    dev_id = hci_get_route(NULL);

    fprintf(log, "the dev id is: %d", dev_id);

    fclose(log);

//    dev_id = hci_devid("48:01:C5:60:33:A0");
    sock = hci_open_dev( dev_id );
    __android_log_print(ANDROID_LOG_DEBUG, "whatever", "dev_ID: %d", dev_id);
    __android_log_print(ANDROID_LOG_DEBUG, "whatever", "socket: %d", sock);
    if (dev_id < 0 || sock < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "whatever", "error opening socket");
//        perror("opening socket");
        //exit(1);
        return -1;
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),
                                 name, 0) < 0)
            strcpy(name, "[unknown]");
        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "%s  %s\n", addr, name);
//        printf("%s  %s\n", addr, name);
    }

    free( ii );
    close( sock );
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_illusionware_shockpair_AIDLService_nativeGetUid(JNIEnv* env, jobject this) {
    __android_log_print(ANDROID_LOG_ERROR, "whatever", "hello from root");
    return getuid();
}

JNIEXPORT jint JNICALL
Java_com_illusionware_shockpair_AIDLService_nativeConnectToDS3(JNIEnv *env, jobject this) {
    //initial_connection();
    //sleep(2000);

//    FILE *file = fopen("/default.prop", "r");
//    if (file == NULL) {
//        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "failed to open file\n");
//        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "ERROR: %s", strerror(errno));
//        return "failed";
//    }
//    char buf[4096];
//    fgets(buf, 4096, file);
//
//    fclose(file);
//    return (*env)->NewStringUTF(env, buf);

    return scan_for_devices();

//    jboolean isCopy;
//    //const char * path = (*env)->GetStringUTFChars(env, "/system/build.prop", &isCopy);
//    //__android_log_print(ANDROID_LOG_DEBUG, "whatever", "%s\n", path);
//    struct stat fileattrib;
//    if (stat("/fidsfjigegjes.txt", &fileattrib) < 0) {
//        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "NATIVE: stat error: [%s]", strerror(errno));
//    } else
//    {
//        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "file size is: %lld", fileattrib.st_size);
//        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "NATIVE: stat success, access perms: [%d]", fileattrib.st_mode);
//        return;
//    }
//
//    return;

    //scan_for_devices();

//    struct sockaddr_l2 loc_addr = { 0 }, rem_addr = { 0 };
//    char buf[1024] = { 0 };
//    int s, client, bytes_read;
//    socklen_t opt = sizeof(rem_addr);
//
//    // allocate socket
//    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
//
//    // bind socket to port 0x1001 of the first available
//    // bluetooth adapter
//    loc_addr.l2_family = AF_BLUETOOTH;
//    loc_addr.l2_bdaddr = *BDADDR_ANY;
//    // remote is D8:75:33:2C:82:5E
//    loc_addr.l2_psm = htobs(0x11);
//
//    __android_log_print(ANDROID_LOG_DEBUG, "whatever", "%d", loc_addr.l2_psm);
//
//    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
//
//    // put socket into listening mode
//    listen(s, 1);
//
//    // accept one connection
//    client = accept(s, (struct sockaddr *)&rem_addr, &opt);
//
//    rem_addr.l2_psm = htobs(0x11);
//    //str2ba("D8:75:33:2C:82:5E", &rem_addr.l2_bdaddr);
//    ba2str( &rem_addr.l2_bdaddr, buf );
//    __android_log_print(ANDROID_LOG_DEBUG, "whatever", "accepted connection from %s\n", buf);
//    //fprintf(stderr, "accepted connection from %s\n", buf);
//
//    memset(buf, 0, sizeof(buf));
//
//    bytes_read = recv(client, buf, sizeof(buf), 0);
//    // read data from the client
////    bytes_read = read(client, buf, sizeof(buf));
////    if( bytes_read > 0 ) {
//        __android_log_print(ANDROID_LOG_DEBUG, "whatever", "received [%d] bytes\n", bytes_read);
////        //printf("received [%s]\n", buf);
////    }
//
//    // close connection
//    close(client);
//    close(s);
}

