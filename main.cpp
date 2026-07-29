#define FREEGLUT_STATIC //obligar a usar la libreria estatica

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

#include <windows.h> // Necesario para la función Sleep() de Windows
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#if defined(__has_include)
  #if __has_include(<SDL/SDL.h>) && __has_include(<SDL/SDL_mixer.h>)
    #include <SDL/SDL.h>
    #include <SDL/SDL_mixer.h>
    #define SDL_AUDIO_AVAILABLE 1
  #else
    #define SDL_AUDIO_AVAILABLE 0
  #endif
#else
  #include <SDL/SDL.h>
  #include <SDL/SDL_mixer.h>
  #define SDL_AUDIO_AVAILABLE 1
#endif

using namespace std;

#define TICK_INTERVAL 20
//estas constantes eran para una posterior implementacion de las texturas
#define CASA_OK 0
#define CASA_SEMI 1
#define CASA_ROTA 2
#define CASA_MUY 3
#define CASA_NO 4

//VARIABLES TIPOS LIBRERIAS
GLuint next_time;

GLuint CanionDL;
GLuint MisilDL;
GLuint MarcianoDL;
GLuint CasaDL;
GLuint OvniDL;
GLuint VidasDL;

#if SDL_AUDIO_AVAILABLE
Mix_Music *musica; //para musica de fondo
const char *file;
#endif


//VARIABLES TIPOS NORMALES
bool keypressed[256];
bool specialpressed[256];

bool suenaMusica;//para que suene la musica


bool idaMarcianos;//booleano para controlar si los marcianos van o vienen

bool idaOvni;


bool disp;// booleano que controla si hemos disparado

int puntos;

int ancho, alto;//ancho y alto de la pantalla

int xCanion ;

int xMisil;
int yMisil;

int xMarcianos;
int yMarcianos;

int xCasa;//coordenada x a partir de la cual se colocan las casas
int yCasa;//coordenada y a partir de la cual se colocan las casas

int xOvni;//pos x ovni
int yOvni;//pos y ovni

int xVidas;//coordenada x a partir de la que se colocan las vidas
int yVidas;//coordenada y a partir de la que se colocan las vidas
int numVidas;//vidas que te quedan;


int limDerechoCanion;

int limderechoOvni;

int limderecho;//limite derecho solo para los marcianos

int limizquierdo;//limite izquierdo comun por que se trabajo con cordenadas con origen en la derecha

//CONSTANTES
const int anchoCanion = 50;
const int altoCanion = 50;

const int anchoMisil = 14;

const int anchoMarciano = 70;
const int altoMarciano = 40;

const int separacionX = 10;//separcion entre marcianos
const int separacionY = 10;

const int anchoCasa = 100;
const int altoCasa = 80;
const int separacionCasaX = 150;

const int anchoOvni =70;
const int altoOvni =30;

const int anchoVida=30;
const int altoVida=30;
const int separacionVidaX=10;

const int margen = 65; // margen para los marcianos ,respecto a la parte inferior de la pantalla
const int marco = 150; // margen para los marcianos ,respecto a la parte superior de la pantalla

int maximaPuntuacion;

/*struct textura{
    int anchoTex, altoTex; //ancho y alto de la textura
    ILubyte *data; //data de la textura
};*/


struct tMarciano{
    int x1,y1;//posicion esquina superior izquierda
    int x2,y2;//posicion esquina inferior  derecha
    int valor; //los puntos que vale cada marciano
    bool vivo;//controla si esta vivo o muerto
};

tMarciano MarcianosST [5][11];//matriz con los 55 marcianos

struct tCasa{
    int x1Casa,y1Casa;//posicion esquina superior izquierda
    int x2Casa,y2Casa;//posicion esquina inferior  derecha
    int estadoCasa; // variable que controla el estado de destriccion de la casa 0 =ok 1=semirota 2=rota 3=my rota 4=no esta
};
tCasa CasasST [4];//array con las 4 casas

struct tOvni{
    int x1Ovni,y1Ovni;//posicion esquina superior izquierda
    int x2Ovni,y2Ovni;//posicion esquina inferior  derecha
    bool esta;
} Ovni;

struct tVidas{
    int x1Vidas, y1Vidas;//posici�n esquina superior izquierda
    int x2Vidas, y2Vidas;//posici�n esquina inferior derecha
    bool VidaVisible;//indica si se muestra la vida o no
};
tVidas VidasST [3]; //Array con 3 vidas

void guardarScore(int ptos){//funcion que carga la maxima puntuacion
    char cadena[5];
    sprintf(cadena, "%d", ptos);

    // Crea el fichero de salidaMarcianos
    ofstream fs("Score.txt");

    // Enviamos la cadena al fichero de salidaMarcianos:
    fs << cadena << endl;

    // Cerrar el fichero,
    // para luego poder abrirlo para lectura:
    fs.close();
}

int cargarScore(){
    char cadena[10] = "0"; // Tamaño seguro e inicializado en 0

    // abre el fichero de entrada
    ifstream fin("Score.txt");

    // Validamos que el archivo realmente exista antes de leer
    if(fin.is_open()){
        fin.getline(cadena, 10);
        fin.close();
    }

    return atoi(cadena);
}


/*Funciones para cargar texturas*/
/*void CargarImagen(struct texture){

    ILuint texid;
    ilGenImages(1, &texid);
    ilBindImage(texid);
    ilLoadImage(texture);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    anchoTex = ilGetInteger(IL_IMAGE_WIDTH);
    altoTex = ilGetInteger(IL_IMAGE_HEIGHT);
    data = ilGetData();

    ilDeleteImages(1, &texid);

    return textura;
}

GLuint loadTexture(const char *file)
{

GLuint textureID;
CargarImagen(file, anchoTex, altoTex, data); // carga la imagen
glGenTextures(1, &textureID);
glBindTexture(GL_TEXTURE_2D, textureID);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
return textureID;
}*///Fin funciones texturas



void initVariables(){//inicializa las variables globales

    ancho = glutGet(GLUT_SCREEN_WIDTH);
    alto  = glutGet(GLUT_SCREEN_HEIGHT);

    suenaMusica = true;//para que suene la musica

    idaMarcianos=true;//booleano para controlar si los marcianos van o vienen

    idaOvni = true;

    disp=false;// booleano que controla si hemos disparado

    puntos = 0;

    maximaPuntuacion=cargarScore();

    limderecho=ancho-((anchoMarciano*11)+(separacionX*10)+100);
    limizquierdo = 100;

    xCanion= limizquierdo;

    xMisil = limizquierdo;
    yMisil = 0;

    xMarcianos= limizquierdo;
    yMarcianos=alto-(altoMarciano*5+separacionY*4)-marco;

    xCasa = limizquierdo+150;//coordenada x a partir de la cual se colocan las casas
    yCasa = margen;//coordenada y a partir de la cual se colocan las casas

    xOvni=limizquierdo;
    yOvni=alto - (marco+50);

    xVidas = limizquierdo +640;
    yVidas = alto-85;

    limDerechoCanion=ancho-(anchoCanion+limizquierdo);

    limderechoOvni=ancho -(anchoOvni +limizquierdo);


    numVidas=3; //desde el principio tienes 3 vidas


    next_time = glutGet(GLUT_ELAPSED_TIME) + TICK_INTERVAL ;
}

void initMarcianos(){ ////////////////INICIALIZACI�N DE LOS MARCIANOS
    for(int i = 0 ; i<5 ;i++){
        for(int j = 0 ; j <11 ; j++){
            MarcianosST[i][j].vivo=true;
            MarcianosST[i][j].x1=j*(anchoMarciano+separacionX);
            MarcianosST[i][j].y1=i*(altoMarciano+separacionY);
            MarcianosST[i][j].x2=j*(anchoMarciano+separacionX)+anchoMarciano;
            MarcianosST[i][j].y2=i*(altoMarciano+separacionY)+altoMarciano;
            MarcianosST[i][j].valor=i*10+10;
        }
    }
}

void initCasas(){  ////////////////INICIALIZACI�N DE LAS CASAS
    for(int j = 0 ; j < 4 ; j++){
            CasasST[j].x1Casa=j*(anchoCasa+separacionCasaX);
            CasasST[j].y1Casa=altoCasa;
            CasasST[j].x2Casa=j*(anchoCasa+separacionCasaX)+anchoCasa;
            CasasST[j].y2Casa=altoCasa+altoCasa;
            CasasST[j].estadoCasa= CASA_OK;
        }

}

void initVidas(){
    for(int i=0; i<3; i++){
            VidasST[i].x1Vidas=i*(anchoVida+separacionVidaX);
            VidasST[i].y1Vidas=altoVida;
            VidasST[i].x2Vidas=i*(anchoVida+separacionVidaX)+anchoVida;
            VidasST[i].y2Vidas=altoVida+altoVida;
            VidasST[i].VidaVisible=true;
    }
}

void initOvni(){  ////////////////INICIALIZACI�N DEL OVNI
            Ovni.x1Ovni=0;
            Ovni.y1Ovni=anchoOvni;
            Ovni.x2Ovni=altoOvni;
            Ovni.y2Ovni=0;
            Ovni.esta= false;
}

#if SDL_AUDIO_AVAILABLE
void initSonidos(){
    file="musica2.ogg";
    SDL_Init(SDL_INIT_AUDIO);
    if(Mix_OpenAudio(22050,AUDIO_S8, 2,1024)>0){

    }else{
        musica=Mix_LoadMUS(file);
        if(musica == NULL){
            cout << "error mixer";
        }else{
        Mix_PlayMusic(musica,-1);
        }
    }
}
#else
void initSonidos(){}
#endif

void initTeclas(){
    glutIgnoreKeyRepeat(1);
    for(int i =0;i<256;i++){
        keypressed[i]=false;
        specialpressed[i]=false;
    }
}

void initListasGL(){
    CanionDL = glGenLists(1);
    glNewList(CanionDL , GL_COMPILE);
        glBegin(GL_QUADS);
            glColor3ub(0, 255, 0);//cuerpo del ca�on
            glVertex2i(0, 0);
            glVertex2i(0, 50);
            glVertex2i(50, 50);
            glVertex2i(50, 0);

            glColor3ub(0, 255, 0);//ca�on
            glVertex2i(15, 50);
            glVertex2i(15, 70);
            glVertex2i(35, 70);
            glVertex2i(35, 50);
        glEnd();
    glEndList();

    MisilDL = glGenLists(1);
    glNewList(MisilDL , GL_COMPILE);
        glBegin(GL_QUADS);
            glColor3ub(0, 0, 255);//Misil
            glVertex2i(18, 50);
            glVertex2i(18, 70);
            glVertex2i(32, 70);
            glVertex2i(32, 50);
        glEnd();
    glEndList();

    MarcianoDL = glGenLists(1);
    glNewList(MarcianoDL , GL_COMPILE);//Marciano
    glBegin(GL_QUADS);
            glColor3ub(134, 137, 93);
            glVertex2i(0, 0);
            glVertex2i(0, altoMarciano);
            glVertex2i(anchoMarciano, altoMarciano);
            glVertex2i(anchoMarciano, 0);
        glEnd();
    glEndList();

    CasaDL = glGenLists(1);
    glNewList(CasaDL , GL_COMPILE);
            glBegin(GL_QUADS);
            glColor3ub(0, 255, 255);///////CASA
            glVertex2i(0, 0);
            glVertex2i(0,altoCasa );
            glVertex2i(anchoCasa,altoCasa );
            glVertex2i(anchoCasa, 0);
            glEnd();
    glEndList();

    OvniDL = glGenLists(1);
    glNewList(OvniDL, GL_COMPILE);
            glBegin(GL_QUADS);
            glColor3ub(0, 255, 255);///////OVNI
            glVertex2i(0, 0);
            glVertex2i(0, altoOvni);
            glVertex2i(anchoOvni,altoOvni);
            glVertex2i(anchoOvni, 0);
            glEnd();
    glEndList();

    VidasDL = glGenLists(1);
    glNewList(VidasDL, GL_COMPILE);
            glBegin(GL_QUADS);
            glColor3ub(0, 255, 0);
            glVertex2i(0,0);
            glVertex2i(0, altoVida);
            glVertex2i(anchoVida,altoVida);
            glVertex2i(anchoVida,0);
            glEnd();
    glEndList();

}

void quitarVida(){
    if(numVidas>0){
        for(int i; i > numVidas -1;i--){
            VidasST[i].VidaVisible=false;
            numVidas-=1;
        }
    }
}

void avanza (int value){

        if(xMarcianos < limderecho && idaMarcianos){
            xMarcianos += 10;
        }else if(xMarcianos >= limderecho){
            if(yMarcianos >= margen){
                yMarcianos -= 10;
                }else if(yMarcianos >= margen && numVidas>0){
                quitarVida();
            }

            idaMarcianos = false;
            xMarcianos -= 10;
        }
        else if(xMarcianos > limizquierdo && !idaMarcianos){
            xMarcianos -= 10;
        }else if(xMarcianos <= limizquierdo){
            if(yMarcianos >= margen){
                yMarcianos -= 10;

            }else if(yMarcianos >= margen && numVidas>0){
                quitarVida();
            }
            idaMarcianos = true;
            xMarcianos +=10;
        }



    glutPostRedisplay();

    glutTimerFunc(500,avanza,0);
}

void mueveOvni(){

    if(Ovni.esta){
    if(xOvni <= limderechoOvni && idaOvni){
        xOvni += 10;
    }else if(xOvni >= limderechoOvni){
        idaOvni=false;
        xOvni -=10;
        Ovni.esta=false;
        }
    }

    if(Ovni.esta){
    if(xOvni >= limizquierdo && !idaOvni){
        xOvni -= 10;
    }else if(xOvni <= limizquierdo){
        idaOvni = true;
        xOvni += 10;
        Ovni.esta=false;
        }
    }

}

void aleatorio(){

    if (!Ovni.esta && (rand() % 2) == 0){
    Ovni.esta=true;
    }
}

/* Funci�n para inicializar algunos par�metros de OpenGL */
void init(void){                                                    
     /*......................................INIT..........*/

    glClearColor(0.0,0.0,0.0,1.0);
    glEnable(GL_DEPTH_TEST);

    srand(time(NULL));//para luego poder generar el numero aleatorio

    initVariables();

    initTeclas();

    initListasGL();

    initMarcianos();

    initCasas();

    initOvni();

    initVidas();

    initSonidos();


    glutTimerFunc(500,avanza,0);
}



/* Funci�n que se llamar� cada vez que se redimensione la ventana */
void reshape(int w, int h){
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

GLuint time_left(void){
    GLuint now;
    now = glutGet(GLUT_ELAPSED_TIME);
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

void special(int key,int x, int y){//pulsacion techas
    specialpressed[key] = true;
}

void specialUp ( int key, int x, int y ){
    specialpressed[key] = false;
}

void keyboard ( unsigned char key, int x, int y ){
    keypressed[key] = true;
}

void keyboardUp ( unsigned char key, int x, int y ){
    keypressed[key] = false;
}

void keyoperations (void){

    if (keypressed[' ']){
    disp=true;
    }

    if (keypressed[27]){
#if SDL_AUDIO_AVAILABLE
            Mix_HaltMusic();
            Mix_FreeMusic(musica);
            SDL_Quit();
#endif
            guardarScore(maximaPuntuacion);
            exit(0);
    }


#if SDL_AUDIO_AVAILABLE
    if(keypressed[109]){
        if(suenaMusica){
        Mix_HaltMusic();
        suenaMusica=false;
        }else if(!suenaMusica){
        Mix_PlayMusic(musica,-1);
        suenaMusica=true;
        }
    }
#endif


    if (specialpressed[GLUT_KEY_RIGHT]){
        if(xCanion<limDerechoCanion)
        xCanion += 10;
    }
    if (specialpressed[GLUT_KEY_LEFT]){
        if(xCanion>limizquierdo)
        xCanion -= 10;
    }


}

void resetMisil(){ // funcion que reinicia el misil y detiene el desparo actual
    disp=false;
    yMisil=0;
    xMisil=xCanion;
}

void incrementarPuntos(int a){//incrementa la puntuacion 'a' puntos
    if(puntos<999){ // puntuacion maxima limitada a 999
        puntos+=a;
        if (puntos>maximaPuntuacion)
        maximaPuntuacion=puntos;
    }

}

void ColisionMarciano(){
    int i,j;
    if(disp){
        for(i = 5 ; i>=0 ;i--){
            for( j = 0 ; j<11 ; j++){
                if (MarcianosST[i][j].vivo &&
                    (xMisil+(anchoMisil/2))>=MarcianosST[i][j].x1+xMarcianos && (xMisil+(anchoMisil/2))<=MarcianosST[i][j].x2+xMarcianos  &&
                        yMisil>=MarcianosST[i][j].y1+yMarcianos && yMisil<=MarcianosST[i][j].y2+yMarcianos){
                            MarcianosST[i][j].vivo=false;
                            resetMisil();
                            incrementarPuntos(MarcianosST[i][j].valor);
                        }
                    }
                }
            }
}

void ColisionOvni(){
    if(Ovni.esta && disp &&
        xMisil + (anchoMisil/2) >= Ovni.x1Ovni+xOvni && xMisil + (anchoMisil/2) >= Ovni.x2Ovni +xOvni &&
            yMisil >= Ovni.y1Ovni && yMisil <=Ovni.y2Ovni){
                Ovni.esta=false;
                xOvni=limizquierdo;
                yOvni=alto-(marco+50);
                resetMisil();


        }



}

void ColisionCasa(){
    if(disp){
        for(int i = 0 ; i < 4 ;i++){
            if(CasasST[i].estadoCasa < 4 &&
                xMisil + (anchoMisil/2) >= CasasST[i].x1Casa + xCasa && xMisil + (anchoMisil/2) <=CasasST[i].x2Casa + xCasa &&
                    yMisil >= CasasST[i].y1Casa + yCasa && yMisil <= CasasST[i].y2Casa+yCasa){
                        CasasST[i].estadoCasa += 1;
                        resetMisil();
                        if(puntos > 0)
                        incrementarPuntos(-10);


            }
        }
    }
}

/*Implementar esta funci�n para que se cuenten las vidas*/
//void ContadorVidas(){}

/* Funci�n que se ejecuta cuando no hay eventos */ /*............................................IDLE........................*/
void idle(void){
        if(disp){

            if(yMisil<alto)
                yMisil +=15;

            if(yMisil >= alto){
                resetMisil();
                }
        }else{
            xMisil=xCanion;

        }
       aleatorio();
        mueveOvni();


        ColisionCasa();
        ColisionMarciano();
        ColisionOvni();
        //ContadorVidas();



    glutPostRedisplay();
}

void dibujaMarcianos(){
    glPushMatrix();
        glTranslatef(xMarcianos,yMarcianos,0);
        for(int i = 0 ; i < 5 ;i++){
            for(int j = 0 ; j < 11 ; j++){
                if(MarcianosST[i][j].vivo){
                    glPushMatrix();
                        glTranslatef(MarcianosST[i][j].x1,MarcianosST[i][j].y1,0);
                        glCallList(MarcianoDL);
                    glPopMatrix();
                }
            }
        }
    glPopMatrix();
}

void dibujaMarcador(){
    char ptos[50]; // Ampliado a 50 bytes para evitar desbordamiento
    sprintf(ptos, "SCORE: %d", puntos);
    glPushMatrix();
                    glColor3ub(0, 255, 0);
                    glRasterPos2i(ancho-200,alto-50);
                    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,
                    (const unsigned char*)ptos);
    glPopMatrix();
}

void dibujaPuntuacionMaxima(){
    char maxPuntu[50]; // Ampliado a 50 bytes
    sprintf(maxPuntu, "MAX SCORE: %d", maximaPuntuacion);
    glPushMatrix();
                    glColor3ub(0, 255, 0);
                    glRasterPos2i(limizquierdo,alto-50);
                    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,
                    (const unsigned char*)maxPuntu);
    glPopMatrix();
}

void dibujaCasas(){
    glPushMatrix();
            glTranslatef(xCasa,yCasa,0);
            for(int j = 0 ; j < 4 ; j++){
                if(CasasST[j].estadoCasa < 4){
                    glPushMatrix();
                        glTranslatef(CasasST[j].x1Casa,CasasST[j].y1Casa,0);
                        glCallList(CasaDL);
                    glPopMatrix();
                    }
                }
    glPopMatrix();
}

void escribeVidas(){
    char vidas[50]; // Ampliado a 50 bytes
    sprintf(vidas, "VIDAS:");
    glPushMatrix();
                    glColor3ub(0, 255, 0);
                    glRasterPos2i((ancho/2)-40, alto-50);
                    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)vidas);
    glPopMatrix();
}



void dibujaVidas(){
    glPushMatrix();
            glTranslatef(xVidas, yVidas, 0);
            for(int i=0; i<3; i++){
                if(VidasST[i].VidaVisible){
                    glPushMatrix();
                        glTranslatef(VidasST[i].x1Vidas, VidasST[i].y1Vidas, 0);
                        glCallList(VidasDL);
                    glPopMatrix();
                }
            }
    glPopMatrix();
}

void dibujaOvni(){
    if(Ovni.esta){
    glPushMatrix();
        glTranslatef(xOvni,yOvni,0);
            glPushMatrix();
                glTranslatef(Ovni.x1Ovni,Ovni.y1Ovni,0);
                glCallList(OvniDL);
            glPopMatrix();
    glPopMatrix();
    }
}

void dibujaMisil(){
    if(disp){
    glPushMatrix();
        glTranslatef(xMisil,yMisil,0);
        glCallList(MisilDL); //DIBUJA EL MISIL
    glPopMatrix();
    }
}

void dibujaCanion(){
    glPushMatrix();
        glTranslatef(xCanion,0,0);
        glCallList(CanionDL);//DIBUJA EL CA�ON
    glPopMatrix();
}

void display ( void ){ /*funcion que dibuja*//*..........................................DISPLAY............................*/

    // cout << "llega";

    keyoperations();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    dibujaCanion();

    dibujaMisil();

    dibujaMarcianos();

    dibujaMarcador();

    dibujaPuntuacionMaxima();

    dibujaCasas();

    dibujaOvni();

    escribeVidas();
    dibujaVidas();

    glutSwapBuffers();

    Sleep(time_left());  //funcion de windows

    next_time += TICK_INTERVAL;

}

/* Funci�n principal */
int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#if SDL_AUDIO_AVAILABLE
    SDL_Init(SDL_INIT_AUDIO);
#endif

    // glutInitWindowSize(500, 300);//cambiar pos y tama�o ventana
    // glutInitWindowPosition(200, 200);

    glutCreateWindow("Space invaders");
    glutFullScreen();

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);//funciones para las flechas

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);//funciones para las teclas normales

    glutSetCursor(GLUT_CURSOR_NONE);
    glutIdleFunc(idle);

    glutMainLoop();

    return 0;
}
