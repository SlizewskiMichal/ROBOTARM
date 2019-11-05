#define GL_SILENCE_DEPRECATION
#include <GLUT/GLUT.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <time.h>
using namespace std;
int window=500;
float szer_klocka=0.1;
float wys_klocka=0.1;
string napis;
struct punkt{
    double wspolrzedna_x;
    double wspolrzedna_y;
};
struct color{
    float r;
    float g;
    float b;
};
vector <punkt> pamiec;
void nagrywanie();
void ustawienie_klockow();
void Display();
void poczatkowo();
void rysowanie_klockow();
void wait();
void odtworz();
bool klocek_stoi_na_klocku();
void tworz_przycisk();
int szerokosc_napisu();
void RenderString();
void grawitacja_klockow();
void SpecialInput();
void processKeys();
void Reshape();


int szerokosc_napisu(std::string napis){
    int szerokosc=0;
    for(int i=0;i<napis.length();i++)
        szerokosc=szerokosc+glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, napis[i]);
    return szerokosc;
}


void RenderString(double x, double y, const std::string &string)
{
    glRasterPos2d(x, y);
    for (int n=0; n<string.size(); ++n) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[n]);
    }
}


class button{
public:
    color kolor_przycisku;
    color kolor_napisu;
    string napis;
    int indeks;
    bool wykryty;
    bool pressed;
    float posx;
    float posy;
    float width;
    float height;


    void rysujButton(){
        float napis_szer=1.0*szerokosc_napisu(napis)/window;
        glColor3f(kolor_przycisku.r, kolor_przycisku.b, kolor_przycisku.g);
        glBegin(GL_POLYGON);
        glVertex2f(posx,posy);
        glVertex2f(posx+width, posy);
        glVertex2f(posx+width, posy+height);
        glVertex2f(posx, posy+height);
        glEnd();
        glColor3f(kolor_napisu.r, kolor_napisu.b, kolor_napisu.g);
        RenderString(posx+(width-2*napis_szer)/2, posy+height/2-16.0/window, napis);
    };


};
button nagraj_ruch;
class ramie{
public:
    punkt punkt_przytwierdzenia;
    punkt punkt_chwytaka;
    double dlugosc_segmentu1;
    double dlugosc_segmentu2;
    float CIRCLE_RADIUS=0.15f;
    double y_nowe=1+punkt_chwytaka.wspolrzedna_y;
    double x_nowe=1-punkt_chwytaka.wspolrzedna_x;


    void rysuj_ramie(){
        y_nowe=1+punkt_chwytaka.wspolrzedna_y;
        x_nowe=1-punkt_chwytaka.wspolrzedna_x;
        double y_ost,x_ost;
        glColor3f(0.5,0.5,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex2f(1.0,-1.0);
        y_ost=-1.0+sin(kat(x_nowe, y_nowe))*3*dlugosc_segmentu2;
        x_ost=1.0-cos(kat(x_nowe, y_nowe))*3*dlugosc_segmentu2;
        glVertex2f(x_ost,y_ost);
        glVertex2f(punkt_chwytaka.wspolrzedna_x, punkt_chwytaka.wspolrzedna_y);
        glEnd();
        drawCircle(x_ost, y_ost, 0.05);
    }


    void drawCircle(GLfloat x, GLfloat y, GLfloat radius)
    {
        int i;
        int triangleAmount = 1000;
        GLfloat twicePi = 2.0 * 3.1415;

        glEnable(GL_LINE_SMOOTH);
        glLineWidth(5.0);

        glBegin(GL_LINES);
        glColor4f(1.0, 0.0, 0.0, 1.0);
        for(i = 0; i <= triangleAmount; i++)
        {
            glVertex2f( x, y);
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)), y + (radius * sin(i * twicePi / triangleAmount)));
        }
        glEnd();
    }


    double kat(double x_kata,double y_kata){
        return atan(y_kata/x_kata)+acos((5*pow(dlugosc_segmentu2,2.0)+pow(x_kata,2.0)+pow(y_kata, 2.0))/(6*(sqrt(pow(x_kata, 2.0)+pow(y_kata, 2.0))*dlugosc_segmentu2)));
    }


    bool czy_up(){
        if(punkt_chwytaka.wspolrzedna_y<=1.00&&kat(x_nowe, y_nowe+0.05)>=0.00&&kat(x_nowe,y_nowe+0.05)<=1.57)
            return true;
        return false;
    }

    bool czy_down(){
        if(punkt_chwytaka.wspolrzedna_y>=-1.00&&kat(x_nowe, y_nowe-0.05)>=0.00&&kat(x_nowe,y_nowe-0.05)<=1.57)
            return true;
        return false;
    }

    bool czy_left(){
        if(punkt_chwytaka.wspolrzedna_x>=-1.00&&kat(x_nowe+0.05, y_nowe)>=0.00&&kat(x_nowe+0.05,y_nowe)<=1.57)
            return true;
        return false;
    }

    bool czy_right(){
        if(punkt_chwytaka.wspolrzedna_x<=1.00&&kat(x_nowe-0.05, y_nowe)>=0.00&&kat(x_nowe-0.05,y_nowe)<=1.57)
            return true;
        return false;
    }


    void up(){
        if(czy_up()){
            punkt_chwytaka.wspolrzedna_y+=0.05;
            if(nagraj_ruch.pressed){
                punkt punkt_do_zapisu;
                punkt_do_zapisu.wspolrzedna_x=punkt_chwytaka.wspolrzedna_x;
                punkt_do_zapisu.wspolrzedna_y=punkt_chwytaka.wspolrzedna_y;
                pamiec.push_back(punkt_do_zapisu);
            }
        }
    }


    void down(){
        if(czy_down())
            punkt_chwytaka.wspolrzedna_y-=0.05;
        if(nagraj_ruch.pressed){
            punkt punkt_do_zapisu;
            punkt_do_zapisu.wspolrzedna_x=punkt_chwytaka.wspolrzedna_x;
            punkt_do_zapisu.wspolrzedna_y=punkt_chwytaka.wspolrzedna_y;
            pamiec.push_back(punkt_do_zapisu);
        }
    }


    void left(){
        if(czy_left())
            punkt_chwytaka.wspolrzedna_x-=0.05;
        if(nagraj_ruch.pressed){
            punkt punkt_do_zapisu;
            punkt_do_zapisu.wspolrzedna_x=punkt_chwytaka.wspolrzedna_x;
            punkt_do_zapisu.wspolrzedna_y=punkt_chwytaka.wspolrzedna_y;
            pamiec.push_back(punkt_do_zapisu);
        }
    }


    void right(){
        if(czy_right())
            punkt_chwytaka.wspolrzedna_x+=0.05;
        if(nagraj_ruch.pressed){
            punkt punkt_do_zapisu;
            punkt_do_zapisu.wspolrzedna_x=punkt_chwytaka.wspolrzedna_x;
            punkt_do_zapisu.wspolrzedna_y=punkt_chwytaka.wspolrzedna_y;
            pamiec.push_back(punkt_do_zapisu);
        }
    }
};
ramie kawassaki;
class klocki{
public:
    float predkosc_grawitacja=0.006;
    bool chwycony;
    float width=0.1;
    float height=0.1;
    float r;
    float g;
    float b;
    float posx;
    float posy;


    void rysujklocek(){
        glColor3f(r,g,b);
        glBegin(GL_POLYGON);
        glVertex2f(posx,posy);
        glVertex2f(posx+width, posy);
        glVertex2f(posx+width, posy+height);
        glVertex2f(posx, posy+height);
        glEnd();
    }


    void up(){
        if(chwycony&&kawassaki.czy_up())
            posy+=0.05;
    }

    void down(){
        if(chwycony&&kawassaki.czy_down())
            posy-=0.05;
    }

    void right(){
        if(chwycony&&kawassaki.czy_right())
            posx+=0.05;
    }

    void left(){
        if(chwycony&&kawassaki.czy_left())
            posx-=0.05;
    }

    void chwyc_pusc(int key_index){
        if (key_index==32&&czy_chwyta(kawassaki.punkt_chwytaka.wspolrzedna_x, kawassaki.punkt_chwytaka.wspolrzedna_y)&&!chwycony){
            chwycony=true;
        }
        else
        if (chwycony)
            chwycony=false;

    }


    void grawitacja(){
        if (chwycony == false && posy>-1.0)
            posy-=0.001;
        if(posy<-1.0)
            posy=-1.0;
    }


    bool czy_chwyta(float x_ramienia,float y_ramienia){
        if(x_ramienia>posx&&x_ramienia<posx+width&&y_ramienia>posy&&y_ramienia<posy+height)
            return true;
        return false;
    }
};

klocki pierwszy,drugi,trzeci,czwarty,piaty;
klocki tab[5]={pierwszy,drugi,trzeci,czwarty,piaty};


void Display(){
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    grawitacja_klockow();
    rysowanie_klockow();
    tworz_przycisk();
    kawassaki.rysuj_ramie();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
};


void poczatkowo(){
    kawassaki.dlugosc_segmentu2=0.20*1.41*2;
    kawassaki.punkt_przytwierdzenia.wspolrzedna_x=1.0;
    kawassaki.punkt_przytwierdzenia.wspolrzedna_y=1.0;
    kawassaki.punkt_chwytaka.wspolrzedna_x=0.0;
    kawassaki.punkt_chwytaka.wspolrzedna_y=0.0;
    tab[0].chwycony=false;
    tab[0].posx=-0.95;
    tab[0].posy=-1.0;
    tab[0].r=1.0;
    tab[0].g=1.0;
    tab[0].b=0.0;
    tab[1].chwycony=false;
    tab[1].posx=-0.8;
    tab[1].posy=-1.0;
    tab[1].r=0.0;
    tab[1].g=1.0;
    tab[1].b=1.0;
    tab[2].chwycony=false;
    tab[2].posx=-0.65;
    tab[2].posy=-1.0;
    tab[2].r=1.0;
    tab[2].g=0.0;
    tab[2].b=1.0;
    tab[3].chwycony=false;
    tab[3].posx=-0.5;
    tab[3].posy=-1.0;
    tab[3].r=0.0;
    tab[3].g=1.0;
    tab[3].b=0.0;
    tab[4].chwycony=false;
    tab[4].posx=-0.35;
    tab[4].posy=-1.0;
    tab[4].r=1.0;
    tab[4].g=0.0;
    tab[4].b=0.0;
}


void rysowanie_klockow(){
    for(int i=0;i<5;i++)
        tab[i].rysujklocek();
}


void wait(float seconds){
    clock_t endwait;
    endwait = clock () + seconds * CLOCKS_PER_SEC ;
    while (clock() < endwait) {}
}


void odtworz(){
    poczatkowo();
    for (vector<punkt>::iterator i = pamiec.begin() ; i != pamiec.end(); ++i){
        if(i->wspolrzedna_x==100.0&&i->wspolrzedna_y==100.0){
            for(int j=0;j<5;j++){
                tab[j].chwyc_pusc(32);
            }

        }
        else{
            for(int j=0;j<5;j++){
                if(tab[j].chwycony){
                    tab[j].posx+=i->wspolrzedna_x-kawassaki.punkt_chwytaka.wspolrzedna_x;
                    tab[j].posy+=i->wspolrzedna_y-kawassaki.punkt_chwytaka.wspolrzedna_y;
                }
            }
            kawassaki.punkt_chwytaka.wspolrzedna_x=i->wspolrzedna_x;
            kawassaki.punkt_chwytaka.wspolrzedna_y=i->wspolrzedna_y;
        }
        wait(0.1);
        Display();
    }
    pamiec.clear();
}


bool klocek_stoi_na_klocku(int indeks){
    for(int i=0;i<5;i++){
        if(indeks!=i&&tab[indeks].posy+wys_klocka-0.002>tab[i].posy&&tab[indeks].posy<=(tab[i].posy+wys_klocka)&&tab[indeks].posx>=(tab[i].posx-szer_klocka)&&tab[indeks].posx<=(tab[i].posx+szer_klocka)){
            return true;
        }
    }
    return false;
}


void tworz_przycisk(){

    nagraj_ruch.height=0.25;
    nagraj_ruch.width=0.25;
    nagraj_ruch.indeks=1;
    nagraj_ruch.napis="R";
    if(nagraj_ruch.pressed==false){
        nagraj_ruch.kolor_przycisku.r=1.0;
        nagraj_ruch.kolor_przycisku.b=0.0;
        nagraj_ruch.kolor_przycisku.g=0.0;
        nagraj_ruch.kolor_napisu.r=1.0;
        nagraj_ruch.kolor_napisu.g=1.0;
        nagraj_ruch.kolor_napisu.b=1.0;
    }

    if(nagraj_ruch.pressed==true){
        nagraj_ruch.kolor_przycisku.r=0.0;
        nagraj_ruch.kolor_przycisku.b=1.0;
        nagraj_ruch.kolor_przycisku.g=0.0;
        nagraj_ruch.kolor_napisu.r=1.0;
        nagraj_ruch.kolor_napisu.g=1.0;
        nagraj_ruch.kolor_napisu.b=1.0;
    }
    nagraj_ruch.posx=-0.95;
    nagraj_ruch.posy=0.65;
    nagraj_ruch.rysujButton();
}


void grawitacja_klockow(){
    for(int i=0;i<5;i++){
        if(!klocek_stoi_na_klocku(i)){
            tab[i].grawitacja();
        }
    }
}


void SpecialInput(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            kawassaki.up();
            for(int i=0;i<5;i++)
                tab[i].up();
            break;
        case GLUT_KEY_DOWN:
            kawassaki.down();
            for(int i=0;i<5;i++)
                tab[i].down();
            break;
        case GLUT_KEY_LEFT:
            kawassaki.left();
            for(int i=0;i<5;i++)
                tab[i].left();
            break;
        case GLUT_KEY_RIGHT:
            kawassaki.right();
            for(int i=0;i<5;i++)
                tab[i].right();
            break;
    }
    glutPostRedisplay();
}


void processKeys(unsigned char key, int x, int y) {

    for(int i=0;i<5;i++)
        tab[i].chwyc_pusc(key);
    if(int(key)==32&&nagraj_ruch.pressed){
        punkt punkt_do_zapisu;
        punkt_do_zapisu.wspolrzedna_y=100.0;
        punkt_do_zapisu.wspolrzedna_x=100.0;
        pamiec.push_back(punkt_do_zapisu);
    }
    if(key=='r'){
        if(nagraj_ruch.pressed==true){
            nagraj_ruch.pressed=false;
            odtworz();
        }
        else{
            nagraj_ruch.pressed=true;
            poczatkowo();
        }
    }
    glutPostRedisplay();

}


void Reshape(int width,int height){
    Display();
}


int main(int argc,char * argv[])
{
    poczatkowo();
    glutInit( & argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(window,window);
    glutCreateWindow("robot");
    glutDisplayFunc(Display);
    glutSpecialFunc(SpecialInput);
    glutKeyboardFunc(processKeys);
    glutReshapeFunc(Reshape);
    glutMainLoop();
    return 0;
}

