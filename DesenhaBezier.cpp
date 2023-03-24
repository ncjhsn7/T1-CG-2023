#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"
#include "Bezier.h"

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

Temporizador T;
double AccumDeltaT=0;
Temporizador T2;

int contadorPontos = 0;
Ponto pontosClicados[3];
Bezier Curvas[20];
unsigned int nCurvas;

Poligono PoligonoDeControle;

// Limites l—gicos da ‡rea de desenho
Ponto Min, Max;

bool desenha = false;

Poligono Mapa, MeiaSeta, Mastro;

float angulo=0.0;

bool FoiClicado = false;
Ponto PontoClicado;
Ponto PontoAtual;
double nFrames=0;
double TempoTotal=0;

void animate() {
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;
    
    if (AccumDeltaT > 1.0 / 30) // fixa a atualiza‹o da tela em 30
    {
        AccumDeltaT = 0;
        angulo += 2;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0) {
        cout << "Tempo Acumulado: " << TempoTotal << " segundos. ";
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames / TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}

void reshape(int w, int h) {
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x, Max.x, Min.y, Max.y, -10, +10);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void DesenhaEixos() {
    Ponto Meio;
    Meio.x = (Max.x + Min.x) / 2;
    Meio.y = (Max.y + Min.y) / 2;
    Meio.z = (Max.z + Min.z) / 2;
    
    glBegin(GL_LINES);
    //  eixo horizontal
    glVertex2f(Min.x, Meio.y);
    glVertex2f(Max.x, Meio.y);
    //  eixo vertical
    glVertex2f(Meio.x, Min.y);
    glVertex2f(Meio.x, Max.y);
    glEnd();
}

void CarregaModelos() {
    Mapa.LePoligono("EstadoRS.txt");
    MeiaSeta.LePoligono("MeiaSeta.txt");
    Mastro.LePoligono("Mastro.txt");
}

void DesenhaPontos() {
    if (contadorPontos == 0)
        return;
    
    defineCor(Red);
    glPointSize(4);
    glBegin(GL_POINTS);
    
    for (int i = 0; i < contadorPontos; i++) {
        glVertex2f(pontosClicados[i].x, pontosClicados[i].y);
    }
    
    glEnd();
    glPointSize(1);
}

void CriaCurvas() {
    Curvas[nCurvas] = Bezier(pontosClicados[0], pontosClicados[1], pontosClicados[2]);
    nCurvas++;
}

void init() {
    glClearColor(.1f, .0f, 0.255f, 1.0f);
    
    //CarregaModelos();
    //CriaCurvas();
    
    float d = 15;
    Min = Ponto(-d, -d);
    Max = Ponto(d, d);
}

void DesenhaCurvas() {
    for (int i = 0; i < nCurvas; i++) {
        defineCor(Light_Purple);
        Curvas[i].Traca();
        defineCor(Scarlet);
        Curvas[i].TracaPoligonoDeControle();
    }
}

void DesenhaMenu() {
    glPushMatrix();
    glTranslated(11, 13, 0); // veja o arquivo MeiaSeta.txt
    MeiaSeta.desenhaPoligono();
    glPopMatrix();
}

void display(void) {
    
    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Define os limites lógicos da área OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // Coloque aqui as chamadas das rotinas que desenham os objetos
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    
    glLineWidth(1);
    glColor3f(1, 1, 1); // R, G, B  [0..1]
    
    DesenhaMenu();
    //DesenhaEixos();

    glLineWidth(3);
    glColor3f(1, 0, 0);
    DesenhaCurvas();
    DesenhaPontos();
    
//    if (FoiClicado) {
//        PontoClicado.imprime("PontoClicado", "\n");
//        FoiClicado = false;
//        pontosClicados[contadorPontos] = PontoClicado;
//        contadorPontos++;
//    }
    glutSwapBuffers();
}

void ContaTempo(double tempo) {
    Temporizador T;
    
    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while (true) {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0) {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Termina o programa qdo
            exit(0); // a tecla ESC for pressionada
            break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
            desenha = !desenha;
            break;
        default:
            break;
    }
}

void arrow_keys(int a_keys, int x, int y) {
    switch (a_keys) {
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
        case GLUT_KEY_UP: // Se pressionar UP
            glutFullScreen(); // Vai para Full Screen
            break;
        case GLUT_KEY_DOWN: // Se pressionar UP
            // Reposiciona a janela
            glutPositionWindow(50, 50);
            glutReshapeWindow(700, 500);
            break;
        default:
            break;
    }
}

Ponto ConvertePonto(Ponto P) {
    GLint viewport[4];
    GLdouble modelview[16],projection[16];
    GLfloat wx=P.x,wy,wz;
    GLdouble ox=0.0,oy=0.0,oz=0.0;
    
    glGetIntegerv(GL_VIEWPORT,viewport);
    
    P.y=viewport[3]-P.y;
    wy=P.y;
    
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glReadPixels(P.x,P.y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
    gluUnProject(wx,wy,wz,modelview,projection,viewport,&ox,&oy,&oz);
    
    return Ponto(ox,oy,oz);
}

void Motion(int x, int y){
    Ponto P(x,y);

    PontoAtual = ConvertePonto(P);
    PontoAtual.imprime("Mouse:");
    
    cout << endl;
}

void DesenhaLinha(Ponto P1, Ponto P2) {
    glBegin(GL_LINES);
    glVertex2f(P1.x,P1.y);
    glVertex2f(P2.x,P2.y);
    glEnd();
}

void Mouse(int button, int state, int x, int y) {
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat wx = x, wy, wz;
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;
    
    //if (state != GLUT_DOWN)
    //    return;
    
    if (button != GLUT_LEFT_BUTTON)
        return;

    glGetIntegerv(GL_VIEWPORT, viewport);
    y = viewport[3] - y;
    wy = y;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, & wz);
    gluUnProject(wx, wy, wz, modelview, projection, viewport, & ox, & oy, & oz);
    
    cout << '\n' + contadorPontos;
    
    if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
        cout << "Botao ESQ Desceu" << endl;
    }
    
    if(state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
        cout << "Botao ESQ Subiu" << endl;
    }
    
    PontoClicado = Ponto(ox, oy, oz);
    pontosClicados[contadorPontos] = PontoClicado;
    contadorPontos++;
    
    if (contadorPontos == 3) {
        CriaCurvas();
        contadorPontos = 0;
    }
    
}

int main(int argc, char ** argv) {
    cout << "Programa OpenGL" << endl;
    
    glutInit( & argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutMotionFunc(Motion);
    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(650, 500);
    
    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de título da janela.
    glutCreateWindow("Animacao com Bezier");
    
    // executa algumas inicializações
    init();
    
    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // será chamada automaticamente quando
    // for necessário redesenhar a janela
    glutDisplayFunc(display);
    
    // Define que o tratador de evento para
    // o invalida‹o da tela. A funcao "display"
    // será chamada automaticamente sempre que a
    // m‡quina estiver ociosa (idle)
    glutIdleFunc(animate);
    
    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // será chamada automaticamente quando
    // o usuário alterar o tamanho da janela
    glutReshapeFunc(reshape);
    
    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // será chamada automaticamente sempre
    // o usuário pressionar uma tecla comum
    glutKeyboardFunc(keyboard);
    
    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" será chamada
    // automaticamente sempre o usuário
    // pressionar uma tecla especial
    glutSpecialFunc(arrow_keys);
    glutMouseFunc(Mouse);
    
    glutMainLoop();
    
    return 0;
}
