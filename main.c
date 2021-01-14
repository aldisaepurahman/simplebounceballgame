#include<windows.h>
#include<stdio.h>
#include<string.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include<GL/glut.h>
#include<math.h>
#include<time.h>
#include <unistd.h>
#include <pthread.h>
//deklarasi bola
typedef struct{
    float uXcenter;
    float uYcenter;
    int xStatus;
    int yStatus;
    float uBolaA;
    float uBolaB;
}bola;

bola ball[3];
//deklarasi variabel yang dibutuhkan
const float PI = 3.14159265;
int cek = 0;
int score = 0;
int kalah = 0;
int level = 0;
//deklarasi thread
pthread_t thread[3];
//deklarasi semua prosedur yang ada di program
void setBola();
void createText(char *kata, int x, int y);
void init();
void drawCircle(int a, int b, int x_center, int y_center);
void drawLine(int x1, int x2, int y1, int y2);
void drawSquare(int x1, int x2, int y1, int y2);
void drawTriangle(int x1, int x2, int x3, int y1);
void drawBoard(int x1, int x2, int y1, int y2);
void move(unsigned char c, int x, int y);
void gerakBola(int i);
void createBola();
void dinding();
void viewGameOver(void);
void gameNormal(void);
void gameEasy(void);
void gameDifficult(void);
void startGame(unsigned char c, int x, int y);
void levelDisplay(void);
void startButton(unsigned char c, int x, int y);
void display(void);
//buat bola dan posisi bolanya
void setBola(){
    srand(time(NULL));
    int i;

    for(i = 0;i < 3; i++){
        ball[i].uXcenter = (rand() % 500) + 50;
        ball[i].uYcenter = (rand() % 400) + 100;
        ball[i].xStatus = rand() % 2;
        ball[i].yStatus = 1;
        ball[i].uBolaA = 10;
        ball[i].uBolaB = 90;
    }
}
//untuk membuat teks
void createText(char *kata, int x, int y){
    char *c;
     glRasterPos2f(x, y);
     for (c=kata; *c != '\0'; c++)
     {
         glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
      }
}
//inisialisasi warna awal layar
void init() {
	glClearColor(0,0.5,0,0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0f, 600.0f, 0.0f, 600.0f);
	glShadeModel (GL_SMOOTH);
}
//untuk menggambar lingkaran
void drawCircle(int a, int b, int x_center, int y_center){
    glBegin(GL_POLYGON);
    for (int i=0;i<=360;i++){
        double sudut = i *(2*PI/b);
        double x= x_center+a*cos(sudut);
        double y= y_center +a*sin(sudut);
        glVertex2d(x,y);
    }
    glEnd();
}
//untuk menggambar garis
void drawLine(int x1, int x2, int y1, int y2){
	glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
	glEnd();
}
//untuk menggambar kotak
void drawSquare(int x1, int x2, int y1, int y2){
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(x1, y1);//kiri bawah
    glVertex2f(x1, y2);//kiri atas
    glVertex2f(x2, y2);//kanan atas
    glVertex2f(x2, y1);//kanan bawah
    glEnd();
}
//untuk menggambar segitiga
void drawTriangle(int x1, int x2, int x3, int y1){
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 0.0);
    glVertex2i(x1, 0);
    glVertex2i(x2, 0);
    glVertex2i(x3, y1);
    glEnd();
}
//untuk gambar papan
void drawBoard(int x1, int x2, int y1, int y2){
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(x1, y1);//kiri bawah
    glVertex2f(x1, y2);//kiri atas
    glVertex2f(x2, y2);//kanan atas
    glVertex2f(x2, y1);//kanan bawah
    glEnd();
}
//prosedur gerak
void move(unsigned char c, int x, int y){
    //huruf a
    if(c == 97 || c == 65){
        /*kalau mengenai dinding*/
        if(275+cek > 80){
            cek-=15;
        }
    }else if(c == 100 || c == 68){//huruf d
        /*kalau mengenai dinding*/
        if(375+cek < 540){
            cek+=15;
        }
    }
}
//atur arah gerakan bola
void gerakBola(int i){
    //jika kepinggir
    if((ball[i].uXcenter >= 0) && (ball[i].uXcenter <= 600)){
        //jika ke kiri
        if(ball[i].uXcenter < 50){
            ball[i].xStatus = 1;
        }
        //jika ke kanan
        else if(ball[i].uXcenter > 550){
            ball[i].xStatus = 0;
        }
    }
    //jika menuju dinding atas atau papan
    if((ball[i].uYcenter >= 50) && (ball[i].uYcenter <= 550)){
        //jika mentok keatas
        if(ball[i].uYcenter > 500){
            ball[i].yStatus = 0;
        }
        //jika mentok dibawah
        else if(ball[i].uYcenter < 105){
            //jika kena papan, tambah 10 skornya
            if((250+cek <= ball[i].uXcenter) && (ball[i].uXcenter < 375+cek)){
                ball[i].yStatus = 1;
                score +=10;
            }
        }
    }
    //jika tidak mengenai papan, hilangkan dari layar
    if(ball[i].uYcenter < 100){
        ball[i].yStatus = 2;
    }
    //jika level easy
    if(level == 1){
        //jika maju ke kanan
        if(ball[i].xStatus == 1){
            ball[i].uXcenter += 0.2;
        }
        //jika ke kiri
        else{
            ball[i].uXcenter -= 0.2;
        }
        //jika ke atas
        if(ball[i].yStatus == 1){
            ball[i].uYcenter += 0.2;
        }
        //jika kebawah
        else{
            ball[i].uYcenter -= 0.2;
        }
    }
    //jika level normal
    else if(level == 2){
        if(ball[i].xStatus == 1){
            ball[i].uXcenter += 0.4;
        }else{
            ball[i].uXcenter -= 0.4;
        }

        if(ball[i].yStatus == 1){
            ball[i].uYcenter += 0.4;
        }else{
            ball[i].uYcenter -= 0.4;
        }
    }
    //jika level no touch
    else if(level == 3){
        if(ball[i].xStatus == 1){
            ball[i].uXcenter += 0.8;
        }else{
            ball[i].uXcenter -= 0.8;
        }

        if(ball[i].yStatus == 1){
            ball[i].uYcenter += 0.8;
        }else{
            ball[i].uYcenter -= 0.8;
        }
    }
}
//buat thread bola berdasarkan levelnya
void createBola(){
    //jika level easy dan normal, buat 2 thread
    if((level == 1) || (level == 2)){
        for(int i = 0; i < 2; i++) {
            pthread_create(&thread[i], NULL, gerakBola, i);
        }

        for(int i = 0; i < 2; i++) {
            pthread_join(thread[i], NULL);
        }
    }
    //jika level no touch buat 3 thread
    else if(level == 3){
        for(int i = 0; i < 3; i++) {
            pthread_create(&thread[i], NULL, gerakBola, i);
        }

        for(int i = 0; i < 3; i++) {
            pthread_join(thread[i], NULL);
        }
    }

    printf("\n");
}
//buat dinding
void dinding(){
    //menampilkan dinding kiri
    glColor3f(0.8, 0.7, 0.2);
    drawBoard(20, 50, 70, 500);
    //menampilkan dinding kanan
    glColor3f(0.8, 0.7, 0.2);
    drawBoard(550, 580, 70, 500);
    //menampilkan dinding atas
    glColor3f(0.8, 0.7, 0.2);
    drawBoard(20, 580, 500, 530);
}
//mode game normal
void gameNormal(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //menampilkan score yang telah didapat
    glColor3f(1.0, 1.0, 1.0);
    createText("Score : ", 460, 560);
    char sScore[10];
    itoa(score, sScore, 10);//convert int to string
    createText(sScore, 510, 560);
    createText("Game Normal ", 50, 560);

    //menampilkan papan
    glColor3f(1.0, 1.0, 1.0);
    drawBoard(250+cek, 375+cek, 70, 100);

    //buat threadnya
    glutIdleFunc(createBola);
    //gambar ulang bolanya jika diset bukan 2 status y nya
    glColor3f(0.8, 0.7, 0.2); //bola berwarna kuning
    for(int i = 0; i < 2; i++) {
        if(ball[i].yStatus != 2){
            drawCircle(ball[i].uBolaA, ball[i].uBolaB, ball[i].uXcenter, ball[i].uYcenter);
        }
    }
    glutPostRedisplay();

    dinding();
    //gerakkan papan
    glutKeyboardFunc(move);
    glutSwapBuffers();
    glFlush();
    //jika sudah dihapus dari layar, tampilkan tulisan game over
    if(ball[0].yStatus == 2 && ball[1].yStatus == 2){
        glutDisplayFunc(viewGameOver);
    }
}
//mode game easy
void gameEasy(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //menampilkan score yang telah didapat
    glColor3f(1.0, 1.0, 1.0);
    createText("Score : ", 460, 560);
    char sScore[10];
    itoa(score, sScore, 10);//convert int to string
    createText(sScore, 510, 560);
    createText("Game Easy ", 50, 560);

    //menampilkan papan
    glColor3f(1.0, 1.0, 1.0);
    drawBoard(250+cek, 375+cek, 70, 100);

    //buat threadnya
    glutIdleFunc(createBola);
    //gambar ulang bolanya jika diset bukan 2 status y nya
    glColor3f(0.8, 0.7, 0.2); //bola berwarna kuning
    int i;
    for(i = 0; i < 2; i++) {
        if(ball[i].yStatus != 2){
            drawCircle(ball[i].uBolaA, ball[i].uBolaB, ball[i].uXcenter, ball[i].uYcenter);
        }
    }
    glutPostRedisplay();

    dinding();
    //gerakkan papan
    glutKeyboardFunc(move);
    glFlush();
    //jika sudah dihapus dari layar, tampilkan tulisan game over
    if(ball[0].yStatus == 2 && ball[1].yStatus == 2){
        glutDisplayFunc(viewGameOver);
    }
}
//mode game sulit
void gameDifficult(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //menampilkan score yang telah didapat
    glColor3f(1.0, 1.0, 1.0);
    createText("Score : ", 460, 560);
    char sScore[10];
    itoa(score, sScore, 10);//convert int to string
    createText(sScore, 510, 560);
    createText("Game No Touch ", 50, 560);

    //menampilkan papan
    glColor3f(1.0, 1.0, 1.0);
    drawBoard(250+cek, 375+cek, 70, 100);

    //buat threadnya
    glutIdleFunc(createBola);
    //gambar ulang bolanya jika diset bukan 2 status y nya
    glColor3f(0.8, 0.7, 0.2); //bola berwarna kuning
    for(int i = 0; i < 3; i++) {
        if(ball[i].yStatus!= 2){
            drawCircle(ball[i].uBolaA, ball[i].uBolaB, ball[i].uXcenter, ball[i].uYcenter);
        }
    }
    glutPostRedisplay();

    dinding();
    //gerakkan papan
    glutKeyboardFunc(move);
    glFlush();
    //jika sudah dihapus dari layar, tampilkan tulisan game over
    if(ball[0].yStatus == 2 && ball[1].yStatus == 2 && ball[2].yStatus == 2){
        glutDisplayFunc(viewGameOver);
    }
}
//untuk memulai game
void startGame(unsigned char c, int x, int y){
    //set ulang skornya
    score = 0;
    //set posisi bolanya
    setBola();
    //jika yang ditekan adalah huruf 'n' untuk normal
    if(c == 110 || c == 78){
        level = 2;
        glutDisplayFunc(gameNormal);
    //jika yang ditekan adalah huruf 'e' untuk easy
    }else if(c == 101 || c == 69){
        level = 1;
        glutDisplayFunc(gameEasy);
    //jika yang ditekan adalah huruf 't' untuk difficult
    }else if(c == 116 || c == 84){
        level = 3;
        glutDisplayFunc(gameDifficult);
    }
    glutPostRedisplay();
}
//tampilan saat memilih level
void levelDisplay(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //menampilkan tulisan bounce the ball
    glColor3f(0.8, 0.7, 0.2);
    createText("Bounce The Ball", 255, 560);

    //menampilkan kotak level
    drawSquare(225, 400, 350, 400);
    drawSquare(225, 400, 275, 325);
    drawSquare(225, 400, 200, 250);

    //menampilkan tulisan untuk level
    glColor3f(1.0, 1.0, 1.0);
    createText("Easy (Press 'e')", 268, 370);
    createText("Normal (Press 'n')", 261, 295);
    createText("No Touch (Press 't')", 258, 220);

    //menampilkan segitiga
    drawTriangle(40, 90, 65, 50);
    drawTriangle(90, 140, 115, 50);
    drawTriangle(140, 190, 165, 50);
    drawTriangle(440, 490, 465, 50);
    drawTriangle(490, 540, 515, 50);
    drawTriangle(540, 590, 565, 50);

    //menampilan garis
    drawLine(150, 200, 150, 200);
    drawLine(140, 100, 140, 100);
    drawLine(550, 600, 300, 350);
    drawLine(540, 500, 290, 250);

    drawCircle(10, 14, 150, 300);

    //mengubah display ketika telah menekan keyboard
    glutKeyboardFunc(startGame);
    glFlush();
}
//prosedur untuk memulai game
void startButton(unsigned char c, int x, int y){
    //jika menekan spasi
    if(c == 32) {
        glutDisplayFunc(levelDisplay);
    }
    //jika menekan tombol 'x', keluar dari game
    else if(c == 120 || c == 88){
        exit(0);
    }
    glutPostRedisplay();
}
//tampilan awal game
void display(void){
    glClear(GL_COLOR_BUFFER_BIT);

    //menampilkan teks
    glColor3f(0.8, 0.7, 0.2);//kuning
    createText("Bounce The Ball", 255, 560);

    //menampilkan teks
    glColor3f(1.0, 1.0, 1.0);//putih
    createText("Press Space Button To Start", 220, 300);

    //menampilkan segitiga
    drawTriangle(40, 90, 65, 50);
    drawTriangle(90, 140, 115, 50);
    drawTriangle(140, 190, 165, 50);
    drawTriangle(440, 490, 465, 50);
    drawTriangle(490, 540, 515, 50);
    drawTriangle(540, 590, 565, 50);

    //menampilan garis
    drawLine(150, 200, 150, 200);
    drawLine(140, 100, 140, 100);
    drawLine(550, 600, 300, 350);
    drawLine(540, 500, 290, 250);

    drawCircle(10, 14, 150, 300);

    //mengubah display ketika telah menekan keyboard
    glutKeyboardFunc(startButton);
    glFlush();
}
//fungsi utama
int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE| GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("Bounce The Ball");

    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
//tampilan ketika game over
void viewGameOver(void){
    glClear(GL_COLOR_BUFFER_BIT);

    drawSquare(230, 385, 400, 250);

    //menampilkan teks
    glColor3f(1.0, 1.0, 1.0);//putih
    createText("GAME OVER!", 265, 350);

    glColor3f(1.0, 1.0, 1.0);//putih
    createText("Score : ", 265, 300);
    char sScore[10];
    itoa(score, sScore, 10);//convert int to string
    createText(sScore, 315, 300);

    //menampilkan teks
    createText("Press Space Button To Next Game or Press X To Exit", 140, 80);

    //menampilkan segitiga
    drawTriangle(40, 90, 65, 50);
    drawTriangle(90, 140, 115, 50);
    drawTriangle(140, 190, 165, 50);
    drawTriangle(440, 490, 465, 50);
    drawTriangle(490, 540, 515, 50);
    drawTriangle(540, 590, 565, 50);

    //menampilan garis
    drawLine(150, 200, 150, 200);
    drawLine(140, 100, 140, 100);
    drawLine(550, 600, 300, 350);
    drawLine(540, 500, 290, 250);

    //menampilkan bola
    drawCircle(10, 14, 150, 300);

    //menampilkan teks
    glColor3f(0.8, 0.7, 0.2);//kuning
    createText("Bounce The Ball", 255, 560);

    //mengubah display ketika telah menekan keyboard
    glutKeyboardFunc(startButton);
    glFlush();
}
