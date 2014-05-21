if [ -z $1 ]; then
    echo "Needs at least 1 parameter"
    exit 1;
fi;

ETAPA=$1;

# Cria pasta tmp
if [ -d "tmp" ]; then
    rm -rf tmp;
fi
mkdir tmp;

# Copia os arquivos para a pasta tmp e entra na pasta tmp
for file in "ast.c ast.h hash.c hash.h main.c Makefile scanner.l syntax.y symbols.h util.c util.h semantic.c semantic.h"
do
    cp $file tmp/;
    cp $file tmp/;
    cp $file tmp/;
done;
cd tmp;

# Compacta os arquivos da pasta tmp e move ele para a raiz
tar cvzf .temporary_file.tgz .;
mv .temporary_file.tgz ../$ETAPA.tgz ;

# Volta para a raiz e move o arquivo para a pasta etapas
cd ..;
if [ ! -d "etapas" ]; then
    mkdir etapas;
fi
if [ -f "etapas/$ETAPA.tgz" ]; then
    rm -f etapas/$ETAPA.tgz;
fi
mv $ETAPA.tgz etapas;

# Remove a pasta tmp
rm -rf tmp;

# Tenta sincronizar o arquivo com o servidor
if [ ! -z "$2" ]; then
    rsync etapas/$ETAPA.tgz $2@html.inf.ufrgs.br:/home/grad/$2/public_html/$ETAPA.tgz;
fi


