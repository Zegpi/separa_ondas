#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

//Este código genera una onda sinusoidal que comienza en un tiempo dado, y tiene una duración acotada. 

int main(void) {

	int num_muestras;
	float params[4],*onda;
	double t;
	size_t num_escrito;
	FILE *fid;

	//Parametros de la señal a generar
	params[0]=150.0;				//Tiempo en que la señal inicia
	params[1]=10.0;					//Duración de la señal
	params[2]=1.25;					//Amplitud de la señal
	params[3]=2.5;					//Frecuencia de la señal

	//Generamos la señal
	fid=fopen("./archivos_entrada/onda6.dat","w");

	num_muestras=20001;
	onda=calloc(num_muestras,sizeof(float));

	for(int i=0;i<num_muestras;i++)
	{
		t=(double)i/100.0;
		if(t>=params[0] && t<=(params[0]+params[1]))
		{
			onda[i]=params[2]*sin(2.0*M_PI*params[3]*(t-params[0]));
		}
		else
		{
			onda[i]=0.0;
		}
		num_escrito=fwrite(&onda[i],sizeof(float),1,fid);
	}

	fclose(fid);

//Liberamos memoria
	free(onda);
//
	return 0;
}