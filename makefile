CFLAGS       =
FFLAGS		 =
CPPFLAGS     =
FPPFLAGS     =
LOCDIR		 =
MANSEC       =
EXAMPLESC	 = Prueba.c Prueba2.c \
				Prueba3.c Prueba4.c \
				
EXAMPLESF	 = 
EXAMPLESCH	 =
EXAMPLESFH   =
DIRS		 =

include ${PETSC_DIR}/lib/petsc/conf/variables
include ${PETSC_DIR}/lib/petsc/conf/rules

#-------------------------------------------------------------------------



include ${PETSC_DIR}/lib/petsc/conf/test
