#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#include "coordenada.h"
using namespace std;


float sx,sy,incx,incy,tx,ty,inctx,ang, incang,lim_i=0,lim_s=10,intervalo;

//variables para mpi
int rango, p, source, dest, tag = 0,conta=0;
MPI_Status status;
float res[102],dx,m;
bool a=true;
vector <coordenada> puntos;
void repartir(float xi,float xf, float dx)
{
    m=(xf - xi) / (100 * dx);

    float dat[3];
    if(floor(m)==m)
    {

        for(int i=0,j=1;i<m-((int)m%(p-1));i++,j++)
        {
            dat[0]=xi;
            dat[1]=xi+100*dx;
            dat[2]=dx;
            //cout<<"envio a "<<j<<" "<<dat[0]<<" "<<dat[1]<<" "<<dat[2]<<endl;
            MPI_Send(dat,3,MPI_FLOAT, j,0,MPI_COMM_WORLD);
            xi+=100*dx;

            if(j==p-1)
                j=0;
        }


        if(0 != (int)m % (p -1)  )

        {

            for(int i=0;i<(int)m % (p -1) ;i++)
           {
               dat[0]=xi;
               dat[1]=xi+100*dx;
               dat[2]=dx;
               //cout<<"envio a "<<i+1<<" "<<dat[0]<<" "<<dat[1]<<" "<<dat[2]<<endl;
               MPI_Send(dat,3,MPI_FLOAT, i+1,0,MPI_COMM_WORLD);
               xi+=100*dx;

           }
        }

    }
    else
    {
        cout<<"100 dx no es divisible entre rango"<<endl;
    }

}
void calcularF(float xi,float xf,float dx )
{
    float y,r[103];
    int j=2;
    r[0]=xi;
    r[1]=xf;
    //cout<<"respusta r1 "<<r[0]<<" rango "<<rango<<endl;
    for(float i=xi;i<=xf;i=dx+i,j++)
    {
        y=5*sin(i/2)+sin(i)+sin(2*i);
        r[j]=y;
        //printf("aaa %d     %d\n",j,rango);
    }

    MPI_Send(r,102,MPI_FLOAT,0,1,MPI_COMM_WORLD);
}
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0,0,0);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 10);
    glMatrixMode(GL_MODELVIEW);
}

void ejes()
{
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex3f(-100,0,0);
    glVertex3f(100,0,0);
    glVertex3f(0,-100,0);
    glVertex3f(0,100,0);
    glEnd();
}
void pintaGrafica()
{
    glBegin(GL_POINTS);
    for(int i=0;i<puntos.size();i++)
    {
        glVertex3f(puntos.at(i).getX(),puntos.at(i).getY(),puntos.at(i).getZ());
        //printf("puntos arreglo %f,%f,%f",puntos.at(i).getX(),puntos.at(i).getY(),puntos.at(i).getZ());
    }
    glEnd();
}
void preProcesado()
{
    float j=0;

    if(a)
    {
        if(conta<m)
        {
            MPI_Recv(res,102,MPI_FLOAT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&status);
            //cout<<"respusta "<<res[0]<<" rango "<<rango<<endl;
            for(int i=2;i<102;i++,j=dx+j)
            {
                puntos.push_back(coordenada(res[0]+j,res[i],0));
            }
            //cout<<conta<<"contador"<<endl;
            conta++;

        }
        else
        {
            float final[3]={0,0,0};
            //MPI_Barrier(MPI_COMM_WORLD);
            for(int i=1;i<p;i++)
            {
                MPI_Send(final,3,MPI_FLOAT,i,0,MPI_COMM_WORLD);
                //cout<<"mande final"<<endl;
            }
            a=false;
        }

    }


}
void display()
{   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0,0,0);
    glLoadIdentity();
    ejes();


    preProcesado();
    pintaGrafica();
    glFlush();
};
void init()
{    glClearColor(1,1,1,0);
}

void idle()
{   display();

}

//prueba
int main(int argc, char*argv[])
{
    char message[100];

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rango);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    //cout<<rango<<p<<endl;


    if (rango != 0)
    {
       float dat_l[3];
        bool a=true;
        int as=0;
        while(a)
        {
            MPI_Recv(dat_l,3,MPI_FLOAT,0,0,MPI_COMM_WORLD,&status);
            //cout<<"los que llegan"<<dat_l[0]<<"  "<<dat_l[1]<<"  "<<dat_l[2]<<"  "<<rango<<endl;

            if(dat_l[0] == 0 && dat_l[1] == 0 && dat_l[2] == 0 )
            {
                a=false;
                //cout<<"falllso"<<endl;
                //cout<<"no  que llegan"<<dat_l[0]<<"  "<<dat_l[1]<<"  "<<dat_l[2]<<"  "<<rango<<endl;

            }

            else
            {

                calcularF(dat_l[0],dat_l[1],dat_l[2]);
            }


        }




    }
    else
    {
        float xinferior,xsuperior;
        cout<<"Escribe el rango inferior"<<endl;
        cin>>xinferior;
        cout<<"Escribe el rango superior"<<endl;
        cin>>xsuperior;
        cout<<"Escribe el dx"<<endl;
        cin>>dx;

        repartir(xinferior,xsuperior,dx);
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
        glutInitWindowPosition(100, 100);
        glutInitWindowSize(500, 500);
        glutCreateWindow("Graficador Distribuido :)");
        init();
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);

        // Manejadores de funcion adicionales para animacion y control de teclado
        glutIdleFunc(idle);
        glutMainLoop();



    }
    MPI_Finalize();

}
