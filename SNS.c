//#define CURL_STATICLIB
//#pragma comment (lib, "wldap32.lib")
//#pragma comment (lib, "ws2_32.lib")
//#pragma comment(lib, "CRYPT32.lib")
//#pragma comment(lib, "Normaliz.lib")

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <curl/curl.h>

#ifndef _WIN32
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#endif
int quit = 0;

struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void MakeDirectory(unsigned char* full_path)
{
    unsigned char temp[256], * sp;
    strcpy(temp, full_path); // ��ι��ڿ��� ����
    sp = temp; // �����͸� ���ڿ� ó������

    while ((sp = strchr(sp, '\\'))) { // ���丮 �����ڸ� ã������
        if (sp > temp && *(sp - 1) != ':') { // ��Ʈ���丮�� �ƴϸ�
            *sp = '\0'; // ��� ���ڿ� ������ ����
#ifdef _WIN32
            _mkdir(temp);
#else
            mkdir(temp, 0777);
#endif
            // ���丮�� ����� (�������� ���� ��)
            * sp = '\\'; // ���ڿ��� ������� ����
        }
        sp++; // �����͸� ���� ���ڷ� �̵�
    }

}

static void
handle_sigint(int signum) {
    quit = 1;
}

int main(void)
{
    CURL* curl_get;
    CURL* curl_post;
    CURLcode res;
    int init = 0;
    int gnum = 0;

    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    struct curl_slist* headerlist = NULL;
    headerlist = curl_slist_append(headerlist, "Content-Type: application/json");

    char name[100] = "test1";
    char date[100] = "2023";
    char acceleration[30] = "1.00,-0.19,0.14";
    char gaussMagnetic[30] = "4.91,-14.87,25.15";
    char angularRate[30] = "0.18,0.49,0.79";
    char underwaterMicrophone[10] = "0.1";
    char gesture[10] = "0.1";
    char proximity[10] = "0.1";
    char dryAmbientLight[10] = "0.1";
    char dryAmbientRGB[30] = "R.20,G.20,B.19";
    char dryBarometricPressure[10] = "0.1";
    char dryTemperature[10] = "0.1";
    char dryHumidity[10] = "0.1";
    char underwaterPressure[10] = "0.1";
    char underwaterTemperature[10] = "0.1";
    char underwaterDepth[10] = "0.1";
    char seaLevelAltitude[10] = "0.1";
    char underwaterPictureLow[10] = "0.1";
    char underwaterPictureHigh[10] = "0.1";
    char underwaterVideoWithoutMic[10] = "0.1";
    char underwaterVideoWithMic[10] = "0.1";
    char gnss[100] = "GNGGA,053008.095,,,,,0,0,,,M,,M,,*54";

    char postthis[1000] = "{\"name\": \"test1\", \"date\": \"2023\", \"acceleration\": \"0.1\", \"gaussMagnetic\": \"0.1\", \"angularRate\": \"0.1\", \"underwaterMicrophone\": \"0.1\", \"gesture\": \"0.1\", \"proximity\": \"0.1\", \"dryAmbientLight\": \"0.1\", \"dryAmbientRGB\": \"0.1\", \"dryBarometricPressure\": \"0.1\", \"dryTemperature\": \"0.1\", \"dryHumidity\": \"0.1\", \"underwaterPressure\": \"0.1\", \"underwaterTemperature\": \"0.1\", \"underwaterDepth\": \"0.1\", \"seaLevelAltitude\": \"0.1\", \"underwaterPictureLow\": \"0.1\", \"underwaterPictureHigh\": \"0.1\", \"underwaterVideoWithoutMic\": \"0.1\", \"underwaterVideoWithMic\": \"0.1\", \"gnss\": \"0.1\"}";
    char input[100];
    //char* temp;
    char chunk_temp[1000];
    //char ID[200];
    //char* ID_CODE;
    char POST_URL[100];

    curl_get = curl_easy_init();
    curl_post = curl_easy_init();

#ifndef _WIN32
    struct sigaction sa;
    struct termios SENSORnewtio;
    struct termios GPSnewtio;
    struct termios LIGHTnewtio;
    int SENSORttyfd = 0;
    int GPSttyfd = 0;
    int LIGHTttyfd = 0;
    int SENSORi = 0;
    int GPSi = 0;
    int LIGHTi = 0;
    int cc = 0;
    int GPSc = 0;
    int GPScc = 0;
    char SENSORbuf[1024];
    char GPSbuf[1024];
    char LIGHTbuf[1024];
    char SENSORM[1024];
    char GPSM[1024];
    char GPSMtemp[1024];
    char LIGHTM[1024];
    char* SENSORtemp;
    char* GPStemp;
    char* GPStempd;
    char* LIGHTtemp;
    char* SENSORttyname = "/dev/ttyACM0";
    char* GPSttyname = "/dev/ttyS3";
    char* LIGHTttyname = "/dev/ttyUSB0";
    SENSORttyfd = open(SENSORttyname, O_RDWR | O_NOCTTY);
    GPSttyfd = open(GPSttyname, O_RDWR | O_NOCTTY);
    LIGHTttyfd = open(LIGHTttyname, O_RDWR | O_NOCTTY);

    if (SENSORttyfd < 0)
    {
        printf(">> tty Open Fail [%s]\r\n ", SENSORttyname);
        return -1;
    }

    if (GPSttyfd < 0)
    {
        printf(">> tty Open Fail [%s]\r\n ", GPSttyname);
        return -1;
    }

    if (LIGHTttyfd < 0)
    {
        printf(">> tty Open Fail [%s]\r\n ", LIGHTttyname);
        return -1;
    }

    memset(&SENSORnewtio, 0, sizeof(SENSORnewtio));
    memset(&GPSnewtio, 0, sizeof(GPSnewtio));
    memset(&LIGHTnewtio, 0, sizeof(LIGHTnewtio));

    SENSORnewtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD | CRTSCTS;
    SENSORnewtio.c_iflag = IGNPAR;
    SENSORnewtio.c_oflag = 0;

    //set input mode (non-canonical, no echo,.....)
    SENSORnewtio.c_lflag = 0;     // LF recive filter unused
    SENSORnewtio.c_cc[VTIME] = 0;     // inter charater timer unused
    SENSORnewtio.c_cc[VMIN] = 0;     // blocking read until 1 character arrives

    GPSnewtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD | CRTSCTS;
    GPSnewtio.c_iflag = IGNPAR;
    GPSnewtio.c_oflag = 0;

    //set input mode (non-canonical, no echo,.....)
    GPSnewtio.c_lflag = 0;     // LF recive filter unused
    GPSnewtio.c_cc[VTIME] = 0;     // inter charater timer unused
    GPSnewtio.c_cc[VMIN] = 0;     // blocking read until 1 character arrives

    LIGHTnewtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD | CRTSCTS;
    LIGHTnewtio.c_iflag = IGNPAR;
    LIGHTnewtio.c_oflag = 0;

    //set input mode (non-canonical, no echo,.....)
    LIGHTnewtio.c_lflag = 0;     // LF recive filter unused
    LIGHTnewtio.c_cc[VTIME] = 0;     // inter charater timer unused
    LIGHTnewtio.c_cc[VMIN] = 0;     // blocking read until 1 character arrives

    tcflush(SENSORttyfd, TCIFLUSH); // inital serial port
    tcsetattr(SENSORttyfd, TCSANOW, &SENSORnewtio); // setting serial communication

    tcflush(GPSttyfd, TCIFLUSH); // inital serial port
    tcsetattr(GPSttyfd, TCSANOW, &GPSnewtio); // setting serial communication

    tcflush(LIGHTttyfd, TCIFLUSH); // inital serial port
    tcsetattr(LIGHTttyfd, TCSANOW, &LIGHTnewtio); // setting serial communication

    //printf("## ttyo1 Opened [%s]\r\n", ttyname);
    while (1) {
        //printf("check1\n");
        SENSORi = read(SENSORttyfd, SENSORbuf, 1024);
        SENSORbuf[SENSORi] = '\0';
        //printf("check2\n");
        GPSi = read(GPSttyfd, GPSbuf, 1024);
        GPSbuf[GPSi] = '\0';
        //printf("check3\n");
        //printf("%s", buf);
        if (cc > 2) {
            sprintf(SENSORM, SENSORbuf);
            sprintf(GPSM, GPSbuf);

            SENSORtemp = strtok(SENSORM, "=\n");
            while (1) {
                //printf("sensor while\n");
                if (SENSORtemp == NULL) {
                    break;
                }
                else if (strcmp(SENSORtemp, "acceleration") == 0) {
                    sprintf(acceleration, strtok(NULL, "=\n"));
                    //printf("%s\n", acceleration);
                }
                else if (strcmp(SENSORtemp, "angularRate") == 0) {
                    sprintf(angularRate, strtok(NULL, "=\n"));
                    //printf("%s\n", angularRate);
                }
                else if (strcmp(SENSORtemp, "gaussMagnetic") == 0) {
                    sprintf(gaussMagnetic, strtok(NULL, "=\n"));
                    //printf("%s\n", gaussMagnetic);
                }
                else if (strcmp(SENSORtemp, "dryBarometricPressure") == 0) {
                    sprintf(dryBarometricPressure, strtok(NULL, "=\n"));
                    //printf("%s\n", dryBarometricPressure);
                }
                else if (strcmp(SENSORtemp, "dryTemperature") == 0) {
                    sprintf(dryTemperature, strtok(NULL, "=\n"));
                    //printf("%s\n", dryTemperature);
                }
                else if (strcmp(SENSORtemp, "dryHumidity") == 0) {
                    sprintf(dryHumidity, strtok(NULL, "=\n"));
                    //printf("%s\n", dryHumidity);
                }
                else if (strcmp(SENSORtemp, "proximity") == 0) {
                    sprintf(proximity, strtok(NULL, "=\n"));
                    //printf("%s\n", proximity);
                }
                else if (strcmp(SENSORtemp, "dryAmbientRGB") == 0) {
                    sprintf(dryAmbientRGB, strtok(NULL, "=\n"));
                    //printf("%s\n", dryAmbientRGB);
                }
                SENSORtemp = strtok(NULL, "=\n");
            }

            while (1) {
                //printf("gps while\n");
                GPStemp = strtok(GPSM, "$\n");
                while (GPSc < GPScc) {
                    GPStemp = strtok(NULL, "$\n");
                    GPSc++;
                }
                if (GPStemp == NULL) {
                    break;
}
                else {
                    sprintf(GPSMtemp, GPStemp);
                    GPStempd = strtok(GPStemp, ",");
                    if (strcmp(GPStempd, "GNGGA") == 0) {
                        sprintf(gnss, GPSMtemp);
                        //printf("%s\n", gnss);
                        break;
                    }
                }
                GPScc++;
                GPSc = 0;
            }
            break;
                }
        //printf("check4\n");
        sleep(1);
        //printf("check5\n");
        cc++;
        //printf("check6\n");
    }

    // �Ʒ� �κ��� �׽�Ʈ �ڵ� �������� ������ ����ϽǶ��� �����ٶ��ϴ�
    //close(SENSORttyfd); //close serial port
    //close(GPSttyfd); //close serial port
    //return 0;
#endif

#ifdef _WIN32
    signal(SIGINT, handle_sigint);
#else
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    /* So we do not exit on a SIGPIPE */
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, NULL);
#endif

    if (!curl_get && !curl_post) {
        printf("curl error\n");
        quit = 1;
    }
    while (!quit) {

        if (!init) {
            curl_easy_setopt(curl_get, CURLOPT_FOLLOWLOCATION, 1L);
            /* send all data to this function  */
            curl_easy_setopt(curl_get, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            /* we pass our 'chunk' struct to the callback function */
            curl_easy_setopt(curl_get, CURLOPT_WRITEDATA, (void*)&chunk);

            curl_easy_setopt(curl_post, CURLOPT_HTTPHEADER, headerlist);
            sprintf(postthis, "{\"name\": \"%s\", \"date\": \"%s\", \"acceleration\": \"%s\", \"gaussMagnetic\": \"%s\", \"angularRate\": \"%s\", \"underwaterMicrophone\": \"%s\", \"gesture\": \"%s\", \"proximity\": \"%s\", \"dryAmbientLight\": \"%s\", \"dryAmbientRGB\": \"%s\", \"dryBarometricPressure\": \"%s\", \"dryTemperature\": \"%s\", \"dryHumidity\": \"%s\", \"underwaterPressure\": \"%s\", \"underwaterTemperature\": \"%s\", \"underwaterDepth\": \"%s\", \"seaLevelAltitude\": \"%s\", \"underwaterPictureLow\": \"%s\", \"underwaterPictureHigh\": \"%s\", \"underwaterVideoWithoutMic\": \"%s\", \"underwaterVideoWithMic\": \"%s\", \"gnss\": \"%s\"}", name, date, acceleration, gaussMagnetic, angularRate, underwaterMicrophone, gesture, proximity, dryAmbientLight, dryAmbientRGB, dryBarometricPressure, dryTemperature, dryHumidity, underwaterPressure, underwaterTemperature, underwaterDepth, seaLevelAltitude, underwaterPictureLow, underwaterPictureHigh, underwaterVideoWithoutMic, underwaterVideoWithMic, gnss);
            curl_easy_setopt(curl_post, CURLOPT_POSTFIELDS, postthis);
            curl_easy_setopt(curl_post, CURLOPT_POST, 1L);
            /* send all data to this function  */
            curl_easy_setopt(curl_post, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            /* we pass our 'chunk' struct to the callback function */
            curl_easy_setopt(curl_post, CURLOPT_WRITEDATA, (void*)&chunk);

            init = 1;
        }
//
//        while (!quit) {//�α���
//#ifdef _WIN32
//            printf("�����ϼ���.\n1.�α���\n2.����\n");
//#else
//            printf("MENU\n1.LOGIN\n2.EXIT\n");
//#endif
//            scanf("%s", input);
//            if (strcmp(input, "1") == 0) {
//#ifdef _WIN32
//                printf("���̵� �Է����ּ���.\n");
//#else
//                printf("ID\n");
//#endif
//                scanf("%s", input);
//                sprintf(ID, "http://52.78.83.63:8080/api/users/user?email=%s", input);
//
//                curl_easy_setopt(curl_get, CURLOPT_URL, ID);
//
//                /* Perform the request, res will get the return code */
//                res = curl_easy_perform(curl_get);
//                /* Check for errors */
//                if (res != CURLE_OK)
//                    fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                        curl_easy_strerror(res));
//
//                //printf("ûũ : %s\n", chunk.memory);
//                sprintf(chunk_temp, "%s", chunk.memory);
//                temp = strtok(chunk_temp, " {}\":,");
//                //printf("ûũ : %s\nûũ���� : %s\n���� : %s\n", chunk.memory, chunk_temp, temp);
//                free(chunk.memory);
//                chunk.memory = malloc(1);
//                chunk.size = 0;
//                if (temp == NULL) {
//#ifdef _WIN32
//                    printf("�������� �ʴ� ���̵��Դϴ�.\n");
//#else
//                    printf("INCORRECT ID\n");
//#endif
//                }
//                else if (strcmp(temp, "id") == 0) {
//#ifdef _WIN32
//                    printf("�α��� ����\n");
//#else
//                    printf("LOGIN SUCCESS\n");
//#endif
//                    ID_CODE = strtok(NULL, " {}\":,");
//                    sprintf(POST_URL, "http://52.78.83.63:8080/api/underwater/%s", ID_CODE);
//                    printf("���˿�:%s\n",POST_URL);
//                    curl_easy_setopt(curl_post, CURLOPT_URL, POST_URL);
//                    break;
//                }
//                else {
//#ifdef _WIN32
//                    printf("�������� �ʴ� ���̵��Դϴ�.\n");
//#else
//                    printf("INCORRECT ID\n");
//#endif
//                }
//
//            }
//            else if (strcmp(input, "2") == 0) {
//                quit = 1;
//#ifdef _WIN32
//                printf("�����մϴ�.\n");
//#else
//                printf("EXIT\n");
//#endif
//                break;
//            }
//            else {
//#ifdef _WIN32
//                printf("�� �� �� �Է��Դϴ�. �ٽ� �Է����ּ���.\n");
//#else
//                printf("INCORRECT NUMBER\n");
//#endif
//            }
//        }
//
//        if (quit) {
//            break;
//        }

        sprintf(POST_URL, "http://52.78.83.63:8080/api/underwater/19");
        curl_easy_setopt(curl_post, CURLOPT_URL, POST_URL);

        while (!quit) {//�����ͼۼ���
//#ifdef _WIN32
//            printf("�����ϼ���.\n1.�α׾ƿ�\n2.�ø��� ������ ����\n3.���� ������ ���� ����\n4.����\n");
//#else
//            printf("MENU\n1.LOGOUT\n2.READ DATA\n3.SEND DATA\n4.EXIT\n");
//#endif
//            scanf("%s", input);
#ifdef _WIN32
            sprintf(input, "4");
#else
            LIGHTi = read(LIGHTttyfd, LIGHTbuf, 1024);
            LIGHTbuf[LIGHTi] = '\0';
            //printf("%s\n", LIGHTbuf);
            sprintf(LIGHTM, LIGHTbuf);
            LIGHTtemp = strtok(LIGHTM, " ");
            if (LIGHTtemp == NULL) {
                sprintf(input, "9");
                //printf("check0\n");
                sleep(1);
                //break;
            }
            else if (strcmp(LIGHTtemp, ">>>") == 0) {
                sprintf(input, "2");
                //printf("check1\n");
            }
            else {
                sprintf(input, "9");
                //printf("check2\n");
                //printf("%s\n",LIGHTM);
                sleep(1);
        }
#endif
            if (strcmp(input, "1") == 0) {
//#ifdef _WIN32
//                printf("�α׾ƿ��մϴ�.\n");
//#else
//                printf("LOGOUT\n");
//#endif
                quit = 1;
                break;
            }
            else if (strcmp(input, "2") == 0) {
//#ifdef _WIN32
//                printf("�����͸� �����մϴ�.\n");
//#else
//                printf("READ DATA\n");
//#endif
#ifdef _WIN32
                sprintf(acceleration, "1.02,-0.17,0.19");
                sprintf(angularRate, "0.22,0.78,0.87");
                sprintf(gaussMagnetic, "5.01,-13.38,25.24");
                sprintf(dryBarometricPressure, "101.02");
                sprintf(dryTemperature, "25.24");
                sprintf(dryHumidity, "28.09");
                sprintf(proximity, "252");
                sprintf(dryAmbientRGB, "R.20,G.20,B.20");
                sprintf(gnss, "GNGGA,053008.095,37.8847493,N,127.1720248,E,0,0,,,M,,M,,*54");
#else
                cc = 0;
                GPSc = 0;
                GPScc = 0;
                while (1) {
                    //printf("check1\n");
                    SENSORi = read(SENSORttyfd, SENSORbuf, 1024);
                    SENSORbuf[SENSORi] = '\0';
                    //printf("check2\n");
                    GPSi = read(GPSttyfd, GPSbuf, 1024);
                    GPSbuf[GPSi] = '\0';
                    //printf("check3\n");
                    //printf("%s", buf);
                    if (cc > 2) {
                        sprintf(SENSORM, SENSORbuf);
                        sprintf(GPSM, GPSbuf);

                        SENSORtemp = strtok(SENSORM, "=\n");
                        while (1) {
                            //printf("sensor while\n");
                            if (SENSORtemp == NULL) {
                                break;
                            }
                            else if (strcmp(SENSORtemp, "acceleration") == 0) {
                                sprintf(acceleration, strtok(NULL, "=\n"));
                                //printf("%s\n", acceleration);
                            }
                            else if (strcmp(SENSORtemp, "angularRate") == 0) {
                                sprintf(angularRate, strtok(NULL, "=\n"));
                                //printf("%s\n", angularRate);
                            }
                            else if (strcmp(SENSORtemp, "gaussMagnetic") == 0) {
                                sprintf(gaussMagnetic, strtok(NULL, "=\n"));
                                //printf("%s\n", gaussMagnetic);
                            }
                            else if (strcmp(SENSORtemp, "dryBarometricPressure") == 0) {
                                sprintf(dryBarometricPressure, strtok(NULL, "=\n"));
                                //printf("%s\n", dryBarometricPressure);
                            }
                            else if (strcmp(SENSORtemp, "dryTemperature") == 0) {
                                sprintf(dryTemperature, strtok(NULL, "=\n"));
                                //printf("%s\n", dryTemperature);
                            }
                            else if (strcmp(SENSORtemp, "dryHumidity") == 0) {
                                sprintf(dryHumidity, strtok(NULL, "=\n"));
                                //printf("%s\n", dryHumidity);
                            }
                            else if (strcmp(SENSORtemp, "proximity") == 0) {
                                sprintf(proximity, strtok(NULL, "=\n"));
                                //printf("%s\n", proximity);
                            }
                            else if (strcmp(SENSORtemp, "dryAmbientRGB") == 0) {
                                sprintf(dryAmbientRGB, strtok(NULL, "=\n"));
                                //printf("%s\n", dryAmbientRGB);
                            }
                            SENSORtemp = strtok(NULL, "=\n");
                        }

                        while (1) {
                            //printf("gps while\n");
                            GPStemp = strtok(GPSM, "$\n");
                            while (GPSc < GPScc) {
                                GPStemp = strtok(NULL, "$\n");
                                GPSc++;
                            }
                            if (GPStemp == NULL) {
                                break;
                            }
                            else {
                                sprintf(GPSMtemp, GPStemp);
                                GPStempd = strtok(GPStemp, ",");
                                if (strcmp(GPStempd, "GNGGA") == 0) {
                                    sprintf(gnss, GPSMtemp);
                                    //printf("%s\n", gnss);
                                    break;
                                }
                            }
                            GPScc++;
                            GPSc = 0;
                        }
                        break;
                    }
                    //printf("check4\n");
                    sleep(1);
                    //printf("check5\n");
                    cc++;
                    //printf("check6\n");
                }
#endif

                sprintf(underwaterMicrophone, "0.2");
                sprintf(gesture, "0.2");
                sprintf(dryAmbientLight, "0.2");
                sprintf(underwaterPressure, "0.2");
                sprintf(underwaterTemperature, "0.2");
                sprintf(underwaterDepth, "0.2");
                sprintf(seaLevelAltitude, "0.2");
                sprintf(underwaterPictureLow, "0.2");
                sprintf(underwaterPictureHigh, "0.2");
                sprintf(underwaterVideoWithoutMic, "0.2");
                sprintf(underwaterVideoWithMic, "0.2");

                sprintf(name, "test2");
                sprintf(date, "2023");
                sprintf(postthis, "{\"name\": \"%s\", \"date\": \"%s\", \"acceleration\": \"%s\", \"gaussMagnetic\": \"%s\", \"angularRate\": \"%s\", \"underwaterMicrophone\": \"%s\", \"gesture\": \"%s\", \"proximity\": \"%s\", \"dryAmbientLight\": \"%s\", \"dryAmbientRGB\": \"%s\", \"dryBarometricPressure\": \"%s\", \"dryTemperature\": \"%s\", \"dryHumidity\": \"%s\", \"underwaterPressure\": \"%s\", \"underwaterTemperature\": \"%s\", \"underwaterDepth\": \"%s\", \"seaLevelAltitude\": \"%s\", \"underwaterPictureLow\": \"%s\", \"underwaterPictureHigh\": \"%s\", \"underwaterVideoWithoutMic\": \"%s\", \"underwaterVideoWithMic\": \"%s\", \"gnss\": \"%s\"}", name, date, acceleration, gaussMagnetic, angularRate, underwaterMicrophone, gesture, proximity, dryAmbientLight, dryAmbientRGB, dryBarometricPressure, dryTemperature, dryHumidity, underwaterPressure, underwaterTemperature, underwaterDepth, seaLevelAltitude, underwaterPictureLow, underwaterPictureHigh, underwaterVideoWithoutMic, underwaterVideoWithMic, gnss);
                //sprintf(postthis, "{\"name\": \"%s\", \"date\": \"%s\"}",name,date);

                curl_easy_setopt(curl_post, CURLOPT_POSTFIELDS, postthis);

                /* Perform the request, res will get the return code */
                res = curl_easy_perform(curl_post);
                /* Check for errors */
                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));

                //printf("ûũ : %s\n", chunk.memory);
                sprintf(chunk_temp, "%s", chunk.memory);
                //temp = strtok(chunk_temp, " {}\":,");
                //printf("ûũ : %s\nûũ���� : %s\n���� : %s\n", chunk.memory, chunk_temp, temp);
                gnum = atoi(chunk_temp);
                free(chunk.memory);
                chunk.memory = malloc(1);
                chunk.size = 0;
                if (gnum == 0) {
#ifdef _WIN32
                    printf("������ ���ۿ� �����߽��ϴ�.\n");
#else
                    printf("FAIL\n");
#endif
                }
                else {
#ifdef _WIN32
                    //printf("%d��° ���� ������ ���ε� �Ϸ�Ǿ����ϴ�.\n", gnum);
                    printf("������ ���ۿ� �����߽��ϴ�.\n");
#else
                    printf("SUCCESS\n");
#endif
                }
            }
            else if (strcmp(input, "3") == 0) {
//#ifdef _WIN32
//                printf("�����͸� �۽��մϴ�.\n");
//#else
//                printf("SEND DATA\n");
//#endif
                //curl_easy_setopt(curl_post, CURLOPT_POSTFIELDS, postthis);

                /* Perform the request, res will get the return code */
                res = curl_easy_perform(curl_post);
                /* Check for errors */
                if (res != CURLE_OK)
                    fprintf(stderr, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));

                //printf("ûũ : %s\n", chunk.memory);
                sprintf(chunk_temp, "%s", chunk.memory);
                //temp = strtok(chunk_temp, " {}\":,");
                //printf("ûũ : %s\nûũ���� : %s\n���� : %s\n", chunk.memory, chunk_temp, temp);
                gnum = atoi(chunk_temp);
                free(chunk.memory);
                chunk.memory = malloc(1);
                chunk.size = 0;
                if (gnum == 0) {
#ifdef _WIN32
                    printf("������ ���ۿ� �����߽��ϴ�.\n");
#else
                    printf("FAIL\n");
#endif
                }
                else {
#ifdef _WIN32
                    //printf("%d��° ���� ������ ���ε� �Ϸ�Ǿ����ϴ�.\n", gnum);
                    printf("������ ���ۿ� �����߽��ϴ�.\n");
#else
                    printf("SUCCESS\n");
#endif
                }

            }
            else if (strcmp(input, "4") == 0) {
                quit = 1;
//#ifdef _WIN32
//                printf("�����մϴ�.\n");
//#else
//                printf("EXIT\n");
//#endif
                break;
            }
            else {
//#ifdef _WIN32
//                printf("�� �� �� �Է��Դϴ�. �ٽ� �Է����ּ���.\n");
//#else
//                printf("INCORRECT NUMBER\n");
//#endif
            }
        }

        if (quit) {
            break;
        }

    }
    printf("EXIT\n");
    //printf("��������1\n");
    /* always cleanup */
    curl_easy_cleanup(curl_get);
    //printf("��������2\n");
    free(chunk.memory);
    //printf("��������3\n");
    curl_easy_cleanup(curl_post);
    //printf("��������4\n");
    curl_slist_free_all(headerlist);
    //printf("��������5\n");
#ifndef _WIN32
    close(SENSORttyfd); //close serial port
    close(GPSttyfd); //close serial port
    close(LIGHTttyfd); //close serial port
#endif
    return 0;
}