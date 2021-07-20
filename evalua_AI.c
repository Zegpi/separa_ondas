#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

//Este código evalua una red neuronal contra todos los archivos de input en la carpeta ./archivos_entrada
int main(void) {

//Variables
	int N,*n,neuronas_capa_inicial,neuronas_capa_anterior,cont,num_ondas_entrada;
	float ***w,**val_capas,aptitud_total,aptitud_actual,val_temp,dt,t,inicio,duracion,amplitud,frecuencia,phi,*onda_salida,P1,P2;
	size_t num_leidos;
	DIR *carpeta;
	struct dirent *lista_archivo;
	char **archivos,raiz[512],ruta[512];
	FILE *fid_red,*fid_onda;

	//Valores fijos
	dt=1.0/100.0;										//Discretización del tiempo
//

//Leemos los pesos que determinan a la red neuronal
	//Abrimos el archivo que contiene la red neuronal
	fid_red=fopen("./redes/red1.dat","r");

	//Leemos el número de capas y el número de neuronas por capa
	num_leidos=fread(&N,sizeof(int),1,fid_red);
	//printf("El número de capas es %d \n",N);

	n=calloc(N,sizeof(int));
	for(int i=0;i<N;i++)
	{
		num_leidos=fread(&n[i],sizeof(int),1,fid_red);
		//printf("La capa %d tiene %d neuronas \n",i,n[i]);
	}
	
	//Creamos un arreglo para guardar los pesos
	neuronas_capa_inicial=20001;			//Num inputs, siempre debe ser 20001 a menos que se esté haciendo debug
	
	neuronas_capa_anterior=neuronas_capa_inicial;
	w=calloc(N,sizeof(float **));
	for(int i=0;i<N;i++)
	{
		w[i]=calloc(n[i],sizeof(float *));

		for(int j=0;j<n[i];j++)
		{
			w[i][j]=calloc(neuronas_capa_anterior+1,sizeof(float));
		}
		neuronas_capa_anterior=n[i];
	}

	//Ahora leemos el archivo y lo ponemos en w
	neuronas_capa_anterior=neuronas_capa_inicial;
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<n[i];j++)
		{
			for(int k=0;k<neuronas_capa_anterior+1;k++)
			{
				num_leidos=fread(&w[i][j][k],sizeof(float),1,fid_red);
			}
		}
		neuronas_capa_anterior=n[i];
	}
	fclose(fid_red);				//Estamos listos, la red neuronal está en w[i][j][k]
//

//Leemos los archivo con las ondas de entrada
	//Revisamos la carpeta con los inputs
	carpeta = opendir("./archivos_entrada");
	if (carpeta == NULL)  //Es null sólo si falla (la carpeta no existe, está bloqueada, etc.)
	{
		printf("No se pudo abrir la carpeta seleccionada");
		return 0;
	}

	//Enumeramos los archivos en la carpeta
	cont=0;
	while (readdir(carpeta) != NULL)	//readdir es como fread, lee linea por linea hasta el final
	{
		cont++;
	}
	closedir(carpeta);
	num_ondas_entrada=cont-2;			//Eliminamos 2 porque readdir pesca . y ..

	archivos=calloc(num_ondas_entrada,sizeof(char*));
	for(int i=0;i<num_ondas_entrada;i++)
	{
		archivos[i]=calloc(128,sizeof(char));				//Muy dificil que la ruta de un archivo tenga más de 128 caracteres, pero tener ojo igual
	}

	//Abrimos de nuevo, para empezar desde el principio
	carpeta = opendir("./archivos_entrada");
	cont=0;
	while ((lista_archivo = readdir(carpeta)) != NULL)
	{
		if(cont>1)	//Esto se salta . y ..
		{
			sprintf(archivos[cont-2],"%s",lista_archivo->d_name);		//Si no hago sprintf aqui, archivo queda con punteros a los archivos, que despues son eliminados por closedir
			//archivos[cont-2]=lista_archivo->d_name;
		}
		cont++;
	}
	closedir(carpeta);		//Ahora "archivos" tiene el nombre de todos los archivos con ondas
//

//Evaluamos la AI para cada onda y acumulamos la aptitud
	aptitud_total=0.0;
	sprintf(raiz,"%s","./archivos_entrada/");
	
	for(int l=0;l<num_ondas_entrada;l++)
	{
		//printf("archivos[%d]= %s \n",l,archivos[l]);
		sprintf(ruta,"%s%s",raiz,archivos[l]);

		printf("La ruta es %s\n",ruta);

		//Creamos un arreglo con los valores de cada capa, incluyendo el input
		val_capas=calloc(N+1,sizeof(float*));

		val_capas[0]=calloc(neuronas_capa_inicial,sizeof(float));
		for(int i=0;i<N;i++)
		{
			val_capas[i+1]=calloc(n[i],sizeof(float));
		}

		fid_onda=fopen(ruta,"r");
		for(int i=0;i<neuronas_capa_inicial;i++)
		{
			num_leidos=fread(&val_capas[0][i],sizeof(float),1,fid_onda);
		}
		fclose(fid_onda);

		//Las siguientes capas se evaluan con la red neuronal (en revisión!!!!)
		neuronas_capa_anterior=neuronas_capa_inicial;
		for(int i=0;i<N;i++)
		{
			for(int j=0;j<n[i];j++)
			{
				val_temp=0.0;
				for(int k=0;k<neuronas_capa_anterior;k++)
				{
					val_temp=val_temp+w[i][j][k]*val_capas[i][k];
				}
				val_temp=val_temp+w[i][j][neuronas_capa_anterior];						//Esto añade el sesgo al valor de la neurona
				val_capas[i+1][j]=fmax(0.01*val_temp,val_temp);							//Funcion "Leaky ReLU", parece ser sencilla y funcionar bien, según la internet
			}
			neuronas_capa_anterior=n[i];
		}
		
		//La red neuronal ya está evaluada, ahora generamos las ondas de salida
		onda_salida=calloc(neuronas_capa_inicial,sizeof(float));

		for(int i=0;i<n[N-1];i=i+4)
		{
			inicio=val_capas[N-1][i];
			duracion=val_capas[N-1][i+1];
			amplitud=val_capas[N-1][i+2];
			frecuencia=val_capas[N-1][i+3];
			phi=val_capas[N-1][i+4];

			//printf("Inicio= %f, duracion= %f, amplitud= %f, frecuencia= %f \n",inicio,duracion,amplitud,frecuencia);

			for(int j=0;j<neuronas_capa_inicial;j++)
			{
				t=dt*j;
				if(t>=inicio && t<=inicio+0.95*duracion)
				{
					onda_salida[j]=onda_salida[j]+amplitud*sin(2.0*M_PI*frecuencia*(t-inicio)+phi);
				}
				else if(t>inicio+0.95*duracion && t<=inicio+duracion)
				{
					onda_salida[j]=onda_salida[j]+amplitud*sin(2.0*M_PI*frecuencia*(t-inicio)+phi)*(1.0-(t-inicio-0.95*duracion)/(0.05*duracion));
				}
			}
		}

		printf("La aptitud total es %f \n",aptitud_total);

		for(int i=0;i<neuronas_capa_inicial;i++)
		{
			aptitud_actual=0.0;
			P1=1000.0;
			P2=0.1;
			for(int i=0;i<neuronas_capa_inicial;i++)
			{
				aptitud_actual=aptitud_actual+P1*dt*dt*(onda_salida[i]-val_capas[0][i])*(onda_salida[i]-val_capas[0][i]);
			}
			
			for(int i=0;i<N;i++)
			{
				aptitud_actual=aptitud_actual+P2*n[i];
			}
			aptitud_total=aptitud_total+aptitud_actual;
		}
	}

	printf("La aptitud total es %f \n",aptitud_total);

//

//Liberamos memoria (esto pasa sólo en el return, pero se supone que es buena práctica)
	free(archivos);
	free(n);

	for(int i=0;i<N;i++)
	{
		for(int j=0;j<n[i];j++)
		{
			free(w[i][j]);
		}
		free(w[i]);
	}
	free(w);

	for(int i=0;i<N+1;i++)
	{
		free(val_capas[i+1]);
	}
	free(val_capas);

	free(onda_salida);
//

	return 0;
}