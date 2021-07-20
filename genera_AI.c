#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

//Este código genera una AI "vacía" (es decir, con todos sus pesos y sesgos en 0) pero con los numeros correctos para N (número de capas) y n[N] (número de neuronas en cada capa)

int main(void) {

	//Variables para el número de capas y la cantidad de neuronas por capas
	int N, *n, n_neuronas_capa_anterior;
	float *zero;
	size_t num_escrito;
	char nombre[128],raiz[128],archivo[256];

	N=3;

	n = (int*)calloc(N, sizeof(int));
	n[0]=7;
	n[1]=6;
	n[2]=5;
	//n[3]=240;		

	sprintf(raiz,"%s","./redes/");
	sprintf(nombre,"%s","red1.dat");
	sprintf(archivo,"%s%s",raiz,nombre);

	FILE *fid;
	fid=fopen(archivo,"w");

	num_escrito=fwrite(&N,sizeof(int),1,fid);

	for(int i=0;i<N;i++)
	{
		num_escrito=fwrite(&n[i],sizeof(int),1,fid);	
	}

	//Ya tenemos guardado el número de capas y el número de neuronas por capa, ahora empezamos a escribir los pesos y sesgos para cada neurona.
	//No olvidar que la primera capa tiene siempre 20001 valores.

	n_neuronas_capa_anterior=20001;

	srand(time(NULL));
	
	for(int i=0;i<N;i++)
	{
		zero=(float*)calloc(n_neuronas_capa_anterior+1,sizeof(float));

		for(int j=0;j<n[i];j++)
		{
			for(int k=0;k<n_neuronas_capa_anterior;k++)
			{
				if (i<N-1)
				{
				zero[k]=(float)rand()/RAND_MAX*2.0-1.0;		//No hay problema con que estos valores sean cero, por ahora estan así para hacer debug, en general es mejor crearlas aleatorias
				//printf("En la capa %d el numero zero[%d] es %f\n",i+1,k+1,zero[k]);
				}
				else
				{
					zero[k]=(float)k;
				}
			}
			zero[n_neuronas_capa_anterior]=10.0;			//Fija los sesgos en 10, para debug en otras funciones
			
			for(int k=0;k<n_neuronas_capa_anterior+1;k++)
			{
				num_escrito=fwrite(&zero[k],sizeof(float),1,fid);
			}
		}
		n_neuronas_capa_anterior=n[i];
	}

	fclose(fid);

	printf("Red Neuronal Generada y guardada como %s \n",nombre);

//Liberamos memoria
	free(n);
	free(zero);
//

	return 0;
}


//Para calcular el tamaño de un archivo de red, se puede usar este código en matlab
/*
clear variables
close all
clc

%La primera no cambia
num_neuronas_capa=[25001 12500 5000 300];

peso_float=4;               %bytes
peso_int=4;                 %bytes

peso=3*peso_int;        %Consideramos que N, n[1] y n[2] estan guardados, añadir más si N>2
for i=2:3
    for j=1:num_neuronas_capa(i)
        peso=peso+peso_float*(num_neuronas_capa(i-1)+1);
    end
end

peso=peso/1024/1024;				%Peso en MB

disp(['El archivo pesa ',num2str(peso),' Mb'])
*/